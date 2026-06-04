#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <wrl/client.h>

#include "Renderer.h"
#include "UIManager.h"
#include "testUI.h"
#include "exampleUI.h"
#include "DesktopCapture.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dcomp.lib")

using Microsoft::WRL::ComPtr;

// globals
static ComPtr<ID3D11Device>        g_d3dDevice;
static ComPtr<ID3D11DeviceContext>  g_d3dContext;
static ComPtr<IDXGISwapChain1>      g_swapChain;
static ComPtr<ID2D1Factory1>        g_d2dFactory;
static ComPtr<ID2D1Device>          g_d2dDevice;
static ComPtr<ID2D1DeviceContext>   g_d2dContext;
static ComPtr<IDWriteFactory>       g_dwriteFactory;

static ComPtr<IDCompositionDevice>  g_dcompDevice;
static ComPtr<IDCompositionTarget>  g_dcompTarget;
static ComPtr<IDCompositionVisual>  g_dcompVisual;

D2DRenderer    g_renderer;
testUI         g_ui;
DesktopCapture g_capture;
static HWND    g_hwnd = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (g_ui.HandleMessage(hwnd, msg, wParam, lParam) == 0)
        return 0;
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool InitD3D(HWND hwnd) {
    HRESULT hr;

    D3D_FEATURE_LEVEL featureLevel;
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0,
        createDeviceFlags, nullptr, 0,
        D3D11_SDK_VERSION,
        g_d3dDevice.GetAddressOf(), &featureLevel, g_d3dContext.GetAddressOf());
    if (FAILED(hr)) return false;

    ComPtr<IDXGIDevice> dxgiDevice;
    g_d3dDevice.As(&dxgiDevice);
    ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
    ComPtr<IDXGIFactory2> dxgiFactory;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = (UINT)GetSystemMetrics(SM_CXSCREEN);
    sd.Height = (UINT)GetSystemMetrics(SM_CYSCREEN);
    sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    sd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    sd.Flags = 0;

    hr = dxgiFactory->CreateSwapChainForComposition(
        g_d3dDevice.Get(), &sd, nullptr, g_swapChain.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = DCompositionCreateDevice(dxgiDevice.Get(),
        __uuidof(IDCompositionDevice), &g_dcompDevice);
    if (FAILED(hr)) return false;

    hr = g_dcompDevice->CreateTargetForHwnd(hwnd, TRUE, g_dcompTarget.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = g_dcompDevice->CreateVisual(g_dcompVisual.GetAddressOf());
    if (FAILED(hr)) return false;

    g_dcompVisual->SetContent(g_swapChain.Get());
    g_dcompTarget->SetRoot(g_dcompVisual.Get());
    g_dcompDevice->Commit();

    D2D1_FACTORY_OPTIONS options = {};
#ifdef _DEBUG
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory1), &options, &g_d2dFactory);
    if (FAILED(hr)) return false;

    hr = g_d2dFactory->CreateDevice(dxgiDevice.Get(), g_d2dDevice.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = g_d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE, g_d2dContext.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory), &g_dwriteFactory);
    if (FAILED(hr)) return false;

    g_renderer.Initialize(g_d2dContext.Get(), g_dwriteFactory.Get());

    if (!g_capture.Initialize(g_d3dDevice.Get(), g_d2dContext.Get())) {
        OutputDebugStringW(L"DesktopCapture init failed — glass blur will be unavailable\n");
    }

    return true;
}

void CleanupD3D() {
    g_capture.Shutdown();
    g_renderer.Shutdown();
    g_d2dContext.Reset();
    g_d2dDevice.Reset();
    g_d2dFactory.Reset();
    g_dwriteFactory.Reset();
    g_dcompVisual.Reset();
    g_dcompTarget.Reset();
    g_dcompDevice.Reset();
    g_swapChain.Reset();
    g_d3dContext.Reset();
    g_d3dDevice.Reset();
}

static ID2D1Bitmap1* g_cachedBackdrop = nullptr;

void RenderFrame() {
    ComPtr<IDXGISurface> dxgiSurface;
    g_swapChain->GetBuffer(0, __uuidof(IDXGISurface), &dxgiSurface);

    D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

    ComPtr<ID2D1Bitmap1> d2dTarget;
    HRESULT hr = g_d2dContext->CreateBitmapFromDxgiSurface(
        dxgiSurface.Get(), &bitmapProps, d2dTarget.GetAddressOf());
    if (FAILED(hr)) return;

    g_d2dContext->SetTarget(d2dTarget.Get());
    g_d2dContext->BeginDraw();
    g_d2dContext->Clear(D2D1::ColorF(0.f, 0.f, 0.f, 0.f));

    g_renderer.BeginFrame();

    g_renderer.SetBackdropBitmap(g_cachedBackdrop);

    g_ui.Render();

    g_renderer.SetBackdropBitmap(nullptr);
    g_d2dContext->EndDraw();
    g_swapChain->Present(1, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    const wchar_t CLASS_NAME[] = L"DXUITestWindow";
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = CLASS_NAME;
    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, L"Failed to register class", L"Error", MB_OK);
        return 1;
    }

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        CLASS_NAME, L"DX UI Test",
        WS_POPUP, 0, 0, screenW, screenH,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) {
        DWORD err = GetLastError();
        wchar_t buf[128];
        swprintf_s(buf, L"CreateWindowEx failed, error code: %lu", err);
        MessageBox(nullptr, buf, L"Fatal Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    g_hwnd = hwnd;

    if (!InitD3D(hwnd)) {
        MessageBox(nullptr, L"DirectX init failed", L"Error", MB_OK);
        return 1;
    }

    // Capture clean desktop backdrop before the window appears on screen,
    // so the glass blur has a static reference without any feedback loop.
    DwmFlush();
    g_cachedBackdrop = g_capture.AcquireFrame();

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    bool running = true;
    g_ui.Initialize(hwnd, &g_renderer);

    MSG msg = {};
    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) running = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        g_ui.NewFrame();
        RenderFrame();
    }

    g_ui.Shutdown();
    CleanupD3D();
    CoUninitialize();
    return 0;
}