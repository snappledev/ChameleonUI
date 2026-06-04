#include "../../include/elements/colorpicker.h"
#include "../../include/skins/skin.h"
#include <algorithm>

#pragma region lifecycle
ColorPicker::ColorPicker(float width, float height)
    : m_pickerWidth(width), m_pickerHeight(height)
{
    SetAutoLayoutVertical(false);
}

void ColorPicker::SetLinkedColor(Color* color)
{
    m_linkedColor = color;
    if (color)
        SyncFromLinked();
}

void ColorPicker::SyncFromLinked()
{
    if (!m_linkedColor) return;
    m_currentColor = *m_linkedColor;
    ColorPickerModal::RGBToHSV(m_currentColor, m_hue, m_saturation, m_brightness);
}

void ColorPicker::SetColor(const Color& color)
{
    m_currentColor = color;
    ColorPickerModal::RGBToHSV(m_currentColor, m_hue, m_saturation, m_brightness);
    WriteToLinked();
}

void ColorPicker::UpdateColorFromHSV()
{
    uint8_t a = m_currentColor.a;
    m_currentColor = ColorPickerModal::HSVToRGB(m_hue, m_saturation, m_brightness, a);
    WriteToLinked();
    if (m_onColorChanged)
        m_onColorChanged(m_currentColor);
}

void ColorPicker::WriteToLinked()
{
    if (m_linkedColor)
        *m_linkedColor = m_currentColor;
}
#pragma endregion

#pragma region drawing
void ColorPicker::Draw(IRenderer* renderer)
{
    ComputeLayoutRects();

    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawColorPicker(renderer, *this, GetAbsoluteRect());
}
#pragma endregion

#pragma region measure & arrange
void ColorPicker::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;
    m_desiredSize.width = m_pickerWidth;
    m_desiredSize.height = m_pickerHeight;
    m_needMeasure = false;
}

void ColorPicker::Arrange(const Rect& finalRect)
{
    UIElement::Arrange(finalRect);
    ComputeLayoutRects();
}

void ColorPicker::ComputeLayoutRects()
{
    Rect abs = GetAbsoluteRect();
    float barT = m_hueBarThickness;
    float gap = m_hueBarGap;

    int barCount = 1;
    if (m_showAlphaBar) barCount = 2;

    float totalBarSpace = barT * barCount + gap * barCount;

    switch (m_hueBarPosition)
    {
    case HueBarPosition::Right:
        m_gradientRect = Rect(abs.x, abs.y, abs.w - totalBarSpace, abs.h);
        m_hueBarRect = Rect(abs.x + abs.w - barT, abs.y, barT, abs.h);
        m_alphaBarRect = Rect(m_hueBarRect.x - barT - gap, abs.y, barT, abs.h);
        break;
    case HueBarPosition::Left:
        m_hueBarRect = Rect(abs.x, abs.y, barT, abs.h);
        m_alphaBarRect = Rect(abs.x + barT + gap, abs.y, barT, abs.h);
        m_gradientRect = Rect(abs.x + totalBarSpace, abs.y, abs.w - totalBarSpace, abs.h);
        break;
    case HueBarPosition::Top:
        m_hueBarRect = Rect(abs.x, abs.y, abs.w, barT);
        m_alphaBarRect = Rect(abs.x, abs.y + barT + gap, abs.w, barT);
        m_gradientRect = Rect(abs.x, abs.y + totalBarSpace, abs.w, abs.h - totalBarSpace);
        break;
    case HueBarPosition::Bottom:
        m_gradientRect = Rect(abs.x, abs.y, abs.w, abs.h - totalBarSpace);
        m_alphaBarRect = Rect(abs.x, abs.y + abs.h - barT * 2 - gap, abs.w, barT);
        m_hueBarRect = Rect(abs.x, abs.y + abs.h - barT, abs.w, barT);
        break;
    }

    if (!m_showAlphaBar)
    {
        float singleBarSpace = barT + gap;
        switch (m_hueBarPosition)
        {
        case HueBarPosition::Right:
            m_gradientRect = Rect(abs.x, abs.y, abs.w - singleBarSpace, abs.h);
            m_hueBarRect = Rect(abs.x + abs.w - barT, abs.y, barT, abs.h);
            break;
        case HueBarPosition::Left:
            m_hueBarRect = Rect(abs.x, abs.y, barT, abs.h);
            m_gradientRect = Rect(abs.x + singleBarSpace, abs.y, abs.w - singleBarSpace, abs.h);
            break;
        case HueBarPosition::Top:
            m_hueBarRect = Rect(abs.x, abs.y, abs.w, barT);
            m_gradientRect = Rect(abs.x, abs.y + singleBarSpace, abs.w, abs.h - singleBarSpace);
            break;
        case HueBarPosition::Bottom:
            m_gradientRect = Rect(abs.x, abs.y, abs.w, abs.h - singleBarSpace);
            m_hueBarRect = Rect(abs.x, abs.y + abs.h - barT, abs.w, barT);
            break;
        }
        m_alphaBarRect = Rect(0, 0, 0, 0);
    }
}
#pragma endregion

#pragma region input
UIElement* ColorPicker::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;
    Rect abs = GetAbsoluteRect();
    if (abs.Contains(absX, absY))
        return this;
    return nullptr;
}

void ColorPicker::OnMouseDown(int button)
{
    if (button != 0) return;

    extern int g_lastMouseX, g_lastMouseY;
    float mx = (float)g_lastMouseX;
    float my = (float)g_lastMouseY;

    if (m_gradientRect.Contains(mx, my))
    {
        m_draggingGradient = true;
        CaptureInput();
        float s = (mx - m_gradientRect.x) / m_gradientRect.w;
        float v = 1.f - (my - m_gradientRect.y) / m_gradientRect.h;
        m_saturation = std::clamp(s, 0.f, 1.f);
        m_brightness = std::clamp(v, 0.f, 1.f);
        UpdateColorFromHSV();
    }
    else if (m_hueBarRect.Contains(mx, my))
    {
        m_draggingHueBar = true;
        CaptureInput();
        bool vertical = (m_hueBarPosition == HueBarPosition::Left || m_hueBarPosition == HueBarPosition::Right);
        float h = vertical
            ? (my - m_hueBarRect.y) / m_hueBarRect.h
            : (mx - m_hueBarRect.x) / m_hueBarRect.w;
        m_hue = std::clamp(h, 0.f, 1.f);
        UpdateColorFromHSV();
    }
    else if (m_showAlphaBar && m_alphaBarRect.Contains(mx, my))
    {
        m_draggingAlphaBar = true;
        CaptureInput();
        bool vertical = (m_hueBarPosition == HueBarPosition::Left || m_hueBarPosition == HueBarPosition::Right);
        float margin = 3.f;
        float t = vertical
            ? (my - m_alphaBarRect.y - margin) / (m_alphaBarRect.h - margin * 2.f)
            : (mx - m_alphaBarRect.x - margin) / (m_alphaBarRect.w - margin * 2.f);
        t = std::clamp(t, 0.f, 1.f);
        m_currentColor.a = (uint8_t)(255.f * (1.f - t));
        WriteToLinked();
        if (m_onColorChanged) m_onColorChanged(m_currentColor);
    }
}
#pragma endregion

void ColorPicker::OnMouseUp(int button)
{
    if (button == 0)
    {
        if (m_draggingGradient || m_draggingHueBar || m_draggingAlphaBar)
            ReleaseInput();
        m_draggingGradient = false;
        m_draggingHueBar = false;
        m_draggingAlphaBar = false;
    }
}

void ColorPicker::OnMouseMove(int x, int y)
{
    float mx = (float)x;
    float my = (float)y;

    if (m_draggingGradient)
    {
        float s = (mx - m_gradientRect.x) / m_gradientRect.w;
        float v = 1.f - (my - m_gradientRect.y) / m_gradientRect.h;
        m_saturation = std::clamp(s, 0.f, 1.f);
        m_brightness = std::clamp(v, 0.f, 1.f);
        UpdateColorFromHSV();
    }
    else if (m_draggingHueBar)
    {
        bool vertical = (m_hueBarPosition == HueBarPosition::Left || m_hueBarPosition == HueBarPosition::Right);
        float h = vertical
            ? (my - m_hueBarRect.y) / m_hueBarRect.h
            : (mx - m_hueBarRect.x) / m_hueBarRect.w;
        m_hue = std::clamp(h, 0.f, 1.f);
        UpdateColorFromHSV();
    }
    else if (m_draggingAlphaBar)
    {
        bool vertical = (m_hueBarPosition == HueBarPosition::Left || m_hueBarPosition == HueBarPosition::Right);
        float margin = 3.f;
        float t = vertical
            ? (my - m_alphaBarRect.y - margin) / (m_alphaBarRect.h - margin * 2.f)
            : (mx - m_alphaBarRect.x - margin) / (m_alphaBarRect.w - margin * 2.f);
        t = std::clamp(t, 0.f, 1.f);
        m_currentColor.a = (uint8_t)(255.f * (1.f - t));
        WriteToLinked();
        if (m_onColorChanged) m_onColorChanged(m_currentColor);
    }
}