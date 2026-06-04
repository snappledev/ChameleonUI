#include "../../include/elements/window.h"
#include "../../include/elements/titlebar.h"
#include "../../include/layouts/panel.h"
#include "../../include/skins/skin.h"
#include "../../include/core/UIContext.h"
#include "../../include/elements/TabControl.h"
#include "../../include/layouts/layout_helpers.h"
#include <algorithm>

#pragma region lifecycle
Window::Window(const std::wstring& title, int x, int y, int w, int h)
    : m_title(title)
{
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;
    SetAutoLayoutVertical(false);

    auto& tb = AddChild<TitleBar>();
    tb.SetOwnerWindow(this);
    tb.SetHeight(30);
    tb.SetPadding(8, 0, 8, 0);
    m_titleBar = &tb;

    auto& panel = AddChild<Panel>();
    panel.SetScrollMode(ScrollMode::Vertical);
    panel.SetAutoLayoutVertical(true);
    panel.SetAutoSpacing(8);
    panel.SetPadding(0, 0, 0, 0);
    panel.SetMargin(0, 0, 0, 0);
    m_contentPanel = &panel;
}

const std::wstring& Window::GetTitle() const { return m_title; }
void Window::SetTitle(const std::wstring& title) { m_title = title; }

int Window::GetTitleBarHeight() const
{
    if (!m_showTitleBar || !m_titleBar) return 0;
    float h = m_titleBar->GetDesiredSize().height;
    if (m_titleBar->GetArrangedHeight() > 0)
        h = (float)m_titleBar->GetArrangedHeight();
    return (int)h;
}

bool Window::IsDragging() const { return m_dragging; }

int Window::GetClientWidth()
{
    int w = (m_arrangedWidth > 0) ? m_arrangedWidth : m_width;
    return w - m_padding.x - m_padding.w;
}

int Window::GetClientHeight()
{
    int h = (m_arrangedHeight > 0) ? m_arrangedHeight : m_height;
    return h - GetTitleBarHeight() - m_padding.y - m_padding.h;
}

Rect Window::GetClientRect() const
{
    Rect abs = GetAbsoluteRect();
    int tbH = const_cast<Window*>(this)->GetTitleBarHeight();
    int clientX = abs.x + m_padding.x;
    int clientY = abs.y + tbH + m_padding.y;
    int clientW = abs.w - m_padding.x - m_padding.w;
    int clientH = abs.h - tbH - m_padding.y - m_padding.h;
    return Rect(clientX, clientY, clientW, clientH);
}
#pragma endregion

#pragma region update
void Window::Update(float dt) { UIElement::Update(dt); }
#pragma endregion

#pragma region drawing
void Window::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    Rect absRect = GetAbsoluteRect();

    int tbH = GetTitleBarHeight();
    Rect bodyRect(absRect.x, absRect.y + tbH, absRect.w, absRect.h - tbH);

    if (skin)
    {
        skin->DrawWindowBackground(renderer, *this, bodyRect);
    }
    else
    {
        renderer->DrawFilledRect(bodyRect, Color(50, 50, 60, 255));
    }
}

void Window::OnClick() {}

void Window::DrawChildren(IRenderer* renderer, const Rect& parentClip)
{
    Rect abs = GetAbsoluteRect();

    if (m_showTitleBar && m_titleBar && m_titleBar->IsVisible())
    {
        Rect tbClip = parentClip.Intersect(m_titleBar->GetAbsoluteRect());
        m_titleBar->Draw(renderer);
        m_titleBar->DrawChildren(renderer, tbClip);
    }

    if (m_contentPanel)
    {
        Rect client = GetClientRect();
        Rect clip = parentClip.Intersect(client);
        m_contentPanel->Draw(renderer);
        m_contentPanel->DrawChildren(renderer, clip);
    }
}
#pragma endregion

#pragma region input
UIElement* Window::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;
    Rect myRect = GetAbsoluteRect();
    if (absX < myRect.x || absX >= myRect.x + myRect.w ||
        absY < myRect.y || absY >= myRect.y + myRect.h)
        return nullptr;

    if (m_showTitleBar && m_titleBar && m_titleBar->IsVisible())
    {
        UIElement* hit = m_titleBar->HitTest(absX, absY);
        if (hit) return hit;
    }

    if (m_contentPanel)
    {
        UIElement* hit = m_contentPanel->HitTest(absX, absY);
        if (hit) return hit;
    }

    return this;
}
#pragma endregion

#pragma region measure & arrange
void Window::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    float winW = availableSize.width;

    float tbH = 0.f;
    if (m_showTitleBar && m_titleBar)
    {
        m_titleBar->Measure(Size{ winW - m_padding.x - m_padding.w, 1000.f });
        tbH = m_titleBar->GetDesiredSize().height;
    }

    float clientWidth = winW - (float)(m_padding.x + m_padding.w);
    float clientHeight = availableSize.height - tbH - (float)(m_padding.y + m_padding.h);
    if (clientWidth < 0) clientWidth = 0;
    if (clientHeight < 0) clientHeight = 0;

    if (m_contentPanel)
        m_contentPanel->Measure(Size{ clientWidth, clientHeight });

    float contentW = m_contentPanel ? m_contentPanel->GetDesiredSize().width : 100.f;
    float contentH = m_contentPanel ? m_contentPanel->GetDesiredSize().height : 0.f;

    float desiredWidth = (std::max)(contentW, 100.0f) + (float)(m_padding.x + m_padding.w);
    float desiredHeight = tbH + (float)(m_padding.y + m_padding.h) + contentH;

    m_desiredSize.width = desiredWidth;
    m_desiredSize.height = desiredHeight;
    m_needMeasure = false;
}

void Window::Arrange(const Rect& finalRect)
{
    bool skipChildren = CanSkipArrange(finalRect);
    UIElement::Arrange(finalRect);
    if (skipChildren) return;

    Rect abs = GetAbsoluteRect();

    if (m_showTitleBar && m_titleBar)
    {
        float tbH = m_titleBar->GetDesiredSize().height;
        Rect tbRect(abs.x, abs.y, abs.w, tbH);
        m_titleBar->Arrange(tbRect);
    }

    if (m_contentPanel)
    {
        Rect client = GetClientRect();
        m_contentPanel->Arrange(Rect(client.x, client.y, client.w, client.h));
    }
}
#pragma endregion