#pragma once
#include "../core/UIElement.h"
#include "scrollbar.h"
#include <string>
#include <vector>
#include <functional>

// pre-computed visual output from textinput, consumed by skin.
// contains only geometry and state, no colors, fonts, or style data.
struct TextInputVisuals
{
    #pragma region geometry
    Rect absRect;
    Rect contentRect;
    #pragma endregion

    #pragma region state
    float hoverT = 0.f;
    float focusT = 0.f;
    bool  multiline = false;
    bool  readOnly = false;
    #pragma endregion

    #pragma region placeholder
    bool          showPlaceholder = false;
    std::wstring  placeholderText;
    TextAnchor    placeholderAnchor = TextAnchor::CenterLeft;
    #pragma endregion

    #pragma region text lines
    struct Line {
        std::wstring text;
        Rect         rect;
    };
    std::vector<Line> lines;
    #pragma endregion

    #pragma region selection
    std::vector<Rect> selectionRects;
    #pragma endregion

    #pragma region cursor
    bool showCursor = false;
    Rect cursorRect;
    #pragma endregion

    #pragma region scrollbar
    bool showScrollbar = false;
    Rect scrollTrackRect;
    Rect scrollThumbRect;
    #pragma endregion
};

class TextInput : public UIElement
{
public:
    TextInput(bool multiline = false, const std::wstring& placeholder = L"");
    virtual ~TextInput() = default;

    #pragma region text
    const std::wstring& GetText() const { return m_text; }
    void SetText(const std::wstring& text);
    void SetLinkedString(std::wstring* linked) { m_linkedStr = linked; }
    #pragma endregion

    #pragma region placeholder
    const std::wstring& GetPlaceholder() const { return m_placeholder; }
    void SetPlaceholder(const std::wstring& ph) { m_placeholder = ph; }
    #pragma endregion

    #pragma region mode
    bool IsMultiline() const { return m_multiline; }
    bool IsPassword() const { return m_password; }
    void SetPassword(bool pw) { m_password = pw; }
    void SetMaxLength(int max) { m_maxLength = max; }
    int  GetMaxLength() const { return m_maxLength; }
    void SetReadOnly(bool ro) { m_readOnly = ro; }
    bool IsReadOnly() const { return m_readOnly; }
    #pragma endregion

    #pragma region input filter
    void SetInputFilter(std::function<std::wstring(const std::wstring&)> filter) {
        m_inputFilter = std::move(filter);
    }
    #pragma endregion

    #pragma region cursor
    int  GetCursorPos() const { return m_cursorPos; }
    int  GetSelectionStart() const { return m_selStart; }
    int  GetSelectionEnd() const { return m_selEnd; }
    bool HasSelection() const { return m_selStart != m_selEnd; }
    std::wstring GetSelectedText() const;
    float GetCursorBlinkPhase() const { return m_blinkTimer; }
    bool  IsCursorVisible() const;
    #pragma endregion

    #pragma region scroll
    float GetScrollOffset() const { return m_textScroll.GetScrollOffset(); }
    float GetContentHeight() const { return m_textScroll.GetContentSize(); }
    bool  NeedsScrollbar() const { return m_textScroll.IsShown(); }
    const Scrollbar& GetTextScrollbar() const { return m_textScroll; }
    #pragma endregion

    #pragma region callbacks
    void SetOnTextChanged(std::function<void()> cb) { m_onChanged = cb; }
    void SetOnSubmit(std::function<void()> cb) { m_onSubmit = cb; }
    #pragma endregion

    #pragma region line data
    struct LineInfo {
        int   startIndex;
        int   length;
        int   textLength;
        float y;
        float height;
    };
    const std::vector<LineInfo>& GetLines() const { return m_lines; }
    void SetLines(std::vector<LineInfo> lines, float totalH);

    void UpdateLines(IRenderer* renderer);
    #pragma endregion

    #pragma region uielement overrides
    void Update(float dt) override;
    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;

    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseWheel(int delta) override;
    void OnClick() override;

    void OnKeyDown(int vkCode) override;
    void OnChar(wchar_t ch) override;

    void OnFocusGained() override;
    void OnFocusLost() override;

    void SyncFromBinding() override;
    #pragma endregion

private:
    void InsertText(const std::wstring& str);
    void DeleteSelection();
    void DeleteChar(bool forward);
    void MoveCursor(int newPos, bool selecting);
    void EnsureCursorVisible();
    void SyncLinkedData();
    int  PositionFromPoint(int x, int y, IRenderer* renderer);
    int  ClampPos(int pos) const;
    int FindCursorLine() const;
    void ComputeVisuals(IRenderer* renderer, TextInputVisuals& out);

    static std::wstring SanitizeRawInput(const std::wstring& input);

    std::wstring                                    m_text;
    std::wstring                                    m_placeholder;
    std::wstring*                                   m_linkedStr = nullptr;
    bool                                            m_multiline = false;
    bool                                            m_password = false;
    bool                                            m_readOnly = false;
    int                                             m_maxLength = 0;
    int                                             m_cursorPos = 0;
    int                                             m_selStart = 0;
    int                                             m_selEnd = 0;
    bool                                            m_selecting = false;
    float                                           m_blinkTimer = 0.f;
    Scrollbar                                       m_textScroll{ ScrollDirection::Vertical };
    float                                           m_scrollX = 0.f;
    std::vector<LineInfo>                           m_lines;
    mutable IRenderer*                              m_cachedRenderer = nullptr;
    std::function<void()>                           m_onChanged;
    std::function<void()>                           m_onSubmit;
    std::function<std::wstring(const std::wstring&)> m_inputFilter;
    int                                             m_lastMouseX = 0, m_lastMouseY = 0;
};