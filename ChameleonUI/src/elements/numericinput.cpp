#include "../../include/elements/numericinput.h"
#include "../../include/skins/skin.h"
#include <cstdio>
#include <algorithm>
#include <cmath>

#pragma region lifecycle
NumericInput::NumericInput(float min, float max, float value)
    : m_min(min), m_max(max), m_value(std::clamp(value, min, max))
{
    m_widthPolicy = SizePolicy::Stretch;
    m_heightPolicy = SizePolicy::ContentFit;

    if (std::floor(min) == min && std::floor(max) == max && (max - min) >= 2.f)
        m_integerMode = true;
}

void NumericInput::SetValue(float value)
{
    value = std::clamp(value, m_min, m_max);
    if (m_integerMode)
        value = std::round(value);
    if (value == m_value) return;
    m_value = value;
    if (m_linkedFloat) *m_linkedFloat = m_value;
    if (m_onValueChanged) m_onValueChanged(m_value);
}

std::wstring NumericInput::GetDisplayText() const
{
    wchar_t buf[64];
    if (m_integerMode)
        swprintf_s(buf, L"%d", (int)m_value);
    else
        swprintf_s(buf, L"%.*f", m_precision, m_value);
    return buf;
}

NumericInput::NumericGeometry NumericInput::ComputeGeometry() const
{
    Rect abs = GetAbsoluteRect();
    NumericGeometry g;
    g.decrementRect = Rect(abs.x, abs.y, m_buttonWidth, abs.h);
    g.incrementRect = Rect(abs.x + abs.w - m_buttonWidth, abs.y, m_buttonWidth, abs.h);
    g.fieldRect = Rect(abs.x + m_buttonWidth, abs.y, abs.w - m_buttonWidth * 2.f, abs.h);
    return g;
}

void NumericInput::Increment() { SetValue(m_value + m_step); }
void NumericInput::Decrement() { SetValue(m_value - m_step); }

void NumericInput::CommitEdit()
{
    if (!m_editing) return;
    m_editing = false;
    try {
        float parsed = std::stof(m_editBuffer);
        SetValue(parsed);
    }
    catch (...) {
    }
}

void NumericInput::CancelEdit()
{
    m_editing = false;
}

void NumericInput::SyncFromBinding()
{
    if (m_linkedFloat)
        SetValue(*m_linkedFloat);
}
#pragma endregion

#pragma region measure & arrange
void NumericInput::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;
    m_desiredSize.width = (m_width > 0) ? (float)m_width : 140.f;
    m_desiredSize.height = (m_height > 0) ? (float)m_height : 28.f;
    m_needMeasure = false;
}
#pragma endregion

#pragma region update
void NumericInput::Update(float dt)
{
    Skin* skin = GetEffectiveSkin();

    bool wasHovered = m_hovered;
    m_anim.Tick(dt);

    UIElement::Update(dt);
}
#pragma endregion

#pragma region drawing
void NumericInput::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawNumericInput(renderer, *this, GetAbsoluteRect());
}
#pragma endregion

#pragma region input
UIElement* NumericInput::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;
    Rect abs = GetAbsoluteRect();
    if (abs.Contains(absX, absY)) return this;
    return nullptr;
}

void NumericInput::OnMouseEnter()
{
    m_hovered = true;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnNumericInputHoverEnter(*this);
}

void NumericInput::OnMouseLeave()
{
    m_hovered = false;
    m_incHovered = false;
    m_decHovered = false;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnNumericInputHoverLeave(*this);
}

void NumericInput::OnMouseMove(int x, int y)
{
    auto g = ComputeGeometry();
    m_incHovered = g.incrementRect.Contains(x, y);
    m_decHovered = g.decrementRect.Contains(x, y);
}

void NumericInput::OnMouseDown(int button)
{
    if (button != 0) return;
    auto g = ComputeGeometry();
    extern int g_lastMouseX, g_lastMouseY;

    if (g.incrementRect.Contains(g_lastMouseX, g_lastMouseY))
        m_incPressed = true;
    else if (g.decrementRect.Contains(g_lastMouseX, g_lastMouseY))
        m_decPressed = true;
    else if (g.fieldRect.Contains(g_lastMouseX, g_lastMouseY))
    {
        if (!m_editing)
        {
            m_editing = true;
            m_editBuffer = GetDisplayText();
            RequestFocus();
        }
    }
}

void NumericInput::OnMouseUp(int button)
{
    if (button != 0) return;
    m_incPressed = false;
    m_decPressed = false;
}

void NumericInput::OnClick()
{
    extern int g_lastMouseX, g_lastMouseY;
    auto g = ComputeGeometry();

    if (g.incrementRect.Contains(g_lastMouseX, g_lastMouseY))
        Increment();
    else if (g.decrementRect.Contains(g_lastMouseX, g_lastMouseY))
        Decrement();
}

void NumericInput::OnFocusGained()
{
    if (!m_editing)
    {
        m_editing = true;
        m_editBuffer = GetDisplayText();
    }
}

void NumericInput::OnFocusLost()
{
    CommitEdit();
}

void NumericInput::OnKeyDown(int vkCode)
{
    if (!m_editing) return;

    if (vkCode == VK_RETURN)
    {
        CommitEdit();
        ReleaseFocus();
    }
    else if (vkCode == VK_ESCAPE)
    {
        CancelEdit();
        ReleaseFocus();
    }
    else if (vkCode == VK_BACK && !m_editBuffer.empty())
    {
        m_editBuffer.pop_back();
    }
    else if (vkCode == VK_UP)
    {
        Increment();
        m_editBuffer = GetDisplayText();
    }
    else if (vkCode == VK_DOWN)
    {
        Decrement();
        m_editBuffer = GetDisplayText();
    }
}

void NumericInput::OnChar(wchar_t ch)
{
    if (!m_editing) return;
    if ((ch >= L'0' && ch <= L'9') || ch == L'-' || ch == L'.')
        m_editBuffer += ch;
}
#pragma endregion