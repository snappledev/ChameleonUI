#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <dxgi.h>
struct Rect {
    float x, y, w, h;
    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
    Rect(int x, int y, int w, int h) : x((float)x), y((float)y), w((float)w), h((float)h) {}
    D2D1_RECT_F ToD2D() const { return D2D1::RectF(x, y, x + w, y + h); }

    bool Contains(float px, float py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
    bool Contains(int px, int py) const { return Contains((float)px, (float)py); }

    Rect Intersect(const Rect& other) const {
        float l = (std::max)(x, other.x);
        float t = (std::max)(y, other.y);
        float r = (std::min)(x + w, other.x + other.w);
        float b = (std::min)(y + h, other.y + other.h);
        return Rect(l, t, (r > l) ? (r - l) : 0.f, (b > t) ? (b - t) : 0.f);
    }

    bool operator==(const Rect& o) const { return x == o.x && y == o.y && w == o.w && h == o.h; }
    bool operator!=(const Rect& o) const { return !(*this == o); }

};

struct Color {
    uint8_t r, g, b, a;
    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
    D2D1_COLOR_F ToD2D() const { return D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f, a / 255.f); }
    bool operator==(const Color& o) const { return r == o.r && g == o.g && b == o.b && a == o.a; }

    static Color Lerp(const Color& a, const Color& b, float t) {
        t = t < 0.f ? 0.f : (t > 1.f ? 1.f : t);
        return Color(
            (uint8_t)(a.r + (b.r - a.r) * t),
            (uint8_t)(a.g + (b.g - a.g) * t),
            (uint8_t)(a.b + (b.b - a.b) * t),
            (uint8_t)(a.a + (b.a - a.a) * t)
        );
    }
    // push channels toward 255 by fraction t
    Color Brighten(float t) const {
        t = t < 0.f ? 0.f : (t > 1.f ? 1.f : t);
        return Color(
            (uint8_t)(r + (255 - r) * t),
            (uint8_t)(g + (255 - g) * t),
            (uint8_t)(b + (255 - b) * t),
            a);
    }

    // push channels toward 0 by fraction t
    Color Darken(float t) const {
        t = t < 0.f ? 0.f : (t > 1.f ? 1.f : t);
        return Color(
            (uint8_t)(r * (1.f - t)),
            (uint8_t)(g * (1.f - t)),
            (uint8_t)(b * (1.f - t)),
            a);
    }

    Color WithAlpha(uint8_t newAlpha) const {
        return Color(r, g, b, newAlpha);
    }

    Color ScaleAlpha(float factor) const {
        float newA = (float)a * factor;
        if (newA > 255.f) newA = 255.f;
        if (newA < 0.f) newA = 0.f;
        return Color(r, g, b, (uint8_t)newA);
    }
};

namespace std {
    template<> struct hash<Color> {
        size_t operator()(const Color& c) const {
            return ((size_t)c.r << 24) | ((size_t)c.g << 16) | ((size_t)c.b << 8) | c.a;
        }
    };
}

enum class TextAnchor {
    TopLeft, TopCenter, TopRight,
    CenterLeft, Center, CenterRight,
    BottomLeft, BottomCenter, BottomRight
};

struct GradientStop {
    float position;
    Color color;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void BeginFrame() = 0;

    #pragma region clipping
    virtual void SetScissor(const Rect& rect) = 0;
    virtual void ResetScissor() = 0;

    #pragma endregion

    #pragma region primitives solid
    virtual void DrawFilledRect(const Rect& rect, Color color, float cornerRadius = 0.f) = 0;
    virtual void DrawOutlineRect(const Rect& rect, Color color, float thickness, float cornerRadius = 0.f) = 0;
    virtual void DrawLine(float x1, float y1, float x2, float y2, Color color, float thickness = 1.f) = 0;
    virtual void DrawCircle(float cx, float cy, float radius, Color color) = 0;
    virtual void DrawCircleOutline(float cx, float cy, float radius, Color color, float thickness = 1.f) = 0;

    #pragma endregion

    #pragma region primitives gradient
    virtual void DrawGradientRect(const Rect& rect, Color topLeft, Color bottomRight,
        bool horizontal = false, float cornerRadius = 0.f) = 0;
    virtual void DrawGradientCircle(float cx, float cy, float radius,
        Color centerColor, Color edgeColor) = 0;
    virtual void DrawMultiGradientRect(const Rect& rect, const GradientStop* stops, int stopCount,
        bool horizontal = false, float cornerRadius = 0.f) = 0;

    #pragma endregion

    #pragma region drop shadow
    virtual void DrawDropShadow(const Rect& rect, float cornerRadius, Color shadowColor,
        float offsetX, float offsetY, float blurRadius) = 0;
    #pragma endregion

    #pragma region images
    virtual void* LoadImage(const wchar_t* filePath) = 0;
    virtual void* LoadImageFromResource(int resourceId, HMODULE hModule = nullptr) = 0;
    virtual void  DrawImage(void* handle, const Rect& dest, float opacity = 1.f) = 0;
    virtual void DrawImageRegion(void* handle, const Rect& srcRect, const Rect& destRect, float opacity = 1.f) = 0;
    virtual void  FreeImage(void* handle) = 0;
    virtual void* LoadImageFromMemory(const void* data, size_t dataSize) = 0;
    #pragma endregion

    #pragma region text
    virtual void DrawText(const std::wstring& text, const Rect& rect, Color color,
        int fontSize = 14, TextAnchor anchor = TextAnchor::TopLeft,
        const wchar_t* fontFamily = L"Segoe UI") = 0;
    virtual Rect MeasureText(const std::wstring& text, int fontSize,
        const wchar_t* fontFamily = L"Segoe UI") = 0;

    #pragma endregion

    #pragma region glassmorphism
    virtual void DrawGlassRect(const Rect& rect, float blurRadius = 20.f,
        Color tint = Color(255, 255, 255, 40),
        float cornerRadius = 12.f, float borderOpacity = 0.15f) = 0;

    virtual void PushTransform(float translateX, float translateY) {}
    virtual void PopTransform() {}

    virtual void PushLayer(const Rect& bounds, float opacity) {}
    virtual void PopLayer() {}

    #pragma endregion

    virtual Rect GetCurrentClip() const = 0;

    virtual int GetClipDepth() const = 0;
    virtual int GetLayerDepth() const = 0;
};

class D2DRenderer : public IRenderer {
public:
    void Initialize(ID2D1DeviceContext* ctx, IDWriteFactory* dwriteFactory);
    void Shutdown();
    void BeginFrame() override;

    void SetScissor(const Rect& rect) override;
    void ResetScissor() override;

    void DrawFilledRect(const Rect& rect, Color color, float cornerRadius = 0.f) override;
    void DrawOutlineRect(const Rect& rect, Color color, float thickness, float cornerRadius = 0.f) override;
    void DrawLine(float x1, float y1, float x2, float y2, Color color, float thickness = 1.f) override;
    void DrawCircle(float cx, float cy, float radius, Color color) override;
    void DrawCircleOutline(float cx, float cy, float radius, Color color, float thickness = 1.f) override;

    void DrawGradientRect(const Rect& rect, Color topLeft, Color bottomRight,
        bool horizontal = false, float cornerRadius = 0.f) override;
    void DrawGradientCircle(float cx, float cy, float radius,
        Color centerColor, Color edgeColor) override;

    void DrawMultiGradientRect(const Rect& rect, const GradientStop* stops, int stopCount,
        bool horizontal = false, float cornerRadius = 0.f) override;

    void DrawDropShadow(const Rect& rect, float cornerRadius, Color shadowColor,
        float offsetX, float offsetY, float blurRadius) override;
    void* LoadImage(const wchar_t* filePath) override;
    void* LoadImageFromResource(int resourceId, HMODULE hModule = nullptr) override;
    void  DrawImage(void* handle, const Rect& dest, float opacity = 1.f) override;
    void  DrawImageRegion(void* handle, const Rect& srcRect, const Rect& destRect, float opacity = 1.f) override;
    void  FreeImage(void* handle) override;
    void SetBackdropBitmap(ID2D1Bitmap1* backdrop);

    void DrawText(const std::wstring& text, const Rect& rect, Color color,
        int fontSize = 14, TextAnchor anchor = TextAnchor::TopLeft,
        const wchar_t* fontFamily = L"Segoe UI") override;
    Rect MeasureText(const std::wstring& text, int fontSize,
        const wchar_t* fontFamily = L"Segoe UI") override;

    void DrawGlassRect(const Rect& rect, float blurRadius = 20.f,
        Color tint = Color(255, 255, 255, 40),
        float cornerRadius = 12.f, float borderOpacity = 0.15f) override;

    void PushTransform(float translateX, float translateY) override;
    void PopTransform() override;
    void PushLayer(const Rect& bounds, float opacity) override;
    void PopLayer() override;
    void* LoadImageFromMemory(const void* data, size_t dataSize) override;

    Rect GetCurrentClip() const override;
    int GetClipDepth() const override;
    int GetLayerDepth() const override;
private:
    ID2D1DeviceContext*                              m_ctx       = nullptr;
    IDWriteFactory*                                  m_dwrite    = nullptr;
    Microsoft::WRL::ComPtr<IWICImagingFactory>       m_wicFactory;
    int                        m_clipCount = 0;
    ID2D1Bitmap1*              m_backdrop  = nullptr;
    std::vector<D2D1_RECT_F>   m_clipStack;

    std::unordered_map<Color, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>> m_brushCache;
    ID2D1SolidColorBrush* GetBrush(const Color& c);

    struct TextFormatKey {
        std::wstring font;
        float size;
        DWRITE_TEXT_ALIGNMENT hAlign;
        DWRITE_PARAGRAPH_ALIGNMENT vAlign;
        bool operator==(const TextFormatKey& o) const {
            return font == o.font && size == o.size && hAlign == o.hAlign && vAlign == o.vAlign;
        }
    };
    struct TextFormatHash {
        size_t operator()(const TextFormatKey& k) const {
            size_t h = std::hash<std::wstring>()(k.font);
            h ^= std::hash<float>()(k.size) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>()((int)k.hAlign) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>()((int)k.vAlign) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
    std::unordered_map<TextFormatKey, Microsoft::WRL::ComPtr<IDWriteTextFormat>, TextFormatHash> m_fmtCache;
    IDWriteTextFormat* GetTextFormat(const wchar_t* font, float size,
        DWRITE_TEXT_ALIGNMENT hAlign, DWRITE_PARAGRAPH_ALIGNMENT vAlign);

    struct RoundedRectKey {
        float w, h, r;
        bool operator==(const RoundedRectKey& o) const { return w == o.w && h == o.h && r == o.r; }
    };
    struct RoundedRectHash {
        size_t operator()(const RoundedRectKey& k) const {
            size_t h = std::hash<float>()(k.w);
            h ^= std::hash<float>()(k.h) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>()(k.r) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
    std::unordered_map<RoundedRectKey, Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry>, RoundedRectHash> m_rrGeoCache;
    ID2D1RoundedRectangleGeometry* GetRoundedRectGeo(float w, float h, float r);

    struct MeasureTextKey {
        std::wstring text;
        int fontSize;
        std::wstring fontFamily;
        bool operator==(const MeasureTextKey& o) const {
            return text == o.text && fontSize == o.fontSize && fontFamily == o.fontFamily;
        }
    };
    struct MeasureTextHash {
        size_t operator()(const MeasureTextKey& k) const {
            size_t h = std::hash<std::wstring>()(k.text);
            h ^= std::hash<int>()(k.fontSize) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<std::wstring>()(k.fontFamily) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
    std::unordered_map<MeasureTextKey, Rect, MeasureTextHash> m_measureTextCache;

    struct GradientKey2 {
        Color c1, c2;
        bool horizontal;
        bool operator==(const GradientKey2& o) const {
            return c1 == o.c1 && c2 == o.c2 && horizontal == o.horizontal;
        }
    };
    struct GradientKey2Hash {
        size_t operator()(const GradientKey2& k) const {
            size_t h = std::hash<Color>()(k.c1);
            h ^= std::hash<Color>()(k.c2) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>()((int)k.horizontal) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
    struct CachedGradient {
        Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> stops;
        Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> brush;
    };
    std::unordered_map<GradientKey2, CachedGradient, GradientKey2Hash> m_gradientCache2;

    struct GradientKeyMulti {
        std::vector<Color> colors;
        std::vector<float> positions;
        bool horizontal;
        bool operator==(const GradientKeyMulti& o) const {
            return colors == o.colors && positions == o.positions && horizontal == o.horizontal;
        }
    };
    struct GradientKeyMultiHash {
        size_t operator()(const GradientKeyMulti& k) const {
            size_t h = 0;
            for (auto& c : k.colors)
                h ^= std::hash<Color>()(c) + 0x9e3779b9 + (h << 6) + (h >> 2);
            for (auto& p : k.positions)
                h ^= std::hash<float>()(p) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>()((int)k.horizontal) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
    std::unordered_map<GradientKeyMulti, CachedGradient, GradientKeyMultiHash> m_gradientCacheMulti;

    struct ImageEntry {
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;
    };
    std::vector<ImageEntry> m_images;

    void* FinalizeWicSource(IWICBitmapSource* source);

    Microsoft::WRL::ComPtr<ID2D1Effect>                        m_blurEffect;
    Microsoft::WRL::ComPtr<ID2D1Effect>                        m_cropEffect;
    std::vector<D2D1_MATRIX_3X2_F>                             m_transformStack;
    std::vector<Microsoft::WRL::ComPtr<ID2D1Layer>>            m_layerStack;
    void EnsureGlassEffects();
};