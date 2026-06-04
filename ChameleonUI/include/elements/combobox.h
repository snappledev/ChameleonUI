#pragma once
#include "../core/UIElement.h"
#include "scrollbar.h"
#include <string>
#include <vector>
#include <functional>

enum class ComboBoxMode
{
    SingleSelect,
    MultiSelect
};

class ComboBox : public UIElement
{
public:
    ComboBox(const std::vector<std::wstring>& items, ComboBoxMode mode = ComboBoxMode::SingleSelect,
        int maxVisibleItems = 6);
    virtual ~ComboBox() = default;

    #pragma region items
    void SetItems(const std::vector<std::wstring>& items);
    const std::vector<std::wstring>& GetItems() const { return m_items; }
    int GetItemCount() const { return (int)m_items.size(); }
    #pragma endregion

    #pragma region mode
    ComboBoxMode GetMode() const { return m_mode; }
    void SetMode(ComboBoxMode mode) { m_mode = mode; }
    #pragma endregion

    #pragma region single-select
    int  GetSelectedIndex() const { return m_selectedIndex; }
    void SetSelectedIndex(int index);
    void SetLinkedInt(int* linked) { m_linkedInt = linked; }
    #pragma endregion

    #pragma region multi-select
    const std::vector<bool>& GetSelectedStates() const { return m_selected; }
    bool IsItemSelected(int index) const;
    void SetItemSelected(int index, bool selected);
    std::vector<int> GetSelectedIndices() const;
    void SetLinkedBools(std::vector<bool>* linked) { m_linkedBools = linked; }
    #pragma endregion

    #pragma region display
    std::wstring GetDisplayText(IRenderer* renderer) const;
    int GetMaxVisibleItems() const { return m_maxVisibleItems; }
    void SetMaxVisibleItems(int n) { m_maxVisibleItems = n; }
    #pragma endregion

    #pragma region dropdown state
    bool IsOpen() const { return m_open; }
    void Open();
    void Close();
    void Toggle();
    float GetItemHeight() const { return m_itemHeight; }
    void SetItemHeight(float h) { m_itemHeight = h; }
    #pragma endregion

    #pragma region dropdown geometry
    Rect GetDropdownRect() const;
    Rect GetDropdownItemRect(int index) const;
    float GetDropdownScrollOffset() const { return m_dropdownScroll.GetScrollOffset(); }
    bool NeedsScrollbar() const;
    Rect GetDropdownTrackRect() const;
    Rect GetDropdownThumbRect() const;
    const Scrollbar& GetDropdownScrollbar() const { return m_dropdownScroll; }
    int GetHoveredItem() const { return m_hoveredItem; }
    AnimState& ItemAnim(int index) { return m_itemAnims[index]; }
    const AnimState& ItemAnim(int index) const { return m_itemAnims[index]; }
    #pragma endregion

    #pragma region callbacks
    void SetOnSelectionChanged(std::function<void()> cb) { m_onChanged = cb; }
    #pragma endregion

    #pragma region uielement overrides
    void Update(float dt) override;
    void Draw(IRenderer* renderer) override;
    void DrawChildren(IRenderer* renderer, const Rect& parentClip) override;
    UIElement* HitTest(int absX, int absY) override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnClick() override;
    void OnMouseWheel(int delta) override;
    void OnMouseMove(int x, int y) override;
    void Measure(const Size& availableSize) override;
    void SyncFromBinding() override;

private:
    #pragma region helpers
    void SelectItem(int index);
    void ToggleItem(int index);
    void SyncLinkedData();
    void RebuildItemAnims();
    float GetTotalContentHeight() const;
    #pragma endregion

    #pragma region members
    ComboBoxMode               m_mode;
    std::vector<std::wstring>  m_items;
    int                        m_maxVisibleItems;
    float                      m_itemHeight     = 28.f;

    int                        m_selectedIndex  = -1;
    int*                       m_linkedInt      = nullptr;

    std::vector<bool>          m_selected;
    std::vector<bool>*         m_linkedBools    = nullptr;

    bool                       m_open           = false;
    int                        m_hoveredItem    = -1;
    int                        m_clickedItem    = -1;
    Scrollbar                  m_dropdownScroll{ ScrollDirection::Vertical };

    std::vector<AnimState>     m_itemAnims;

    std::function<void()>      m_onChanged;

    mutable IRenderer*         m_cachedRenderer = nullptr;

    int                        m_lastMouseX     = 0;
    int                        m_lastMouseY     = 0;
    #pragma endregion
};