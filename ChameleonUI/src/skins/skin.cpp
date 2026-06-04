#include "../../include/skins/skin.h"
#include "../../include/core/UIElement.h"
#include "../../include/elements/checkbox.h"
#include "../../include/elements/button.h"
#include "../../include/elements/combobox.h"
#include "../../include/elements/slider.h"
#include "../../include/elements/textinput.h"
#include "../../include/elements/keybind.h"
#include "../../include/elements/listbox.h"
#include "../../include/elements/tabpage.h"
#include "../../include/elements/colorswatch.h"
#include "../../include/elements/colorpickermodal.h"
#include "../../include/elements/separator.h"
#include "../../include/elements/numericinput.h"
#include "../../include/elements/colorpicker.h"

#pragma region style

Skin::Skin()
{
}

UIStyle* Skin::GetStyle()
{
    if (!m_style)
    {
        m_ownedStyle = CreateDefaultStyle();
        m_style = m_ownedStyle.get();
    }
    return m_style;
}
std::unique_ptr<UIStyle> Skin::CreateDefaultStyle() const
{
    return std::make_unique<UIStyle>();
}
#pragma endregion

#pragma region layout queries

Size Skin::GetContentSize(const UIElement& element, IRenderer* renderer) const
{
    if (auto* chk = dynamic_cast<const Checkbox*>(&element))
        return GetCheckboxContentSize(*chk);
    if (auto* btn = dynamic_cast<const Button*>(&element))
        return GetButtonContentSize(*btn);
    if (dynamic_cast<const ComboBox*>(&element))
        return { 0, GetComboBoxMinHeight() };
    return { 0, 0 };
}

Rect Skin::GetHitRect(const UIElement&, const Rect& absRect) const { return absRect; }
Size Skin::GetCheckboxContentSize(const Checkbox&) const { return { 0, 0 }; }
Size Skin::GetButtonContentSize(const Button&) const { return { 0, 0 }; }
float Skin::GetComboBoxMinHeight() const { return 0.f; }
#pragma endregion

#pragma region animation helpers

void Skin::OnNumericInputHoverEnter(NumericInput& input) { DefaultHoverIn(input); }
void Skin::OnNumericInputHoverLeave(NumericInput& input) { DefaultHoverOut(input); }

void Skin::DefaultHoverIn(UIElement& el)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    el.Anim().hover.easing = ac.hoverEasing;
    el.Anim().hover.TransitionTo(1.f, ac.hoverIn);
}

void Skin::DefaultHoverOut(UIElement& el)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    el.Anim().hover.easing = ac.hoverEasing;
    el.Anim().hover.TransitionTo(0.f, ac.hoverOut);
}

void Skin::DefaultPressIn(UIElement& el)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    el.Anim().press.easing = ac.pressEasing;
    el.Anim().press.TransitionTo(1.f, ac.pressIn);
}

void Skin::DefaultPressOut(UIElement& el)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    el.Anim().press.easing = ac.pressEasing;
    el.Anim().press.TransitionTo(0.f, ac.pressOut);
}

void Skin::DefaultFocusIn(UIElement& el)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    el.Anim().focus.easing = ac.hoverEasing;
    el.Anim().focus.TransitionTo(1.f, ac.hoverIn);
}

void Skin::DefaultFocusOut(UIElement& el)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    el.Anim().focus.easing = ac.hoverEasing;
    el.Anim().focus.TransitionTo(0.f, ac.hoverOut);
}

void Skin::DefaultCheckChanged(AnimState& anim, bool nowChecked)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    anim.checked.easing = ac.checkEasing;
    anim.checked.TransitionTo(nowChecked ? 1.f : 0.f, ac.checkToggle);
}

void Skin::DefaultItemHoverIn(AnimState& anim)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    anim.hover.easing = ac.hoverEasing;
    anim.hover.TransitionTo(1.f, ac.hoverIn);
}

void Skin::DefaultItemHoverOut(AnimState& anim)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    anim.hover.easing = ac.hoverEasing;
    anim.hover.TransitionTo(0.f, ac.hoverOut);
}
#pragma endregion

#pragma region animation events

void Skin::OnButtonHoverEnter(Button& button) { DefaultHoverIn(button); }
void Skin::OnButtonHoverLeave(Button& button) { DefaultHoverOut(button); DefaultPressOut(button); }
void Skin::OnButtonPressed(Button& button) { DefaultPressIn(button); }
void Skin::OnButtonReleased(Button& button) { DefaultPressOut(button); }

void Skin::OnCheckboxHoverEnter(Checkbox& checkbox) { DefaultHoverIn(checkbox); }
void Skin::OnCheckboxHoverLeave(Checkbox& checkbox) { DefaultHoverOut(checkbox); }
void Skin::OnCheckboxToggled(Checkbox& checkbox, bool nowChecked)
{
    DefaultCheckChanged(checkbox.Anim(), nowChecked);
}

void Skin::OnSliderDragStarted(Slider& slider) { DefaultPressIn(slider); }
void Skin::OnSliderDragStopped(Slider& slider) { DefaultPressOut(slider); }

void Skin::OnComboBoxHoverEnter(ComboBox& combo) { DefaultHoverIn(combo); }
void Skin::OnComboBoxHoverLeave(ComboBox& combo) { DefaultHoverOut(combo); }
void Skin::OnComboBoxOpened(ComboBox& combo)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    combo.Anim().opacity.easing = ac.fadeEasing;
    combo.Anim().opacity.TransitionTo(1.f, ac.fadeIn);
}
void Skin::OnComboBoxClosed(ComboBox& combo)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    combo.Anim().opacity.easing = ac.fadeEasing;
    combo.Anim().opacity.TransitionTo(0.f, ac.fadeOut);
}
void Skin::OnComboBoxItemHoverEnter(AnimState& ia) { DefaultItemHoverIn(ia); }
void Skin::OnComboBoxItemHoverLeave(AnimState& ia) { DefaultItemHoverOut(ia); }
void Skin::OnComboBoxItemCheckChanged(AnimState& ia, bool checked) { DefaultCheckChanged(ia, checked); }

void Skin::OnListBoxHoverEnter(ListBox& listbox) { DefaultHoverIn(listbox); }
void Skin::OnListBoxHoverLeave(ListBox& listbox) { DefaultHoverOut(listbox); }
void Skin::OnListBoxItemHoverEnter(AnimState& ia) { DefaultItemHoverIn(ia); }
void Skin::OnListBoxItemHoverLeave(AnimState& ia) { DefaultItemHoverOut(ia); }
void Skin::OnListBoxItemCheckChanged(AnimState& ia, bool checked) { DefaultCheckChanged(ia, checked); }

void Skin::OnTextInputHoverEnter(TextInput& input) { DefaultHoverIn(input); }
void Skin::OnTextInputHoverLeave(TextInput& input) { DefaultHoverOut(input); }
void Skin::OnTextInputFocusEnter(TextInput& input) { DefaultFocusIn(input); }
void Skin::OnTextInputFocusLeave(TextInput& input) { DefaultFocusOut(input); }

void Skin::OnKeyBindHoverEnter(KeyBind& keybind) { DefaultHoverIn(keybind); }
void Skin::OnKeyBindHoverLeave(KeyBind& keybind) { DefaultHoverOut(keybind); }
void Skin::OnKeyBindFocusEnter(KeyBind& keybind) { DefaultFocusIn(keybind); }
void Skin::OnKeyBindFocusLeave(KeyBind& keybind) { DefaultFocusOut(keybind); }
void Skin::OnKeyBindListeningStarted(KeyBind& keybind)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    keybind.Anim().glow.easing = ac.glowEasing;
    keybind.Anim().glow.TransitionTo(1.f, ac.glowIn);
}
void Skin::OnKeyBindListeningStopped(KeyBind& keybind)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    keybind.Anim().glow.easing = ac.glowEasing;
    keybind.Anim().glow.TransitionTo(0.f, ac.glowOut);
}

void Skin::OnColorSwatchHoverEnter(ColorSwatch& swatch) { DefaultHoverIn(swatch); }
void Skin::OnColorSwatchHoverLeave(ColorSwatch& swatch) { DefaultHoverOut(swatch); }

void Skin::OnTabActivated(TabPage& tabPage)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    tabPage.Anim().active.easing = ac.tabEasing;
    tabPage.Anim().active.TransitionTo(1.f, ac.tabSwitch);
}
void Skin::OnTabDeactivated(TabPage& tabPage)
{
    const AnimConfig& ac = m_style ? m_style->anim : AnimConfig{};
    tabPage.Anim().active.easing = ac.tabEasing;
    tabPage.Anim().active.TransitionTo(0.f, ac.tabSwitch);
}
#pragma endregion

#pragma region drawing

void Skin::DrawWindowBackground(IRenderer*, const Window&, const Rect&) {}
void Skin::DrawWindowTitleBar(IRenderer*, const Window&, const Rect&) {}
void Skin::DrawButton(IRenderer*, const Button&, const Rect&, bool, bool) {}
void Skin::DrawCheckbox(IRenderer*, const Checkbox&, const Rect&) {}
void Skin::DrawLabel(IRenderer*, const Label&, const Rect&) {}
void Skin::DrawGroupBox(IRenderer*, const GroupBox&, const Rect&) {}
void Skin::DrawSlider(IRenderer*, const Slider&, const Rect&) {}
void Skin::DrawTabControl(IRenderer*, const TabControl&, const Rect&) {}
void Skin::DrawTabPageBody(IRenderer*, const TabPage&, const Rect&) {}
void Skin::DrawSelectedTabButton(IRenderer*, const TabPage&, const Rect&) {}
void Skin::DrawUnselectedTabButton(IRenderer*, const TabPage&, const Rect&) {}
void Skin::DrawScrollbarTrack(IRenderer*, const Rect&, bool) {}
void Skin::DrawScrollbarThumb(IRenderer*, const Rect&, bool, bool) {}
void Skin::DrawScrollbar(IRenderer*, const Rect&, const Rect&, bool, bool) {}
void Skin::DrawComboBox(IRenderer*, const ComboBox&, const Rect&) {}
void Skin::DrawComboBoxDropdown(IRenderer*, const ComboBox&) {}
void Skin::DrawTextInput(IRenderer*, const TextInputVisuals&) {}
void Skin::DrawKeyBind(IRenderer*, const KeyBind&, const Rect&) {}
void Skin::DrawListBox(IRenderer*, const ListBox&, const Rect&) {}
void Skin::DrawColorPicker(IRenderer*, const ColorPicker&, const Rect& ){}

void Skin::DrawSVGradient(IRenderer* renderer, const Rect& rect, float hue, float cornerRadius, uint8_t alpha)
{
    if (alpha < 255)
    {
        float checkSize = 6.f;
        Color checkA(200, 200, 200, 255);
        Color checkB(120, 120, 120, 255);
        renderer->SetScissor(rect);
        for (float cy = rect.y; cy < rect.y + rect.h; cy += checkSize) {
            for (float cx = rect.x; cx < rect.x + rect.w; cx += checkSize) {
                int ix = (int)((cx - rect.x) / checkSize);
                int iy = (int)((cy - rect.y) / checkSize);

                float cw = (std::min)(checkSize, (rect.x + rect.w) - cx);
                float ch = (std::min)(checkSize, (rect.y + rect.h) - cy);

                renderer->DrawFilledRect(
                    Rect(cx, cy, cw, ch),
                    ((ix + iy) % 2 == 0) ? checkA : checkB);
            }
        }
        renderer->ResetScissor();
    }

    Color hueColor = ColorPickerModal::HSVToRGB(hue, 1.f, 1.f, alpha);
    Color white = Color(255, 255, 255, alpha);
    Color blackTransparent = Color(0, 0, 0, 0);
    Color blackOpaque = Color(0, 0, 0, alpha);

    renderer->DrawGradientRect(rect, white, hueColor, true, cornerRadius);
    renderer->DrawGradientRect(rect, blackTransparent, blackOpaque, false, cornerRadius);
}

void Skin::DrawHueBarFill(IRenderer* renderer, const Rect& rect, bool vertical)
{
    constexpr int stops = 6;
    if (vertical) {
        float bandH = rect.h / (float)stops;
        for (int i = 0; i < stops; i++) {
            Color c0 = ColorPickerModal::HSVToRGB((float)i / stops, 1.f, 1.f);
            Color c1 = ColorPickerModal::HSVToRGB((float)(i + 1) / stops, 1.f, 1.f);
            renderer->DrawGradientRect(
                Rect(rect.x, rect.y + i * bandH, rect.w, bandH + 1.f),
                c0, c1, false, 0.f);
        }
    }
    else {
        float bandW = rect.w / (float)stops;
        for (int i = 0; i < stops; i++) {
            Color c0 = ColorPickerModal::HSVToRGB((float)i / stops, 1.f, 1.f);
            Color c1 = ColorPickerModal::HSVToRGB((float)(i + 1) / stops, 1.f, 1.f);
            renderer->DrawGradientRect(
                Rect(rect.x + i * bandW, rect.y, bandW + 1.f, rect.h),
                c0, c1, true, 0.f);
        }
    }
}

void Skin::DrawAlphaBarFill(IRenderer* renderer, const Rect& rect, const Color& currentColor,
    bool vertical, float checkSize)
{
    Color checkA(200, 200, 200, 255);
    Color checkB(120, 120, 120, 255);

    Rect ambient = renderer->GetCurrentClip();
    float clipL = (std::max)(rect.x, ambient.x);
    float clipT = (std::max)(rect.y, ambient.y);
    float clipR = (std::min)(rect.x + rect.w, ambient.x + ambient.w);
    float clipB = (std::min)(rect.y + rect.h, ambient.y + ambient.h);
    Rect drawRect(clipL, clipT,
        (clipR > clipL) ? (clipR - clipL) : 0.f,
        (clipB > clipT) ? (clipB - clipT) : 0.f);

    if (drawRect.w <= 0.f || drawRect.h <= 0.f)
        return;

    renderer->SetScissor(drawRect);
    for (float cy = rect.y; cy < rect.y + rect.h; cy += checkSize) {
        for (float cx = rect.x; cx < rect.x + rect.w; cx += checkSize) {
            int ix = (int)((cx - rect.x) / checkSize);
            int iy = (int)((cy - rect.y) / checkSize);

            float cellR = (std::min)(cx + checkSize, drawRect.x + drawRect.w);
            float cellB = (std::min)(cy + checkSize, drawRect.y + drawRect.h);
            float cellL = (std::max)(cx, drawRect.x);
            float cellT = (std::max)(cy, drawRect.y);
            if (cellR <= cellL || cellB <= cellT)
                continue;

            renderer->DrawFilledRect(
                Rect(cellL, cellT, cellR - cellL, cellB - cellT),
                ((ix + iy) % 2 == 0) ? checkA : checkB);
        }
    }

    Color solid = currentColor.WithAlpha(255);
    Color clear = currentColor.WithAlpha(0);
    renderer->DrawGradientRect(rect, solid, clear, !vertical, 0.f);

    renderer->ResetScissor();
}

void Skin::DrawColorSwatch(IRenderer* renderer, const ColorSwatch& swatch, const Rect& absRect)
{
    renderer->DrawFilledRect(absRect, Color(200, 200, 200, 255), 3.f);
    renderer->DrawFilledRect(absRect, swatch.GetColor(), 3.f);
    renderer->DrawOutlineRect(absRect, Color(255, 255, 255, 60), 1.f, 3.f);
}

void Skin::DrawColorPickerModal(IRenderer* renderer, ColorPickerModal& modal, const Rect& absRect)
{
    renderer->DrawFilledRect(absRect, Color(30, 30, 40, 240), 8.f);
    renderer->DrawOutlineRect(absRect, Color(255, 255, 255, 30), 1.f, 8.f);
}

void Skin::DrawTabButton(IRenderer* renderer, const TabPage& tabPage, const Rect& absRect, bool active)
{
    if (active) DrawSelectedTabButton(renderer, tabPage, absRect);
    else DrawUnselectedTabButton(renderer, tabPage, absRect);
}

void Skin::DrawCheckMark(IRenderer* renderer, const Rect& boxRect,
    float checkT, Color markColor, float strokeWidth)
{
    if (checkT <= 0.01f) return;
    float cx = boxRect.x + boxRect.w * 0.5f;
    float cy = boxRect.y + boxRect.h * 0.5f;
    float scale = boxRect.w / 18.f;
    float s = checkT;
    uint8_t alpha = (uint8_t)((float)markColor.a * checkT);
    Color mark(markColor.r, markColor.g, markColor.b, alpha);
    renderer->DrawLine(cx - 4.f * s * scale, cy, cx - 1.f * s * scale, cy + 3.f * s * scale, mark, strokeWidth);
    renderer->DrawLine(cx - 1.f * s * scale, cy + 3.f * s * scale, cx + 4.f * s * scale, cy - 3.f * s * scale, mark, strokeWidth);
}

void Skin::DrawSeparator(IRenderer* renderer, const Separator& separator, const Rect& absRect)
{
    Color color = m_style ? m_style->separatorColor : Color(255, 255, 255, 30);

    if (separator.GetOrientation() == Separator::Orientation::Horizontal)
    {
        float cy = absRect.y + absRect.h * 0.5f;
        renderer->DrawLine(absRect.x, cy, absRect.x + absRect.w, cy, color, absRect.h);
    }
    else
    {
        float cx = absRect.x + absRect.w * 0.5f;
        renderer->DrawLine(cx, absRect.y, cx, absRect.y + absRect.h, color, absRect.w);
    }
}

void Skin::DrawTooltip(IRenderer* renderer, const std::wstring& text, int mouseX, int mouseY)
{
    if (text.empty()) return;

    int fontSize = m_style ? m_style->tooltipFontSize : 12;
    const wchar_t* font = m_style ? m_style->fontFamily.c_str() : L"Segoe UI";
    float pad = m_style ? m_style->tooltipPadding : 6.f;
    float cr = m_style ? m_style->tooltipCornerRadius : 4.f;
    Color bg = m_style ? m_style->tooltipBackground : Color(30, 30, 40, 240);
    Color border = m_style ? m_style->tooltipBorder : Color(255, 255, 255, 40);
    Color textColor = m_style ? m_style->tooltipText : Color(230, 235, 245, 255);

    Rect textSize = renderer->MeasureText(text, fontSize, font);

    float tipW = textSize.w + pad * 2.f;
    float tipH = textSize.h + pad * 2.f;

    float tipX = (float)mouseX + 16.f;
    float tipY = (float)mouseY + 16.f;

    Rect bgRect(tipX, tipY, tipW, tipH);

    renderer->DrawFilledRect(bgRect, bg, cr);
    renderer->DrawOutlineRect(bgRect, border, 1.f, cr);

    Rect textRect(tipX + pad, tipY + pad, textSize.w, textSize.h);
    renderer->DrawText(text, textRect, textColor, fontSize, TextAnchor::TopLeft, font);
}

void Skin::DrawNumericInput(IRenderer* renderer, const NumericInput& input, const Rect& absRect)
{
    auto g = input.ComputeGeometry();
    float cr = m_style ? m_style->cornerRadius : 6.f;
    int fontSize = m_style ? m_style->fontSize : 13;
    const wchar_t* font = m_style ? m_style->fontFamily.c_str() : L"Segoe UI";
    Color bg = m_style ? (input.IsHovered() ? m_style->numericHovered : m_style->numericBackground)
        : Color(255, 255, 255, 12);
    Color border = m_style ? m_style->numericBorder : Color(255, 255, 255, 20);
    Color textCol = m_style ? m_style->numericText : Color(230, 235, 245, 255);

    renderer->DrawFilledRect(absRect, bg, cr);
    renderer->DrawOutlineRect(absRect, border, 1.f, cr);


    auto drawButton = [&](const Rect& rect, const std::wstring& symbol, bool hovered, bool pressed) {
        Color btnBg = m_style ? m_style->numericButtonNormal : Color(255, 255, 255, 15);
        if (pressed)       btnBg = m_style ? m_style->numericButtonPressed : Color(255, 255, 255, 8);
        else if (hovered)  btnBg = m_style ? m_style->numericButtonHovered : Color(255, 255, 255, 30);
        Color btnText = m_style ? m_style->numericButtonText : Color(200, 210, 230, 255);
        renderer->DrawFilledRect(rect, btnBg, cr);
        renderer->DrawText(symbol, rect, btnText, fontSize, TextAnchor::Center, font);
        };

    drawButton(g.decrementRect, L"\u2212", input.IsDecrementHovered(), input.IsDecrementPressed());
    drawButton(g.incrementRect, L"+", input.IsIncrementHovered(), input.IsIncrementPressed());

    std::wstring display = input.IsEditing() ? input.GetEditBuffer() + L"|" : input.GetDisplayText();
    renderer->DrawText(display, g.fieldRect, textCol, fontSize, TextAnchor::Center, font);
}

void Skin::DrawComboBoxItem(IRenderer*, const Rect&, const std::wstring&, float, float, bool, float) {}
void Skin::DrawListBoxItem(IRenderer*, const Rect&, const std::wstring&, float, float, bool, float) {}
#pragma endregion