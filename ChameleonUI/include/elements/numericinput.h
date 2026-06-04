#pragma once
#include "../core/UIElement.h"
#include <string>
#include <functional>

class NumericInput : public UIElement
{
public:
    NumericInput(float min = 0.f, float max = 100.f, float value = 0.f);

    float GetValue() const { return m_value; }
    void  SetValue(float value);

    float GetMin() const { return m_min; }
    float GetMax() const { return m_max; }
    void  SetRange(float min, float max) { m_min = min; m_max = max; SetValue(m_value); }

    float GetStep() const { return m_step; }
    void  SetStep(float step) { m_step = step; }

    bool  IsIntegerMode() const { return m_integerMode; }
    void  SetIntegerMode(bool integer) { m_integerMode = integer; InvalidateMeasure(); }

    #pragma region binding
    void SetLinkedFloat(float* linked) { m_linkedFloat = linked; }
    float* GetLinkedFloat() const { return m_linkedFloat; }
    #pragma endregion

    #pragma region display
    std::wstring GetDisplayText() const;
    int GetPrecision() const { return m_precision; }
    void SetPrecision(int digits) { m_precision = digits; }
    #pragma endregion

    #pragma region interaction state
    bool IsHovered() const { return m_hovered; }
    bool IsEditing() const { return m_editing; }
    bool IsIncrementHovered() const { return m_incHovered; }
    bool IsDecrementHovered() const { return m_decHovered; }
    bool IsIncrementPressed() const { return m_incPressed; }
    bool IsDecrementPressed() const { return m_decPressed; }
    #pragma endregion

    #pragma region geometry
    struct NumericGeometry {
        Rect fieldRect;
        Rect incrementRect;
        Rect decrementRect;
    };
    NumericGeometry ComputeGeometry() const;
    #pragma endregion

    #pragma region callbacks
    void SetOnValueChanged(std::function<void(float)> cb) { m_onValueChanged = std::move(cb); }
    #pragma endregion

    #pragma region uielement overrides
    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;
    void Update(float dt) override;
    UIElement* HitTest(int absX, int absY) override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnClick() override;
    void OnMouseMove(int x, int y) override;
    void OnKeyDown(int vkCode) override;
    void OnChar(wchar_t ch) override;
    void OnFocusGained() override;
    void OnFocusLost() override;
    void SyncFromBinding() override;
    const std::wstring& GetEditBuffer() const { return m_editBuffer; }
private:
    void Increment();
    void Decrement();
    void CommitEdit();
    void CancelEdit();

    float                      m_min, m_max, m_value;
    float                      m_step = 1.f;
    int                        m_precision = 2;
    bool                       m_integerMode = false;
    float*                     m_linkedFloat = nullptr;
    bool                       m_hovered = false;
    bool                       m_incHovered = false;
    bool                       m_decHovered = false;
    bool                       m_incPressed = false;
    bool                       m_decPressed = false;
    bool                       m_editing = false;
    std::wstring               m_editBuffer;
    float                      m_buttonWidth = 24.f;
    std::function<void(float)> m_onValueChanged;
};