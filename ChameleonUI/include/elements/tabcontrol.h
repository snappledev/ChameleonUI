#pragma once
#include "../core/UIElement.h"
#include <vector>
#include <string>
#include <functional>

enum class TabOrientation
{
    Horizontal,
    Vertical
};

enum class TabAlignment
{
    Fill,
    AlignLeft,
    AlignRight,
    AlignCenter   
};

enum class TabTransition
{
    None,             // Instant swap (original behavior)
    Fade,             // Cross-fade between pages
    SlideHorizontal,  // Pages slide left/right
    SlideVertical     // Pages slide up/down
};

enum class TabScrollbarPosition
{
    Auto,    // Horizontal tabs -> Bottom, Vertical tabs -> Right
    Top,
    Bottom,
    Left,
    Right
};

class TabPage;

class TabControl : public UIElement
{
public:
    TabControl(TabOrientation orientation);
    virtual ~TabControl() = default;

    void DrawChildren(IRenderer* renderer, const Rect& parentClip) override;

    #pragma region page management
    void AddPage(TabPage* page);
    void SetActiveIndex(int index);
    void SetActiveIndexSilent(int index);
    int  GetActiveIndex() const;
    TabPage* GetActivePage() const;
    TabPage* GetPage(int index) const;
    #pragma endregion

    #pragma region orientation & alignment
    TabOrientation GetOrientation() const { return m_orientation; }

    TabAlignment GetAlignment() const { return m_alignment; }
    void SetAlignment(TabAlignment align) { m_alignment = align; InvalidateMeasure(); }

    int  GetStripThickness() const { return m_stripThickness; }
    void SetStripThickness(int size) { m_stripThickness = size; InvalidateMeasure(); }
    #pragma endregion

    #pragma region page transition
    TabTransition GetPageTransition() const { return m_pageTransition; }
    void SetPageTransition(TabTransition t) { m_pageTransition = t; }
    #pragma endregion

    #pragma region tab strip scrollbar
    TabScrollbarPosition GetScrollbarPosition() const { return m_scrollbarPosition; }
    void SetScrollbarPosition(TabScrollbarPosition pos) { m_scrollbarPosition = pos; InvalidateMeasure(); }
    #pragma endregion

    #pragma region detached mode
    bool IsDetached() const { return m_detached; }
    void SetDetached(bool d) { m_detached = d; InvalidateMeasure(); }
    #pragma endregion

    #pragma region callbacks
    void SetOnTabChanged(std::function<void(int, int)> cb) { m_onTabChanged = std::move(cb); }
    #pragma endregion

    #pragma region renderer cache
    void SetRenderer(IRenderer* renderer) { m_renderer = renderer; }
    IRenderer* GetRenderer() const { return m_renderer; }
    #pragma endregion

    #pragma region strip-fit mode
    bool IsStripFit() const { return m_stripFit; }
    void SetStripFit(bool fit) { m_stripFit = fit; }
    #pragma endregion

    #pragma region uielement overrides
    Rect GetClientRect() const override;
    void Update(float dt) override;
    void Draw(IRenderer* renderer) override;
    UIElement* HitTest(int absX, int absY) override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseWheel(int delta) override;
    void OnMouseLeave() override;

    void Measure(const Size& availableSize) override;
    void Arrange(const Rect& finalRect) override;
    #pragma endregion

    #pragma region geometry queries
    Rect  GetTabButtonRect(int index) const;
    int   GetEffectiveTabStripSize() const;
    float GetTotalTabExtent() const;
    Rect  GetStripRect() const;
    bool  IsStripScrollNeeded() const;

    const std::vector<TabPage*>& GetPages() const { return m_pages; }
    #pragma endregion

    #pragma region transition state
    bool  IsTransitioning() const { return m_transitionProgress.IsAnimating(); }
    float GetTransitionProgress() const { return m_transitionProgress.Value(); }
    int   GetPreviousActiveIndex() const { return m_previousActiveIndex; }
    int   GetTransitionDirection() const { return m_transitionDirection; }
    int   GetTotalStripArea() const;

    Rect GetStripButtonsRect() const;
    #pragma endregion
private:
    TabOrientation                m_orientation;
    TabAlignment                  m_alignment = TabAlignment::Fill;
    std::vector<TabPage*>         m_pages;
    int                           m_activeIndex = -1;
    int                           m_stripThickness = 0;
    int                           m_defaultTabSize = 30;
    mutable int                   m_hitTabIndex = -1;
    IRenderer*                    m_renderer = nullptr;
    bool                          m_stripFit = false;
    TabTransition                 m_pageTransition = TabTransition::None;
    AnimValue<float>              m_transitionProgress{ 0.f };
    int                           m_previousActiveIndex = -1;
    int                           m_transitionDirection = 0;
    TabScrollbarPosition          m_scrollbarPosition = TabScrollbarPosition::Auto;
    float                         m_tabScrollOffset = 0.f;
    bool                          m_tabScrollThumbDragging = false;
    int                           m_tabScrollDragStartMouse = 0;
    float                         m_tabScrollDragStartOffset = 0.f;
    bool                          m_tabScrollThumbHovered = false;
    mutable Rect                  m_tabScrollThumbRect;
    bool                          m_detached = false;
    std::function<void(int, int)> m_onTabChanged;

    void ClampTabScroll();
    Rect ComputeTabScrollTrack() const;
    Rect ComputeTabScrollThumb() const;
    int  GetEffectiveScrollbarSize(Skin* skin = nullptr) const;
    bool IsStripScrollNeededForSize(float viewport) const;
    float ComputeTabContentWidth(int index, Skin* skin = nullptr) const;
    float ComputeTabContentHeight(int index, Skin* skin = nullptr) const;
    float ComputeUniformButtonSize(Skin* skin = nullptr) const;

    void DrawPageContent(IRenderer* renderer, TabPage* page,
        const Rect& clientClip, float translateX, float translateY, float opacity);
};