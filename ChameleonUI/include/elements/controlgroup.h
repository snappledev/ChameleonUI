#pragma once
#include "../core/UIElement.h"
#include "../layouts/elementposition.h"
#include "Label.h"
#include <functional>

class ColorSwatch;

enum class ColorSwatchAlignment
{
    None,   // No swatch
    Left,
    Right   // Anchored to right edge (default for attached swatches)
};

class ControlGroup : public UIElement
{
public:
    ControlGroup();

    #pragma region sub-elements
    UIElement* GetControl() const { return m_control; }
    Label* GetLabel() const { return m_label; }
    ColorSwatch* GetColorSwatch() const { return m_colorSwatch; }
    void SetControl(UIElement* ctrl) { m_control = ctrl; }
    void SetLabel(Label* lbl) { m_label = lbl; }
    void SetColorSwatch(ColorSwatch* swatch) { m_colorSwatch = swatch; }
    #pragma endregion

    #pragma region layout config
    ElementPosition GetLabelPosition() const { return m_labelPos; }
    void SetLabelPosition(ElementPosition pos) { m_labelPos = pos; InvalidateMeasure(); }
    float GetControlIndent() const { return m_controlIndent; }
    void SetControlIndent(float indent) { m_controlIndent = indent; InvalidateMeasure(); }
    float GetLabelSpacing() const { return m_labelSpacing; }
    void SetLabelSpacing(float spacing) { m_labelSpacing = spacing; InvalidateMeasure(); }
    ColorSwatchAlignment GetSwatchAlignment() const { return m_swatchAlign; }
    void SetSwatchAlignment(ColorSwatchAlignment a) { m_swatchAlign = a; InvalidateMeasure(); }
    float GetSwatchSpacing() const { return m_swatchSpacing; }
    void SetSwatchSpacing(float spacing) { m_swatchSpacing = spacing; InvalidateMeasure(); }
    bool GetClickLabelToggles() const { return m_clickLabelToggles; }
    void SetClickLabelToggles(bool enable) { m_clickLabelToggles = enable; }
    #pragma endregion

    #pragma region uielement overrides
    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    void Draw(IRenderer* renderer) override;
    UIElement* HitTest(int absX, int absY) override;

private:
    #pragma region members
    UIElement*            m_control           = nullptr;
    Label*                m_label             = nullptr;
    ColorSwatch*          m_colorSwatch       = nullptr;
    ElementPosition       m_labelPos          = ElementPosition::Left;
    ColorSwatchAlignment  m_swatchAlign       = ColorSwatchAlignment::None;
    float                 m_controlIndent     = 0.f;
    float                 m_labelSpacing      = 8.f;
    float                 m_swatchSpacing     = 4.f;
    bool                  m_clickLabelToggles = true;
    #pragma endregion
};