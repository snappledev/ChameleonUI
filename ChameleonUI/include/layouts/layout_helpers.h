#pragma once
#include "../core/UIElement.h"
#include <algorithm>
#include <memory>
#include <vector>

struct VStackMeasure
{
    float totalHeight = 0;
    float maxWidth = 0;
    bool  containsStretchH = false;
};

inline VStackMeasure MeasureVerticalStack(
    const std::vector<std::unique_ptr<UIElement>>& children,
    float availableWidth,
    int autoSpacing)
{
    VStackMeasure r{};
    int visible = 0;
    for (auto& child : children)
    {
        if (!child->IsVisible())
            continue;

        Rect margin = child->GetMargin();
        float maxW = availableWidth - margin.x - margin.w;
        if (maxW < 0) maxW = 0;

        float childAvailWidth = child->ResolveWidth(maxW);

        child->Measure(Size{ childAvailWidth, INFINITY });
        Size cs = child->GetDesiredSize();

        const bool childStretches = child->WantsStretchH();
        r.containsStretchH = r.containsStretchH || childStretches;

        float effH;
        if (childStretches)
            effH = (cs.height > 0) ? cs.height : 0.0f;
        else
            effH = (cs.height > 0) ? cs.height : 30.0f;

        r.totalHeight += effH + margin.y + margin.h;
        r.maxWidth = (std::max)(r.maxWidth, cs.width + margin.x + margin.w);

        if (autoSpacing > 0)
            r.totalHeight += autoSpacing;

        ++visible;
    }
    if (visible > 0 && autoSpacing > 0)
        r.totalHeight -= autoSpacing;
    return r;
}

inline void ArrangeVerticalStack(
    const std::vector<std::unique_ptr<UIElement>>& children,
    const Rect& clientAbs,
    int autoSpacing)
{
    auto isStretchChild = [](UIElement* c) {
        return (c->GetHeight() <= 0) && c->WantsStretchH();
        };

    float usedHeight = 0.f;
    int stretchCount = 0;
    int visibleCount = 0;

    for (auto& child : children)
    {
        if (!child->IsVisible()) continue;
        Rect margin = child->GetMargin();
        ++visibleCount;

        if (isStretchChild(child.get()))
        {
            ++stretchCount;
            usedHeight += margin.y + margin.h;
        }
        else
        {
            float h = child->ResolveHeight(clientAbs.h);
            if (h <= 0) h = 30.f;
            usedHeight += h + margin.y + margin.h;
        }
    }
    if (visibleCount > 1 && autoSpacing > 0)
        usedHeight += (float)autoSpacing * (visibleCount - 1);

    float remainingHeight = clientAbs.h - usedHeight;
    if (remainingHeight < 0) remainingHeight = 0;
    float stretchHeight = (stretchCount > 0) ? remainingHeight / (float)stretchCount : 0.f;
    if (stretchHeight < 0) stretchHeight = 0;

    float yPos = (float)clientAbs.y;
    for (auto& child : children)
    {
        if (!child->IsVisible())
            continue;

        Rect margin = child->GetMargin();
        float maxW = clientAbs.w - margin.x - margin.w;
        if (maxW < 0) maxW = 0;

        const bool stretch = isStretchChild(child.get());
        float childHeight = stretch ? stretchHeight : child->ResolveHeight(clientAbs.h);
        if (!stretch && childHeight <= 0) childHeight = 30.f;

        float childWidth = child->ResolveWidth(maxW);
        float childX = (float)clientAbs.x + margin.x + child->ResolveX(maxW, childWidth);

        Rect childAbsRect(childX, yPos + margin.y, childWidth, childHeight);
        child->Arrange(childAbsRect);
        yPos += childHeight + margin.y + margin.h + autoSpacing;
    }
}