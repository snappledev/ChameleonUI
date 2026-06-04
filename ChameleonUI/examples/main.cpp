#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <wrl/client.h>
#include "exampleUI.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dcomp.lib")

using Microsoft::WRL::ComPtr;

// host-owned d3d objects; ui manager creates d2d/dwrite internally
static ComPtr<ID3D11Device>        g_d3dDevice;
static ComPtr<ID3D11DeviceContext>  g_d3dContext;
static ComPtr<IDXGISwapChain1>      g_swapChain;

// dcomp for transparent overlay window
static ComPtr<IDCompositionDevice>  g_dcompDevice;
static ComPtr<IDCompositionTarget>  g_dcompTarget;
static ComPtr<IDCompositionVisual>  g_dcompVisual;

ExampleUI g_ui;

static HWND g_hwnd = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        // esc/insert to close overlay
        if (wParam == VK_ESCAPE)
        {
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    }

    // let ui manager handle input first
    //////////////////////////
    if (g_ui.HandleMessage(hwnd, msg, wParam, lParam) == 0)
        return 0;
    //////////////////////////

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool InitD3D(HWND hwnd)
{
    HRESULT hr;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL featureLevel;
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flags,
        nullptr, 0, D3D11_SDK_VERSION,
        g_d3dDevice.GetAddressOf(), &featureLevel, g_d3dContext.GetAddressOf());
    if (FAILED(hr)) return false;

    ComPtr<IDXGIDevice> dxgiDevice;
    g_d3dDevice.As(&dxgiDevice);
    ComPtr<IDXGIAdapter> adapter;
    dxgiDevice->GetAdapter(adapter.GetAddressOf());
    ComPtr<IDXGIFactory2> factory;
    adapter->GetParent(__uuidof(IDXGIFactory2), &factory);

    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = (UINT)GetSystemMetrics(SM_CXSCREEN);
    sd.Height = (UINT)GetSystemMetrics(SM_CYSCREEN);
    sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    sd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    hr = factory->CreateSwapChainForComposition(
        g_d3dDevice.Get(), &sd, nullptr, g_swapChain.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = DCompositionCreateDevice(dxgiDevice.Get(),
        __uuidof(IDCompositionDevice), &g_dcompDevice);
    if (FAILED(hr)) return false;

    g_dcompDevice->CreateTargetForHwnd(hwnd, TRUE, g_dcompTarget.GetAddressOf());
    g_dcompDevice->CreateVisual(g_dcompVisual.GetAddressOf());
    g_dcompVisual->SetContent(g_swapChain.Get());
    g_dcompTarget->SetRoot(g_dcompVisual.Get());
    g_dcompDevice->Commit();

    return true;
}

void CleanupD3D()
{
    g_dcompVisual.Reset();
    g_dcompTarget.Reset();
    g_dcompDevice.Reset();
    g_swapChain.Reset();
    g_d3dContext.Reset();
    g_d3dDevice.Reset();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    const wchar_t CLASS_NAME[] = L"DXUITestWindow";
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassEx(&wc)) 
        return 1;

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW, CLASS_NAME, L"DX UI Test", WS_POPUP, 0, 0, screenW, screenH, nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) 
        return 1;

    g_hwnd = hwnd;

    if (!InitD3D(hwnd)) return 1;

    //////////////////////////
    if (!g_ui.Initialize(hwnd, g_d3dDevice.Get(), g_swapChain.Get()))
    {
        MessageBox(nullptr, L"UIFramework init failed", L"Error", MB_OK);
        return 1;
    }

    // Capture clean desktop backdrop before the window appears on screen,
    // so the glass blur has a static reference without any feedback loop.
    g_ui.CaptureBackdrop();
    //////////////////////////

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {};
    bool running = true;
    while (running)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) running = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        ///////////////////////////////
        g_ui.NewFrame();
        g_ui.Render();

        g_swapChain->Present(1, 0);
    }

    //////////////////////////
    g_ui.Shutdown();
    //////////////////////////
    CleanupD3D();
    CoUninitialize();
    return 0;
}