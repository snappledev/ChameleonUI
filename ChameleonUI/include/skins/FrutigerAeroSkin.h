#pragma once
#include "DefaultSkin.h"

class FrutigerAeroSkin : public DefaultSkin
{
public:
    #pragma region animation
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
    void DrawComboBoxItem(IRenderer* renderer, const Rect& itemRect, const std::wstring& text, float hoverT, float checkT, bool isMultiSelect, float cornerRadius) override;
    void DrawTextInput(IRenderer* renderer, const TextInputVisuals& visuals) override;
    void DrawKeyBind(IRenderer* renderer, const KeyBind& keybind, const Rect& absRect) override;
    void DrawScrollbarTrack(IRenderer* renderer, const Rect& rect, bool vertical) override;
    void DrawScrollbarThumb(IRenderer* renderer, const Rect& rect, bool vertical, bool hovered) override;
    void DrawListBox(IRenderer* renderer, const ListBox& listbox, const Rect& absRect) override;
    void DrawListBoxItem(IRenderer* renderer, const Rect& itemRect, const std::wstring& text, float hoverT, float checkT, bool isMultiSelect, float cornerRadius) override;
    void DrawColorSwatch(IRenderer* renderer, const ColorSwatch& swatch, const Rect& absRect) override;
    void DrawColorPickerModal(IRenderer* renderer, ColorPickerModal& modal, const Rect& absRect) override;
    void DrawColorPicker(IRenderer* renderer, const ColorPicker& picker, const Rect& absRect) override;
    void DrawCheckMark(IRenderer* renderer, const Rect& boxRect, float checkT, Color markColor, float strokeWidth = 2.f) override;
    void DrawNumericInput(IRenderer* renderer, const NumericInput& input, const Rect& absRect) override;
    void OnTabActivated(TabPage& tabPage) override;
    void OnTabDeactivated(TabPage& tabPage) override;
    #pragma endregion
protected:
    std::unique_ptr<UIStyle> CreateDefaultStyle() const override;

private:
    #pragma region structural helpers
    void DrawBevelRaised(IRenderer* renderer, const Rect& rect, float cr, uint8_t strength = 60);
    void DrawBevelSunken(IRenderer* renderer, const Rect& rect, float cr, uint8_t strength = 40);
    void DrawGripLines(IRenderer* renderer, float cx, float cy, float width, int count, Color light, Color dark);
    void DrawGloss(IRenderer* renderer, const Rect& rect, float cr, uint8_t alpha = 50);
    #pragma endregion
};