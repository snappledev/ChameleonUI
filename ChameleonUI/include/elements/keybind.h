#pragma once
#include "../core/UIElement.h"
#include <string>
#include <functional>

class KeyBind : public UIElement
{
public:
    KeyBind(int initialVkCode = 0);
    virtual ~KeyBind() = default;

    #pragma region bound key
    int  GetBoundKey() const { return m_boundKey; }
    void SetBoundKey(int vkCode);
    bool IsBoundToMouse() const { return m_isMouse; }
    int  GetBoundMouseButton() const { return m_mouseButton; }
    void SetBoundMouseButton(int button);
    #pragma endregion

    #pragma region state
    bool IsListening() const { return m_listening; }
    void StartListening();
    void StopListening();
    #pragma endregion

    #pragma region display
    std::wstring GetDisplayText() const;
    #pragma endregion

    #pragma region data binding
    void SetLinkedInt(int* linked) { m_linkedInt = linked; }
    void SetOnKeyBound(std::function<void(int vkCode, bool isMouse)> cb) { m_onBound = cb; }
    #pragma endregion

    #pragma region uielement overrides
    void Update(float dt) override;
    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;

    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseDown(int button) override;
    void OnClick() override;
    void OnKeyDown(int vkCode) override;
    void OnFocusGained() override;
    void OnFocusLost() override;
    void SyncFromBinding() override;

    static std::wstring VkCodeToString(int vkCode);
    static std::wstring MouseButtonToString(int button);

private:
    #pragma region members
    int                            m_boundKey    = 0;
    bool                           m_isMouse     = false;
    int                            m_mouseButton = -1;
    bool                           m_listening   = false;
    int*                           m_linkedInt   = nullptr;
    std::function<void(int, bool)> m_onBound;
    #pragma endregion
};