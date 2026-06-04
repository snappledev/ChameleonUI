#include "../../include/rendering/renderer.h"
#include "../../include/elements/tabcontrol.h"
#include <d2d1effects.h>
#include <algorithm>

using Microsoft::WRL::ComPtr;

#pragma region anchor mapping
static void AnchorToAlignment(TextAnchor anchor,
    DWRITE_TEXT_ALIGNMENT& hAlign,
    DWRITE_PARAGRAPH_ALIGNMENT& vAlign)
{
    switch (anchor) {
    case TextAnchor::TopLeft:      hAlign = DWRITE_TEXT_ALIGNMENT_LEADING;  vAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;   break;
    case TextAnchor::TopCenter:    hAlign = DWRITE_TEXT_ALIGNMENT_CENTER;   vAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;   break;
    case TextAnchor::TopRight:     hAlign = DWRITE_TEXT_ALIGNMENT_TRAILING; vAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;   break;
    case TextAnchor::CenterLeft:   hAlign = DWRITE_TEXT_ALIGNMENT_LEADING;  vAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER; break;
    case TextAnchor::Center:       hAlign = DWRITE_TEXT_ALIGNMENT_CENTER;   vAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER; break;
    case TextAnchor::CenterRight:  hAlign = DWRITE_TEXT_ALIGNMENT_TRAILING; vAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER; break;
    case TextAnchor::BottomLeft:   hAlign = DWRITE_TEXT_ALIGNMENT_LEADING;  vAlign = DWRITE_PARAGRAPH_ALIGNMENT_FAR;    break;
    case TextAnchor::BottomCenter: hAlign = DWRITE_TEXT_ALIGNMENT_CENTER;   vAlign = DWRITE_PARAGRAPH_ALIGNMENT_FAR;    break;
    case TextAnchor::BottomRight:  hAlign = DWRITE_TEXT_ALIGNMENT_TRAILING; vAlign = DWRITE_PARAGRAPH_ALIGNMENT_FAR;    break;
    }
}
#pragma endregion anchor mapping

#pragma region lifecycle
void D2DRenderer::Initialize(ID2D1DeviceContext* ctx, IDWriteFactory* dwriteFactory) {
    m_ctx = ctx;
    m_dwrite = dwriteFactory;
    m_brushCache.clear();
    m_fmtCache.clear();
    m_rrGeoCache.clear();
    m_images.clear();
    m_blurEffect.Reset();
    m_cropEffect.Reset();

    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(m_wicFactory.GetAddressOf()));
}


void D2DRenderer::Shutdown() {
    m_blurEffect.Reset();
    m_cropEffect.Reset();
    m_brushCache.clear();
    m_fmtCache.clear();
    m_rrGeoCache.clear();
    m_images.clear();
    m_wicFactory.Reset();
    m_ctx = nullptr;
    m_dwrite = nullptr;
}
#pragma endregion lifecycle

#pragma region resource cache

ID2D1SolidColorBrush* D2DRenderer::GetBrush(const Color& c) {
    auto it = m_brushCache.find(c);
    if (it != m_brushCache.end()) return it->second.Get();
    ComPtr<ID2D1SolidColorBrush> b;
    m_ctx->CreateSolidColorBrush(c.ToD2D(), b.GetAddressOf());
    m_brushCache[c] = b;
    return b.Get();
}

IDWriteTextFormat* D2DRenderer::GetTextFormat(const wchar_t* font, float size,
    DWRITE_TEXT_ALIGNMENT hAlign,
    DWRITE_PARAGRAPH_ALIGNMENT vAlign) {
    TextFormatKey key{ font, size, hAlign, vAlign };
    auto it = m_fmtCache.find(key);
    if (it != m_fmtCache.end()) return it->second.Get();

    ComPtr<IDWriteTextFormat> fmt;
    m_dwrite->CreateTextFormat(font, nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        size, L"en-US", fmt.GetAddressOf());
    if (fmt) {
        fmt->SetTextAlignment(hAlign);
        fmt->SetParagraphAlignment(vAlign);
        fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }
    m_fmtCache[key] = fmt;
    return fmt.Get();
}

ID2D1RoundedRectangleGeometry* D2DRenderer::GetRoundedRectGeo(float w, float h, float r) {
    RoundedRectKey key{ w, h, r };
    auto it = m_rrGeoCache.find(key);
    if (it != m_rrGeoCache.end()) return it->second.Get();

    ComPtr<ID2D1Factory> factory;
    m_ctx->GetFactory(factory.GetAddressOf());
    ComPtr<ID2D1Factory1> factory1;
    factory.As(&factory1);

    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(D2D1::RectF(0, 0, w, h), r, r);
    ComPtr<ID2D1RoundedRectangleGeometry> geo;
    factory1->CreateRoundedRectangleGeometry(rr, geo.GetAddressOf());
    m_rrGeoCache[key] = geo;
    return geo.Get();
}

void D2DRenderer::EnsureGlassEffects() {
    if (!m_blurEffect)
        m_ctx->CreateEffect(CLSID_D2D1GaussianBlur, m_blurEffect.GetAddressOf());
    if (!m_cropEffect)
        m_ctx->CreateEffect(CLSID_D2D1Crop, m_cropEffect.GetAddressOf());
}
#pragma endregion resource cache

#pragma region clip & transform

void D2DRenderer::BeginFrame() {
    m_clipStack.clear();
    m_clipCount = 0;
}

void D2DRenderer::SetScissor(const Rect& rect) {
    if (!m_ctx) return;

    D2D1_RECT_F requested = rect.ToD2D();

    D2D1_RECT_F effective;
    if (!m_clipStack.empty()) {
        const D2D1_RECT_F& current = m_clipStack.back();
        effective.left = (std::max)(requested.left, current.left);
        effective.top = (std::max)(requested.top, current.top);
        effective.right = (std::min)(requested.right, current.right);
        effective.bottom = (std::min)(requested.bottom, current.bottom);
        if (effective.right < effective.left) effective.right = effective.left;
        if (effective.bottom < effective.top) effective.bottom = effective.top;
    }
    else {
        effective = requested;
    }

    m_clipStack.push_back(effective);

    m_ctx->PushAxisAlignedClip(effective, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    m_clipCount++;
}

void D2DRenderer::ResetScissor() {
    if (!m_ctx || m_clipStack.empty()) return;

    m_ctx->PopAxisAlignedClip();
    m_clipCount--;
    m_clipStack.pop_back();
}

Rect D2DRenderer::GetCurrentClip() const {
    if (m_clipStack.empty()) {
        D2D1_SIZE_F s = m_ctx->GetSize();
        return Rect(0.f, 0.f, s.width, s.height);
    }
    const D2D1_RECT_F& r = m_clipStack.back();
    return Rect(r.left, r.top, r.right - r.left, r.bottom - r.top);
}


int D2DRenderer::GetClipDepth() const {
    return m_clipCount;
}

int D2DRenderer::GetLayerDepth() const {
    return (int)m_layerStack.size();
}

void D2DRenderer::PushTransform(float translateX, float translateY)
{
    D2D1_MATRIX_3X2_F current;
    m_ctx->GetTransform(&current);
    m_transformStack.push_back(current);
    D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(translateX, translateY);
    m_ctx->SetTransform(current * translation);
}

void D2DRenderer::PopTransform()
{
    if (m_transformStack.empty()) return;
    m_ctx->SetTransform(m_transformStack.back());
    m_transformStack.pop_back();
}

void D2DRenderer::PushLayer(const Rect& bounds, float opacity)
{
    Microsoft::WRL::ComPtr<ID2D1Layer> layer;
    m_ctx->CreateLayer(&layer);
    m_ctx->PushLayer(
        D2D1::LayerParameters(
            bounds.ToD2D(),
            nullptr,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
            D2D1::IdentityMatrix(),
            opacity,
            nullptr,
            D2D1_LAYER_OPTIONS_NONE),
        layer.Get());
    m_layerStack.push_back(std::move(layer));
}

void D2DRenderer::PopLayer()
{
    if (m_layerStack.empty()) return;
    m_ctx->PopLayer();
    m_layerStack.pop_back();
}
#pragma endregion clip & transform

#pragma region primitives

void D2DRenderer::DrawFilledRect(const Rect& rect, Color color, float cornerRadius) {
    if (!m_ctx) return;
    auto* brush = GetBrush(color);
    if (cornerRadius <= 0.f) {
        m_ctx->FillRectangle(rect.ToD2D(), brush);
    }
    else {
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rect.ToD2D(), cornerRadius, cornerRadius);
        m_ctx->FillRoundedRectangle(rr, brush);
    }
}

void D2DRenderer::DrawOutlineRect(const Rect& rect, Color color, float thickness, float cornerRadius) {
    if (!m_ctx) return;
    auto* brush = GetBrush(color);
    if (cornerRadius <= 0.f) {
        m_ctx->DrawRectangle(rect.ToD2D(), brush, thickness);
    }
    else {
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rect.ToD2D(), cornerRadius, cornerRadius);
        m_ctx->DrawRoundedRectangle(rr, brush, thickness);
    }
}

void D2DRenderer::DrawLine(float x1, float y1, float x2, float y2, Color color, float thickness) {
    if (!m_ctx) return;
    m_ctx->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), GetBrush(color), thickness);
}

void D2DRenderer::DrawCircle(float cx, float cy, float radius, Color color) {
    if (!m_ctx) return;
    D2D1_ELLIPSE e = D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius);
    m_ctx->FillEllipse(e, GetBrush(color));
}

void D2DRenderer::DrawCircleOutline(float cx, float cy, float radius, Color color, float thickness) {
    if (!m_ctx) return;
    D2D1_ELLIPSE e = D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius);
    m_ctx->DrawEllipse(e, GetBrush(color), thickness);
}
#pragma endregion primitives

#pragma region gradients

void D2DRenderer::DrawGradientRect(const Rect& rect, Color c1, Color c2,
    bool horizontal, float cornerRadius) {
    if (!m_ctx) return;

    GradientKey2 key{ c1, c2, horizontal };
    auto it = m_gradientCache2.find(key);
    ID2D1LinearGradientBrush* gb = nullptr;

    if (it != m_gradientCache2.end()) {
        gb = it->second.brush.Get();
    } else {
        D2D1_GRADIENT_STOP stops[2];
        stops[0] = { 0.f, c1.ToD2D() };
        stops[1] = { 1.f, c2.ToD2D() };

        CachedGradient cg;
        m_ctx->CreateGradientStopCollection(stops, 2, cg.stops.GetAddressOf());
        if (!cg.stops) return;

        D2D1_POINT_2F start = D2D1::Point2F(0.f, 0.f);
        D2D1_POINT_2F end = horizontal ? D2D1::Point2F(1.f, 0.f) : D2D1::Point2F(0.f, 1.f);
        m_ctx->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(start, end),
            cg.stops.Get(), cg.brush.GetAddressOf());
        if (!cg.brush) return;
        gb = cg.brush.Get();
        m_gradientCache2[key] = std::move(cg);
    }

    D2D1_POINT_2F start = D2D1::Point2F(rect.x, rect.y);
    D2D1_POINT_2F end = horizontal
        ? D2D1::Point2F(rect.x + rect.w, rect.y)
        : D2D1::Point2F(rect.x, rect.y + rect.h);
    gb->SetStartPoint(start);
    gb->SetEndPoint(end);

    if (cornerRadius <= 0.f) {
        m_ctx->FillRectangle(rect.ToD2D(), gb);
    }
    else {
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rect.ToD2D(), cornerRadius, cornerRadius);
        m_ctx->FillRoundedRectangle(rr, gb);
    }
}

void D2DRenderer::DrawGradientCircle(float cx, float cy, float radius,
    Color centerColor, Color edgeColor) {
    if (!m_ctx) return;

    D2D1_GRADIENT_STOP stops[2];
    stops[0] = { 0.f, centerColor.ToD2D() };
    stops[1] = { 1.f, edgeColor.ToD2D() };

    ComPtr<ID2D1GradientStopCollection> coll;
    m_ctx->CreateGradientStopCollection(stops, 2, coll.GetAddressOf());
    if (!coll) return;

    ComPtr<ID2D1RadialGradientBrush> rb;
    m_ctx->CreateRadialGradientBrush(
        D2D1::RadialGradientBrushProperties(
            D2D1::Point2F(cx, cy), D2D1::Point2F(0, 0), radius, radius),
        coll.Get(), rb.GetAddressOf());
    if (!rb) return;

    D2D1_ELLIPSE e = D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius);
    m_ctx->FillEllipse(e, rb.Get());
}

void D2DRenderer::DrawMultiGradientRect(const Rect& rect, const GradientStop* stops, int stopCount,
    bool horizontal, float cornerRadius)
{
    if (!m_ctx || stopCount < 2) return;

    GradientKeyMulti key;
    key.horizontal = horizontal;
    key.colors.resize(stopCount);
    key.positions.resize(stopCount);
    for (int i = 0; i < stopCount; i++) {
        key.colors[i] = stops[i].color;
        key.positions[i] = stops[i].position;
    }

    ID2D1LinearGradientBrush* gb = nullptr;
    auto it = m_gradientCacheMulti.find(key);
    if (it != m_gradientCacheMulti.end()) {
        gb = it->second.brush.Get();
    } else {
        std::vector<D2D1_GRADIENT_STOP> d2dStops(stopCount);
        for (int i = 0; i < stopCount; i++) {
            d2dStops[i] = { stops[i].position, stops[i].color.ToD2D() };
        }

        CachedGradient cg;
        m_ctx->CreateGradientStopCollection(d2dStops.data(), stopCount, cg.stops.GetAddressOf());
        if (!cg.stops) return;

        D2D1_POINT_2F start = D2D1::Point2F(0.f, 0.f);
        D2D1_POINT_2F end = horizontal ? D2D1::Point2F(1.f, 0.f) : D2D1::Point2F(0.f, 1.f);
        m_ctx->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(start, end),
            cg.stops.Get(), cg.brush.GetAddressOf());
        if (!cg.brush) return;
        gb = cg.brush.Get();
        m_gradientCacheMulti[key] = std::move(cg);
    }

    D2D1_POINT_2F start = D2D1::Point2F(rect.x, rect.y);
    D2D1_POINT_2F end = horizontal
        ? D2D1::Point2F(rect.x + rect.w, rect.y)
        : D2D1::Point2F(rect.x, rect.y + rect.h);
    gb->SetStartPoint(start);
    gb->SetEndPoint(end);

    if (cornerRadius <= 0.f) {
        m_ctx->FillRectangle(rect.ToD2D(), gb);
    }
    else {
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rect.ToD2D(), cornerRadius, cornerRadius);
        m_ctx->FillRoundedRectangle(rr, gb);
    }
}
#pragma endregion gradients

#pragma region shadows

void D2DRenderer::DrawDropShadow(const Rect& rect, float cornerRadius, Color shadowColor,
    float offsetX, float offsetY, float blurRadius)
{
    if (!m_ctx) return;

    int layers = (int)(blurRadius / 2.f);
    if (layers < 2) layers = 2;
    if (layers > 8) layers = 8;

    for (int i = layers; i >= 1; --i)
    {
        float expand = (blurRadius / (float)layers) * (float)i;
        float t = (float)i / (float)layers;
        uint8_t alpha = (uint8_t)((float)shadowColor.a * (1.f - t) * (1.f / (float)layers) * 2.f);
        if (alpha == 0) continue;

        Rect shadowRect(
            rect.x + offsetX - expand,
            rect.y + offsetY - expand,
            rect.w + expand * 2.f,
            rect.h + expand * 2.f);

        DrawFilledRect(shadowRect,
            Color(shadowColor.r, shadowColor.g, shadowColor.b, alpha),
            cornerRadius + expand * 0.5f);
    }
}
#pragma endregion shadows

#pragma region images
void* D2DRenderer::FinalizeWicSource(IWICBitmapSource* source) {
    if (!m_ctx || !m_wicFactory || !source) return nullptr;

    ComPtr<IWICFormatConverter> converter;
    HRESULT hr = m_wicFactory->CreateFormatConverter(converter.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    hr = converter->Initialize(source, GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) return nullptr;

    ComPtr<ID2D1Bitmap1> bitmap;
    hr = m_ctx->CreateBitmapFromWicBitmap(converter.Get(), nullptr, bitmap.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    ImageEntry entry;
    entry.bitmap = bitmap;
    m_images.push_back(std::move(entry));
    return (void*)(uintptr_t)m_images.size();
}

void* D2DRenderer::LoadImage(const wchar_t* filePath) {
    if (!m_wicFactory) return nullptr;

    ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(filePath, nullptr,
        GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    return FinalizeWicSource(frame.Get());
}

void* D2DRenderer::LoadImageFromMemory(const void* data, size_t dataSize) {
    if (!m_wicFactory || !data || dataSize == 0) return nullptr;

    ComPtr<IWICStream> stream;
    HRESULT hr = m_wicFactory->CreateStream(stream.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    hr = stream->InitializeFromMemory(
        static_cast<BYTE*>(const_cast<void*>(data)),
        static_cast<DWORD>(dataSize));
    if (FAILED(hr)) return nullptr;

    ComPtr<IWICBitmapDecoder> decoder;
    hr = m_wicFactory->CreateDecoderFromStream(stream.Get(), nullptr,
        WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    return FinalizeWicSource(frame.Get());
}

void* D2DRenderer::LoadImageFromResource(int resourceId, HMODULE hModule) {
    if (!m_wicFactory) return nullptr;

    if (!hModule) hModule = GetModuleHandle(nullptr);

    HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (!hRes) return nullptr;

    HGLOBAL hData = LoadResource(hModule, hRes);
    if (!hData) return nullptr;

    void* data = LockResource(hData);
    DWORD dataSize = SizeofResource(hModule, hRes);
    if (!data || dataSize == 0) return nullptr;

    ComPtr<IWICStream> stream;
    HRESULT hr = m_wicFactory->CreateStream(stream.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    hr = stream->InitializeFromMemory(static_cast<BYTE*>(data), dataSize);
    if (FAILED(hr)) return nullptr;

    ComPtr<IWICBitmapDecoder> decoder;
    hr = m_wicFactory->CreateDecoderFromStream(stream.Get(), nullptr,
        WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    return FinalizeWicSource(frame.Get());
}

void D2DRenderer::DrawImage(void* handle, const Rect& dest, float opacity) {
    if (!m_ctx || !handle) return;
    size_t idx = (size_t)(uintptr_t)handle - 1;
    if (idx >= m_images.size() || !m_images[idx].bitmap) return;
    m_ctx->DrawBitmap(m_images[idx].bitmap.Get(), dest.ToD2D(), opacity,
        D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
}

void D2DRenderer::DrawImageRegion(void* handle, const Rect& srcRect, const Rect& destRect, float opacity) {
    if (!m_ctx || !handle) return;
    size_t idx = (size_t)(uintptr_t)handle - 1;
    if (idx >= m_images.size() || !m_images[idx].bitmap) return;
    D2D1_RECT_F src = D2D1::RectF(srcRect.x, srcRect.y, srcRect.x + srcRect.w, srcRect.y + srcRect.h);
    m_ctx->DrawBitmap(m_images[idx].bitmap.Get(), destRect.ToD2D(), opacity,
        D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, &src);
}

void D2DRenderer::FreeImage(void* handle) {
    if (!handle) return;
    size_t idx = (size_t)(uintptr_t)handle - 1;
    if (idx < m_images.size()) m_images[idx].bitmap.Reset();
}
#pragma endregion images

#pragma region text

void D2DRenderer::DrawText(const std::wstring& text, const Rect& rect, Color color,
    int fontSize, TextAnchor anchor, const wchar_t* fontFamily) {
    if (!m_ctx || !m_dwrite) return;

    DWRITE_TEXT_ALIGNMENT hAlign;
    DWRITE_PARAGRAPH_ALIGNMENT vAlign;
    AnchorToAlignment(anchor, hAlign, vAlign);

    auto* fmt = GetTextFormat(fontFamily, (float)fontSize, hAlign, vAlign);
    if (!fmt) return;
    m_ctx->DrawText(text.c_str(), (UINT32)text.length(), fmt, rect.ToD2D(), GetBrush(color));
}

Rect D2DRenderer::MeasureText(const std::wstring& text, int fontSize, const wchar_t* fontFamily) {
    if (!m_dwrite) return {};

    MeasureTextKey key{ text, fontSize, fontFamily ? fontFamily : L"" };
    auto it = m_measureTextCache.find(key);
    if (it != m_measureTextCache.end())
        return it->second;

    auto* fmt = GetTextFormat(fontFamily, (float)fontSize,
        DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    if (!fmt) return {};

    ComPtr<IDWriteTextLayout> layout;
    m_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.length(), fmt,
        100000.f, 100000.f, layout.GetAddressOf());
    if (!layout) return {};

    DWRITE_TEXT_METRICS metrics;
    layout->GetMetrics(&metrics);
    Rect result(0.0, 0.0, (float)metrics.widthIncludingTrailingWhitespace, metrics.height);
    m_measureTextCache[key] = result;
    return result;
}
void D2DRenderer::SetBackdropBitmap(ID2D1Bitmap1* backdrop) {
    m_backdrop = backdrop;
}
#pragma endregion text

#pragma region glassmorphism

void D2DRenderer::DrawGlassRect(const Rect& rect, float blurRadius,
    Color tint, float cornerRadius, float borderOpacity)
{
    if (!m_ctx || !m_backdrop) return;

    auto pixelSize = m_backdrop->GetPixelSize();
    float sx = (std::max)(rect.x, 0.f);
    float sy = (std::max)(rect.y, 0.f);
    float sw = (std::min)(rect.x + rect.w, (float)pixelSize.width) - sx;
    float sh = (std::min)(rect.y + rect.h, (float)pixelSize.height) - sy;
    if (sw <= 0.f || sh <= 0.f) return;

    D2D1_SIZE_U copySize = D2D1::SizeU((UINT32)sw, (UINT32)sh);
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_NONE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

    ComPtr<ID2D1Bitmap1> snapshot;
    HRESULT hr = m_ctx->CreateBitmap(copySize, nullptr, 0, props, snapshot.GetAddressOf());
    if (FAILED(hr)) return;

    D2D1_POINT_2U destPt = D2D1::Point2U(0, 0);
    D2D1_RECT_U srcRegion = D2D1::RectU(
        (UINT32)sx, (UINT32)sy, (UINT32)(sx + sw), (UINT32)(sy + sh));

    hr = snapshot->CopyFromBitmap(&destPt, m_backdrop, &srcRegion);
    if (FAILED(hr)) return;

    EnsureGlassEffects();
    if (!m_blurEffect || !m_cropEffect) return;

    m_blurEffect->SetInput(0, snapshot.Get());
    m_blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurRadius);
    m_blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);

    m_cropEffect->SetInputEffect(0, m_blurEffect.Get());
    D2D1_VECTOR_4F cropRect = { 0.f, 0.f, sw, sh };
    m_cropEffect->SetValue(D2D1_CROP_PROP_RECT, cropRect);

    if (cornerRadius > 0.f) {
        ComPtr<ID2D1Factory> factory;
        m_ctx->GetFactory(factory.GetAddressOf());
        ComPtr<ID2D1Factory1> factory1;
        factory.As(&factory1);

        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rect.ToD2D(), cornerRadius, cornerRadius);
        ComPtr<ID2D1RoundedRectangleGeometry> geo;
        factory1->CreateRoundedRectangleGeometry(rr, geo.GetAddressOf());

        D2D1_LAYER_PARAMETERS1 layerParams = {};
        layerParams.contentBounds = rect.ToD2D();
        layerParams.geometricMask = geo.Get();
        layerParams.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
        layerParams.maskTransform = D2D1::Matrix3x2F::Identity();
        layerParams.opacity = 1.f;
        layerParams.opacityBrush = nullptr;
        layerParams.layerOptions = D2D1_LAYER_OPTIONS1_NONE;

        m_ctx->PushLayer(layerParams, nullptr);

        m_ctx->DrawImage(m_cropEffect.Get(), D2D1::Point2F(sx, sy));

        D2D1_ROUNDED_RECT tintRR = D2D1::RoundedRect(rect.ToD2D(), cornerRadius, cornerRadius);
        m_ctx->FillRoundedRectangle(tintRR, GetBrush(tint));

        m_ctx->PopLayer();
    }
    else {
        m_ctx->PushAxisAlignedClip(rect.ToD2D(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        m_ctx->DrawImage(m_cropEffect.Get(), D2D1::Point2F(sx, sy));
        m_ctx->FillRectangle(rect.ToD2D(), GetBrush(tint));
        m_ctx->PopAxisAlignedClip();
    }

    m_blurEffect->SetInput(0, nullptr);
    m_cropEffect->SetInput(0, nullptr);

    if (borderOpacity > 0.f) {
        Color borderColor(255, 255, 255, (uint8_t)(borderOpacity * 255.f));
        DrawOutlineRect(rect, borderColor, 1.f, cornerRadius);
    }
}
#pragma endregion glassmorphism