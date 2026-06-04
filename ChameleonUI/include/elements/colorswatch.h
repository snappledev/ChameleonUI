#pragma once
#include "../core/UIElement.h"

class ColorPickerModal;

class ColorSwatch : public UIElement
{
public:
    ColorSwatch();

    Color GetColor() const { return m_color; }
    void SetColor(const Color& color);

    void SetLinkedColor(Color* linked) { m_linkedColor = linked; }
    Color* GetLinkedColor() const { return m_linkedColor; }
    float GetSwatchWidth() const { return m_swatchWidth; }
    float GetSwatchHeight() const { return m_swatchHeight; }
    void SetSwatchSize(float w, float h) { m_swatchWidth = w; m_swatchHeight = h; InvalidateMeasure(); }
    void SetModal(ColorPickerModal* modal) { m_modal = modal; }
    ColorPickerModal* GetModal() const { return m_modal; }

    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnClick() override;

private:
    #pragma region members
    Color             m_color{ 255, 255, 255, 255 };
    Color*            m_linkedColor  = nullptr;
    float             m_swatchWidth  = 24.f;
    float             m_swatchHeight = 16.f;
    ColorPickerModal* m_modal        = nullptr;
    #pragma endregion
};