#include "../../include/layouts/panel.h"
#include "../../include/skins/skin.h"
#include "../../include/core/uistyle.h"
#include "../../include/layouts/layout_helpers.h"
#include "../../include/elements/tabcontrol.h"
#include <algorithm>
extern int g_lastMouseX, g_lastMouseY;

#pragma region lifecycle
Panel::Panel()
{
    SetAutoLayoutVertical(true);
    SetAutoSpacing(8);
}

void Panel::SetScrollMode(ScrollMode mode)
{
    m_scrollMode = mode;

    if (mode == ScrollMode::Vertical || mode == ScrollMode::Both)
    {
        if (!m_vScroll)
            m_vScroll = std::make_unique<Scrollbar>(ScrollDirection::Vertical);
    }
    else
    {
        m_vScroll.reset();
    }

    if (mode == ScrollMode::Horizontal || mode == ScrollMode::Both)
    {
        if (!m_hScroll)
            m_hScroll = std::make_unique<Scrollbar>(ScrollDirection::Horizontal);
    }
    else
    {
        m_hScroll.reset();
    }

    InvalidateMeasure();
}

void Panel::ApplyScrollOffset()
{
    if (m_children.empty()) return;
    UIElement* content = m_children[0].get();
    float yOff = m_vScroll ? -m_vScroll->GetScrollOffset() : 0.f;
    content->SetY((int)yOff);
}
#pragma endregion

#pragma region measure & arrange
void Panel::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    float clientW = availableSize.width - (float)(m_padding.x + m_padding.w);
    if (clientW < 0) clientW = 0;

    if (m_children.empty() || !m_children[0]->IsVisible())
    {
        if (m_vScroll) m_vScroll->SetContentSize(0);
        m_desiredSize.width = (m_width > 0) ? (float)m_width : availableSize.width;
        m_desiredSize.height = (m_height > 0) ? (float)m_height : 0.f;
        m_containsStretchH = false;
        m_needMeasure = false;
        return;
    }

    UIElement* content = m_children[0].get();

    float measureH = m_vScroll ? INFINITY : availableSize.height;
    content->Measure(Size{ clientW, measureH });
    float contentHeight = content->GetDesiredSize().height;
    float contentWidth = content->GetDesiredSize().width;

    if (m_vScroll)
        m_vScroll->SetContentSize(contentHeight);

    m_desiredSize.width = (m_width > 0)
        ? (float)m_width
        : contentWidth + (float)(m_padding.x + m_padding.w);

    m_desiredSize.height = (m_height > 0)
        ? (float)m_height
        : contentHeight + (float)(m_padding.y + m_padding.h);

    m_containsStretchH = content->WantsStretchH();

    m_needMeasure = false;
}

void Panel::Arrange(const Rect& finalRect)
{
    if (CanSkipArrange(finalRect)) return;
    UIElement::Arrange(finalRect);
    if (m_children.empty()) return;

    UIElement* content = m_children[0].get();
    if (!content->IsVisible()) return;

    Rect client = GetClientRect();

    bool needsVBar = false;
    float contentW = (float)client.w;

    if (m_vScroll)
    {
        needsVBar = m_vScroll->GetContentSize() > (float)client.h + 1.f;
        if (needsVBar)
        {
            float sbWidth = 12.f;
            Skin* skin = GetEffectiveSkin();
            if (skin && skin->GetStyle())
                sbWidth = (float)skin->GetStyle()->scrollbarWidth;
            m_vScroll->SetWidth(sbWidth);
            contentW -= sbWidth;
            if (contentW < 0) contentW = 0;

            content->ForceLocalRemeasure();
            content->Measure(Size{ contentW, INFINITY });
            m_vScroll->SetContentSize(content->GetDesiredSize().height);
        }
        m_vScroll->SetShown(needsVBar);
        m_vScroll->SetViewportSize((float)client.h);
        m_vScroll->Clamp();
    }

    float scrollY = (m_vScroll && needsVBar) ? m_vScroll->GetScrollOffset() : 0.f;
    float childHeight = (m_vScroll && needsVBar) ? m_vScroll->GetContentSize() : (float)client.h;

    Rect contentAbs((float)client.x, (float)client.y - scrollY,
        contentW, childHeight);
    content->Arrange(contentAbs);
}
#pragma endregion

#pragma region drawing
void Panel::Draw(IRenderer* renderer)
{
    if (m_vScroll && m_vScroll->IsShown())
        m_vScroll->Draw(renderer, GetEffectiveSkin(), GetClientRect());

    if (m_hScroll && m_hScroll->IsShown())
        m_hScroll->Draw(renderer, GetEffectiveSkin(), GetClientRect());
}

void Panel::DrawChildren(IRenderer* renderer, const Rect& parentClip) {
    DrawChildrenClipped(renderer, parentClip, GetClientRect());
}
#pragma endregion

#pragma region input
UIElement* Panel::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;
    Rect myRect = GetAbsoluteRect();
    if (!myRect.Contains(absX, absY)) return nullptr;

    if (m_vScroll && m_vScroll->IsShown() &&
        m_vScroll->GetLastThumbRect().Contains(absX, absY))
        return this;
    if (m_hScroll && m_hScroll->IsShown() &&
        m_hScroll->GetLastThumbRect().Contains(absX, absY))
        return this;

    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
    {
        UIElement* hit = (*it)->HitTest(absX, absY);
        if (hit) return hit;
    }
    return this;
}

void Panel::OnMouseWheel(int delta)
{
    if (m_vScroll && m_vScroll->IsShown())
    {
        if (m_vScroll->HandleWheel(delta))
        {
            ApplyScrollOffset();
            return;
        }
    }
    UIElement::OnMouseWheel(delta);
}

void Panel::OnMouseDown(int button)
{
    if (button != 0) return;
    if (m_vScroll && m_vScroll->IsShown())
        m_vScroll->HandleMouseDown(g_lastMouseX, g_lastMouseY);
    if (m_hScroll && m_hScroll->IsShown())
        m_hScroll->HandleMouseDown(g_lastMouseX, g_lastMouseY);
}

void Panel::OnMouseMove(int x, int y)
{
    Rect client = GetClientRect();

    auto handleScroll = [&](Scrollbar* sb) {
        if (!sb) return false;
        Rect track = sb->ComputeTrackRect(client);
        if (sb->IsDragging()) {
            sb->HandleMouseMove(x, y, track);
            ApplyScrollOffset();
            return true;
        }
        if (sb->IsShown())
            sb->HandleMouseMove(x, y, track);
        return false;
        };

    if (handleScroll(m_vScroll.get())) return;
    handleScroll(m_hScroll.get());
}

void Panel::OnMouseUp(int button)
{
    if (button != 0) return;
    if (m_vScroll) m_vScroll->HandleMouseUp();
    if (m_hScroll) m_hScroll->HandleMouseUp();
}

void Panel::OnMouseLeave()
{
    if (m_vScroll) m_vScroll->HandleMouseLeave();
    if (m_hScroll) m_hScroll->HandleMouseLeave();
}
#pragma endregion