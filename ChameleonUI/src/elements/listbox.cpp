#include "../../include/elements/listbox.h"
#include "../../include/skins/skin.h"
#include "../../include/core/UIStyle.h"
#include <algorithm>

#pragma region lifecycle
ListBox::ListBox(const std::vector<std::wstring>& items, ListBoxMode mode)
    : m_items(items), m_mode(mode)
{
    m_selected.resize(items.size(), false);
    RebuildItemAnims();
    if (m_mode == ListBoxMode::SingleSelect && !items.empty()) {
        m_selectedIndex = 0;
        m_selected[0] = true;
        m_itemAnims[0].checked.Set(1.f);
    }

    SetAutoLayoutVertical(false);
}

void ListBox::SetItems(const std::vector<std::wstring>& items)
{
    m_items = items;
    m_selected.resize(items.size(), false);
    m_selectedIndex = -1;
    RebuildItemAnims();
    InvalidateMeasure();
}

void ListBox::SetSelectedIndex(int index)
{
    if (index < 0 || index >= (int)m_items.size()) return;
    if (m_mode == ListBoxMode::SingleSelect) {
        if (index == m_selectedIndex) return;

        Skin* skin = GetEffectiveSkin();
        if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_selected.size()) {
            m_selected[m_selectedIndex] = false;
            if (skin) skin->OnListBoxItemCheckChanged(m_itemAnims[m_selectedIndex], false);
        }
        std::fill(m_selected.begin(), m_selected.end(), false);
        m_selected[index] = true;
        m_selectedIndex = index;
        if (skin) skin->OnListBoxItemCheckChanged(m_itemAnims[index], true);
        SyncLinkedData();
    }
}

bool ListBox::IsItemSelected(int index) const
{
    if (index < 0 || index >= (int)m_selected.size()) return false;
    return m_selected[index];
}

void ListBox::SetItemSelected(int index, bool selected)
{
    if (index < 0 || index >= (int)m_selected.size()) return;
    m_selected[index] = selected;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnListBoxItemCheckChanged(m_itemAnims[index], selected);
    SyncLinkedData();
}

std::vector<int> ListBox::GetSelectedIndices() const
{
    std::vector<int> result;
    for (int i = 0; i < (int)m_selected.size(); ++i)
        if (m_selected[i]) result.push_back(i);
    return result;
}

void ListBox::RebuildItemAnims()
{
    m_itemAnims.resize(m_items.size());
    for (size_t i = 0; i < m_items.size(); ++i) {
        m_itemAnims[i] = AnimState{};
        m_itemAnims[i].checked.Set(m_selected[i] ? 1.f : 0.f);
    }
}

void ListBox::SelectItem(int index)
{
    if (index < 0 || index >= (int)m_items.size()) return;
    if (m_mode == ListBoxMode::SingleSelect && index == m_selectedIndex) return;

    Skin* skin = GetEffectiveSkin();

    if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_selected.size()) {
        m_selected[m_selectedIndex] = false;
        if (skin) skin->OnListBoxItemCheckChanged(m_itemAnims[m_selectedIndex], false);
    }

    m_selectedIndex = index;
    m_selected[index] = true;
    if (skin) skin->OnListBoxItemCheckChanged(m_itemAnims[index], true);

    SyncLinkedData();
    if (m_onChanged) m_onChanged(m_selectedIndex);
}

void ListBox::ToggleItem(int index)
{
    if (index < 0 || index >= (int)m_items.size()) return;
    m_selected[index] = !m_selected[index];
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnListBoxItemCheckChanged(m_itemAnims[index], m_selected[index]);
    SyncLinkedData();
    if (m_onChanged) m_onChanged(index);
}

void ListBox::SyncLinkedData()
{
    if (m_linkedInt && m_mode == ListBoxMode::SingleSelect)
        *m_linkedInt = m_selectedIndex;
    if (m_linkedBools && m_mode == ListBoxMode::MultiSelect) {
        m_linkedBools->resize(m_selected.size());
        *m_linkedBools = m_selected;
    }
}

int ListBox::HitTestItem(int x, int y) const
{
    Rect client = GetClientRect();
    if (!client.Contains(x, y)) return -1;

    Scrollbar* vsb = m_contentPanel ? m_contentPanel->GetVScrollbar() : nullptr;
    float scrollY = (vsb && vsb->IsShown()) ? vsb->GetScrollOffset() : 0.f;

    // Check if click is on scrollbar track area
    if (vsb && vsb->IsShown()) {
        Rect track = vsb->ComputeTrackRect(client);
        if (track.Contains(x, y)) return -1;
    }

    float sbWidth = (vsb && vsb->IsShown()) ? vsb->GetWidth() : 0.f;
    float contentW = client.w - sbWidth;

    for (int i = 0; i < (int)m_items.size(); ++i) {
        float itemY = (float)client.y + i * m_itemHeight - scrollY;
        Rect itemRect((float)client.x, itemY, contentW, m_itemHeight);
        if (itemY + m_itemHeight < client.y || itemY > client.y + client.h)
            continue;
        if (itemRect.Contains((float)x, (float)y))
            return i;
    }
    return -1;
}
#pragma endregion

#pragma region update
void ListBox::Update(float dt)
{
    m_anim.Tick(dt);
    for (auto& ia : m_itemAnims)
        ia.Tick(dt);
    UIElement::Update(dt);
}
#pragma endregion

#pragma region measure & arrange
void ListBox::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;
    m_desiredSize.width = (m_width > 0) ? (float)m_width : 200.f;

    int visCount = m_maxVisibleItems > 0 ? m_maxVisibleItems : (int)m_items.size();
    if (visCount <= 0) visCount = 1;
    m_desiredSize.height = (m_height > 0) ? (float)m_height : (float)visCount * m_itemHeight;
    m_needMeasure = false;
}

void ListBox::Arrange(const Rect& finalRect)
{
    UIElement::Arrange(finalRect);

    if (!m_contentPanel) {
        auto& panel = AddChild<Panel>();
        panel.SetScrollMode(ScrollMode::Vertical);
        panel.SetAutoLayoutVertical(false);
        panel.SetPadding(0, 0, 0, 0);
        panel.SetMargin(0, 0, 0, 0);
        m_contentPanel = &panel;
    }

    Rect client = GetClientRect();

    Scrollbar* vsb = m_contentPanel->GetVScrollbar();
    if (vsb) {
        float totalH = (float)m_items.size() * m_itemHeight;
        vsb->SetContentSize(totalH);
        vsb->SetViewportSize((float)client.h);

        bool needed = totalH > (float)client.h;
        vsb->SetShown(needed);
        if (needed) {
            Skin* skin = GetEffectiveSkin();
            float sbWidth = 12.f;
            if (skin && skin->GetStyle())
                sbWidth = (float)skin->GetStyle()->scrollbarWidth;
            vsb->SetWidth(sbWidth);
        }
        vsb->Clamp();
    }

    Rect panelRect((float)client.x, (float)client.y, (float)client.w, (float)client.h);
    m_contentPanel->Arrange(panelRect);
}
#pragma endregion

#pragma region drawing
void ListBox::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    if (!skin) return;
    skin->DrawListBox(renderer, *this, GetAbsoluteRect());
}

void ListBox::DrawChildren(IRenderer* renderer, const Rect& parentClip)
{
    if (m_contentPanel) {
        Rect client = GetClientRect();
        Rect clip(
            (std::max)(parentClip.x, client.x),
            (std::max)(parentClip.y, client.y),
            (std::min)(parentClip.x + parentClip.w, client.x + client.w) - (std::max)(parentClip.x, client.x),
            (std::min)(parentClip.y + parentClip.h, client.y + client.h) - (std::max)(parentClip.y, client.y));
        if (clip.w < 0) clip.w = 0;
        if (clip.h < 0) clip.h = 0;
        renderer->SetScissor(clip);
        m_contentPanel->Draw(renderer);
        renderer->ResetScissor();
    }
}
#pragma endregion

#pragma region input
UIElement* ListBox::HitTest(int absX, int absY)
{
    if (!m_visible || !m_enabled) return nullptr;
    Rect myRect = GetAbsoluteRect();
    if (!myRect.Contains(absX, absY)) return nullptr;

    if (m_contentPanel) {
        Scrollbar* vsb = m_contentPanel->GetVScrollbar();
        if (vsb && vsb->IsShown() && vsb->GetLastThumbRect().Contains(absX, absY))
            return this;
    }
    return this;
}

void ListBox::OnMouseEnter()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnListBoxHoverEnter(*this);
}

void ListBox::OnMouseLeave()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) {
        skin->OnListBoxHoverLeave(*this);
        if (m_hoveredItem >= 0 && m_hoveredItem < (int)m_itemAnims.size())
            skin->OnListBoxItemHoverLeave(m_itemAnims[m_hoveredItem]);
    }
    m_hoveredItem = -1;

    if (m_contentPanel) {
        Scrollbar* vsb = m_contentPanel->GetVScrollbar();
        if (vsb) vsb->HandleMouseLeave();
    }
}

void ListBox::OnMouseDown(int button)
{
    if (button != 0) return;

    // Check scrollbar first
    if (m_contentPanel) {
        Scrollbar* vsb = m_contentPanel->GetVScrollbar();
        if (vsb && vsb->IsShown()) {
            Rect client = GetClientRect();
            Skin* skin = GetEffectiveSkin();
            int minThumb = (skin && skin->GetStyle()) ? skin->GetStyle()->scrollbarMinThumbHeight : 20;
            Rect track = vsb->ComputeTrackRect(client);
            vsb->SetLastThumbRect(vsb->ComputeThumbRect(track, minThumb));
            if (vsb->HandleMouseDown(m_lastMouseX, m_lastMouseY))
                return;
        }
    }

    m_clickedItem = HitTestItem(m_lastMouseX, m_lastMouseY);
}

void ListBox::OnMouseUp(int button)
{
    if (button != 0) return;
    if (m_contentPanel) {
        Scrollbar* vsb = m_contentPanel->GetVScrollbar();
        if (vsb) vsb->HandleMouseUp();
    }
}

void ListBox::OnClick()
{
    if (m_clickedItem >= 0) {
        int current = HitTestItem(m_lastMouseX, m_lastMouseY);
        if (current == m_clickedItem) {
            if (m_mode == ListBoxMode::SingleSelect)
                SelectItem(m_clickedItem);
            else
                ToggleItem(m_clickedItem);
        }
        m_clickedItem = -1;
    }
}

void ListBox::OnMouseWheel(int delta)
{
    if (m_contentPanel) {
        Scrollbar* vsb = m_contentPanel->GetVScrollbar();
        if (vsb && vsb->IsShown()) {
            if (vsb->HandleWheel(delta))
                return;
        }
    }
    UIElement::OnMouseWheel(delta);
}

void ListBox::OnMouseMove(int x, int y)
{
    m_lastMouseX = x;
    m_lastMouseY = y;

    if (m_contentPanel) {
        Scrollbar* vsb = m_contentPanel->GetVScrollbar();
        if (vsb) {
            if (vsb->IsDragging()) {
                Rect client = GetClientRect();
                Rect track = vsb->ComputeTrackRect(client);
                vsb->HandleMouseMove(x, y, track);
                return;
            }
            if (vsb->IsShown()) {
                Rect client = GetClientRect();
                Rect track = vsb->ComputeTrackRect(client);
                vsb->HandleMouseMove(x, y, track);
            }
        }
    }

    int newHover = HitTestItem(x, y);
    if (newHover != m_hoveredItem) {
        Skin* skin = GetEffectiveSkin();
        if (m_hoveredItem >= 0 && m_hoveredItem < (int)m_itemAnims.size()) {
            if (skin) skin->OnListBoxItemHoverLeave(m_itemAnims[m_hoveredItem]);
        }
        m_hoveredItem = newHover;
        if (m_hoveredItem >= 0 && m_hoveredItem < (int)m_itemAnims.size()) {
            if (skin) skin->OnListBoxItemHoverEnter(m_itemAnims[m_hoveredItem]);
        }
    }
}
#pragma endregion