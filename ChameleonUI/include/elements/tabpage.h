#pragma once
#include "../core/UIElement.h"
#include "../util/iconposition.h"
#include "../util/iconref.h"
#include <string>

class Panel;

class TabPage : public UIElement
{
public:
    TabPage(const std::wstring& title);
    virtual ~TabPage() = default;

    const std::wstring& GetTitle() const { return m_title; }
    void SetTitle(const std::wstring& title) { m_title = title; }

    // tab button image, shown in the tab strip button (not the page content)
    void SetIcon(void* imageHandle) {
        m_iconRef = IconRef(imageHandle);
        if (imageHandle) m_showIcon = true;
    }

    void SetIconRef(const IconRef& ref) {
        m_iconRef = ref;
        m_showIcon = ref.IsValid();
    }

    void* GetIcon() const { return m_iconRef.imageHandle; }
    const IconRef& GetIconRef() const { return m_iconRef; }

    // whether to show text, icon, or both on the tab button
    bool GetShowText() const { return m_showText; }
    void SetShowText(bool show) { m_showText = show; }
    bool GetShowIcon() const { return m_showIcon; }
    void SetShowIcon(bool show) { m_showIcon = show; }

    // icon position relative to text
    IconPosition GetIconPosition() const { return m_iconPosition; }
    void SetIconPosition(IconPosition pos) { m_iconPosition = pos; }

    // icon-to-text spacing
    float GetIconSpacing() const { return m_iconSpacing; }
    void SetIconSpacing(float spacing) { m_iconSpacing = spacing; }

    // per-tab-button padding, space inside the button between edge and content
    void SetTabPadding(int left, int top, int right, int bottom) {
        m_tabPadding = Rect((float)left, (float)top, (float)right, (float)bottom);
    }
    Rect GetTabPadding() const { return m_tabPadding; }

    // per-tab-button margin, space between this button and adjacent buttons
    void SetTabMargin(int left, int top, int right, int bottom) {
        m_tabMargin = Rect((float)left, (float)top, (float)right, (float)bottom);
    }
    Rect GetTabMargin() const { return m_tabMargin; }

    // per-tab-button height override, 0 means use tabcontrol default
    void SetTabButtonHeight(int h) { m_tabButtonHeight = h; }
    int  GetTabButtonHeight() const { return m_tabButtonHeight; }

    // icon size when drawn on the tab button
    void SetIconSize(float w, float h) { m_iconRef.naturalWidth = w; m_iconRef.naturalHeight = h; }
    float GetIconWidth() const { return m_iconRef.naturalWidth; }
    float GetIconHeight() const { return m_iconRef.naturalHeight; }

    // returns the panel used as the content area, add children to it
    Panel* GetContentPanel() const { return m_contentPanel; }

    #pragma region tab button layout
    struct TabButtonLayout {
        Rect       iconRect;
        Rect       textRect;
        bool       hasIcon = false;
        bool       hasText = false;
        TextAnchor textAnchor = TextAnchor::Center;
    };

    TabButtonLayout ComputeTabButtonLayout(const Rect& buttonRect, IRenderer* renderer,
        int fontSize, const wchar_t* fontFamily) const;
    #pragma endregion

    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;

private:
    std::wstring  m_title;
    IconRef       m_iconRef;
    bool          m_showText = true;
    bool          m_showIcon = false;
    IconPosition  m_iconPosition = IconPosition::Left;
    float         m_iconSpacing = 6.f;
    Rect          m_tabPadding{ 6, 4, 6, 4 };
    Rect          m_tabMargin{ 2, 2, 2, 2 };
    int           m_tabButtonHeight = 0;
    Panel*        m_contentPanel = nullptr;
};