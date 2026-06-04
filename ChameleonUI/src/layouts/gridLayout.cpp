#include "../../include/layouts/GridLayout.h"
#include <algorithm>
#include <cfloat>

#pragma region lifecycle
GridLayout::GridLayout()
{
    SetAutoLayoutVertical(false);
    m_hitTestTransparent = true;
}

void GridLayout::AddRow(GridSize size)
{
    m_rows.push_back({ size.value });
    InvalidateMeasure();
}

void GridLayout::AddColumn(GridSize size)
{
    m_columns.push_back({ size.value });
    InvalidateMeasure();
}

void GridLayout::PlaceChild(UIElement& child, int row, int col, int colSpan, int rowSpan)
{
    m_placements.push_back({ &child, row, col, rowSpan, colSpan });
    InvalidateMeasure();
}
#pragma endregion

#pragma region geometry
void GridLayout::ComputeLayout(float totalWidth, float totalHeight, bool measureAutos)
{
    int rowCount = (int)m_rows.size();
    int colCount = (int)m_columns.size();
    if (rowCount == 0 || colCount == 0) return;

    m_rowHeights.assign(rowCount, 0.0f);
    m_colWidths.assign(colCount, 0.0f);

    float fixedHeightTotal = 0.0f;
    float fixedWidthTotal = 0.0f;
    for (int i = 0; i < rowCount; ++i)
        if (m_rows[i].height > 0)
        {
            m_rowHeights[i] = m_rows[i].height;
            fixedHeightTotal += m_rowHeights[i];
        }
    for (int i = 0; i < colCount; ++i)
        if (m_columns[i].width > 0)
        {
            m_colWidths[i] = m_columns[i].width;
            fixedWidthTotal += m_colWidths[i];
        }

    if (measureAutos)
    {
        for (auto& p : m_placements)
        {
            if (!p.element || !p.element->IsVisible()) continue;
            if (p.row < 0 || p.row >= rowCount) continue;
            if (p.col < 0 || p.col >= colCount) continue;
            Size ds = p.element->GetDesiredSize();
            Rect margin = p.element->GetMargin();
            float h = ds.height + margin.y + margin.h;
            float w = ds.width + margin.x + margin.w;
            if (m_rows[p.row].height == 0.0f && h > m_rowHeights[p.row])
                m_rowHeights[p.row] = h;
            if (m_columns[p.col].width == 0.0f && w > m_colWidths[p.col])
                m_colWidths[p.col] = w;
        }
    }
    float autoHeightTotal = 0.0f, autoWidthTotal = 0.0f;
    for (int i = 0; i < rowCount; ++i)
        if (m_rows[i].height == 0.0f) autoHeightTotal += m_rowHeights[i];
    for (int i = 0; i < colCount; ++i)
        if (m_columns[i].width == 0.0f) autoWidthTotal += m_colWidths[i];

    float remainingHeight = totalHeight - fixedHeightTotal - autoHeightTotal;
    if (remainingHeight < 0) remainingHeight = 0;
    float starWeightHeight = 0.0f;
    for (int i = 0; i < rowCount; ++i)
        if (m_rows[i].height < 0) starWeightHeight += -m_rows[i].height;
    for (int i = 0; i < rowCount; ++i)
        if (m_rows[i].height < 0)
            m_rowHeights[i] = (starWeightHeight > 0)
            ? (-m_rows[i].height / starWeightHeight) * remainingHeight
            : 0.0f;

    float remainingWidth = totalWidth - fixedWidthTotal - autoWidthTotal;
    if (remainingWidth < 0) remainingWidth = 0;
    float starWeightWidth = 0.0f;
    for (int i = 0; i < colCount; ++i)
        if (m_columns[i].width < 0) starWeightWidth += -m_columns[i].width;
    for (int i = 0; i < colCount; ++i)
        if (m_columns[i].width < 0)
            m_colWidths[i] = (starWeightWidth > 0)
            ? (-m_columns[i].width / starWeightWidth) * remainingWidth
            : 0.0f;

    m_rowPositions.assign(rowCount, 0.0f);
    float y = 0;
    for (int i = 0; i < rowCount; ++i) { m_rowPositions[i] = y; y += m_rowHeights[i]; }
    m_colPositions.assign(colCount, 0.0f);
    float x = 0;
    for (int i = 0; i < colCount; ++i) { m_colPositions[i] = x; x += m_colWidths[i]; }

    m_layoutValid = true;
}
#pragma endregion

#pragma region measure & arrange
void GridLayout::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    int rowCount = (int)m_rows.size();
    int colCount = (int)m_columns.size();
    if (rowCount == 0 || colCount == 0)
    {
        m_desiredSize.width = (float)m_width;
        m_desiredSize.height = (float)m_height;
        m_needMeasure = false;
        return;
    }

    float availableW = availableSize.width - (float)(m_padding.x + m_padding.w);
    float availableH = availableSize.height - (float)(m_padding.y + m_padding.h);
    if (availableW < 0 || availableW >= FLT_MAX / 2) availableW = 1000.0f;
    if (availableH < 0 || availableH >= FLT_MAX / 2) availableH = 0.0f;

    ComputeLayout(availableW, availableH, false);

    for (auto& p : m_placements)
    {
        UIElement* child = p.element;
        if (!child || !child->IsVisible()) continue;
        if (p.row < 0 || p.row >= rowCount) continue;
        if (p.col < 0 || p.col >= colCount) continue;

        int endRow = (std::min)(p.row + p.rowSpan, rowCount);
        int endCol = (std::min)(p.col + p.colSpan, colCount);

        float cellW = 0;
        for (int c = p.col; c < endCol; ++c)
        {
            cellW += (m_columns[c].width == 0.0f) ? availableW : m_colWidths[c];
        }
        Rect margin = child->GetMargin();
        float childAvailW = cellW - margin.x - margin.w;
        if (childAvailW < 0) childAvailW = 0;
        child->Measure(Size{ childAvailW, INFINITY });
    }

    ComputeLayout(availableW, availableH, true);

    float totalGridW = 0, totalGridH = 0;
    for (float w : m_colWidths)  totalGridW += w;
    for (float h : m_rowHeights) totalGridH += h;

    m_desiredSize.width = (m_width > 0) ? (float)m_width : totalGridW + (float)(m_padding.x + m_padding.w);
    m_desiredSize.height = (m_height > 0) ? (float)m_height : totalGridH + (float)(m_padding.y + m_padding.h);
    bool anyStretch = false;
    for (auto& p : m_placements) {
        if (p.element && p.element->IsVisible() && p.element->WantsStretchH()) {
            anyStretch = true; break;
        }
    }
    if (!anyStretch)
        for (auto& row : m_rows) if (row.height < 0) { anyStretch = true; break; }
    m_containsStretchH = anyStretch;

    bool anyStretchW = false;
    for (auto& p : m_placements) {
        if (p.element && p.element->IsVisible() && p.element->WantsStretchW()) {
            anyStretchW = true; break;
        }
    }
    if (!anyStretchW)
        for (auto& col : m_columns) if (col.width < 0) { anyStretchW = true; break; }
    m_containsStretchW = anyStretchW;

    m_needMeasure = false;
}

void GridLayout::Arrange(const Rect& finalRect)
{
    UIElement::Arrange(finalRect);

    Rect client = GetClientRect();
    if (client.w <= 0 || client.h <= 0) return;

    ComputeLayout((float)client.w, (float)client.h, true);

    for (auto& p : m_placements)
    {
        UIElement* child = p.element;
        if (!child || !child->IsVisible()) continue;
        if (p.row < 0 || p.row >= (int)m_rows.size()) continue;
        if (p.col < 0 || p.col >= (int)m_columns.size()) continue;

        int endRow = (std::min)(p.row + p.rowSpan, (int)m_rows.size());
        int endCol = (std::min)(p.col + p.colSpan, (int)m_columns.size());

        float cellX = m_colPositions[p.col];
        float cellY = m_rowPositions[p.row];
        float cellW = 0, cellH = 0;
        for (int c = p.col; c < endCol; ++c) cellW += m_colWidths[c];
        for (int r = p.row; r < endRow; ++r) cellH += m_rowHeights[r];

        float absX = (float)client.x + cellX;
        float absY = (float)client.y + cellY;

        Rect margin = child->GetMargin();
        float maxW = cellW - margin.x - margin.w;
        float maxH = cellH - margin.y - margin.h;
        if (maxW < 0) maxW = 0;
        if (maxH < 0) maxH = 0;

        float childW = child->ResolveWidth(maxW);
        float childH = child->ResolveHeight(maxH);
        float childX = absX + margin.x + child->ResolveX(maxW, childW);
        float childY = absY + margin.y;

        child->Arrange(Rect(childX, childY, childW, childH));
    }
}
#pragma endregion

#pragma region update
void GridLayout::Update(float dt)
{
    UIElement::Update(dt);
}
#pragma endregion

#pragma region drawing
void GridLayout::Draw(IRenderer* /*renderer*/)
{
}
#pragma endregion