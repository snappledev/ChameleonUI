#pragma once
#include "../core/UIElement.h"
#include <functional>

class ColorSwatch;
class ColorPicker;

class ColorPickerModal : public UIElement
{
public:
    ColorPickerModal();

    #pragma region lifecycle
    void Open(const Color& initialColor);
    void Close();
    bool IsOpen() const { return m_open; }
    #pragma endregion

    #pragma region owner swatch
    void SetOwnerSwatch(ColorSwatch* swatch) { m_ownerSwatch = swatch; }
    ColorSwatch* GetOwnerSwatch() const { return m_ownerSwatch; }
    #pragma endregion

    #pragma region color state
    Color GetCurrentColor() const;
    Color GetPreviousColor() const { return m_previousColor; }
    float GetHue() const;
    float GetSaturation() const;
    float GetBrightness() const;
    ColorPicker* GetPicker() const { return m_picker; }
    #pragma endregion

    #pragma region interaction
    bool IsDraggingGradient() const;
    bool IsDraggingHueBar() const;
    #pragma endregion

    #pragma region layout rects
    Rect GetGradientRect() const;
    Rect GetHueBarRect() const;
    Rect GetPreviousColorRect() const { return m_prevColorRect; }
    Rect GetCurrentColorRect() const { return m_currColorRect; }
    Rect GetApplyButtonRect() const { return m_applyRect; }
    Rect GetCancelButtonRect() const { return m_cancelRect; }
    bool IsApplyHovered() const { return m_applyHovered; }
    bool IsCancelHovered() const { return m_cancelHovered; }
    #pragma endregion

    #pragma region callbacks & size
    void SetOnApply(std::function<void(const Color&)> cb) { m_onApply = cb; }
    void SetOnCancel(std::function<void()> cb) { m_onCancel = cb; }
    float GetModalWidth() const { return m_modalWidth; }
    float GetModalHeight() const { return m_modalHeight; }
    void SetModalSize(float w, float h) { m_modalWidth = w; m_modalHeight = h; }
    void UpdateAnchorPosition();
    #pragma endregion

    // UIElement overrides
    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    UIElement* HitTest(int absX, int absY) override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseMove(int x, int y) override;
    void Update(float dt) override;

    // HSV to RGB conversion (static utility)
    static Color HSVToRGB(float h, float s, float v, uint8_t a = 255);
    static void RGBToHSV(const Color& c, float& h, float& s, float& v);

private:
    #pragma region helpers
    void Apply();
    void Cancel();
    void ComputeModalRects();
    #pragma endregion

    #pragma region members
    bool                              m_open           = false;
    Color                             m_previousColor{ 255, 255, 255, 255 };

    ColorSwatch*                      m_ownerSwatch    = nullptr;
    ColorPicker*                      m_picker         = nullptr;

    bool                              m_applyHovered   = false;
    bool                              m_cancelHovered  = false;

    Rect                              m_prevColorRect, m_currColorRect;
    Rect                              m_applyRect, m_cancelRect;

    float                             m_modalWidth     = 300.f;
    float                             m_modalHeight    = 260.f;

    std::function<void(const Color&)> m_onApply;
    std::function<void()>             m_onCancel;
    #pragma endregion
};