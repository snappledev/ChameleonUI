#include "../../include/elements/label.h"
#include "../../include/skins/skin.h"

#pragma region lifecycle
Label::Label() = default;
Label::Label(const std::wstring& text) : m_text(text) {}

const std::wstring& Label::GetText() const { return m_text; }
void Label::SetText(const std::wstring& text) { m_text = text; m_needFirstTextMeasure = true; InvalidateMeasure(); }
#pragma endregion

#pragma region drawing
void Label::Draw(IRenderer* renderer)
{
    if (!m_cachedRenderer)
        m_cachedRenderer = renderer;

    if (m_needFirstTextMeasure && m_cachedRenderer)
    {
        Skin* skin = GetEffectiveSkin();
        if (skin && skin->GetStyle() && !m_text.empty())
        {
            Rect tb = m_cachedRenderer->MeasureText(m_text,
                skin->GetStyle()->fontSize, skin->GetStyle()->fontFamily.c_str());
            m_cachedTextWidth = tb.w;
        }
        else
        {
            m_cachedTextWidth = 0.f;
        }
        m_needFirstTextMeasure = false;
    }

    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawLabel(renderer, *this, GetAbsoluteRect());
}
#pragma endregion

#pragma region measure & arrange
void Label::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    float textW = 0.f, textH = 0.f;
    if (m_cachedRenderer)
    {
        Skin* skin = GetEffectiveSkin();
        if (skin && skin->GetStyle() && !m_text.empty())
        {
            Rect tb = m_cachedRenderer->MeasureText(m_text,
                skin->GetStyle()->fontSize, skin->GetStyle()->fontFamily.c_str());
            textW = tb.w;
            textH = tb.h;
            m_cachedTextWidth = textW;
        }
    }

    switch (m_widthPolicy)
    {
    case SizePolicy::ContentFit:
        m_desiredSize.width = (textW > 0) ? textW : (m_width > 0 ? (float)m_width : 60.f);
        break;
    case SizePolicy::Fixed:
        m_desiredSize.width = (m_width > 0) ? (float)m_width : 200.f;
        break;
    case SizePolicy::Stretch:
    default:
        m_desiredSize.width = (m_width > 0) ? (float)m_width : availableSize.width;
        break;
    }

    m_desiredSize.height = (m_height > 0) ? (float)m_height : ((textH > 0) ? textH + 4.f : 20.f);
    m_needMeasure = false;
}
#pragma endregion

#pragma region input
UIElement* Label::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;

    Rect myRect = GetAbsoluteRect();

    if (absY < myRect.y || absY >= myRect.y + myRect.h)
        return nullptr;

    float hitWidth = (m_cachedTextWidth > 0.f) ? m_cachedTextWidth : myRect.w;
    if (hitWidth > myRect.w) hitWidth = myRect.w;

    if (absX < myRect.x || absX >= myRect.x + hitWidth)
        return nullptr;

    return this;
}
#pragma endregion