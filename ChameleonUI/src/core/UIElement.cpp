#include "../../include/core/UIElement.h"
#include "../../include/skins/skin.h"
#include "../../include/core/UIContext.h"
#include "../../include/util/scoped_timer.h"
#include "../../include/elements/tabcontrol.h"
#include <algorithm>

#pragma region lifecycle & context
UIElement::UIElement() = default;
#pragma endregion

Skin* UIElement::s_defaultSkin = nullptr;

#pragma region hierarchy
UIElement* UIElement::GetParent() const {
    return m_parent;
}

void UIElement::SetParent(UIElement* parent) {
    m_parent = parent;
}

const std::vector<std::unique_ptr<UIElement>>& UIElement::GetChildren() const {
    return m_children;
}

std::vector<std::unique_ptr<UIElement>>& UIElement::GetChildren() {
    return m_children;
}
#pragma endregion

#pragma region position & size
int UIElement::GetX() const { return m_x; }
void UIElement::SetX(int x) { m_x = x; }
int UIElement::GetY() const { return m_y; }
void UIElement::SetY(int y) { m_y = y; }
int UIElement::GetWidth() const { return m_width; }
void UIElement::SetWidth(int width) { m_width = width; }
int UIElement::GetHeight() const { return m_height; }
void UIElement::SetHeight(int height) { m_height = height; }
#pragma endregion

#pragma region skin
void UIElement::SetDefaultSkin(Skin* skin) { s_defaultSkin = skin; }
Skin* UIElement::GetDefaultSkin() { return s_defaultSkin; }
#pragma endregion

#pragma region layout resolution
float UIElement::ResolveWidth(float maxWidth) const
{
    switch (m_widthPolicy) {
    case SizePolicy::Fixed: {
        float w = (m_width > 0) ? (float)m_width : maxWidth;
        return (w > maxWidth) ? maxWidth : w;
    }
    case SizePolicy::ContentFit: {
        if (m_containsStretchW) return maxWidth;
        float w = m_desiredSize.width;
        if (w <= 0) w = maxWidth;
        return (w > maxWidth) ? maxWidth : w;
    }
    default: return maxWidth;
    }
}

float UIElement::ResolveHeight(float maxHeight) const
{
    switch (m_heightPolicy) {
    case SizePolicy::Fixed: {
        float h = (m_height > 0) ? (float)m_height : maxHeight;
        return (h > maxHeight) ? maxHeight : h;
    }
    case SizePolicy::ContentFit: {
        if (m_containsStretchH) return maxHeight;
        float h = m_desiredSize.height;
        if (h <= 0) h = maxHeight;
        return (h > maxHeight) ? maxHeight : h;
    }
    default: return maxHeight;
    }
}

float UIElement::ResolveX(float maxWidth, float resolvedWidth) const
{
    if (resolvedWidth >= maxWidth) return 0.f;
    switch (m_hAlign) {
    case HorizontalAlignment::Center: return (maxWidth - resolvedWidth) * 0.5f;
    case HorizontalAlignment::Right:  return maxWidth - resolvedWidth;
    default: return 0.f;
    }
}


Rect UIElement::GetAbsoluteRect() const {
    int w = (m_arrangedWidth > 0) ? m_arrangedWidth : m_width;
    int h = (m_arrangedHeight > 0) ? m_arrangedHeight : m_height;
    if (m_parent) {
        Rect parentClient = m_parent->GetClientRect();
        return Rect((float)(parentClient.x + m_x) + m_drawOffsetX,
            (float)(parentClient.y + m_y) + m_drawOffsetY,
            (float)w, (float)h);
    }
    return Rect((float)(m_x + m_drawOffsetX), (float)(m_y + m_drawOffsetY), (float)w, (float)h);
}
#pragma endregion

#pragma region drawing
void UIElement::DrawChildrenClipped(IRenderer* renderer, const Rect& parentClip, const Rect& clientArea)
{
    Rect clip = parentClip.Intersect(clientArea);
    if (clip.w <= 0 || clip.h <= 0) return;
    renderer->SetScissor(clip);
    UIElement::DrawChildren(renderer, clip);
    renderer->ResetScissor();
}
#pragma endregion

#pragma region bindings
void UIElement::SyncAllFromBindings()
{
    SyncFromBinding();
    for (auto& child : m_children)
        child->SyncAllFromBindings();
}

Rect UIElement::GetAbsoluteClipRect(const Rect& parentClip) const {
    return GetAbsoluteRect().Intersect(parentClip);
}
#pragma endregion

#pragma region visibility
bool UIElement::IsVisible() const {
    return m_visible;
}

void UIElement::SetVisible(bool visible) {
    m_visible = visible;
}

bool UIElement::IsEnabled() const {
    return m_enabled;
}

void UIElement::SetEnabled(bool enabled) {
    m_enabled = enabled;
}
#pragma endregion

#pragma region skin
Skin* UIElement::GetSkin() const {
    return m_skin;
}

void UIElement::SetSkin(Skin* skin) {
    m_skin = skin;
}

Skin* UIElement::GetEffectiveSkin() const {
    const UIElement* el = this;
    while (el) {
        if (el->m_skin)
            return el->m_skin;
        el = el->m_parent;
    }
    if (s_defaultSkin)
        return s_defaultSkin;
    return nullptr;
}
#pragma endregion

#pragma region auto layout
void UIElement::SetAutoLayoutVertical(bool enable)
{
    m_autoLayoutVertical = enable;
    if (enable)
        ArrangeChildren();
}

bool UIElement::IsAutoLayoutVertical() const
{
    return m_autoLayoutVertical;
}

void UIElement::SetAutoSpacing(int spacing)
{
    m_autoSpacing = spacing;
    if (m_autoLayoutVertical)
        ArrangeChildren();
}

int UIElement::GetAutoSpacing() const
{
    return m_autoSpacing;
}
#pragma endregion

#pragma region input & focus
void UIElement::CaptureInput()
{
    if (s_layoutContext)
        s_layoutContext->SetInputCapture(this);
}

void UIElement::ReleaseInput()
{
    if (s_layoutContext)
        s_layoutContext->ReleaseInputCapture(this);
}

void UIElement::RequestFocus()
{
    if (s_layoutContext)
        s_layoutContext->SetFocus(this);
}

void UIElement::ReleaseFocus()
{
    if (s_layoutContext)
        s_layoutContext->ClearFocus(this);
}

bool UIElement::IsFocused() const
{
    if (s_layoutContext)
        return s_layoutContext->GetFocusedElement() == this;
    return false;
}
#pragma endregion

#pragma region hierarchy helpers
size_t UIElement::GetChildCount() const { return m_children.size(); }
UIElement* UIElement::GetChildAt(int index) {
    if (index < 0 || index >= (int)m_children.size()) return nullptr;
    return m_children[index].get();
}
#pragma endregion

#pragma region auto layout arrange
void UIElement::ArrangeChildren()
{
    if (!m_autoLayoutVertical) return;

    int parentClientWidth = m_width - m_padding.x - m_padding.w;
    if (parentClientWidth < 0) parentClientWidth = 0;

    int currentY = 0;

    for (auto& child : m_children)
    {
        if (!child->IsVisible()) continue;

        Rect childMargin = child->GetMargin();

        int childX = childMargin.x;
        int childWidth = parentClientWidth - childMargin.x - childMargin.w;
        if (childWidth < 0) childWidth = 0;

        child->SetX(childX);
        child->SetY(currentY + childMargin.y);
        child->SetWidth(childWidth);

        int childHeight = child->GetHeight();
        if (childHeight <= 0) childHeight = 30;
        currentY += childHeight + childMargin.y + childMargin.h + m_autoSpacing;
    }
}
#pragma endregion

#pragma region overlay draw
void UIElement::QueueOverlayDraw(std::function<void(IRenderer*)> drawFn)
{
    if (s_layoutContext)
        s_layoutContext->QueueOverlay(std::move(drawFn));
}
#pragma endregion

#pragma region update
void UIElement::Update(float dt)
{
    m_anim.Tick(dt);

    if (m_visibilityBinding)
    {
        bool shouldBeVisible = m_visibilityBinding();
        if (shouldBeVisible != m_visible)
        {
            m_visible = shouldBeVisible;
            if (m_parent)
                m_parent->InvalidateMeasure();
        }
    }

    for (auto& child : m_children)
        child->Update(dt);
}
#pragma endregion

#pragma region drawing children
void UIElement::DrawChildren(IRenderer* renderer, const Rect& parentClip) {
    for (auto& child : m_children) {
        if (!child->m_visible)
            continue;
        Rect childClip = child->GetAbsoluteClipRect(parentClip);

        if (childClip.w <= 0 || childClip.h <= 0)
            continue;

        renderer->SetScissor(childClip);

        int clipBefore = renderer->GetClipDepth();
        int layerBefore = renderer->GetLayerDepth();

        child->Draw(renderer);

        int clipAfter = renderer->GetClipDepth();
        int layerAfter = renderer->GetLayerDepth();

        while (layerAfter > layerBefore) {
            renderer->PopLayer();
            --layerAfter;
        }
        while (clipAfter > clipBefore) {
            renderer->ResetScissor();
            --clipAfter;
        }

        child->DrawChildren(renderer, childClip);
        renderer->ResetScissor();
    }
}
#pragma endregion

#pragma region hit testing & mouse
UIElement* UIElement::HitTest(int absX, int absY) {
    if (!m_visible || !m_enabled)
        return nullptr;
    Rect myRect = GetAbsoluteRect();
    if (absX < myRect.x || absX >= myRect.x + myRect.w ||
        absY < myRect.y || absY >= myRect.y + myRect.h)
        return nullptr;
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        UIElement* hit = (*it)->HitTest(absX, absY);
        if (hit)
            return hit;
    }
    return this;
}

void UIElement::OnMouseEnter() {}
void UIElement::OnMouseLeave() {}
void UIElement::OnMouseDown(int button) {}
void UIElement::OnMouseUp(int button) {}
void UIElement::OnClick() {}

void UIElement::OnMouseWheel(int delta)
{
    if (m_parent)
        m_parent->OnMouseWheel(delta);
}
#pragma endregion

#pragma region padding & margin
void UIElement::SetPadding(int left, int top, int right, int bottom)
{
    m_padding = Rect(left, top, right, bottom);
}

Rect UIElement::GetPadding() const 
{ 
    return m_padding; 
}

void UIElement::SetMargin(int left, int top, int right, int bottom)
{
    m_margin = Rect(left, top, right, bottom);
}

Rect UIElement::GetMargin() const 
{ 
    return m_margin; 
}

Rect UIElement::GetClientRect() const
{
    Rect abs = GetAbsoluteRect();
    return Rect(abs.x + m_padding.x, abs.y + m_padding.y,
        abs.w - m_padding.x - m_padding.w,
        abs.h - m_padding.y - m_padding.h);
}

int UIElement::GetClientWidth()
{
    int w = (m_arrangedWidth > 0) ? m_arrangedWidth : m_width;
    return w - m_padding.x - m_padding.w;
}

int UIElement::GetClientHeight()
{
    int h = (m_arrangedHeight > 0) ? m_arrangedHeight : m_height;
    return h - m_padding.y - m_padding.h;
}
#pragma endregion

#pragma region measure & arrange
void UIElement::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    m_desiredSize.width = (float)m_width;
    m_desiredSize.height = (float)m_height;
    m_needMeasure = false;
}

void UIElement::Arrange(const Rect& finalRect)
{
    if (m_parent) {
        Rect parentClient = m_parent->GetClientRect();
        SetX((int)(finalRect.x - parentClient.x));
        SetY((int)(finalRect.y - parentClient.y));
    }
    else {
        SetX((int)finalRect.x);
        SetY((int)finalRect.y);
    }
    m_arrangedWidth = (int)finalRect.w;
    m_arrangedHeight = (int)finalRect.h;
    m_lastArrangedRect = finalRect;
    m_needArrange = false;
}


bool UIElement::CanSkipArrange(const Rect& finalRect) const
{
    return !m_needArrange &&
        finalRect.x == m_lastArrangedRect.x && finalRect.y == m_lastArrangedRect.y &&
        finalRect.w == m_lastArrangedRect.w && finalRect.h == m_lastArrangedRect.h;
}

void UIElement::InvalidateMeasure()
{
    m_needMeasure = true;
    m_needArrange = true;
    UIElement* el = m_parent;
    while (el)
    {
        if (el->m_needMeasure && el->m_needArrange) break;
        el->m_needMeasure = true;
        el->m_needArrange = true;
        el = el->m_parent;
    }
    if (s_layoutContext)
        s_layoutContext->SetLayoutDirty();
}

void UIElement::MarkLayoutDirty()
{
    InvalidateMeasure();
}
#pragma endregion

UIContext* UIElement::s_layoutContext = nullptr;
void UIElement::SetLayoutContext(UIContext* ctx) { s_layoutContext = ctx; }