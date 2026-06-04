#include "../../include/elements/Slider.h"
#include "../../include/skins/skin.h"
#include <cmath>
#include <cstdio>

#pragma region lifecycle
Slider::Slider(float min, float max, float value)
    : m_min(min), m_max(max), m_value(value) {}

float Slider::GetValue() const { return m_value; }
void Slider::SetValue(float value)
{
    m_value = std::clamp(value, m_min, m_max);
    if (m_linkedFloat)
        *m_linkedFloat = m_value;
}
#pragma endregion

#pragma region drawing
void Slider::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawSlider(renderer, *this, GetAbsoluteRect());
}
#pragma endregion

#pragma region measure & arrange
void Slider::Measure(const Size& /*availableSize*/)
{
    if (!m_needMeasure) return;
    m_desiredSize.width = (m_width > 0) ? (float)m_width : 120.0f;
    m_desiredSize.height = (m_height > 0) ? (float)m_height : 20.0f;
    m_needMeasure = false;
}
#pragma endregion

#pragma region update
void Slider::Update(float dt)
{
    Skin* skin = GetEffectiveSkin();

    if (m_dragging != m_wasDragging)
    {
        if (m_dragging) {
            if (skin) skin->OnSliderDragStarted(*this);
        }
        else {
            if (skin) skin->OnSliderDragStopped(*this);
        }
        m_wasDragging = m_dragging;
    }

    UIElement::Update(dt);
}

void Slider::SyncFromBinding()
{
    if (m_linkedFloat)
        SetValue(*m_linkedFloat);
}
#pragma endregion

#pragma region input
void Slider::OnMouseDown(int button)
{
    if (button == 0)
        m_dragging = true;
}

void Slider::UpdateDrag(float mouseX)
{
    if (!m_dragging) return;
    Rect abs = GetAbsoluteRect();
    float thumbWidth = 10.0f;
    float t = (mouseX - abs.x - thumbWidth / 2) / (abs.w - thumbWidth);
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    m_value = m_min + t * (m_max - m_min);
    if (m_linkedFloat)
        *m_linkedFloat = m_value;
}

void Slider::OnMouseUp(int button)
{
    if (button == 0)
        m_dragging = false;
}
#pragma endregion

#pragma region geometry
Slider::SliderGeometry Slider::ComputeGeometry(float trackHeight, float thumbRadius) const
{
    Rect absRect = GetAbsoluteRect();
    SliderGeometry g;

    g.normalizedValue = (m_max > m_min) ? (m_value - m_min) / (m_max - m_min) : 0.f;

    float trackY = absRect.y + (absRect.h - trackHeight) * 0.5f;
    g.trackRect = Rect(absRect.x, trackY, absRect.w, trackHeight);

    float usableW = absRect.w - thumbRadius * 2.f;
    g.thumbCX = absRect.x + thumbRadius + g.normalizedValue * usableW;
    g.thumbCY = absRect.y + absRect.h * 0.5f;

    float fillW = g.thumbCX - absRect.x;
    g.fillRect = Rect(absRect.x, trackY, fillW > 0.f ? fillW : 0.f, trackHeight);

    return g;
}

std::wstring Slider::GetFormattedValue() const
{
    wchar_t buf[32];
    if ((m_max - m_min) >= 2.f)
        swprintf_s(buf, L"%d", (int)std::round(m_value));
    else
        swprintf_s(buf, L"%.2f", m_value);
    return std::wstring(buf);
}
#pragma endregion