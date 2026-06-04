#include "../../include/elements/contentswitcher.h"
#include <algorithm>

#pragma region lifecycle
ContentSwitcher::ContentSwitcher()
{
    SetAutoLayoutVertical(false);
}

void ContentSwitcher::SetActiveIndex(int index)
{
    if (m_activeIndex == index) return;
    m_activeIndex = index;
    ApplyVisibility();
    InvalidateMeasure();
}

void ContentSwitcher::ApplyVisibility()
{
    for (int i = 0; i < (int)m_children.size(); ++i)
        m_children[i]->SetVisible(i == m_activeIndex);
}
#pragma endregion

#pragma region update
void ContentSwitcher::Update(float dt)
{
    if (m_indexBinding)
    {
        int newIndex = m_indexBinding();
        if (newIndex != m_activeIndex)
            SetActiveIndex(newIndex);
    }
    UIElement::Update(dt);
}
#pragma endregion

#pragma region measure & arrange
void ContentSwitcher::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    ApplyVisibility();

    float maxW = 0, maxH = 0;
    for (auto& child : m_children)
    {
        child->Measure(availableSize);
        Size ds = child->GetDesiredSize();
        Rect margin = child->GetMargin();
        maxW = (std::max)(maxW, ds.width + margin.x + margin.w);
        maxH = (std::max)(maxH, ds.height + margin.y + margin.h);
    }

    m_desiredSize.width = (m_width > 0) ? (float)m_width : maxW;
    m_desiredSize.height = (m_height > 0) ? (float)m_height : maxH;
    m_needMeasure = false;
}

void ContentSwitcher::Arrange(const Rect& finalRect)
{
    UIElement::Arrange(finalRect);
    Rect client = GetClientRect();

    for (int i = 0; i < (int)m_children.size(); ++i)
    {
        if (i == m_activeIndex)
        {
            Rect margin = m_children[i]->GetMargin();
            Rect childRect(
                client.x + margin.x,
                client.y + margin.y,
                client.w - margin.x - margin.w,
                client.h - margin.y - margin.h);
            m_children[i]->Arrange(childRect);
        }
    }
}
#pragma endregion

#pragma region drawing
void ContentSwitcher::Draw(IRenderer* /*renderer*/)
{
}
#pragma endregion