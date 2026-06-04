#include "../../include/elements/ComboBox.h"
#include "../../include/skins/skin.h"
#include "../../include/core/UIStyle.h"
#include <algorithm>
extern int g_lastMouseX, g_lastMouseY;

#pragma region lifecycle
ComboBox::ComboBox(const std::vector<std::wstring>& items, ComboBoxMode mode, int maxVisibleItems)
    : m_items(items), m_mode(mode), m_maxVisibleItems(maxVisibleItems)
{
    m_selected.resize(items.size(), false);
    if (m_mode == ComboBoxMode::SingleSelect && !items.empty()) {
        m_selectedIndex = 0;
        m_selected[0] = true;
    }
    RebuildItemAnims();
}

void ComboBox::SetItems(const std::vector<std::wstring>& items)
{
    m_items = items;
    m_selected.resize(items.size(), false);
    m_selectedIndex = -1;
    RebuildItemAnims();
}

void ComboBox::SetSelectedIndex(int index)
{
    if (index < 0 || index >= (int)m_items.size()) return;
    if (m_mode == ComboBoxMode::SingleSelect) {
        if (index == m_selectedIndex) return;

        Skin* skin = GetEffectiveSkin();

        if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_selected.size()) {
            m_selected[m_selectedIndex] = false;
            if (skin) skin->OnComboBoxItemCheckChanged(m_itemAnims[m_selectedIndex], false);
        }

        std::fill(m_selected.begin(), m_selected.end(), false);
        m_selected[index] = true;
        m_selectedIndex = index;
        if (skin) skin->OnComboBoxItemCheckChanged(m_itemAnims[index], true);

        SyncLinkedData();
    }
}

bool ComboBox::IsItemSelected(int index) const
{
    if (index < 0 || index >= (int)m_selected.size()) return false;
    return m_selected[index];
}

void ComboBox::SetItemSelected(int index, bool selected)
{
    if (index < 0 || index >= (int)m_selected.size()) return;
    m_selected[index] = selected;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnComboBoxItemCheckChanged(m_itemAnims[index], selected);
    SyncLinkedData();
}

std::vector<int> ComboBox::GetSelectedIndices() const
{
    std::vector<int> result;
    for (int i = 0; i < (int)m_selected.size(); ++i)
        if (m_selected[i]) result.push_back(i);
    return result;
}

std::wstring ComboBox::GetDisplayText(IRenderer* renderer) const
{
    if (m_mode == ComboBoxMode::SingleSelect) {
        if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_items.size())
            return m_items[m_selectedIndex];
        return L"Select...";
    }

    auto indices = GetSelectedIndices();
    if (indices.empty()) return L"Select...";

    std::wstring full;
    for (size_t i = 0; i < indices.size(); ++i) {
        if (i > 0) full += L" + ";
        full += m_items[indices[i]];
    }

    if (renderer) {
        Skin* skin = GetEffectiveSkin();
        int fontSize = 13;
        std::wstring fontCopy = L"Segoe UI";
        if (skin && skin->GetStyle()) {
            fontSize = skin->GetStyle()->fontSize;
            fontCopy = skin->GetStyle()->fontFamily;
        }

        float availW = (float)m_width - 32.f;
        Rect bounds = renderer->MeasureText(full, fontSize, fontCopy.c_str());
        if (bounds.w > availW && full.size() > 3) {
            size_t lo = 1, hi = full.size();
            while (lo < hi) {
                size_t mid = (lo + hi + 1) / 2;
                std::wstring test = full.substr(0, mid) + L"...";
                Rect tb = renderer->MeasureText(test, fontSize, fontCopy.c_str());
                if (tb.w <= availW) lo = mid;
                else hi = mid - 1;
            }
            full = full.substr(0, lo) + L"...";
        }
    }
    return full;
}

void ComboBox::Open()
{
    if (m_open) return;
    m_open = true;

    float contentH = GetTotalContentHeight();
    int visCount = (std::min)(m_maxVisibleItems, (int)m_items.size());
    float viewportH = visCount * m_itemHeight + 4.f;

    m_dropdownScroll.SetScrollOffset(0);
    m_dropdownScroll.SetContentSize(contentH);
    m_dropdownScroll.SetViewportSize(viewportH);
    m_dropdownScroll.SetShown(NeedsScrollbar());
    m_dropdownScroll.SetWidth(8.f);

    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnComboBoxOpened(*this);

    CaptureInput();
}

void ComboBox::Close()
{
    if (!m_open) return;
    m_open = false;
    m_hoveredItem = -1;
    m_dropdownScroll.HandleMouseUp();

    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnComboBoxClosed(*this);

    ReleaseInput();
}

void ComboBox::SyncFromBinding()
{
    if (m_linkedInt)
        SetSelectedIndex(*m_linkedInt);
}

void ComboBox::Toggle() { m_open ? Close() : Open(); }

Rect ComboBox::GetDropdownRect() const
{
    Rect abs = GetAbsoluteRect();
    int visCount = (std::min)(m_maxVisibleItems, (int)m_items.size());
    float dropH = visCount * m_itemHeight + 4.f;
    float dropW = abs.w;
    return Rect(abs.x, abs.y + abs.h + 2.f, dropW, dropH);
}

Rect ComboBox::GetDropdownItemRect(int index) const
{
    Rect drop = GetDropdownRect();
    float sbWidth = NeedsScrollbar() ? 8.f : 0.f;
    float itemW = drop.w - sbWidth - 4.f;
    float itemY = drop.y + 2.f + index * m_itemHeight - m_dropdownScroll.GetScrollOffset();
    return Rect(drop.x + 2.f, itemY, itemW, m_itemHeight);
}

bool ComboBox::NeedsScrollbar() const
{
    return (int)m_items.size() > m_maxVisibleItems;
}

float ComboBox::GetTotalContentHeight() const
{
    return (float)m_items.size() * m_itemHeight;
}

Rect ComboBox::GetDropdownTrackRect() const
{
    Rect drop = GetDropdownRect();
    return Rect(drop.x + drop.w - 10.f, drop.y + 2.f, 8.f, drop.h - 4.f);
}

Rect ComboBox::GetDropdownThumbRect() const
{
    if (!NeedsScrollbar()) return Rect();
    Rect track = GetDropdownTrackRect();
    Skin* skin = GetEffectiveSkin();
    int minThumb = (skin && skin->GetStyle()) ? skin->GetStyle()->scrollbarMinThumbHeight : 20;
    return m_dropdownScroll.ComputeThumbRect(track, minThumb);
}

void ComboBox::RebuildItemAnims()
{
    m_itemAnims.resize(m_items.size());
    for (size_t i = 0; i < m_items.size(); ++i) {
        m_itemAnims[i] = AnimState{};
        m_itemAnims[i].checked.Set(m_selected[i] ? 1.f : 0.f);
    }
}

void ComboBox::SelectItem(int index)
{
    if (index < 0 || index >= (int)m_items.size()) return;
    Skin* skin = GetEffectiveSkin();

    if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_selected.size()) {
        m_selected[m_selectedIndex] = false;
        if (skin) skin->OnComboBoxItemCheckChanged(m_itemAnims[m_selectedIndex], false);
    }

    m_selectedIndex = index;
    m_selected[index] = true;
    if (skin) skin->OnComboBoxItemCheckChanged(m_itemAnims[index], true);

    SyncLinkedData();
    if (m_onChanged) m_onChanged();
    Close();
}

void ComboBox::ToggleItem(int index)
{
    if (index < 0 || index >= (int)m_items.size()) return;
    m_selected[index] = !m_selected[index];
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnComboBoxItemCheckChanged(m_itemAnims[index], m_selected[index]);
    SyncLinkedData();
    if (m_onChanged) m_onChanged();
}

void ComboBox::SyncLinkedData()
{
    if (m_linkedInt && m_mode == ComboBoxMode::SingleSelect)
        *m_linkedInt = m_selectedIndex;
    if (m_linkedBools && m_mode == ComboBoxMode::MultiSelect) {
        m_linkedBools->resize(m_selected.size());
        *m_linkedBools = m_selected;
    }
}
#pragma endregion

#pragma region update
void ComboBox::Update(float dt)
{
    m_anim.Tick(dt);
    for (auto& ia : m_itemAnims)
        ia.Tick(dt);
}
#pragma endregion

#pragma region measure & arrange
void ComboBox::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    Skin* skin = GetEffectiveSkin();
    UIStyle* style = skin ? skin->GetStyle() : nullptr;
    float minH = style ? style->comboMinHeight : 28.f;

    switch (m_widthPolicy)
    {
    case SizePolicy::Fixed:
        m_desiredSize.width = (m_width > 0) ? (float)m_width : 200.f;
        break;
    case SizePolicy::ContentFit:
    {
        float textW = 200.f;
        if (m_cachedRenderer && style) {
            std::wstring display = GetDisplayText(m_cachedRenderer);
            Rect tb = m_cachedRenderer->MeasureText(display,
                style->fontSize, style->fontFamily.c_str());
            textW = tb.w + 20.f;
        }
        float arrowW = style ? style->comboArrowWidth : 28.f;
        m_desiredSize.width = textW + arrowW;
        break;
    }
    case SizePolicy::Stretch:
    default:
        m_desiredSize.width = availableSize.width;
        break;
    }

    m_desiredSize.height = (m_height > 0) ? (float)m_height : minH;
    if (m_desiredSize.height < minH) m_desiredSize.height = minH;
    m_needMeasure = false;
}
#pragma endregion

#pragma region drawing
void ComboBox::Draw(IRenderer* renderer)
{
    m_cachedRenderer = renderer;
    Skin* skin = GetEffectiveSkin();
    if (!skin) return;

    skin->DrawComboBox(renderer, *this, GetAbsoluteRect());

    if (m_open) {
        QueueOverlayDraw([this, skin](IRenderer* r) {
            skin->DrawComboBoxDropdown(r, *this);
            });
    }
}

void ComboBox::DrawChildren(IRenderer* renderer, const Rect& parentClip)
{
}
#pragma endregion

#pragma region input
UIElement* ComboBox::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;

    Rect myRect = GetAbsoluteRect();
    if (myRect.Contains(absX, absY))
        return this;

    return nullptr;
}

void ComboBox::OnMouseEnter()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnComboBoxHoverEnter(*this);
}

void ComboBox::OnMouseLeave()
{
    if (!m_open) {
        Skin* skin = GetEffectiveSkin();
        if (skin) skin->OnComboBoxHoverLeave(*this);
    }
}

void ComboBox::OnMouseDown(int button)
{
    if (button != 0) return;

    if (m_open) {
        Rect abs = GetAbsoluteRect();
        Rect drop = GetDropdownRect();

        if (abs.Contains((float)m_lastMouseX, (float)m_lastMouseY)) {
            m_clickedItem = -2;
            return;
        }

        if (drop.Contains((float)m_lastMouseX, (float)m_lastMouseY)) {
            if (NeedsScrollbar()) {
                Rect track = GetDropdownTrackRect();
                Skin* skin = GetEffectiveSkin();
                int minThumb = (skin && skin->GetStyle()) ? skin->GetStyle()->scrollbarMinThumbHeight : 20;
                m_dropdownScroll.SetLastThumbRect(m_dropdownScroll.ComputeThumbRect(track, minThumb));
                if (m_dropdownScroll.HandleMouseDown(m_lastMouseX, m_lastMouseY))
                    return;
            }

            for (int i = 0; i < (int)m_items.size(); ++i) {
                Rect itemRect = GetDropdownItemRect(i);
                if (itemRect.y + itemRect.h < drop.y || itemRect.y > drop.y + drop.h)
                    continue;
                if (itemRect.Contains((float)m_lastMouseX, (float)m_lastMouseY)) {
                    m_clickedItem = i;
                    return;
                }
            }
            return;
        }

        Close();
        return;
    }

    m_clickedItem = -2;
}

void ComboBox::OnMouseUp(int button)
{
    if (button != 0) return;
    m_dropdownScroll.HandleMouseUp();
}
void ComboBox::OnClick()
{
    if (m_clickedItem >= 0) {
        if (m_mode == ComboBoxMode::SingleSelect)
            SelectItem(m_clickedItem);
        else
            ToggleItem(m_clickedItem);
        m_clickedItem = -1;
        return;
    }

    if (m_clickedItem == -2) {
        Toggle();
        m_clickedItem = -1;
        return;
    }

    m_clickedItem = -1;
}

void ComboBox::OnMouseWheel(int delta)
{
    if (m_open && NeedsScrollbar()) {
        m_dropdownScroll.HandleWheel(delta);
    }
}

void ComboBox::OnMouseMove(int x, int y)
{
    m_lastMouseX = x;
    m_lastMouseY = y;

    if (m_open) {
        if (m_dropdownScroll.IsDragging()) {
            Rect track = GetDropdownTrackRect();
            m_dropdownScroll.HandleMouseMove(x, y, track);
            return;
        }

        Rect drop = GetDropdownRect();
        int newHover = -1;
        if (drop.Contains((float)x, (float)y)) {
            for (int i = 0; i < (int)m_items.size(); ++i) {
                Rect itemRect = GetDropdownItemRect(i);
                if (itemRect.y + itemRect.h < drop.y || itemRect.y > drop.y + drop.h)
                    continue;
                if (itemRect.Contains((float)x, (float)y)) {
                    newHover = i;
                    break;
                }
            }
        }

        if (newHover != m_hoveredItem) {
            Skin* skin = GetEffectiveSkin();
            if (m_hoveredItem >= 0 && m_hoveredItem < (int)m_itemAnims.size()) {
                if (skin) skin->OnComboBoxItemHoverLeave(m_itemAnims[m_hoveredItem]);
            }
            m_hoveredItem = newHover;
            if (m_hoveredItem >= 0 && m_hoveredItem < (int)m_itemAnims.size()) {
                if (skin) skin->OnComboBoxItemHoverEnter(m_itemAnims[m_hoveredItem]);
            }
        }
    }
}
#pragma endregion