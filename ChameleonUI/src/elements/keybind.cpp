#include "../../include/elements/keybind.h"
#include "../../include/skins/skin.h"
#include "../../include/core/UIStyle.h"
#include <windows.h>
#include <sstream>

#pragma region lifecycle
KeyBind::KeyBind(int initialVkCode)
    : m_boundKey(initialVkCode)
{}

void KeyBind::SetBoundKey(int vkCode)
{
    m_boundKey = vkCode;
    m_isMouse = false;
    m_mouseButton = -1;
    if (m_linkedInt) *m_linkedInt = vkCode;
}

void KeyBind::SetBoundMouseButton(int button)
{
    m_isMouse = true;
    m_mouseButton = button;
    m_boundKey = 0;
    if (m_linkedInt) *m_linkedInt = -(button + 1);
}

void KeyBind::StartListening()
{
    if (m_listening) return;
    m_listening = true;
    RequestFocus();
    CaptureInput();
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnKeyBindListeningStarted(*this);
}

void KeyBind::StopListening()
{
    if (!m_listening) return;
    m_listening = false;
    ReleaseInput();
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnKeyBindListeningStopped(*this);
}

void KeyBind::SyncFromBinding()
{
    if (m_linkedInt)
        SetBoundKey(*m_linkedInt);
}

std::wstring KeyBind::GetDisplayText() const
{
    if (m_listening) return L"Press a key...";
    if (m_isMouse) return MouseButtonToString(m_mouseButton);
    if (m_boundKey == 0) return L"None";
    return VkCodeToString(m_boundKey);
}
#pragma endregion

#pragma region update
void KeyBind::Update(float dt)
{
    UIElement::Update(dt);
}
#pragma endregion

#pragma region measure & arrange
void KeyBind::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;

    float minH = 30.f;
    Skin* skin = GetEffectiveSkin();
    if (skin && skin->GetStyle())
        minH = skin->GetStyle()->keybindMinHeight;

    if (m_widthPolicy == SizePolicy::ContentFit)
    {
        m_desiredSize.width = (m_width > 0) ? (float)m_width : 120.f;
    }
    else
    {
        m_desiredSize.width = (m_width > 0) ? (float)m_width : 150.f;
    }

    m_desiredSize.height = (m_height > 0) ? (float)m_height : minH;
    m_needMeasure = false;
}
#pragma endregion

#pragma region drawing
void KeyBind::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawKeyBind(renderer, *this, GetAbsoluteRect());
}
#pragma endregion

#pragma region input
void KeyBind::OnMouseEnter()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnKeyBindHoverEnter(*this);
}

void KeyBind::OnMouseLeave()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnKeyBindHoverLeave(*this);
}

void KeyBind::OnMouseDown(int button)
{
    if (m_listening) {
        SetBoundMouseButton(button);
        if (m_onBound) m_onBound(-(button + 1), true);
        StopListening();
        return;
    }
}

void KeyBind::OnClick()
{
    if (!m_listening) {
        StartListening();
    }
}

void KeyBind::OnKeyDown(int vkCode)
{
    if (!m_listening) return;

    if (vkCode == VK_ESCAPE) {
        StopListening();
        return;
    }

    SetBoundKey(vkCode);
    if (m_onBound) m_onBound(vkCode, false);
    StopListening();
}

void KeyBind::OnFocusGained()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnKeyBindFocusEnter(*this);
}

void KeyBind::OnFocusLost()
{
    StopListening();
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnKeyBindFocusLeave(*this);
}
#pragma endregion

#pragma region key string utilities
std::wstring KeyBind::VkCodeToString(int vk)
{
    switch (vk) {
    case VK_SPACE:    return L"Space";
    case VK_RETURN:   return L"Enter";
    case VK_TAB:      return L"Tab";
    case VK_ESCAPE:   return L"Escape";
    case VK_BACK:     return L"Backspace";
    case VK_DELETE:   return L"Delete";
    case VK_INSERT:   return L"Insert";
    case VK_HOME:     return L"Home";
    case VK_END:      return L"End";
    case VK_PRIOR:    return L"Page Up";
    case VK_NEXT:     return L"Page Down";
    case VK_UP:       return L"Up";
    case VK_DOWN:     return L"Down";
    case VK_LEFT:     return L"Left";
    case VK_RIGHT:    return L"Right";
    case VK_SHIFT: case VK_LSHIFT: case VK_RSHIFT: return L"Shift";
    case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL: return L"Ctrl";
    case VK_MENU: case VK_LMENU: case VK_RMENU: return L"Alt";
    case VK_CAPITAL:  return L"Caps Lock";
    case VK_NUMLOCK:  return L"Num Lock";
    case VK_SCROLL:   return L"Scroll Lock";
    case VK_SNAPSHOT: return L"Print Screen";
    case VK_PAUSE:    return L"Pause";
    }

    if (vk >= VK_F1 && vk <= VK_F24) {
        return L"F" + std::to_wstring(vk - VK_F1 + 1);
    }

    if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) {
        return L"Num " + std::to_wstring(vk - VK_NUMPAD0);
    }

    if ((vk >= '0' && vk <= '9') || (vk >= 'A' && vk <= 'Z')) {
        return std::wstring(1, (wchar_t)vk);
    }

    UINT scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    wchar_t name[64] = {};
    if (GetKeyNameTextW(scanCode << 16, name, 64) > 0)
        return name;

    return L"Key " + std::to_wstring(vk);
}

std::wstring KeyBind::MouseButtonToString(int button)
{
    switch (button) {
    case 0: return L"Mouse Left";
    case 1: return L"Mouse Right";
    case 2: return L"Mouse Middle";
    default: return L"Mouse " + std::to_wstring(button + 1);
    }
}
#pragma endregion