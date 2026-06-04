#pragma once
#include "../core/UIElement.h"
#include <string>

class Panel;
class TitleBar;

class Window : public UIElement
{
public:
    Window(const std::wstring& title, int x, int y, int w, int h);

    const std::wstring& GetTitle() const;
    void SetTitle(const std::wstring& title);

    int GetTitleBarHeight() const;
    bool IsDragging() const;
    bool ShowTitleBar() const { return m_showTitleBar; }
    void SetShowTitleBar(bool show) { m_showTitleBar = show; }

    TitleBar* GetTitleBar() const { return m_titleBar; }

    Panel* GetContentPanel() const { return m_contentPanel; }

    Rect GetClientRect() const override;
    void Update(float dt) override;
    void Draw(IRenderer* renderer) override;
    void DrawChildren(IRenderer* renderer, const Rect& parentClip) override;
    void OnClick() override;
    UIElement* HitTest(int absX, int absY) override;

    int GetClientWidth() override;
    int GetClientHeight() override;

    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;

private:
    #pragma region members
    std::wstring m_title;
    bool         m_showTitleBar  = true;
    bool         m_dragging      = false;
    int          m_dragOffsetX   = 0;
    int          m_dragOffsetY   = 0;
    TitleBar*    m_titleBar      = nullptr;
    Panel*       m_contentPanel  = nullptr;
    #pragma endregion
};