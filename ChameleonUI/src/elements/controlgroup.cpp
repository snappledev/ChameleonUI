#include "../../include/elements/controlgroup.h"
#include "../../include/elements/colorswatch.h"
#include <algorithm>

#pragma region lifecycle
ControlGroup::ControlGroup()
{
    SetAutoLayoutVertical(false);
}
#pragma endregion

#pragma region measure & arrange
void ControlGroup::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;
    if (!m_control) { m_needMeasure = false; return; }

    float indent = m_controlIndent;

    Size swatchSize = { 0, 0 };
    if (m_colorSwatch && m_swatchAlign != ColorSwatchAlignment::None)
    {
        m_colorSwatch->Measure(Size{ availableSize.width, INFINITY });
        swatchSize = m_colorSwatch->GetDesiredSize();
    }
    float swatchReserved = (swatchSize.width > 0) ? swatchSize.width + m_swatchSpacing : 0.f;

    float availForContent = availableSize.width - indent - swatchReserved;
    if (availForContent < 0) availForContent = 0;

    m_control->Measure(Size{ availForContent, INFINITY });
    Size ctrlSize = m_control->GetDesiredSize();

    Size lblSize = { 0, 0 };
    if (m_label && m_labelPos != ElementPosition::None)
    {
        m_label->Measure(Size{ availForContent, INFINITY });
        lblSize = m_label->GetDesiredSize();
    }

    float contentW = 0, contentH = 0;
    switch (m_labelPos)
    {
    case ElementPosition::Left:
        contentW = ctrlSize.width + m_labelSpacing + lblSize.width;
        contentH = (std::max)(ctrlSize.height, lblSize.height);
        break;
    case ElementPosition::Right:
        contentW = lblSize.width + m_labelSpacing + ctrlSize.width;
        contentH = (std::max)(ctrlSize.height, lblSize.height);
        break;
    case ElementPosition::Above:
        contentW = (std::max)(ctrlSize.width, lblSize.width);
        contentH = lblSize.height + m_labelSpacing + ctrlSize.height;
        break;
    case ElementPosition::Below:
        contentW = (std::max)(ctrlSize.width, lblSize.width);
        contentH = ctrlSize.height + m_labelSpacing + lblSize.height;
        break;
    default:
        contentW = ctrlSize.width;
        contentH = ctrlSize.height;
        break;
    }

    m_desiredSize.width = indent + contentW + swatchReserved;
    m_desiredSize.height = (std::max)(contentH, swatchSize.height);

    if (m_widthPolicy == SizePolicy::Stretch)
        m_desiredSize.width = availableSize.width;

    if (m_desiredSize.width <= 0) m_desiredSize.width = 100;
    if (m_desiredSize.height <= 0) m_desiredSize.height = 20;
    m_needMeasure = false;
}

void ControlGroup::Arrange(const Rect& finalRect)
{
    UIElement::Arrange(finalRect);
    if (!m_control) return;

    Rect client = GetClientRect();
    Size ctrlSize = m_control->GetDesiredSize();
    Size lblSize = (m_label) ? m_label->GetDesiredSize() : Size{ 0, 0 };

    float indent = m_controlIndent;

    Size swatchSize = { 0, 0 };
    float swatchReserved = 0.f;
    if (m_colorSwatch && m_swatchAlign != ColorSwatchAlignment::None)
    {
        swatchSize = m_colorSwatch->GetDesiredSize();
        swatchReserved = swatchSize.width + m_swatchSpacing;
    }

    float contentLeft = client.x + indent;
    float contentRight = client.x + client.w;
    if (m_swatchAlign == ColorSwatchAlignment::Right)
        contentRight -= swatchReserved;
    else if (m_swatchAlign == ColorSwatchAlignment::Left)
        contentLeft += swatchReserved;

    float contentW = contentRight - contentLeft;
    if (contentW < 0) contentW = 0;

    switch (m_labelPos)
    {
    case ElementPosition::Left:
    {
        float ctrlX = contentLeft;
        float ctrlY = client.y + (client.h - ctrlSize.height) * 0.5f;
        m_control->Arrange(Rect(ctrlX, ctrlY, ctrlSize.width, ctrlSize.height));
        if (m_label)
        {
            float lblX = ctrlX + ctrlSize.width + m_labelSpacing;
            float lblY = client.y + (client.h - lblSize.height) * 0.5f;
            float lblW = contentRight - lblX;
            if (lblW < 0) lblW = 0;
            m_label->Arrange(Rect(lblX, lblY, lblW, lblSize.height));
        }
        break;
    }
    case ElementPosition::Right:
    {
        if (m_label)
        {
            float lblX = contentLeft;
            float lblY = client.y + (client.h - lblSize.height) * 0.5f;
            m_label->Arrange(Rect(lblX, lblY, lblSize.width, lblSize.height));
        }
        float ctrlX = contentLeft + lblSize.width + m_labelSpacing;
        float ctrlY = client.y + (client.h - ctrlSize.height) * 0.5f;
        float ctrlW = contentRight - ctrlX;
        if (ctrlW < 0) ctrlW = 0;
        m_control->Arrange(Rect(ctrlX, ctrlY,
            (m_control->GetWidthPolicy() == SizePolicy::Stretch) ? ctrlW : ctrlSize.width,
            ctrlSize.height));
        break;
    }
    case ElementPosition::Above:
    {
        if (m_label)
            m_label->Arrange(Rect(contentLeft, client.y, contentW, lblSize.height));
        float ctrlY = client.y + lblSize.height + m_labelSpacing;
        float ctrlW = (m_control->GetWidthPolicy() == SizePolicy::Stretch) ? contentW : (std::min)(ctrlSize.width, contentW);
        m_control->Arrange(Rect(contentLeft, ctrlY, ctrlW, client.h - lblSize.height - m_labelSpacing));
        break;
    }
    case ElementPosition::Below:
    {
        float ctrlW = (m_control->GetWidthPolicy() == SizePolicy::Stretch) ? contentW : (std::min)(ctrlSize.width, contentW);
        m_control->Arrange(Rect(contentLeft, client.y, ctrlW, ctrlSize.height));
        if (m_label)
        {
            float lblY = client.y + ctrlSize.height + m_labelSpacing;
            m_label->Arrange(Rect(contentLeft, lblY, contentW, client.h - ctrlSize.height - m_labelSpacing));
        }
        break;
    }
    default:
        m_control->Arrange(Rect(contentLeft, client.y, contentW, client.h));
        break;
    }

    if (m_colorSwatch && m_swatchAlign != ColorSwatchAlignment::None)
    {
        float swatchY = client.y + (client.h - swatchSize.height) * 0.5f;
        float swatchX;
        if (m_swatchAlign == ColorSwatchAlignment::Right)
            swatchX = client.x + client.w - swatchSize.width;
        else
            swatchX = client.x;

        m_colorSwatch->Arrange(Rect(swatchX, swatchY, swatchSize.width, swatchSize.height));
    }
}
#pragma endregion

#pragma region drawing
void ControlGroup::Draw(IRenderer* /*renderer*/)
{
}
#pragma endregion

#pragma region input
UIElement* ControlGroup::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;

    Rect myRect = GetAbsoluteRect();
    if (!myRect.Contains(absX, absY)) return nullptr;

    if (m_colorSwatch)
    {
        UIElement* hit = m_colorSwatch->HitTest(absX, absY);
        if (hit) return hit;
    }

    if (m_control)
    {
        UIElement* hit = m_control->HitTest(absX, absY);
        if (hit) return hit;
    }

    if (m_clickLabelToggles && m_label && m_control)
    {
        UIElement* hit = m_label->HitTest(absX, absY);
        if (hit) return m_control;
    }
    if (m_label)
    {
        UIElement* hit = m_label->HitTest(absX, absY);
        if (hit) return hit;
    }

    return nullptr;
}
#pragma endregion