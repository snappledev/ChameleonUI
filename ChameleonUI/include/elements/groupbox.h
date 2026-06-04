#pragma once
#include "../core/UIElement.h"
#include <string>

class Panel;

class GroupBox : public UIElement
{
public:
    GroupBox(const std::wstring& title, int x, int y, int w, int h);

    const std::wstring& GetTitle() const;
    void SetTitle(const std::wstring& title);

    bool ShowTitleBar() const { return m_showTitleBar; }
    void SetShowTitleBar(bool show) { m_showTitleBar = show; }

    int GetLabelHeight() const { return m_showTitleBar ? m_labelHeight : 0; }

    Panel* GetContentPanel() const { return m_contentPanel; }

    Rect GetClientRect() const override;

    void Draw(IRenderer* renderer) override;
    void DrawChildren(IRenderer* renderer, const Rect& parentClip) override;
    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    UIElement* HitTest(int absX, int absY) override;

private:
    #pragma region members
    std::wstring  m_title;
    int           m_borderWidth   = 2;
    int           m_labelHeight   = 20;
    bool          m_showTitleBar  = true;
    Panel*        m_contentPanel  = nullptr;
    #pragma endregion
};