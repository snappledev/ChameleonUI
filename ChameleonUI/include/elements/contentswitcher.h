#pragma once
#include "../core/UIElement.h"
#include <functional>

class ContentSwitcher : public UIElement
{
public:
    ContentSwitcher();

    int  GetActiveIndex() const { return m_activeIndex; }
    void SetActiveIndex(int index);
    void SetIndexBinding(std::function<int()> binding) { m_indexBinding = std::move(binding); }

    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    void Update(float dt) override;
    void Draw(IRenderer* renderer) override;

private:
    #pragma region members
    void                   ApplyVisibility();
    int                    m_activeIndex    = 0;
    std::function<int()>   m_indexBinding;
    #pragma endregion
};