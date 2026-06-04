#pragma once
#include "../core/UIElement.h"
#include "ColorPickerModal.h"
#include <functional>
#include "../util/huebarposition.h"

class ColorPicker : public UIElement
{
public:
    ColorPicker(float width = 200.f, float height = 200.f);

    #pragma region color
    void SetLinkedColor(Color* color);
    Color* GetLinkedColor() const { return m_linkedColor; }
    void SyncFromLinked();
    Color GetCurrentColor() const { return m_currentColor; }
    void SetColor(const Color& color);
    #pragma endregion

    #pragma region hue bar
    HueBarPosition GetHueBarPosition() const { return m_hueBarPosition; }
    void SetHueBarPosition(HueBarPosition pos) { m_hueBarPosition = pos; InvalidateMeasure(); }
    float GetHueBarThickness() const { return m_hueBarThickness; }
    void SetHueBarThickness(float t) { m_hueBarThickness = t; InvalidateMeasure(); }
    float GetHueBarGap() const { return m_hueBarGap; }
    void SetHueBarGap(float g) { m_hueBarGap = g; InvalidateMeasure(); }
    bool GetShowAlphaBar() const { return m_showAlphaBar; }
    void SetShowAlphaBar(bool show) { m_showAlphaBar = show; InvalidateMeasure(); }
    #pragma endregion

    #pragma region hsv access
    float GetHue() const { return m_hue; }
    float GetSaturation() const { return m_saturation; }
    float GetBrightness() const { return m_brightness; }
    uint8_t GetAlpha() const { return m_currentColor.a; }
    #pragma endregion

    #pragma region layout rects
    Rect GetGradientRect() const { return m_gradientRect; }
    Rect GetHueBarRect() const { return m_hueBarRect; }
    Rect GetAlphaBarRect() const { return m_alphaBarRect; }
    #pragma endregion

    #pragma region interaction
    bool IsDraggingGradient() const { return m_draggingGradient; }
    bool IsDraggingHueBar() const { return m_draggingHueBar; }
    bool IsDraggingAlphaBar() const { return m_draggingAlphaBar; }
    #pragma endregion

    #pragma region callback
    void SetOnColorChanged(std::function<void(const Color&)> cb) { m_onColorChanged = cb; }
    #pragma endregion

    // UIElement overrides
    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    UIElement* HitTest(int absX, int absY) override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseMove(int x, int y) override;

private:
    #pragma region helpers
    void ComputeLayoutRects();
    void UpdateColorFromHSV();
    void WriteToLinked();
    #pragma endregion

    #pragma region members
    Color*                            m_linkedColor    = nullptr;
    Color                             m_currentColor{ 255, 255, 255, 255 };
    float                             m_hue            = 0.f, m_saturation = 0.f, m_brightness = 1.f;

    HueBarPosition                    m_hueBarPosition  = HueBarPosition::Right;
    float                             m_hueBarThickness = 18.f;
    float                             m_hueBarGap       = 8.f;
    float                             m_pickerWidth     = 200.f;
    float                             m_pickerHeight    = 200.f;
    bool                              m_showAlphaBar    = false;

    bool                              m_draggingGradient = false;
    bool                              m_draggingHueBar   = false;
    bool                              m_draggingAlphaBar = false;

    Rect                              m_gradientRect;
    Rect                              m_hueBarRect;
    Rect                              m_alphaBarRect;

    std::function<void(const Color&)> m_onColorChanged;
    #pragma endregion
};