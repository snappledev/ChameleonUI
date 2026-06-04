#pragma once
#include "../include/core/ChameleonUI.h"
#include "../include/skins/DefaultSkin.h"
#include "../include/skins/FrutigerAeroSkin.h"
#include "../include/assets/icons/cat.h"
#include "../include/assets/icons/feathericons.h"

#pragma region example ui - full feature showcase
// a complete demo covering every widget, layout, and feature.
// copy this file, then strip out what you don't need.

class ExampleUI : public ChameleonUI
{
public:
    // pick which skin to render with
    std::unique_ptr<Skin> CreateSkin() override
    {
        // to use the aero skin instead, swap the return:
        // return std::make_unique<FrutigerAeroSkin>();
        return std::make_unique<DefaultSkin>();
    }

protected:
    // config files live in %appdata%/exampleapp/
    std::wstring GetConfigAppName() const override { return L"ExampleApp"; }
    std::wstring GetDefaultConfigName() const override { return L"default"; }

    // bind variables so their values survive restarts
    void RegisterBindings() override
    {
        auto& c = GetConfig();

        // general
        c.Bind(L"general.enabled", &m_enabled);
        c.Bind(L"general.speed", &m_speed);
        c.Bind(L"general.volume", &m_volume);
        c.Bind(L"general.quality", &m_quality);
        c.Bind(L"general.name", &m_name);
        c.Bind(L"general.accent", &m_accentColor);

        // input
        c.Bind(L"input.hotkey", &m_hotkey);
        c.Bind(L"input.brightness", &m_brightness);

        // display
        c.Bind(L"display.fullscreen", &m_fullscreen);
        c.Bind(L"display.vsync", &m_vsync);
        c.Bind(L"display.fov", &m_fov);
        c.Bind(L"display.highlight", &m_highlightColor);

        // language
        c.Bind(L"lang.index", &m_langIdx);
    }

    // build the ui tree — called once, then again on language switch
    void BuildUI() override
    {
        // add a second skin so we can demo per-widget overrides below
        m_altSkin = AddSkin<FrutigerAeroSkin>();

        // register icon atlases
        RegisterIcons(kcatIcons_AtlasData, kcatIcons_AtlasDataSize, kcatIcons, kcatIconsCount); //Cat image
        RegisterIcons(kfeathericonsIcons_AtlasData, kfeathericonsIcons_AtlasDataSize, kfeathericonsIcons, kfeathericonsIconsCount); //Feather icons (stock)

        AddWindow(Tr(L"window.title"), 80, 40, 820, 640,
            Props().Padding(8),
            [&]()
            {
                AddTabControl(TabOrientation::Horizontal,
                    Props().StripSize(32)
                    .TabAlign(TabAlignment::AlignLeft)
                    .Transition(TabTransition::SlideHorizontal),
                    [&]()
                    {
                        BuildControlsPage();
                        BuildLayoutsPage();
                        BuildAdvancedPage();
                        BuildConfigPage();
                    });
            });
    }

private:
#pragma region page 1: controls

    void BuildControlsPage()
    {
        AddTabPage(Tr(L"page.controls"), Props().Padding(8).Scrollable()
            .Icon(RetrieveIcon(L"sliders")).IconSize(24, 24), [&]()
            {
                AddGrid(Props(), [&](GridLayout& grid)
                    {
                        grid.AddRow(GridSize::Star());
                        grid.AddColumn(GridSize::Star());
                        grid.AddColumn(GridSize::Star());

                        // left column
                        AddCell(grid, 0, 0, [&]()
                            {
                                // checkbox
                                AddGroupBox(Props().Title(Tr(L"group.checkbox")).Padding(8).Margin(2), [&]()
                                    {
                                        AddCheckbox(Props().Text(Tr(L"checkbox.enable_feature"))
                                            .Label(ElementPosition::Left)
                                            .Bind(&m_enabled)
                                            .Tooltip(Tr(L"tooltip.feature")));

                                        AddCheckbox(Props().Text(Tr(L"checkbox.fullscreen"))
                                            .Label(ElementPosition::Left)
                                            .Bind(&m_fullscreen)
                                            .ColorSwatch(&m_accentColor));

                                        AddCheckbox(Props().Text(Tr(L"checkbox.vsync"))
                                            .Label(ElementPosition::Left)
                                            .Bind(&m_vsync));
                                    });

                                // slider
                                AddGroupBox(Props().Title(Tr(L"group.slider")).Padding(8).Margin(2), [&]()
                                    {
                                        AddSlider(Props().Label(Tr(L"slider.speed"), ElementPosition::Above)
                                            .Range(0, 100).Bind(&m_speed).Indent(26));

                                        AddSlider(Props().Label(Tr(L"slider.volume"), ElementPosition::Above)
                                            .Range(0.0f, 1.0f).Value(0.75f).Bind(&m_volume).Indent(26));
                                    });

                                // numeric input
                                AddGroupBox(Props().Title(Tr(L"group.numeric_input")).Padding(8).Margin(2), [&]()
                                    {
                                        AddNumericInput(Props().Label(Tr(L"numeric.brightness"), ElementPosition::Above)
                                            .Range(0, 255).Step(1).IntegerMode()
                                            .Bind(&m_brightness).Indent(26));

                                        AddNumericInput(Props().Label(Tr(L"numeric.fov"), ElementPosition::Above)
                                            .Range(60.0f, 120.0f).Step(0.5f).Precision(1)
                                            .Bind(&m_fov).Indent(26));
                                    });

                                // button
                                AddGroupBox(Props().Title(Tr(L"group.button")).Padding(8).Margin(2), [&]()
                                    {
                                        AddButton(Props().Text(Tr(L"btn.click_me"))
                                            .Tooltip(Tr(L"tooltip.reset_speed")))
                                            .SetOnClick([this]() { m_speed = 50.f; });

                                        AddButton(Props().Text(Tr(L"btn.styled"))
                                            .UseSkin(m_altSkin));
                                    });
                            });

                        // right column
                        AddCell(grid, 0, 1, [&]()
                            {
                                // combobox
                                AddGroupBox(Props().Title(Tr(L"group.combobox")).Padding(8).Margin(2), [&]()
                                    {
                                        AddComboBox(Props().Label(Tr(L"combobox.quality"), ElementPosition::Above)
                                            .Items({ Tr(L"quality.low"), Tr(L"quality.medium"), Tr(L"quality.high"), Tr(L"quality.ultra") })
                                            .Bind(&m_quality).Indent(26));

                                        AddComboBox(Props().Label(Tr(L"combobox.multiselect"), ElementPosition::Above)
                                            .Items({ Tr(L"option.a"), Tr(L"option.b"), Tr(L"option.c"), Tr(L"option.d") })
                                            .ComboMode(ComboBoxMode::MultiSelect).Indent(26));
                                    });

                                // text input
                                AddGroupBox(Props().Title(Tr(L"group.text_input")).Padding(8).Margin(2), [&]()
                                    {
                                        AddTextInput(Props().Label(Tr(L"input.name"), ElementPosition::Above)
                                            .Placeholder(Tr(L"input.name_placeholder"))
                                            .Bind(&m_name).Indent(26));

                                        AddTextInput(Props().Label(Tr(L"input.notes"), ElementPosition::Above)
                                            .Placeholder(Tr(L"input.notes_placeholder"))
                                            .Multiline().Indent(26)
                                            .Height(AutoSize::Fixed, 80));
                                    });

                                // keybind
                                AddGroupBox(Props().Title(Tr(L"group.keybind")).Padding(8).Margin(2), [&]()
                                    {
                                        AddKeyBind(Props().Label(Tr(L"keybind.hotkey"), ElementPosition::Left)
                                            .Bind(&m_hotkey).Indent(26)
                                            .Tooltip(Tr(L"tooltip.keybind")));
                                    });

                                // listbox
                                AddGroupBox(Props().Title(Tr(L"group.listbox")).Padding(8).Margin(2), [&]()
                                    {
                                        AddListBox(Props()
                                            .Items({ Tr(L"list.item_1"), Tr(L"list.item_2"), Tr(L"list.item_3"),
                                                     Tr(L"list.item_4"), Tr(L"list.item_5") })
                                            .MaxVisible(4));
                                    });

                                // color
                                AddGroupBox(Props().Title(Tr(L"group.color")).Padding(8).Margin(2), [&]()
                                    {
                                        AddColorSwatch(&m_highlightColor,
                                            Props().Label(Tr(L"color.highlight"), ElementPosition::Left));
                                    });
                            });
                    });
            });
    }

#pragma endregion

#pragma region page 2: layouts

    void BuildLayoutsPage()
    {
        AddTabPage(Tr(L"page.layouts"), Props().Padding(8).Scrollable()
            .Icon(RetrieveIcon(L"layout")).IconSize(24, 24), [&]()
            {
                // vertical tabcontrol nested inside a horizontal one
                AddGroupBox(Props().Title(Tr(L"group.vertical_tab")).Padding(8).Margin(2), [&]()
                    {
                        AddTabControl(TabOrientation::Vertical,
                            Props().StripSize(100).Height(AutoSize::Fixed, 250)
                            .TabAlign(TabAlignment::Fill)
                            .Transition(TabTransition::SlideVertical),
                            [&]()
                            {
                                AddTabPage(Tr(L"tab.a"), Props().Padding(8), [&]()
                                    {
                                        AddLabel(Tr(L"tab.a_content"));
                                        AddCheckbox(Props().Text(Tr(L"checkbox.option_tab_a"))
                                            .Label(ElementPosition::Left));
                                    });

                                AddTabPage(Tr(L"tab.b"), Props().Padding(8), [&]()
                                    {
                                        AddLabel(Tr(L"tab.b_content"));
                                        AddSlider(Props().Label(Tr(L"slider.value"), ElementPosition::Above)
                                            .Range(0, 50).Value(25).Indent(26));
                                    });

                                AddTabPage(Tr(L"tab.c"), Props().Padding(8), [&]()
                                    {
                                        AddLabel(Tr(L"tab.c_content"));
                                    });
                            });
                    });

                // grid with star + fixed column mix
                AddGroupBox(Props().Title(Tr(L"group.grid_layout")).Padding(8).Margin(2), [&]()
                    {
                        AddGrid(Props().Height(AutoSize::Fixed, 100), [&](GridLayout& grid)
                            {
                                grid.AddRow(GridSize::Star());
                                grid.AddColumn(GridSize::Fixed(120));
                                grid.AddColumn(GridSize::Star());
                                grid.AddColumn(GridSize::Fixed(120));

                                AddCell(grid, 0, 0, [&]() {
                                    AddButton(Props().Text(Tr(L"btn.fixed_120")));
                                    });
                                AddCell(grid, 0, 1, [&]() {
                                    AddLabel(Tr(L"label.stretch"));
                                    });
                                AddCell(grid, 0, 2, [&]() {
                                    AddButton(Props().Text(Tr(L"btn.fixed_120")));
                                    });
                            });
                    });

                // scrollable groupbox
                AddGroupBox(Props().Title(Tr(L"group.scrollable_panel")).Padding(8).Margin(2)
                    .Height(AutoSize::Fixed, 150).Scrollable(), [&]()
                    {
                        for (int i = 0; i < 12; ++i)
                        {
                            AddLabel(Tr(L"label.scrollable_item") + std::to_wstring(i + 1));
                        }
                    });

                AddSeparator(Props().Margin(0, 8, 0, 8));
                AddLabel(Tr(L"label.separator"));
            });
    }

#pragma endregion

#pragma region page 3: advanced

    void BuildAdvancedPage()
    {
        AddTabPage(Tr(L"page.advanced"), Props().Padding(8).Scrollable()
            .Icon(RetrieveIcon(L"cpu")).IconSize(24, 24), [&]()
            {
                // inline colorpicker with alpha bar
                AddGroupBox(Props().Title(Tr(L"group.color_picker")).Padding(8).Margin(2), [&]()
                    {
                        AddColorPicker(Props()
                            .Width(AutoSize::Fill)
                            .Height(AutoSize::Fixed, 200)
                            .HueBar(HueBarPosition::Right)
                            .ShowAlpha()
                            .Bind(&m_highlightColor));
                    });

                // contentswitcher driven by a listbox on the left
                AddGroupBox(Props().Title(Tr(L"group.content_switcher"))
                    .Padding(8).Margin(2), [&]()
                    {
                        AddGrid(Props().Height(AutoSize::Fixed, 140), [&](GridLayout& grid)
                            {
                                grid.AddRow(GridSize::Star());
                                grid.AddColumn(GridSize::Fixed(140));
                                grid.AddColumn(GridSize::Star());

                                AddCell(grid, 0, 0, [&]()
                                    {
                                        m_switcherList = &AddListBox(Props()
                                            .Items({ Tr(L"page.1"), Tr(L"page.2"), Tr(L"page.3") })
                                            .MaxVisible(3).Height(AutoSize::Fill));
                                        m_switcherList->SetSelectedIndex(0);
                                    });

                                AddCell(grid, 0, 1, [&]()
                                    {
                                        auto& switcher = AddContentSwitcher(
                                            Props().Width(AutoSize::Fill).Height(AutoSize::Fill));

                                        // wire the switcher to track the listbox selection
                                        switcher.SetIndexBinding([this]() {
                                            return m_switcherList ? m_switcherList->GetSelectedIndex() : 0;
                                            });

                                        // each addchild becomes a switchable page
                                        switcher.AddChild<Label>(Tr(L"page.1_content"));
                                        switcher.AddChild<Label>(Tr(L"page.2_content"));
                                        switcher.AddChild<Label>(Tr(L"page.3_content"));
                                    });
                            });
                    });

                // per-widget skin override
                AddGroupBox(Props().Title(Tr(L"group.skin_override")).Padding(8).Margin(2), [&]()
                    {
                        AddLabel(Tr(L"label.skin_info"));
                        AddButton(Props().Text(Tr(L"btn.aero")).UseSkin(m_altSkin));
                        AddCheckbox(Props().Text(Tr(L"checkbox.aero_styled"))
                            .Label(ElementPosition::Left).UseSkin(m_altSkin));
                    });


                // cat image from embedded atlas
                AddGroupBox(Props().Title(Tr(L"group.cat_image")).Padding(8).Margin(2), [&]()
                    {
                        auto catRef = RetrieveIcon(L"cat", 48.f, 48.f);
                        AddImage(Props()
                            .Icon(catRef)
                            .Width(AutoSize::Fixed, 96)
                            .Height(AutoSize::Fixed, 96));
                    });

                // multi-select listbox
                AddGroupBox(Props().Title(Tr(L"group.multiselect_list")).Padding(8).Margin(2), [&]()
                    {
                        AddListBox(Props()
                            .Items({ Tr(L"list.alpha"), Tr(L"list.bravo"), Tr(L"list.charlie"), Tr(L"list.delta"), Tr(L"list.echo") })
                            .ListMode(ListBoxMode::MultiSelect)
                            .MaxVisible(4));
                    });
            });
    }

#pragma endregion

#pragma region page 4: config & localization

    void BuildConfigPage()
    {
        AddTabPage(Tr(L"page.config"), Props().Padding(8)
            .Icon(RetrieveIcon(L"settings")).IconSize(24, 24), [&]()
            {
                AddGrid(Props(), [&](GridLayout& grid)
                    {
                        grid.AddRow(GridSize::Star());
                        grid.AddColumn(GridSize::Star());
                        grid.AddColumn(GridSize::Star());

                        // config profiles
                        AddCell(grid, 0, 0, [&]()
                            {
                                AddGroupBox(Props().Title(Tr(L"group.config_profiles"))
                                    .Padding(8).Margin(2).Height(AutoSize::Fill), [&]()
                                    {
                                        AddConfigSelector(Props().MaxVisible(5));
                                    });
                            });

                        // live language switching
                        AddCell(grid, 0, 1, [&]()
                            {
                                AddGroupBox(Props().Title(Tr(L"group.language"))
                                    .Padding(8).Margin(2).Height(AutoSize::Fill), [&]()
                                    {
                                        auto& langCombo = AddComboBox(
                                            Props().Label(Tr(L"language.label"), ElementPosition::Above)
                                            .Items({ Tr(L"language.english"), Tr(L"language.russian") })
                                            .Bind(&m_langIdx).Indent(26));

                                        langCombo.SetOnSelectionChanged([this]() {
                                            const wchar_t* codes[] = { L"en", L"ru" };
                                            if (m_langIdx >= 0 && m_langIdx < 2)
                                                SetLanguage(codes[m_langIdx]);
                                            });

                                        AddSeparator(Props().Margin(0, 12, 0, 12));

                                        AddLabel(Tr(L"language.info"));
                                    });
                            });
                    });
            });
    }

#pragma endregion

#pragma region bound data
    // all variables bound in registerbindings() appear here

    // general
    bool         m_enabled = false;
    float        m_speed = 50.f;
    float        m_volume = 0.75f;
    int          m_quality = 2;
    std::wstring m_name;
    Color        m_accentColor{ 50, 150, 255, 255 };

    // input
    int          m_hotkey = 0;
    float        m_brightness = 128.f;

    // display
    bool         m_fullscreen = false;
    bool         m_vsync = true;
    float        m_fov = 90.f;
    Color        m_highlightColor{ 0, 255, 0, 255 };

    // misc
    int          m_langIdx = 0;
    bool         m_glassEnabled = true;

    // internal pointers, not serialized
    Skin*    m_altSkin       = nullptr;
    ListBox* m_switcherList  = nullptr;
#pragma endregion
};
