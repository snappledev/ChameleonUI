#include "../../include/elements/button.h"
#include "../../include/skins/skin.h"

#pragma region lifecycle
Button::Button() = default;
Button::Button(const std::wstring& text) : m_text(text) {}

const std::wstring& Button::GetText() const { return m_text; }
void Button::SetText(const std::wstring& text) { m_text = text; InvalidateMeasure(); }

bool Button::IsHovered() const { return m_isHovered; }
bool Button::IsPressed() const { return m_isPressed; }
bool Button::WasClicked() const { return m_wasClicked; }
#pragma endregion

#pragma region update
void Button::Update(float dt)
{
    m_wasClicked = false;
    UIElement::Update(dt);
}
#pragma endregion

#pragma region drawing
void Button::Draw(IRenderer* renderer)
{
    m_cachedRenderer = renderer;

    Skin* skin = GetEffectiveSkin();
    if (skin && skin->GetStyle()) {
        Rect textBounds = renderer->MeasureText(m_text,
            skin->GetStyle()->fontSize, skin->GetStyle()->fontFamily.c_str());
        m_cachedTextWidth = textBounds.w;
        m_cachedTextHeight = textBounds.h;

        if (m_widthPolicy == SizePolicy::ContentFit && m_needFirstTextMeasure)
        {
            m_needFirstTextMeasure = false;
            InvalidateMeasure();
        }
    }

    if (skin)
        skin->DrawButton(renderer, *this, GetAbsoluteRect(), m_isHovered, m_isPressed);
}
#pragma endregion

#pragma region measure & arrange
void Button::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    Skin* skin = GetEffectiveSkin();
    UIStyle* style = (skin) ? skin->GetStyle() : nullptr;
    float minH = style ? style->buttonMinHeight : 30.f;

    // Try to measure text eagerly if we have a cached renderer
    if (m_widthPolicy == SizePolicy::ContentFit && m_cachedRenderer && style)
    {
        Rect textBounds = m_cachedRenderer->MeasureText(m_text,
            style->fontSize, style->fontFamily.c_str());
        m_cachedTextWidth = textBounds.w;
        m_cachedTextHeight = textBounds.h;
    }

    switch (m_widthPolicy)
    {
    case SizePolicy::ContentFit:
        if (m_cachedTextWidth > 0.f)
            m_desiredSize.width = m_cachedTextWidth + m_contentPadding.x + m_contentPadding.w;
        else
            m_desiredSize.width = 100.f; // fallback until first Draw
        break;
    case SizePolicy::Fixed:
        m_desiredSize.width = (m_width > 0) ? (float)m_width : 100.f;
        break;
    case SizePolicy::Stretch:
    default:
        m_desiredSize.width = (m_width > 0) ? (float)m_width : availableSize.width;
        break;
    }

    switch (m_heightPolicy)
    {
    case SizePolicy::ContentFit:
        if (m_cachedTextHeight > 0.f)
            m_desiredSize.height = m_cachedTextHeight + m_contentPadding.y + m_contentPadding.h;
        else
            m_desiredSize.height = minH;
        if (m_desiredSize.height < minH) m_desiredSize.height = minH;
        break;
    case SizePolicy::Fixed:
        m_desiredSize.height = (m_height > 0) ? (float)m_height : minH;
        break;
    case SizePolicy::Stretch:
    default:
        m_desiredSize.height = (m_height > 0) ? (float)m_height : minH;
        break;
    }

    if (m_desiredSize.width <= 0) m_desiredSize.width = 100;
    if (m_desiredSize.height <= 0) m_desiredSize.height = minH;
    m_needMeasure = false;
}
#pragma endregion

#pragma region input
void Button::OnMouseEnter()
{
    m_isHovered = true;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnButtonHoverEnter(*this);
}

void Button::OnMouseLeave()
{
    m_isHovered = false;
    m_isPressed = false;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnButtonHoverLeave(*this);
}

void Button::OnMouseDown(int button)
{
    if (button == 0) {
        m_isPressed = true;
        Skin* skin = GetEffectiveSkin();
        if (skin) skin->OnButtonPressed(*this);
    }
}

void Button::OnMouseUp(int button)
{
    if (button == 0) {
        m_isPressed = false;
        Skin* skin = GetEffectiveSkin();
        if (skin) skin->OnButtonReleased(*this);
    }
}

void Button::OnClick()
{
    m_wasClicked = true;
    if (m_onClick)
        m_onClick();
}
#pragma endregion