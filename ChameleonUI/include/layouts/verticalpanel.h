#pragma once
#include "../core/UIElement.h"

class VerticalPanel : public UIElement
{
public:
    VerticalPanel();
    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    void Draw(IRenderer* renderer) override;
};
