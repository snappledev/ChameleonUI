#pragma once
#include "../core/UIElement.h"
#include <string>

class Label : public UIElement
{
public:
    Label();
    Label(const std::wstring& text);

    const std::wstring& GetText() const;
    void SetText(const std::wstring& text);

    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;
    UIElement* HitTest(int absX, int absY) override;
    void SetRenderer(IRenderer* renderer) { m_cachedRenderer = renderer; }

private:
    #pragma region members
    std::wstring         m_text;
    mutable IRenderer*   m_cachedRenderer        = nullptr;
    bool                 m_needFirstTextMeasure  = true;
    float                m_cachedTextWidth       = 0.f;
    #pragma endregion
};