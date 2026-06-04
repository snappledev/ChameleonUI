#pragma once
#include "../rendering/renderer.h"
#include <algorithm>

struct ScrollState
{
    float scrollY = 0;
    float contentHeight = 0;
    bool  scrollbarShown = false;

    bool  dragging = false;
    int   dragStartMouseY = 0;
    int   dragStartThumbY = 0;
    bool  thumbHovered = false;
    Rect  lastThumbRect;
};

inline void ScrollClamp(ScrollState& s, int viewportHeight)
{
    float maxY = (std::max)(0.0f, s.contentHeight - (float)viewportHeight);
    if (s.scrollY < 0)    s.scrollY = 0;
    if (s.scrollY > maxY) s.scrollY = maxY;
}

inline void ScrollApplyWheel(ScrollState& s, int delta, int viewportHeight)
{
    s.scrollY -= (float)delta / 4.0f;
    ScrollClamp(s, viewportHeight);
}

inline Rect ScrollComputeThumb(const ScrollState& s, const Rect& track, int minThumbHeight)
{
    if (s.contentHeight <= (float)track.h) return Rect();
    float visibleRatio = (float)track.h / s.contentHeight;
    float thumbHeight = (float)track.h * visibleRatio;
    if (thumbHeight < (float)minThumbHeight) thumbHeight = (float)minThumbHeight;
    float maxScroll = s.contentHeight - (float)track.h;
    float progress = (maxScroll > 0) ? (s.scrollY / maxScroll) : 0.0f;
    float thumbY = (float)track.y + progress * ((float)track.h - thumbHeight);
    float maxThumbY = (float)track.y + (float)track.h - thumbHeight;
    if (thumbY < (float)track.y) thumbY = (float)track.y;
    if (thumbY > maxThumbY) thumbY = maxThumbY;
    return Rect(track.x, (float)thumbY, track.w, (float)thumbHeight);
}

inline bool ScrollTryBeginThumbDrag(ScrollState& s, int mouseX, int mouseY)
{
    if (!s.scrollbarShown) return false;
    if (!s.lastThumbRect.Contains(mouseX, mouseY)) return false;
    s.dragging = true;
    s.dragStartMouseY = mouseY;
    s.dragStartThumbY = s.lastThumbRect.y;
    return true;
}

inline void ScrollUpdateThumbDrag(ScrollState& s, int mouseY, const Rect& track)
{
    if (!s.dragging) return;
    int thumbHeight = s.lastThumbRect.h;
    int newThumbY = s.dragStartThumbY + (mouseY - s.dragStartMouseY);
    int top = track.y;
    int bottom = track.y + track.h - thumbHeight;
    if (newThumbY < top)    newThumbY = top;
    if (newThumbY > bottom) newThumbY = bottom;

    float trackRange = (float)(track.h - thumbHeight);
    if (trackRange <= 0) return;
    float scrollRange = s.contentHeight - (float)track.h;
    float progress = (float)(newThumbY - top) / trackRange;
    s.scrollY = progress * scrollRange;
    ScrollClamp(s, track.h);
}
