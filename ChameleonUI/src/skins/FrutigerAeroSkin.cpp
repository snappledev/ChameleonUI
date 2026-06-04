#include "../../include/skins/FrutigerAeroSkin.h"
#include "../../include/core/UIElement.h"
#include "../../include/elements/Window.h"
#include "../../include/elements/button.h"
#include "../../include/elements/checkbox.h"
#include "../../include/elements/label.h"
#include "../../include/elements/tabcontrol.h"
#include "../../include/elements/tabpage.h"
#include "../../include/elements/slider.h"
#include "../../include/elements/groupbox.h"
#include "../../include/elements/combobox.h"
#include "../../include/elements/textinput.h"
#include "../../include/elements/keybind.h"
#include "../../include/elements/listbox.h"
#include "../../include/elements/colorswatch.h"
#include "../../include/elements/numericinput.h"
#include "../../include/elements/colorpicker.h"
#include "../../include/elements/colorpickermodal.h"
#include <cstdio>

#pragma region style
std::unique_ptr<UIStyle> FrutigerAeroSkin::CreateDefaultStyle() const
{
    auto s = std::make_unique<UIStyle>();

    s->cornerRadius = 6.f;
    s->glassBlurRadius = 0.f;
    s->glassBorderAlpha = 0.f;
    s->shadowColor = Color(0, 20, 60, 60);

    s->accent = Color(50, 150, 235, 255);
    s->accentHover = Color(85, 175, 250, 255);
    s->accentPressed = Color(25, 115, 200, 255);

    // window
    s->windowBackground = Color(220, 235, 250, 255);
    s->titleBarBackground = Color(60, 130, 210, 255);
    s->titleBarText = Color(255, 255, 255, 255);
    s->titleBarHeight = 32;

    // button
    s->buttonNormal = Color(235, 245, 255, 255);
    s->buttonHovered = Color(220, 238, 255, 255);
    s->buttonPressed = Color(195, 218, 240, 255);
    s->buttonText = Color(20, 50, 120, 255);
    s->buttonBorder = Color(90, 140, 195, 255);
    s->buttonMinHeight = 32.f;

    // groupbox
    s->groupboxTitleBackground = Color(70, 140, 215, 255);
    s->groupboxTitleOutline = Color(50, 120, 195, 255);
    s->groupboxBodyBackground = Color(238, 244, 252, 255);
    s->groupboxTitleText = Color(255, 255, 255, 255);

    // checkbox
    s->checkboxBoxNormal = Color(240, 245, 252, 255);
    s->checkboxBoxChecked = Color(50, 170, 95, 255);
    s->checkboxText = Color(25, 60, 130, 255);
    s->checkboxCheckMark = Color(255, 255, 255, 255);
    s->checkboxBoxSize = 22.f;
    s->checkboxCornerRadius = 11.f;
    s->checkboxLabelSpacing = 8.f;

    // label
    s->labelText = Color(25, 60, 130, 255);

    // tab
    s->tabNormalBackground = Color(210, 225, 242, 255);
    s->tabSelectedBackground = Color(238, 244, 252, 255);
    s->tabHoveredBackground = Color(225, 238, 252, 255);
    s->tabText = Color(70, 100, 150, 255);
    s->tabSelectedText = Color(15, 55, 130, 255);
    s->tabControlBackground = Color(205, 220, 240, 255);

    // slider
    s->sliderTrack = Color(190, 205, 225, 255);
    s->sliderFill = Color(50, 150, 235, 255);
    s->sliderThumb = Color(240, 245, 252, 255);
    s->sliderTrackHeight = 8.f;
    s->sliderThumbRadius = 10.f;

    // combobox
    s->comboBackground = Color(240, 246, 255, 255);
    s->comboHovered = Color(225, 238, 255, 255);
    s->comboBorder = Color(90, 140, 195, 255);
    s->comboText = Color(20, 55, 120, 255);
    s->comboArrow = Color(50, 90, 150, 255);
    s->comboDropBackground = Color(255, 255, 255, 255);
    s->comboDropBorder = Color(90, 140, 195, 255);
    s->comboItemNormal = Color(0, 0, 0, 0);
    s->comboItemHovered = Color(200, 225, 252, 255);
    s->comboItemSelected = Color(50, 150, 235, 40);
    s->comboItemText = Color(25, 60, 130, 255);
    s->comboItemTextSelected = Color(15, 45, 110, 255);
    s->comboCheckMark = Color(50, 170, 95, 255);
    s->comboArrowWidth = 30.f;
    s->comboMinHeight = 30.f;

    // text input
    s->inputBackground = Color(255, 255, 255, 255);
    s->inputHovered = Color(255, 255, 255, 255);
    s->inputFocused = Color(255, 255, 255, 255);
    s->inputBorder = Color(140, 170, 205, 255);
    s->inputBorderFocused = Color(50, 150, 235, 255);
    s->inputText = Color(15, 30, 70, 255);
    s->inputPlaceholder = Color(120, 150, 190, 180);
    s->inputSelection = Color(50, 150, 235, 100);
    s->inputCursor = Color(10, 20, 50, 255);
    s->inputPadX = 8.f;
    s->inputMinHeight = 30.f;

    // keybind
    s->keybindBackground = Color(240, 245, 252, 255);
    s->keybindHovered = Color(230, 240, 255, 255);
    s->keybindBorder = Color(100, 150, 200, 255);
    s->keybindListening = Color(50, 150, 235, 60);
    s->keybindText = Color(20, 55, 120, 255);
    s->keybindTextListening = Color(40, 120, 210, 255);
    s->keybindMinHeight = 32.f;

    // scrollbar
    s->scrollbarTrack = Color(215, 228, 242, 255);
    s->scrollbarThumb = Color(170, 195, 220, 255);
    s->scrollbarThumbHover = Color(140, 175, 215, 255);
    s->scrollbarWidth = 14;
    s->scrollbarMinThumbHeight = 28;

    // listbox
    s->listboxBackground = Color(255, 255, 255, 255);
    s->listboxBorder = Color(140, 170, 205, 255);
    s->listboxItemNormal = Color(0, 0, 0, 0);
    s->listboxItemHovered = Color(200, 225, 252, 255);
    s->listboxItemSelected = Color(50, 150, 235, 50);
    s->listboxItemText = Color(25, 60, 130, 255);
    s->listboxItemTextSelected = Color(15, 45, 110, 255);

    // color picker
    s->colorSwatchBorder = Color(120, 160, 200, 255);
    s->colorSwatchHoverBorder = Color(50, 150, 235, 255);
    s->colorSwatchCornerRadius = 11.f;
    s->colorSwatchDefaultWidth = 22.f;
    s->colorSwatchDefaultHeight = 22.f;
    s->colorPickerBackground = Color(240, 246, 255, 255);
    s->colorPickerBorder = Color(90, 140, 195, 255);
    s->colorPickerButtonNormal = Color(235, 245, 255, 255);
    s->colorPickerButtonHovered = Color(220, 238, 255, 255);
    s->colorPickerButtonText = Color(20, 55, 120, 255);
    s->colorPickerLabelText = Color(30, 70, 140, 255);
    s->colorPickerCornerRadius = 6.f;
    s->colorPickerWidth = 310.f;
    s->colorPickerHeight = 270.f;

    s->fontFamily = L"Segoe UI";
    s->fontSize = 13;

    // separator
    s->separatorColor = Color(140, 170, 205, 100);

    // numeric input
    s->numericBackground = Color(30, 50, 70, 200);
    s->numericHovered = Color(40, 60, 85, 210);
    s->numericBorder = Color(90, 140, 195, 255);
    s->numericText = Color(230, 240, 255, 255);
    s->numericButtonNormal = Color(60, 95, 140, 255);
    s->numericButtonHovered = Color(80, 120, 170, 255);
    s->numericButtonPressed = Color(40, 60, 80, 255);
    s->numericButtonText = Color(240, 245, 255, 255);
    s->numericMinHeight = 28.f;

    // animation
    s->anim.hoverIn = 0.10f;
    s->anim.hoverOut = 0.18f;
    s->anim.pressIn = 0.04f;
    s->anim.pressOut = 0.20f;
    s->anim.checkToggle = 0.28f;
    s->anim.tabSwitch = 0.22f;
    s->anim.fadeIn = 0.16f;
    s->anim.fadeOut = 0.12f;
    s->anim.glowIn = 0.12f;
    s->anim.glowOut = 0.45f;
    s->anim.hoverEasing = Easing::CubicOut;
    s->anim.pressEasing = Easing::QuadOut;
    s->anim.checkEasing = Easing::BackOut;
    s->anim.glowEasing = Easing::ExpoOut;

    return s;
}
#pragma endregion

#pragma region structural helpers
void FrutigerAeroSkin::DrawBevelRaised(IRenderer* renderer, const Rect& rect, float cr, uint8_t strength)
{
    renderer->DrawLine(rect.x + cr, rect.y, rect.x + rect.w - cr, rect.y,
        Color(255, 255, 255, strength), 1.f);
    renderer->DrawLine(rect.x, rect.y + cr, rect.x, rect.y + rect.h - cr,
        Color(255, 255, 255, (uint8_t)(strength * 0.8f)), 1.f);
    renderer->DrawLine(rect.x + cr, rect.y + rect.h, rect.x + rect.w - cr, rect.y + rect.h,
        Color(0, 0, 0, (uint8_t)(strength * 0.5f)), 1.f);
    renderer->DrawLine(rect.x + rect.w, rect.y + cr, rect.x + rect.w, rect.y + rect.h - cr,
        Color(0, 0, 0, (uint8_t)(strength * 0.4f)), 1.f);
}

void FrutigerAeroSkin::DrawBevelSunken(IRenderer* renderer, const Rect& rect, float cr, uint8_t strength)
{
    renderer->DrawLine(rect.x + cr, rect.y + 1.f, rect.x + rect.w - cr, rect.y + 1.f,
        Color(0, 0, 0, strength), 1.f);
    renderer->DrawLine(rect.x + 1.f, rect.y + cr, rect.x + 1.f, rect.y + rect.h - cr,
        Color(0, 0, 0, (uint8_t)(strength * 0.7f)), 1.f);
    renderer->DrawLine(rect.x + cr, rect.y + rect.h - 1.f, rect.x + rect.w - cr, rect.y + rect.h - 1.f,
        Color(255, 255, 255, (uint8_t)(strength * 0.8f)), 1.f);
    renderer->DrawLine(rect.x + rect.w - 1.f, rect.y + cr, rect.x + rect.w - 1.f, rect.y + rect.h - cr,
        Color(255, 255, 255, (uint8_t)(strength * 0.6f)), 1.f);
}

void FrutigerAeroSkin::DrawGripLines(IRenderer* renderer, float cx, float cy,
    float width, int count, Color light, Color dark)
{
    float totalH = (float)(count * 3 - 1);
    float startY = cy - totalH * 0.5f;
    float x0 = cx - width * 0.5f;
    float x1 = cx + width * 0.5f;
    for (int i = 0; i < count; i++) {
        float y = startY + i * 3.f;
        renderer->DrawLine(x0, y, x1, y, dark, 1.f);
        renderer->DrawLine(x0, y + 1.f, x1, y + 1.f, light, 1.f);
    }
}

void FrutigerAeroSkin::DrawGloss(IRenderer* renderer, const Rect& rect, float cr, uint8_t alpha)
{
    Rect top(rect.x + 1.f, rect.y + 1.f, rect.w - 2.f, rect.h * 0.42f);
    renderer->DrawGradientRect(top, Color(255, 255, 255, alpha), Color(255, 255, 255, 0), false, cr);
}
#pragma endregion

#pragma region animation events
void FrutigerAeroSkin::OnButtonPressed(Button& button)
{
    if (!m_style) return;
    button.Anim().press.easing = Easing::QuadIn;
    button.Anim().press.TransitionTo(1.f, 0.04f);
    button.Anim().glow.easing = Easing::ExpoOut;
    button.Anim().glow.TransitionTo(1.f, 0.12f);
}

void FrutigerAeroSkin::OnButtonReleased(Button& button)
{
    if (!m_style) return;
    button.Anim().press.easing = Easing::BackOut;
    button.Anim().press.TransitionTo(0.f, 0.25f);
    button.Anim().glow.easing = Easing::ExpoOut;
    button.Anim().glow.TransitionTo(0.f, 0.40f);
}

void FrutigerAeroSkin::OnCheckboxToggled(Checkbox& checkbox, bool nowChecked)
{
    if (!m_style) return;
    checkbox.Anim().checked.easing = Easing::BackOut;
    checkbox.Anim().checked.TransitionTo(nowChecked ? 1.f : 0.f, 0.30f);
    checkbox.Anim().glow.easing = Easing::ExpoOut;
    checkbox.Anim().glow.Set(1.f);
    checkbox.Anim().glow.TransitionTo(0.f, 0.5f);
}
#pragma endregion

#pragma region window
void FrutigerAeroSkin::DrawWindowBackground(IRenderer* renderer, const Window& window,
    const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius;
    Color bg = m_style->windowBackground;

    renderer->DrawDropShadow(absRect, cr, m_style->shadowColor, 0.f, 3.f, 12.f);

    GradientStop grad[] = {
        { 0.0f, bg.Brighten(0.15f) },
        { 0.3f, bg },
        { 1.0f, bg.Darken(0.10f) }
    };
    renderer->DrawMultiGradientRect(absRect, grad, 3, false, cr);

    renderer->DrawOutlineRect(absRect, bg.Darken(0.35f), 1.f, cr);
    DrawBevelRaised(renderer, absRect, cr, 80);
}

void FrutigerAeroSkin::DrawWindowTitleBar(IRenderer* renderer, const Window& window,
    const Rect& absRect)
{
    if (!m_style) return;
    if (!window.ShowTitleBar()) return;
    float cr = m_style->cornerRadius;
    Color base = m_style->titleBarBackground;

    GradientStop bar[] = {
        { 0.0f,  base.Brighten(0.25f) },
        { 0.45f, base },
        { 0.50f, base.Darken(0.12f) },
        { 1.0f,  base.Darken(0.30f) }
    };
    renderer->DrawMultiGradientRect(absRect, bar, 4, false, cr);

    DrawGloss(renderer, absRect, cr, 70);
    DrawBevelRaised(renderer, absRect, cr, 50);

    float sepY = absRect.y + absRect.h - 1.f;
    renderer->DrawLine(absRect.x + 4.f, sepY, absRect.x + absRect.w - 4.f, sepY,
        base.Darken(0.8f).WithAlpha(40), 1.f);

    // embossed text
    Rect textDown(absRect.x, absRect.y + 1.f, absRect.w, absRect.h);
    renderer->DrawText(window.GetTitle(), textDown, base.Darken(0.7f).WithAlpha(100),
        m_style->fontSize + 1, TextAnchor::Center, m_style->fontFamily.c_str());
    Rect textUp(absRect.x, absRect.y - 1.f, absRect.w, absRect.h);
    renderer->DrawText(window.GetTitle(), textUp, m_style->titleBarText.WithAlpha(50),
        m_style->fontSize + 1, TextAnchor::Center, m_style->fontFamily.c_str());
    renderer->DrawText(window.GetTitle(), absRect, m_style->titleBarText,
        m_style->fontSize + 1, TextAnchor::Center, m_style->fontFamily.c_str());
}
#pragma endregion

#pragma region button
void FrutigerAeroSkin::DrawButton(IRenderer* renderer, const Button& button,
    const Rect& absRect, bool hovered, bool pressed)
{
    if (!m_style) return;
    const AnimState& a = button.Anim();
    float pressT = a.press.Value();
    float hoverT = a.hover.Value();
    float cr = absRect.h * 0.4f;

    Color base = m_style->buttonNormal;
    Color pBase = m_style->buttonPressed;

    Color topA = Color::Lerp(base.Brighten(0.08f), pBase.Darken(0.10f), pressT);
    Color midA = Color::Lerp(base, pBase, pressT);
    Color midB = Color::Lerp(base.Darken(0.10f), pBase.Brighten(0.08f), pressT);
    Color botA = Color::Lerp(base.Darken(0.18f), pBase.Brighten(0.15f), pressT);

    topA = Color::Lerp(topA, m_style->buttonHovered, hoverT * 0.3f);

    GradientStop grad[] = {
        { 0.0f, topA }, { 0.46f, midA }, { 0.54f, midB }, { 1.0f, botA }
    };
    renderer->DrawMultiGradientRect(absRect, grad, 4, false, cr);

    Color border = Color::Lerp(m_style->buttonBorder, m_style->accentHover, hoverT * 0.5f);
    renderer->DrawOutlineRect(absRect, border, 1.f, cr);

    if (pressT < 0.5f)
        DrawBevelRaised(renderer, absRect, cr, (uint8_t)(50 * (1.f - pressT * 2.f)));
    else
        DrawBevelSunken(renderer, absRect, cr, (uint8_t)(30 * (pressT * 2.f - 1.f)));

    if (a.glow.Value() > 0.01f) {
        uint8_t ga = (uint8_t)(180.f * a.glow.Value());
        float barW = absRect.w * 0.6f;
        Rect glowRect(absRect.x + (absRect.w - barW) * 0.5f,
            absRect.y + absRect.h - 2.f, barW, 2.f);
        renderer->DrawFilledRect(glowRect, m_style->accent.WithAlpha(ga), 1.f);
    }

    Rect cp = button.GetContentPadding();
    Rect textRect(absRect.x + cp.x, absRect.y + cp.y,
        absRect.w - cp.x - cp.w, absRect.h - cp.y - cp.h);
    renderer->DrawText(button.GetText(), textRect, m_style->buttonText,
        m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
}
#pragma endregion

#pragma region checkbox
void FrutigerAeroSkin::DrawCheckbox(IRenderer* renderer, const Checkbox& checkbox,
    const Rect& absRect)
{
    if (!m_style) return;
    const AnimState& a = checkbox.Anim();
    float checkT = a.checked.Value();
    float hoverT = a.hover.Value();

    float cx = absRect.x + absRect.w * 0.5f;
    float cy = absRect.y + absRect.h * 0.5f;
    float outerR = absRect.w * 0.5f;

    Color boxBase = m_style->checkboxBoxNormal;
    renderer->DrawGradientCircle(cx, cy, outerR, boxBase, boxBase.Darken(0.10f));
    renderer->DrawCircleOutline(cx, cy, outerR, boxBase.Darken(0.35f), 1.5f);
    renderer->DrawCircleOutline(cx, cy, outerR - 1.5f, boxBase.Darken(0.80f).WithAlpha(25), 1.f);

    if (checkT > 0.01f) {
        float innerR = (outerR - 4.f) * checkT;
        renderer->DrawCircle(cx, cy, innerR, m_style->checkboxBoxChecked);
        if (innerR > 3.f) {
            renderer->DrawCircle(cx, cy - innerR * 0.25f, innerR * 0.5f,
                m_style->checkboxBoxChecked.Brighten(0.6f).WithAlpha((uint8_t)(60.f * checkT)));
        }
    }

    if (hoverT > 0.01f) {
        renderer->DrawCircleOutline(cx, cy, outerR + 2.f,
            m_style->accent.WithAlpha((uint8_t)(40.f * hoverT)), 2.f);
    }

    if (a.glow.Value() > 0.01f) {
        float pulseR = outerR + 5.f * a.glow.Value();
        uint8_t pa = (uint8_t)(50.f * a.glow.Value());
        Color pc = checkT > 0.5f ? m_style->checkboxBoxChecked.WithAlpha(pa) : m_style->accent.WithAlpha(pa);
        renderer->DrawCircle(cx, cy, pulseR, pc);
    }
}
#pragma endregion

#pragma region checkmark
void FrutigerAeroSkin::DrawCheckMark(IRenderer* renderer, const Rect& boxRect,
    float checkT, Color markColor, float strokeWidth)
{
    if (checkT <= 0.01f) return;
    float cx = boxRect.x + boxRect.w * 0.5f;
    float cy = boxRect.y + boxRect.h * 0.5f;
    float scale = boxRect.w / 18.f;
    float s = checkT;
    uint8_t alpha = (uint8_t)((float)markColor.a * checkT);
    Color mark(markColor.r, markColor.g, markColor.b, alpha);

    renderer->DrawLine(cx - 4.5f * s * scale, cy + 0.5f,
        cx - 1.f * s * scale, cy + 3.5f * s * scale, mark, strokeWidth + 0.5f);
    renderer->DrawLine(cx - 1.f * s * scale, cy + 3.5f * s * scale,
        cx + 5.f * s * scale, cy - 3.5f * s * scale, mark, strokeWidth + 0.5f);
}
#pragma endregion

#pragma region label
void FrutigerAeroSkin::DrawLabel(IRenderer* renderer, const Label& label, const Rect& absRect)
{
    if (!m_style) return;
    // emboss highlight
    Rect shadow(absRect.x, absRect.y + 1.f, absRect.w, absRect.h);
    renderer->DrawText(label.GetText(), shadow,
        m_style->windowBackground.Brighten(0.30f).WithAlpha(80),
        m_style->fontSize, TextAnchor::CenterLeft, m_style->fontFamily.c_str());
    renderer->DrawText(label.GetText(), absRect, m_style->labelText,
        m_style->fontSize, TextAnchor::CenterLeft, m_style->fontFamily.c_str());
}
#pragma endregion

#pragma region groupbox
void FrutigerAeroSkin::DrawGroupBox(IRenderer* renderer, const GroupBox& groupBox,
    const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius;
    float headerH = 26.f;
    Color bodyBg = m_style->groupboxBodyBackground;
    Color bodyBorder = bodyBg.Darken(0.15f);

    if (groupBox.ShowTitleBar()) {
        Rect bodyRect(absRect.x, absRect.y + headerH - 2.f,
            absRect.w, absRect.h - headerH + 2.f);
        renderer->DrawFilledRect(bodyRect, bodyBg, cr * 0.5f);
        renderer->DrawOutlineRect(bodyRect, bodyBorder, 1.f, cr * 0.5f);
        DrawBevelSunken(renderer, bodyRect, cr * 0.5f, 30);

        Color hBase = m_style->groupboxTitleBackground;
        Rect headerRect(absRect.x, absRect.y, absRect.w, headerH);
        GradientStop hdr[] = {
            { 0.0f,  hBase.Brighten(0.22f) },
            { 0.45f, hBase },
            { 0.55f, hBase.Darken(0.10f) },
            { 1.0f,  hBase.Darken(0.22f) }
        };
        renderer->DrawMultiGradientRect(headerRect, hdr, 4, false, cr);
        DrawGloss(renderer, headerRect, cr, 60);
        DrawBevelRaised(renderer, headerRect, cr, 40);

        // embossed title text
        Rect tShadow(headerRect.x, headerRect.y + 1.f, headerRect.w, headerRect.h);
        renderer->DrawText(groupBox.GetTitle(), tShadow,
            hBase.Darken(0.6f).WithAlpha(80),
            m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
        renderer->DrawText(groupBox.GetTitle(), headerRect, m_style->groupboxTitleText,
            m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
    }
    else {
        renderer->DrawFilledRect(absRect, bodyBg, cr * 0.5f);
        renderer->DrawOutlineRect(absRect, bodyBorder, 1.f, cr * 0.5f);
        DrawBevelSunken(renderer, absRect, cr * 0.5f, 30);
    }
}
#pragma endregion

#pragma region slider

void FrutigerAeroSkin::DrawSlider(IRenderer* renderer, const Slider& slider,
    const Rect& absRect)
{
    if (!m_style) return;
    float trackH = m_style->sliderTrackHeight;
    float thumbR = m_style->sliderThumbRadius;

    Slider::SliderGeometry g = slider.ComputeGeometry(trackH, thumbR);
    const AnimState& a = slider.Anim();

    renderer->DrawFilledRect(g.trackRect, m_style->sliderTrack, trackH * 0.5f);
    DrawBevelSunken(renderer, g.trackRect, trackH * 0.5f, 50);

    if (g.fillRect.w > 0.f) {
        Color fb = m_style->sliderFill;
        GradientStop fill[] = {
            { 0.0f,  fb.Brighten(0.35f) },
            { 0.48f, fb },
            { 0.52f, fb.Darken(0.10f) },
            { 1.0f,  fb.Darken(0.25f) }
        };
        renderer->DrawMultiGradientRect(g.fillRect, fill, 4, false, trackH * 0.5f);
    }

    float thumbW = thumbR * 1.4f;
    float thumbH = absRect.h;
    float thumbX = g.thumbCX - thumbW * 0.5f;
    float thumbY = absRect.y;
    float thumbCR = 3.f;
    Rect thumbRect(thumbX, thumbY, thumbW, thumbH);

    Color tb = m_style->sliderThumb;
    GradientStop thumb[] = {
        { 0.0f,  tb.Brighten(0.06f) },
        { 0.45f, tb },
        { 0.55f, tb.Darken(0.10f) },
        { 1.0f,  tb.Darken(0.18f) }
    };
    renderer->DrawMultiGradientRect(thumbRect, thumb, 4, false, thumbCR);
    renderer->DrawOutlineRect(thumbRect, tb.Darken(0.35f), 1.f, thumbCR);
    DrawBevelRaised(renderer, thumbRect, thumbCR, 50);

    float gripCX = thumbX + thumbW * 0.5f;
    float gripCY = thumbY + thumbH * 0.5f;
    DrawGripLines(renderer, gripCX, gripCY, thumbW * 0.5f, 3,
        tb.Brighten(0.5f).WithAlpha(120), tb.Darken(0.5f).WithAlpha(30));

    if (a.hover.Value() > 0.01f) {
        uint8_t ha = (uint8_t)(35.f * a.hover.Value());
        Rect hoverRect(thumbX - 2.f, thumbY - 1.f, thumbW + 4.f, thumbH + 2.f);
        renderer->DrawOutlineRect(hoverRect,
            m_style->accent.WithAlpha(ha), 2.f, thumbCR + 1.f);
    }

    float pressT = a.press.Value();
    if (pressT > 0.01f) {
        std::wstring valStr = slider.GetFormattedValue();
        float fontSize = (float)(m_style->fontSize - 1);
        std::wstring fontFamily = m_style->fontFamily;
        float tcx = g.thumbCX, tcy = thumbY;
        Color tipBg = m_style->accent;
        Color tipText = m_style->titleBarText;
        Color tipBorder = m_style->accent.Darken(0.25f);

        UIElement::QueueOverlayDraw(
            [valStr, fontSize, fontFamily, tcx, tcy, pressT, tipBg, tipText, tipBorder](IRenderer* r) {
                Rect ts = r->MeasureText(valStr, (int)fontSize, fontFamily.c_str());
                float pw = ts.w + 14.f, ph = ts.h + 8.f;
                float px = tcx - pw * 0.5f, py = tcy - ph - 4.f;
                float pcr = 4.f;
                uint8_t bgA = (uint8_t)(255.f * pressT);
                uint8_t tA = (uint8_t)(255.f * pressT);

                r->DrawFilledRect(Rect(px, py, pw, ph), tipBg.WithAlpha(bgA), pcr);
                r->DrawGradientRect(Rect(px, py, pw, ph * 0.4f),
                    tipBg.Brighten(0.4f).WithAlpha((uint8_t)(50.f * pressT)),
                    tipBg.Brighten(0.4f).WithAlpha(0), false, pcr);
                r->DrawOutlineRect(Rect(px, py, pw, ph), tipBorder.WithAlpha(bgA), 1.f, pcr);
                r->DrawText(valStr, Rect(px, py, pw, ph), tipText.WithAlpha(tA),
                    (int)fontSize, TextAnchor::Center, fontFamily.c_str());
            });
    }
}
#pragma endregion

#pragma region tab control

void FrutigerAeroSkin::DrawTabControl(IRenderer* renderer, const TabControl& tabControl,
    const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius;
    Color bg = m_style->tabControlBackground;

    int stripSize = tabControl.GetEffectiveTabStripSize();
    Rect stripRect = absRect;
    if (tabControl.GetOrientation() == TabOrientation::Horizontal)
        stripRect.h = (float)stripSize;
    else
        stripRect.w = (float)stripSize;

    renderer->DrawFilledRect(stripRect, bg, cr);

    if (tabControl.IsStripFit())
        return;

    renderer->DrawOutlineRect(absRect, bg.Darken(0.15f).ScaleAlpha(0.8f), 1.f, cr);

    Color sepLine = bg.Darken(0.25f);
    Color sepHighlight = bg.Brighten(0.25f).WithAlpha(60);

    if (tabControl.GetOrientation() == TabOrientation::Horizontal) {
        float sy = absRect.y + (float)stripSize;
        renderer->DrawLine(absRect.x, sy, absRect.x + absRect.w, sy, sepLine, 1.f);
        renderer->DrawLine(absRect.x, sy + 1.f, absRect.x + absRect.w, sy + 1.f, sepHighlight, 1.f);
    }
    else {
        float sx = absRect.x + (float)stripSize;
        renderer->DrawLine(sx, absRect.y, sx, absRect.y + absRect.h, sepLine, 1.f);
        renderer->DrawLine(sx + 1.f, absRect.y, sx + 1.f, absRect.y + absRect.h, sepHighlight, 1.f);
    }
}
#pragma endregion

#pragma region tab button

void FrutigerAeroSkin::DrawTabButton(IRenderer* renderer, const TabPage& tabPage,
    const Rect& absRect, bool active)
{
    if (!m_style) return;
    const AnimState& a = tabPage.Anim();
    float activeT = a.active.Value();
    float hoverT = a.hover.Value();
    float glowT = a.glow.Value();
    float cr = m_style->cornerRadius * 0.6f;

    Color norm = m_style->tabNormalBackground;
    Color sel = m_style->tabSelectedBackground;

    Color raisedTop = norm.Brighten(0.15f);
    Color raisedMid = norm;
    Color raisedBot = norm.Darken(0.12f);

    Color sunkenTop = sel.Darken(0.20f);
    Color sunkenMid = sel.Darken(0.08f);
    Color sunkenBot = sel.Brighten(0.05f);

    Color top = Color::Lerp(raisedTop, sunkenTop, activeT);
    Color mid = Color::Lerp(raisedMid, sunkenMid, activeT);
    Color bot = Color::Lerp(raisedBot, sunkenBot, activeT);

    top = Color::Lerp(top, m_style->tabHoveredBackground, hoverT * 0.3f * (1.f - activeT));

    GradientStop grad[] = {
        { 0.0f, top },
        { 0.46f, mid },
        { 0.54f, Color::Lerp(mid, bot, 0.3f) },
        { 1.0f, bot }
    };
    renderer->DrawMultiGradientRect(absRect, grad, 4, false, cr);

    Color borderColor = Color::Lerp(norm.Darken(0.20f).ScaleAlpha(0.8f),
        sel.Darken(0.25f), activeT);
    renderer->DrawOutlineRect(absRect, borderColor, 1.f, cr);

    float raisedStrength = (1.f - activeT);
    float sunkenStrength = activeT;
    if (raisedStrength > 0.05f)
        DrawBevelRaised(renderer, absRect, cr, (uint8_t)(55.f * raisedStrength));
    if (sunkenStrength > 0.05f)
        DrawBevelSunken(renderer, absRect, cr, (uint8_t)(45.f * sunkenStrength));

    if (raisedStrength > 0.1f)
        DrawGloss(renderer, absRect, cr, (uint8_t)(40.f * raisedStrength));

    if (sunkenStrength > 0.1f) {
        Rect topShadow(absRect.x + 2.f, absRect.y + 1.f, absRect.w - 4.f, 5.f);
        renderer->DrawGradientRect(topShadow,
            sel.Darken(0.6f).WithAlpha((uint8_t)(30.f * sunkenStrength)),
            sel.Darken(0.6f).WithAlpha(0), false, cr);
    }

    if (glowT > 0.01f) {
        renderer->DrawFilledRect(absRect, sel.Brighten(0.3f).WithAlpha((uint8_t)(50.f * glowT)), cr);
    }

    if (activeT > 0.05f) {
        uint8_t accentAlpha = (uint8_t)(220.f * activeT);
        Color ac = m_style->accent.WithAlpha(accentAlpha);
        if (absRect.w >= absRect.h) {
            Rect topBar(absRect.x + 4.f, absRect.y + 1.f, absRect.w - 8.f, 2.5f);
            renderer->DrawFilledRect(topBar, ac, 1.f);
        }
        else {
            Rect leftBar(absRect.x + 1.f, absRect.y + 4.f, 2.5f, absRect.h - 8.f);
            renderer->DrawFilledRect(leftBar, ac, 1.f);
        }
    }

    Color textColor = Color::Lerp(m_style->tabText, m_style->tabSelectedText, activeT);
    TabPage::TabButtonLayout layout = tabPage.ComputeTabButtonLayout(
        absRect, renderer, m_style->fontSize, m_style->fontFamily.c_str());

    if (layout.hasIcon) tabPage.GetIconRef().Draw(renderer, layout.iconRect);
    if (layout.hasText) {
        renderer->DrawText(tabPage.GetTitle(), layout.textRect, textColor,
            m_style->fontSize, layout.textAnchor, m_style->fontFamily.c_str());
    }
}
#pragma endregion

#pragma region tab page body

void FrutigerAeroSkin::DrawTabPageBody(IRenderer* renderer, const TabPage& tabPage, const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius * 0.4f;

    Color body = m_style->tabSelectedBackground.Brighten(0.05f);
    renderer->DrawFilledRect(absRect, body, cr);
    renderer->DrawOutlineRect(absRect, body.Darken(0.15f).ScaleAlpha(0.8f), 1.f, cr);
    DrawBevelSunken(renderer, absRect, cr, 20);
}
#pragma endregion

#pragma region numeric input

void FrutigerAeroSkin::DrawNumericInput(IRenderer* renderer, const NumericInput& input, const Rect& absRect)
{
    if (!m_style) return;
    auto g = input.ComputeGeometry();
    float cr = m_style->cornerRadius;
    int fontSize = m_style->fontSize;
    const wchar_t* font = m_style->fontFamily.c_str();

    renderer->DrawFilledRect(absRect, m_style->numericBackground, cr);
    DrawBevelSunken(renderer, absRect, cr, 30);

    auto drawBtn = [&](const Rect& rect, const std::wstring& symbol, bool hovered, bool pressed) {
        Color base = pressed ? m_style->numericButtonPressed
            : hovered ? m_style->numericButtonHovered
            : m_style->numericButtonNormal;
        renderer->DrawGradientRect(rect, base.Brighten(0.08f), base.Darken(0.15f), false, cr);
        if (pressed)
            DrawBevelSunken(renderer, rect, cr, 25);
        else
            DrawBevelRaised(renderer, rect, cr, 35);
        DrawGloss(renderer, rect, cr, 30);

        Color textCol = pressed ? m_style->numericButtonText.Darken(0.25f) : m_style->numericButtonText;
        renderer->DrawText(symbol, rect, textCol, fontSize, TextAnchor::Center, font);
        };

    drawBtn(g.decrementRect, L"\u2212", input.IsDecrementHovered(), input.IsDecrementPressed());
    drawBtn(g.incrementRect, L"+", input.IsIncrementHovered(), input.IsIncrementPressed());

    std::wstring display = input.IsEditing() ? input.GetEditBuffer() + L"|" : input.GetDisplayText();
    renderer->DrawText(display, g.fieldRect, m_style->numericText, fontSize, TextAnchor::Center, font);
}
#pragma endregion

#pragma region combobox

void FrutigerAeroSkin::DrawComboBox(IRenderer* renderer, const ComboBox& combo,
    const Rect& absRect)
{
    if (!m_style) return;
    const AnimState& a = combo.Anim();
    float hoverT = a.hover.Value();
    float cr = m_style->cornerRadius * 0.6f;

    Color base = m_style->comboBackground;
    Color top = Color::Lerp(base.Brighten(0.05f), m_style->comboHovered.Brighten(0.05f), hoverT);
    Color bot = Color::Lerp(base.Darken(0.08f), m_style->comboHovered.Darken(0.05f), hoverT);
    renderer->DrawGradientRect(absRect, top, bot, false, cr);

    Color border = Color::Lerp(m_style->comboBorder, m_style->accent, hoverT * 0.3f);
    renderer->DrawOutlineRect(absRect, border, 1.f, cr);
    DrawBevelRaised(renderer, absRect, cr, (uint8_t)(35 + 15.f * hoverT));

    float arrowW = m_style->comboArrowWidth;
    float sepX = absRect.x + absRect.w - arrowW;
    renderer->DrawLine(sepX, absRect.y + 4.f, sepX, absRect.y + absRect.h - 4.f,
        m_style->comboBorder.Darken(0.3f).WithAlpha(25), 1.f);
    renderer->DrawLine(sepX + 1.f, absRect.y + 4.f, sepX + 1.f, absRect.y + absRect.h - 4.f,
        base.Brighten(0.15f).WithAlpha(60), 1.f);

    Rect textRect(absRect.x + 10.f, absRect.y, absRect.w - arrowW - 4.f, absRect.h);
    renderer->DrawText(combo.GetDisplayText(renderer), textRect, m_style->comboText,
        m_style->fontSize, TextAnchor::CenterLeft, m_style->fontFamily.c_str());

    float arrowCX = absRect.x + absRect.w - arrowW * 0.5f;
    float arrowCY = absRect.y + absRect.h * 0.5f;
    float as = 4.f;
    Color arrowColor = m_style->comboArrow;
    if (combo.IsOpen()) {
        renderer->DrawLine(arrowCX - as, arrowCY + 1.5f, arrowCX, arrowCY - 2.5f, arrowColor, 2.f);
        renderer->DrawLine(arrowCX, arrowCY - 2.5f, arrowCX + as, arrowCY + 1.5f, arrowColor, 2.f);
        renderer->DrawLine(arrowCX - as, arrowCY + 1.5f, arrowCX + as, arrowCY + 1.5f, arrowColor, 2.f);
    }
    else {
        renderer->DrawLine(arrowCX - as, arrowCY - 1.5f, arrowCX, arrowCY + 2.5f, arrowColor, 2.f);
        renderer->DrawLine(arrowCX, arrowCY + 2.5f, arrowCX + as, arrowCY - 1.5f, arrowColor, 2.f);
        renderer->DrawLine(arrowCX - as, arrowCY - 1.5f, arrowCX + as, arrowCY - 1.5f, arrowColor, 2.f);
    }
}
#pragma endregion

#pragma region combobox dropdown

void FrutigerAeroSkin::DrawComboBoxDropdown(IRenderer* renderer, const ComboBox& combo)
{
    if (!m_style) return;
    Rect dropRect = combo.GetDropdownRect();
    float dropCR = m_style->cornerRadius * 0.4f;

    renderer->DrawDropShadow(dropRect, dropCR, m_style->shadowColor, 0.f, 3.f, 8.f);
    renderer->DrawFilledRect(dropRect, m_style->comboDropBackground, dropCR);
    renderer->DrawOutlineRect(dropRect, m_style->comboDropBorder, 1.f, dropCR);

    renderer->SetScissor(dropRect);

    bool isMulti = combo.GetMode() == ComboBoxMode::MultiSelect;
    int itemCount = combo.GetItemCount();
    float itemCR = 3.f;

    for (int i = 0; i < itemCount; ++i) {
        Rect itemRect = combo.GetDropdownItemRect(i);
        if (itemRect.y + itemRect.h < dropRect.y || itemRect.y > dropRect.y + dropRect.h) continue;
        const AnimState& ia = combo.ItemAnim(i);
        DrawComboBoxItem(renderer, itemRect, combo.GetItems()[i],
            ia.hover.Value(), ia.checked.Value(), isMulti, itemCR);
    }

    renderer->ResetScissor();

    if (combo.NeedsScrollbar()) {
        Rect track = combo.GetDropdownTrackRect();
        Rect thumb = combo.GetDropdownThumbRect();
        DrawScrollbar(renderer, track, thumb, true, combo.GetDropdownScrollbar().IsThumbHovered());
    }
}
#pragma endregion

#pragma region combobox item

void FrutigerAeroSkin::DrawComboBoxItem(IRenderer* renderer, const Rect& itemRect,
    const std::wstring& text, float hoverT, float checkT,
    bool isMultiSelect, float cornerRadius)
{
    if (!m_style) return;

    if (hoverT > 0.01f) {
        renderer->DrawFilledRect(itemRect,
            m_style->comboItemHovered.WithAlpha((uint8_t)(255.f * hoverT)), cornerRadius);
    }

    if (checkT > 0.01f) {
        float barW = 3.f * checkT;
        Rect accentBar(itemRect.x, itemRect.y + 2.f, barW, itemRect.h - 4.f);
        renderer->DrawFilledRect(accentBar, m_style->accent.WithAlpha((uint8_t)(255.f * checkT)), 1.5f);

        renderer->DrawFilledRect(itemRect, m_style->comboItemSelected.WithAlpha((uint8_t)(m_style->comboItemSelected.a * checkT)), cornerRadius);
    }

    float textOffsetX = 0.f;
    if (isMultiSelect) {
        float boxSize = 16.f;
        float boxCX = itemRect.x + 7.f + boxSize * 0.5f;
        float boxCY = itemRect.y + itemRect.h * 0.5f;
        float boxR = boxSize * 0.5f;

        Color boxBase = m_style->checkboxBoxNormal;
        renderer->DrawGradientCircle(boxCX, boxCY, boxR, boxBase, boxBase.Darken(0.10f));
        renderer->DrawCircleOutline(boxCX, boxCY, boxR, boxBase.Darken(0.35f), 1.f);

        if (checkT > 0.01f) {
            float innerR = (boxR - 3.f) * checkT;
            renderer->DrawCircle(boxCX, boxCY, innerR, m_style->comboCheckMark);
        }
        textOffsetX = boxSize + 14.f;
    }
    else {
        textOffsetX = 10.f;
    }

    Color itemTextColor = Color::Lerp(m_style->comboItemText, m_style->comboItemTextSelected, checkT);
    Rect tRect(itemRect.x + textOffsetX, itemRect.y, itemRect.w - textOffsetX - 4.f, itemRect.h);
    renderer->DrawText(text, tRect, itemTextColor,
        m_style->fontSize, TextAnchor::CenterLeft, m_style->fontFamily.c_str());
}
#pragma endregion

#pragma region text input

void FrutigerAeroSkin::DrawTextInput(IRenderer* renderer, const TextInputVisuals& vis)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius * 0.4f;

    renderer->DrawFilledRect(vis.absRect, m_style->inputBackground, cr);

    DrawBevelSunken(renderer, vis.absRect, cr, (uint8_t)(50 + 20.f * vis.focusT));

    Color border = Color::Lerp(m_style->inputBorder, m_style->inputBorderFocused, vis.focusT);
    renderer->DrawOutlineRect(vis.absRect, border, 1.f + 0.5f * vis.focusT, cr);

    if (vis.focusT > 0.1f) {
        Rect outer(vis.absRect.x - 1.f, vis.absRect.y - 1.f,
            vis.absRect.w + 2.f, vis.absRect.h + 2.f);
        renderer->DrawOutlineRect(outer,
            m_style->accent.WithAlpha((uint8_t)(60.f * vis.focusT)),
            1.5f, cr + 1.f);
    }

    renderer->SetScissor(vis.contentRect);

    if (vis.showPlaceholder) {
        renderer->DrawText(vis.placeholderText, vis.contentRect, m_style->inputPlaceholder,
            m_style->fontSize, vis.placeholderAnchor, m_style->fontFamily.c_str());
    }
    else {
        for (const auto& sel : vis.selectionRects)
            renderer->DrawFilledRect(sel, m_style->inputSelection, 2.f);

        for (const auto& line : vis.lines)
            renderer->DrawText(line.text, line.rect, m_style->inputText,
                m_style->fontSize, TextAnchor::CenterLeft, m_style->fontFamily.c_str());

        if (vis.showCursor) {
            Rect cursorRect(vis.cursorRect.x, vis.cursorRect.y + 1.f,
                2.f, vis.cursorRect.h - 2.f);
            renderer->DrawFilledRect(cursorRect, m_style->inputCursor);
        }
    }

    renderer->ResetScissor();

    if (vis.showScrollbar) {
        DrawScrollbar(renderer, vis.scrollTrackRect, vis.scrollThumbRect, true, false);
    }
}
#pragma endregion

#pragma region keybind

void FrutigerAeroSkin::DrawKeyBind(IRenderer* renderer, const KeyBind& keybind,
    const Rect& absRect)
{
    if (!m_style) return;
    const AnimState& a = keybind.Anim();
    float cr = m_style->cornerRadius * 0.4f;
    float hoverT = a.hover.Value();
    float glowT = a.glow.Value();

    Color shell = m_style->keybindBackground;
    Color shellColor = Color::Lerp(shell.Darken(0.15f), shell.Darken(0.08f), hoverT);
    shellColor = Color::Lerp(shellColor, shell, glowT);
    renderer->DrawFilledRect(absRect, shellColor, cr);

    Color borderColor = Color::Lerp(m_style->keybindBorder, m_style->accent, glowT);
    renderer->DrawOutlineRect(absRect, borderColor, 1.f + 0.5f * glowT, cr);

    float step = 3.f - 1.f * glowT;
    Rect face(absRect.x + step, absRect.y + 1.f,
        absRect.w - step * 2.f, absRect.h - step - 1.f);
    float faceCR = cr > 1.f ? cr - 1.f : 0.f;

    Color faceTop = Color::Lerp(shell.Brighten(0.06f), m_style->keybindHovered.Brighten(0.06f), glowT);
    Color faceBot = Color::Lerp(shell.Darken(0.06f), m_style->keybindHovered, glowT);
    renderer->DrawGradientRect(face, faceTop, faceBot, false, faceCR);
    DrawBevelRaised(renderer, face, faceCR, (uint8_t)(40 * (1.f - glowT)));

    if (glowT > 0.01f) {
        uint8_t ga = (uint8_t)(200.f * glowT);
        Rect accentBar(face.x + 4.f, face.y + face.h - 3.f, face.w - 8.f, 2.5f);
        renderer->DrawFilledRect(accentBar, m_style->accent.WithAlpha(ga), 1.f);
    }

    Color textColor = Color::Lerp(m_style->keybindText, m_style->keybindTextListening, glowT);
    renderer->DrawText(keybind.GetDisplayText(), face, textColor,
        m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
}
#pragma endregion

#pragma region tab events

void FrutigerAeroSkin::OnTabActivated(TabPage& tabPage)
{
    tabPage.Anim().active.easing = Easing::BackOut;
    tabPage.Anim().active.TransitionTo(1.f, 0.18f);
    tabPage.Anim().glow.easing = Easing::ExpoOut;
    tabPage.Anim().glow.Set(1.f);
    tabPage.Anim().glow.TransitionTo(0.f, 0.5f);
}

void FrutigerAeroSkin::OnTabDeactivated(TabPage& tabPage)
{
    tabPage.Anim().active.easing = Easing::CubicOut;
    tabPage.Anim().active.TransitionTo(0.f, 0.25f);
    tabPage.Anim().glow.Set(0.f);
}
#pragma endregion

#pragma region scrollbar

void FrutigerAeroSkin::DrawScrollbarTrack(IRenderer* renderer, const Rect& rect, bool vertical)
{
    if (!m_style) return;
    float cr = (float)m_style->scrollbarWidth * 0.3f;
    renderer->DrawFilledRect(rect, m_style->scrollbarTrack, cr);
    DrawBevelSunken(renderer, rect, cr, 20);
}

void FrutigerAeroSkin::DrawScrollbarThumb(IRenderer* renderer, const Rect& rect,
    bool vertical, bool hovered)
{
    if (!m_style) return;
    float cr = 3.f;
    float inset = 2.f;
    Rect thumbRect = vertical
        ? Rect(rect.x + inset, rect.y, rect.w - inset * 2.f, rect.h)
        : Rect(rect.x, rect.y + inset, rect.w, rect.h - inset * 2.f);

    Color base = hovered ? m_style->scrollbarThumbHover : m_style->scrollbarThumb;
    GradientStop thumb[] = {
        { 0.0f,  base.Brighten(0.10f) },
        { 0.48f, base },
        { 0.52f, base.Darken(0.06f) },
        { 1.0f,  base.Darken(0.10f) }
    };
    bool horiz = !vertical;
    renderer->DrawMultiGradientRect(thumbRect, thumb, 4, horiz, cr);
    renderer->DrawOutlineRect(thumbRect, base.Darken(0.20f).ScaleAlpha(0.8f), 1.f, cr);
    DrawBevelRaised(renderer, thumbRect, cr, 30);

    float gcx = thumbRect.x + thumbRect.w * 0.5f;
    float gcy = thumbRect.y + thumbRect.h * 0.5f;
    Color gripLight = base.Brighten(0.35f).WithAlpha(100);
    Color gripDark = base.Darken(0.25f).WithAlpha(25);
    if (vertical && thumbRect.h > 20.f) {
        DrawGripLines(renderer, gcx, gcy, thumbRect.w * 0.5f, 3, gripLight, gripDark);
    }
    else if (!vertical && thumbRect.w > 20.f) {
        float totalW = 8.f;
        float startX = gcx - totalW * 0.5f;
        for (int i = 0; i < 3; i++) {
            float x = startX + i * 3.f;
            renderer->DrawLine(x, gcy - thumbRect.h * 0.25f, x, gcy + thumbRect.h * 0.25f, gripDark, 1.f);
            renderer->DrawLine(x + 1.f, gcy - thumbRect.h * 0.25f, x + 1.f, gcy + thumbRect.h * 0.25f, gripLight, 1.f);
        }
    }
}
#pragma endregion

#pragma region listbox

void FrutigerAeroSkin::DrawListBox(IRenderer* renderer, const ListBox& listbox,
    const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius * 0.4f;

    renderer->DrawFilledRect(absRect, m_style->listboxBackground, cr);
    renderer->DrawOutlineRect(absRect, m_style->listboxBorder, 1.f, cr);
    DrawBevelSunken(renderer, absRect, cr, 35);

    Rect client = listbox.GetClientRect();
    renderer->SetScissor(client);

    Panel* panel = listbox.GetContentPanel();
    Scrollbar* vsb = panel ? panel->GetVScrollbar() : nullptr;
    float scrollY = (vsb && vsb->IsShown()) ? vsb->GetScrollOffset() : 0.f;
    float sbWidth = (vsb && vsb->IsShown()) ? vsb->GetWidth() : 0.f;
    float contentW = client.w - sbWidth;

    bool isMulti = listbox.GetMode() == ListBoxMode::MultiSelect;
    int itemCount = listbox.GetItemCount();
    float itemH = listbox.GetItemHeight();
    float itemCR = 3.f;

    for (int i = 0; i < itemCount; ++i) {
        float itemY = (float)client.y + i * itemH - scrollY;
        if (itemY + itemH < client.y || itemY > client.y + client.h) continue;
        Rect itemRect(client.x + 2.f, itemY + 1.f, contentW - 4.f, itemH - 2.f);

        if (i % 2 == 1)
            renderer->DrawFilledRect(itemRect, m_style->listboxBackground.Darken(0.03f).WithAlpha(8), itemCR);

        const AnimState& ia = listbox.ItemAnim(i);
        DrawListBoxItem(renderer, itemRect, listbox.GetItems()[i],
            ia.hover.Value(), ia.checked.Value(), isMulti, itemCR);
    }
    renderer->ResetScissor();
}

void FrutigerAeroSkin::DrawListBoxItem(IRenderer* renderer, const Rect& itemRect,
    const std::wstring& text, float hoverT, float checkT,
    bool isMultiSelect, float cornerRadius)
{
    DrawComboBoxItem(renderer, itemRect, text, hoverT, checkT, isMultiSelect, cornerRadius);
}
#pragma endregion

#pragma region color swatch

void FrutigerAeroSkin::DrawColorSwatch(IRenderer* renderer, const ColorSwatch& swatch,
    const Rect& absRect)
{
    if (!m_style) return;
    const AnimState& a = swatch.Anim();

    float cx = absRect.x + absRect.w * 0.5f;
    float cy = absRect.y + absRect.h * 0.5f;
    float r = (std::min)(absRect.w, absRect.h) * 0.5f;

    renderer->DrawCircle(cx, cy, r, m_style->colorSwatchBorder.Brighten(0.40f));
    renderer->DrawCircle(cx, cy, r, swatch.GetColor());
    renderer->DrawCircle(cx, cy - r * 0.2f, r * 0.5f,
        swatch.GetColor().Brighten(0.6f).WithAlpha(40));

    Color border = Color::Lerp(m_style->colorSwatchBorder, m_style->colorSwatchHoverBorder, a.hover.Value());
    renderer->DrawCircleOutline(cx, cy, r, border, 1.5f);
}
#pragma endregion

#pragma region color picker modal

void FrutigerAeroSkin::DrawColorPickerModal(IRenderer* renderer, ColorPickerModal& modal,
    const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->colorPickerCornerRadius;
    Color bg = m_style->colorPickerBackground;
    Color borderColor = m_style->colorPickerBorder;

    renderer->DrawDropShadow(absRect, cr, m_style->shadowColor, 0.f, 4.f, 12.f);

    GradientStop bgGrad[] = {
        { 0.0f, bg.Brighten(0.05f) },
        { 1.0f, bg.Darken(0.05f) }
    };
    renderer->DrawMultiGradientRect(absRect, bgGrad, 2, false, cr);
    renderer->DrawOutlineRect(absRect, borderColor, 1.f, cr);
    DrawBevelRaised(renderer, absRect, cr, 40);

    Rect grad = modal.GetGradientRect();
    Color hueColor = ColorPickerModal::HSVToRGB(modal.GetHue(), 1.f, 1.f);
    renderer->DrawGradientRect(grad, Color(255, 255, 255, 255), hueColor, true, 2.f);
    renderer->DrawGradientRect(grad, Color(0, 0, 0, 0), Color(0, 0, 0, 255), false, 2.f);
    renderer->DrawOutlineRect(grad, borderColor.Darken(0.10f).ScaleAlpha(0.8f), 1.f, 2.f);
    DrawBevelSunken(renderer, grad, 2.f, 25);

    float crossX = grad.x + modal.GetSaturation() * grad.w;
    float crossY = grad.y + (1.f - modal.GetBrightness()) * grad.h;
    renderer->DrawCircleOutline(crossX, crossY, 6.f, Color(255, 255, 255, 230), 2.f);
    renderer->DrawCircleOutline(crossX, crossY, 5.f, Color(0, 0, 0, 150), 1.f);

    Rect hueBar = modal.GetHueBarRect();
    constexpr int hueStops = 6;
    float bandH = hueBar.h / (float)hueStops;
    for (int i = 0; i < hueStops; i++) {
        Color c0 = ColorPickerModal::HSVToRGB((float)i / hueStops, 1.f, 1.f);
        Color c1 = ColorPickerModal::HSVToRGB((float)(i + 1) / hueStops, 1.f, 1.f);
        renderer->DrawGradientRect(Rect(hueBar.x, hueBar.y + i * bandH, hueBar.w, bandH + 1.f),
            c0, c1, false, 0.f);
    }
    renderer->DrawOutlineRect(hueBar, borderColor.Darken(0.10f).ScaleAlpha(0.8f), 1.f, 2.f);

    float hueY = hueBar.y + modal.GetHue() * hueBar.h;
    Rect hueInd(hueBar.x - 2.f, hueY - 3.f, hueBar.w + 4.f, 6.f);
    renderer->DrawFilledRect(hueInd, bg.Brighten(0.10f), 3.f);
    renderer->DrawOutlineRect(hueInd, borderColor.ScaleAlpha(0.8f), 1.f, 3.f);
    DrawBevelRaised(renderer, hueInd, 3.f, 40);

    Rect prev = modal.GetPreviousColorRect();
    Rect curr = modal.GetCurrentColorRect();
    renderer->DrawText(L"Old", Rect(prev.x, prev.y - 14.f, prev.w, 13.f),
        m_style->colorPickerLabelText, m_style->fontSize - 2,
        TextAnchor::CenterLeft, m_style->fontFamily.c_str());
    renderer->DrawText(L"New", Rect(curr.x, curr.y - 14.f, curr.w, 13.f),
        m_style->colorPickerLabelText, m_style->fontSize - 2,
        TextAnchor::CenterLeft, m_style->fontFamily.c_str());

    Color swatchBg = bg.Darken(0.08f);
    for (const auto& sr : { prev, curr }) {
        renderer->DrawFilledRect(sr, swatchBg, 3.f);
    }
    renderer->DrawFilledRect(prev, modal.GetPreviousColor(), 3.f);
    renderer->DrawFilledRect(curr, modal.GetCurrentColor(), 3.f);
    for (const auto& sr : { prev, curr }) {
        renderer->DrawOutlineRect(sr, borderColor.ScaleAlpha(0.7f), 1.f, 3.f);
        DrawBevelSunken(renderer, sr, 3.f, 20);
    }

    Color c = modal.GetCurrentColor();
    wchar_t buf[64];
    swprintf_s(buf, L"R:%d G:%d B:%d", c.r, c.g, c.b);
    Rect applyRect = modal.GetApplyButtonRect();
    float rgbLeft = curr.x + curr.w + 8.f;
    float rgbW = applyRect.x - 8.f - rgbLeft;
    if (rgbW > 10.f)
        renderer->DrawText(buf, Rect(rgbLeft, curr.y, rgbW, curr.h),
            m_style->colorPickerLabelText, m_style->fontSize - 2,
            TextAnchor::CenterLeft, m_style->fontFamily.c_str());

    Rect cancelRect = modal.GetCancelButtonRect();
    float btnCR = absRect.h * 0.02f;
    Color btnNorm = m_style->colorPickerButtonNormal;
    Color btnHov = m_style->colorPickerButtonHovered;

    for (const auto& [btnRect, btnText, isHovered] : {
        std::tuple{applyRect, L"Apply", modal.IsApplyHovered()},
        std::tuple{cancelRect, L"Cancel", modal.IsCancelHovered()} })
    {
        float bh = isHovered ? 1.f : 0.f;
        Color base = Color::Lerp(btnNorm, btnHov, bh);
        Color bTop = base.Brighten(0.05f);
        Color bBot = base.Darken(0.08f);
        renderer->DrawGradientRect(btnRect, bTop, bBot, false, btnCR);
        renderer->DrawOutlineRect(btnRect, base.Darken(0.25f).ScaleAlpha(0.8f), 1.f, btnCR);
        DrawBevelRaised(renderer, btnRect, btnCR, (uint8_t)(35 + 15.f * bh));
        renderer->DrawText(btnText, btnRect, m_style->colorPickerButtonText,
            m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
    }
}
#pragma endregion

#pragma region color picker

void FrutigerAeroSkin::DrawColorPicker(IRenderer* renderer, const ColorPicker& picker, const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius * 0.4f;
    Color borderColor = m_style->colorPickerBorder;
    Color bg = m_style->colorPickerBackground;
    bool vertical = (picker.GetHueBarPosition() == HueBarPosition::Left ||
        picker.GetHueBarPosition() == HueBarPosition::Right);

    Rect grad = picker.GetGradientRect();
    DrawSVGradient(renderer, grad, picker.GetHue(), cr, picker.GetAlpha());
    renderer->DrawOutlineRect(grad, borderColor.ScaleAlpha(0.8f), 1.f, cr);
    DrawBevelSunken(renderer, grad, cr, 25);

    float crossX = grad.x + picker.GetSaturation() * grad.w;
    float crossY = grad.y + (1.f - picker.GetBrightness()) * grad.h;
    renderer->DrawCircleOutline(crossX, crossY, 6.f, Color(255, 255, 255, 230), 2.f);
    renderer->DrawCircleOutline(crossX, crossY, 5.f, Color(0, 0, 0, 150), 1.f);

    Rect hueBar = picker.GetHueBarRect();
    DrawHueBarFill(renderer, hueBar, vertical);
    renderer->DrawOutlineRect(hueBar, borderColor.ScaleAlpha(0.8f), 1.f, cr);

    if (vertical) {
        float hy = hueBar.y + picker.GetHue() * hueBar.h;
        Rect ind(hueBar.x - 2.f, hy - 3.f, hueBar.w + 4.f, 6.f);
        renderer->DrawFilledRect(ind, bg.Brighten(0.10f), 3.f);
        renderer->DrawOutlineRect(ind, borderColor.ScaleAlpha(0.8f), 1.f, 3.f);
        DrawBevelRaised(renderer, ind, 3.f, 40);
    }
    else {
        float hx = hueBar.x + picker.GetHue() * hueBar.w;
        Rect ind(hx - 3.f, hueBar.y - 2.f, 6.f, hueBar.h + 4.f);
        renderer->DrawFilledRect(ind, bg.Brighten(0.10f), 3.f);
        renderer->DrawOutlineRect(ind, borderColor.ScaleAlpha(0.8f), 1.f, 3.f);
        DrawBevelRaised(renderer, ind, 3.f, 40);
    }

    if (picker.GetShowAlphaBar()) {
        Rect alphaBar = picker.GetAlphaBarRect();
        DrawAlphaBarFill(renderer, alphaBar, picker.GetCurrentColor(), vertical);
        renderer->DrawOutlineRect(alphaBar, borderColor.ScaleAlpha(0.8f), 1.f, cr);
        DrawBevelSunken(renderer, alphaBar, cr, 15);

        float alphaT = 1.f - picker.GetAlpha() / 255.f;
        if (vertical) {
            float ay = alphaBar.y + 3.f + alphaT * (alphaBar.h - 6.f);
            Rect ind(alphaBar.x - 2.f, ay - 3.f, alphaBar.w + 4.f, 6.f);
            renderer->DrawFilledRect(ind, bg.Brighten(0.10f), 3.f);
            renderer->DrawOutlineRect(ind, Color(0, 0, 0, 80), 1.f, 3.f);
            DrawBevelRaised(renderer, ind, 3.f, 40);
        }
        else {
            float ax = alphaBar.x + 3.f + alphaT * (alphaBar.w - 6.f);
            Rect ind(ax - 3.f, alphaBar.y - 2.f, 6.f, alphaBar.h + 4.f);
            renderer->DrawFilledRect(ind, bg.Brighten(0.10f), 3.f);
            renderer->DrawOutlineRect(ind, Color(0, 0, 0, 80), 1.f, 3.f);
            DrawBevelRaised(renderer, ind, 3.f, 40);
        }
    }
}
#pragma endregion