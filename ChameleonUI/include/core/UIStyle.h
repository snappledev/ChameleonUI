#pragma once
#include "../rendering/renderer.h"
#include "animation.h"

struct UIStyle
{
    #pragma region global
    float cornerRadius      = 8.f;
    float glassBlurRadius   = 16.f;
    float glassBorderAlpha  = 0.12f;
    #pragma endregion

    #pragma region animation
    AnimConfig anim;
    #pragma endregion

    #pragma region accent
    Color accent        = Color(100, 160, 255, 255);
    Color accentHover   = Color(130, 180, 255, 255);
    Color accentPressed = Color(70, 130, 220, 255);
    #pragma endregion

    #pragma region window
    Color windowBackground   = Color(18, 18, 24, 220);
    Color titleBarBackground = Color(18, 18, 24, 230);
    Color titleBarText       = Color(230, 235, 245, 255);
    int   titleBarHeight     = 34;
    #pragma endregion

    #pragma region button
    Color buttonNormal     = Color(255, 255, 255, 18);
    Color buttonHovered    = Color(255, 255, 255, 30);
    Color buttonPressed    = Color(255, 255, 255, 10);
    Color buttonText       = Color(230, 235, 245, 255);
    Color buttonBorder     = Color(255, 255, 255, 25);
    float buttonMinHeight  = 30.f;
    #pragma endregion

    #pragma region groupbox
    Color groupboxTitleBackground = Color(255, 255, 255, 10);
    Color groupboxTitleOutline    = Color(255, 255, 255, 20);
    Color groupboxBodyBackground  = Color(255, 255, 255, 6);
    Color groupboxTitleText       = Color(220, 225, 240, 255);
    #pragma endregion

    #pragma region checkbox
    Color checkboxBoxNormal     = Color(255, 255, 255, 18);
    Color checkboxBoxChecked    = Color(100, 160, 255, 255);
    Color checkboxText          = Color(210, 215, 230, 255);
    Color checkboxCheckMark     = Color(255, 255, 255, 255);
    float checkboxBoxSize       = 18.f;
    float checkboxCornerRadius  = 4.f;
    float checkboxLabelSpacing  = 8.f;
    #pragma endregion

    #pragma region label
    Color labelText = Color(200, 210, 230, 200);
    #pragma endregion

    #pragma region tab control
    Color tabNormalBackground   = Color(255, 255, 255, 8);
    Color tabSelectedBackground = Color(100, 160, 255, 40);
    Color tabHoveredBackground  = Color(255, 255, 255, 18);
    Color tabText               = Color(160, 170, 190, 255);
    Color tabSelectedText       = Color(255, 255, 255, 255);
    Color tabControlBackground  = Color(255, 255, 255, 6);
    #pragma endregion

    #pragma region slider
    Color sliderTrack       = Color(255, 255, 255, 15);
    Color sliderFill        = Color(100, 160, 255, 200);
    Color sliderThumb       = Color(255, 255, 255, 240);
    float sliderTrackHeight = 4.f;
    float sliderThumbRadius = 7.f;
    #pragma endregion

    #pragma region combobox
    Color comboBackground        = Color(255, 255, 255, 18);
    Color comboHovered           = Color(255, 255, 255, 28);
    Color comboBorder            = Color(255, 255, 255, 25);
    Color comboText              = Color(230, 235, 245, 255);
    Color comboArrow             = Color(180, 190, 210, 255);
    Color comboDropBackground    = Color(22, 22, 30, 240);
    Color comboDropBorder        = Color(255, 255, 255, 15);
    Color comboItemNormal        = Color(0, 0, 0, 0);
    Color comboItemHovered       = Color(255, 255, 255, 18);
    Color comboItemSelected      = Color(100, 160, 255, 35);
    Color comboItemText          = Color(210, 215, 230, 255);
    Color comboItemTextSelected  = Color(255, 255, 255, 255);
    Color comboCheckMark         = Color(100, 160, 255, 255);
    float comboArrowWidth        = 28.f;
    float comboMinHeight         = 28.f;
    #pragma endregion

    #pragma region text input
    Color inputBackground    = Color(255, 255, 255, 12);
    Color inputHovered       = Color(255, 255, 255, 18);
    Color inputFocused       = Color(255, 255, 255, 22);
    Color inputBorder        = Color(255, 255, 255, 20);
    Color inputBorderFocused = Color(100, 160, 255, 120);
    Color inputText          = Color(230, 235, 245, 255);
    Color inputPlaceholder   = Color(140, 150, 170, 120);
    Color inputSelection     = Color(100, 160, 255, 80);
    Color inputCursor        = Color(100, 160, 255, 255);
    float inputPadX          = 8.f;
    float inputMinHeight     = 28.f;
    #pragma endregion

    #pragma region keybind
    Color keybindBackground    = Color(255, 255, 255, 12);
    Color keybindHovered       = Color(255, 255, 255, 18);
    Color keybindBorder        = Color(255, 255, 255, 20);
    Color keybindListening     = Color(100, 160, 255, 30);
    Color keybindText          = Color(230, 235, 245, 255);
    Color keybindTextListening = Color(100, 160, 255, 255);
    float keybindMinHeight     = 30.f;
    #pragma endregion

    #pragma region scrollbar
    Color scrollbarTrack       = Color(255, 255, 255, 6);
    Color scrollbarThumb       = Color(255, 255, 255, 40);
    Color scrollbarThumbHover  = Color(255, 255, 255, 70);
    int   scrollbarWidth       = 8;
    int   scrollbarMinThumbHeight = 24;
    #pragma endregion

    #pragma region listbox
    Color listboxBackground      = Color(255, 255, 255, 8);
    Color listboxBorder          = Color(255, 255, 255, 15);
    Color listboxItemNormal      = Color(255, 255, 255, 10);
    Color listboxItemHovered     = Color(255, 255, 255, 22);
    Color listboxItemSelected    = Color(100, 160, 255, 60);
    Color listboxItemText        = Color(210, 215, 230, 255);
    Color listboxItemTextSelected = Color(255, 255, 255, 255);
    #pragma endregion

    #pragma region color picker
    Color colorSwatchBorder        = Color(255, 255, 255, 60);
    Color colorSwatchHoverBorder   = Color(100, 160, 255, 180);
    float colorSwatchCornerRadius  = 3.f;
    float colorSwatchDefaultWidth  = 24.f;
    float colorSwatchDefaultHeight = 16.f;

    Color colorPickerBackground    = Color(24, 24, 32, 245);
    Color colorPickerBorder        = Color(255, 255, 255, 25);
    Color colorPickerButtonNormal  = Color(255, 255, 255, 18);
    Color colorPickerButtonHovered = Color(255, 255, 255, 30);
    Color colorPickerButtonText    = Color(230, 235, 245, 255);
    Color colorPickerLabelText     = Color(180, 190, 210, 200);
    float colorPickerCornerRadius  = 8.f;
    float colorPickerWidth         = 300.f;
    float colorPickerHeight        = 260.f;
    #pragma endregion

    #pragma region shadows
    Color shadowColor = Color(0, 0, 0, 60);
    #pragma endregion

    #pragma region font
    std::wstring fontFamily = L"Segoe UI";
    int          fontSize   = 13;
    #pragma endregion

    #pragma region separator
    Color separatorColor = Color(255, 255, 255, 30);
    #pragma endregion

    #pragma region numeric input
    Color numericBackground    = Color(255, 255, 255, 12);
    Color numericHovered       = Color(255, 255, 255, 18);
    Color numericBorder        = Color(255, 255, 255, 20);
    Color numericText          = Color(230, 235, 245, 255);
    Color numericButtonNormal  = Color(255, 255, 255, 15);
    Color numericButtonHovered = Color(255, 255, 255, 30);
    Color numericButtonPressed = Color(255, 255, 255, 8);
    Color numericButtonText    = Color(200, 210, 230, 255);
    float numericMinHeight     = 28.f;
    #pragma endregion

    #pragma region tooltip
    Color tooltipBackground     = Color(30, 30, 40, 240);
    Color tooltipBorder         = Color(255, 255, 255, 40);
    Color tooltipText           = Color(230, 235, 245, 255);
    float tooltipCornerRadius   = 4.f;
    float tooltipPadding        = 6.f;
    int   tooltipFontSize       = 12;
    #pragma endregion
};