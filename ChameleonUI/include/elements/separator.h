#pragma once
#include "../core/UIElement.h"

class Separator : public UIElement
{
public:
    enum class Orientation { Horizontal, Vertical };

    Separator(Orientation orientation = Orientation::Horizontal);

    Orientation GetOrientation() const { return m_orientation; }
    void SetOrientation(Orientation o) { m_orientation = o; InvalidateMeasure(); }

    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;

private:
    Orientation m_orientation;
};