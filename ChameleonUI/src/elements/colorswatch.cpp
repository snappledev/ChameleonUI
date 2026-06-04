#include "../../include/elements/colorswatch.h"
#include "../../include/elements/colorpickermodal.h"
#include "../../include/skins/skin.h"

#pragma region lifecycle
ColorSwatch::ColorSwatch()
{
    SetWidthPolicy(SizePolicy::ContentFit);
    SetHeightPolicy(SizePolicy::ContentFit);
}

void ColorSwatch::SetColor(const Color& color)
{
    m_color = color;
    if (m_linkedColor)
        *m_linkedColor = color;
}
#pragma endregion

#pragma region drawing
void ColorSwatch::Draw(IRenderer* renderer)
{
    if (m_linkedColor)
        m_color = *m_linkedColor;

    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawColorSwatch(renderer, *this, GetAbsoluteRect());
}
#pragma endregion

#pragma region measure & arrange
void ColorSwatch::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;
    m_desiredSize.width = m_swatchWidth;
    m_desiredSize.height = m_swatchHeight;
    m_needMeasure = false;
}
#pragma endregion

#pragma region input
void ColorSwatch::OnMouseEnter()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnColorSwatchHoverEnter(*this);
}

void ColorSwatch::OnMouseLeave()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnColorSwatchHoverLeave(*this);
}

void ColorSwatch::OnClick()
{
    if (!m_modal) return;

    if (m_modal->IsOpen() && m_modal->GetOwnerSwatch() == this)
    {
        m_modal->Close();
        return;
    }

    m_modal->SetOwnerSwatch(this);

    Skin* skin = GetEffectiveSkin();
    float mw = (skin && skin->GetStyle()) ? skin->GetStyle()->colorPickerWidth : 300.f;
    float mh = (skin && skin->GetStyle()) ? skin->GetStyle()->colorPickerHeight : 260.f;
    m_modal->SetModalSize(mw, mh);

    m_modal->Open(m_color);
    m_modal->UpdateAnchorPosition();
}
#pragma endregion