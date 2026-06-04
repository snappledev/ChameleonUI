#include "../../include/elements/TitleBar.h"
#include "../../include/elements/Window.h"
#include "../../include/skins/skin.h"
#include "../../include/layouts/layout_helpers.h"
#include "../../include/layouts/GridLayout.h"
#include <algorithm>

#pragma region lifecycle
TitleBar::TitleBar()
{
    SetAutoLayoutVertical(false);
}
#pragma endregion

#pragma region drawing
void TitleBar::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    if (skin && m_ownerWindow)
    {
        Rect absRect = GetAbsoluteRect();
        skin->DrawWindowTitleBar(renderer, *m_ownerWindow, absRect);
    }
}
#pragma endregion

#pragma region measure & arrange
void TitleBar::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    float padH = (float)(m_padding.y + m_padding.h);
    float padW = (float)(m_padding.x + m_padding.w);

    float childAvailH = (m_height > 0) ? (float)m_height - padH : availableSize.height;
    float childAvailW = availableSize.width - padW;

    float maxChildH = 0.f;
    float totalChildW = 0.f;

    for (auto& child : m_children)
    {
        if (!child->IsVisible()) continue;
        child->Measure(Size{ childAvailW, childAvailH });
        Size childSize = child->GetDesiredSize();
        maxChildH = (std::max)(maxChildH, childSize.height);
        totalChildW += childSize.width + (float)(child->GetMargin().x + child->GetMargin().w);
    }

    float desiredH = (m_height > 0) ? (float)m_height : maxChildH + padH;
    float desiredW = totalChildW + padW;

    m_desiredSize.width = desiredW;
    m_desiredSize.height = desiredH;
    m_needMeasure = false;
}

void TitleBar::Arrange(const Rect& finalRect)
{
    UIElement::Arrange(finalRect);

    Rect client = GetClientRect();
    float xCursor = client.x;

    for (auto& child : m_children)
    {
        if (!child->IsVisible()) continue;

        Size childSize = child->GetDesiredSize();
        float mx = (float)child->GetMargin().x;
        float mw = (float)child->GetMargin().w;
        float my = (float)child->GetMargin().y;
        float mh = (float)child->GetMargin().h;

        xCursor += mx;

        float childW = childSize.width;
        float childH = childSize.height;

        if (child->GetWidthPolicy() == SizePolicy::Stretch)
        {
            float remaining = client.x + client.w - xCursor - mw;
            childW = (std::max)(remaining, 0.f);
        }

        if (child->GetHeightPolicy() == SizePolicy::Stretch)
        {
            childH = client.h - my - mh;
        }

        float childY;
        if (child->GetHeightPolicy() == SizePolicy::Stretch)
            childY = client.y + my;
        else
            childY = client.y + my + (client.h - my - mh - childH) * 0.5f;

        child->Arrange(Rect(xCursor, childY, childW, childH));
        xCursor += childW + mw;
    }
}
#pragma endregion

#pragma region input
UIElement* TitleBar::DeepHitTest(UIElement* element, int absX, int absY)
{
    for (auto it = element->GetChildren().rbegin();
        it != element->GetChildren().rend(); ++it)
    {
        UIElement* child = it->get();
        if (!child->IsVisible() || !child->IsEnabled()) continue;

        Rect childRect = child->GetAbsoluteRect();
        if (!childRect.Contains(absX, absY)) continue;

        if (child->IsHitTestTransparent())
        {
            if (UIElement* deeper = DeepHitTest(child, absX, absY))
                return deeper;
            continue;
        }

        if (UIElement* childHit = child->HitTest(absX, absY))
            return childHit;
    }

    return nullptr;
}

UIElement* TitleBar::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;
    Rect abs = GetAbsoluteRect();
    if (!abs.Contains(absX, absY)) return nullptr;

    UIElement* deepHit = DeepHitTest(this, absX, absY);
    if (deepHit && deepHit != this)
        return deepHit;

    return this;
}
#pragma endregion