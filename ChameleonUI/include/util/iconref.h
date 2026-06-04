#pragma once
#include "../rendering/renderer.h"

struct IconRef
{
    void* imageHandle = nullptr;
    Rect  srcRect{ 0, 0, 0, 0 };
    float naturalWidth = 16.f;
    float naturalHeight = 16.f;

    IconRef() = default;

    IconRef(void* handle, float w = 16.f, float h = 16.f)
        : imageHandle(handle), naturalWidth(w), naturalHeight(h) {}

    IconRef(void* handle, const Rect& src, float displayW = 16.f, float displayH = 16.f)
        : imageHandle(handle), srcRect(src), naturalWidth(displayW), naturalHeight(displayH) {}

    bool IsValid() const { return imageHandle != nullptr; }
    bool IsAtlasRegion() const { return srcRect.w > 0 && srcRect.h > 0; }

    void Draw(IRenderer* renderer, const Rect& dest, float opacity = 1.f) const
    {
        if (!imageHandle || !renderer) return;
        if (IsAtlasRegion())
            renderer->DrawImageRegion(imageHandle, srcRect, dest, opacity);
        else
            renderer->DrawImage(imageHandle, dest, opacity);
    }
};