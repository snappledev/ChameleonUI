#pragma once
#include "../layouts/elementposition.h"
#include "../elements/controlgroup.h"
#include "../util/iconposition.h"
#include "../util/iconref.h"
#include "../util/huebarposition.h"
#include <vector>
#include <string>

class Skin;
enum class SizePolicy;
enum class HorizontalAlignment;
enum class TabAlignment;
enum class TabOrientation;
enum class TabTransition;
enum class TabScrollbarPosition;
enum class ComboBoxMode;
enum class ListBoxMode;
enum class ScrollMode;

enum class AutoSize { Auto, Fill, Fixed };

struct UIProps
{
    #pragma region data binding
    bool*           bindBool   = nullptr;
    int*            bindInt    = nullptr;
    float*          bindFloat  = nullptr;
    std::wstring*   bindString = nullptr;

    UIProps& Bind(bool* val)          { bindBool   = val;  return *this; }
    UIProps& Bind(int* val)           { bindInt    = val;  return *this; }
    UIProps& Bind(float* val)         { bindFloat  = val;  return *this; }
    UIProps& Bind(std::wstring* val)  { bindString = val;  return *this; }
    UIProps& Bind(Color* val)         { bindColor  = val;  return *this; }
    #pragma endregion

    #pragma region layout
    int                 paddingLeft     = 0, paddingTop    = 0, paddingRight  = 0, paddingBottom = 0;
    int                 marginLeft      = 0, marginTop     = 0, marginRight   = 0, marginBottom  = 0;
    int                 fixedWidth      = 0;
    int                 fixedHeight     = 0;

    SizePolicy          widthPolicy     = (SizePolicy)0;
    SizePolicy          heightPolicy    = SizePolicy::ContentFit;
    HorizontalAlignment hAlign          = (HorizontalAlignment)0;

    bool                scrollable      = false;
    Skin*               skin            = nullptr;
    #pragma endregion

    #pragma region label composition
    std::wstring    labelText;
    ElementPosition labelPosition       = ElementPosition::None;
    float           labelSpacing        = 0.f;
    float           controlIndent       = 0.f;
    bool            clickLabelToggles   = true;
    #pragma endregion

    #pragma region tab-specific
    TabAlignment         tabAlignment          = (TabAlignment)0;
    int                  stripThickness        = 0;
    int                  tabButtonSize         = 0;
    TabTransition        tabTransition         = (TabTransition)0;
    TabScrollbarPosition tabScrollbarPosition = (TabScrollbarPosition)0;
    bool                 tabDetached           = false;
    #pragma endregion

    #pragma region icon
    IconRef      iconRef;
    IconPosition iconPosition = IconPosition::Left;
    float        iconSpacing  = 6.f;
    bool         showText     = true;
    #pragma endregion

    #pragma region color swatch
    Color*               bindColor       = nullptr;
    ColorSwatchAlignment swatchAlignment = ColorSwatchAlignment::None;
    float                swatchSpacing   = 8.f;
    float                swatchWidth     = 0.f;
    float                swatchHeight    = 0.f;
    #pragma endregion

    #pragma region domain params
    std::wstring              title;
    std::wstring              text;
    std::wstring              placeholder;
    std::vector<std::wstring> items;
    float                     rangeMin    = 0.f, rangeMax = 100.f;
    float                     initialValue = 0.f;
    int                       initialKey   = 0;
    bool                      multiline     = false;
    ComboBoxMode              comboMode     = (ComboBoxMode)0;
    ListBoxMode               listMode      = (ListBoxMode)0;
    int                       maxVisibleItems = 6;
    #pragma endregion

    #pragma region image
    void* imageHandle          = nullptr;
    float imageOpacity         = 1.f;
    float imageNaturalWidth    = 0.f;
    float imageNaturalHeight   = 0.f;
    #pragma endregion

    #pragma region separator
    bool separatorVertical = false;
    #pragma endregion

    #pragma region tooltip
    std::wstring tooltip;
    #pragma endregion

    #pragma region numeric input
    float step        = 1.f;
    int   precision   = 2;
    bool  integerMode = false;
    #pragma endregion

    #pragma region misc
    bool           noTitleBar     = false;
    HueBarPosition hueBarPosition = HueBarPosition::Right;
    bool           showAlphaBar   = false;

    int  titleBarHeight = 0;
    bool tabStripFit    = false;
    #pragma endregion

    #pragma region chainable setters
    UIProps& Padding(int all) {
        paddingLeft = paddingTop = paddingRight = paddingBottom = all; return *this;
    }
    UIProps& Padding(int lr, int tb) {
        paddingLeft = paddingRight = lr; paddingTop = paddingBottom = tb; return *this;
    }
    UIProps& Padding(int l, int t, int r, int b) {
        paddingLeft = l; paddingTop = t; paddingRight = r; paddingBottom = b; return *this;
    }
    UIProps& Margin(int all) {
        marginLeft = marginTop = marginRight = marginBottom = all; return *this;
    }
    UIProps& Margin(int l, int t, int r, int b) {
        marginLeft = l; marginTop = t; marginRight = r; marginBottom = b; return *this;
    }

    UIProps& Width(AutoSize mode, int px = 0) {
        if (mode == AutoSize::Auto)       widthPolicy = SizePolicy::ContentFit;
        else if (mode == AutoSize::Fill)  widthPolicy = SizePolicy::Stretch;
        else { widthPolicy = SizePolicy::Fixed; fixedWidth = px; }
        return *this;
    }
    UIProps& Height(AutoSize mode, int px = 0) {
        if (mode == AutoSize::Auto)       heightPolicy = SizePolicy::ContentFit;
        else if (mode == AutoSize::Fill)  heightPolicy = SizePolicy::Stretch;
        else { heightPolicy = SizePolicy::Fixed; fixedHeight = px; }
        return *this;
    }
    UIProps& Align(HorizontalAlignment a) { hAlign = a; return *this; }

    UIProps& Label(const std::wstring& txt, ElementPosition pos = ElementPosition::Left, float spacing = 0.f) {
        labelText = txt; labelPosition = pos; labelSpacing = spacing; return *this;
    }
    UIProps& Label(ElementPosition pos, float spacing = 0.f) {
        labelPosition = pos; labelSpacing = spacing; return *this;
    }
    UIProps& Indent(float indent) { controlIndent = indent; return *this; }
    UIProps& ClickLabel(bool enable) { clickLabelToggles = enable; return *this; }

    UIProps& Title(const std::wstring& t) { title = t; return *this; }
    UIProps& Text(const std::wstring& t) { text = t; return *this; }
    UIProps& Placeholder(const std::wstring& t) { placeholder = t; return *this; }
    UIProps& Items(const std::vector<std::wstring>& i) { items = i; return *this; }
    UIProps& Items(std::initializer_list<std::wstring> i) { items = i; return *this; }
    UIProps& Range(float min, float max) { rangeMin = min; rangeMax = max; return *this; }
    UIProps& Value(float v) { initialValue = v; return *this; }
    UIProps& Key(int vk) { initialKey = vk; return *this; }
    UIProps& Multiline(bool m = true) { multiline = m; return *this; }
    UIProps& ComboMode(ComboBoxMode m) { comboMode = m; return *this; }
    UIProps& ListMode(ListBoxMode m) { listMode = m; return *this; }
    UIProps& MaxVisible(int n) { maxVisibleItems = n; return *this; }
    UIProps& HueBar(HueBarPosition pos) { hueBarPosition = pos; return *this; }
    UIProps& ShowAlpha(bool show = true) { showAlphaBar = show; return *this; }

    UIProps& Icon(void* img, IconPosition pos = IconPosition::Left) {
        iconRef = IconRef(img); iconPosition = pos; return *this;
    }
    UIProps& Icon(const IconRef& ref, IconPosition pos = IconPosition::Left) {
        iconRef = ref; iconPosition = pos; return *this;
    }
    UIProps& IconSize(float w, float h) { iconRef.naturalWidth = w; iconRef.naturalHeight = h; return *this; }
    UIProps& IconGap(float spacing) { iconSpacing = spacing; return *this; }
    UIProps& ShowText(bool show) { showText = show; return *this; }

    UIProps& TabAlign(TabAlignment a) { tabAlignment = a; return *this; }
    UIProps& StripSize(int size) { stripThickness = size; return *this; }
    UIProps& ButtonSize(int size) { tabButtonSize = size; return *this; }
    UIProps& Transition(TabTransition t) { tabTransition = t; return *this; }
    UIProps& TabScrollbar(TabScrollbarPosition pos) { tabScrollbarPosition = pos; return *this; }
    UIProps& Detached(bool d = true) { tabDetached = d; return *this; }
    UIProps& StripFit(bool fit = true) { tabStripFit = fit; return *this; }

    UIProps& ColorSwatch(Color* color, ColorSwatchAlignment align = ColorSwatchAlignment::Right) {
        bindColor = color; swatchAlignment = align; return *this;
    }
    UIProps& SwatchSpacing(float spacing) { swatchSpacing = spacing; return *this; }
    UIProps& SwatchSize(float w, float h) { swatchWidth = w; swatchHeight = h; return *this; }

    UIProps& Scrollable(bool enable = true) { scrollable = enable; return *this; }
    UIProps& NoTitleBar(bool hide = true) { noTitleBar = hide; return *this; }
    UIProps& UseSkin(Skin* s) { skin = s; return *this; }

    UIProps& ImageSource(void* handle) { imageHandle = handle; return *this; }
    UIProps& ImageOpacity(float o) { imageOpacity = o; return *this; }
    UIProps& ImageNaturalSize(float w, float h) { imageNaturalWidth = w; imageNaturalHeight = h; return *this; }

    UIProps& Tooltip(const std::wstring& tip) { tooltip = tip; return *this; }

    UIProps& Step(float s) { step = s; return *this; }
    UIProps& Precision(int digits) { precision = digits; return *this; }
    UIProps& IntegerMode(bool i = true) { integerMode = i; return *this; }

    UIProps& TitleBarHeight(int h) { titleBarHeight = h; return *this; }
    #pragma endregion
};

// short alias
inline UIProps Props() { return UIProps{}; }