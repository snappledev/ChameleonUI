#include "../../include/rendering/desktopcapture.h"

using Microsoft::WRL::ComPtr;

#pragma region lifecycle
bool DesktopCapture::Initialize(ID3D11Device* d3dDevice, ID2D1DeviceContext* d2dContext)
{
    m_d3dDevice = d3dDevice;
    m_d2dContext = d2dContext;
    m_d3dDevice->GetImmediateContext(m_d3dImmCtx.GetAddressOf());
    return CreateDuplication();
}
#pragma endregion

#pragma region capture
bool DesktopCapture::CreateDuplication()
{
    m_duplication.Reset();

    ComPtr<IDXGIDevice> dxgiDevice;
    m_d3dDevice->QueryInterface(dxgiDevice.GetAddressOf());

    ComPtr<IDXGIAdapter> adapter;
    dxgiDevice->GetAdapter(adapter.GetAddressOf());

    ComPtr<IDXGIOutput> output;
    if (FAILED(adapter->EnumOutputs(0, output.GetAddressOf())))
        return false;

    ComPtr<IDXGIOutput1> output1;
    output.As(&output1);
    if (!output1) return false;

    HRESULT hr = output1->DuplicateOutput(m_d3dDevice, m_duplication.GetAddressOf());
    return SUCCEEDED(hr);
}

bool DesktopCapture::CreateStagingResources(UINT width, UINT height)
{
    m_stagingTex.Reset();
    m_bitmap.Reset();
    m_width = width;
    m_height = height;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

    HRESULT hr = m_d3dDevice->CreateTexture2D(&desc, nullptr, m_stagingTex.GetAddressOf());
    if (FAILED(hr)) return false;

    ComPtr<IDXGISurface> surface;
    m_stagingTex.As(&surface);

    D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_NONE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

    hr = m_d2dContext->CreateBitmapFromDxgiSurface(
        surface.Get(), &bitmapProps, m_bitmap.GetAddressOf());
    return SUCCEEDED(hr);
}

ID2D1Bitmap1* DesktopCapture::AcquireFrame()
{
    if (!m_duplication) {
        if (!CreateDuplication())
            return m_bitmap.Get();
    }

    ComPtr<IDXGIResource> resource;
    DXGI_OUTDUPL_FRAME_INFO frameInfo = {};

    HRESULT hr = m_duplication->AcquireNextFrame(0, &frameInfo, resource.GetAddressOf());

    if (hr == DXGI_ERROR_ACCESS_LOST) {
        m_duplication.Reset();
        return m_bitmap.Get();
    }

    if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        return m_bitmap.Get();
    }

    if (FAILED(hr))
        return m_bitmap.Get();

    ComPtr<ID3D11Texture2D> desktopTex;
    resource.As(&desktopTex);

    if (desktopTex) {
        D3D11_TEXTURE2D_DESC desc;
        desktopTex->GetDesc(&desc);

        if (!m_stagingTex || desc.Width != m_width || desc.Height != m_height) {
            if (!CreateStagingResources(desc.Width, desc.Height)) {
                m_duplication->ReleaseFrame();
                return nullptr;
            }
        }

        m_d3dImmCtx->CopyResource(m_stagingTex.Get(), desktopTex.Get());
    }

    m_duplication->ReleaseFrame();
    return m_bitmap.Get();
}
#pragma endregion

#pragma region shutdown
void DesktopCapture::Shutdown()
{
    m_bitmap.Reset();
    m_stagingTex.Reset();
    if (m_duplication) {
        m_duplication->ReleaseFrame();
        m_duplication.Reset();
    }
    m_d3dImmCtx.Reset();
    m_d3dDevice = nullptr;
    m_d2dContext = nullptr;
}
#pragma endregion