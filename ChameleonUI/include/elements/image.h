#pragma once
#include "../core/UIElement.h"
#include <string>

class Image : public UIElement
{
public:
    Image();

    void SetImageHandle(void* handle) { m_handle = handle; }
    void* GetImageHandle() const { return m_handle; }

    void SetOpacity(float opacity) { m_opacity = opacity; }
    float GetOpacity() const { return m_opacity; }

    void SetSourceRect(const Rect& src) { m_sourceRect = src; m_useSourceRect = true; }
    void ClearSourceRect() { m_useSourceRect = false; }
    bool HasSourceRect() const { return m_useSourceRect; }
    Rect GetSourceRect() const { return m_sourceRect; }

    void SetNaturalSize(float w, float h) { m_naturalWidth = w; m_naturalHeight = h; InvalidateMeasure(); }
    float GetNaturalWidth() const { return m_naturalWidth; }
    float GetNaturalHeight() const { return m_naturalHeight; }

    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;

private:
    #pragma region members
    void* m_handle          = nullptr;
    float m_opacity         = 1.f;
    float m_naturalWidth    = 0.f;
    float m_naturalHeight   = 0.f;
    Rect  m_sourceRect;
    bool  m_useSourceRect   = false;
    #pragma endregion
};