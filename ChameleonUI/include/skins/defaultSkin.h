#pragma once
#include "Skin.h"

class DefaultSkin : public Skin
{
public:
    std::unique_ptr<UIStyle> CreateDefaultStyle() const override
    {
        auto style = std::make_unique<UIStyle>();
        return style;
    }
    #pragma region content size
    Size  GetCheckboxContentSize(const Checkbox& checkbox) const override;
    Size  GetButtonContentSize(const Button& button) const override;
    float GetComboBoxMinHeight() const override;

    Rect GetHitRect(const UIElement& element, const Rect& absRect) const override;
    #pragma endregion

    #pragma region animation overrides
    void OnButtonPressed(Button& button) override;
    void OnButtonReleased(Button& button) override;
    void OnCheckboxToggled(Checkbox& checkbox, bool nowChecked) override;
    #pragma endregion

    #pragma region drawing
    void DrawWindowBackground(IRenderer* renderer, const Window& window, const Rect& absRect) override;
    void DrawWindowTitleBar(IRenderer* renderer, const Window& window, const Rect& absRect) override;
    void DrawButton(IRenderer* renderer, const Button& button, const Rect& absRect, bool hovered, bool pressed) override;
    void DrawCheckbox(IRenderer* renderer, const Checkbox& checkbox, const Rect& absRect) override;
    void DrawLabel(IRenderer* renderer, const Label& label, const Rect& absRect) override;
    void DrawGroupBox(IRenderer* renderer, const GroupBox& groupBox, const Rect& absRect) override;
    void DrawSlider(IRenderer* renderer, const Slider& slider, const Rect& absRect) override;
    void DrawTabControl(IRenderer* renderer, const TabControl& tabControl, const Rect& absRect) override;
    void DrawTabPageBody(IRenderer* renderer, const TabPage& tabPage, const Rect& absRect) override;
    void DrawTabButton(IRenderer* renderer, const TabPage& tabPage, const Rect& absRect, bool active) override;
    void DrawComboBox(IRenderer* renderer, const ComboBox& combo, const Rect& absRect) override;
    void DrawComboBoxDropdown(IRenderer* renderer, const ComboBox& combo) override;
    void DrawScrollbarTrack(IRenderer* renderer, const Rect& rect, bool vertical) override;
    void DrawScrollbarThumb(IRenderer* renderer, const Rect& rect, bool vertical, bool hovered) override;
    void DrawScrollbar(IRenderer* renderer, const Rect& trackRect, const Rect& thumbRect, bool vertical, bool thumbHovered) override;
    void DrawTextInput(IRenderer*, const TextInputVisuals&) override;
    void DrawKeyBind(IRenderer* renderer, const KeyBind& keybind, const Rect& absRect) override;
    void DrawListBox(IRenderer* renderer, const ListBox& listbox, const Rect& absRect) override;
    void DrawColorSwatch(IRenderer* renderer, const ColorSwatch& swatch, const Rect& absRect) override;
    void DrawColorPickerModal(IRenderer* renderer, ColorPickerModal& modal, const Rect& absRect) override;
    void DrawNumericInput(IRenderer* renderer, const NumericInput& input, const Rect& absRect) override;
    void DrawColorPicker(IRenderer* renderer, const ColorPicker& picker, const Rect& absRect) override;
    #pragma endregion

    #pragma region shared primitives
    void DrawCheckMark(IRenderer* renderer, const Rect& boxRect,
        float checkT, Color markColor, float strokeWidth = 2.f) override;
    #pragma endregion

    #pragma region per-item drawing
    void DrawComboBoxItem(IRenderer* renderer, const Rect& itemRect,
        const std::wstring& text, float hoverT, float checkT,
        bool isMultiSelect, float cornerRadius) override;
    void DrawListBoxItem(IRenderer* renderer, const Rect& itemRect,
        const std::wstring& text, float hoverT, float checkT,
        bool isMultiSelect, float cornerRadius) override;
    #pragma endregion
};