#include "../../include/layouts/verticalpanel.h"
#include "../../include/layouts/layout_helpers.h"
#include "../../include/util/scoped_timer.h"
#include <algorithm>

#pragma region lifecycle
VerticalPanel::VerticalPanel()
{
    SetAutoLayoutVertical(true);
    SetAutoSpacing(8);
}
#pragma endregion

#pragma region measure & arrange
void VerticalPanel::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    float effectiveWidth = availableSize.width - (float)(m_padding.x + m_padding.w);
    if (effectiveWidth < 10) effectiveWidth = 200;

    VStackMeasure r = MeasureVerticalStack(m_children, effectiveWidth, m_autoSpacing);

    float totalHeight = r.totalHeight + (float)(m_padding.y + m_padding.h);
    if (!m_children.empty() && totalHeight < 10 && !r.containsStretchH)
        totalHeight = 100;

    m_desiredSize.width = r.maxWidth + (float)(m_padding.x + m_padding.w);
    m_desiredSize.height = totalHeight;

    m_containsStretchH = r.containsStretchH;

    m_needMeasure = false;
}
void VerticalPanel::Arrange(const Rect& finalRect)
{
    if (CanSkipArrange(finalRect)) return;
    UIElement::Arrange(finalRect);
    if (!m_autoLayoutVertical) return;
    ArrangeVerticalStack(m_children, GetClientRect(), m_autoSpacing);
}
#pragma endregion

#pragma region drawing
void VerticalPanel::Draw(IRenderer* renderer)
{
}
#pragma endregion
