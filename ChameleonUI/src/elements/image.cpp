#include "../../include/elements/image.h"

#pragma region lifecycle
Image::Image()
{
    m_widthPolicy = SizePolicy::ContentFit;
    m_heightPolicy = SizePolicy::ContentFit;
}
#pragma endregion

#pragma region measure & arrange
void Image::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    float w = (m_width > 0) ? (float)m_width : m_naturalWidth;
    float h = (m_height > 0) ? (float)m_height : m_naturalHeight;

    if (w <= 0) w = 64.f;
    if (h <= 0) h = 64.f;

    m_desiredSize.width = w;
    m_desiredSize.height = h;
    m_needMeasure = false;
}
#pragma endregion

#pragma region drawing
void Image::Draw(IRenderer* renderer)
{
    if (!m_handle) return;

    Rect absRect = GetAbsoluteRect();

    if (m_useSourceRect)
        renderer->DrawImageRegion(m_handle, m_sourceRect, absRect, m_opacity);
    else
        renderer->DrawImage(m_handle, absRect, m_opacity);
}
#pragma endregion