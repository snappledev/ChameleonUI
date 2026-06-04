# ChameleonUI

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-17%2B-%2300599C)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/platform-Windows%2010%2B-0078D6)](https://www.microsoft.com/windows)

<p align="center"> <img width="300" height="255" alt="Chameleon" src="https://github.com/user-attachments/assets/fb93197d-8f0d-4450-9c2d-64b8c468feca" /> </p>
A high-performance, fully skinnable C++ UI framework built on Direct2D and DirectWrite. ChameleonUI runs as a **guest** inside any DirectX 11 host — give it a device and a swap chain, call three functions per frame, and you get a complete desktop-class UI with animation, localization, glassmorphism, and config persistence out of the box.






<p align="center">
  <iframe width="560" height="315" src="https://github.com/user-attachments/assets/e7543885-e208-43de-a86c-d4e9bfeb92c9" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

---

## Table of Contents

- [Key Features](#key-features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
  - [Minimal Example](#minimal-example)
- [Architecture](#architecture)
  - [DX11 Guest Model](#dx11-guest-model)
  - [Lifecycle](#lifecycle)
- [Skinning System](#skinning-system)
  - [Tier 1 — UIStyle (data-driven)](#tier-1--uistyle-data-driven)
  - [Tier 2 — Skin inheritance (structural)](#tier-2--skin-inheritance-structural)
  - [Per-widget overrides](#per-widget-overrides)
  - [Built-in skins](#built-in-skins)
- [Control Reference](#control-reference)
- [Props Builder Reference](#props-builder-reference)
- [Layout System](#layout-system)
- [Icon Atlas System](#icon-atlas-system)
- [Configuration & Localization](#configuration--localization)
- [Advanced Topics](#advanced-topics)
  - [Custom controls](#custom-controls)
  - [Animation engine](#animation-engine)
  - [Desktop capture & glassmorphism](#desktop-capture--glassmorphism)
- [License](#license)
- [Contributing](#contributing)

---

## Key Features

- **Zero-allocation immediate-style API** — declare your entire UI in one `BuildUI()` override using chainable `Props()` builders
- **Complete control suite** — 20+ controls: Window, Button, Checkbox, Slider, ComboBox, TextInput, KeyBind, ListBox, NumericInput, ColorPicker, ColorSwatch, TabControl, GroupBox, GridLayout, Panel, Separator, Image, ContentSwitcher, Label, ConfigSelector
- **Deep skinning system** — swap visual identities by returning a different `Skin` subclass; tweak ~120 colors/sizes via `UIStyle`; override individual `Draw*()` methods for custom rendering; per-widget `.UseSkin()` overrides
- **Built-in animation engine** — hover, press, focus, and toggle transitions on every control with configurable easing (Linear, QuadOut, CubicOut, ExpoOut, BackOut, ElasticOut, SmoothStep, OvershootOut)
- **Glassmorphism** — real-time desktop capture and Gaussian blur behind UI windows via DXGI Output Duplication
- **Icon atlas system** — Python tooling generates embedded C++ sprite atlases from SVG or PNG folders; named icon lookup
- **Localization** — compile-time language packs with live switching that rebuilds the UI tree while preserving state
- **Configuration persistence** — automatic save/load to `%APPDATA%` with named config profiles and a built-in profile selector widget
- **Self-contained** — single `Initialize()` call; creates D2D/DWrite internally from your D3D11 device

---

## Getting Started

### Prerequisites

| Requirement | Notes |
|---|---|
| Windows 10 2004+ (build 19041) | Required for `WDA_EXCLUDEFROMCAPTURE` (desktop capture) |
| Visual Studio 2022 | Or any MSVC-compatible C++17 compiler |
| Windows SDK 10.0.19041.0+ | Ships with Visual Studio |

### Building

1. Clone the repository
2. Open the solution in Visual Studio 2022
3. Build — all dependencies are system-provided (DirectX, DComp)

The framework links against:
- `d3d11.lib` — Direct3D 11
- `dxgi.lib` — DXGI (swap chains, desktop duplication)
- `d2d1.lib` — Direct2D rendering
- `dwrite.lib` — DirectWrite text
- `dcomp.lib` — DirectComposition (transparent overlays)

### Minimal Example

```cpp
#include "include/core/UIManager.h"
#include "include/skins/DefaultSkin.h"

class MyApp : public UIManager
{
public:
    std::unique_ptr<Skin> CreateSkin() override
    {
        return std::make_unique<DefaultSkin>();
    }

protected:
    std::wstring GetConfigAppName() const override { return L"MyApp"; }

    void RegisterBindings() override
    {
        GetConfig().Bind(L"enabled", &m_enabled);
        GetConfig().Bind(L"speed",   &m_speed);
    }

    void BuildUI() override
    {
        AddWindow(L"My Application", 100, 100, 400, 300,
            Props().Padding(10), [&]()
        {
            AddGroupBox(Props().Title(L"Settings").Padding(10), [&]()
            {
                AddCheckbox(Props().Text(L"Enable Feature")
                    .Label(ElementPosition::Left)
                    .Bind(&m_enabled));

                AddSlider(Props().Label(L"Speed", ElementPosition::Above)
                    .Range(0, 100).Bind(&m_speed));
            });
        });
    }

    bool  m_enabled = false;
    float m_speed    = 50.f;
};
```

**In your WinMain or render loop:**

```cpp
MyApp g_ui;

// one-time setup — pass your D3D11 device and swap chain
g_ui.Initialize(hwnd, d3dDevice, swapChain);

// per frame
g_ui.NewFrame();
g_ui.Render();
swapChain->Present(1, 0);

// shutdown
g_ui.Shutdown();
```

A complete runnable example with D3D11/DComp setup is provided in `ChameleonUI/examples/main.cpp`.

---

## Architecture

### DX11 Guest Model

ChameleonUI never creates D3D devices, swap chains, or windows. It accepts **your** pre-existing D3D11 device and swap chain via `Initialize()`, creates D2D/DWrite/DComp internally, and renders into the host's swap chain each frame.

```
  Your App                          ChameleonUI
  ─────────                         ────────────
  Create D3D11Device ──────────────► Initialize()
  Create SwapChain  ──────────────►     creates D2D factory
  Create HWND                                  DWrite factory
                                         DComp device + visual

  while(running) {
      NewFrame() ◄──────────────────  begin draw, capture desktop
      Render()   ◄──────────────────  draw UI tree via Skin
      Present()                        end draw
  }

  Shutdown() ◄──────────────────────  release D2D/DWrite/DComp
```

### Lifecycle

| Phase | Method | Purpose |
|---|---|---|
| Construction | Constructor | Instantiate your derived class |
| Init | `Initialize(hwnd, d3dDevice, swapChain)` | Set up D2D/DWrite/DComp, load config |
| Per-frame | `NewFrame()` + `Render()` | Begin draw, capture desktop, render UI |
| Shutdown | `Shutdown()` | Save config, release resources |

### Key Overrides

| Virtual Method | Purpose |
|---|---|
| `BuildUI()` | Declare the entire UI tree using `Add*()` methods |
| `CreateSkin()` | Return the skin instance used for rendering |
| `RegisterBindings()` | Bind member variables to config keys for persistence |
| `GetConfigAppName()` | Set the `%APPDATA%/<name>/` folder for config files |
| `GetDefaultConfigName()` | Name of the config file loaded on startup |

---

## Skinning System

ChameleonUI provides two complementary tiers of customization.

### Tier 1 — UIStyle (data-driven)

The `UIStyle` struct contains ~120 color, size, font, and animation parameters. Override `CreateDefaultStyle()` in your `Skin` subclass to set them:

```cpp
class MySkin : public DefaultSkin
{
protected:
    std::unique_ptr<UIStyle> CreateDefaultStyle() const override
    {
        auto s = std::make_unique<UIStyle>();
        s->accent           = Color(0, 200, 120, 255);
        s->windowBackground = Color(20, 20, 28, 240);
        s->cornerRadius     = 12.f;
        s->buttonMinHeight  = 36.f;
        s->anim.hoverIn     = 0.12f;   // hover transition duration
        s->fontFamily       = L"Inter";
        s->fontSize         = 14;
        return s;
    }
};
```

### Tier 2 — Skin inheritance (structural)

Override individual `Draw*()` methods for fundamentally different rendering:

```cpp
class NeonSkin : public DefaultSkin
{
public:
    void DrawButton(IRenderer* renderer, const Button& button,
                    const Rect& absRect, bool hovered, bool pressed) override
    {
        UIStyle* s = GetStyle();
        float hoverT = button.GetAnimState().hoverT.current;

        // glow behind the button on hover
        if (hoverT > 0.01f)
        {
            Color glow = s->accent.ScaleAlpha((uint8_t)(hoverT * 80));
            renderer->DrawDropShadow(absRect, s->cornerRadius, glow, 0, 0, 12.f);
        }

        // gradient fill
        Color top = Color::Lerp(s->buttonNormal, s->buttonHovered, hoverT);
        Color bot = top.Darken(0.15f);
        renderer->DrawGradientRect(absRect, top, bot, false, s->cornerRadius);

        // border
        renderer->DrawOutlineRect(absRect, s->buttonBorder, 1.f, s->cornerRadius);

        // text
        renderer->DrawText(button.GetText(), absRect, s->buttonText,
                           s->fontSize, TextAnchor::Center, s->fontFamily.c_str());
    }

    void OnButtonPressed(Button& button) override
    {
        button.GetAnimState().pressT.TransitionTo(1.f, 0.05f);
        button.GetAnimState().glowT.TransitionTo(1.f, 0.15f);
    }

    void OnButtonReleased(Button& button) override
    {
        button.GetAnimState().pressT.TransitionTo(0.f, 0.2f);
        button.GetAnimState().glowT.TransitionTo(0.f, 0.4f);
    }
};
```

### Per-widget overrides

Any control can use a different skin without affecting the rest of the UI:

```cpp
// create a second skin instance
Skin* myAltSkin = AddSkin<FrutigerAeroSkin>();

// apply it to individual widgets
AddButton(Props().Text(L"Styled").UseSkin(myAltSkin));
AddCheckbox(Props().Text(L"Retro").UseSkin(myAltSkin));
```

### Built-in skins

| Skin | Style |
|---|---|
| `DefaultSkin` | Modern glassmorphic dark theme with smooth animations |
| `FrutigerAeroSkin` | Retro 3D-beveled Windows Aero aesthetic |

---

## Control Reference

| Control | Description | Key Props |
|---|---|---|
| **Window** | Top-level draggable container | `TitleBarHeight`, `NoTitleBar`, position, size |
| **Button** | Clickable push button with callback | `Text`, `Icon`, `Width`, `Height`, `SetOnClick()` |
| **Checkbox** | Toggle with label and optional color swatch | `Text`, `Label`, `Bind(bool*)`, `ColorSwatch(Color*)` |
| **Slider** | Horizontal range slider | `Range(min,max)`, `Value`, `Bind(float*)`, `Label` |
| **ComboBox** | Dropdown selection (single or multi-select) | `Items({...})`, `Bind(int*)`, `ComboMode`, `MaxVisible` |
| **TextInput** | Single or multi-line text field | `Placeholder`, `Multiline`, `Bind(wstring*)` |
| **KeyBind** | Keyboard shortcut capture | `Bind(int*)` |
| **ListBox** | Scrollable item list (single or multi-select) | `Items({...})`, `ListMode`, `MaxVisible`, `Bind(int*)` |
| **NumericInput** | Numeric stepper with increment/decrement | `Range`, `Step`, `Precision`, `IntegerMode` |
| **ColorPicker** | Inline HSV color picker | `HueBar(Right/Left)`, `ShowAlpha()`, `Bind(Color*)` |
| **ColorSwatch** | Clickable color preview that opens picker | `Color*`, `SwatchSize` |
| **Label** | Static read-only text | Text, `Align` |
| **Image** | Image display from bitmap or embedded data | `ImageSource`, `ImageNaturalSize`, `ImageOpacity` |
| **Separator** | Horizontal or vertical divider line | `Margin`, `separatorVertical` |
| **GroupBox** | Titled bordered container | `Title`, `Padding`, `Scrollable` |
| **TabControl** | Tab strip with page switching | `TabOrientation`, `TabAlign`, `StripSize`, `Transition`, `Detached` |
| **TabPage** | Single page inside a TabControl | Title, `Icon`, `IconSize`, `Scrollable` |
| **TabPageHost** | Detached tab body host | TabControl reference |
| **GridLayout** | Flexible row/column grid | `AddRow(Star/Auto/Fixed)`, `AddColumn(Star/Auto/Fixed)` |
| **Panel** | Scrollable container | `ScrollMode` |
| **ContentSwitcher** | Index-driven content switching | `SetIndexBinding()` |
| **ConfigSelector** | Built-in config profile save/load widget | `MaxVisible` |

---

## Props Builder Reference

Every `Add*()` call accepts a `Props()` builder object. `UIProps` is a chainable struct — call setters in any order, ending with the control-specific parameters.

```cpp
// props() is a free function returning UIProps{}
AddSlider(Props().Label(L"Volume", ElementPosition::Above)
    .Range(0.0f, 1.0f).Value(0.75f).Bind(&m_volume).Indent(26));
```

### Layout

| Prop | Arguments | Effect |
|---|---|---|
| `Padding(int all)` | px | Uniform padding inside the element |
| `Padding(int lr, int tb)` | px, px | Horizontal + vertical padding |
| `Padding(int l, int t, int r, int b)` | px × 4 | Per-side padding |
| `Margin(int all)` | px | Uniform margin outside the element |
| `Margin(int l, int t, int r, int b)` | px × 4 | Per-side margin |
| `Width(AutoSize, int px=0)` | `Auto\|Fill\|Fixed`, px | Width policy: content-sized, stretch, or fixed |
| `Height(AutoSize, int px=0)` | `Auto\|Fill\|Fixed`, px | Height policy: content-sized, stretch, or fixed |
| `Align(HorizontalAlignment)` | enum | Horizontal alignment within parent |
| `Scrollable(bool=true)` | bool | Enable vertical scrolling (groupboxes, tab pages) |
| `Indent(float px)` | px | Left indent for the control body when a label is above |

### Label

| Prop | Arguments | Effect |
|---|---|---|
| `Label(text, position, spacing)` | wstring, ElementPosition, float | Attaches a label at `Above`, `Left`, `Right`, or `None` |
| `ClickLabel(bool=true)` | bool | When true, clicking the label toggles the bound checkbox |

### Data Binding

| Prop | Arguments | Effect |
|---|---|---|
| `Bind(bool*)` | pointer | Two-way bind to a bool (checkbox) |
| `Bind(int*)` | pointer | Two-way bind to an int (combobox, keybind, listbox index) |
| `Bind(float*)` | pointer | Two-way bind to a float (slider, numeric input) |
| `Bind(wstring*)` | pointer | Two-way bind to a wstring (text input) |
| `Bind(Color*)` | pointer | Two-way bind to a Color (color picker, swatch) |

All bound values are automatically saved/loaded via the config system when registered in `RegisterBindings()`.

### Text & Content

| Prop | Arguments | Used By |
|---|---|---|
| `Title(wstring)` | Window/groupbox title | Window, GroupBox |
| `Text(wstring)` | Display text | Button, Checkbox, Label |
| `Placeholder(wstring)` | Hint text when empty | TextInput |
| `Items({...})` | vector or initializer list of wstrings | ComboBox, ListBox |
| `Range(min, max)` | float, float | Slider, NumericInput |
| `Value(float)` | Initial value | Slider, NumericInput |
| `Key(int)` | Virtual key code | KeyBind |
| `Multiline(bool=true)` | Enable multi-line mode | TextInput |
| `ComboMode(ComboBoxMode)` | `SingleSelect` or `MultiSelect` | ComboBox |
| `ListMode(ListBoxMode)` | `SingleSelect` or `MultiSelect` | ListBox |
| `MaxVisible(int)` | Max dropdown/visible rows | ComboBox, ListBox, ConfigSelector |

### Tabs

| Prop | Arguments | Effect |
|---|---|---|
| `TabAlign(TabAlignment)` | `AlignLeft`, `AlignCenter`, `AlignRight`, `Fill` | Tab button alignment within the strip |
| `StripSize(int)` | px | Tab strip thickness (height for horizontal, width for vertical) |
| `ButtonSize(int)` | px | Per-tab button size along the strip axis |
| `Transition(TabTransition)` | `SlideHorizontal`, `SlideVertical`, `Fade` | Page switch animation |
| `TabScrollbar(TabScrollbarPosition)` | `Top`, `Bottom`, `Auto` | Scrollbar position for overflowing tabs |
| `Detached(bool=true)` | bool | TabPageHost mode — the tab strip floats independently |
| `StripFit(bool=true)` | bool | Shrink tab buttons to fit the available strip width |

### Icons

| Prop | Arguments | Effect |
|---|---|---|
| `Icon(handle, pos)` | `void*` or `IconRef`, `IconPosition` | Sets the icon image and position (`Left`/`Right`) |
| `IconSize(w, h)` | float, float | Override the icon's natural display size |
| `IconGap(float)` | px | Spacing between icon and text |
| `ShowText(bool)` | bool | Hide the text label, showing only the icon |

### Color Swatch

| Prop | Arguments | Effect |
|---|---|---|
| `ColorSwatch(Color*, align)` | pointer, `ColorSwatchAlignment` | Adds a colored square (default right-aligned) to a Checkbox |
| `SwatchSpacing(float)` | px | Gap between the swatch and the control |
| `SwatchSize(w, h)` | float, float | Override swatch dimensions |

### Color Picker

| Prop | Arguments | Effect |
|---|---|---|
| `HueBar(HueBarPosition)` | `Left`, `Right` | Position of the hue gradient bar |
| `ShowAlpha(bool=true)` | bool | Show the alpha (transparency) bar |

### Numeric Input

| Prop | Arguments | Effect |
|---|---|---|
| `Step(float)` | Increment per click | Step size for the +/- buttons |
| `Precision(int)` | Decimal digits | Number of decimal places displayed |
| `IntegerMode(bool=true)` | bool | Restrict to whole numbers only |

### Image

| Prop | Arguments | Effect |
|---|---|---|
| `ImageSource(void*)` | Bitmap handle | Sets the image to display |
| `ImageOpacity(float)` | 0.0 – 1.0 | Opacity multiplier |
| `ImageNaturalSize(w, h)` | float, float | Source image dimensions for aspect ratio |

### Window

| Prop | Arguments | Effect |
|---|---|---|
| `NoTitleBar(bool=true)` | bool | Hide the title bar and drag handle |
| `TitleBarHeight(int)` | px | Override the default title bar height |

### Tooltip

| Prop | Arguments | Effect |
|---|---|---|
| `Tooltip(wstring)` | Tooltip text | Shows a tooltip on hover (any control) |

### Skin Override

| Prop | Arguments | Effect |
|---|---|---|
| `UseSkin(Skin*)` | Pointer to a registered skin | Render this specific widget with a different skin |

### Separator

| Prop | Arguments | Effect |
|---|---|---|
| `separatorVertical` (member) | bool | Draw a vertical line instead of horizontal |

---

## Layout System

ChameleonUI provides flexible layout options:

### GridLayout

```cpp
AddGrid(Props(), [&](GridLayout& grid)
{
    grid.AddRow(GridSize::Star());          // stretches to fill
    grid.AddRow(GridSize::Fixed(80));       // exact pixel height
    grid.AddColumn(GridSize::Fixed(200));   // exact pixel width
    grid.AddColumn(GridSize::Star());       // takes remaining space
    grid.AddColumn(GridSize::Auto());       // sizes to content

    AddCell(grid, 0, 0, [&]() { /* row 0, col 0 */ });
    AddCell(grid, 1, 1, [&]() { /* row 1, col 1 */ });
});
```

### Nesting

All containers support infinite nesting — put a grid inside a tab page inside a groupbox inside a window:

```cpp
AddWindow(...)
    AddTabControl(...)
        AddTabPage(...)
            AddGroupBox(...)
                AddGrid(...)
                    AddCell(...)
```

### Auto-sizing

Controls and containers accept `AutoSize::Star` (fill), `AutoSize::Fixed(px)`, and `AutoSize::Auto` (content-sized) for width and height via `Props().Width(...)` and `Props().Height(...)`.

---

## Icon Atlas System

The `atlas_gen.py` script in `ChameleonUI/src/util/tools/` converts a folder of SVG or PNG icons into a single embeddable C++ header.

### Prerequisites

- **Python 3** — no additional packages required
- **Spreet** (SVG input) — [github.com/priteau/spreet](https://github.com/priteau/spreet)
- **ImageMagick** (PNG input) — [imagemagick.org](https://imagemagick.org/script/download.php)

### Usage

```bash
# basic — outputs icons.h in the input folder
python atlas_gen.py /path/to/icon_folder

# custom output name
python atlas_gen.py /path/to/icon_folder -o my_atlas

# custom path and C++ array name
python atlas_gen.py /path/to/icon_folder -o /custom/path.h --name GameIcons
```

### Input formats

- **SVG** — Spreet auto-packs into an efficient sprite sheet
- **PNG** — ImageMagick creates a grid atlas (configurable cell size and columns)

### Output

A single `.h` file containing:
- `k<Name>_AtlasData[]` — embedded PNG binary
- `k<Name>_AtlasDataSize` — byte size
- `k<Name>[]` — `IconData` array mapping names to pixel coordinates/sizes

### Registration

```cpp
#include "assets/icons/my_icons.h"

void MyApp::OnBeforeBuildUI() override
{
    RegisterIcons(kMyIcons_AtlasData, kMyIcons_AtlasDataSize,
                  kMyIcons, kMyIconsCount);
}
```

Icons are then retrieved by name:

```cpp
AddTabPage(L"Settings", Props()
    .Icon(RetrieveIcon(L"settings_gear"))
    .IconSize(24, 24),
    [&]() { /* page content */ });
```

---

## Configuration & Localization

### Configuration

All variables bound in `RegisterBindings()` are automatically persisted to `%APPDATA%/<AppName>/<ConfigName>.json`. Named profiles are managed via the built-in `AddConfigSelector()` widget.

```cpp
void RegisterBindings() override
{
    auto& c = GetConfig();
    c.Bind(L"display.fullscreen", &m_fullscreen);
    c.Bind(L"display.vsync",      &m_vsync);
    c.Bind(L"audio.volume",       &m_volume);
}

std::wstring GetConfigAppName() const override     { return L"MyApp"; }
std::wstring GetDefaultConfigName() const override  { return L"default"; }
```

### Localization

Register language packs once, then switch at runtime. The UI tree is rebuilt automatically while preserving window positions, tab selections, and bound data.

```cpp
void OnBeforeBuildUI() override
{
    RegisterLanguage(L"en", kStrings_en, kStrings_en_Count);
    RegisterLanguage(L"ru", kStrings_ru, kStrings_ru_Count);
}

// switching language rebuilds the entire UI tree in-place
SetLanguage(L"ru");
```

---

## Advanced Topics

### Custom controls

Create new controls by inheriting from `UIElement`:

```cpp
class MyGauge : public UIElement
{
protected:
    void OnDraw(IRenderer* r, const Rect& absRect) override { /* ... */ }
    void OnLayout(const Rect& parentRect) override         { /* ... */ }
    bool OnInput(const InputEvent& e) override              { /* ... */ }
};
```

Register them in `BuildUI()`:

```cpp
parent->AddChild<MyGauge>(Props().Width(AutoSize::Fill).Height(AutoSize::Fixed, 40));
```

### Animation engine

Every control exposes an `AnimState` with multiple float channels: `hoverT`, `pressT`, `focusT`, `checkT`, `glowT`. Override `On*()` callbacks in your `Skin` to drive transitions:

```cpp
void OnButtonHoverEnter(Button& button) override
{
    button.GetAnimState().hoverT.TransitionTo(1.f, 0.12f, Easing::CubicOut);
}

void OnButtonHoverLeave(Button& button) override
{
    button.GetAnimState().hoverT.TransitionTo(0.f, 0.25f, Easing::QuadOut);
}
```

Available easing functions: `Linear`, `QuadOut`, `CubicOut`, `ExpoOut`, `BackOut`, `ElasticOut`, `SmoothStep`, `OvershootOut`.

### Desktop capture & glassmorphism

Enable real-time desktop capture for a glass blur effect behind UI windows:

```cpp
// toggle at runtime
SetDesktopCaptureEnabled(true);
```

Desktop capture uses DXGI Output Duplication. The captured frame is Gaussian-blurred and drawn behind every window's background. Call `SetDesktopCaptureEnabled(false)` to disable if you don't need the effect.

---

## License

MIT — see [LICENSE](LICENSE) for full text.

---

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

When contributing:
- Follow the existing code style and patterns
- Keep comments short, lowercase, and explanatory
- Use `#pragma region` to organize large files
- Test with both `DefaultSkin` and `FrutigerAeroSkin`

---

<p align="center">
  <sub>Built with Direct2D, DirectWrite, and DirectComposition</sub>
</p>
