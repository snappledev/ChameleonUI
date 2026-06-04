#pragma once
#include "../core/UIElement.h"
#include "../layouts/panel.h"
#include <string>
#include <vector>
#include <functional>

enum class ListBoxMode
{
    SingleSelect,
    MultiSelect
};

class ListBox : public UIElement
{
public:
    ListBox(const std::vector<std::wstring>& items,
        ListBoxMode mode = ListBoxMode::SingleSelect);
    virtual ~ListBox() = default;

    #pragma region items & mode
    void SetItems(const std::vector<std::wstring>& items);
    const std::vector<std::wstring>& GetItems() const { return m_items; }
    int GetItemCount() const { return (int)m_items.size(); }
    ListBoxMode GetMode() const { return m_mode; }
    void SetMode(ListBoxMode mode) { m_mode = mode; }
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

    #pragma region item display
    float GetItemHeight() const { return m_itemHeight; }
    void SetItemHeight(float h) { m_itemHeight = h; }
    int GetHoveredItem() const { return m_hoveredItem; }
    AnimState& ItemAnim(int index) { return m_itemAnims[index]; }
    const AnimState& ItemAnim(int index) const { return m_itemAnims[index]; }
    void SetOnSelectionChanged(std::function<void(int)> cb) { m_onChanged = cb; }
    Panel* GetContentPanel() const { return m_contentPanel; }
    void SetMaxVisibleItems(int count) { m_maxVisibleItems = count; InvalidateMeasure(); }
    int GetMaxVisibleItems() const { return m_maxVisibleItems; }
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
    void Arrange(const Rect& finalRect) override;

private:
    void SelectItem(int index);
    void ToggleItem(int index);
    void SyncLinkedData();
    void RebuildItemAnims();
    int  HitTestItem(int x, int y) const;

    ListBoxMode                   m_mode;
    std::vector<std::wstring>     m_items;
    float                         m_itemHeight = 30.f;
    int                           m_selectedIndex = -1;
    int*                          m_linkedInt = nullptr;
    std::vector<bool>             m_selected;
    std::vector<bool>*            m_linkedBools = nullptr;
    int                           m_hoveredItem = -1;
    int                           m_clickedItem = -1;
    int                           m_maxVisibleItems = 0;
    std::vector<AnimState>        m_itemAnims;
    std::function<void(int)>      m_onChanged;
    Panel*                        m_contentPanel = nullptr;
    int                           m_lastMouseX = 0;
    int                           m_lastMouseY = 0;
};