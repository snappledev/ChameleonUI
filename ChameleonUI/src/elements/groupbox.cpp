#include "../../include/elements/GroupBox.h"
#include "../../include/layouts/Panel.h"
#include "../../include/skins/Skin.h"
#include "../../include/layouts/layout_helpers.h"
#include "../../include/util/scoped_timer.h"
#include "../../include/elements/TabControl.h"
#include <algorithm>

#pragma region lifecycle
GroupBox::GroupBox(const std::wstring& title, int x, int y, int w, int h)
    : m_title(title)
{
    SetX(x); SetY(y); SetWidth(w); SetHeight(h);
    SetAutoLayoutVertical(false);

    auto& panel = AddChild<Panel>();
    panel.SetScrollMode(ScrollMode::Vertical);
    panel.SetAutoLayoutVertical(true);
    panel.SetAutoSpacing(8);
    panel.SetPadding(0, 0, 0, 0);
    panel.SetMargin(0, 0, 0, 0);
    m_contentPanel = &panel;
}

const std::wstring& GroupBox::GetTitle() const { return m_title; }
void GroupBox::SetTitle(const std::wstring& title) { m_title = title; }

Rect GroupBox::GetClientRect() const
{
    Rect abs = GetAbsoluteRect();
    int labelH = m_showTitleBar ? m_labelHeight : 0;
    float insetX = m_borderWidth + 2.f + m_padding.x;
    float insetTop = labelH + m_borderWidth + 2.f + m_padding.y;
    float insetRight = m_borderWidth + 2.f + m_padding.w;
    float insetBottom = m_borderWidth + 2.f + m_padding.h;
    return Rect(abs.x + insetX,
        abs.y + insetTop,
        abs.w - insetX - insetRight,
        abs.h - insetTop - insetBottom);
}
#pragma endregion

#pragma region drawing
void GroupBox::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawGroupBox(renderer, *this, GetAbsoluteRect());
}

void GroupBox::DrawChildren(IRenderer* renderer, const Rect& parentClip)
{
    Rect abs = GetAbsoluteRect();
    int labelH = m_showTitleBar ? m_labelHeight : 0;
    float insetX = m_borderWidth + 2.f;
    float insetTop = labelH + m_borderWidth + 2.f;
    float insetRight = m_borderWidth + 2.f;
    float insetBottom = m_borderWidth + 2.f;
    Rect interior(abs.x + insetX, abs.y + insetTop,
        abs.w - insetX - insetRight,
        abs.h - insetTop - insetBottom);

    Rect clip = parentClip.Intersect(interior);
    UIElement::DrawChildren(renderer, clip);
}
#pragma endregion

#pragma region measure & arrange
void GroupBox::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    int border = m_borderWidth + 2;
    int labelH = m_showTitleBar ? m_labelHeight : 0;

    float clientWidth = availableSize.width - border * 2 - m_padding.x - m_padding.w;
    if (clientWidth < 0) clientWidth = 0;

    if (m_contentPanel)
        m_contentPanel->Measure(Size{ clientWidth, INFINITY });

    float contentW = m_contentPanel ? m_contentPanel->GetDesiredSize().width : 0.f;
    float contentH = m_contentPanel ? m_contentPanel->GetDesiredSize().height : 0.f;

    float chromeH = (float)(labelH + border * 2) + m_padding.y + m_padding.h;
    float naturalW = contentW + border * 2 + m_padding.x + m_padding.w;
    float naturalH = contentH + chromeH;

    m_desiredSize.width = (m_width > 0) ? (float)m_width : naturalW;
    m_desiredSize.height = (m_height > 0) ? (float)m_height : naturalH;

    m_containsStretchH = m_contentPanel ? m_contentPanel->WantsStretchH() : false;
    m_containsStretchW = m_contentPanel ? m_contentPanel->WantsStretchW() : false;

    m_needMeasure = false;
}

void GroupBox::Arrange(const Rect& finalRect)
{
    if (CanSkipArrange(finalRect)) return;
    UIElement::Arrange(finalRect);
    if (!m_contentPanel) return;

    Rect client = GetClientRect();
    Rect panelRect((float)client.x, (float)client.y, (float)client.w, (float)client.h);
    m_contentPanel->Arrange(panelRect);
}
#pragma endregion

#pragma region input
UIElement* GroupBox::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;
    Rect myRect = GetAbsoluteRect();
    if (!myRect.Contains(absX, absY)) return nullptr;
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
    {
        UIElement* hit = (*it)->HitTest(absX, absY);
        if (hit) return hit;
    }
    return this;
}
#pragma endregion