#pragma once
#include "../../../include/util/localization.h"

// Russian Language Pack

inline constexpr LocalizedString kStrings_ru[] = {
    // window
    { L"window.title",              L"Пример ChameleonUI" },

    // tab pages
    { L"page.controls",             L"Элементы" },
    { L"page.layouts",              L"Макеты" },
    { L"page.advanced",             L"Продвинутые" },
    { L"page.config",               L"Конфиг" },

    // group box titles
    { L"group.checkbox",            L"Чекбокс" },
    { L"group.slider",              L"Слайдер" },
    { L"group.numeric_input",       L"Числовой ввод" },
    { L"group.button",              L"Кнопка" },
    { L"group.combobox",            L"Выпадающий список" },
    { L"group.text_input",          L"Текстовый ввод" },
    { L"group.keybind",             L"Привязка клавиш" },
    { L"group.listbox",             L"Список" },
    { L"group.color",               L"Цвет" },
    { L"group.vertical_tab",        L"Вертикальный TabControl" },
    { L"group.grid_layout",         L"GridLayout (Star + Fixed колонки)" },
    { L"group.scrollable_panel",    L"Прокручиваемая панель" },
    { L"group.color_picker",        L"Встроенный ColorPicker" },
    { L"group.content_switcher",    L"ContentSwitcher (управляется ListBox)" },
    { L"group.skin_override",       L"Переопределение скина" },
    { L"group.multiselect_list",    L"Список с мультивыбором" },
    { L"group.config_profiles",     L"Профили конфигураций" },
    { L"group.cat_image",           L"Изображение кота" },
    { L"group.language",            L"Язык" },

    // checkboxes
    { L"checkbox.enable_feature",   L"Включить функцию" },
    { L"checkbox.fullscreen",       L"Полный экран" },
    { L"checkbox.vsync",            L"Верт. синхронизация" },
    { L"checkbox.option_tab_a",     L"Опция внутри Tab A" },
    { L"checkbox.aero_styled",      L"Чекбокс в стиле Aero" },

    // sliders
    { L"slider.speed",              L"Скорость" },
    { L"slider.volume",             L"Громкость" },
    { L"slider.value",              L"Значение" },

    // numeric inputs
    { L"numeric.brightness",        L"Яркость" },
    { L"numeric.fov",               L"Поле зрения" },

    // buttons
    { L"btn.click_me",              L"Нажми меня" },
    { L"btn.styled",                L"Стилизованная кнопка" },
    { L"btn.fixed_120",             L"Фикс. 120px" },
    { L"btn.aero",                  L"Кнопка в стиле Aero" },

    // combo boxes
    { L"combobox.quality",          L"Качество" },
    { L"quality.low",               L"Низкое" },
    { L"quality.medium",            L"Среднее" },
    { L"quality.high",              L"Высокое" },
    { L"quality.ultra",             L"Ультра" },
    { L"combobox.multiselect",      L"Мультивыбор" },
    { L"option.a",                  L"Опция A" },
    { L"option.b",                  L"Опция B" },
    { L"option.c",                  L"Опция C" },
    { L"option.d",                  L"Опция D" },

    // text inputs
    { L"input.name",                L"Имя" },
    { L"input.name_placeholder",    L"Введите ваше имя..." },
    { L"input.notes",               L"Заметки" },
    { L"input.notes_placeholder",   L"Многострочный текст..." },

    // keybind
    { L"keybind.hotkey",            L"Горячая клавиша" },

    // list items
    { L"list.item_1",               L"Элемент 1" },
    { L"list.item_2",               L"Элемент 2" },
    { L"list.item_3",               L"Элемент 3" },
    { L"list.item_4",               L"Элемент 4" },
    { L"list.item_5",               L"Элемент 5" },

    // color
    { L"color.highlight",           L"Подсветка" },

    // vertical tab control
    { L"tab.a",                     L"Вкладка A" },
    { L"tab.a_content",             L"Это вкладка A внутри вертикального TabControl." },
    { L"tab.b",                     L"Вкладка B" },
    { L"tab.b_content",             L"Это вкладка B со слайдером." },
    { L"tab.c",                     L"Вкладка C" },
    { L"tab.c_content",             L"Вкладка C \u2014 демонстрация перехода затуханием." },

    // grid layout
    { L"label.stretch",             L"Эта колонка растягивается, заполняя оставшееся место." },

    // scrollable
    { L"label.scrollable_item",     L"Прокручиваемый элемент #" },
    { L"label.separator",           L"Строка выше — горизонтальный разделитель." },

    // content switcher
    { L"page.1",                    L"Страница 1" },
    { L"page.2",                    L"Страница 2" },
    { L"page.3",                    L"Страница 3" },
    { L"page.1_content",            L"Содержимое страницы 1 \u2014 здесь могут быть любые виджеты." },
    { L"page.2_content",            L"Содержимое страницы 2 \u2014 управляется ListBox слева." },
    { L"page.3_content",            L"Содержимое страницы 3 \u2014 переключение без накладных расходов." },

    // skin override
    { L"label.skin_info",           L"Кнопка ниже использует FrutigerAeroSkin, а всё остальное — DefaultSkin:" },

    // multi-select listbox
    { L"list.alpha",                L"Альфа" },
    { L"list.bravo",                L"Браво" },
    { L"list.charlie",              L"Чарли" },
    { L"list.delta",                L"Дельта" },
    { L"list.echo",                 L"Эхо" },

    // language
    { L"language.label",            L"Язык" },
    { L"language.english",          L"English" },
    { L"language.russian",          L"Русский" },
    { L"language.info",             L"Переключение языка перестраивает всё дерево UI, сохраняя позиции окон, выбранные вкладки и все привязанные данные." },

    // tooltips
    { L"tooltip.feature",           L"Включает/выключает основную функцию" },
    { L"tooltip.reset_speed",       L"Сбрасывает слайдер скорости на 50" },
    { L"tooltip.keybind",           L"Нажмите, затем нажмите любую клавишу для привязки" },

    // config (preserved)
    { L"config.name_hint",          L"Имя конфига..." },
    { L"config.btn_new",            L"Новый конфиг" },
    { L"config.btn_save",           L"Сохранить" },
    { L"config.btn_load",           L"Загрузить" },
    { L"config.btn_delete",         L"Удалить" },
    { L"config.btn_reset",          L"Сбросить" },
};

inline constexpr size_t kStrings_ru_Count = sizeof(kStrings_ru) / sizeof(kStrings_ru[0]);
