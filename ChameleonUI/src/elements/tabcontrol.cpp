#include "../../include/elements/TabControl.h"
#include "../../include/elements/TabPage.h"
#include "../../include/skins/skin.h"
#include "../../include/core/uistyle.h"
#include "../../include/util/scoped_timer.h"
#include "../../include/layouts/panel.h"
#include "../../include/util/iconposition.h"
#include <algorithm>

extern int g_lastMouseX, g_lastMouseY;

#pragma region lifecycle
TabControl::TabControl(TabOrientation orientation)
    : m_orientation(orientation)
{}

void TabControl::DrawChildren(IRenderer* renderer, const Rect& parentClip)
{
    for (auto& child : m_children)
    {
        if (!child->IsVisible()) continue;
        if (child->IsTabPage()) continue;

        Rect childClip = child->GetAbsoluteClipRect(parentClip);
        if (childClip.w <= 0 || childClip.h <= 0) continue;

        renderer->SetScissor(childClip);
        child->Draw(renderer);
        child->DrawChildren(renderer, childClip);
        renderer->ResetScissor();
    }
}

void TabControl::AddPage(TabPage* page)
{
    if (!page) return;
    page->SetParent(this);
    page->SetVisible(false);
    page->Anim().active.Set(0.f);
    m_pages.push_back(page);
    if (m_pages.size() == 1) {
        m_activeIndex = -1;
        page->Anim().active.Set(1.f);
        page->SetVisible(true);
        m_activeIndex = 0;
        InvalidateMeasure();
    }
    else {
        InvalidateMeasure();
    }
}

void TabControl::SetActiveIndex(int index)
{
    if (index < 0 || index >= (int)m_pages.size()) return;
    if (index == m_activeIndex) return;

    Skin* skin = GetEffectiveSkin();
    int oldIndex = m_activeIndex;

    m_previousActiveIndex = m_activeIndex;
    m_transitionDirection = (index > oldIndex) ? -1 : 1;

    if (m_activeIndex >= 0 && m_activeIndex < (int)m_pages.size()) {
        if (m_pageTransition == TabTransition::None)
            m_pages[m_activeIndex]->SetVisible(false);
        if (skin) skin->OnTabDeactivated(*m_pages[m_activeIndex]);
    }

    m_activeIndex = index;
    m_pages[m_activeIndex]->SetVisible(true);

    m_pages[m_activeIndex]->ForceLocalRemeasure();
    m_pages[m_activeIndex]->ForceLocalRearrange();

    if (skin) skin->OnTabActivated(*m_pages[m_activeIndex]);

    if (m_pageTransition != TabTransition::None && oldIndex >= 0) {
        const AnimConfig& ac = (skin && skin->GetStyle()) ? skin->GetStyle()->anim : AnimConfig{};
        m_transitionProgress.easing = ac.tabTransitionEasing;
        m_transitionProgress.Set(0.f);
        m_transitionProgress.TransitionTo(1.f, ac.tabTransition);
    }

    m_needMeasure = true;
    m_needArrange = true;

    InvalidateMeasure();

    if (m_onTabChanged) m_onTabChanged(oldIndex, m_activeIndex);
}

void TabControl::SetActiveIndexSilent(int index)
{
    if (index < 0 || index >= (int)m_pages.size()) return;

    if (m_activeIndex >= 0 && m_activeIndex < (int)m_pages.size())
    {
        m_pages[m_activeIndex]->SetVisible(false);
        m_pages[m_activeIndex]->Anim().active.Set(0.f);
    }

    m_activeIndex = index;
    m_pages[m_activeIndex]->SetVisible(true);
    m_pages[m_activeIndex]->Anim().active.Set(1.f);
    m_pages[m_activeIndex]->ForceLocalRemeasure();
    m_pages[m_activeIndex]->ForceLocalRearrange();

    m_previousActiveIndex = -1;
    m_transitionDirection = 0;
    m_transitionProgress.Set(0.f);

    m_needMeasure = true;
    m_needArrange = true;
    InvalidateMeasure();
}



int TabControl::GetActiveIndex() const { return m_activeIndex; }

TabPage* TabControl::GetActivePage() const
{
    if (m_activeIndex >= 0 && m_activeIndex < (int)m_pages.size())
        return m_pages[m_activeIndex];
    return nullptr;
}

TabPage* TabControl::GetPage(int index) const
{
    if (index >= 0 && index < (int)m_pages.size())
        return m_pages[index];
    return nullptr;
}

float TabControl::ComputeUniformButtonSize(Skin* skin) const
{
    if (m_stripThickness > 0)
        return (float)m_stripThickness;

    float maxVal = 0.f;
    for (int i = 0; i < (int)m_pages.size(); ++i)
    {
        float v = (m_orientation == TabOrientation::Horizontal)
            ? ComputeTabContentHeight(i, skin)
            : ComputeTabContentWidth(i, skin);
        if (v > maxVal) maxVal = v;
    }
    return (maxVal > 0.f) ? maxVal : (float)m_defaultTabSize;
}

int TabControl::GetEffectiveTabStripSize() const
{
    Skin* skin = GetEffectiveSkin();

    if (m_alignment == TabAlignment::Fill)
    {
        if (m_stripThickness > 0)
            return m_stripThickness;

        if (m_orientation == TabOrientation::Vertical)
        {
            float maxW = 0.f;
            for (int i = 0; i < (int)m_pages.size(); ++i)
            {
                float w = ComputeTabContentWidth(i, skin);
                Rect tm = m_pages[i]->GetTabMargin();
                float total = w + tm.x + tm.w;
                if (total > maxW) maxW = total;
            }
            return (maxW > 0.f) ? (int)(maxW + 0.5f) : m_defaultTabSize;
        }
        else
        {
            float maxH = 0.f;
            for (int i = 0; i < (int)m_pages.size(); ++i)
            {
                float h = ComputeTabContentHeight(i, skin);
                Rect tm = m_pages[i]->GetTabMargin();
                float total = h + tm.y + tm.h;
                if (total > maxH) maxH = total;
            }
            return (maxH > 0.f) ? (int)(maxH + 0.5f) : m_defaultTabSize;
        }
    }

    float btnSize = ComputeUniformButtonSize(skin);

    if (m_orientation == TabOrientation::Horizontal)
    {
        float maxMargin = 0.f;
        for (int i = 0; i < (int)m_pages.size(); ++i)
        {
            Rect tm = m_pages[i]->GetTabMargin();
            maxMargin = (std::max)(maxMargin, tm.y + tm.h);
        }
        float total = btnSize + maxMargin;
        return (total > 0.f) ? (int)(total + 0.5f) : m_defaultTabSize;
    }
    else
    {
        float maxMargin = 0.f;
        for (int i = 0; i < (int)m_pages.size(); ++i)
        {
            Rect tm = m_pages[i]->GetTabMargin();
            maxMargin = (std::max)(maxMargin, tm.x + tm.w);
        }
        float total = btnSize + maxMargin;
        return (total > 0.f) ? (int)(total + 0.5f) : m_defaultTabSize;
    }
}

Rect TabControl::GetClientRect() const
{
    if (m_detached)
        return Rect(0, 0, 0, 0);

    Rect abs = GetAbsoluteRect();
    int totalStrip = GetTotalStripArea();

    if (m_orientation == TabOrientation::Horizontal)
        return Rect(abs.x + m_padding.x,
            abs.y + totalStrip + m_padding.y,
            abs.w - m_padding.x - m_padding.w,
            abs.h - totalStrip - m_padding.y - m_padding.h);
    return Rect(abs.x + totalStrip + m_padding.x,
        abs.y + m_padding.y,
        abs.w - totalStrip - m_padding.x - m_padding.w,
        abs.h - m_padding.y - m_padding.h);
}

float TabControl::ComputeTabContentWidth(int index, Skin* skin) const
{
    if (index < 0 || index >= (int)m_pages.size()) return (float)m_defaultTabSize;
    auto* page = m_pages[index];
    Rect tp = page->GetTabPadding();

    bool hasIcon = page->GetShowIcon() && page->GetIcon();
    bool hasText = page->GetShowText() && !page->GetTitle().empty();
    IconPosition iconPos = page->GetIconPosition();
    float iconW = hasIcon ? page->GetIconWidth() : 0.f;
    float spacing = (hasIcon && hasText) ? page->GetIconSpacing() : 0.f;

    float textW = 0.f;
    if (hasText)
    {
        if (m_renderer)
        {
            if (!skin) skin = GetEffectiveSkin();
            int fontSize = 13;
            std::wstring fontCopy = L"Segoe UI";
            if (skin && skin->GetStyle()) {
                fontSize = skin->GetStyle()->fontSize;
                fontCopy = skin->GetStyle()->fontFamily;
            }
            Rect textBounds = m_renderer->MeasureText(page->GetTitle(), fontSize, fontCopy.c_str());
            textW = textBounds.w;
        }
        else
        {
            textW = page->GetTitle().size() * 7.f;
        }
    }

    float contentW = 0.f;
    if (iconPos == IconPosition::Left || iconPos == IconPosition::Right)
        contentW = iconW + spacing + textW;
    else
        contentW = (std::max)(iconW, textW);

    if (contentW <= 0.f) contentW = 40.f;
    return contentW + tp.x + tp.w;
}

float TabControl::ComputeTabContentHeight(int index, Skin* skin) const
{
    if (index < 0 || index >= (int)m_pages.size()) return (float)m_defaultTabSize;
    auto* page = m_pages[index];

    int explicitH = page->GetTabButtonHeight();
    if (explicitH > 0) return (float)explicitH;

    bool hasIcon = page->GetShowIcon() && page->GetIcon();
    bool hasText = page->GetShowText() && !page->GetTitle().empty();
    IconPosition iconPos = page->GetIconPosition();
    float iconH = hasIcon ? page->GetIconHeight() : 0.f;
    float spacing = (hasIcon && hasText) ? page->GetIconSpacing() : 0.f;

    if (!skin) skin = GetEffectiveSkin();
    float textH = (skin && skin->GetStyle()) ? (float)skin->GetStyle()->fontSize + 4.f : 17.f;
    if (!hasText) textH = 0.f;

    Rect tp = page->GetTabPadding();
    float contentH = 0.f;

    if (iconPos == IconPosition::Above || iconPos == IconPosition::Below)
        contentH = iconH + spacing + textH;
    else
        contentH = (std::max)(iconH, textH);

    if (contentH <= 0.f) contentH = (float)m_defaultTabSize;
    return contentH + tp.y + tp.h;
}
#pragma endregion

#pragma region geometry
float TabControl::GetTotalTabExtent() const
{
    if (m_alignment == TabAlignment::Fill) return 0.f;
    float total = 0.f;
    int count = (int)m_pages.size();

    if (m_orientation == TabOrientation::Horizontal)
    {
        for (int i = 0; i < count; ++i)
        {
            Rect tm = m_pages[i]->GetTabMargin();
            total += tm.x + ComputeTabContentWidth(i) + tm.w;
        }
    }
    else
    {
        for (int i = 0; i < count; ++i)
        {
            Rect tm = m_pages[i]->GetTabMargin();
            total += tm.y + ComputeTabContentHeight(i) + tm.h;
        }
    }
    return total;
}

Rect TabControl::GetStripButtonsRect() const
{
    if (m_pages.empty()) return GetStripRect();

    int stripSize = GetEffectiveTabStripSize();
    bool needsScroll = IsStripScrollNeeded();
    int sbExtra = needsScroll ? GetEffectiveScrollbarSize() : 0;

    Rect abs = GetAbsoluteRect();
    float totalExtent = GetTotalTabExtent();

    if (m_orientation == TabOrientation::Horizontal)
    {
        float stripH = (float)(stripSize + sbExtra);
        float startX;

        if (m_alignment == TabAlignment::Fill)
            return Rect(abs.x, abs.y, abs.w, stripH);
        else if (m_alignment == TabAlignment::AlignRight)
            startX = abs.x + abs.w - totalExtent;
        else if (m_alignment == TabAlignment::AlignCenter)
            startX = abs.x + (std::max)((abs.w - totalExtent) * 0.5f, 0.f);
        else // AlignLeft
            startX = abs.x;

        return Rect(startX, abs.y, totalExtent, stripH);
    }
    else
    {
        float stripW = (float)(stripSize + sbExtra);
        float startY;

        if (m_alignment == TabAlignment::Fill)
            return Rect(abs.x, abs.y, stripW, abs.h);
        else if (m_alignment == TabAlignment::AlignRight)
            startY = abs.y + abs.h - totalExtent;
        else if (m_alignment == TabAlignment::AlignCenter)
            startY = abs.y + (std::max)((abs.h - totalExtent) * 0.5f, 0.f);
        else // AlignLeft
            startY = abs.y;

        return Rect(abs.x, startY, stripW, totalExtent);
    }
}

Rect TabControl::GetStripRect() const
{
    Rect abs = GetAbsoluteRect();
    int strip = GetEffectiveTabStripSize();
    bool needsScroll = IsStripScrollNeeded();
    int sbExtra = needsScroll ? GetEffectiveScrollbarSize() : 0;

    auto pos = m_scrollbarPosition;
    if (pos == TabScrollbarPosition::Auto)
        pos = (m_orientation == TabOrientation::Horizontal)
        ? TabScrollbarPosition::Bottom : TabScrollbarPosition::Right;

    if (m_orientation == TabOrientation::Horizontal)
    {
        float stripH = m_detached ? abs.h - (float)sbExtra : (float)strip;
        float y = abs.y;
        if (needsScroll && pos == TabScrollbarPosition::Top)
            y += (float)sbExtra;
        return Rect(abs.x, y, abs.w, stripH);
    }
    else
    {
        float stripW = m_detached ? abs.w - (float)sbExtra : (float)strip;
        float x = abs.x;
        if (needsScroll && pos == TabScrollbarPosition::Left)
            x += (float)sbExtra;
        return Rect(x, abs.y, stripW, abs.h);
    }
}

bool TabControl::IsStripScrollNeeded() const
{
    if (m_alignment == TabAlignment::Fill) return false;
    if (m_pages.empty()) return false;
    Rect abs = GetAbsoluteRect();
    float viewport = (m_orientation == TabOrientation::Horizontal) ? abs.w : abs.h;
    return IsStripScrollNeededForSize(viewport);
}

bool TabControl::IsStripScrollNeededForSize(float viewport) const
{
    if (m_alignment == TabAlignment::Fill) return false;
    if (m_pages.empty()) return false;
    if (viewport <= 0.f) return false;
    float extent = GetTotalTabExtent();
    return extent > viewport + 1.f;
}

int TabControl::GetTotalStripArea() const
{
    int strip = GetEffectiveTabStripSize();
    if (IsStripScrollNeeded())
        strip += GetEffectiveScrollbarSize();
    return strip;
}

int TabControl::GetEffectiveScrollbarSize(Skin* skin) const
{
    if (!skin) skin = GetEffectiveSkin();
    return (skin && skin->GetStyle()) ? skin->GetStyle()->scrollbarWidth : 8;
}

void TabControl::ClampTabScroll()
{
    if (!IsStripScrollNeeded()) { m_tabScrollOffset = 0.f; return; }
    Rect strip = GetStripRect();
    float viewport = (m_orientation == TabOrientation::Horizontal) ? strip.w : strip.h;
    float maxScroll = GetTotalTabExtent() - viewport;
    if (maxScroll < 0.f) maxScroll = 0.f;
    if (m_tabScrollOffset < 0.f) m_tabScrollOffset = 0.f;
    if (m_tabScrollOffset > maxScroll) m_tabScrollOffset = maxScroll;
}

Rect TabControl::ComputeTabScrollTrack() const
{
    Rect strip = GetStripRect();
    float sbSize = (float)GetEffectiveScrollbarSize();

    auto pos = m_scrollbarPosition;
    if (pos == TabScrollbarPosition::Auto)
        pos = (m_orientation == TabOrientation::Horizontal)
        ? TabScrollbarPosition::Bottom : TabScrollbarPosition::Right;

    if (m_orientation == TabOrientation::Horizontal)
    {
        if (pos == TabScrollbarPosition::Bottom)
            return Rect(strip.x, strip.y + strip.h, strip.w, sbSize);
        else
            return Rect(strip.x, strip.y - sbSize, strip.w, sbSize);
    }
    else
    {
        if (pos == TabScrollbarPosition::Right)
            return Rect(strip.x + strip.w, strip.y, sbSize, strip.h);
        else
            return Rect(strip.x - sbSize, strip.y, sbSize, strip.h);
    }
}

Rect TabControl::ComputeTabScrollThumb() const
{
    if (!IsStripScrollNeeded()) return Rect();
    Rect track = ComputeTabScrollTrack();
    float totalExtent = GetTotalTabExtent();

    Skin* skin = GetEffectiveSkin();
    float minThumb = (skin && skin->GetStyle()) ? (float)skin->GetStyle()->scrollbarMinThumbHeight : 20.f;

    if (m_orientation == TabOrientation::Horizontal)
    {
        float ratio = track.w / totalExtent;
        float thumbW = (std::max)(track.w * ratio, minThumb);
        float maxScroll = totalExtent - track.w;
        float progress = (maxScroll > 0.f) ? (m_tabScrollOffset / maxScroll) : 0.f;
        float thumbX = track.x + progress * (track.w - thumbW);
        return Rect(thumbX, track.y, thumbW, track.h);
    }
    else
    {
        float ratio = track.h / totalExtent;
        float thumbH = (std::max)(track.h * ratio, minThumb);
        float maxScroll = totalExtent - track.h;
        float progress = (maxScroll > 0.f) ? (m_tabScrollOffset / maxScroll) : 0.f;
        float thumbY = track.y + progress * (track.h - thumbH);
        return Rect(track.x, thumbY, track.w, thumbH);
    }
}

Rect TabControl::GetTabButtonRect(int index) const
{
    if (index < 0 || index >= (int)m_pages.size() || m_pages.empty()) return Rect();
    Rect abs = GetAbsoluteRect();
    int count = (int)m_pages.size();

    if (m_orientation == TabOrientation::Horizontal)
    {
        int stripH = GetEffectiveTabStripSize();

        float effectiveStripH = m_detached ? abs.h : (float)stripH;

        bool needsScroll = IsStripScrollNeeded();
        float stripOffsetY = 0.f;
        if (needsScroll)
        {
            auto pos = m_scrollbarPosition;
            if (pos == TabScrollbarPosition::Auto)
                pos = TabScrollbarPosition::Bottom;
            if (pos == TabScrollbarPosition::Top)
                stripOffsetY = (float)GetEffectiveScrollbarSize();
            effectiveStripH -= (float)GetEffectiveScrollbarSize();
        }

        if (m_alignment == TabAlignment::Fill)
        {
            float tabW = abs.w / (float)count;
            return Rect(abs.x + tabW * index, abs.y + stripOffsetY, tabW, effectiveStripH);
        }

        Rect tm = m_pages[index]->GetTabMargin();
        float uniformBtnH = m_detached
            ? effectiveStripH - tm.y - tm.h
            : ComputeUniformButtonSize();

        float offset = 0.f;
        for (int i = 0; i < index; ++i) {
            Rect itm = m_pages[i]->GetTabMargin();
            offset += itm.x + ComputeTabContentWidth(i) + itm.w;
        }

        float btnW = ComputeTabContentWidth(index);
        float btnY = abs.y + stripOffsetY + tm.y;

        Rect result;

        if (m_alignment == TabAlignment::AlignLeft)
            result = Rect(abs.x + offset + tm.x, btnY, btnW, uniformBtnH);
        else if (m_alignment == TabAlignment::AlignRight)
        {
            float totalW = GetTotalTabExtent();
            float startX = abs.x + abs.w - totalW;
            result = Rect(startX + offset + tm.x, btnY, btnW, uniformBtnH);
        }
        else // AlignCenter
        {
            float totalW = GetTotalTabExtent();
            float centerOffset = (abs.w - totalW) * 0.5f;
            if (centerOffset < 0.f) centerOffset = 0.f;
            result = Rect(abs.x + centerOffset + offset + tm.x, btnY, btnW, uniformBtnH);
        }

        result.x -= m_tabScrollOffset;
        return result;
    }
    else // Vertical
    {
        int stripW = GetEffectiveTabStripSize();

        float effectiveStripW = m_detached ? abs.w : (float)stripW;

        bool needsScroll = IsStripScrollNeeded();
        float stripOffsetX = 0.f;
        if (needsScroll)
        {
            auto pos = m_scrollbarPosition;
            if (pos == TabScrollbarPosition::Auto)
                pos = TabScrollbarPosition::Right;
            if (pos == TabScrollbarPosition::Left)
                stripOffsetX = (float)GetEffectiveScrollbarSize();
            effectiveStripW -= (float)GetEffectiveScrollbarSize();
        }

        if (m_alignment == TabAlignment::Fill)
        {
            float tabH = abs.h / (float)count;
            return Rect(abs.x + stripOffsetX, abs.y + tabH * index, effectiveStripW, tabH);
        }

        Rect tm = m_pages[index]->GetTabMargin();
        float uniformBtnW = m_detached
            ? effectiveStripW - tm.x - tm.w
            : ComputeUniformButtonSize();

        float offset = 0.f;
        for (int i = 0; i < index; ++i) {
            Rect itm = m_pages[i]->GetTabMargin();
            offset += itm.y + ComputeTabContentHeight(i) + itm.h;
        }

        float btnH = ComputeTabContentHeight(index);
        float btnX = abs.x + stripOffsetX + tm.x;

        Rect result;

        if (m_alignment == TabAlignment::AlignLeft)
            result = Rect(btnX, abs.y + offset + tm.y, uniformBtnW, btnH);
        else if (m_alignment == TabAlignment::AlignRight)
        {
            float totalH = GetTotalTabExtent();
            float startY = abs.y + abs.h - totalH;
            result = Rect(btnX, startY + offset + tm.y, uniformBtnW, btnH);
        }
        else // AlignCenter
        {
            float totalH = GetTotalTabExtent();
            float centerOffset = (abs.h - totalH) * 0.5f;
            if (centerOffset < 0.f) centerOffset = 0.f;
            result = Rect(btnX, abs.y + centerOffset + offset + tm.y, uniformBtnW, btnH);
        }

        result.y -= m_tabScrollOffset;
        return result;
    }
}
#pragma endregion

#pragma region update
void TabControl::Update(float dt)
{
    for (auto* page : m_pages)
        page->Anim().Tick(dt);

    bool wasTransitioning = m_transitionProgress.IsAnimating();
    m_transitionProgress.Tick(dt);

    if (wasTransitioning && !m_transitionProgress.IsAnimating())
    {
        if (m_previousActiveIndex >= 0 && m_previousActiveIndex < (int)m_pages.size())
            m_pages[m_previousActiveIndex]->SetVisible(false);
        m_previousActiveIndex = -1;
        m_transitionDirection = 0;
        m_transitionProgress.Set(0.f);
    }

    TabPage* active = GetActivePage();
    if (active) 
        active->Update(dt);

    if (m_transitionProgress.IsAnimating()
        && m_previousActiveIndex >= 0 && m_previousActiveIndex < (int)m_pages.size())
    {
        m_pages[m_previousActiveIndex]->Update(dt);
    }
}
#pragma endregion

#pragma region drawing
void TabControl::DrawPageContent(IRenderer* renderer, TabPage* page,
    const Rect& pageClip, float translateX, float translateY, float opacity)
{
    if (!page || !page->IsVisible()) return;

    page->SetDrawOffset(translateX, translateY);

    renderer->SetScissor(pageClip);

    Rect currentClip = renderer->GetCurrentClip();
    Rect effectiveClip;
    {
        float l = (std::max)(pageClip.x, currentClip.x);
        float t = (std::max)(pageClip.y, currentClip.y);
        float r = (std::min)(pageClip.x + pageClip.w, currentClip.x + currentClip.w);
        float b = (std::min)(pageClip.y + pageClip.h, currentClip.y + currentClip.h);
        effectiveClip = Rect(l, t,
            (r > l) ? (r - l) : 0.f,
            (b > t) ? (b - t) : 0.f);
    }

    if (opacity < 0.99f) renderer->PushLayer(effectiveClip, opacity);

    page->Draw(renderer);
    page->DrawChildren(renderer, effectiveClip);

    if (opacity < 0.99f) renderer->PopLayer();

    renderer->ResetScissor();
    page->SetDrawOffset(0.f, 0.f);
}

void TabControl::Draw(IRenderer* renderer)
{
    m_renderer = renderer;

    Skin* skin = GetEffectiveSkin();
    if (!skin) return;

    Rect absRect = GetAbsoluteRect();
    Rect skinRect = m_stripFit ? GetStripButtonsRect() : absRect;
    skin->DrawTabControl(renderer, *this, skinRect);

    bool needsScroll = IsStripScrollNeeded();
    Rect stripRect = GetStripRect();

    if (needsScroll) renderer->SetScissor(stripRect);

    for (int i = 0; i < (int)m_pages.size(); ++i)
    {
        Rect tabRect = GetTabButtonRect(i);

        if (needsScroll)
        {
            if (m_orientation == TabOrientation::Horizontal)
            {
                if (tabRect.x + tabRect.w < stripRect.x || tabRect.x > stripRect.x + stripRect.w)
                    continue;
            }
            else
            {
                if (tabRect.y + tabRect.h < stripRect.y || tabRect.y > stripRect.y + stripRect.h)
                    continue;
            }
        }
        skin->DrawTabButton(renderer, *m_pages[i], tabRect, i == m_activeIndex);
    }

    if (needsScroll)
    {
        renderer->ResetScissor();

        Rect track = ComputeTabScrollTrack();
        m_tabScrollThumbRect = ComputeTabScrollThumb();
        bool isVert = (m_orientation == TabOrientation::Vertical);
        skin->DrawScrollbar(renderer, track, m_tabScrollThumbRect,
            isVert, m_tabScrollThumbHovered);
    }

    if (m_detached) return;

    Rect clientClip = GetClientRect();

    bool cond_transition = (m_pageTransition != TabTransition::None);
    bool cond_animating = m_transitionProgress.IsAnimating();
    bool cond_prevValid = (m_previousActiveIndex >= 0 && m_previousActiveIndex < (int)m_pages.size());

    bool transitioning = cond_transition && cond_animating && cond_prevValid;

    if (transitioning)
    {
        float t = m_transitionProgress.Value();
        TabPage* outgoing = m_pages[m_previousActiveIndex];
        TabPage* incoming = GetActivePage();

        switch (m_pageTransition)
        {
        case TabTransition::Fade:
            DrawPageContent(renderer, outgoing, clientClip, 0.f, 0.f, 1.f - t);
            DrawPageContent(renderer, incoming, clientClip, 0.f, 0.f, t);
            break;

        case TabTransition::SlideHorizontal:
        {
            float dir = (float)m_transitionDirection;
            DrawPageContent(renderer, outgoing, clientClip, dir * t * clientClip.w, 0.f, 1.f);
            DrawPageContent(renderer, incoming, clientClip, dir * (t - 1.f) * clientClip.w, 0.f, 1.f);
            break;
        }

        case TabTransition::SlideVertical:
        {
            float dir = (float)m_transitionDirection;
            DrawPageContent(renderer, outgoing, clientClip, 0.f, dir * t * clientClip.h, 1.f);
            DrawPageContent(renderer, incoming, clientClip, 0.f, dir * (t - 1.f) * clientClip.h, 1.f);
            break;
        }

        default:
            break;
        }
    }
    else
    {
        TabPage* active = GetActivePage();
        if (active && active->IsVisible())
        {
            renderer->SetScissor(clientClip);
            Rect effectiveClip = renderer->GetCurrentClip();
            active->Draw(renderer);
            active->DrawChildren(renderer, effectiveClip);
            renderer->ResetScissor();
        }
    }
}
#pragma endregion

#pragma region input
UIElement* TabControl::HitTest(int absX, int absY)
{
    m_hitTabIndex = -1;
    if (!m_visible || !m_enabled) return nullptr;

    Rect myRect = GetAbsoluteRect();
    if (absX < myRect.x || absX >= myRect.x + myRect.w ||
        absY < myRect.y || absY >= myRect.y + myRect.h)
        return nullptr;

    if (IsStripScrollNeeded() && m_tabScrollThumbRect.Contains(absX, absY))
        return this;

    Rect stripRect = GetStripRect();
    if (stripRect.Contains(absX, absY))
    {
        for (int i = 0; i < (int)m_pages.size(); ++i)
        {
            Rect tabRect = GetTabButtonRect(i);
            if (tabRect.Contains(absX, absY) && stripRect.Contains(absX, absY))
            {
                m_hitTabIndex = i;
                return this;
            }
        }
    }

    if (!m_detached)
    {
        TabPage* active = GetActivePage();
        if (active && active->IsVisible())
        {
            UIElement* hit = active->HitTest(absX, absY);
            if (hit) return hit;
        }
    }

    if (m_stripFit)
        return nullptr;

    return this;
}

void TabControl::OnMouseDown(int button)
{
    if (button != 0) return;

    if (IsStripScrollNeeded() && m_tabScrollThumbRect.Contains(g_lastMouseX, g_lastMouseY))
    {
        m_tabScrollThumbDragging = true;
        m_tabScrollDragStartMouse = (m_orientation == TabOrientation::Horizontal)
            ? g_lastMouseX : g_lastMouseY;
        m_tabScrollDragStartOffset = m_tabScrollOffset;
        CaptureInput();
        return;
    }

    if (m_hitTabIndex >= 0 && m_hitTabIndex < (int)m_pages.size())
        SetActiveIndex(m_hitTabIndex);
}

void TabControl::OnMouseUp(int button)
{
    if (button == 0)
    {
        if (m_tabScrollThumbDragging)
        {
            m_tabScrollThumbDragging = false;
            ReleaseInput();
        }
    }
}

void TabControl::OnMouseMove(int x, int y)
{
    if (m_tabScrollThumbDragging)
    {
        Rect track = ComputeTabScrollTrack();
        float totalExtent = GetTotalTabExtent();

        if (m_orientation == TabOrientation::Horizontal)
        {
            float thumbW = ComputeTabScrollThumb().w;
            float trackRange = track.w - thumbW;
            if (trackRange <= 0.f) return;
            float mouseDelta = (float)(x - m_tabScrollDragStartMouse);
            float scrollRange = totalExtent - track.w;
            m_tabScrollOffset = m_tabScrollDragStartOffset + (mouseDelta / trackRange) * scrollRange;
        }
        else
        {
            float thumbH = ComputeTabScrollThumb().h;
            float trackRange = track.h - thumbH;
            if (trackRange <= 0.f) return;
            float mouseDelta = (float)(y - m_tabScrollDragStartMouse);
            float scrollRange = totalExtent - track.h;
            m_tabScrollOffset = m_tabScrollDragStartOffset + (mouseDelta / trackRange) * scrollRange;
        }
        ClampTabScroll();
        return;
    }

    if (IsStripScrollNeeded())
    {
        bool wasHover = m_tabScrollThumbHovered;
        m_tabScrollThumbHovered = m_tabScrollThumbRect.Contains(x, y);
    }
}

void TabControl::OnMouseWheel(int delta)
{
    Rect strip = GetStripRect();
    if (IsStripScrollNeeded() && strip.Contains(g_lastMouseX, g_lastMouseY))
    {
        m_tabScrollOffset -= (float)delta / 4.f;
        ClampTabScroll();
        return;
    }

    UIElement::OnMouseWheel(delta);
}

void TabControl::OnMouseLeave()
{
    m_tabScrollThumbHovered = false;
    m_tabScrollThumbDragging = false;
}
#pragma endregion

#pragma region measure & arrange
void TabControl::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    float ownWidth = (m_width > 0) ? (float)m_width : availableSize.width;
    float ownHeight = (m_height > 0) ? (float)m_height : availableSize.height;

    bool widthUnconstrained = (ownWidth <= 0 || ownWidth >= 1e6f);
    bool heightUnconstrained = (ownHeight <= 0 || ownHeight >= 1e6f);

    if (widthUnconstrained)  ownWidth = 200.f;
    if (heightUnconstrained) ownHeight = 200.f;

    float viewport = (m_orientation == TabOrientation::Horizontal) ? ownWidth : ownHeight;
    bool needsScroll = IsStripScrollNeededForSize(viewport);

    int stripSize = GetEffectiveTabStripSize();
    int scrollbarExtra = needsScroll ? GetEffectiveScrollbarSize() : 0;
    float tabStripSize = (float)(stripSize + scrollbarExtra);

    bool contentFitAlongAxis =
        (m_orientation == TabOrientation::Horizontal && m_widthPolicy == SizePolicy::ContentFit) ||
        (m_orientation == TabOrientation::Vertical && m_heightPolicy == SizePolicy::ContentFit);

    if (m_detached)
    {
        if (m_orientation == TabOrientation::Horizontal)
        {
            m_desiredSize.width = contentFitAlongAxis ? GetTotalTabExtent() : ownWidth;
            m_desiredSize.height = tabStripSize;
        }
        else
        {
            m_desiredSize.width = tabStripSize;
            m_desiredSize.height = contentFitAlongAxis ? GetTotalTabExtent() : ownHeight;
        }
        m_needMeasure = false;
        return;
    }

    float clientWidth, clientHeight;
    if (m_orientation == TabOrientation::Horizontal)
    {
        clientWidth = ownWidth - (float)(m_padding.x + m_padding.w);
        clientHeight = ownHeight - tabStripSize - (float)(m_padding.y + m_padding.h);
    }
    else
    {
        clientWidth = ownWidth - tabStripSize - (float)(m_padding.x + m_padding.w);
        clientHeight = ownHeight - (float)(m_padding.y + m_padding.h);
    }
    if (clientWidth < 0)  clientWidth = 0;
    if (clientHeight < 0) clientHeight = 0;

    auto measurePage = [&](TabPage* page) {
        if (!page) return;
        page->Measure(Size{ clientWidth,
            (page->GetHeight() > 0 && (float)page->GetHeight() < clientHeight)
            ? (float)page->GetHeight() : clientHeight });
        };

    measurePage(GetActivePage());
    if (m_previousActiveIndex >= 0 && m_previousActiveIndex < (int)m_pages.size())
    {
        measurePage(m_pages[m_previousActiveIndex]);
    }

    if (contentFitAlongAxis)
    {
        float extent = GetTotalTabExtent();
        if (m_orientation == TabOrientation::Horizontal)
        {
            m_desiredSize.width = extent;
            m_desiredSize.height = ownHeight;
        }
        else
        {
            m_desiredSize.width = ownWidth;
            m_desiredSize.height = extent;
        }
    }
    else
    {
        m_desiredSize.width = ownWidth;
        m_desiredSize.height = ownHeight;
    }
    m_needMeasure = false;
}

void TabControl::Arrange(const Rect& finalRect)
{
    UIElement::Arrange(finalRect);
    ClampTabScroll();

    if (m_detached) return;

    Rect client = GetClientRect();

    auto ensurePageLayout = [&](TabPage* page) {
        if (!page) return;
        float pageH = client.h;
        if (page->GetHeight() > 0 && (float)page->GetHeight() <= client.h)
            pageH = (float)page->GetHeight();
        page->Measure(Size{ client.w, pageH });
        page->Arrange(Rect(client.x, client.y, client.w, pageH));
        };

    ensurePageLayout(GetActivePage());

    if (m_previousActiveIndex >= 0 && m_previousActiveIndex < (int)m_pages.size())
    {
        ensurePageLayout(m_pages[m_previousActiveIndex]);
    }
}
#pragma endregion
