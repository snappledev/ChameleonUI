#pragma once
#include "../core/UIElement.h"
#include "../elements/scrollbar.h"
#include <memory>
#include <string>

enum class ScrollMode { None, Vertical, Horizontal, Both };

class Panel : public UIElement
{
public:
    Panel();

    void SetScrollMode(ScrollMode mode);
    ScrollMode GetScrollMode() const { return m_scrollMode; }

    Scrollbar* GetVScrollbar() { return m_vScroll.get(); }
    Scrollbar* GetHScrollbar() { return m_hScroll.get(); }

    #pragma region uielement overrides
    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    void Draw(IRenderer* renderer) override;
    void DrawChildren(IRenderer* renderer, const Rect& parentClip) override;

    void OnMouseWheel(int delta) override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseLeave() override;
    UIElement* HitTest(int absX, int absY) override;
    #pragma endregion

private:
    void ApplyScrollOffset();

    ScrollMode                   m_scrollMode = ScrollMode::None;
    std::unique_ptr<Scrollbar>   m_vScroll;
    std::unique_ptr<Scrollbar>   m_hScroll;
};