#pragma once
#include "../rendering/renderer.h"
#include "../core/UIStyle.h"
#include <string>
#include <memory>

class UIElement;
class Window;
class Button;
class Checkbox;
class Label;
class GroupBox;
class Slider;
class TabControl;
class TabPage;
class ComboBox;
class TextInput;
class KeyBind;
class ListBox;
struct Size;
struct Rect;
class ColorSwatch;
class ColorPickerModal;
class Separator;
class Image;
class NumericInput;
struct TextInputVisuals;
class ColorPicker;

class Skin
{
public:
    Skin();
    virtual ~Skin() = default;

    #pragma region style ownership
    UIStyle* GetStyle();
    #pragma endregion

    #pragma region layout queries
    virtual Size GetContentSize(const UIElement& element, IRenderer* renderer) const;
    virtual Rect GetHitRect(const UIElement& element, const Rect& absRect) const;

    virtual Size  GetCheckboxContentSize(const Checkbox& checkbox) const;
    virtual Size  GetButtonContentSize(const Button& button) const;
    virtual float GetComboBoxMinHeight() const;
    #pragma endregion

    #pragma region animation events

    #pragma region button
    virtual void OnButtonHoverEnter(Button& button);
    virtual void OnButtonHoverLeave(Button& button);
    virtual void OnButtonPressed(Button& button);
    virtual void OnButtonReleased(Button& button);
    #pragma endregion

    #pragma region checkbox
    virtual void OnCheckboxHoverEnter(Checkbox& checkbox);
    virtual void OnCheckboxHoverLeave(Checkbox& checkbox);
    virtual void OnCheckboxToggled(Checkbox& checkbox, bool nowChecked);
    #pragma endregion

    #pragma region slider
    virtual void OnSliderDragStarted(Slider& slider);
    virtual void OnSliderDragStopped(Slider& slider);
    #pragma endregion

    #pragma region combobox
    virtual void OnComboBoxHoverEnter(ComboBox& combo);
    virtual void OnComboBoxHoverLeave(ComboBox& combo);
    virtual void OnComboBoxOpened(ComboBox& combo);
    virtual void OnComboBoxClosed(ComboBox& combo);
    virtual void OnComboBoxItemHoverEnter(AnimState& itemAnim);
    virtual void OnComboBoxItemHoverLeave(AnimState& itemAnim);
    virtual void OnComboBoxItemCheckChanged(AnimState& itemAnim, bool nowChecked);
    #pragma endregion

    #pragma region listbox
    virtual void OnListBoxHoverEnter(ListBox& listbox);
    virtual void OnListBoxHoverLeave(ListBox& listbox);
    virtual void OnListBoxItemHoverEnter(AnimState& itemAnim);
    virtual void OnListBoxItemHoverLeave(AnimState& itemAnim);
    virtual void OnListBoxItemCheckChanged(AnimState& itemAnim, bool nowChecked);
    #pragma endregion

    #pragma region textinput
    virtual void OnTextInputHoverEnter(TextInput& input);
    virtual void OnTextInputHoverLeave(TextInput& input);
    virtual void OnTextInputFocusEnter(TextInput& input);
    virtual void OnTextInputFocusLeave(TextInput& input);
    #pragma endregion

    #pragma region keybind
    virtual void OnKeyBindHoverEnter(KeyBind& keybind);
    virtual void OnKeyBindHoverLeave(KeyBind& keybind);
    virtual void OnKeyBindFocusEnter(KeyBind& keybind);
    virtual void OnKeyBindFocusLeave(KeyBind& keybind);
    virtual void OnKeyBindListeningStarted(KeyBind& keybind);
    virtual void OnKeyBindListeningStopped(KeyBind& keybind);
    #pragma endregion

    #pragma region colorswatch
    virtual void OnColorSwatchHoverEnter(ColorSwatch& swatch);
    virtual void OnColorSwatchHoverLeave(ColorSwatch& swatch);
    #pragma endregion

    #pragma region tabcontrol
    virtual void OnTabActivated(TabPage& tabPage);
    virtual void OnTabDeactivated(TabPage& tabPage);
    #pragma endregion

    #pragma region numericinput
    virtual void OnNumericInputHoverEnter(NumericInput& input);
    virtual void OnNumericInputHoverLeave(NumericInput& input);
    #pragma endregion
    #pragma endregion

    #pragma region drawing controls

    virtual void DrawWindowBackground(IRenderer* renderer, const Window& window, const Rect& absRect);
    virtual void DrawWindowTitleBar(IRenderer* renderer, const Window& window, const Rect& absRect);
    virtual void DrawButton(IRenderer* renderer, const Button& button, const Rect& absRect, bool hovered, bool pressed);
    virtual void DrawCheckbox(IRenderer* renderer, const Checkbox& checkbox, const Rect& absRect);
    virtual void DrawLabel(IRenderer* renderer, const Label& label, const Rect& absRect);
    virtual void DrawGroupBox(IRenderer* renderer, const GroupBox& groupBox, const Rect& absRect);
    virtual void DrawSlider(IRenderer* renderer, const Slider& slider, const Rect& absRect);
    virtual void DrawTabControl(IRenderer* renderer, const class TabControl& tabControl, const Rect& absRect);
    virtual void DrawTabPageBody(IRenderer* renderer, const TabPage& tabPage, const Rect& absRect);
    virtual void DrawSelectedTabButton(IRenderer* renderer, const TabPage& tabPage, const Rect& absRect);
    virtual void DrawUnselectedTabButton(IRenderer* renderer, const TabPage& tabPage, const Rect& absRect);
    virtual void DrawTabButton(IRenderer* renderer, const TabPage& tabPage, const Rect& absRect, bool active);
    virtual void DrawComboBox(IRenderer* renderer, const ComboBox& combo, const Rect& absRect);
    virtual void DrawComboBoxDropdown(IRenderer* renderer, const ComboBox& combo);
    virtual void DrawScrollbarTrack(IRenderer* renderer, const Rect& rect, bool vertical);
    virtual void DrawScrollbarThumb(IRenderer* renderer, const Rect& rect, bool vertical, bool hovered);
    virtual void DrawScrollbar(IRenderer* renderer, const Rect& trackRect, const Rect& thumbRect, bool vertical, bool thumbHovered);
    virtual void DrawColorPicker(IRenderer* renderer, const ColorPicker& picker, const Rect& absRect);
    void        DrawSVGradient(IRenderer* renderer, const Rect& rect, float hue, float cornerRadius = 0.f, uint8_t alpha = 255);
    void        DrawHueBarFill(IRenderer* renderer, const Rect& rect, bool vertical);
    void        DrawAlphaBarFill(IRenderer* renderer, const Rect& rect, const Color& currentColor, bool vertical, float checkSize = 4.f);
    virtual void DrawTextInput(IRenderer* renderer, const TextInputVisuals& visuals);

    virtual void DrawKeyBind(IRenderer* renderer, const KeyBind& keybind, const Rect& absRect);
    virtual void DrawListBox(IRenderer* renderer, const ListBox& listbox, const Rect& absRect);
    virtual void DrawColorSwatch(IRenderer* renderer, const ColorSwatch& swatch, const Rect& absRect);
    virtual void DrawColorPickerModal(IRenderer* renderer, ColorPickerModal& modal, const Rect& absRect);
    virtual void DrawSeparator(IRenderer* renderer, const Separator& separator, const Rect& absRect);
    virtual void DrawTooltip(IRenderer* renderer, const std::wstring& text, int mouseX, int mouseY);
    virtual void DrawNumericInput(IRenderer* renderer, const NumericInput& input, const Rect& absRect);
    #pragma endregion

    #pragma region drawing shared primitives
    virtual void DrawCheckMark(IRenderer* renderer, const Rect& boxRect,
        float checkT, Color markColor, float strokeWidth = 2.f);

    virtual void DrawComboBoxItem(IRenderer* renderer, const Rect& itemRect,
        const std::wstring& text, float hoverT, float checkT,
        bool isMultiSelect, float cornerRadius);

    virtual void DrawListBoxItem(IRenderer* renderer, const Rect& itemRect,
        const std::wstring& text, float hoverT, float checkT,
        bool isMultiSelect, float cornerRadius);
    #pragma endregion

protected:
    virtual std::unique_ptr<UIStyle> CreateDefaultStyle() const;

    std::unique_ptr<UIStyle> m_ownedStyle;
    UIStyle*                 m_style = nullptr;
    #pragma region default animation helpers
    void DefaultHoverIn(UIElement& el);
    void DefaultHoverOut(UIElement& el);
    void DefaultPressIn(UIElement& el);
    void DefaultPressOut(UIElement& el);
    void DefaultFocusIn(UIElement& el);
    void DefaultFocusOut(UIElement& el);
    void DefaultCheckChanged(AnimState& anim, bool nowChecked);
    void DefaultItemHoverIn(AnimState& anim);
    void DefaultItemHoverOut(AnimState& anim);
    #pragma endregion
};