#pragma once
#include "../core/UIElement.h"
#include <string>
#include <functional>

class Button : public UIElement
{
public:
    Button();
    Button(const std::wstring& text);

    const std::wstring& GetText() const;
    void SetText(const std::wstring& text);

    bool IsHovered() const;
    bool IsPressed() const;
    bool WasClicked() const;

    #pragma region content padding
    void SetContentPadding(float left, float top, float right, float bottom) {
        m_contentPadding = Rect(left, top, right, bottom);
        InvalidateMeasure();
    }
    void SetContentPadding(float horizontal, float vertical) {
        m_contentPadding = Rect(horizontal, vertical, horizontal, vertical);
        InvalidateMeasure();
    }
    void SetContentPadding(float all) {
        m_contentPadding = Rect(all, all, all, all);
        InvalidateMeasure();
    }
    Rect GetContentPadding() const { return m_contentPadding; }
    #pragma endregion

    #pragma region callback
    void SetOnClick(std::function<void()> cb) { m_onClick = cb; }
    #pragma endregion

    #pragma region cached metrics
    float GetCachedTextWidth() const { return m_cachedTextWidth; }
    float GetCachedTextHeight() const { return m_cachedTextHeight; }
    #pragma endregion

    void Update(float dt) override;
    void Draw(IRenderer* renderer) override;

    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnClick() override;
    void Measure(const Size& availableSize) override;

private:
    #pragma region members
    std::wstring          m_text;
    bool                  m_isHovered             = false;
    bool                  m_isPressed             = false;
    bool                  m_wasClicked            = false;

    Rect                  m_contentPadding{ 12.f, 6.f, 12.f, 6.f };

    std::function<void()> m_onClick;

    float                 m_cachedTextWidth       = 0.f;
    float                 m_cachedTextHeight      = 0.f;
    mutable IRenderer*    m_cachedRenderer        = nullptr;
    bool                  m_needFirstTextMeasure  = true;
    #pragma endregion
};