#include "../../include/elements/separator.h"
#include "../../include/skins/skin.h"

#pragma region lifecycle
Separator::Separator(Orientation orientation)
    : m_orientation(orientation)
{
    if (m_orientation == Orientation::Horizontal)
    {
        m_widthPolicy = SizePolicy::Stretch;
        m_heightPolicy = SizePolicy::Fixed;
        m_height = 1;
    }
    else
    {
        m_widthPolicy = SizePolicy::Fixed;
        m_heightPolicy = SizePolicy::Stretch;
        m_width = 1;
    }
}
#pragma endregion

#pragma region measure & arrange
void Separator::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    if (m_orientation == Orientation::Horizontal)
    {
        m_desiredSize.width = availableSize.width;
        m_desiredSize.height = (m_height > 0) ? (float)m_height : 1.f;
    }
    else
    {
        m_desiredSize.width = (m_width > 0) ? (float)m_width : 1.f;
        m_desiredSize.height = availableSize.height;
    }

    m_needMeasure = false;
}
#pragma endregion

#pragma region drawing
void Separator::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawSeparator(renderer, *this, GetAbsoluteRect());
}
#pragma endregion