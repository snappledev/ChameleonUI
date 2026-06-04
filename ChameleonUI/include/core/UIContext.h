#pragma once
#include "UIElement.h"
#include "../elements/window.h"
#include <vector>
#include <memory>
#include <functional>

class ComboBox;
class Slider;
class ColorPickerModal;

class UIContext
{
public:
    #pragma region lifecycle
    UIContext();
    ~UIContext();

    void Initialize(HWND hwnd);
    void Shutdown();

    void ProcessInput();
    void Update(float dt);
    void Draw(IRenderer* renderer);
    #pragma endregion

    #pragma region windows
    Window& CreateUIWindow(const std::wstring& title, int x, int y, int w, int h);
    #pragma endregion

    #pragma region mouse input
    void SetMousePosition(int x, int y);
    void SetMouseButtonDown(int button);
    void SetMouseButtonUp(int button);

    HWND GetHwnd() const;
    int GetMouseX() const;
    int GetMouseY() const;
    bool IsMouseButtonDown(int button) const;
    void PerformLayout();
    void SetMouseWheel(int delta);
    #pragma endregion

    #pragma region layout dirty
    void SetLayoutDirty() { m_layoutDirty = true; }
    bool IsLayoutDirty() const { return m_layoutDirty; }
    void ClearLayoutDirty() { m_layoutDirty = false; }
    bool IsDragging() const { return m_draggingWindow != nullptr; }
    #pragma endregion

    #pragma region input capture
    void SetInputCapture(UIElement* element);
    void ReleaseInputCapture(UIElement* element);
    UIElement* GetInputCapture() const { return m_inputCapture; }
    bool HasInputCapture() const { return m_inputCapture != nullptr; }

    void SetFocus(UIElement* element);
    void ClearFocus(UIElement* element);
    UIElement* GetFocusedElement() const { return m_focused; }
    #pragma endregion

    #pragma region window management
    void BringWindowToFront(Window* window);
    Window* GetFrontWindow() const;
    #pragma endregion

    #pragma region overlays
    void QueueOverlay(std::function<void(IRenderer*)> drawFn);
    void AddPersistentOverlay(UIElement* element);
    void RemovePersistentOverlay(UIElement* element);
    #pragma endregion

    #pragma region keyboard
    void OnKeyDown(int vkCode);
    void OnKeyUp(int vkCode);
    void OnChar(wchar_t ch);
    #pragma endregion

    #pragma region tooltips
    UIElement* FindTooltipElement(UIElement* element);
    void SetTooltipDelay(float seconds) { m_tooltipDelay = seconds; }
    float GetTooltipDelay() const { return m_tooltipDelay; }
    void SetTooltipsEnabled(bool enabled) { m_tooltipsEnabled = enabled; }
    bool AreTooltipsEnabled() const { return m_tooltipsEnabled; }
    #pragma endregion

    #pragma region window access
    const std::vector<std::unique_ptr<Window>>& GetWindows() const { return m_windows; }
    std::vector<std::unique_ptr<Window>>& GetWindows() { return m_windows; }
    #pragma endregion

    #pragma region color picker
    void SetColorPickerModal(ColorPickerModal* modal) { m_colorPickerModal = modal; }
    ColorPickerModal* GetColorPickerModal() const { return m_colorPickerModal; }
    #pragma endregion

private:
    #pragma region private methods
    void ResolveHover();
    void ClearEdgeFlags();
    UIElement* HitTestWindows();
    #pragma endregion

    #pragma region private members
    HWND m_hwnd = nullptr;
    std::vector<std::unique_ptr<Window>> m_windows;
    std::vector<UIElement*> m_persistentOverlays;

    int m_mouseX = 0;
    int m_mouseY = 0;
    bool m_mouseButtons[3] = { false, false, false };
    bool m_mouseDownThisFrame[3] = { false, false, false };
    bool m_mouseUpThisFrame[3] = { false, false, false };

    UIElement* m_hovered = nullptr;
    UIElement* m_active = nullptr;
    Slider* m_activeSlider = nullptr;
    UIElement* m_inputCapture = nullptr;
    UIElement* m_focused = nullptr;
    Window* m_activeWindow = nullptr;
    Window* m_draggingWindow = nullptr;
    int m_dragOffsetX = 0;
    int m_dragOffsetY = 0;
    int m_mouseWheelDelta = 0;
    bool m_layoutDirty = true;

    ColorPickerModal* m_colorPickerModal = nullptr;
    std::vector<std::function<void(IRenderer*)>> m_overlayQueue;

    bool m_tooltipsEnabled = true;
    float m_tooltipDelay = 0.5f;
    float m_tooltipTimer = 0.f;
    UIElement* m_tooltipTarget = nullptr;
    int m_tooltipMouseX = 0;
    int m_tooltipMouseY = 0;
    bool m_tooltipVisible = false;
    #pragma endregion
};
