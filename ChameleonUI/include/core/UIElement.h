#pragma once
#include "../rendering/renderer.h"
#include "animation.h"
#include <vector>
#include <memory>
#include <functional>

struct Size { float width = 0, height = 0; };

enum class HorizontalAlignment
{
    Stretch,    // Fill the layout-assigned width (default)
    Left,       // Use desired width, align left
    Center,     // Use desired width, align center
    Right       // Use desired width, align right
};

enum class SizePolicy
{
    Stretch,    // Fill available space (default)
    Fixed,      // Use explicit SetWidth/SetHeight; layout does not override
    ContentFit  // Use desired size from Measure (text + padding, skin metrics, etc.)
};

class Skin;
class UIContext;

class UIElement {
public:
    #pragma region construction
    UIElement();
    virtual ~UIElement() = default;
    template<typename T, typename... Args>
    T& AddChild(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *ptr;
        ptr->m_parent = this;
        m_children.push_back(std::move(ptr));
        InvalidateMeasure();
        return ref;
    }
    #pragma endregion

    #pragma region hierarchy
    UIElement* GetParent() const;
    const std::vector<std::unique_ptr<UIElement>>& GetChildren() const;
    std::vector<std::unique_ptr<UIElement>>& GetChildren();
    size_t      GetChildCount() const;
    UIElement*  GetChildAt(int index);
    void        SetParent(UIElement* parent);
    #pragma endregion

    #pragma region layout
    int         GetX() const;
    int         GetY() const;
    int         GetWidth() const;
    SizePolicy  GetWidthPolicy() const { return m_widthPolicy; }
    bool        WantsStretchW() const { return m_widthPolicy == SizePolicy::Stretch || m_containsStretchW; }
    int         GetArrangedWidth() const { return m_arrangedWidth; }
    float       ResolveWidth(float maxWidth) const;

    int         GetHeight() const;
    SizePolicy  GetHeightPolicy() const { return m_heightPolicy; }
    int         GetArrangedHeight() const { return m_arrangedHeight; }
    bool        WantsStretchH() const { return m_heightPolicy == SizePolicy::Stretch || m_containsStretchH; }
    float       ResolveHeight(float maxHeight) const;

    float       ResolveX(float maxWidth, float resolvedWidth) const;
    float       GetDrawOffsetX() const { return m_drawOffsetX; }
    float       GetDrawOffsetY() const { return m_drawOffsetY; }

    HorizontalAlignment GetHorizontalAlignment() const { return m_hAlign; }

    void        SetX(int x);
    void        SetY(int y);
    void        SetWidth(int width);
    void        SetHeight(int height);

    void        SetWidthPolicy(SizePolicy p) { m_widthPolicy = p; InvalidateMeasure(); }
    void        SetHeightPolicy(SizePolicy p) { m_heightPolicy = p; InvalidateMeasure(); }
    void        SetContainsStretchH(bool v) { m_containsStretchH = v; }
    void        SetContainsStretchW(bool v) { m_containsStretchW = v; }
    void        SetHorizontalAlignment(HorizontalAlignment align) { m_hAlign = align; InvalidateMeasure(); }
    void        SetDrawOffset(float dx, float dy) { m_drawOffsetX = dx; m_drawOffsetY = dy; }

    void        SetVisible(bool visible);
    void        SetEnabled(bool enabled);
    bool        IsVisible() const;
    bool        IsEnabled() const;

    Rect        GetAbsoluteRect() const;
    virtual Rect GetClientRect() const;
    Rect        GetAbsoluteClipRect(const Rect& parentClip) const;
    #pragma endregion

    #pragma region skin
    static Skin* GetDefaultSkin();
    Skin*       GetSkin() const;
    Skin*       GetEffectiveSkin() const;
    static void SetDefaultSkin(Skin* skin);
    void        SetSkin(Skin* skin);
    #pragma endregion

    #pragma region input / focus
    void CaptureInput();
    void ReleaseInput();
    void RequestFocus();
    void ReleaseFocus();
    bool IsFocused() const;
    #pragma endregion

    #pragma region measurement
    virtual void    Measure(const Size& availableSize);
    virtual void    Arrange(const Rect& finalRect);
    const Size&     GetDesiredSize() const { return m_desiredSize; }
    void            InvalidateMeasure();
    bool            NeedsLayout() const { return m_needMeasure || m_needArrange; }
    void            ForceLocalRemeasure() { m_needMeasure = true; }
    void            ForceLocalRearrange() { m_needArrange = true; }
    bool            CanSkipArrange(const Rect& finalRect) const;
    static void     SetLayoutContext(UIContext* ctx);
    #pragma endregion

    #pragma region drawing
    virtual void Draw(IRenderer* renderer) = 0;
    virtual void DrawChildren(IRenderer* renderer, const Rect& parentClip);
    static void QueueOverlayDraw(std::function<void(IRenderer*)> drawFn);
    #pragma endregion

    #pragma region hit testing / mouse events
    virtual UIElement*  HitTest(int absX, int absY);
    virtual void        OnMouseEnter();
    virtual void        OnMouseWheel(int delta);
    virtual void        OnMouseLeave();
    virtual void        OnMouseDown(int button);
    virtual void        OnMouseUp(int button);
    virtual void        OnMouseMove(int x, int y) {}
    virtual void        OnClick();
    virtual void        OnKeyDown(int vkCode) {}
    virtual void        OnKeyUp(int vkCode) {}
    virtual void        OnChar(wchar_t ch) {}
    #pragma endregion

    #pragma region tooltip
    void            SetTooltip(const std::wstring& text) { m_tooltip = text; }
    const std::wstring& GetTooltip() const { return m_tooltip; }
    bool            HasTooltip() const { return !m_tooltip.empty(); }
    #pragma endregion

    #pragma region padding / margin
    void SetPadding(int left, int top, int right, int bottom);
    Rect GetPadding() const;
    void SetMargin(int left, int top, int right, int bottom);
    Rect GetMargin() const;
    #pragma endregion

    #pragma region auto layout
    void        SetAutoLayoutVertical(bool enable);
    bool        IsAutoLayoutVertical() const;
    void        SetAutoSpacing(int spacing);
    int         GetAutoSpacing() const;
    virtual int GetClientWidth();
    virtual int GetClientHeight();
    #pragma endregion

    #pragma region animation
    AnimState&          Anim() { return m_anim; }
    const AnimState&    Anim() const { return m_anim; }
    #pragma endregion

    #pragma region misc
    virtual void Update(float dt);
    void        SetVisibilityBinding(std::function<bool()> predicate) { m_visibilityBinding = std::move(predicate); }
    virtual void SyncFromBinding() {}
    void        SyncAllFromBindings();
    bool        IsTabPage() const { return m_isTabPage; }
    bool        IsHitTestTransparent() const { return m_hitTestTransparent; }
    void        SetHitTestTransparent(bool transparent) { m_hitTestTransparent = transparent; }
    #pragma endregion

    virtual void OnFocusGained() {}
    virtual void OnFocusLost() {}

protected:
    #pragma region helper methods
    void            DrawChildrenClipped(IRenderer* renderer, const Rect& parentClip, const Rect& clientArea);
    virtual void    ArrangeChildren();
    void            MarkLayoutDirty();
    #pragma endregion

    #pragma region member variables
    UIElement*                                              m_parent = nullptr;
    std::vector<std::unique_ptr<UIElement>>                 m_children;
    int                                                     m_x = 0;
    int                                                     m_y = 0;
    int                                                     m_width = 0;
    int                                                     m_height = 0;
    int                                                     m_arrangedWidth = 0;
    int                                                     m_arrangedHeight = 0;
    bool                                                    m_visible = true;
    bool                                                    m_enabled = true;
    Skin*                                                   m_skin = nullptr;
    static Skin*                                            s_defaultSkin;
    static UIContext*                                       s_layoutContext;
    Rect                                                    m_padding{ 0,0,0,0 };
    Rect                                                    m_margin{ 0,0,0,0 };

    bool                                                    m_hitTestTransparent = false;

    float                                                   m_drawOffsetX = 0.f;
    float                                                   m_drawOffsetY = 0.f;

    bool                                                    m_autoLayoutVertical = false;
    int                                                     m_autoSpacing = 8;

    Size                                                    m_desiredSize;
    bool                                                    m_needMeasure = true;
    bool                                                    m_needArrange = true;
    std::wstring                                            m_tooltip;
    SizePolicy                                              m_widthPolicy = SizePolicy::Stretch;
    SizePolicy                                              m_heightPolicy = SizePolicy::Stretch;
    HorizontalAlignment                                     m_hAlign = HorizontalAlignment::Stretch;
    bool                                                    m_containsStretchH = false;
    bool                                                    m_containsStretchW = false;
    std::function<bool()>                                   m_visibilityBinding;
    Rect                                                    m_lastArrangedRect;
    bool                                                    m_isTabPage = false;
    AnimState                                               m_anim;
    #pragma endregion
};
