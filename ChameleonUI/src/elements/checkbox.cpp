#include "../../include/elements/Checkbox.h"
#include "../../include/skins/skin.h"

#pragma region lifecycle
Checkbox::Checkbox() = default;
Checkbox::Checkbox(const std::wstring& text, bool checked)
    : m_text(text), m_checked(checked)
{
    m_anim.checked.Set(checked ? 1.f : 0.f);
}

const std::wstring& Checkbox::GetText() const { return m_text; }
void Checkbox::SetText(const std::wstring& text) { m_text = text; }

bool Checkbox::IsChecked() const { return m_checked; }
void Checkbox::SetChecked(bool checked) {
    m_checked = checked;
    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->OnCheckboxToggled(*this, m_checked);
}

bool Checkbox::IsHovered() const { return m_isHovered; }

float Checkbox::GetEffectiveBoxSize() const {
    if (m_boxSize > 0.f) return m_boxSize;
    Skin* skin = GetEffectiveSkin();
    if (skin && skin->GetStyle()) return skin->GetStyle()->checkboxBoxSize;
    return 18.f;
}

void Checkbox::SetLinkedBool(bool* linkedBool) { m_linkedBool = linkedBool; }
bool* Checkbox::GetLinkedBool() const { return m_linkedBool; }
#pragma endregion

#pragma region update
void Checkbox::SyncFromBinding()
{
    if (m_linkedBool)
        SetChecked(*m_linkedBool);
}
#pragma endregion

#pragma region drawing
void Checkbox::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawCheckbox(renderer, *this, GetAbsoluteRect());
}
#pragma endregion

#pragma region measure & arrange
void Checkbox::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    float boxSize = GetEffectiveBoxSize();

    switch (m_widthPolicy)
    {
    case SizePolicy::ContentFit:
        m_desiredSize.width = boxSize;
        break;
    case SizePolicy::Fixed:
        m_desiredSize.width = (m_width > 0) ? (float)m_width : boxSize;
        break;
    case SizePolicy::Stretch:
    default:
        m_desiredSize.width = (m_width > 0) ? (float)m_width : boxSize;
        break;
    }
    m_desiredSize.height = (m_height > 0) ? (float)m_height : boxSize;
    m_needMeasure = false;
}
#pragma endregion

#pragma region input
void Checkbox::OnMouseEnter()
{
    m_isHovered = true;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnCheckboxHoverEnter(*this);
}

void Checkbox::OnMouseLeave()
{
    m_isHovered = false;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnCheckboxHoverLeave(*this);
}

void Checkbox::OnClick()
{
    m_checked = !m_checked;
    if (m_linkedBool)
        *m_linkedBool = m_checked;

    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->OnCheckboxToggled(*this, m_checked);

    if (m_onClickCb)
        m_onClickCb();
}
#pragma endregion