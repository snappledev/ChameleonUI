#include "../../include/elements/colorpickermodal.h"
#include "../../include/elements/colorpicker.h"
#include "../../include/elements/colorswatch.h"
#include "../../include/skins/skin.h"
#include "../../include/core/UIContext.h"
#include <algorithm>
#include <cmath>

#pragma region lifecycle
ColorPickerModal::ColorPickerModal()
{
    SetVisible(false);
    SetAutoLayoutVertical(false);

    auto& picker = AddChild<ColorPicker>(0.f, 0.f);
    m_picker = &picker;
    m_picker->SetShowAlphaBar(true);
}

void ColorPickerModal::Open(const Color& initialColor)
{
    m_previousColor = initialColor;
    m_picker->SetColor(initialColor);
    m_open = true;
    SetVisible(true);

    if (s_layoutContext)
        s_layoutContext->AddPersistentOverlay(this);
}

void ColorPickerModal::Close()
{
    m_open = false;
    SetVisible(false);

    if (s_layoutContext)
        s_layoutContext->RemovePersistentOverlay(this);

    ReleaseInput();
}

void ColorPickerModal::Apply()
{
    Color result = m_picker->GetCurrentColor();
    if (m_ownerSwatch)
        m_ownerSwatch->SetColor(result);
    if (m_onApply)
        m_onApply(result);
    Close();
}

void ColorPickerModal::Cancel()
{
    if (m_onCancel)
        m_onCancel();
    Close();
}
#pragma endregion

#pragma region drawing
void ColorPickerModal::Draw(IRenderer* renderer)
{
    if (!m_open) return;

    UpdateAnchorPosition();

    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawColorPickerModal(renderer, *this, GetAbsoluteRect());
}
#pragma endregion

#pragma region measure & arrange
void ColorPickerModal::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;
    m_desiredSize.width = m_modalWidth;
    m_desiredSize.height = m_modalHeight;
    m_needMeasure = false;
}

void ColorPickerModal::Arrange(const Rect& finalRect)
{
    UIElement::Arrange(finalRect);
    ComputeModalRects();
}

void ColorPickerModal::UpdateAnchorPosition()
{
    if (!m_ownerSwatch) return;

    Rect swatchAbs = m_ownerSwatch->GetAbsoluteRect();
    float modalX = swatchAbs.x;
    float modalY = swatchAbs.y + swatchAbs.h + 4.f;

    m_x = (int)modalX;
    m_y = (int)modalY;
    m_arrangedWidth = (int)m_modalWidth;
    m_arrangedHeight = (int)m_modalHeight;
    m_lastArrangedRect = Rect(modalX, modalY, m_modalWidth, m_modalHeight);
    ComputeModalRects();
}

void ColorPickerModal::ComputeModalRects()
{
    Rect abs = GetAbsoluteRect();
    float pad = 12.f;
    float bottomAreaH = 50.f;

    float pickerH = abs.h - pad * 2.f - bottomAreaH;
    float pickerW = abs.w - pad * 2.f;
    Rect pickerRect(abs.x + pad, abs.y + pad, pickerW, pickerH);

    if (m_picker)
    {
        m_picker->Arrange(pickerRect);
    }

    float bottomY = abs.y + pad + pickerH + 12.f;
    float previewW = 32.f;
    float previewH = 24.f;
    m_prevColorRect = Rect(abs.x + pad, bottomY, previewW, previewH);
    m_currColorRect = Rect(m_prevColorRect.x + previewW + 4.f, bottomY, previewW, previewH);

    float btnW = 54.f;
    float btnH = 24.f;
    m_cancelRect = Rect(abs.x + abs.w - pad - btnW, bottomY, btnW, btnH);
    m_applyRect = Rect(m_cancelRect.x - btnW - 6.f, bottomY, btnW, btnH);
}
#pragma endregion

#pragma region input
UIElement* ColorPickerModal::HitTest(int absX, int absY)
{
    if (!m_visible || !m_open) return nullptr;
    Rect abs = GetAbsoluteRect();
    if (abs.Contains(absX, absY))
        return this;
    return nullptr;
}

void ColorPickerModal::OnMouseDown(int button)
{
    if (button != 0) return;

    extern int g_lastMouseX, g_lastMouseY;
    float mx = (float)g_lastMouseX;
    float my = (float)g_lastMouseY;

    Rect pickerRect = m_picker->GetAbsoluteRect();
    if (pickerRect.Contains(mx, my))
    {
        m_picker->OnMouseDown(button);
        return;
    }

    if (m_applyRect.Contains(mx, my))
    {
        Apply();
    }
    else if (m_cancelRect.Contains(mx, my))
    {
        Cancel();
    }
    else
    {
        Rect abs = GetAbsoluteRect();
        if (!abs.Contains(mx, my))
            Cancel();
    }
}

void ColorPickerModal::OnMouseUp(int button)
{
    m_picker->OnMouseUp(button);
}

void ColorPickerModal::OnMouseMove(int x, int y)
{
    float mx = (float)x;
    float my = (float)y;

    m_applyHovered = m_applyRect.Contains(mx, my);
    m_cancelHovered = m_cancelRect.Contains(mx, my);

    m_picker->OnMouseMove(x, y);
}
#pragma endregion

#pragma region update
void ColorPickerModal::Update(float dt)
{
    m_anim.Tick(dt);
}
#pragma endregion

#pragma region hsv utilities
Color ColorPickerModal::HSVToRGB(float h, float s, float v, uint8_t a)
{
    float c = v * s;
    float x = c * (1.f - std::fabs(std::fmod(h * 6.f, 2.f) - 1.f));
    float m = v - c;
    float r, g, b;

    int sector = (int)(h * 6.f) % 6;
    switch (sector)
    {
    case 0: r = c; g = x; b = 0; break;
    case 1: r = x; g = c; b = 0; break;
    case 2: r = 0; g = c; b = x; break;
    case 3: r = 0; g = x; b = c; break;
    case 4: r = x; g = 0; b = c; break;
    default: r = c; g = 0; b = x; break;
    }

    return Color((uint8_t)((r + m) * 255.f), (uint8_t)((g + m) * 255.f),
        (uint8_t)((b + m) * 255.f), a);
}

void ColorPickerModal::RGBToHSV(const Color& c, float& h, float& s, float& v)
{
    float r = c.r / 255.f, g = c.g / 255.f, b = c.b / 255.f;
    float mx = (std::max)({ r, g, b });
    float mn = (std::min)({ r, g, b });
    float d = mx - mn;

    v = mx;
    s = (mx > 0.f) ? d / mx : 0.f;

    if (d < 1e-6f) { h = 0.f; return; }

    if (mx == r)      h = (g - b) / d + (g < b ? 6.f : 0.f);
    else if (mx == g)  h = (b - r) / d + 2.f;
    else               h = (r - g) / d + 4.f;
    h /= 6.f;
}

Color ColorPickerModal::GetCurrentColor() const { return m_picker->GetCurrentColor(); }
float ColorPickerModal::GetHue() const { return m_picker->GetHue(); }
float ColorPickerModal::GetSaturation() const { return m_picker->GetSaturation(); }
float ColorPickerModal::GetBrightness() const { return m_picker->GetBrightness(); }
bool ColorPickerModal::IsDraggingGradient() const { return m_picker->IsDraggingGradient(); }
bool ColorPickerModal::IsDraggingHueBar() const { return m_picker->IsDraggingHueBar(); }
Rect ColorPickerModal::GetGradientRect() const { return m_picker->GetGradientRect(); }
Rect ColorPickerModal::GetHueBarRect() const { return m_picker->GetHueBarRect(); }
#pragma endregion