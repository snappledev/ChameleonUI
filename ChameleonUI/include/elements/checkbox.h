#pragma once
#include "../core/UIElement.h"
#include <string>
#include <functional>

class Checkbox : public UIElement
{
public:
    Checkbox();
    Checkbox(const std::wstring& text, bool checked = false);

    const std::wstring& GetText() const;
    void SetText(const std::wstring& text);

    bool IsChecked() const;
    void SetChecked(bool checked);

    bool IsHovered() const;

    float GetBoxSize() const { return m_boxSize; }
    void  SetBoxSize(float size) { m_boxSize = size; InvalidateMeasure(); }
    float GetEffectiveBoxSize() const;
    void  SyncFromBinding() override;
    void  SetOnClick(std::function<void()> cb) { m_onClickCb = cb; }

    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;

    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnClick() override;

    void SetLinkedBool(bool* linkedBool);
    bool* GetLinkedBool() const;
private:
    #pragma region members
    std::wstring          m_text;
    bool                  m_checked     = false;
    bool                  m_isHovered   = false;
    bool*                 m_linkedBool  = nullptr;
    float                 m_boxSize     = 0.f;
    std::function<void()> m_onClickCb;
    #pragma endregion
};