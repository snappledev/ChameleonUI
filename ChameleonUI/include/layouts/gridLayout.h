#pragma once
#include "../core/UIElement.h"
#include <vector>

struct GridSize
{
    float value = 0.f;

    static GridSize Auto() { GridSize s; s.value = 0.f; return s; }
    static GridSize Fixed(float px) { GridSize s; s.value = px; return s; }
    static GridSize Star(float weight = 1.f) { GridSize s; s.value = -weight; return s; }

private:
    explicit GridSize(float v) : value(v) {}
    GridSize() = default;
    friend struct GridRowDef;
    friend struct GridColumnDef;
    friend class GridLayout;
};

struct GridRowDef { float height = 0.0f; };
struct GridColumnDef { float width = 0.0f; };

struct GridPlaceInfo
{
    UIElement* element = nullptr;
    int row = 0;
    int col = 0;
    int rowSpan = 1;
    int colSpan = 1;
};

class GridLayout : public UIElement
{
public:
    GridLayout();

    void AddRow(GridSize size = GridSize::Auto());
    void AddColumn(GridSize size = GridSize::Auto());
    void PlaceChild(UIElement& child, int row, int col, int colSpan = 1, int rowSpan = 1);

    #pragma region uielement overrides
    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    void Update(float dt) override;
    void Draw(IRenderer* renderer) override;
    #pragma endregion

private:
    std::vector<GridRowDef>    m_rows;
    std::vector<GridColumnDef> m_columns;
    std::vector<GridPlaceInfo> m_placements;
    std::vector<float>         m_rowHeights;
    std::vector<float>         m_colWidths;
    std::vector<float>         m_rowPositions;
    std::vector<float>         m_colPositions;

    void ComputeLayout(float totalWidth, float totalHeight, bool measureAutos);
    bool m_layoutValid = false;
};