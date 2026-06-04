#pragma once
#include "../rendering/renderer.h"
#include "../core/UIProps.h"
#include "../layouts/gridLayout.h"
#include "../layouts/panel.h"
#include "../elements/window.h"
#include "../elements/button.h"
#include "../elements/checkbox.h"
#include "../elements/label.h"
#include "../elements/groupbox.h"
#include "../elements/tabcontrol.h"
#include "../elements/tabpage.h"
#include "../elements/combobox.h"
#include "../elements/slider.h"
#include "../elements/textinput.h"
#include "../elements/keybind.h"
#include "../elements/listbox.h"
#include "../elements/ContentSwitcher.h"
#include "../elements/colorpickermodal.h"
#include "../elements/ControlGroup.h"
#include "../elements/TabPageHost.h"
#include "../elements/image.h"
#include "../elements/separator.h"
#include "../elements/numericinput.h"
#include "../elements/colorpicker.h"
#include "../util/localization.h"
#include "../util/iconlibrary.h"
#include "../util/configstore.h"
#include <string>
#include <functional>
#include <memory>
#include <stack>

class UIContext;
class DefaultSkin;
class UIStyle;

struct ID3D11Device;
struct IDXGISwapChain1;
struct ID2D1Bitmap1;

class ChameleonUI
{
public:
    ChameleonUI();
    virtual ~ChameleonUI();

    #pragma region lifecycle
    bool Initialize(HWND hwnd, ID3D11Device* d3dDevice, IDXGISwapChain1* swapChain);
    void Initialize(HWND hwnd, IRenderer* renderer);
    void Shutdown();
    void NewFrame();
    void Render();

    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    #pragma endregion

    #pragma region desktop capture
    void SetDesktopCaptureEnabled(bool enabled);
    bool IsDesktopCaptureEnabled() const { return m_desktopCaptureEnabled; }
    void SetClearColor(float r, float g, float b, float a);

    void CaptureBackdrop();
    #pragma endregion

    #pragma region skin & style
    virtual std::unique_ptr<Skin> CreateSkin();
    void SetSkin(std::unique_ptr<Skin> skin);
    Skin* GetSkin() const;
    UIStyle& GetStyle();

    template<typename T = DefaultSkin, typename... Args>
    T* AddSkin(Args&&... args)
    {
        auto skin = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = skin.get();
        m_additionalSkins.push_back(std::move(skin));
        return ptr;
    }
    #pragma endregion

    #pragma region widget creation - containers
    Window&         AddWindow(const std::wstring& title, int x, int y, int w, int h, const UIProps& props, std::function<void()> content);
    Window&         AddWindow(const std::wstring& title, int x, int y, int w, int h, const UIProps& props, std::function<void()> titleBarContent, std::function<void()> content);
    GroupBox&       AddGroupBox(const UIProps& props, std::function<void()> content);
    TabControl&     AddTabControl(TabOrientation orientation, const UIProps& props, std::function<void()> content);
    TabPage&        AddTabPage(const std::wstring& title, const UIProps& props, std::function<void()> content);
    TabPage&        AddTabPage(const std::wstring& title, std::function<void()> content);
    TabPageHost&    AddTabPageHost(TabControl& tabControl, const UIProps& props = {});
    GridLayout&     AddGrid(const UIProps& props, std::function<void(GridLayout&)> content);
    GridLayout&     AddGrid(const UIProps& props = {});
    Panel&          AddPanel(const UIProps& props = {}, ScrollMode scrollMode = ScrollMode::None);
    void            AddCell(GridLayout& grid, int row, int col, std::function<void()> content, int colSpan = 1, int rowSpan = 1);
    #pragma endregion

    #pragma region widget creation - controls
    Button&             AddButton(const UIProps& props = {});
    Checkbox&           AddCheckbox(const UIProps& props = {});
    Slider&             AddSlider(const UIProps& props = {});
    ComboBox&           AddComboBox(const UIProps& props = {});
    TextInput&          AddTextInput(const UIProps& props = {});
    NumericInput&       AddNumericInput(const UIProps& props = {});
    KeyBind&            AddKeyBind(const UIProps& props = {});
    ListBox&            AddListBox(const UIProps& props = {});
    Label&              AddLabel(const std::wstring& text, const UIProps& props = {});
    Image&              AddImage(const UIProps& props = {});
    Separator&          AddSeparator(const UIProps& props = {});
    ColorSwatch&        AddColorSwatch(Color* linkedColor, const UIProps& props = {});
    ColorPicker&        AddColorPicker(const UIProps& props = {});
    ContentSwitcher&    AddContentSwitcher(const UIProps& props = {});

    enum class ConfigSelectorMode { ListBox, ComboBox };
    void AddConfigSelector(const UIProps& props = {}, ConfigSelectorMode mode = ConfigSelectorMode::ListBox);
    #pragma endregion

    #pragma region icons
    void RegisterIcons(const unsigned char* atlasData, size_t atlasDataSize,
        const IconData* icons, int iconCount) {
        m_iconLib.RegisterIcons(m_renderer, atlasData, atlasDataSize, icons, (size_t)iconCount);
    }
    void RegisterIcons(int atlasResourceID, const IconData* icons, int iconCount) {
        m_iconLib.RegisterIcons(m_renderer, atlasResourceID, icons, (size_t)iconCount);
    }
    void RegisterIcons(const wchar_t* atlasFilePath, const IconData* icons, int iconCount) {
        m_iconLib.RegisterIcons(m_renderer, atlasFilePath, icons, (size_t)iconCount);
    }
    IconRef RetrieveIcon(const std::wstring& name, float w = 24.f, float h = 24.f) {
        return m_iconLib.RetrieveIcon(name, w, h);
    }
    const IconLibrary& GetIconLibrary() const { return m_iconLib; }
    #pragma endregion

    #pragma region localization
    void RegisterLanguage(const std::wstring& langCode, const LocalizedString* strings, size_t count) {
        m_localization.RegisterLanguage(langCode, strings, count);
    }
    void SetLanguage(const std::wstring& langCode);
    const std::wstring& Tr(const std::wstring& key) const { return m_localization.Tr(key); }
    Localization& GetLocalization() { return m_localization; }
    const Localization& GetLocalization() const { return m_localization; }
    #pragma endregion

    #pragma region configuration
    ConfigStore& GetConfig() { return m_config; }
    const ConfigStore& GetConfig() const { return m_config; }
    bool SaveConfig(const std::wstring& name) { return m_config.SaveConfig(name); }
    bool LoadConfig(const std::wstring& name);
    bool DeleteConfig(const std::wstring& name) { return m_config.DeleteConfig(name); }
    std::vector<std::wstring> ListConfigs() const { return m_config.ListConfigs(); }
    void ResetConfig();
    #pragma endregion

    #pragma region resources
    IRenderer* GetRenderer() const { return m_renderer; }
    void* LoadManagedImage(const wchar_t* filePath);
    void* LoadManagedImageFromResource(int resourceId);
    #pragma endregion

protected:
    #pragma region extension points
    virtual void BuildUI();
    virtual void LoadLanguages();
    virtual void RegisterBindings() {}
    virtual void OnBeforeBuildUI() { m_configSelectorControl = nullptr; }
    virtual std::wstring GetConfigAppName() const { return L"UIFramework"; }
    virtual std::wstring GetDefaultConfigName() const { return L"default"; }

    void RebuildUI();
    #pragma endregion

private:
    #pragma region managed graphics
    struct ManagedGraphics;
    std::unique_ptr<ManagedGraphics> m_gfx;
    void InitializeCommon();
    #pragma endregion

    #pragma region parent stack & tree building
    Window* GetCurrentWindow();
    UIElement* GetCurrentParent();
    void PushParent(UIElement* parent);
    void PopParent();

    void ApplyProps(UIElement& el, const UIProps& props);
    UIElement* BuildContentContainer(UIElement* parent, const UIProps& props);

    template<typename TControl>
    TControl& WrapWithLabel(const UIProps& props, TControl& ctrl, bool clickToggles = false);

    void ApplyBinding(Checkbox& ctrl, const UIProps& props);
    void ApplyBinding(Slider& ctrl, const UIProps& props);
    void ApplyBinding(ComboBox& ctrl, const UIProps& props);
    void ApplyBinding(TextInput& ctrl, const UIProps& props);
    void ApplyBinding(KeyBind& ctrl, const UIProps& props);
    #pragma endregion

    #pragma region color picker modal
    void AttachColorSwatch(ControlGroup& group, const UIProps& props);
    void EnsureColorPickerModal();
    #pragma endregion

    #pragma region config selector
    void RefreshConfigSelector();
    std::wstring GetConfigSelectorSelectedName() const;
    #pragma endregion

    #pragma region ui state preservation
    struct TabState { int activeIndex = 0; float scrollOffset = 0.f; };
    struct WindowState { int x, y, w, h; };
    void SaveTabStates(UIElement* root, std::vector<TabState>& out);
    void RestoreTabStates(UIElement* root, const std::vector<TabState>& states, size_t& idx);
    #pragma endregion

    #pragma region member data
    std::unique_ptr<UIContext>                        m_context;
    std::unique_ptr<Skin>                             m_skin;
    std::vector<std::unique_ptr<Skin>>                m_additionalSkins;
    IconLibrary                                       m_iconLib;
    Localization                                      m_localization;
    ConfigStore                                       m_config;

    std::unique_ptr<ColorPickerModal>                 m_colorPickerModalOwned;
    ColorPickerModal*                                 m_colorPickerModal = nullptr;
    UIElement*                                        m_configSelectorControl = nullptr;
    std::wstring*                                     m_configNameInput = nullptr;

    HWND                                              m_hwnd = nullptr;
    IRenderer*                                        m_renderer = nullptr;
    std::stack<UIElement*>                            m_parentStack;
    std::vector<void*>                                m_managedImages;

    std::wstring                                      m_pendingLanguage;
    std::wstring                                      m_internalConfigName;

    bool                                              m_desktopCaptureEnabled = true;
    ID2D1Bitmap1*                                     m_cachedBackdrop = nullptr;
    float                                             m_clearColor[4] = { 0.f, 0.f, 0.f, 0.f };
    #pragma endregion
};
