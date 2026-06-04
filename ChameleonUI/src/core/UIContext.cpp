#include "../../include/core/UIContext.h"
#include "../../include/elements/slider.h"
#include "../../include/elements/combobox.h"
#include "../../include/skins/skin.h"
#include "../../include/elements/colorpickermodal.h"
#include "../../include/elements/titlebar.h"

#pragma region lifecycle
UIContext::UIContext()
{
    UIElement::SetLayoutContext(this);
}
UIContext::~UIContext()
{
    UIElement::SetLayoutContext(nullptr);
}

void UIContext::Initialize(HWND hwnd)
{
    m_hwnd = hwnd;
}

void UIContext::Shutdown()
{
    m_windows.clear();
    m_hovered = nullptr;
    m_active = nullptr;
    m_activeSlider = nullptr;
    m_activeWindow = nullptr;
    m_inputCapture = nullptr;
    m_focused = nullptr;

    m_tooltipTarget = nullptr;
    m_tooltipVisible = false;
    m_tooltipTimer = 0.f;
}
#pragma endregion

#pragma region window management
Window& UIContext::CreateUIWindow(const std::wstring& title, int x, int y, int w, int h)
{
    auto window = std::make_unique<Window>(title, x, y, w, h);
    Window& ref = *window;
    m_windows.push_back(std::move(window));
    return ref;
}
#pragma endregion

#pragma region mouse state
int g_lastMouseX = 0, g_lastMouseY = 0;
void UIContext::SetMousePosition(int x, int y)
{
    m_mouseX = x; m_mouseY = y;
    g_lastMouseX = x; g_lastMouseY = y;
}

void UIContext::SetMouseButtonDown(int button)
{
    if (button >= 0 && button < 3)
    {
        m_mouseButtons[button] = true;
        m_mouseDownThisFrame[button] = true;
    }
}

void UIContext::SetMouseButtonUp(int button)
{
    if (button >= 0 && button < 3)
    {
        m_mouseButtons[button] = false;
        m_mouseUpThisFrame[button] = true;
    }
}

HWND UIContext::GetHwnd() const { return m_hwnd; }
int UIContext::GetMouseX() const { return m_mouseX; }
int UIContext::GetMouseY() const { return m_mouseY; }
bool UIContext::IsMouseButtonDown(int button) const
{
    if (button >= 0 && button < 3)
        return m_mouseButtons[button];
    return false;
}
#pragma endregion

#pragma region z-ordering
void UIContext::BringWindowToFront(Window* window)
{
    if (!window) return;
    for (auto it = m_windows.begin(); it != m_windows.end(); ++it)
    {
        if (it->get() == window)
        {
            if (it == m_windows.end() - 1) return;

            auto owned = std::move(*it);
            m_windows.erase(it);
            m_windows.push_back(std::move(owned));
            return;
        }
    }
}

Window* UIContext::GetFrontWindow() const
{
    if (m_windows.empty()) return nullptr;
    return m_windows.back().get();
}
#pragma endregion

#pragma region hit testing
UIElement* UIContext::HitTestWindows()
{
    m_activeWindow = nullptr;

    for (auto it = m_persistentOverlays.rbegin(); it != m_persistentOverlays.rend(); ++it)
    {
        UIElement* overlay = *it;
        if (!overlay || !overlay->IsVisible()) continue;
        UIElement* hit = overlay->HitTest(m_mouseX, m_mouseY);
        if (hit) return hit;
    }

    for (auto it = m_windows.rbegin(); it != m_windows.rend(); ++it)
    {
        Window* win = it->get();
        if (!win->IsVisible()) continue;
        Rect absRect = win->GetAbsoluteRect();
        if (m_mouseX >= absRect.x && m_mouseX < absRect.x + absRect.w &&
            m_mouseY >= absRect.y && m_mouseY < absRect.y + absRect.h)
        {
            m_activeWindow = win;
            break;
        }
    }
    if (!m_activeWindow)
        return nullptr;
    return m_activeWindow->HitTest(m_mouseX, m_mouseY);
}

void UIContext::ResolveHover()
{
    UIElement* target = m_inputCapture ? m_inputCapture : HitTestWindows();

    if (target != m_hovered)
    {
        if (m_hovered)
            m_hovered->OnMouseLeave();

        m_hovered = target;

        if (m_hovered)
            m_hovered->OnMouseEnter();

        m_tooltipTimer = 0.f;
        m_tooltipVisible = false;
        m_tooltipTarget = FindTooltipElement(m_hovered);
        m_tooltipMouseX = m_mouseX;
        m_tooltipMouseY = m_mouseY;
    }
}

UIElement* UIContext::FindTooltipElement(UIElement* element)
{
    UIElement* el = element;
    while (el)
    {
        if (el->HasTooltip())
            return el;
        el = el->GetParent();
    }
    return nullptr;
}
#pragma endregion

#pragma region input
void UIContext::SetMouseWheel(int delta)
{
    m_mouseWheelDelta = delta;
}

void UIContext::SetInputCapture(UIElement* element)
{
    m_inputCapture = element;
}

void UIContext::ReleaseInputCapture(UIElement* element)
{
    if (m_inputCapture == element)
        m_inputCapture = nullptr;
}

void UIContext::ProcessInput()
{
    ResolveHover();

    if (m_inputCapture)
    {
        UIElement* capture = m_inputCapture;

        capture->OnMouseMove(m_mouseX, m_mouseY);

        if (m_mouseDownThisFrame[0])
            capture->OnMouseDown(0);

        if (m_mouseUpThisFrame[0])
        {
            capture->OnMouseUp(0);
            if (m_inputCapture == capture)
                capture->OnClick();
        }

        if (m_mouseWheelDelta != 0)
        {
            UIElement* wheelTarget = m_inputCapture ? m_inputCapture : capture;
            wheelTarget->OnMouseWheel(m_mouseWheelDelta);
            m_mouseWheelDelta = 0;
        }

        ClearEdgeFlags();
        return;
    }

    if (m_draggingWindow)
    {
        m_draggingWindow->OnMouseMove(m_mouseX, m_mouseY);
    }
    else if (m_active)
    {
        m_active->OnMouseMove(m_mouseX, m_mouseY);
    }
    else if (m_hovered)
    {
        m_hovered->OnMouseMove(m_mouseX, m_mouseY);
    }

    if (m_mouseDownThisFrame[0])
    {
        if (m_colorPickerModal && m_colorPickerModal->IsOpen())
        {
            UIElement* hit = HitTestWindows();
            if (hit != m_colorPickerModal)
            {
                m_colorPickerModal->Close();
            }
        }

        if (m_activeWindow)
            BringWindowToFront(m_activeWindow);

        TitleBar* hitTitleBar = dynamic_cast<TitleBar*>(m_hovered);
        Window* hitWindow = dynamic_cast<Window*>(m_hovered);

        if (hitTitleBar && hitTitleBar->GetOwnerWindow())
        {
            Window* win = hitTitleBar->GetOwnerWindow();
            Rect absRect = win->GetAbsoluteRect();
            m_draggingWindow = win;
            m_dragOffsetX = m_mouseX - absRect.x;
            m_dragOffsetY = m_mouseY - absRect.y;
        }
        else if (hitWindow)
        {
            if (!hitWindow->ShowTitleBar())
            {
                Rect absRect = hitWindow->GetAbsoluteRect();
                m_draggingWindow = hitWindow;
                m_dragOffsetX = m_mouseX - absRect.x;
                m_dragOffsetY = m_mouseY - absRect.y;
            }
        }

        if (!m_draggingWindow && m_hovered)
        {
            if (m_focused && m_focused != m_hovered)
            {
                m_focused->OnFocusLost();
                m_focused = nullptr;
            }

            m_active = m_hovered;
            m_activeSlider = dynamic_cast<Slider*>(m_active);
            m_active->OnMouseDown(0);
        }
        else if (!m_draggingWindow && !m_hovered)
        {
            if (m_focused)
            {
                m_focused->OnFocusLost();
                m_focused = nullptr;
            }
        }
    }

    if (m_mouseUpThisFrame[0])
    {
        if (m_draggingWindow)
        {
            m_draggingWindow = nullptr;
        }
        else if (m_active)
        {
            m_active->OnMouseUp(0);
            if (m_active == m_hovered)
                m_active->OnClick();
            m_active = nullptr;
            m_activeSlider = nullptr;
        }
        if (m_hovered)
            m_hovered->OnMouseUp(0);
    }

    if (m_mouseWheelDelta != 0 && m_hovered)
    {
        m_hovered->OnMouseWheel(m_mouseWheelDelta);
        m_mouseWheelDelta = 0;
    }

    ClearEdgeFlags();
}

void UIContext::ClearEdgeFlags()
{
    for (int i = 0; i < 3; ++i)
    {
        m_mouseDownThisFrame[i] = false;
        m_mouseUpThisFrame[i] = false;
    }
}
#pragma endregion

#pragma region update
void UIContext::Update(float dt)
{
    if (m_draggingWindow)
    {
        int newX = m_mouseX - m_dragOffsetX;
        int newY = m_mouseY - m_dragOffsetY;
        if (newX != m_draggingWindow->GetX() || newY != m_draggingWindow->GetY())
        {
            m_draggingWindow->SetX(newX);
            m_draggingWindow->SetY(newY);
            m_layoutDirty = true;
        }
    }

    for (auto& window : m_windows)
        window->Update(dt);

    if (m_activeSlider && m_mouseButtons[0])
    {
        m_activeSlider->UpdateDrag((float)m_mouseX);
    }

    if (m_tooltipsEnabled && m_tooltipTarget && m_tooltipTarget->HasTooltip())
    {
        if (std::abs(m_mouseX - m_tooltipMouseX) > 4 || std::abs(m_mouseY - m_tooltipMouseY) > 4)
        {
            m_tooltipTimer = 0.f;
            m_tooltipVisible = false;
            m_tooltipMouseX = m_mouseX;
            m_tooltipMouseY = m_mouseY;
        }

        if (!m_tooltipVisible)
        {
            m_tooltipTimer += dt;
            if (m_tooltipTimer >= m_tooltipDelay)
                m_tooltipVisible = true;
        }
    }
    else
    {
        m_tooltipTimer = 0.f;
        m_tooltipVisible = false;
    }
}

void UIContext::PerformLayout()
{
    if (!m_layoutDirty) return;
    for (auto& win : m_windows)
    {
        Size available{ (float)win->GetWidth(), (float)win->GetHeight() };
        win->Measure(available);
        Rect finalRect{ (float)win->GetX(), (float)win->GetY(),
                        (float)win->GetWidth(), (float)win->GetHeight() };
        win->Arrange(finalRect);
    }
    m_layoutDirty = false;
}
#pragma endregion

#pragma region overlays
void UIContext::QueueOverlay(std::function<void(IRenderer*)> drawFn)
{
    m_overlayQueue.push_back(std::move(drawFn));
}
#pragma endregion

#pragma region focus
void UIContext::SetFocus(UIElement* element)
{
    if (m_focused == element) return;
    if (m_focused) m_focused->OnFocusLost();
    m_focused = element;
    if (m_focused) m_focused->OnFocusGained();
}

void UIContext::ClearFocus(UIElement* element)
{
    if (m_focused == element) {
        m_focused->OnFocusLost();
        m_focused = nullptr;
    }
}
#pragma endregion

#pragma region persistent overlays
void UIContext::AddPersistentOverlay(UIElement* element)
{
    for (auto* e : m_persistentOverlays)
        if (e == element) return;
    m_persistentOverlays.push_back(element);
}

void UIContext::RemovePersistentOverlay(UIElement* element)
{
    m_persistentOverlays.erase(
        std::remove(m_persistentOverlays.begin(), m_persistentOverlays.end(), element),
        m_persistentOverlays.end());
}
#pragma endregion

#pragma region keyboard

void UIContext::OnKeyDown(int vkCode)
{
    UIElement* target = m_inputCapture ? m_inputCapture : m_focused;
    if (target) target->OnKeyDown(vkCode);
}

void UIContext::OnKeyUp(int vkCode)
{
    UIElement* target = m_inputCapture ? m_inputCapture : m_focused;
    if (target) target->OnKeyUp(vkCode);
}

void UIContext::OnChar(wchar_t ch)
{
    UIElement* target = m_inputCapture ? m_inputCapture : m_focused;
    if (target) target->OnChar(ch);
}
#pragma endregion

#pragma region draw
void UIContext::Draw(IRenderer* renderer)
{
    for (auto& window : m_windows)
    {
        if (!window->IsVisible())
            continue;
        Rect windowClip = window->GetAbsoluteRect();
        renderer->SetScissor(windowClip);
        window->Draw(renderer);
        window->DrawChildren(renderer, windowClip);
        renderer->ResetScissor();
    }

    for (auto& fn : m_overlayQueue)
        fn(renderer);
    m_overlayQueue.clear();

    for (auto* element : m_persistentOverlays)
    {
        if (element && element->IsVisible())
            element->Draw(renderer);
    }

    if (m_tooltipsEnabled && m_tooltipVisible && m_tooltipTarget && m_tooltipTarget->HasTooltip())
    {
        Skin* skin = UIElement::GetDefaultSkin();
        if (skin)
            skin->DrawTooltip(renderer, m_tooltipTarget->GetTooltip(), m_tooltipMouseX, m_tooltipMouseY);
    }
}
#pragma endregion