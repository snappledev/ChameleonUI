#include "../../include/core/ChameleonUI.h"
#include "../../include/core/UIContext.h"
#include "../../include/skins/DefaultSkin.h"
#include "../../include/core/UIStyle.h"
#include "../../include/util/scoped_timer.h"
#include "../../include/elements/ColorSwatch.h"
#include "../../include/elements/TabControl.h"
#include "../../include/elements/ColorPickerModal.h"
#include "../../include/elements/colorpicker.h"
#include "../../include/elements/titlebar.h"
#include "../../include/layouts/verticalpanel.h"
#include <cassert>
#include <windowsx.h>
#include <chrono>
#include "../../include/assets/lang/lang_en.h"
#include "../../include/assets/lang/lang_ru.h"

#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <wrl/client.h>
#include "../../include/rendering/desktopcapture.h"


using Microsoft::WRL::ComPtr;

struct ChameleonUI::ManagedGraphics
{
    ComPtr<ID2D1Factory1>      d2dFactory;
    ComPtr<ID2D1Device>        d2dDevice;
    ComPtr<ID2D1DeviceContext> d2dContext;
    ComPtr<IDWriteFactory>     dwriteFactory;

    D2DRenderer renderer;

    ComPtr<IDXGISwapChain1> swapChain;

    std::unique_ptr<DesktopCapture> capture;
};

#pragma region lifecycle
ChameleonUI::ChameleonUI()
    : m_context(std::make_unique<UIContext>())
{}

std::unique_ptr<Skin> ChameleonUI::CreateSkin()
{
    return std::make_unique<DefaultSkin>();
}

ChameleonUI::~ChameleonUI() = default;

void ChameleonUI::InitializeCommon()
{
    if (!m_skin)
    {
        m_skin = CreateSkin();
        UIElement::SetDefaultSkin(m_skin.get());
    }

    m_config.SetAppName(GetConfigAppName());
    RegisterBindings();
    m_config.SnapshotDefaults();
    m_config.LoadConfig(GetDefaultConfigName());

    m_context->Initialize(m_hwnd);

    LoadLanguages();
    if (m_localization.GetLanguage().empty())
        m_localization.SetLanguage(L"en");

    OnBeforeBuildUI();
    BuildUI();

    m_context->Update(0.0f);
}


bool ChameleonUI::Initialize(HWND hwnd, ID3D11Device* d3dDevice, IDXGISwapChain1* swapChain)
{
    assert(d3dDevice && "ChameleonUI::Initialize requires a valid ID3D11Device");
    assert(swapChain && "ChameleonUI::Initialize requires a valid IDXGISwapChain1");

    m_hwnd = hwnd;

    auto gfx = std::make_unique<ManagedGraphics>();
    gfx->swapChain = swapChain;

    HRESULT hr;

    D2D1_FACTORY_OPTIONS opts = {};
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory1), &opts,
        reinterpret_cast<void**>(gfx->d2dFactory.GetAddressOf()));
    if (FAILED(hr)) return false;

    ComPtr<IDXGIDevice> dxgiDevice;
    hr = d3dDevice->QueryInterface(dxgiDevice.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = gfx->d2dFactory->CreateDevice(dxgiDevice.Get(), gfx->d2dDevice.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = gfx->d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE, gfx->d2dContext.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(gfx->dwriteFactory.GetAddressOf()));
    if (FAILED(hr)) return false;

    gfx->renderer.Initialize(gfx->d2dContext.Get(), gfx->dwriteFactory.Get());
    m_renderer = &gfx->renderer;

    if (m_desktopCaptureEnabled)
    {
        gfx->capture = std::make_unique<DesktopCapture>();
        if (!gfx->capture->Initialize(d3dDevice, gfx->d2dContext.Get()))
        {
            OutputDebugStringW(L"UIFramework: DesktopCapture init failed — glass blur unavailable\n");
            gfx->capture.reset();
        }
    }

    m_gfx = std::move(gfx);

    InitializeCommon();
    return true;
}

void ChameleonUI::Initialize(HWND hwnd, IRenderer* renderer)
{
    m_hwnd = hwnd;
    m_renderer = renderer;
    InitializeCommon();
}

void ChameleonUI::LoadLanguages()
{
    m_localization.RegisterLanguage(L"en", kStrings_en, _countof(kStrings_en));
    m_localization.RegisterLanguage(L"ru", kStrings_ru, _countof(kStrings_ru));
}

void ChameleonUI::Shutdown()
{
    for (void* img : m_managedImages)
        if (m_renderer) m_renderer->FreeImage(img);
    m_managedImages.clear();

    m_iconLib.FreeAll(m_renderer);

    m_context->Shutdown();

    if (m_gfx)
    {
        if (m_gfx->capture)
        {
            m_gfx->capture->Shutdown();
            m_gfx->capture.reset();
        }
        m_gfx->renderer.Shutdown();
        m_gfx.reset();
    }

    m_renderer = nullptr;
    m_cachedBackdrop = nullptr;
    while (!m_parentStack.empty())
        m_parentStack.pop();
}

void ChameleonUI::SetDesktopCaptureEnabled(bool enabled)
{
    if (m_desktopCaptureEnabled == enabled)
        return;

    m_desktopCaptureEnabled = enabled;

    if (m_gfx)
    {
        if (enabled && !m_gfx->capture)
        {
            ComPtr<IDXGIDevice> dxgiDevice;
            m_gfx->d2dDevice->QueryInterface(
                __uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgiDevice.GetAddressOf()));

            if (dxgiDevice)
            {
                ComPtr<ID3D11Device> d3dDevice;
                dxgiDevice.As(&d3dDevice);

                if (d3dDevice)
                {
                    m_gfx->capture = std::make_unique<DesktopCapture>();
                    if (!m_gfx->capture->Initialize(d3dDevice.Get(), m_gfx->d2dContext.Get()))
                    {
                        OutputDebugStringW(L"UIFramework: DesktopCapture re-init failed\n");
                        m_gfx->capture.reset();
                    }
                }
            }
        }
        else if (!enabled && m_gfx->capture)
        {
            m_gfx->capture->Shutdown();
            m_gfx->capture.reset();
        }
    }
}

void ChameleonUI::SetClearColor(float r, float g, float b, float a)
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

void ChameleonUI::CaptureBackdrop()
{
    if (!m_desktopCaptureEnabled || !m_gfx || !m_gfx->capture || !m_hwnd)
    {
        m_cachedBackdrop = nullptr;
        return;
    }

    // Only capture once — a static snapshot avoids the feedback loop
    // where the window's own rendered content gets re-blurred.
    if (m_cachedBackdrop)
        return;

    DwmFlush();
    m_cachedBackdrop = m_gfx->capture->AcquireFrame();
}
#pragma endregion

#pragma region frame
void ChameleonUI::NewFrame()
{
    static auto lastTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;
    if (dt > 0.1f)    dt = 0.1f;
    if (dt < 0.0001f) dt = 0.0001f;

    if (!m_pendingLanguage.empty())
    {
        std::wstring lang = std::move(m_pendingLanguage);
        m_pendingLanguage.clear();
        if (m_localization.SetLanguage(lang))
            RebuildUI();
    }

    if (m_context)
    {
        if (!m_context->IsDragging())
            m_context->PerformLayout();
        m_context->Update(dt);
    }
}

bool ChameleonUI::LoadConfig(const std::wstring& name)
{
    if (!m_config.LoadConfig(name))
        return false;

    if (m_context)
    {
        for (auto& win : m_context->GetWindows())
            win->SyncAllFromBindings();
        m_context->SetLayoutDirty();
    }

    return true;
}

void ChameleonUI::ResetConfig()
{
    m_config.ResetAll();
    if (m_context)
    {
        for (auto& win : m_context->GetWindows())
            win->SyncAllFromBindings();
        m_context->SetLayoutDirty();
    }
}

void ChameleonUI::BuildUI() { /* Override in child class */ }

void ChameleonUI::Render()
{
    if (!m_renderer || !m_context)
        return;

    if (m_gfx)
    {
        // Use cached backdrop from CaptureBackdrop() called after previous Present().
        ID2D1Bitmap1* backdrop = m_cachedBackdrop;

        ComPtr<IDXGISurface> surface;
        HRESULT hr = m_gfx->swapChain->GetBuffer(
            0, __uuidof(IDXGISurface), &surface);
        if (FAILED(hr)) return;

        D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                D2D1_ALPHA_MODE_PREMULTIPLIED));

        ComPtr<ID2D1Bitmap1> target;
        hr = m_gfx->d2dContext->CreateBitmapFromDxgiSurface(
            surface.Get(), &bitmapProps, target.GetAddressOf());
        if (FAILED(hr)) return;

        m_gfx->d2dContext->SetTarget(target.Get());
        m_gfx->d2dContext->BeginDraw();
        m_gfx->d2dContext->Clear(D2D1::ColorF(
            m_clearColor[0], m_clearColor[1],
            m_clearColor[2], m_clearColor[3]));

        m_gfx->renderer.BeginFrame();
        m_gfx->renderer.SetBackdropBitmap(backdrop);

        m_context->Draw(m_renderer);

        m_gfx->renderer.SetBackdropBitmap(nullptr);
        m_gfx->d2dContext->EndDraw();
    }
    else
    {
        m_context->Draw(m_renderer);
    }
}
#pragma endregion

#pragma region message handling
LRESULT ChameleonUI::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_MOUSEMOVE:   m_context->SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
    case WM_LBUTTONDOWN: m_context->SetMouseButtonDown(0); break;
    case WM_LBUTTONUP:   m_context->SetMouseButtonUp(0);   break;
    case WM_RBUTTONDOWN: m_context->SetMouseButtonDown(1); break;
    case WM_RBUTTONUP:   m_context->SetMouseButtonUp(1);   break;
    case WM_MBUTTONDOWN: m_context->SetMouseButtonDown(2); break;
    case WM_MBUTTONUP:   m_context->SetMouseButtonUp(2);   break;
    case WM_MOUSEWHEEL:  m_context->SetMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)); break;
    case WM_KEYDOWN:     m_context->OnKeyDown((int)wParam); break;
    case WM_KEYUP:       m_context->OnKeyUp((int)wParam);   break;
    case WM_CHAR:        m_context->OnChar((wchar_t)wParam); break;
    case WM_SIZE:
        if (m_context) m_context->SetLayoutDirty();
        break;
    default:             return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    m_context->ProcessInput();
    return 0;
}
#pragma endregion

#pragma region parent stack
void ChameleonUI::PushParent(UIElement* parent) { m_parentStack.push(parent); }
void ChameleonUI::PopParent() { if (!m_parentStack.empty()) m_parentStack.pop(); }
UIElement* ChameleonUI::GetCurrentParent()
{
    return m_parentStack.empty() ? nullptr : m_parentStack.top();
}

Window* ChameleonUI::GetCurrentWindow()
{
    UIElement* p = GetCurrentParent();
    while (p)
    {
        if (auto* win = dynamic_cast<Window*>(p)) return win;
        p = p->GetParent();
    }
    return nullptr;
}
#pragma endregion

#pragma region managed images
void* ChameleonUI::LoadManagedImage(const wchar_t* filePath)
{
    void* h = m_renderer->LoadImage(filePath);
    if (h) m_managedImages.push_back(h);
    return h;
}

void* ChameleonUI::LoadManagedImageFromResource(int resourceId)
{
    void* h = m_renderer->LoadImageFromResource(resourceId);
    if (h) m_managedImages.push_back(h);
    return h;
}
#pragma endregion

#pragma region shared helpers
void ChameleonUI::ApplyProps(UIElement& el, const UIProps& props)
{
    el.SetPadding(props.paddingLeft, props.paddingTop, props.paddingRight, props.paddingBottom);
    el.SetMargin(props.marginLeft, props.marginTop, props.marginRight, props.marginBottom);
    el.SetWidthPolicy(props.widthPolicy);
    el.SetHeightPolicy(props.heightPolicy);
    el.SetHorizontalAlignment(props.hAlign);

    if (props.fixedWidth > 0)  el.SetWidth(props.fixedWidth);
    if (props.fixedHeight > 0) el.SetHeight(props.fixedHeight);

    if (props.skin) el.SetSkin(props.skin);

    if (!props.tooltip.empty()) el.SetTooltip(props.tooltip);
}

UIElement* ChameleonUI::BuildContentContainer(UIElement* parent, const UIProps& props)
{
    if (props.scrollable)
    {
        auto& panel = parent->AddChild<Panel>();
        panel.SetScrollMode(ScrollMode::Vertical);
        panel.SetPadding(0, 0, 0, 0);
        panel.SetMargin(0, 0, 0, 0);
        if (props.skin) panel.SetSkin(props.skin);

        auto& vp = panel.AddChild<VerticalPanel>();
        vp.SetPadding(0, 0, 0, 0);
        vp.SetMargin(0, 0, 0, 0);
        if (props.skin) vp.SetSkin(props.skin);
        return &vp;
    }

    auto& vp = parent->AddChild<VerticalPanel>();
    vp.SetPadding(0, 0, 0, 0);
    vp.SetMargin(0, 0, 0, 0);
    if (props.skin) vp.SetSkin(props.skin);
    return &vp;
}

void ChameleonUI::EnsureColorPickerModal()
{
    if (m_colorPickerModal) return;
    m_colorPickerModalOwned = std::make_unique<ColorPickerModal>();
    m_colorPickerModal = m_colorPickerModalOwned.get();
    m_context->SetColorPickerModal(m_colorPickerModal);
}

void ChameleonUI::AttachColorSwatch(ControlGroup& group, const UIProps& props)
{
    if (!props.bindColor || props.swatchAlignment == ColorSwatchAlignment::None)
        return;

    EnsureColorPickerModal();

    auto& swatch = group.AddChild<ColorSwatch>();
    swatch.SetLinkedColor(props.bindColor);
    swatch.SetColor(*props.bindColor);
    swatch.SetModal(m_colorPickerModal);

    Skin* skin = swatch.GetEffectiveSkin();
    if (props.swatchWidth > 0 && props.swatchHeight > 0)
        swatch.SetSwatchSize(props.swatchWidth, props.swatchHeight);
    else if (skin && skin->GetStyle())
        swatch.SetSwatchSize(skin->GetStyle()->colorSwatchDefaultWidth,
            skin->GetStyle()->colorSwatchDefaultHeight);

    group.SetColorSwatch(&swatch);
    group.SetSwatchAlignment(props.swatchAlignment);
    group.SetSwatchSpacing(props.swatchSpacing);
}
#pragma endregion

#pragma region binding helpers
void ChameleonUI::ApplyBinding(Checkbox& ctrl, const UIProps& props)
{
    if (props.bindBool) ctrl.SetLinkedBool(props.bindBool);
}

void ChameleonUI::ApplyBinding(Slider& ctrl, const UIProps& props)
{
    if (props.bindFloat) ctrl.SetLinkedFloat(props.bindFloat);
}

void ChameleonUI::ApplyBinding(ComboBox& ctrl, const UIProps& props)
{
    if (props.bindInt)
    {
        ctrl.SetLinkedInt(props.bindInt);
        ctrl.SetSelectedIndex(*props.bindInt);
    }
}

void ChameleonUI::ApplyBinding(TextInput& ctrl, const UIProps& props)
{
    if (props.bindString) ctrl.SetLinkedString(props.bindString);
}

void ChameleonUI::ApplyBinding(KeyBind& ctrl, const UIProps& props)
{
    if (props.bindInt) ctrl.SetLinkedInt(props.bindInt);
}
#pragma endregion

#pragma region label wrapper
template<typename TControl>
TControl& ChameleonUI::WrapWithLabel(const UIProps& props, TControl& ctrl, bool clickToggles)
{
    ControlGroup* group = dynamic_cast<ControlGroup*>(ctrl.GetParent());
    if (!group) return ctrl;

    SizePolicy ctrlWidth = (props.widthPolicy == SizePolicy::ContentFit)
        ? SizePolicy::ContentFit : SizePolicy::Stretch;
    ctrl.SetWidthPolicy(ctrlWidth);
    ctrl.SetHeightPolicy(SizePolicy::ContentFit);

    std::wstring lblText = props.labelText.empty() ? props.text : props.labelText;
    auto& lbl = group->AddChild<Label>(lblText);
    lbl.SetRenderer(m_renderer);
    lbl.SetWidthPolicy(SizePolicy::ContentFit);
    lbl.SetHeightPolicy(SizePolicy::ContentFit);

    group->SetControl(&ctrl);
    group->SetLabel(&lbl);
    group->SetLabelPosition(props.labelPosition);
    group->SetControlIndent(props.controlIndent);

    float spacing = props.labelSpacing;
    if (spacing <= 0.f)
    {
        Skin* skin = ctrl.GetEffectiveSkin();
        spacing = (skin && skin->GetStyle()) ? skin->GetStyle()->checkboxLabelSpacing : 8.f;
    }
    group->SetLabelSpacing(spacing);
    group->SetClickLabelToggles(clickToggles);
    AttachColorSwatch(*group, props);

    return ctrl;
}
#pragma endregion

#pragma region control api
static bool NeedsLabel(const UIProps& props)
{
    if (props.labelPosition == ElementPosition::None) return false;
    return !props.labelText.empty() || !props.text.empty();
}

#pragma region containers
Window& ChameleonUI::AddWindow(const std::wstring& title, int x, int y, int w, int h,
    const UIProps& props, std::function<void()> content)
{
    auto& win = m_context->CreateUIWindow(title, x, y, w, h);
    win.SetPadding(props.paddingLeft, props.paddingTop, props.paddingRight, props.paddingBottom);
    win.SetMargin(props.marginLeft, props.marginTop, props.marginRight, props.marginBottom);
    if (props.skin) win.SetSkin(props.skin);

    if (props.noTitleBar)
        win.SetShowTitleBar(false);

    Panel* cp = win.GetContentPanel();
    auto& vp = cp->AddChild<VerticalPanel>();
    vp.SetPadding(0, 0, 0, 0);
    vp.SetMargin(0, 0, 0, 0);
    if (props.skin) vp.SetSkin(props.skin);

    PushParent(&vp);
    content();
    PopParent();
    return win;
}

Window& ChameleonUI::AddWindow(const std::wstring& title, int x, int y, int w, int h,
    const UIProps& props, std::function<void()> titleBarContent,
    std::function<void()> content)
{
    auto& win = m_context->CreateUIWindow(title, x, y, w, h);
    win.SetPadding(props.paddingLeft, props.paddingTop, props.paddingRight, props.paddingBottom);
    win.SetMargin(props.marginLeft, props.marginTop, props.marginRight, props.marginBottom);
    if (props.skin) win.SetSkin(props.skin);

    if (props.noTitleBar)
        win.SetShowTitleBar(false);

    TitleBar* tb = win.GetTitleBar();
    if (tb && props.titleBarHeight > 0 && !props.noTitleBar)
        tb->SetHeight(props.titleBarHeight);

    if (tb && !props.noTitleBar && titleBarContent)
    {
        PushParent(tb);
        titleBarContent();
        PopParent();
    }

    Panel* cp = win.GetContentPanel();
    auto& vp = cp->AddChild<VerticalPanel>();
    vp.SetPadding(0, 0, 0, 0);
    vp.SetMargin(0, 0, 0, 0);
    if (props.skin) vp.SetSkin(props.skin);

    PushParent(&vp);
    content();
    PopParent();
    return win;
}

Image& ChameleonUI::AddImage(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    auto& img = parent->AddChild<Image>();
    ApplyProps(img, props);

    if (props.iconRef.IsValid())
    {
        img.SetImageHandle(props.iconRef.imageHandle);
        if (props.iconRef.IsAtlasRegion())
            img.SetSourceRect(props.iconRef.srcRect);
        float nw = (props.imageNaturalWidth > 0)  ? props.imageNaturalWidth  : props.iconRef.naturalWidth;
        float nh = (props.imageNaturalHeight > 0) ? props.imageNaturalHeight : props.iconRef.naturalHeight;
        img.SetNaturalSize(nw, nh);
    }
    else
    {
        if (props.imageHandle)
            img.SetImageHandle(props.imageHandle);
        if (props.imageNaturalWidth > 0 && props.imageNaturalHeight > 0)
            img.SetNaturalSize(props.imageNaturalWidth, props.imageNaturalHeight);
    }

    if (props.imageOpacity < 1.f)
        img.SetOpacity(props.imageOpacity);

    return img;
}


NumericInput& ChameleonUI::AddNumericInput(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    float initVal = props.initialValue;
    if (props.bindFloat && *props.bindFloat != 0.f)
        initVal = *props.bindFloat;

    if (props.labelPosition != ElementPosition::None && !props.labelText.empty())
    {
        auto& group = parent->AddChild<ControlGroup>();
        ApplyProps(group, props);
        auto& ni = group.AddChild<NumericInput>(props.rangeMin, props.rangeMax, initVal);
        ni.SetStep(props.step);
        ni.SetPrecision(props.precision);
        if (props.integerMode) ni.SetIntegerMode(true);
        if (props.bindFloat) ni.SetLinkedFloat(props.bindFloat);
        return WrapWithLabel(props, ni, false);
    }

    auto& ni = parent->AddChild<NumericInput>(props.rangeMin, props.rangeMax, initVal);
    ApplyProps(ni, props);
    ni.SetStep(props.step);
    ni.SetPrecision(props.precision);
    if (props.integerMode) ni.SetIntegerMode(true);
    if (props.bindFloat) ni.SetLinkedFloat(props.bindFloat);
    return ni;
}

Separator& ChameleonUI::AddSeparator(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    auto& sep = parent->AddChild<Separator>(
        props.separatorVertical ? Separator::Orientation::Vertical
        : Separator::Orientation::Horizontal);
    ApplyProps(sep, props);

    return sep;
}

GroupBox& ChameleonUI::AddGroupBox(const UIProps& props, std::function<void()> content)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& gb = parent->AddChild<GroupBox>(props.title, 0, 0, 0, 0);
    ApplyProps(gb, props);

    if (props.noTitleBar)
        gb.SetShowTitleBar(false);

    Panel* cp = gb.GetContentPanel();
    auto& vp = cp->AddChild<VerticalPanel>();
    vp.SetPadding(0, 0, 0, 0);
    vp.SetMargin(0, 0, 0, 0);
    if (props.skin) vp.SetSkin(props.skin);

    PushParent(&vp);
    content();
    PopParent();
    return gb;
}

TabControl& ChameleonUI::AddTabControl(TabOrientation orientation, const UIProps& props,
    std::function<void()> content)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& tc = parent->AddChild<TabControl>(orientation);
    ApplyProps(tc, props);
    if (props.heightPolicy == SizePolicy::ContentFit)
        tc.SetHeightPolicy(SizePolicy::Stretch);

    tc.SetAlignment(props.tabAlignment);
    if (props.stripThickness > 0)
        tc.SetStripThickness(props.stripThickness);

    tc.SetPageTransition(props.tabTransition);
    tc.SetScrollbarPosition(props.tabScrollbarPosition);
    tc.SetDetached(props.tabDetached);
    tc.SetDetached(props.tabDetached);
    tc.SetStripFit(props.tabStripFit);

    PushParent(&tc);
    content();
    PopParent();
    return tc;
}

TabPageHost& ChameleonUI::AddTabPageHost(TabControl& tabControl, const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& host = parent->AddChild<TabPageHost>();
    ApplyProps(host, props);
    host.SetTabControl(&tabControl);
    host.SetHeightPolicy(SizePolicy::Stretch);
    host.SetWidthPolicy(SizePolicy::Stretch);
    return host;
}

TabPage& ChameleonUI::AddTabPage(const std::wstring& title, const UIProps& props,
    std::function<void()> content)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto* tc = dynamic_cast<TabControl*>(parent);
    assert(tc);

    auto& page = parent->AddChild<TabPage>(title);
    page.SetPadding(props.paddingLeft, props.paddingTop, props.paddingRight, props.paddingBottom);
    page.SetMargin(props.marginLeft, props.marginTop, props.marginRight, props.marginBottom);
    if (props.skin) page.SetSkin(props.skin);

    if (props.iconRef.IsValid())
    {
        page.SetIconRef(props.iconRef);
        page.SetIconPosition(props.iconPosition);
        page.SetIconSpacing(props.iconSpacing);
    }
    page.SetShowText(props.showText);

    if (props.tabButtonSize > 0)
        page.SetTabButtonHeight(props.tabButtonSize);

    Panel* cp = page.GetContentPanel();
    auto& vp = cp->AddChild<VerticalPanel>();
    vp.SetPadding(0, 0, 0, 0);
    vp.SetMargin(0, 0, 0, 0);
    if (props.skin) vp.SetSkin(props.skin);

    PushParent(&vp);
    content();
    PopParent();

    tc->AddPage(&page);
    return page;
}

TabPage& ChameleonUI::AddTabPage(const std::wstring& title, std::function<void()> content)
{
    return AddTabPage(title, UIProps{}, content);
}

GridLayout& ChameleonUI::AddGrid(const UIProps& props, std::function<void(GridLayout&)> content)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& grid = parent->AddChild<GridLayout>();
    ApplyProps(grid, props);
    if (props.heightPolicy == SizePolicy::ContentFit)
        grid.SetHeightPolicy(SizePolicy::Stretch);

    PushParent(&grid);
    content(grid);
    PopParent();
    return grid;
}

GridLayout& ChameleonUI::AddGrid(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& grid = parent->AddChild<GridLayout>();
    ApplyProps(grid, props);
    if (props.heightPolicy == SizePolicy::ContentFit)
        grid.SetHeightPolicy(SizePolicy::Stretch);
    return grid;
}

Panel& ChameleonUI::AddPanel(const UIProps& props, ScrollMode scrollMode)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& panel = parent->AddChild<Panel>();
    panel.SetScrollMode(scrollMode);
    ApplyProps(panel, props);
    return panel;
}

void ChameleonUI::AddCell(GridLayout& grid, int row, int col,
    std::function<void()> content, int colSpan, int rowSpan)
{
    auto& wrapper = grid.AddChild<VerticalPanel>();
    wrapper.SetPadding(0, 0, 0, 0);
    wrapper.SetMargin(0, 0, 0, 0);
    wrapper.SetAutoSpacing(8);
    wrapper.SetWidthPolicy(SizePolicy::Stretch);
    wrapper.SetHeightPolicy(SizePolicy::Stretch);
    wrapper.SetHitTestTransparent(true);

    PushParent(&wrapper);
    content();
    PopParent();

    grid.PlaceChild(wrapper, row, col, colSpan, rowSpan);
}
#pragma endregion

#pragma region leaf controls
Checkbox& ChameleonUI::AddCheckbox(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    std::wstring lblText = props.text.empty() ? props.labelText : props.text;
    ElementPosition labelPos = props.labelPosition;
    if (labelPos == ElementPosition::None && !lblText.empty())
        labelPos = ElementPosition::Left;

    UIProps resolved = props;
    resolved.labelText = lblText;
    resolved.labelPosition = labelPos;

    bool initVal = props.bindBool ? *props.bindBool : false;

    if (labelPos != ElementPosition::None && !lblText.empty())
    {
        auto& group = parent->AddChild<ControlGroup>();
        ApplyProps(group, resolved);
        auto& chk = group.AddChild<Checkbox>(L"", initVal);
        ApplyBinding(chk, resolved);
        return WrapWithLabel(resolved, chk, resolved.clickLabelToggles);
    }

    auto& chk = parent->AddChild<Checkbox>(lblText, initVal);
    ApplyProps(chk, resolved);
    ApplyBinding(chk, resolved);
    return chk;
}

Button& ChameleonUI::AddButton(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& btn = parent->AddChild<Button>(props.text);
    ApplyProps(btn, props);
    return btn;
}

Slider& ChameleonUI::AddSlider(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    float value = props.bindFloat ? *props.bindFloat : props.initialValue;

    if (NeedsLabel(props))
    {
        auto& group = parent->AddChild<ControlGroup>();
        ApplyProps(group, props);
        auto& slider = group.AddChild<Slider>(props.rangeMin, props.rangeMax, value);
        ApplyBinding(slider, props);
        return WrapWithLabel(props, slider, false);
    }

    auto& slider = parent->AddChild<Slider>(props.rangeMin, props.rangeMax, value);
    ApplyBinding(slider, props);
    ApplyProps(slider, props);
    return slider;
}

ComboBox& ChameleonUI::AddComboBox(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    if (NeedsLabel(props))
    {
        auto& group = parent->AddChild<ControlGroup>();
        ApplyProps(group, props);
        auto& combo = group.AddChild<ComboBox>(props.items, props.comboMode, props.maxVisibleItems);
        ApplyBinding(combo, props);
        return WrapWithLabel(props, combo, false);
    }

    auto& combo = parent->AddChild<ComboBox>(props.items, props.comboMode, props.maxVisibleItems);
    ApplyBinding(combo, props);
    ApplyProps(combo, props);
    return combo;
}

TextInput& ChameleonUI::AddTextInput(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    if (NeedsLabel(props))
    {
        auto& group = parent->AddChild<ControlGroup>();
        ApplyProps(group, props);
        auto& input = group.AddChild<TextInput>(props.multiline, props.placeholder);
        ApplyBinding(input, props);
        return WrapWithLabel(props, input, false);
    }

    auto& input = parent->AddChild<TextInput>(props.multiline, props.placeholder);
    ApplyBinding(input, props);
    ApplyProps(input, props);
    return input;
}

KeyBind& ChameleonUI::AddKeyBind(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    if (NeedsLabel(props))
    {
        auto& group = parent->AddChild<ControlGroup>();
        ApplyProps(group, props);
        auto& kb = group.AddChild<KeyBind>(props.initialKey);
        ApplyBinding(kb, props);
        return WrapWithLabel(props, kb, false);
    }

    auto& kb = parent->AddChild<KeyBind>(props.initialKey);
    ApplyBinding(kb, props);
    ApplyProps(kb, props);
    return kb;
}

ListBox& ChameleonUI::AddListBox(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& lb = parent->AddChild<ListBox>(props.items, props.listMode);
    ApplyProps(lb, props);
    if (props.maxVisibleItems > 0)
        lb.SetMaxVisibleItems(props.maxVisibleItems);
    return lb;
}

Label& ChameleonUI::AddLabel(const std::wstring& text, const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& lbl = parent->AddChild<Label>(text);
    lbl.SetRenderer(m_renderer);
    ApplyProps(lbl, props);
    return lbl;
}

ColorSwatch& ChameleonUI::AddColorSwatch(Color* linkedColor, const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    EnsureColorPickerModal();

    auto createSwatch = [&](UIElement& container) -> ColorSwatch& {
        auto& swatch = container.AddChild<ColorSwatch>();
        swatch.SetWidthPolicy(SizePolicy::ContentFit);
        swatch.SetHeightPolicy(SizePolicy::ContentFit);

        if (linkedColor)
        {
            swatch.SetLinkedColor(linkedColor);
            swatch.SetColor(*linkedColor);
        }

        Skin* skin = swatch.GetEffectiveSkin();
        if (props.swatchWidth > 0 && props.swatchHeight > 0)
            swatch.SetSwatchSize(props.swatchWidth, props.swatchHeight);
        else if (skin && skin->GetStyle())
            swatch.SetSwatchSize(skin->GetStyle()->colorSwatchDefaultWidth,
                skin->GetStyle()->colorSwatchDefaultHeight);

        swatch.SetModal(m_colorPickerModal);
        return swatch;
        };

    if (NeedsLabel(props))
    {
        auto& group = parent->AddChild<ControlGroup>();
        ApplyProps(group, props);
        auto& swatch = createSwatch(group);
        return WrapWithLabel(props, swatch, false);
    }

    auto& swatch = createSwatch(*parent);
    ApplyProps(swatch, props);
    return swatch;
}

ColorPicker& ChameleonUI::AddColorPicker(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    float w = props.fixedWidth > 0 ? (float)props.fixedWidth : 200.f;
    float h = props.fixedHeight > 0 ? (float)props.fixedHeight : 200.f;

    auto& picker = parent->AddChild<ColorPicker>(w, h);
    ApplyProps(picker, props);

    picker.SetHueBarPosition(props.hueBarPosition);
    picker.SetShowAlphaBar(props.showAlphaBar);
    if (props.bindColor)
        picker.SetLinkedColor(props.bindColor);

    return picker;
}

ContentSwitcher& ChameleonUI::AddContentSwitcher(const UIProps& props)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);
    auto& switcher = parent->AddChild<ContentSwitcher>();
    ApplyProps(switcher, props);
    return switcher;
}
#pragma endregion

#pragma region config selector
void ChameleonUI::RefreshConfigSelector()
{
    if (!m_configSelectorControl) return;

    auto configs = ListConfigs();
    std::vector<std::wstring> items;
    for (auto& name : configs)
        items.push_back(name);

    if (auto* lb = dynamic_cast<ListBox*>(m_configSelectorControl))
    {
        lb->SetItems(items);
        const auto& active = m_config.GetActiveConfigName();
        for (int i = 0; i < (int)items.size(); ++i)
            if (items[i] == active) { lb->SetSelectedIndex(i); break; }
    }
    else if (auto* cb = dynamic_cast<ComboBox*>(m_configSelectorControl))
    {
        cb->SetItems(items);
        const auto& active = m_config.GetActiveConfigName();
        for (int i = 0; i < (int)items.size(); ++i)
            if (items[i] == active) { cb->SetSelectedIndex(i); break; }
    }
}

std::wstring ChameleonUI::GetConfigSelectorSelectedName() const
{
    if (!m_configSelectorControl) return L"default";

    int idx = -1;
    const std::vector<std::wstring>* items = nullptr;

    if (auto* lb = dynamic_cast<ListBox*>(m_configSelectorControl))
    {
        idx = lb->GetSelectedIndex();
        items = &lb->GetItems();
    }
    else if (auto* cb = dynamic_cast<ComboBox*>(m_configSelectorControl))
    {
        idx = cb->GetSelectedIndex();
        items = &cb->GetItems();
    }

    if (items && idx >= 0 && idx < (int)items->size())
        return (*items)[idx];
    return L"default";
}

void ChameleonUI::AddConfigSelector(const UIProps& props, ConfigSelectorMode mode)
{
    UIElement* parent = GetCurrentParent();
    assert(parent);

    if (mode == ConfigSelectorMode::ListBox)
    {
        auto& lb = parent->AddChild<ListBox>(std::vector<std::wstring>{}, ListBoxMode::SingleSelect);
        ApplyProps(lb, props);
        if (props.maxVisibleItems > 0)
            lb.SetMaxVisibleItems(props.maxVisibleItems);
        else
            lb.SetMaxVisibleItems(5);
        m_configSelectorControl = &lb;
    }
    else
    {
        auto& cb = parent->AddChild<ComboBox>(std::vector<std::wstring>{}, ComboBoxMode::SingleSelect,
            props.maxVisibleItems > 0 ? props.maxVisibleItems : 5);
        ApplyProps(cb, props);
        m_configSelectorControl = &cb;
    }

    {
        auto& input = parent->AddChild<TextInput>(false);
        input.SetPlaceholder(Tr(L"config.name_hint"));
        input.SetLinkedString(&m_internalConfigName);
        input.SetWidthPolicy(SizePolicy::Stretch);
        input.SetHeightPolicy(SizePolicy::ContentFit);
        input.SetMargin(0, 4, 0, 4);
    }

    auto makeButton = [&](const std::wstring& key, std::function<void()> onClick) -> Button& {
        auto& btn = parent->AddChild<Button>(Tr(key));
        btn.SetWidthPolicy(SizePolicy::Stretch);
        btn.SetHeightPolicy(SizePolicy::ContentFit);
        btn.SetMargin(0, 2, 0, 2);
        btn.SetOnClick(std::move(onClick));
        return btn;
        };

    makeButton(L"config.btn_new", [this]() {
        std::wstring name = m_internalConfigName.empty() ? L"new_config" : m_internalConfigName;
        SaveConfig(name);
        RefreshConfigSelector();
        });

    makeButton(L"config.btn_save", [this]() {
        SaveConfig(GetConfigSelectorSelectedName());
        });

    makeButton(L"config.btn_load", [this]() {
        LoadConfig(GetConfigSelectorSelectedName());
        });

    makeButton(L"config.btn_delete", [this]() {
        DeleteConfig(GetConfigSelectorSelectedName());
        RefreshConfigSelector();
        });

    makeButton(L"config.btn_reset", [this]() {
        ResetConfig();
        });

    RefreshConfigSelector();
}
#pragma endregion
#pragma endregion

#pragma region style & skin
UIStyle& ChameleonUI::GetStyle() { return *m_skin->GetStyle(); }
Skin* ChameleonUI::GetSkin() const { return m_skin.get(); }

void ChameleonUI::SetSkin(std::unique_ptr<Skin> skin)
{
    m_skin = std::move(skin);
    UIElement::SetDefaultSkin(m_skin.get());
}

void ChameleonUI::SetLanguage(const std::wstring& langCode)
{
    m_pendingLanguage = langCode;
}

void ChameleonUI::RebuildUI()
{
    if (!m_context || !m_renderer) return;

    std::vector<WindowState> savedWindows;
    std::vector<std::vector<TabState>> savedTabs;

    for (auto& win : m_context->GetWindows())
    {
        savedWindows.push_back({ win->GetX(), win->GetY(), win->GetWidth(), win->GetHeight() });
        std::vector<TabState> winTabs;
        SaveTabStates(win.get(), winTabs);
        savedTabs.push_back(std::move(winTabs));
    }

    m_context->Shutdown();
    while (!m_parentStack.empty())
        m_parentStack.pop();

    m_context->Initialize(m_hwnd);

    LoadLanguages();

    OnBeforeBuildUI();
    BuildUI();

    auto& windows = m_context->GetWindows();
    for (size_t i = 0; i < savedWindows.size() && i < windows.size(); ++i)
    {
        windows[i]->SetX(savedWindows[i].x);
        windows[i]->SetY(savedWindows[i].y);
        windows[i]->SetWidth(savedWindows[i].w);
        windows[i]->SetHeight(savedWindows[i].h);
    }

    for (size_t i = 0; i < savedTabs.size() && i < windows.size(); ++i)
    {
        size_t idx = 0;
        RestoreTabStates(windows[i].get(), savedTabs[i], idx);
    }

    for (auto& win : windows)
        win->SyncAllFromBindings();

    m_context->Update(0.0f);
}
#pragma endregion

#pragma region tab state
void ChameleonUI::SaveTabStates(UIElement* root, std::vector<TabState>& out)
{
    if (!root) return;
    if (auto* tc = dynamic_cast<TabControl*>(root))
    {
        out.push_back({ tc->GetActiveIndex(), 0.f });
    }
    for (auto& child : root->GetChildren())
        SaveTabStates(child.get(), out);
    if (auto* tc = dynamic_cast<TabControl*>(root))
    {
        for (auto* page : tc->GetPages())
            SaveTabStates(page, out);
    }
}

void ChameleonUI::RestoreTabStates(UIElement* root, const std::vector<TabState>& states, size_t& idx)
{
    if (!root) return;
    if (auto* tc = dynamic_cast<TabControl*>(root))
    {
        if (idx < states.size())
        {
            int savedIdx = states[idx].activeIndex;
            if (savedIdx >= 0 && savedIdx < (int)tc->GetPages().size())
                tc->SetActiveIndexSilent(savedIdx);
            ++idx;
        }
    }
    for (auto& child : root->GetChildren())
        RestoreTabStates(child.get(), states, idx);
    if (auto* tc = dynamic_cast<TabControl*>(root))
    {
        for (auto* page : tc->GetPages())
            RestoreTabStates(page, states, idx);
    }
}
#pragma endregion