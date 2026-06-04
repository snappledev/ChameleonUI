#pragma once
#include "../../../include/util/localization.h"

// English Language Pack

inline constexpr LocalizedString kStrings_en[] = {
    // window
    { L"window.title",              L"ChameleonUI Example" },

    // tab pages
    { L"page.controls",             L"Controls" },
    { L"page.layouts",              L"Layouts" },
    { L"page.advanced",             L"Advanced" },
    { L"page.config",               L"Config" },

    // group box titles
    { L"group.checkbox",            L"Checkbox" },
    { L"group.slider",              L"Slider" },
    { L"group.numeric_input",       L"NumericInput" },
    { L"group.button",              L"Button" },
    { L"group.combobox",            L"ComboBox" },
    { L"group.text_input",          L"TextInput" },
    { L"group.keybind",             L"KeyBind" },
    { L"group.listbox",             L"ListBox" },
    { L"group.color",               L"Color" },
    { L"group.vertical_tab",        L"Vertical TabControl" },
    { L"group.grid_layout",         L"GridLayout (Star + Fixed columns)" },
    { L"group.scrollable_panel",    L"Scrollable Panel" },
    { L"group.color_picker",        L"Inline ColorPicker" },
    { L"group.content_switcher",    L"ContentSwitcher (driven by ListBox)" },
    { L"group.skin_override",       L"Per-Widget Skin Override" },
    { L"group.multiselect_list",    L"Multi-Select ListBox" },
    { L"group.config_profiles",     L"Configuration Profiles" },
    { L"group.cat_image",           L"Cat Image" },
    { L"group.language",            L"Language" },

    // checkboxes
    { L"checkbox.enable_feature",   L"Enable feature" },
    { L"checkbox.fullscreen",       L"Fullscreen" },
    { L"checkbox.vsync",            L"V-Sync" },
    { L"checkbox.option_tab_a",     L"Option inside Tab A" },
    { L"checkbox.aero_styled",      L"Aero-styled Checkbox" },

    // sliders
    { L"slider.speed",              L"Speed" },
    { L"slider.volume",             L"Volume" },
    { L"slider.value",              L"Value" },

    // numeric inputs
    { L"numeric.brightness",        L"Brightness" },
    { L"numeric.fov",               L"FOV" },

    // buttons
    { L"btn.click_me",              L"Click Me" },
    { L"btn.styled",                L"Styled Button" },
    { L"btn.fixed_120",             L"Fixed 120px" },
    { L"btn.aero",                  L"Aero-styled Button" },

    // combo boxes
    { L"combobox.quality",          L"Quality" },
    { L"quality.low",               L"Low" },
    { L"quality.medium",            L"Medium" },
    { L"quality.high",              L"High" },
    { L"quality.ultra",             L"Ultra" },
    { L"combobox.multiselect",      L"Multi-Select" },
    { L"option.a",                  L"Option A" },
    { L"option.b",                  L"Option B" },
    { L"option.c",                  L"Option C" },
    { L"option.d",                  L"Option D" },

    // text inputs
    { L"input.name",                L"Name" },
    { L"input.name_placeholder",    L"Enter your name..." },
    { L"input.notes",               L"Notes" },
    { L"input.notes_placeholder",   L"Multi-line text area..." },

    // keybind
    { L"keybind.hotkey",            L"Hotkey" },

    // list items
    { L"list.item_1",               L"Item 1" },
    { L"list.item_2",               L"Item 2" },
    { L"list.item_3",               L"Item 3" },
    { L"list.item_4",               L"Item 4" },
    { L"list.item_5",               L"Item 5" },

    // color
    { L"color.highlight",           L"Highlight" },

    // vertical tab control
    { L"tab.a",                     L"Tab A" },
    { L"tab.a_content",             L"This is Tab A inside a vertical TabControl." },
    { L"tab.b",                     L"Tab B" },
    { L"tab.b_content",             L"This is Tab B with a slider." },
    { L"tab.c",                     L"Tab C" },
    { L"tab.c_content",             L"Tab C \u2014 demonstrates fade transition." },

    // grid layout
    { L"label.stretch",             L"This column stretches to fill remaining space." },

    // scrollable
    { L"label.scrollable_item",     L"Scrollable item #" },
    { L"label.separator",           L"The line above is a horizontal Separator." },

    // content switcher
    { L"page.1",                    L"Page 1" },
    { L"page.2",                    L"Page 2" },
    { L"page.3",                    L"Page 3" },
    { L"page.1_content",            L"Content for Page 1 \u2014 any widgets can go here." },
    { L"page.2_content",            L"Content for Page 2 \u2014 driven by the ListBox on the left." },
    { L"page.3_content",            L"Content for Page 3 \u2014 zero-overhead switching." },

    // skin override
    { L"label.skin_info",           L"The button below uses FrutigerAeroSkin while everything else uses DefaultSkin:" },

    // multi-select listbox
    { L"list.alpha",                L"Alpha" },
    { L"list.bravo",                L"Bravo" },
    { L"list.charlie",              L"Charlie" },
    { L"list.delta",                L"Delta" },
    { L"list.echo",                 L"Echo" },

    // language
    { L"language.label",            L"Language" },
    { L"language.english",          L"English" },
    { L"language.russian",          L"Russian" },
    { L"language.info",             L"Language switching rebuilds the entire UI tree while preserving window positions, tab selections, and all bound data." },

    // tooltips
    { L"tooltip.feature",           L"Toggles the main feature on/off" },
    { L"tooltip.reset_speed",       L"Resets the speed slider to 50" },
    { L"tooltip.keybind",           L"Click then press any key to bind" },

    // config (preserved)
    { L"config.name_hint",          L"Config name..." },
    { L"config.btn_new",            L"New Config" },
    { L"config.btn_save",           L"Save" },
    { L"config.btn_load",           L"Load" },
    { L"config.btn_delete",         L"Delete" },
    { L"config.btn_reset",          L"Reset Defaults" },
};

inline constexpr size_t kStrings_en_Count = sizeof(kStrings_en) / sizeof(kStrings_en[0]);
