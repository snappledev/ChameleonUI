#include "../../include/elements/TextInput.h"
#include "../../include/skins/skin.h"
#include "../../include/core/UIStyle.h"
#include <algorithm>
#include <windowsx.h>

#pragma region lifecycle
TextInput::TextInput(bool multiline, const std::wstring& placeholder)
    : m_multiline(multiline), m_placeholder(placeholder)
{}

/*static*/ std::wstring TextInput::SanitizeRawInput(const std::wstring& input)
{
    std::wstring safe;
    safe.reserve(input.size());
    for (wchar_t c : input) {
        if (c == L'\0')                         continue;
        if (c == 0x7F)                          continue;
        if (c >= 0xFDD0 && c <= 0xFDEF)         continue;
        if ((c & 0xFFFE) == 0xFFFE)             continue;
        if (c < 0x20 && c != L'\n' && c != L'\t') continue;
        safe += c;
    }
    return safe;
}

void TextInput::SetText(const std::wstring& text)
{
    std::wstring safe = SanitizeRawInput(text);
    if (m_maxLength > 0 && (int)safe.size() > m_maxLength)
        safe = safe.substr(0, m_maxLength);

    m_text = std::move(safe);
    m_cursorPos = ClampPos((int)m_text.size());
    m_selStart = m_selEnd = m_cursorPos;
    SyncLinkedData();
}

std::wstring TextInput::GetSelectedText() const
{
    int lo = (std::min)(m_selStart, m_selEnd);
    int hi = (std::max)(m_selStart, m_selEnd);
    return m_text.substr(lo, hi - lo);
}

bool TextInput::IsCursorVisible() const
{
    return fmod(m_blinkTimer, 1.0f) < 0.5f;
}

int TextInput::ClampPos(int pos) const
{
    if (pos < 0) return 0;
    if (pos > (int)m_text.size()) return (int)m_text.size();
    return pos;
}

void TextInput::SyncFromBinding()
{
    if (m_linkedStr)
        SetText(*m_linkedStr);
}

void TextInput::InsertText(const std::wstring& str)
{
    if (m_readOnly) return;
    if (HasSelection()) DeleteSelection();

    std::wstring toInsert = str;

    if (!m_multiline) {
        std::wstring filtered;
        filtered.reserve(toInsert.size());
        for (wchar_t c : toInsert)
            if (c != L'\n' && c != L'\r') filtered += c;
        toInsert = std::move(filtered);
    }

    toInsert = SanitizeRawInput(toInsert);

    if (m_inputFilter)
        toInsert = m_inputFilter(toInsert);

    if (toInsert.empty()) return;

    if (m_maxLength > 0) {
        int avail = m_maxLength - (int)m_text.size();
        if (avail <= 0) return;
        if ((int)toInsert.size() > avail) toInsert = toInsert.substr(0, avail);
    }

    m_text.insert(m_cursorPos, toInsert);
    m_cursorPos += (int)toInsert.size();
    m_selStart = m_selEnd = m_cursorPos;
    m_blinkTimer = 0.f;
    SyncLinkedData();
    if (m_onChanged) m_onChanged();
}

void TextInput::DeleteSelection()
{
    if (!HasSelection()) return;
    int lo = (std::min)(m_selStart, m_selEnd);
    int hi = (std::max)(m_selStart, m_selEnd);
    m_text.erase(lo, hi - lo);
    m_cursorPos = lo;
    m_selStart = m_selEnd = lo;
    SyncLinkedData();
    if (m_onChanged) m_onChanged();
}

void TextInput::DeleteChar(bool forward)
{
    if (m_readOnly) return;
    if (HasSelection()) { DeleteSelection(); return; }
    if (forward) {
        if (m_cursorPos < (int)m_text.size())
            m_text.erase(m_cursorPos, 1);
    }
    else {
        if (m_cursorPos > 0) {
            m_cursorPos--;
            m_text.erase(m_cursorPos, 1);
        }
    }
    m_selStart = m_selEnd = m_cursorPos;
    m_blinkTimer = 0.f;
    SyncLinkedData();
    if (m_onChanged) m_onChanged();
}

void TextInput::MoveCursor(int newPos, bool selecting)
{
    newPos = ClampPos(newPos);
    m_cursorPos = newPos;
    if (selecting) {
        m_selEnd = newPos;
    }
    else {
        m_selStart = m_selEnd = newPos;
    }
    m_blinkTimer = 0.f;
}

int TextInput::FindCursorLine() const
{
    if (m_lines.empty()) return 0;
    for (int i = 0; i < (int)m_lines.size() - 1; ++i) {
        if (m_cursorPos >= m_lines[i].startIndex &&
            m_cursorPos <= m_lines[i].startIndex + m_lines[i].textLength)
            return i;
    }
    return (int)m_lines.size() - 1;
}

void TextInput::EnsureCursorVisible()
{
    if (m_multiline && !m_lines.empty()) {
        int lineIdx = FindCursorLine();
        float cursorY = m_lines[lineIdx].y;
        float lineH = m_lines[lineIdx].height;
        Rect client = GetClientRect();
        float viewH = client.h;
        float scrollY = m_textScroll.GetScrollOffset();

        if (cursorY < scrollY)
            scrollY = cursorY;
        else if (cursorY + lineH > scrollY + viewH)
            scrollY = cursorY + lineH - viewH;

        m_textScroll.SetViewportSize(viewH);
        m_textScroll.SetScrollOffset(scrollY);
    }
}

void TextInput::SyncLinkedData()
{
    if (m_linkedStr) *m_linkedStr = m_text;
}

void TextInput::SetLines(std::vector<LineInfo> lines, float totalH)
{
    m_lines = std::move(lines);
    Rect client = GetClientRect();
    m_textScroll.SetContentSize(totalH);
    m_textScroll.SetViewportSize((float)client.h);
    m_textScroll.SetShown(m_multiline && (totalH > client.h));
}
#pragma endregion

#pragma region update
void TextInput::Update(float dt)
{
    m_anim.Tick(dt);
    if (IsFocused())
        m_blinkTimer += dt;
}
#pragma endregion

#pragma region measure & arrange
void TextInput::Measure(const Size& availableSize)
{
    if (!m_needMeasure) return;
    m_desiredSize.width = (m_width > 0) ? (float)m_width : 200.f;
    m_desiredSize.height = (m_height > 0) ? (float)m_height : (m_multiline ? 100.f : 30.f);
    m_needMeasure = false;
}

int TextInput::PositionFromPoint(int x, int y, IRenderer* renderer)
{
    if (!renderer) return (int)m_text.size();

    Skin* skin = GetEffectiveSkin();
    int fontSize = (skin && skin->GetStyle()) ? skin->GetStyle()->fontSize : 13;
    std::wstring fontFamily = (skin && skin->GetStyle()) ? skin->GetStyle()->fontFamily : L"Segoe UI";

    std::wstring displayText = m_password
        ? std::wstring(m_text.size(), L'\x25CF')
        : m_text;

    Rect absRect = GetAbsoluteRect();
    float padX = 8.f;
    float padY = m_multiline ? 6.f : 0.f;
    float contentX = absRect.x + padX;
    float contentY = absRect.y + padY;
    float localX = (float)x - contentX;
    float localY = (float)y - contentY;

    if (m_multiline && !m_lines.empty())
    {
        float scrollY = m_textScroll.GetScrollOffset();
        localY += scrollY;

        int lineIdx = 0;
        for (int i = 0; i < (int)m_lines.size(); ++i)
        {
            if (localY < m_lines[i].y + m_lines[i].height) {
                lineIdx = i;
                break;
            }
            lineIdx = i;
        }

        const LineInfo& line = m_lines[lineIdx];
        std::wstring lineStr = displayText.substr(line.startIndex, line.textLength);

        int bestPos = 0;
        float bestDist = std::abs(localX);

        for (int c = 1; c <= (int)lineStr.size(); ++c)
        {
            Rect measured = renderer->MeasureText(lineStr.substr(0, c), fontSize, fontFamily.c_str());
            float dist = std::abs(localX - measured.w);
            if (dist < bestDist) {
                bestDist = dist;
                bestPos = c;
            }
        }

        return line.startIndex + bestPos;
    }
    else
    {
        int bestPos = 0;
        float bestDist = std::abs(localX);

        for (int c = 1; c <= (int)displayText.size(); ++c)
        {
            Rect measured = renderer->MeasureText(displayText.substr(0, c), fontSize, fontFamily.c_str());
            float dist = std::abs(localX - measured.w);
            if (dist < bestDist) {
                bestDist = dist;
                bestPos = c;
            }
        }

        return bestPos;
    }
}
#pragma endregion

#pragma region drawing
void TextInput::UpdateLines(IRenderer* renderer)
{
    if (!m_multiline) {
        m_lines.clear();
        return;
    }

    Skin* skin = GetEffectiveSkin();
    int fontSize = (skin && skin->GetStyle()) ? skin->GetStyle()->fontSize : 13;
    float lineH = (float)fontSize + 4.f;

    const std::wstring& rawText = m_text;
    std::wstring displayText = m_password
        ? std::wstring(rawText.size(), L'\x25CF')
        : rawText;

    std::vector<LineInfo> lines;
    float yOff = 0.f;
    size_t idx = 0;

    while (idx <= displayText.size()) {
        size_t nl = displayText.find(L'\n', idx);
        if (nl == std::wstring::npos) nl = displayText.size();

        LineInfo li;
        li.startIndex = (int)idx;
        li.textLength = (int)(nl - idx);
        li.length = li.textLength + (nl < displayText.size() ? 1 : 0);
        li.y = yOff;
        li.height = lineH;
        lines.push_back(li);

        yOff += lineH;
        idx = nl + 1;
        if (nl == displayText.size()) break;
    }

    SetLines(std::move(lines), yOff);
}

void TextInput::ComputeVisuals(IRenderer* renderer, TextInputVisuals& vis)
{
    Skin* skin = GetEffectiveSkin();
    int fontSize = (skin && skin->GetStyle()) ? skin->GetStyle()->fontSize : 13;
    std::wstring fontFamily = (skin && skin->GetStyle())
        ? skin->GetStyle()->fontFamily : L"Segoe UI";

    vis.absRect = GetAbsoluteRect();
    vis.hoverT = m_anim.hover.Value();
    vis.focusT = m_anim.focus.Value();
    vis.multiline = m_multiline;
    vis.readOnly = m_readOnly;

    float padX = 8.f;
    float padY = m_multiline ? 6.f : 0.f;
    vis.contentRect = Rect(vis.absRect.x + padX, vis.absRect.y + padY,
        vis.absRect.w - padX * 2.f, vis.absRect.h - padY * 2.f);

    if (m_text.empty() && !IsFocused()) {
        vis.showPlaceholder = true;
        vis.placeholderText = m_placeholder;
        vis.placeholderAnchor = m_multiline ? TextAnchor::TopLeft : TextAnchor::CenterLeft;
        return;
    }

    std::wstring displayText = m_password
        ? std::wstring(m_text.size(), L'\x25CF')
        : m_text;

    int selLo = (std::min)(m_selStart, m_selEnd);
    int selHi = (std::max)(m_selStart, m_selEnd);

    if (m_multiline)
    {
        for (const auto& line : m_lines)
        {
            float drawY = vis.contentRect.y + line.y - GetScrollOffset();
            float lineH = line.height;

            if (drawY + lineH < vis.contentRect.y ||
                drawY > vis.contentRect.y + vis.contentRect.h)
                continue;

            TextInputVisuals::Line vl;
            vl.text = displayText.substr(line.startIndex, line.textLength);
            vl.rect = Rect(vis.contentRect.x, drawY, vis.contentRect.w, lineH);
            vis.lines.push_back(std::move(vl));

            if (selLo != selHi && selHi > line.startIndex &&
                selLo < line.startIndex + line.textLength)
            {
                int localLo = (std::max)(0, selLo - line.startIndex);
                int localHi = (std::min)(line.textLength, selHi - line.startIndex);
                if (localLo < localHi) {
                    std::wstring lineStr = displayText.substr(line.startIndex, line.textLength);
                    Rect beforeSel = renderer->MeasureText(
                        lineStr.substr(0, localLo), fontSize, fontFamily.c_str());
                    Rect selText = renderer->MeasureText(
                        lineStr.substr(localLo, localHi - localLo),
                        fontSize, fontFamily.c_str());
                    vis.selectionRects.push_back(
                        Rect(vis.contentRect.x + beforeSel.w, drawY, selText.w, lineH));
                }
            }
        }

        if (IsFocused() && IsCursorVisible())
        {
            vis.showCursor = true;
            float cursorX = vis.contentRect.x;
            float cursorY = vis.contentRect.y - GetScrollOffset();
            float cursorH = m_lines.empty()
                ? (float)fontSize + 4.f : m_lines[0].height;

            int lineIdx = FindCursorLine();
            if (lineIdx < (int)m_lines.size()) {
                const auto& line = m_lines[lineIdx];
                int localPos = m_cursorPos - line.startIndex;
                if (localPos < 0) localPos = 0;
                if (localPos > line.textLength) localPos = line.textLength;
                std::wstring beforeCursor = displayText.substr(
                    line.startIndex, localPos);
                Rect cb = renderer->MeasureText(
                    beforeCursor, fontSize, fontFamily.c_str());
                cursorX = vis.contentRect.x + cb.w;
                cursorY = vis.contentRect.y + line.y - GetScrollOffset();
                cursorH = line.height;
            }
            vis.cursorRect = Rect(cursorX, cursorY, 1.5f, cursorH);
        }
    }
    else
    {
        TextInputVisuals::Line vl;
        vl.text = displayText;
        vl.rect = vis.contentRect;
        vis.lines.push_back(std::move(vl));

        if (selLo != selHi) {
            Rect beforeSel = renderer->MeasureText(
                displayText.substr(0, selLo), fontSize, fontFamily.c_str());
            Rect selMeasure = renderer->MeasureText(
                displayText.substr(selLo, selHi - selLo),
                fontSize, fontFamily.c_str());
            vis.selectionRects.push_back(
                Rect(vis.contentRect.x + beforeSel.w,
                    vis.contentRect.y + 2.f, selMeasure.w,
                    vis.contentRect.h - 4.f));
        }

        if (IsFocused() && IsCursorVisible()) {
            vis.showCursor = true;
            Rect beforeCursor = renderer->MeasureText(
                displayText.substr(0, m_cursorPos), fontSize, fontFamily.c_str());
            float cx = vis.contentRect.x + beforeCursor.w;
            vis.cursorRect = Rect(cx, vis.contentRect.y + 4.f,
                1.5f, vis.contentRect.h - 8.f);
        }
    }

    if (m_multiline && NeedsScrollbar()) {
        vis.showScrollbar = true;
        vis.scrollTrackRect = Rect(vis.absRect.x + vis.absRect.w - 10.f,
            vis.absRect.y + 2.f,
            8.f, vis.absRect.h - 4.f);
        int minThumb = (skin && skin->GetStyle())
            ? skin->GetStyle()->scrollbarMinThumbHeight : 24;
        vis.scrollThumbRect = m_textScroll.ComputeThumbRect(
            vis.scrollTrackRect, minThumb);
    }
}

void TextInput::Draw(IRenderer* renderer)
{
    m_cachedRenderer = renderer;

    if (m_multiline)
        UpdateLines(renderer);

    TextInputVisuals vis;
    ComputeVisuals(renderer, vis);

    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawTextInput(renderer, vis);
}
#pragma endregion

#pragma region input
void TextInput::OnMouseEnter()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnTextInputHoverEnter(*this);
}

void TextInput::OnMouseLeave()
{
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnTextInputHoverLeave(*this);
    m_selecting = false;
}

void TextInput::OnMouseDown(int button)
{
    if (button != 0) return;
    RequestFocus();
    m_blinkTimer = 0.f;

    bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

    if (m_cachedRenderer)
    {
        int pos = PositionFromPoint(m_lastMouseX, m_lastMouseY, m_cachedRenderer);
        if (shift) {
            m_selEnd = pos;
            m_cursorPos = pos;
        }
        else {
            m_cursorPos = pos;
            m_selStart = pos;
            m_selEnd = pos;
        }
    }

    m_selecting = true;
    CaptureInput();
}

void TextInput::OnMouseUp(int button)
{
    if (button == 0) {
        m_selecting = false;
        ReleaseInput();
    }
}

void TextInput::OnMouseMove(int x, int y)
{
    m_lastMouseX = x;
    m_lastMouseY = y;

    if (m_selecting && m_cachedRenderer)
    {
        int pos = PositionFromPoint(x, y, m_cachedRenderer);
        m_selEnd = pos;
        m_cursorPos = pos;
        m_blinkTimer = 0.f;
        EnsureCursorVisible();
    }
}

void TextInput::OnMouseWheel(int delta)
{
    if (m_multiline && m_textScroll.IsShown()) {
        if (m_textScroll.HandleWheel(delta))
            return;
    }
    UIElement::OnMouseWheel(delta);
}

void TextInput::OnClick()
{
}

void TextInput::OnKeyDown(int vkCode)
{
    bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

    switch (vkCode)
    {
    case VK_LEFT:
        if (ctrl) {
            int pos = m_cursorPos;
            while (pos > 0 && m_text[pos - 1] == L' ') pos--;
            while (pos > 0 && m_text[pos - 1] != L' ') pos--;
            MoveCursor(pos, shift);
        }
        else {
            MoveCursor(m_cursorPos - 1, shift);
        }
        EnsureCursorVisible();
        break;

    case VK_RIGHT:
        if (ctrl) {
            int pos = m_cursorPos;
            while (pos < (int)m_text.size() && m_text[pos] != L' ') pos++;
            while (pos < (int)m_text.size() && m_text[pos] == L' ') pos++;
            MoveCursor(pos, shift);
        }
        else {
            MoveCursor(m_cursorPos + 1, shift);
        }
        EnsureCursorVisible();
        break;

    case VK_UP:
        if (m_multiline && !m_lines.empty()) {
            int lineIdx = FindCursorLine();
            if (lineIdx > 0) {
                int colInLine = m_cursorPos - m_lines[lineIdx].startIndex;
                int newPos = m_lines[lineIdx - 1].startIndex
                    + (std::min)(colInLine, m_lines[lineIdx - 1].textLength);
                MoveCursor(newPos, shift);
            }
            EnsureCursorVisible();
        }
        break;

    case VK_DOWN:
        if (m_multiline && !m_lines.empty()) {
            int lineIdx = FindCursorLine();
            if (lineIdx < (int)m_lines.size() - 1) {
                int colInLine = m_cursorPos - m_lines[lineIdx].startIndex;
                int newPos = m_lines[lineIdx + 1].startIndex
                    + (std::min)(colInLine, m_lines[lineIdx + 1].textLength);
                MoveCursor(newPos, shift);
            }
            EnsureCursorVisible();
        }
        break;

    case VK_HOME:
        if (ctrl) MoveCursor(0, shift);
        else {
            if (m_multiline && !m_lines.empty()) {
                int lineIdx = FindCursorLine();
                MoveCursor(m_lines[lineIdx].startIndex, shift);
            }
            else {
                MoveCursor(0, shift);
            }
        }
        EnsureCursorVisible();
        break;

    case VK_END:
        if (ctrl) MoveCursor((int)m_text.size(), shift);
        else {
            if (m_multiline && !m_lines.empty()) {
                int lineIdx = FindCursorLine();
                MoveCursor(m_lines[lineIdx].startIndex + m_lines[lineIdx].textLength, shift);
            }
            else {
                MoveCursor((int)m_text.size(), shift);
            }
        }
        EnsureCursorVisible();
        break;

    case VK_BACK:
        DeleteChar(false);
        EnsureCursorVisible();
        break;

    case VK_DELETE:
        DeleteChar(true);
        EnsureCursorVisible();
        break;

    case VK_RETURN:
        if (m_multiline) {
            InsertText(L"\n");
            EnsureCursorVisible();
        }
        else {
            if (m_onSubmit) m_onSubmit();
        }
        break;

    case 'A':
        if (ctrl) {
            m_selStart = 0;
            m_selEnd = (int)m_text.size();
            m_cursorPos = m_selEnd;
        }
        break;

    case 'C':
        if (ctrl && HasSelection()) {
            std::wstring sel = GetSelectedText();
            if (OpenClipboard(nullptr)) {
                EmptyClipboard();
                HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (sel.size() + 1) * sizeof(wchar_t));
                if (hMem) {
                    wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
                    wcscpy_s(pMem, sel.size() + 1, sel.c_str());
                    GlobalUnlock(hMem);
                    SetClipboardData(CF_UNICODETEXT, hMem);
                }
                CloseClipboard();
            }
        }
        break;

    case 'V':
        if (ctrl) {
            if (OpenClipboard(nullptr)) {
                HANDLE hData = GetClipboardData(CF_UNICODETEXT);
                if (hData) {
                    wchar_t* pText = (wchar_t*)GlobalLock(hData);
                    if (pText) {
                        SIZE_T cbSize = GlobalSize(hData);
                        size_t maxChars = cbSize / sizeof(wchar_t);
                        size_t len = wcsnlen(pText, maxChars);

                        constexpr size_t kMaxPasteChars = 512 * 1024;
                        if (len > kMaxPasteChars) len = kMaxPasteChars;

                        std::wstring pasteStr(pText, len);
                        GlobalUnlock(hData);

                        InsertText(pasteStr);
                        EnsureCursorVisible();
                    }
                }
                CloseClipboard();
            }
        }
        break;

    case 'X':
        if (ctrl && HasSelection()) {
            OnKeyDown('C');
            DeleteSelection();
            EnsureCursorVisible();
        }
        break;
    }
}

void TextInput::OnChar(wchar_t ch)
{
    if (ch < 32 && ch != L'\n' && ch != L'\r' && ch != L'\t') return;
    if (ch == L'\r') return;
    if (ch == L'\t') return;

    InsertText(std::wstring(1, ch));
    EnsureCursorVisible();
}

void TextInput::OnFocusGained()
{
    m_blinkTimer = 0.f;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnTextInputFocusEnter(*this);
}

void TextInput::OnFocusLost()
{
    m_selStart = m_selEnd = m_cursorPos;
    Skin* skin = GetEffectiveSkin();
    if (skin) skin->OnTextInputFocusLeave(*this);
}
#pragma endregion