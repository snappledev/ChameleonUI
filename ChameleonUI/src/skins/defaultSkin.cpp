#include "../../include/skins/DefaultSkin.h"
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
#pragma region layout queries
Size DefaultSkin::GetCheckboxContentSize(const Checkbox& checkbox) const
{
    if (!m_style) return { 0, 0 };
    float bs = checkbox.GetEffectiveBoxSize();
    return { bs, bs };
}

Size DefaultSkin::GetButtonContentSize(const Button& button) const
{
    if (!m_style) return { 0, 0 };
    float tw = button.GetCachedTextWidth();
    float th = button.GetCachedTextHeight();
    Rect cp = button.GetContentPadding();
    float w = (tw > 0) ? tw + cp.x + cp.w : 100.f;
    float h = (th > 0) ? th + cp.y + cp.h : m_style->buttonMinHeight;
    if (h < m_style->buttonMinHeight) h = m_style->buttonMinHeight;
    return { w, h };
}

float DefaultSkin::GetComboBoxMinHeight() const
{
    return m_style ? m_style->comboMinHeight : 0.f;
}

Rect DefaultSkin::GetHitRect(const UIElement& /*element*/, const Rect& absRect) const
{
    return absRect;
}
#pragma endregion layout queries

#pragma region checkmark
void DefaultSkin::DrawCheckMark(IRenderer* renderer, const Rect& boxRect,
    float checkT, Color markColor, float strokeWidth)
{
    if (checkT <= 0.01f) return;

    float cx = boxRect.x + boxRect.w * 0.5f;
    float cy = boxRect.y + boxRect.h * 0.5f;
    float scale = boxRect.w / 18.f;
    float s = checkT;

    uint8_t alpha = (uint8_t)((float)markColor.a * checkT);
    Color mark(markColor.r, markColor.g, markColor.b, alpha);

    renderer->DrawLine(
        cx - 4.f * s * scale, cy,
        cx - 1.f * s * scale, cy + 3.f * s * scale,
        mark, strokeWidth);
    renderer->DrawLine(
        cx - 1.f * s * scale, cy + 3.f * s * scale,
        cx + 4.f * s * scale, cy - 3.f * s * scale,
        mark, strokeWidth);
}
#pragma endregion checkmark

#pragma region window
void DefaultSkin::DrawWindowBackground(IRenderer* renderer, const Window& window, const Rect& absRect)
{
    if (!m_style) return;
    renderer->DrawGlassRect(absRect, m_style->glassBlurRadius,
        Color(m_style->windowBackground.r, m_style->windowBackground.g,
            m_style->windowBackground.b, m_style->windowBackground.a),
        m_style->cornerRadius, m_style->glassBorderAlpha);
}

void DefaultSkin::DrawWindowTitleBar(IRenderer* renderer, const Window& window, const Rect& absRect)
{
    if (!m_style) return;
    
    if (!window.ShowTitleBar()) return;

    
    renderer->DrawGlassRect(absRect, 20.0f, m_style->titleBarBackground.WithAlpha(200), m_style->cornerRadius);

    float sepY = absRect.y + absRect.h - 1.f;
    renderer->DrawLine(absRect.x + 12.f, sepY, absRect.x + absRect.w - 12.f, sepY,
        Color(255, 255, 255, 18), 1.f);
    renderer->DrawText(window.GetTitle(), absRect, m_style->titleBarText,
        m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
}
#pragma endregion window

#pragma region button
void DefaultSkin::DrawButton(IRenderer* renderer, const Button& button, const Rect& absRect,
    bool hovered, bool pressed)
{
    if (!m_style) return;
    const AnimState& a = button.Anim();
    float cr = m_style->cornerRadius * 0.75f;

    Color bg = m_style->buttonNormal;
    bg = Color::Lerp(bg, m_style->buttonHovered, a.hover.Value());
    bg = Color::Lerp(bg, m_style->buttonPressed, a.press.Value());
    renderer->DrawFilledRect(absRect, bg, cr);

    uint8_t borderAlpha = (uint8_t)(25 + 15.f * a.hover.Value());
    renderer->DrawOutlineRect(absRect, Color(255, 255, 255, borderAlpha), 1.f, cr);

    if (a.glow.Value() > 0.01f) {
        uint8_t glowAlpha = (uint8_t)(120.f * a.glow.Value());
        Rect glowRect(absRect.x + 4.f, absRect.y + absRect.h - 3.f, absRect.w - 8.f, 2.f);
        renderer->DrawFilledRect(glowRect,
            Color(m_style->accent.r, m_style->accent.g, m_style->accent.b, glowAlpha), 1.f);
    }

    Rect cp = button.GetContentPadding();
    Rect textRect(absRect.x + cp.x, absRect.y + cp.y,
        absRect.w - cp.x - cp.w, absRect.h - cp.y - cp.h);
    renderer->DrawText(button.GetText(), textRect, m_style->buttonText,
        m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
}
#pragma endregion button

#pragma region checkbox
void DefaultSkin::DrawCheckbox(IRenderer* renderer, const Checkbox& checkbox, const Rect& absRect)
{
    if (!m_style) return;
    const AnimState& a = checkbox.Anim();
    float checkT = a.checked.Value();
    float boxCR = m_style->checkboxCornerRadius;

    Color boxBg = Color::Lerp(m_style->checkboxBoxNormal, m_style->checkboxBoxChecked, checkT);
    renderer->DrawFilledRect(absRect, boxBg, boxCR);

    if (checkT < 0.99f) {
        uint8_t borderAlpha = (uint8_t)(30.f * (1.f - checkT));
        renderer->DrawOutlineRect(absRect, Color(255, 255, 255, borderAlpha), 1.f, boxCR);
    }

    DrawCheckMark(renderer, absRect, checkT, m_style->checkboxCheckMark, 2.f);

    if (a.glow.Value() > 0.01f) {
        uint8_t glowAlpha = (uint8_t)(40.f * a.glow.Value());
        renderer->DrawFilledRect(
            Rect(absRect.x - 2.f, absRect.y - 2.f, absRect.w + 4.f, absRect.h + 4.f),
            Color(m_style->accent.r, m_style->accent.g, m_style->accent.b, glowAlpha),
            boxCR + 2.f);
    }

    if (a.hover.Value() > 0.01f) {
        uint8_t hoverAlpha = (uint8_t)(15.f * a.hover.Value());
        renderer->DrawFilledRect(absRect, Color(255, 255, 255, hoverAlpha), boxCR);
    }
}
#pragma endregion checkbox

#pragma region label
void DefaultSkin::DrawLabel(IRenderer* renderer, const Label& label, const Rect& absRect)
{
    if (!m_style) return;
    renderer->DrawText(label.GetText(), absRect, m_style->labelText,
        m_style->fontSize, TextAnchor::CenterLeft, m_style->fontFamily.c_str());
}
#pragma endregion label

#pragma region color swatch
void DefaultSkin::DrawColorSwatch(IRenderer* renderer, const ColorSwatch& swatch, const Rect& absRect)
{
    if (!m_style) return;
    const AnimState& a = swatch.Anim();
    float cr = m_style->colorSwatchCornerRadius;

    renderer->DrawFilledRect(absRect, Color(200, 200, 200, 255), cr);
    renderer->DrawFilledRect(absRect, swatch.GetColor(), cr);

    Color border = Color::Lerp(m_style->colorSwatchBorder, m_style->colorSwatchHoverBorder, a.hover.Value());
    renderer->DrawOutlineRect(absRect, border, 1.f, cr);
}
#pragma endregion color swatch

#pragma region color picker
void DefaultSkin::DrawColorPickerModal(IRenderer* renderer, ColorPickerModal& modal, const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->colorPickerCornerRadius;

    renderer->DrawGlassRect(absRect, m_style->glassBlurRadius,
        m_style->colorPickerBackground, cr, 0.f);
    renderer->DrawOutlineRect(absRect, m_style->colorPickerBorder, 1.f, cr);

    Rect grad = modal.GetGradientRect();
    float hue = modal.GetHue();
    Color hueColor = ColorPickerModal::HSVToRGB(hue, 1.f, 1.f);

    renderer->DrawGradientRect(grad, Color(255, 255, 255, 255), hueColor, true, 2.f);
    renderer->DrawGradientRect(grad, Color(0, 0, 0, 0), Color(0, 0, 0, 255), false, 2.f);
    renderer->DrawOutlineRect(grad, Color(255, 255, 255, 20), 1.f, 2.f);

    float crossX = grad.x + modal.GetSaturation() * grad.w;
    float crossY = grad.y + (1.f - modal.GetBrightness()) * grad.h;
    renderer->DrawCircleOutline(crossX, crossY, 5.f, Color(255, 255, 255, 200), 1.5f);
    renderer->DrawCircleOutline(crossX, crossY, 4.f, Color(0, 0, 0, 150), 1.f);

    Rect hueBar = modal.GetHueBarRect();
    constexpr int hueStops = 6;
    float bandH = hueBar.h / (float)hueStops;
    for (int i = 0; i < hueStops; i++)
    {
        float h0 = (float)i / (float)hueStops;
        float h1 = (float)(i + 1) / (float)hueStops;
        Color c0 = ColorPickerModal::HSVToRGB(h0, 1.f, 1.f);
        Color c1 = ColorPickerModal::HSVToRGB(h1, 1.f, 1.f);
        Rect band(hueBar.x, hueBar.y + i * bandH, hueBar.w, bandH + 1.f);
        renderer->DrawGradientRect(band, c0, c1, false, 0.f);
    }
    renderer->DrawOutlineRect(hueBar, Color(255, 255, 255, 20), 1.f, 2.f);

    float hueY = hueBar.y + hue * hueBar.h;
    renderer->DrawFilledRect(Rect(hueBar.x - 2, hueY - 2, hueBar.w + 4, 4.0),
        Color(255, 255, 255, 220), 2.f);

    Rect prev = modal.GetPreviousColorRect();
    Rect curr = modal.GetCurrentColorRect();

    renderer->DrawText(L"Old", Rect(prev.x, prev.y - 13.f, prev.w, 12.f),
        m_style->colorPickerLabelText, m_style->fontSize - 2,
        TextAnchor::CenterLeft, m_style->fontFamily.c_str());
    renderer->DrawText(L"New", Rect(curr.x, curr.y - 13.f, curr.w, 12.f),
        m_style->colorPickerLabelText, m_style->fontSize - 2,
        TextAnchor::CenterLeft, m_style->fontFamily.c_str());

    renderer->DrawFilledRect(prev, Color(200, 200, 200, 255), 3.f);
    renderer->DrawFilledRect(prev, modal.GetPreviousColor(), 3.f);
    renderer->DrawOutlineRect(prev, Color(255, 255, 255, 30), 1.f, 3.f);

    renderer->DrawFilledRect(curr, Color(200, 200, 200, 255), 3.f);
    renderer->DrawFilledRect(curr, modal.GetCurrentColor(), 3.f);
    renderer->DrawOutlineRect(curr, Color(255, 255, 255, 30), 1.f, 3.f);

    Color c = modal.GetCurrentColor();
    wchar_t buf[64];
    swprintf_s(buf, L"R:%d G:%d B:%d", c.r, c.g, c.b);

    Rect applyRect = modal.GetApplyButtonRect();
    float rgbLeft = curr.x + curr.w + 8.f;
    float rgbRight = applyRect.x - 8.f;
    float rgbW = rgbRight - rgbLeft;
    if (rgbW > 10.f)
    {
        Rect textRect(rgbLeft, curr.y, rgbW, curr.h);
        renderer->DrawText(buf, textRect, m_style->colorPickerLabelText,
            m_style->fontSize - 2, TextAnchor::CenterLeft, m_style->fontFamily.c_str());
    }

    Rect cancelRect = modal.GetCancelButtonRect();

    Color applyBg = modal.IsApplyHovered() ? m_style->colorPickerButtonHovered : m_style->colorPickerButtonNormal;
    Color cancelBg = modal.IsCancelHovered() ? m_style->colorPickerButtonHovered : m_style->colorPickerButtonNormal;
    float btnCR = cr * 0.5f;

    renderer->DrawFilledRect(applyRect, applyBg, btnCR);
    renderer->DrawOutlineRect(applyRect, Color(255, 255, 255, 20), 1.f, btnCR);
    renderer->DrawText(L"Apply", applyRect, m_style->colorPickerButtonText,
        m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());

    renderer->DrawFilledRect(cancelRect, cancelBg, btnCR);
    renderer->DrawOutlineRect(cancelRect, Color(255, 255, 255, 20), 1.f, btnCR);
    renderer->DrawText(L"Cancel", cancelRect, m_style->colorPickerButtonText,
        m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
}

void DefaultSkin::DrawColorPicker(IRenderer* renderer, const ColorPicker& picker, const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius * 0.4f;
    bool vertical = (picker.GetHueBarPosition() == HueBarPosition::Left ||
        picker.GetHueBarPosition() == HueBarPosition::Right);

    Rect grad = picker.GetGradientRect();
    DrawSVGradient(renderer, grad, picker.GetHue(), cr, picker.GetAlpha());
    renderer->DrawOutlineRect(grad, Color(255, 255, 255, 20), 1.f, cr);

    float crossX = grad.x + picker.GetSaturation() * grad.w;
    float crossY = grad.y + (1.f - picker.GetBrightness()) * grad.h;
    renderer->DrawCircleOutline(crossX, crossY, 5.f, Color(255, 255, 255, 200), 1.5f);
    renderer->DrawCircleOutline(crossX, crossY, 4.f, Color(0, 0, 0, 150), 1.f);

    Rect hueBar = picker.GetHueBarRect();
    DrawHueBarFill(renderer, hueBar, vertical);
    renderer->DrawOutlineRect(hueBar, Color(255, 255, 255, 20), 1.f, cr);

    if (vertical) {
        float hy = hueBar.y + picker.GetHue() * hueBar.h;
        renderer->DrawFilledRect(Rect(hueBar.x - 2, hy - 2, hueBar.w + 4, 4.f),
            Color(255, 255, 255, 220), 2.f);
    }
    else {
        float hx = hueBar.x + picker.GetHue() * hueBar.w;
        renderer->DrawFilledRect(Rect(hx - 2, hueBar.y - 2, 4.f, hueBar.h + 4),
            Color(255, 255, 255, 220), 2.f);
    }

    if (picker.GetShowAlphaBar()) {
        Rect alphaBar = picker.GetAlphaBarRect();
        DrawAlphaBarFill(renderer, alphaBar, picker.GetCurrentColor(), vertical);
        renderer->DrawOutlineRect(alphaBar, Color(255, 255, 255, 20), 1.f, cr);

        float alphaT = 1.f - picker.GetAlpha() / 255.f;
        if (vertical) {
            float hy = alphaBar.y + 2.f + alphaT * (alphaBar.h - 4.f); // clamp 2px from edges
            Rect ind(alphaBar.x - 2, hy - 2, alphaBar.w + 4, 4.f);
            renderer->DrawFilledRect(ind, Color(255, 255, 255, 240), 2.f);
            renderer->DrawOutlineRect(ind, Color(0, 0, 0, 120), 1.f, 2.f);
        }
        else {
            float hx = alphaBar.x + 2.f + alphaT * (alphaBar.w - 4.f);
            Rect ind(hx - 2, alphaBar.y - 2, 4.f, alphaBar.h + 4);
            renderer->DrawFilledRect(ind, Color(255, 255, 255, 240), 2.f);
            renderer->DrawOutlineRect(ind, Color(0, 0, 0, 120), 1.f, 2.f);
        }
    }
}
#pragma endregion color picker

#pragma region groupbox
void DefaultSkin::DrawGroupBox(IRenderer* renderer, const GroupBox& groupBox, const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius;

    renderer->DrawFilledRect(absRect, m_style->groupboxBodyBackground, cr);
    renderer->DrawOutlineRect(absRect, m_style->groupboxTitleOutline, 1.f, cr);

    if (groupBox.ShowTitleBar()) {
        Rect titleBounds = renderer->MeasureText(groupBox.GetTitle(), m_style->fontSize, m_style->fontFamily.c_str());
        float pillW = titleBounds.w + 20.f;
        float pillH = 22.f;
        Rect pillRect(absRect.x + 14.f, absRect.y, pillW, pillH);
        renderer->DrawFilledRect(pillRect, m_style->groupboxTitleBackground, cr * 0.6f);
        renderer->DrawOutlineRect(pillRect, m_style->groupboxTitleOutline, 1.f, cr * 0.6f);
        renderer->DrawText(groupBox.GetTitle(), pillRect, m_style->groupboxTitleText,
            m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
    }
}
#pragma endregion groupbox

#pragma region slider
void DefaultSkin::DrawSlider(IRenderer* renderer, const Slider& slider, const Rect& absRect)
{
    if (!m_style) return;

    float trackH = m_style->sliderTrackHeight;
    float thumbR = m_style->sliderThumbRadius;
    float cr = trackH * 0.5f;

    Slider::SliderGeometry g = slider.ComputeGeometry(trackH, thumbR);

    renderer->DrawFilledRect(g.trackRect, m_style->sliderTrack, cr);

    if (g.fillRect.w > 0.f)
        renderer->DrawFilledRect(g.fillRect, m_style->sliderFill, cr);

    const AnimState& a = slider.Anim();
    float hoverGlow = a.hover.Value();
    float glowRadius = thumbR + 2.f + 3.f * hoverGlow;
    uint8_t glowAlpha = (uint8_t)(40 + 30.f * hoverGlow);
    renderer->DrawCircle(g.thumbCX, g.thumbCY, glowRadius,
        Color(m_style->accent.r, m_style->accent.g, m_style->accent.b, glowAlpha));
    renderer->DrawCircle(g.thumbCX, g.thumbCY, thumbR, m_style->sliderThumb);

    float pressT = a.press.Value();
    if (pressT > 0.01f) {
        std::wstring valStr = slider.GetFormattedValue();
        float fontSize = m_style->fontSize - 1;
        std::wstring fontFamily = m_style->fontFamily;
        float thumbCX = g.thumbCX;
        float thumbCY = g.thumbCY;

        const_cast<Slider&>(slider).QueueOverlayDraw(
            [valStr, fontSize, fontFamily, thumbCX, thumbCY, thumbR, pressT]
            (IRenderer* r)
            {
                Rect textSize = r->MeasureText(valStr, fontSize, fontFamily.c_str());
                float pillW = textSize.w + 12.f;
                float pillH = textSize.h + 6.f;
                float pillX = thumbCX - pillW * 0.5f;
                float pillY = thumbCY - thumbR - pillH - 6.f;
                float pillCR = 4.f;

                uint8_t bgAlpha = (uint8_t)(220.f * pressT);
                uint8_t textAlpha = (uint8_t)(255.f * pressT);

                Rect pillRect(pillX, pillY, pillW, pillH);
                r->DrawFilledRect(pillRect, Color(30, 30, 40, bgAlpha), pillCR);
                r->DrawOutlineRect(pillRect, Color(255, 255, 255, (uint8_t)(30.f * pressT)), 1.f, pillCR);
                r->DrawText(valStr, pillRect, Color(255, 255, 255, textAlpha),
                    fontSize, TextAnchor::Center, fontFamily.c_str());
            });
    }
}
#pragma endregion slider

#pragma region tab control
void DefaultSkin::DrawTabControl(IRenderer* renderer, const TabControl& tabControl, const Rect& absRect)
{
    if (!m_style) return;

    renderer->DrawFilledRect(absRect, m_style->tabControlBackground, m_style->cornerRadius);

    int totalStrip = tabControl.GetTotalStripArea();
    if (tabControl.GetOrientation() == TabOrientation::Horizontal) {
        float sy = absRect.y + (float)totalStrip;
        renderer->DrawLine(absRect.x + 8.f, sy, absRect.x + absRect.w - 8.f, sy,
            Color(255, 255, 255, 15), 1.f);
    }
    else {
        float sx = absRect.x + (float)totalStrip;
        renderer->DrawLine(sx, absRect.y + 8.f, sx, absRect.y + absRect.h - 8.f,
            Color(255, 255, 255, 15), 1.f);
    }
}

void DefaultSkin::DrawTabPageBody(IRenderer* renderer, const TabPage& tabPage, const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius * 0.5f;

    // Subtle semi-transparent panel background
    renderer->DrawFilledRect(absRect, Color(255, 255, 255, 8), cr);

    // Thin inner border for definition
    renderer->DrawOutlineRect(absRect, Color(255, 255, 255, 12), 1.f, cr);
}

void DefaultSkin::DrawNumericInput(IRenderer* renderer, const NumericInput& input, const Rect& absRect)
{
    Skin::DrawNumericInput(renderer, input, absRect);
}
#pragma endregion tab control

#pragma region tab button
void DefaultSkin::DrawTabButton(IRenderer* renderer, const TabPage& tabPage,
    const Rect& absRect, bool active)
{
    if (!m_style) return;
    const AnimState& a = tabPage.Anim();
    float activeT = a.active.Value();
    float cr = m_style->cornerRadius * 0.6f;

    Color bg = Color::Lerp(m_style->tabNormalBackground, m_style->tabSelectedBackground, activeT);
    renderer->DrawFilledRect(absRect, bg, cr);

    if (activeT > 0.01f) {
        uint8_t borderAlpha = (uint8_t)(30.f * activeT);
        renderer->DrawOutlineRect(absRect,
            Color(m_style->accent.r, m_style->accent.g, m_style->accent.b, borderAlpha), 1.f, cr);

        float barScale = activeT;
        uint8_t indicatorAlpha = (uint8_t)(m_style->accent.a * activeT);
        Color indicatorColor(m_style->accent.r, m_style->accent.g, m_style->accent.b, indicatorAlpha);

        if (absRect.w >= absRect.h) {
            float barW = absRect.w * 0.6f * barScale;
            float barX = absRect.x + (absRect.w - barW) * 0.5f;
            renderer->DrawFilledRect(Rect(barX, absRect.y + absRect.h - 2.f, barW, 2.f), indicatorColor, 1.f);
        }
        else {
            float barH = absRect.h * 0.6f * barScale;
            float barY = absRect.y + (absRect.h - barH) * 0.5f;
            renderer->DrawFilledRect(Rect(absRect.x + absRect.w - 2.f, barY, 2.f, barH), indicatorColor, 1.f);
        }
    }

    Color textColor = Color::Lerp(m_style->tabText, m_style->tabSelectedText, activeT);

    TabPage::TabButtonLayout layout = tabPage.ComputeTabButtonLayout(
        absRect, renderer, m_style->fontSize, m_style->fontFamily.c_str());

    if (layout.hasIcon) {
        tabPage.GetIconRef().Draw(renderer, layout.iconRect);
    }

    if (layout.hasText) {
        renderer->DrawText(tabPage.GetTitle(), layout.textRect, textColor,
            m_style->fontSize, layout.textAnchor, m_style->fontFamily.c_str());
    }
}
#pragma endregion tab button

#pragma region combobox
void DefaultSkin::DrawComboBox(IRenderer* renderer, const ComboBox& combo, const Rect& absRect)
{
    if (!m_style) return;
    const AnimState& a = combo.Anim();
    float cr = m_style->cornerRadius * 0.6f;

    Color bg = Color::Lerp(m_style->comboBackground, m_style->comboHovered, a.hover.Value());
    renderer->DrawFilledRect(absRect, bg, cr);

    uint8_t borderAlpha = (uint8_t)(m_style->comboBorder.a + 15.f * a.hover.Value());
    renderer->DrawOutlineRect(absRect, Color(255, 255, 255, borderAlpha), 1.f, cr);

    float arrowW = m_style->comboArrowWidth;
    Rect textRect(absRect.x + 10.f, absRect.y, absRect.w - arrowW - 2.f, absRect.h);
    std::wstring displayText = combo.GetDisplayText(renderer);
    renderer->DrawText(displayText, textRect, m_style->comboText,
        m_style->fontSize, TextAnchor::CenterLeft, m_style->fontFamily.c_str());

    float arrowX = absRect.x + absRect.w - arrowW * 0.65f;
    float arrowCY = absRect.y + absRect.h * 0.5f;
    float arrowSize = 4.f;
    Color arrowColor = m_style->comboArrow;
    if (combo.IsOpen()) {
        renderer->DrawLine(arrowX - arrowSize, arrowCY + 2.f, arrowX, arrowCY - 2.f, arrowColor, 1.5f);
        renderer->DrawLine(arrowX, arrowCY - 2.f, arrowX + arrowSize, arrowCY + 2.f, arrowColor, 1.5f);
    }
    else {
        renderer->DrawLine(arrowX - arrowSize, arrowCY - 2.f, arrowX, arrowCY + 2.f, arrowColor, 1.5f);
        renderer->DrawLine(arrowX, arrowCY + 2.f, arrowX + arrowSize, arrowCY - 2.f, arrowColor, 1.5f);
    }
}
#pragma endregion combobox

#pragma region combobox dropdown
void DefaultSkin::DrawComboBoxDropdown(IRenderer* renderer, const ComboBox& combo)
{
    if (!m_style) return;

    Rect dropRect = combo.GetDropdownRect();
    float dropCR = m_style->cornerRadius * 0.5f;

    renderer->DrawGlassRect(dropRect, m_style->glassBlurRadius * 0.6f,
        m_style->comboDropBackground, dropCR, 0.f);
    renderer->DrawOutlineRect(dropRect, m_style->comboDropBorder, 1.f, dropCR);

    renderer->SetScissor(dropRect);

    bool isMulti = combo.GetMode() == ComboBoxMode::MultiSelect;
    int itemCount = combo.GetItemCount();
    float itemCR = dropCR * 0.5f;

    for (int i = 0; i < itemCount; ++i)
    {
        Rect itemRect = combo.GetDropdownItemRect(i);
        if (itemRect.y + itemRect.h < dropRect.y || itemRect.y > dropRect.y + dropRect.h)
            continue;

        const AnimState& ia = combo.ItemAnim(i);
        float hoverT = ia.hover.Value();
        float checkT = ia.checked.Value();

        DrawComboBoxItem(renderer, itemRect, combo.GetItems()[i],
            hoverT, checkT, isMulti, itemCR);
    }

    renderer->ResetScissor();

    if (combo.NeedsScrollbar()) {
        Rect track = combo.GetDropdownTrackRect();
        Rect thumb = combo.GetDropdownThumbRect();
        bool thumbHovered = combo.GetDropdownScrollbar().IsThumbHovered();
        DrawScrollbar(renderer, track, thumb, true, thumbHovered);
    }
}
#pragma endregion combobox dropdown

#pragma region combobox item
void DefaultSkin::DrawComboBoxItem(IRenderer* renderer, const Rect& itemRect,
    const std::wstring& text, float hoverT, float checkT,
    bool isMultiSelect, float cornerRadius)
{
    if (!m_style) return;

    Color itemBg = m_style->comboItemNormal;
    itemBg = Color::Lerp(itemBg, m_style->comboItemHovered, hoverT);
    itemBg = Color::Lerp(itemBg, m_style->comboItemSelected, checkT);

    if (itemBg.a > 0)
        renderer->DrawFilledRect(itemRect, itemBg, cornerRadius);

    float textOffsetX = 0.f;

    if (isMultiSelect) {
        float boxSize = 14.f;
        float boxCR = 3.f;
        float boxX = itemRect.x + 6.f;
        float boxY = itemRect.y + (itemRect.h - boxSize) * 0.5f;
        Rect boxRect(boxX, boxY, boxSize, boxSize);

        Color boxBg = Color::Lerp(m_style->checkboxBoxNormal, m_style->comboCheckMark, checkT);
        renderer->DrawFilledRect(boxRect, boxBg, boxCR);

        if (checkT < 0.99f) {
            uint8_t bAlpha = (uint8_t)(25.f * (1.f - checkT));
            renderer->DrawOutlineRect(boxRect, Color(255, 255, 255, bAlpha), 1.f, boxCR);
        }

        DrawCheckMark(renderer, boxRect, checkT, Color(255, 255, 255, 255), 1.5f);

        textOffsetX = boxSize + 10.f;
    }
    else {
        if (checkT > 0.01f) {
            float dotR = 3.f * checkT;
            float dotX = itemRect.x + 10.f;
            float dotY = itemRect.y + itemRect.h * 0.5f;
            renderer->DrawCircle(dotX, dotY, dotR,
                Color(m_style->accent.r, m_style->accent.g, m_style->accent.b,
                    (uint8_t)(255.f * checkT)));
            textOffsetX = 20.f;
        }
        else {
            textOffsetX = 8.f;
        }
    }

    Color itemTextColor = Color::Lerp(m_style->comboItemText, m_style->comboItemTextSelected, checkT);
    Rect itemTextRect(itemRect.x + textOffsetX, itemRect.y,
        itemRect.w - textOffsetX - 4.f, itemRect.h);
    renderer->DrawText(text, itemTextRect, itemTextColor,
        m_style->fontSize, TextAnchor::CenterLeft, m_style->fontFamily.c_str());
}
#pragma endregion combobox item

#pragma region text input
void DefaultSkin::DrawTextInput(IRenderer* renderer, const TextInputVisuals& vis)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius * 0.5f;

    Color bg = m_style->inputBackground;
    bg = Color::Lerp(bg, m_style->inputHovered, vis.hoverT);
    bg = Color::Lerp(bg, m_style->inputFocused, vis.focusT);
    renderer->DrawFilledRect(vis.absRect, bg, cr);

    Color border = Color::Lerp(m_style->inputBorder, m_style->inputBorderFocused, vis.focusT);
    renderer->DrawOutlineRect(vis.absRect, border, 1.f, cr);

    renderer->SetScissor(vis.contentRect);

    if (vis.showPlaceholder) {
        renderer->DrawText(vis.placeholderText, vis.contentRect,
            m_style->inputPlaceholder, m_style->fontSize,
            vis.placeholderAnchor, m_style->fontFamily.c_str());
    }
    else {
        for (const auto& sel : vis.selectionRects)
            renderer->DrawFilledRect(sel, m_style->inputSelection, 2.f);

        for (const auto& line : vis.lines)
            renderer->DrawText(line.text, line.rect, m_style->inputText,
                m_style->fontSize, TextAnchor::CenterLeft,
                m_style->fontFamily.c_str());

        if (vis.showCursor)
            renderer->DrawFilledRect(vis.cursorRect, m_style->inputCursor);
    }

    renderer->ResetScissor();

    if (vis.showScrollbar)
        DrawScrollbar(renderer, vis.scrollTrackRect, vis.scrollThumbRect, true, false);
}
#pragma endregion text input

#pragma region keybind
void DefaultSkin::DrawKeyBind(IRenderer* renderer, const KeyBind& keybind, const Rect& absRect)
{
    if (!m_style) return;
    const AnimState& a = keybind.Anim();
    float cr = m_style->cornerRadius * 0.5f;
    float hoverT = a.hover.Value();
    float glowT = a.glow.Value();

    Color bg = m_style->keybindBackground;
    bg = Color::Lerp(bg, m_style->keybindHovered, hoverT);
    bg = Color::Lerp(bg, m_style->keybindListening, glowT);
    renderer->DrawFilledRect(absRect, bg, cr);

    Color border = m_style->keybindBorder;
    if (glowT > 0.01f) {
        border = Color::Lerp(border,
            Color(m_style->accent.r, m_style->accent.g, m_style->accent.b, 150),
            glowT);
    }
    renderer->DrawOutlineRect(absRect, border, 1.f, cr);

    if (glowT > 0.01f) {
        uint8_t ga = (uint8_t)(100.f * glowT);
        Rect glowRect(absRect.x + 6.f, absRect.y + absRect.h - 2.f, absRect.w - 12.f, 2.f);
        renderer->DrawFilledRect(glowRect,
            Color(m_style->accent.r, m_style->accent.g, m_style->accent.b, ga), 1.f);
    }

    Color textColor = Color::Lerp(m_style->keybindText, m_style->keybindTextListening, glowT);
    Rect textRect(absRect.x + 10.f, absRect.y, absRect.w - 20.f, absRect.h);
    renderer->DrawText(keybind.GetDisplayText(), textRect, textColor,
        m_style->fontSize, TextAnchor::Center, m_style->fontFamily.c_str());
}
#pragma endregion keybind

#pragma region scrollbar
void DefaultSkin::DrawScrollbarTrack(IRenderer* renderer, const Rect& rect, bool vertical)
{
    if (!m_style) return;
    float cr = (float)m_style->scrollbarWidth * 0.5f;
    renderer->DrawFilledRect(rect, m_style->scrollbarTrack, cr);
}

void DefaultSkin::DrawScrollbarThumb(IRenderer* renderer, const Rect& rect, bool vertical, bool hovered)
{
    if (!m_style) return;
    float cr = (float)m_style->scrollbarWidth * 0.5f;
    Color color = hovered ? m_style->scrollbarThumbHover : m_style->scrollbarThumb;
    float inset = 2.f;
    Rect thumbRect = vertical
        ? Rect(rect.x + inset, rect.y, rect.w - inset * 2.f, rect.h)
        : Rect(rect.x, rect.y + inset, rect.w, rect.h - inset * 2.f);
    renderer->DrawFilledRect(thumbRect, color, cr - inset);
}

void DefaultSkin::DrawScrollbar(IRenderer* renderer, const Rect& trackRect, const Rect& thumbRect,
    bool vertical, bool thumbHovered)
{
    DrawScrollbarTrack(renderer, trackRect, vertical);
    DrawScrollbarThumb(renderer, thumbRect, vertical, thumbHovered);
}
#pragma endregion scrollbar

#pragma region listbox
void DefaultSkin::DrawListBox(IRenderer* renderer, const ListBox& listbox, const Rect& absRect)
{
    if (!m_style) return;
    float cr = m_style->cornerRadius * 0.5f;

    renderer->DrawFilledRect(absRect, m_style->listboxBackground, cr);
    renderer->DrawOutlineRect(absRect, m_style->listboxBorder, 1.f, cr);

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
    float itemCR = cr * 0.5f;
    float itemMargin = 2.f;

    for (int i = 0; i < itemCount; ++i)
    {
        float itemY = (float)client.y + i * itemH - scrollY;

        if (itemY + itemH < client.y || itemY > client.y + client.h)
            continue;

        Rect itemRect(client.x + itemMargin, itemY + 1.f,
            contentW - itemMargin * 2.f, itemH - 2.f);

        const AnimState& ia = listbox.ItemAnim(i);
        float hoverT = ia.hover.Value();
        float checkT = ia.checked.Value();

        DrawListBoxItem(renderer, itemRect, listbox.GetItems()[i],
            hoverT, checkT, isMulti, itemCR);
    }

    renderer->ResetScissor();
}
#pragma endregion listbox

#pragma region listbox item
void DefaultSkin::DrawListBoxItem(IRenderer* renderer, const Rect& itemRect,
    const std::wstring& text, float hoverT, float checkT,
    bool isMultiSelect, float cornerRadius)
{
    if (!m_style) return;

    Color itemBg = m_style->listboxItemNormal;
    itemBg = Color::Lerp(itemBg, m_style->listboxItemHovered, hoverT);
    itemBg = Color::Lerp(itemBg, m_style->listboxItemSelected, checkT);
    renderer->DrawFilledRect(itemRect, itemBg, cornerRadius);

    if (checkT > 0.01f) {
        uint8_t bAlpha = (uint8_t)(40.f * checkT);
        renderer->DrawOutlineRect(itemRect,
            Color(m_style->accent.r, m_style->accent.g, m_style->accent.b, bAlpha),
            1.f, cornerRadius);
    }

    if (hoverT > 0.01f && checkT < 0.5f) {
        uint8_t hAlpha = (uint8_t)(20.f * hoverT);
        renderer->DrawOutlineRect(itemRect, Color(255, 255, 255, hAlpha), 1.f, cornerRadius);
    }

    float textOffsetX = 10.f;
    if (isMultiSelect) {
        float boxSize = 14.f;
        float boxCR = 3.f;
        float boxX = itemRect.x + 8.f;
        float boxY = itemRect.y + (itemRect.h - boxSize) * 0.5f;
        Rect boxRect(boxX, boxY, boxSize, boxSize);

        Color boxBg = Color::Lerp(m_style->checkboxBoxNormal, m_style->accent, checkT);
        renderer->DrawFilledRect(boxRect, boxBg, boxCR);

        if (checkT < 0.99f) {
            uint8_t bAlpha = (uint8_t)(25.f * (1.f - checkT));
            renderer->DrawOutlineRect(boxRect, Color(255, 255, 255, bAlpha), 1.f, boxCR);
        }

        DrawCheckMark(renderer, boxRect, checkT, Color(255, 255, 255, 255), 1.5f);

        textOffsetX = boxSize + 16.f;
    }

    Color textColor = Color::Lerp(m_style->listboxItemText,
        m_style->listboxItemTextSelected, checkT);
    Rect textRect(itemRect.x + textOffsetX, itemRect.y,
        itemRect.w - textOffsetX - 4.f, itemRect.h);
    renderer->DrawText(text, textRect, textColor,
        m_style->fontSize, TextAnchor::CenterLeft, m_style->fontFamily.c_str());
}
#pragma endregion listbox item

#pragma region animation events
void DefaultSkin::OnButtonPressed(Button& button)
{
    if (!m_style) return;
    const AnimConfig& ac = m_style->anim;
    button.Anim().press.easing = ac.pressEasing;
    button.Anim().press.TransitionTo(1.f, ac.pressIn);
    button.Anim().glow.easing = ac.glowEasing;
    button.Anim().glow.TransitionTo(1.f, ac.glowIn);
}

void DefaultSkin::OnButtonReleased(Button& button)
{
    if (!m_style) return;
    const AnimConfig& ac = m_style->anim;
    button.Anim().press.easing = ac.pressEasing;
    button.Anim().press.TransitionTo(0.f, ac.pressOut);
    button.Anim().glow.easing = ac.glowEasing;
    button.Anim().glow.TransitionTo(0.f, ac.glowOut);
}

void DefaultSkin::OnCheckboxToggled(Checkbox& checkbox, bool nowChecked)
{
    if (!m_style) return;
    const AnimConfig& ac = m_style->anim;
    checkbox.Anim().checked.easing = ac.checkEasing;
    checkbox.Anim().checked.TransitionTo(nowChecked ? 1.f : 0.f, ac.checkToggle);
    checkbox.Anim().glow.easing = ac.glowEasing;
    checkbox.Anim().glow.Set(1.f);
    checkbox.Anim().glow.TransitionTo(0.f, ac.glowOut);
}
#pragma endregion animation events