#pragma once
#include "../core/UIElement.h"
#include "tabcontrol.h"

class TabPageHost : public UIElement
{
public:
    TabPageHost() = default;
    virtual ~TabPageHost() = default;

    void SetTabControl(TabControl* tc)
    {
        m_tabControl = tc;
        m_lastActiveIndex = tc ? tc->GetActiveIndex() : -1;
        InvalidateMeasure();
    }
    TabControl* GetTabControl() const { return m_tabControl; }

    void Draw(IRenderer* renderer) override
    {
        if (!m_tabControl) return;

        Rect clientClip = GetClientRect();
        bool transitioning = m_tabControl->GetPageTransition() != TabTransition::None
            && m_tabControl->IsTransitioning()
            && m_tabControl->GetPreviousActiveIndex() >= 0
            && m_tabControl->GetPreviousActiveIndex() < (int)m_tabControl->GetPages().size();

        if (transitioning)
        {
            float t = m_tabControl->GetTransitionProgress();
            int prevIdx = m_tabControl->GetPreviousActiveIndex();
            TabPage* outgoing = m_tabControl->GetPage(prevIdx);
            TabPage* incoming = m_tabControl->GetActivePage();
            int dir = m_tabControl->GetTransitionDirection();

            switch (m_tabControl->GetPageTransition())
            {
            case TabTransition::Fade:
                DrawPage(renderer, outgoing, clientClip, 0.f, 0.f, 1.f - t);
                DrawPage(renderer, incoming, clientClip, 0.f, 0.f, t);
                break;
            case TabTransition::SlideHorizontal:
                DrawPage(renderer, outgoing, clientClip, dir * t * clientClip.w, 0.f, 1.f);
                DrawPage(renderer, incoming, clientClip, dir * (t - 1.f) * clientClip.w, 0.f, 1.f);
                break;
            case TabTransition::SlideVertical:
                DrawPage(renderer, outgoing, clientClip, 0.f, dir * t * clientClip.h, 1.f);
                DrawPage(renderer, incoming, clientClip, 0.f, dir * (t - 1.f) * clientClip.h, 1.f);
                break;
            default:
                break;
            }
        }
        else
        {
            TabPage* active = m_tabControl->GetActivePage();
            if (active && active->IsVisible())
                DrawPage(renderer, active, clientClip, 0.f, 0.f, 1.f);
        }
    }

    void Measure(const Size& availableSize) override
    {
        if (!m_needMeasure) return;

        // pages are measured in arrange() with the real client size, not here

        float ownW = (m_width > 0) ? (float)m_width : availableSize.width;
        float ownH = (m_height > 0) ? (float)m_height : availableSize.height;

        // report zero desired size when unconstrained, we are a stretch element
        m_desiredSize.width = (m_width > 0) ? (float)m_width
            : (ownW < 1e6f ? ownW : 0.f);
        m_desiredSize.height = (m_height > 0) ? (float)m_height
            : (ownH < 1e6f ? ownH : 0.f);

        m_containsStretchH = true;
        m_containsStretchW = true;

        m_needMeasure = false;
    }

    void Arrange(const Rect& finalRect) override
    {
        UIElement::Arrange(finalRect);
        if (!m_tabControl) return;

        Rect client = GetClientRect();

        // always force layout on the active page since the detached tabcontrol
        // lives in a different branch of the tree and invalidation won't propagate.
        LayoutPage(m_tabControl->GetActivePage(), client);

        if (m_tabControl->IsTransitioning())
        {
            int prevIdx = m_tabControl->GetPreviousActiveIndex();
            LayoutPage(m_tabControl->GetPage(prevIdx), client);
        }
    }

    void Update(float dt) override
    {
        if (!m_tabControl) return;

        // detect outer tab changes (host is in different branch of tree)
        int currentIdx = m_tabControl->GetActiveIndex();
        if (currentIdx != m_lastActiveIndex)
        {
            m_lastActiveIndex = currentIdx;
            InvalidateMeasure();
        }

        TabPage* active = m_tabControl->GetActivePage();

        // detect inner changes from nested tabcontrols that cannot propagate to us
        if (active && active->NeedsLayout())
            InvalidateMeasure();

        if (active) active->Update(dt);

        if (m_tabControl->IsTransitioning())
        {
            int prevIdx = m_tabControl->GetPreviousActiveIndex();
            TabPage* outgoing = m_tabControl->GetPage(prevIdx);
            if (outgoing) outgoing->Update(dt);
        }
    }

    UIElement* HitTest(int absX, int absY) override
    {
        if (!m_visible || !m_enabled) return nullptr;
        Rect myRect = GetAbsoluteRect();
        if (!myRect.Contains(absX, absY)) return nullptr;

        if (m_tabControl)
        {
            TabPage* active = m_tabControl->GetActivePage();
            if (active && active->IsVisible())
            {
                UIElement* hit = active->HitTest(absX, absY);
                if (hit) return hit;
            }
        }
        return this;
    }

private:
    TabControl* m_tabControl = nullptr;
    int m_lastActiveIndex = -1;

    // measures and arranges a page against the real client rect, not in measure() because it may receive infinity
    void LayoutPage(TabPage* page, const Rect& client)
    {
        if (!page || !page->IsVisible()) return;

        float pageH = client.h;
        if (page->GetHeight() > 0 && (float)page->GetHeight() <= client.h)
            pageH = (float)page->GetHeight();

        // force a fresh measure so the page learns the real viewport height
        page->ForceLocalRemeasure();
        page->Measure(Size{ client.w, pageH });
        page->ForceLocalRearrange();
        page->Arrange(Rect(client.x, client.y, client.w, pageH));
    }

    void DrawPage(IRenderer* renderer, TabPage* page, const Rect& pageClip,
        float translateX, float translateY, float opacity)
    {
        if (!page || !page->IsVisible()) return;

        // bridge the coordinate gap between detached tabcontrol and host
        Rect hostClient = GetClientRect();
        Rect pageAbs = page->GetAbsoluteRect();

        float correctionX = hostClient.x - pageAbs.x;
        float correctionY = hostClient.y - pageAbs.y;

        page->SetDrawOffset(correctionX + translateX, correctionY + translateY);

        renderer->SetScissor(pageClip);

        // intersect pageClip with current stack to guard against skin clip leaks
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

        // snapshot renderer state to recover from unbalanced skin draws
        int clipBefore = renderer->GetClipDepth();
        int layerBefore = renderer->GetLayerDepth();

        page->Draw(renderer);
        page->DrawChildren(renderer, effectiveClip);

        // recover from any unbalanced clips/layers left by skin code
        int layerAfter = renderer->GetLayerDepth();
        int clipAfter = renderer->GetClipDepth();
        while (layerAfter > layerBefore + (opacity < 0.99f ? 1 : 0)) {
            renderer->PopLayer();
            --layerAfter;
        }
        while (clipAfter > clipBefore) {
            renderer->ResetScissor();
            --clipAfter;
        }

        if (opacity < 0.99f) renderer->PopLayer();

        renderer->ResetScissor();
        page->SetDrawOffset(0.f, 0.f);
    }
};