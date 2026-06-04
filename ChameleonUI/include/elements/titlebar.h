#pragma once
#include "../core/UIElement.h"

class Window;

class TitleBar : public UIElement
{
public:
    TitleBar();

    void SetOwnerWindow(Window* win) { m_ownerWindow = win; }
    Window* GetOwnerWindow() const { return m_ownerWindow; }

    #pragma region uielement overrides
    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    UIElement* HitTest(int absX, int absY) override;
    #pragma endregion

private:
    #pragma region members
    Window* m_ownerWindow = nullptr;
    #pragma endregion

    static UIElement* DeepHitTest(UIElement* element, int absX, int absY);
};