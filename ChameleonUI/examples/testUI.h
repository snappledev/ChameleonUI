#pragma once
#include "../include/core/UIManager.h"
#include "../include/skins/FrutigerAeroSkin.h"
#include "../include/assets/icons/feathericons.h"
#include "../include/assets/lang/lang_en.h"
#include "../include/assets/lang/lang_ru.h"

// ═══════════════════════════════════════════════════════════════════════════
//  testUI — Feature showcase for the UIFramework
//
//  Demonstrates:
//    • Localization (English + Russian) with live language switching
//    • Vertical & horizontal TabControls with all alignment modes
//    • All three page transitions (SlideHorizontal, SlideVertical, Fade)
//    • Tab strip scrollbar (Top, Bottom, Auto)
//    • Detached TabControl + TabPageHost (horizontal and vertical)
//    • Grid layouts with Star/Auto/Fixed columns
//    • GroupBoxes with padding
//    • Every leaf control: Checkbox, Slider, ComboBox, TextInput,
//      KeyBind, Button, Label, ListBox, ColorSwatch
//    • Data binding (bool, int, float, string, color)
//    • Icon atlases on tab buttons
// ═══════════════════════════════════════════════════════════════════════════

class testUI : public UIManager
{
public:
    std::unique_ptr<Skin> CreateSkin() override
    {
        return std::make_unique<FrutigerAeroSkin>();
    }
protected:
    // ── Config: app folder name → %APPDATA%/testUI/ ─────────────────────
    std::wstring GetConfigAppName() const override { return L"testUI"; }
    std::wstring GetDefaultConfigName() const override { return L"default"; }

    // ── Config: bind all members to serialization keys ──────────────────
    void RegisterBindings() override
    {
        auto& c = GetConfig();

        // General
        c.Bind(L"general.enable", &m_enable);
        c.Bind(L"general.overlay", &m_overlay);
        c.Bind(L"general.autosave", &m_autoSave);
        c.Bind(L"general.opacity", &m_opacity);
        c.Bind(L"general.quality", &m_quality);
        c.Bind(L"general.overlay_color", &m_overlayColor);
        c.Bind(L"general.accent_color", &m_accentColor);

        // Input
        c.Bind(L"input.username", &m_username);
        c.Bind(L"input.address", &m_address);
        c.Bind(L"input.toggle_key", &m_toggleKey);
        c.Bind(L"input.action_key", &m_actionKey);

        // Aim
        c.Bind(L"aim.enabled", &m_aimEnable);
        c.Bind(L"aim.fov", &m_aimFov);
        c.Bind(L"aim.trigger_active", &m_triggerActive);
        c.Bind(L"aim.rcs", &m_rcs);
        c.Bind(L"aim.autoscope", &m_autoScope);

        // Visuals
        c.Bind(L"esp.box", &m_espBox);
        c.Bind(L"esp.name", &m_espName);
        c.Bind(L"esp.health", &m_espHealth);
        c.Bind(L"esp.box_color", &m_espBoxColor);
        c.Bind(L"esp.name_color", &m_espNameColor);
        c.Bind(L"chams.enabled", &m_chams);
        c.Bind(L"chams.color", &m_chamsColor);

        // Config page
        c.Bind(L"config.index", &m_configIdx);
        c.Bind(L"config.name", &m_configName);
        c.Bind(L"display.fullscreen", &m_fullscreen);
        c.Bind(L"display.vsync", &m_vsync);

        // Effects
        c.Bind(L"effects.walls", &m_walls);
        c.Bind(L"effects.glow", &m_glow);
        c.Bind(L"effects.trail", &m_trail);
        c.Bind(L"effects.glow_color", &m_glowColor);
        c.Bind(L"effects.trail_color", &m_trailColor);

        // Settings
        c.Bind(L"settings.lang_idx", &m_langIdx);

        // Misc
        c.Bind(L"misc.radar", &m_radar);
        c.Bind(L"misc.crosshair", &m_crosshair);
        c.Bind(L"misc.crosshair_color", &m_crosshairColor);
        c.Bind(L"misc.option_a", &m_optA);
        c.Bind(L"misc.option_b", &m_optB);
    }
    void OnBeforeBuildUI() override
    {
        UIManager::OnBeforeBuildUI();
        m_detachedHoriz = nullptr;
        m_detachedVert = nullptr;
        m_titleBarTabs = nullptr;
    }
    void BuildUI() override
    {
        // ── Register languages (before any UI strings are used) ──
        RegisterLanguage(L"en", kStrings_en, kStrings_en_Count);
        RegisterLanguage(L"ru", kStrings_ru, kStrings_ru_Count);
        
        RegisterIcons(kfeathericonsIcons_AtlasData, kfeathericonsIcons_AtlasDataSize,
            kfeathericonsIcons, kfeathericonsIconsCount);

        AddWindow(L"", 100, 60, 960, 780,
            Props().Padding(10).TitleBarHeight(80),
            [&]() {
                // Title bar content — use a grid: image left, tabs right
                AddGrid(Props().Width(AutoSize::Fill).Height(AutoSize::Fill), [&](GridLayout& grid)
                    {
                        grid.AddRow(GridSize::Star());
                        grid.AddColumn(GridSize::Auto());   // image column
                        grid.AddColumn(GridSize::Star());   // tabs fill remaining
                        grid.AddColumn(GridSize::Auto());   // tabs fill remaining
                        AddCell(grid, 0, 0, [&]()
                            {
                                void* logo = GetRenderer()->LoadImageFromMemory(
                                    kfeathericonsIcons_AtlasData, kfeathericonsIcons_AtlasDataSize);
                                AddImage(Props()
                                    .ImageSource(logo)
                                    .ImageNaturalSize(32, 32)
                                    .Width(AutoSize::Fixed, 32)
                                    .Height(AutoSize::Fixed, 32)
                                    .Margin(0, 0, 12, 0));
                            });
                       /* { L"bold", 0, 312, 24, 24 },
                        { L"book", 0, 336, 24, 24 },
                        { L"book-open", 0, 360, 24, 24 },
                        { L"bookmark", 0, 384, 24, 24 },
                        { L"box", 0, 408, 24, 24 },
                        { L"briefcase", 0, 432, 24, 24 },
                        { L"calendar", 0, 456, 24, 24 },*/
                        AddCell(grid, 0, 2, [&]()
                            {
                                m_titleBarTabs = &AddTabControl(TabOrientation::Horizontal,
                                    Props().Width(AutoSize::Auto).Height(AutoSize::Fill)
                                    .StripSize(36)
                                    .TabAlign(TabAlignment::AlignRight).StripFit()
                                    .Detached()
                                    .Transition(TabTransition::SlideHorizontal),
                                    [&]() {
                                        AddTabPage(L"File", [&]() {
                                            AddTabControl(TabOrientation::Vertical,
                                                Props().Margin(4)
                                                .TabAlign(TabAlignment::Fill)
                                                .StripSize(80)
                                                .Transition(TabTransition::SlideVertical),
                                                [&]()
                                                {
                                                    // ──────────────────────────────────────────────────
                                                    //  PAGE 1 — All Controls + Grid Layout
                                                    // ──────────────────────────────────────────────────
                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                        .Icon(RetrieveIcon(L"bold")).IconSize(34, 34),
                                                        [&]()
                                                        {
                                                            AddGrid(Props().Margin(2), [&](GridLayout& grid)
                                                                {
                                                                    grid.AddRow(GridSize::Star());
                                                                    grid.AddColumn(GridSize::Star());
                                                                    grid.AddColumn(GridSize::Star());

                                                                    AddCell(grid, 0, 0, [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"general")).Margin(2).Padding(10).Height(AutoSize::Fill), [&]()
                                                                                {
                                                                                    AddCheckbox(Props().Text(Tr(L"general.enable"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_enable));
                                                                                    AddCheckbox(Props().Text(Tr(L"general.overlay"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_overlay)
                                                                                        .ColorSwatch(&m_overlayColor));
                                                                                    AddCheckbox(Props().Text(Tr(L"general.autosave"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_autoSave)
                                                                                        .ColorSwatch(&m_accentColor));

                                                                                    AddSlider(Props().Label(Tr(L"general.opacity"), ElementPosition::Above)
                                                                                        .Range(0, 100).Bind(&m_opacity).Indent(26));
                                                                                    AddSlider(Props().Label(Tr(L"general.speed"), ElementPosition::Above)
                                                                                        .Range(0.1f, 10.f).Value(2.5f).Indent(26));

                                                                                    AddComboBox(Props().Label(Tr(L"general.quality"), ElementPosition::Above).Indent(26)
                                                                                        .Items({ Tr(L"quality.low"), Tr(L"quality.medium"),
                                                                                                 Tr(L"quality.high"), Tr(L"quality.ultra") })
                                                                                        .Bind(&m_quality));
                                                                                });
                                                                        });

                                                                    AddCell(grid, 0, 1, [&]()
                                                                        {
                                                                            AddGrid(Props().Margin(2), [&](GridLayout& inner)
                                                                                {
                                                                                    inner.AddRow(GridSize::Star());
                                                                                    inner.AddRow(GridSize::Star());
                                                                                    inner.AddColumn(GridSize::Star());

                                                                                    AddCell(inner, 0, 0, [&]()
                                                                                        {
                                                                                            AddGroupBox(Props().Title(Tr(L"input")).Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddTextInput(Props().Label(Tr(L"input.username"), ElementPosition::Above)
                                                                                                        .Placeholder(Tr(L"input.username.hint")).Bind(&m_username).Indent(26));
                                                                                                    AddTextInput(Props().Label(Tr(L"input.address"), ElementPosition::Above)
                                                                                                        .Placeholder(L"127.0.0.1").Bind(&m_address).Indent(26));
                                                                                                    AddKeyBind(Props().Label(Tr(L"input.toggle_key"), ElementPosition::Left)
                                                                                                        .Bind(&m_toggleKey).Width(AutoSize::Auto).Indent(26));
                                                                                                    AddKeyBind(Props().Label(Tr(L"input.action_key"), ElementPosition::Left)
                                                                                                        .Bind(&m_actionKey).Width(AutoSize::Auto).Indent(26));

                                                                                                    // ── Separator between sections ──
                                                                                                    AddSeparator(Props().Margin(0, 8, 0, 8));

                                                                                                    // ── Image from resource ──
                                                                                                    void* banner = nullptr;// GetRenderer()->LoadImageFromResource(IDR_ICON_CSGO);//AI REPLACE THIS REGISTER ICONS WITH OUR BYTE DATA
                                                                                                    AddImage(Props()
                                                                                                        .ImageSource(banner)
                                                                                                        .ImageNaturalSize(400, 100)
                                                                                                        .Width(AutoSize::Fixed, 400)
                                                                                                        .Height(AutoSize::Fixed, 100));

                                                                                                    // ── Tooltip on any existing control ──
                                                                                                    // Just add .Tooltip() to any existing Props() chain:
                                                                                                    AddCheckbox(Props().Text(Tr(L"general.enable"))
                                                                                                        .Label(ElementPosition::Left).Bind(&m_enable)
                                                                                                        .Tooltip(Tr(L"tooltip.enable")));  // ← localized tooltip

                                                                                                    AddSlider(Props().Label(Tr(L"aim.fov"), ElementPosition::Above)
                                                                                                        .Range(0, 180).Bind(&m_aimFov).Indent(26)
                                                                                                        .Tooltip(Tr(L"tooltip.aim_fov")));  // ← localized tooltip

                                                                                                    // ── Tooltip on a button ──
                                                                                                    AddButton(Props().Text(Tr(L"btn.save_config"))
                                                                                                        .Tooltip(Tr(L"tooltip.save_config")));

                                                                                                    // ── Separator vertical (inside a grid row) ──
                                                                                                    AddSeparator(Props().Margin(8, 0, 8, 0)
                                                                                                        .Width(AutoSize::Fixed, 1).Height(AutoSize::Fill));
                                                                                                });
                                                                                        });

                                                                                    AddCell(inner, 1, 0, [&]()
                                                                                        {
                                                                                            AddGroupBox(Props().Title(Tr(L"presets")).Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddListBox(Props()
                                                                                                        .Items({ Tr(L"presets.default"), Tr(L"presets.competitive"),
                                                                                                                 Tr(L"presets.casual"), Tr(L"presets.custom1"),
                                                                                                                 Tr(L"presets.custom2") })
                                                                                                        .MaxVisible(4));
                                                                                                    AddButton(Props().Text(Tr(L"btn.apply")));
                                                                                                    AddButton(Props().Text(Tr(L"btn.reset"))).SetOnClick([this]() {
                                                                                                        ResetConfig();
                                                                                                        });
                                                                                                });
                                                                                        });
                                                                                });
                                                                        });
                                                                });
                                                        });

                                                    // ──────────────────────────────────────────────────
                                                    //  PAGE 2 — Nested Horizontal Tabs + SlideHorizontal
                                                    // ──────────────────────────────────────────────────
                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                        .Icon(RetrieveIcon(L"bold")).IconSize(34, 34),
                                                        [&]()
                                                        {
                                                            AddTabControl(TabOrientation::Horizontal,
                                                                Props().Margin(4)
                                                                .TabAlign(TabAlignment::AlignLeft)
                                                                .StripSize(55)
                                                                .Transition(TabTransition::SlideHorizontal),
                                                                [&]()
                                                                {
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 23), [&]()
                                                                        {
                                                                            AddGrid(Props().Margin(2, 8, 2, 2), [&](GridLayout& g)
                                                                                {
                                                                                    g.AddRow(GridSize::Star());
                                                                                    g.AddColumn(GridSize::Star());
                                                                                    g.AddColumn(GridSize::Star());

                                                                                    AddCell(g, 0, 0, [&]()
                                                                                        {
                                                                                            AddCheckbox(Props().Text(Tr(L"aim.enabled"))
                                                                                                .Label(ElementPosition::Left).Bind(&m_aimEnable));
                                                                                            AddSlider(Props().Label(Tr(L"aim.fov"), ElementPosition::Above)
                                                                                                .Range(0, 180).Bind(&m_aimFov).Indent(26));
                                                                                            AddSlider(Props().Label(Tr(L"aim.smooth"), ElementPosition::Above)
                                                                                                .Range(1, 100).Value(15).Indent(26));
                                                                                            AddComboBox(Props().Label(Tr(L"aim.bone"), ElementPosition::Above).Indent(26)
                                                                                                .Items({ Tr(L"bone.head"), Tr(L"bone.neck"),
                                                                                                            Tr(L"bone.chest"), Tr(L"bone.pelvis") }));
                                                                                        });

                                                                                    AddCell(g, 0, 1, [&]()
                                                                                        {
                                                                                            AddGroupBox(Props().Title(Tr(L"triggerbot")).Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddCheckbox(Props().Text(Tr(L"triggerbot.active"))
                                                                                                        .Label(ElementPosition::Left).Bind(&m_triggerActive));
                                                                                                    AddSlider(Props().Label(Tr(L"triggerbot.delay"), ElementPosition::Above)
                                                                                                        .Range(0, 200).Value(25).Indent(26));
                                                                                                    AddKeyBind(Props().Label(Tr(L"triggerbot.hold_key"), ElementPosition::Left)
                                                                                                        .Width(AutoSize::Auto).Indent(26));
                                                                                                });
                                                                                        });
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(60, 35), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"pistol")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddCheckbox(Props().Text(Tr(L"pistol.rcs"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_rcs));
                                                                                    AddSlider(Props().Label(Tr(L"pistol.rcs_strength"), ElementPosition::Above)
                                                                                        .Range(0, 100).Value(80).Indent(26));
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(110, 45), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"sniper")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddCheckbox(Props().Text(Tr(L"sniper.autoscope"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_autoScope));
                                                                                    AddSlider(Props().Label(Tr(L"sniper.zoom_sens"), ElementPosition::Above)
                                                                                        .Range(0.1f, 5.f).Value(1.f).Indent(26));
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.rifle"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(80, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.melee"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.famas"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.galil"));
                                                                        });
                                                                });
                                                        });

                                                    // ──────────────────────────────────────────────────
                                                    //  PAGE 3 — Fade + AlignCenter + Bottom Scrollbar
                                                    // ──────────────────────────────────────────────────
                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                        .Icon(RetrieveIcon(L"bold")).IconSize(34, 34), [&]()
                                                        {
                                                            AddTabControl(TabOrientation::Horizontal,
                                                                Props().Margin(4)
                                                                .TabAlign(TabAlignment::AlignCenter)
                                                                .Transition(TabTransition::Fade)
                                                                .TabScrollbar(TabScrollbarPosition::Bottom), [&]()
                                                                {
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 23), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"esp")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddCheckbox(Props().Text(Tr(L"esp.box"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_espBox)
                                                                                        .ColorSwatch(&m_espBoxColor));
                                                                                    AddCheckbox(Props().Text(Tr(L"esp.name"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_espName)
                                                                                        .ColorSwatch(&m_espNameColor));
                                                                                    AddCheckbox(Props().Text(Tr(L"esp.health"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_espHealth));
                                                                                    AddSlider(Props().Label(Tr(L"esp.distance"), ElementPosition::Above)
                                                                                        .Range(100, 5000).Value(2000).Indent(26));
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(60, 35), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"chams")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddCheckbox(Props().Text(Tr(L"chams.enable"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_chams)
                                                                                        .ColorSwatch(&m_chamsColor));
                                                                                    AddComboBox(Props().Label(Tr(L"chams.material"), ElementPosition::Above).Indent(26)
                                                                                        .Items({ Tr(L"material.flat"), Tr(L"material.textured"),
                                                                                                 Tr(L"material.metallic"), Tr(L"material.glass") }));
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(110, 45), [&]() {
                                                                            AddLabel(Tr(L"placeholder.glow"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.world"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(80, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.viewmodel"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.sky"));
                                                                        });
                                                                });
                                                        });

                                                    // ──────────────────────────────────────────────────
                                                    //  PAGE 4 — Top Scrollbar + AlignLeft
                                                    // ──────────────────────────────────────────────────
                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                        .Icon(RetrieveIcon(L"bold")).IconSize(34, 34), [&]()
                                                        {
                                                            AddTabControl(TabOrientation::Horizontal,
                                                                Props().Margin(4)
                                                                .TabAlign(TabAlignment::AlignLeft)
                                                                .Transition(TabTransition::SlideHorizontal)
                                                                .TabScrollbar(TabScrollbarPosition::Top), [&]()
                                                                {
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 23), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"bold")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddConfigSelector(Props().MaxVisible(5));
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(60, 35), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"display")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddComboBox(Props().Label(Tr(L"display.resolution"), ElementPosition::Above).Indent(26)
                                                                                        .Items({ L"1280x720", L"1920x1080", L"2560x1440", L"3840x2160" }));
                                                                                    AddCheckbox(Props().Text(Tr(L"display.fullscreen"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_fullscreen));
                                                                                    AddCheckbox(Props().Text(Tr(L"display.vsync"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_vsync));
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(110, 45), [&]() {
                                                                            AddLabel(Tr(L"placeholder.audio"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.network"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(80, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.binds"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 26), [&]() {
                                                                            AddLabel(Tr(L"placeholder.advanced"));
                                                                        });
                                                                });
                                                        });

                                                    // ──────────────────────────────────────────────────
                                                    //  PAGE 5 — Detached Horizontal + Language Selector
                                                    // ──────────────────────────────────────────────────
                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                        .Icon(RetrieveIcon(L"bold")).IconSize(34, 34), [&]()
                                                        {
                                                            AddGrid(Props().Margin(2, 8, 2, 2), [&](GridLayout& g)
                                                                {
                                                                    g.AddRow(GridSize::Auto());
                                                                    g.AddRow(GridSize::Star());
                                                                    g.AddColumn(GridSize::Star());

                                                                    AddCell(g, 0, 0, [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"navigation")).Margin(2).Padding(6), [&]()
                                                                                {
                                                                                    m_detachedHoriz = &AddTabControl(TabOrientation::Horizontal,
                                                                                        Props().Margin(2)
                                                                                        .TabAlign(TabAlignment::AlignCenter)
                                                                                        .StripSize(40)
                                                                                        .Detached()
                                                                                        .Transition(TabTransition::Fade), [&]()
                                                                                        {
                                                                                            AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                                                .Icon(RetrieveIcon(L"bold")).IconSize(24, 24), [&]()
                                                                                                {
                                                                                                    AddCheckbox(Props().Text(Tr(L"walls"))
                                                                                                        .Label(ElementPosition::Left).Bind(&m_walls));
                                                                                                    AddSlider(Props().Label(Tr(L"walls.alpha"), ElementPosition::Above)
                                                                                                        .Range(0, 255).Value(200).Indent(26));
                                                                                                    AddComboBox(Props().Label(Tr(L"walls.mode"), ElementPosition::Above).Indent(26)
                                                                                                        .Items({ Tr(L"mode.outline"), Tr(L"mode.filled"),
                                                                                                                 Tr(L"mode.wireframe") }));
                                                                                                });

                                                                                            AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                                                .Icon(RetrieveIcon(L"bold")).IconSize(24, 24), [&]()
                                                                                                {
                                                                                                    AddCheckbox(Props().Text(Tr(L"effects.glow"))
                                                                                                        .Label(ElementPosition::Left).Bind(&m_glow)
                                                                                                        .ColorSwatch(&m_glowColor));
                                                                                                    AddCheckbox(Props().Text(Tr(L"effects.trail"))
                                                                                                        .Label(ElementPosition::Left).Bind(&m_trail)
                                                                                                        .ColorSwatch(&m_trailColor));
                                                                                                    AddSlider(Props().Label(Tr(L"effects.intensity"), ElementPosition::Above)
                                                                                                        .Range(0, 100).Value(50).Indent(26));
                                                                                                });

                                                                                            AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                                                .Icon(RetrieveIcon(L"bold")).IconSize(24, 24), [&]()
                                                                                                {
                                                                                                    // ── Language selector ──
                                                                                                    auto& langCombo = AddComboBox(
                                                                                                        Props().Label(Tr(L"settings.language"), ElementPosition::Above).Indent(26)
                                                                                                        .Items({ Tr(L"lang.en"), Tr(L"lang.ru") })
                                                                                                        .Bind(&m_langIdx));

                                                                                                    langCombo.SetOnSelectionChanged([this]() {
                                                                                                        const wchar_t* codes[] = { L"en", L"ru" };
                                                                                                        if (m_langIdx >= 0 && m_langIdx < 2)
                                                                                                            SetLanguage(codes[m_langIdx]);
                                                                                                        });

                                                                                                    AddComboBox(Props().Label(Tr(L"settings.theme"), ElementPosition::Above).Indent(26)
                                                                                                        .Items({ Tr(L"theme.light"), Tr(L"theme.dark"),
                                                                                                                 Tr(L"theme.system") }));
                                                                                                });
                                                                                        });
                                                                                });
                                                                        });

                                                                    AddCell(g, 1, 0, [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"page_content")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddTabPageHost(*m_detachedHoriz);
                                                                                });
                                                                        });
                                                                });
                                                        });

                                                    // ──────────────────────────────────────────────────
                                                    //  PAGE 6 — Detached Vertical Sidebar
                                                    // ──────────────────────────────────────────────────
                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                        .Icon(RetrieveIcon(L"bold")).IconSize(34, 34), [&]()
                                                        {
                                                            AddGrid(Props().Margin(2, 8, 2, 2), [&](GridLayout& g)
                                                                {
                                                                    g.AddRow(GridSize::Star());
                                                                    g.AddColumn(GridSize::Fixed(140));
                                                                    g.AddColumn(GridSize::Star());

                                                                    AddCell(g, 0, 0, [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"menu")).Margin(2).Padding(4), [&]()
                                                                                {
                                                                                    m_detachedVert = &AddTabControl(TabOrientation::Vertical,
                                                                                        Props().Margin(2)
                                                                                        .TabAlign(TabAlignment::AlignLeft)
                                                                                        .StripSize(240)
                                                                                        .Detached()
                                                                                        .Transition(TabTransition::SlideVertical).Height(AutoSize::Fill), [&]()
                                                                                        {
                                                                                            AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                                                .Icon(RetrieveIcon(L"bold")).IconSize(24, 24), [&]()
                                                                                                {
                                                                                                    AddCheckbox(Props().Text(Tr(L"misc.radar"))
                                                                                                        .Label(ElementPosition::Left).Bind(&m_radar));
                                                                                                    AddSlider(Props().Label(Tr(L"misc.zoom"), ElementPosition::Above)
                                                                                                        .Range(0.5f, 5.f).Value(1.f).Indent(26));
                                                                                                });

                                                                                            AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                                                .Icon(RetrieveIcon(L"bold")).IconSize(24, 24), [&]()
                                                                                                {
                                                                                                    AddCheckbox(Props().Text(Tr(L"crosshair"))
                                                                                                        .Label(ElementPosition::Left).Bind(&m_crosshair)
                                                                                                        .ColorSwatch(&m_crosshairColor));
                                                                                                    AddSlider(Props().Label(Tr(L"crosshair.gap"), ElementPosition::Above)
                                                                                                        .Range(0, 20).Value(4).Indent(26));
                                                                                                    AddSlider(Props().Label(Tr(L"crosshair.thickness"), ElementPosition::Above)
                                                                                                        .Range(1, 6).Value(2).Indent(26));
                                                                                                });

                                                                                            AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                                                .Icon(RetrieveIcon(L"bold")).IconSize(24, 24), [&]()
                                                                                                {
                                                                                                    AddTextInput(Props().Label(Tr(L"console"), ElementPosition::Above)
                                                                                                        .Placeholder(Tr(L"console.hint")).Multiline());
                                                                                                    AddButton(Props().Text(Tr(L"btn.execute")));
                                                                                                });
                                                                                        });
                                                                                });
                                                                        });

                                                                    AddCell(g, 0, 1, [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"details")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddTabPageHost(*m_detachedVert);
                                                                                });
                                                                        });
                                                                });
                                                        });

                                                    // ──────────────────────────────────────────────────
                                                    //  PAGE 7 — StripSize Uniformity Demo
                                                    // ──────────────────────────────────────────────────
                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                        .Icon(RetrieveIcon(L"bold")).IconSize(34, 34), [&]()
                                                        {
                                                            AddLabel(Tr(L"demo.strip_fixed"));

                                                            AddTabControl(TabOrientation::Horizontal,
                                                                Props().Margin(4)
                                                                .TabAlign(TabAlignment::AlignLeft)
                                                                .StripSize(55)
                                                                .Transition(TabTransition::SlideVertical), [&]()
                                                                {
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 23), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"demo.small_icon")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddCheckbox(Props().Text(Tr(L"demo.option_a"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_optA));
                                                                                    AddCheckbox(Props().Text(Tr(L"demo.option_b"))
                                                                                        .Label(ElementPosition::Left).Bind(&m_optB));
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(110, 45), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"demo.large_icon")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddSlider(Props().Label(L"X", ElementPosition::Above)
                                                                                        .Range(0, 100).Value(50).Indent(26));
                                                                                    AddSlider(Props().Label(L"Y", ElementPosition::Above)
                                                                                        .Range(0, 100).Value(50).Indent(26));
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(80, 26), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"demo.medium_icon")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddTextInput(Props().Label(Tr(L"demo.notes"), ElementPosition::Above)
                                                                                        .Placeholder(L"...").Indent(26));
                                                                                });
                                                                        });

                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(60, 35), [&]()
                                                                        {
                                                                            AddGroupBox(Props().Title(Tr(L"demo.listbox")).Margin(2).Padding(10), [&]()
                                                                                {
                                                                                    AddListBox(Props()
                                                                                        .Items({ Tr(L"demo.entry") + L" 1", Tr(L"demo.entry") + L" 2",
                                                                                                 Tr(L"demo.entry") + L" 3", Tr(L"demo.entry") + L" 4",
                                                                                                 Tr(L"demo.entry") + L" 5" })
                                                                                        .MaxVisible(4));
                                                                                });
                                                                        });
                                                                });

                                                            AddLabel(Tr(L"demo.strip_auto"));

                                                            AddTabControl(TabOrientation::Horizontal,
                                                                Props().Margin(4)
                                                                .TabAlign(TabAlignment::AlignLeft)
                                                                .Transition(TabTransition::Fade), [&]()
                                                                {
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 23), [&]() {
                                                                            AddLabel(Tr(L"demo.short_matches"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(110, 45), [&]() {
                                                                            AddLabel(Tr(L"demo.tallest_sets"));
                                                                        });
                                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                                        .Icon(RetrieveIcon(L"bold")).IconSize(70, 26), [&]() {
                                                                            AddLabel(Tr(L"demo.medium_matches"));
                                                                        });
                                                                });
                                                        });

                                                    // ──────────────────────────────────────────────────
                                                    //  PAGE 8 — Console
                                                    // ──────────────────────────────────────────────────
                                                    AddTabPage(L"", Props().Padding(10).Scrollable()
                                                        .Icon(RetrieveIcon(L"bold")).IconSize(34, 34), [&]()
                                                        {
                                                            AddGroupBox(Props().Title(Tr(L"console")).Margin(2).Padding(10), [&]()
                                                                {
                                                                    AddTextInput(Props().Label(Tr(L"console.command"), ElementPosition::Above)
                                                                        .Placeholder(Tr(L"console.command.hint")).Multiline());
                                                                    AddButton(Props().Text(Tr(L"btn.execute")));
                                                                    AddButton(Props().Text(Tr(L"btn.clear")));
                                                                });
                                                        });
                                                    // ──────────────────────────────────────────────────
                            //  PAGE — Theme Colors
                            // ──────────────────────────────────────────────────
                                                    AddTabPage(L"Theme", Props().Padding(10).Scrollable(), [&]()
                                                        {
                                                            UIStyle& s = GetStyle();

                                                            AddTabControl(TabOrientation::Horizontal,
                                                                Props().Margin(4)
                                                                .TabAlign(TabAlignment::AlignLeft)
                                                                .StripSize(32)
                                                                .Transition(TabTransition::SlideHorizontal),
                                                                [&]()
                                                                {
                                                                    // ── General ──
                                                                    AddTabPage(L"General", Props().Padding(10).Scrollable(), [&]()
                                                                        {
                                                                            AddGrid(Props().Margin(2), [&](GridLayout& grid)
                                                                                {
                                                                                    grid.AddRow(GridSize::Star());
                                                                                    grid.AddColumn(GridSize::Star());
                                                                                    grid.AddColumn(GridSize::Star());

                                                                                    AddCell(grid, 0, 0, [&]()
                                                                                        {
                                                                                            AddGroupBox(Props().Title(L"Window").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.windowBackground, Props().Label(L"Background", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.titleBarBackground, Props().Label(L"Title Bar", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.titleBarText, Props().Label(L"Title Text", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.shadowColor, Props().Label(L"Shadow", ElementPosition::Left));
                                                                                                });

                                                                                            AddGroupBox(Props().Title(L"Accent").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.accent, Props().Label(L"Primary", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.accentHover, Props().Label(L"Hover", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.accentPressed, Props().Label(L"Pressed", ElementPosition::Left));
                                                                                                });
                                                                                        });

                                                                                    AddCell(grid, 0, 1, [&]()
                                                                                        {
                                                                                            AddGroupBox(Props().Title(L"Label").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.labelText, Props().Label(L"Text", ElementPosition::Left));
                                                                                                });

                                                                                            AddGroupBox(Props().Title(L"Separator").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.separatorColor, Props().Label(L"Color", ElementPosition::Left));
                                                                                                });

                                                                                            AddGroupBox(Props().Title(L"Scrollbar").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.scrollbarTrack, Props().Label(L"Track", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.scrollbarThumb, Props().Label(L"Thumb", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.scrollbarThumbHover, Props().Label(L"Thumb Hovered", ElementPosition::Left));
                                                                                                });
                                                                                        });
                                                                                });
                                                                        });

                                                                    // ── Controls ──
                                                                    AddTabPage(L"Controls", Props().Padding(10).Scrollable(), [&]()
                                                                        {
                                                                            AddGrid(Props().Margin(2), [&](GridLayout& grid)
                                                                                {
                                                                                    grid.AddRow(GridSize::Star());
                                                                                    grid.AddColumn(GridSize::Star());
                                                                                    grid.AddColumn(GridSize::Star());

                                                                                    AddCell(grid, 0, 0, [&]()
                                                                                        {
                                                                                            AddGroupBox(Props().Title(L"Button").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.buttonNormal, Props().Label(L"Normal", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.buttonHovered, Props().Label(L"Hovered", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.buttonPressed, Props().Label(L"Pressed", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.buttonText, Props().Label(L"Text", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.buttonBorder, Props().Label(L"Border", ElementPosition::Left));
                                                                                                });

                                                                                            AddGroupBox(Props().Title(L"Checkbox").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.checkboxBoxNormal, Props().Label(L"Normal", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.checkboxBoxChecked, Props().Label(L"Checked", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.checkboxCheckMark, Props().Label(L"Check Mark", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.checkboxText, Props().Label(L"Text", ElementPosition::Left));
                                                                                                });

                                                                                            AddGroupBox(Props().Title(L"Slider").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.sliderTrack, Props().Label(L"Track", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.sliderFill, Props().Label(L"Fill", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.sliderThumb, Props().Label(L"Thumb", ElementPosition::Left));
                                                                                                });
                                                                                        });

                                                                                    AddCell(grid, 0, 1, [&]()
                                                                                        {
                                                                                            AddGroupBox(Props().Title(L"KeyBind").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.keybindBackground, Props().Label(L"Background", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.keybindHovered, Props().Label(L"Hovered", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.keybindBorder, Props().Label(L"Border", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.keybindText, Props().Label(L"Text", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.keybindTextListening, Props().Label(L"Listening Text", ElementPosition::Left));
                                                                                                });

                                                                                            AddGroupBox(Props().Title(L"Text Input").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.inputBackground, Props().Label(L"Background", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.inputBorder, Props().Label(L"Border", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.inputBorderFocused, Props().Label(L"Border Focused", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.inputText, Props().Label(L"Text", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.inputPlaceholder, Props().Label(L"Placeholder", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.inputSelection, Props().Label(L"Selection", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.inputCursor, Props().Label(L"Cursor", ElementPosition::Left));
                                                                                                });
                                                                                        });
                                                                                });
                                                                        });

                                                                    // ── Containers ──
                                                                    AddTabPage(L"Containers", Props().Padding(10).Scrollable(), [&]()
                                                                        {
                                                                            AddGrid(Props().Margin(2), [&](GridLayout& grid)
                                                                                {
                                                                                    grid.AddRow(GridSize::Star());
                                                                                    grid.AddColumn(GridSize::Star());
                                                                                    grid.AddColumn(GridSize::Star());

                                                                                    AddCell(grid, 0, 0, [&]()
                                                                                        {
                                                                                            AddGroupBox(Props().Title(L"GroupBox").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.groupboxTitleBackground, Props().Label(L"Title Background", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.groupboxTitleText, Props().Label(L"Title Text", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.groupboxTitleOutline, Props().Label(L"Title Outline", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.groupboxBodyBackground, Props().Label(L"Body Background", ElementPosition::Left));
                                                                                                });

                                                                                            AddGroupBox(Props().Title(L"Tabs").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.tabControlBackground, Props().Label(L"Control Background", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.tabNormalBackground, Props().Label(L"Normal", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.tabSelectedBackground, Props().Label(L"Selected", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.tabHoveredBackground, Props().Label(L"Hovered", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.tabText, Props().Label(L"Text", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.tabSelectedText, Props().Label(L"Selected Text", ElementPosition::Left));
                                                                                                });
                                                                                        });

                                                                                    AddCell(grid, 0, 1, [&]()
                                                                                        {
                                                                                            AddGroupBox(Props().Title(L"ComboBox").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.comboBackground, Props().Label(L"Background", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboHovered, Props().Label(L"Hovered", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboBorder, Props().Label(L"Border", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboText, Props().Label(L"Text", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboArrow, Props().Label(L"Arrow", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboDropBackground, Props().Label(L"Dropdown Background", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboDropBorder, Props().Label(L"Dropdown Border", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboItemHovered, Props().Label(L"Item Hovered", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboItemSelected, Props().Label(L"Item Selected", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboItemText, Props().Label(L"Item Text", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.comboCheckMark, Props().Label(L"Check Mark", ElementPosition::Left));
                                                                                                });

                                                                                            AddGroupBox(Props().Title(L"ListBox").Margin(2).Padding(10), [&]()
                                                                                                {
                                                                                                    AddColorSwatch(&s.listboxBackground, Props().Label(L"Background", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.listboxBorder, Props().Label(L"Border", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.listboxItemHovered, Props().Label(L"Item Hovered", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.listboxItemSelected, Props().Label(L"Item Selected", ElementPosition::Left));
                                                                                                    AddColorSwatch(&s.listboxItemText, Props().Label(L"Item Text", ElementPosition::Left));
                                                                                                });
                                                                                        });
                                                                                });
                                                                        });
                                                                });
                                                        });
                                                    // ──────────────────────────────────────────────────
                                                    //  PAGE — Color Editor
                                                    // ──────────────────────────────────────────────────
                                                    AddTabPage(L"Editor", Props().Padding(10), [&]()
                                                        {
                                                            UIStyle& s = GetStyle();

                                                            // Build the name→pointer mapping
                                                            struct ColorEntry { std::wstring name; Color* color; };
                                                            static std::vector<ColorEntry> colorEntries;
                                                            colorEntries = {
                                                                { L"Window Background",    &s.windowBackground },
                                                                { L"Title Bar",            &s.titleBarBackground },
                                                                { L"Title Text",           &s.titleBarText },
                                                                { L"Accent",               &s.accent },
                                                                { L"Accent Hover",         &s.accentHover },
                                                                { L"Accent Pressed",       &s.accentPressed },
                                                                { L"Button Normal",        &s.buttonNormal },
                                                                { L"Button Hovered",       &s.buttonHovered },
                                                                { L"Button Pressed",       &s.buttonPressed },
                                                                { L"Button Text",          &s.buttonText },
                                                                { L"Button Border",        &s.buttonBorder },
                                                                { L"Checkbox Normal",      &s.checkboxBoxNormal },
                                                                { L"Checkbox Checked",     &s.checkboxBoxChecked },
                                                                { L"Checkbox Mark",        &s.checkboxCheckMark },
                                                                { L"Label Text",           &s.labelText },
                                                                { L"GroupBox Title Bg",    &s.groupboxTitleBackground },
                                                                { L"GroupBox Title Text",  &s.groupboxTitleText },
                                                                { L"GroupBox Body Bg",     &s.groupboxBodyBackground },
                                                                { L"Tab Normal",           &s.tabNormalBackground },
                                                                { L"Tab Selected",         &s.tabSelectedBackground },
                                                                { L"Tab Text",             &s.tabText },
                                                                { L"Tab Selected Text",    &s.tabSelectedText },
                                                                { L"Tab Control Bg",       &s.tabControlBackground },
                                                                { L"Slider Track",         &s.sliderTrack },
                                                                { L"Slider Fill",          &s.sliderFill },
                                                                { L"Slider Thumb",         &s.sliderThumb },
                                                                { L"Input Background",     &s.inputBackground },
                                                                { L"Input Border",         &s.inputBorder },
                                                                { L"Input Border Focused", &s.inputBorderFocused },
                                                                { L"Input Text",           &s.inputText },
                                                                { L"Input Placeholder",    &s.inputPlaceholder },
                                                                { L"Input Cursor",         &s.inputCursor },
                                                                { L"ComboBox Background",  &s.comboBackground },
                                                                { L"ComboBox Border",      &s.comboBorder },
                                                                { L"ComboBox Text",        &s.comboText },
                                                                { L"ComboBox Arrow",       &s.comboArrow },
                                                                { L"KeyBind Background",   &s.keybindBackground },
                                                                { L"KeyBind Border",       &s.keybindBorder },
                                                                { L"KeyBind Text",         &s.keybindText },
                                                                { L"Scrollbar Track",      &s.scrollbarTrack },
                                                                { L"Scrollbar Thumb",      &s.scrollbarThumb },
                                                                { L"ListBox Background",   &s.listboxBackground },
                                                                { L"ListBox Border",       &s.listboxBorder },
                                                            };

                                                            // Build items list for the listbox
                                                            std::vector<std::wstring> colorNames;
                                                            for (auto& entry : colorEntries)
                                                                colorNames.push_back(entry.name);

                                                            AddGrid(Props().Margin(2), [&](GridLayout& grid)
                                                                {
                                                                    grid.AddRow(GridSize::Star());
                                                                    grid.AddColumn(GridSize::Fixed(220));
                                                                    grid.AddColumn(GridSize::Star());

                                                                    AddCell(grid, 0, 0, [&]()
                                                                        {
                                                                            auto& lb = AddListBox(Props().Items(colorNames).MaxVisible(16)
                                                                                .Height(AutoSize::Fill));

                                                                            // When selection changes, retarget the picker
                                                                            lb.SetOnSelectionChanged([this](int index) {
                                                                                if (m_themeColorPicker && index >= 0 &&
                                                                                    index < (int)colorEntries.size())
                                                                                {
                                                                                    m_themeColorPicker->SetLinkedColor(colorEntries[index].color);
                                                                                }
                                                                                });

                                                                            // Select first item by default
                                                                            lb.SetSelectedIndex(0);
                                                                        });

                                                                    AddCell(grid, 0, 1, [&]()
                                                                        {
                                                                            auto& picker = AddColorPicker(
                                                                                Props().Width(AutoSize::Fill)
                                                                                .Height(AutoSize::Fill)
                                                                                .HueBar(HueBarPosition::Right).ShowAlpha());

                                                                            m_themeColorPicker = &picker;

                                                                            // Start linked to the first entry
                                                                            if (!colorEntries.empty())
                                                                                picker.SetLinkedColor(colorEntries[0].color);
                                                                        });
                                                                });
                                                        });
                                                });
                                            });



                                        AddTabPage(L"Edit", [&]() {});
                                        AddTabPage(L"View", [&]() {});
                                    });
                            });
                    });
            },
            [&]() {
                AddTabPageHost(*m_titleBarTabs, Props());

            });
    }

public:
    TabControl* m_titleBarTabs = nullptr;
    ColorPicker* m_themeColorPicker = nullptr;

    // ── Bindings ──
    bool m_enable = false;
    bool m_overlay = true;
    bool m_autoSave = true;
    float m_opacity = 75.f;
    int m_quality = 2;
    std::wstring m_username;
    std::wstring m_address;
    int m_toggleKey = 0;
    int m_actionKey = 0;
    Color m_overlayColor{ 255, 100, 50, 255 };
    Color m_accentColor{ 50, 150, 255, 255 };

    bool m_aimEnable = false;
    float m_aimFov = 90.f;
    bool m_triggerActive = false;
    bool m_rcs = false;
    bool m_autoScope = false;

    bool m_espBox = false;
    bool m_espName = false;
    bool m_espHealth = true;
    bool m_chams = false;
    Color m_espBoxColor{ 255, 50, 50, 255 };
    Color m_espNameColor{ 255, 255, 255, 255 };
    Color m_chamsColor{ 100, 200, 255, 255 };

    int m_configIdx = 0;
    std::wstring m_configName;
    bool m_fullscreen = false;
    bool m_vsync = true;

    bool m_walls = false;
    bool m_glow = false;
    bool m_trail = false;
    Color m_glowColor{ 255, 200, 0, 255 };
    Color m_trailColor{ 0, 255, 150, 255 };
    int m_langIdx = 0;  // 0 = English, 1 = Russian

    bool m_radar = false;
    bool m_crosshair = true;
    Color m_crosshairColor{ 0, 255, 0, 255 };

    bool m_optA = false;
    bool m_optB = true;

    TabControl* m_detachedHoriz = nullptr;
    TabControl* m_detachedVert = nullptr;
};