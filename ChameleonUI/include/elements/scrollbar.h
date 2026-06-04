#pragma once
#include "../util/scroll_state.h"
#include <algorithm>

class Skin;
class IRenderer;

enum class ScrollDirection { Vertical, Horizontal };

// standalone scrollbar helper, not a uielement. owns a scrollstate and
// encapsulates scroll math, input handling, and draw delegation.
class Scrollbar
{
public:
    explicit Scrollbar(ScrollDirection dir = ScrollDirection::Vertical)
        : m_direction(dir) {}

    #pragma region content & viewport

    void SetContentSize(float size) { m_state.contentHeight = size; }
    float GetContentSize() const { return m_state.contentHeight; }

    void SetViewportSize(float size) { m_viewportSize = size; }
    float GetViewportSize() const { return m_viewportSize; }

    void SetWidth(float w) { m_width = w; }
    float GetWidth() const { return m_width; }

    #pragma endregion

    #pragma region scroll position

    float GetScrollOffset() const { return m_state.scrollY; }

    void SetScrollOffset(float offset)
    {
        m_state.scrollY = offset;
        Clamp();
    }

    void ScrollBy(float delta)
    {
        m_state.scrollY += delta;
        Clamp();
    }

    void Clamp()
    {
        ScrollClamp(m_state, (int)m_viewportSize);
    }

    bool IsNeeded() const { return m_state.contentHeight > m_viewportSize; }
    bool IsShown() const { return m_state.scrollbarShown; }
    void SetShown(bool shown) { m_state.scrollbarShown = shown; }

    #pragma endregion

    #pragma region input

    bool HandleWheel(int delta)
    {
        float oldY = m_state.scrollY;
        ScrollApplyWheel(m_state, delta, (int)m_viewportSize);
        return m_state.scrollY != oldY;
    }

    bool HandleMouseDown(int x, int y)
    {
        return ScrollTryBeginThumbDrag(m_state, x, y);
    }

    void HandleMouseMove(int x, int y, const Rect& track)
    {
        if (m_state.dragging)
        {
            ScrollUpdateThumbDrag(m_state, y, track);
        }
        else if (m_state.scrollbarShown)
        {
            bool hover = m_state.lastThumbRect.Contains(x, y);
            if (hover != m_state.thumbHovered)
                m_state.thumbHovered = hover;
        }
    }

    void HandleMouseUp()
    {
        m_state.dragging = false;
    }

    void HandleMouseLeave()
    {
        m_state.thumbHovered = false;
    }

    bool IsDragging() const { return m_state.dragging; }
    bool IsThumbHovered() const { return m_state.thumbHovered; }

    #pragma endregion

    #pragma region geometry

    Rect ComputeTrackRect(const Rect& clientRect) const
    {
        if (m_direction == ScrollDirection::Vertical)
            return Rect((float)(clientRect.x + clientRect.w - (int)m_width),
                (float)clientRect.y, m_width, (float)clientRect.h);
        else
            return Rect((float)clientRect.x,
                (float)(clientRect.y + clientRect.h - (int)m_width),
                (float)clientRect.w, m_width);
    }

    Rect ComputeThumbRect(const Rect& track, int minThumbHeight) const
    {
        return ScrollComputeThumb(m_state, track, minThumbHeight);
    }

    const Rect& GetLastThumbRect() const { return m_state.lastThumbRect; }
    void SetLastThumbRect(const Rect& r) { m_state.lastThumbRect = r; }

    #pragma endregion

    #pragma region draw

    void Draw(IRenderer* renderer, Skin* skin, const Rect& clientRect);

    #pragma endregion

    #pragma region direct access

    const ScrollState& GetState() const { return m_state; }
    ScrollState& GetState() { return m_state; }
    ScrollDirection GetDirection() const { return m_direction; }
    #pragma endregion

private:
    ScrollState m_state;
    ScrollDirection m_direction;
    float m_width = 12.f;
    float m_viewportSize = 0.f;
};