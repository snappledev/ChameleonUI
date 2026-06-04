#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <wrl/client.h>

class DesktopCapture {
public:
    bool Initialize(ID3D11Device* d3dDevice, ID2D1DeviceContext* d2dContext);
    void Shutdown();

    // returns latest desktop frame as d2d bitmap, or last captured if no new frame
    ID2D1Bitmap1* AcquireFrame();

private:
    ID3D11Device*                                  m_d3dDevice  = nullptr;
    ID2D1DeviceContext*                            m_d2dContext = nullptr;

    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_d3dImmCtx;
    Microsoft::WRL::ComPtr<IDXGIOutputDuplication> m_duplication;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_stagingTex;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1>           m_bitmap;

    UINT m_width  = 0;
    UINT m_height = 0;

    bool CreateDuplication();
    bool CreateStagingResources(UINT width, UINT height);
};