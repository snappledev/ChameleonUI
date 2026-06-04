#pragma once
#include "../core/UIElement.h"
#include <string>
#include <cmath>

class Slider : public UIElement
{
public:
    Slider(float min = 0.0f, float max = 1.0f, float value = 0.5f);

    float GetValue() const;
    void SetValue(float value);
    bool IsDragging() const { return m_dragging; }

    void Update(float dt) override;
    void Draw(IRenderer* renderer) override;
    void Measure(const Size& availableSize) override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void UpdateDrag(float mouseX);
    float GetMin() const { return m_min; }
    float GetMax() const { return m_max; }

    void SetLinkedFloat(float* linked) { m_linkedFloat = linked; }
    float* GetLinkedFloat() const { return m_linkedFloat; }
    void SyncFromBinding() override;

    #pragma region geometry
    struct SliderGeometry {
        Rect  trackRect;
        Rect  fillRect;
        float thumbCX, thumbCY;
        float normalizedValue;
    };

    SliderGeometry ComputeGeometry(float trackHeight, float thumbRadius) const;
    #pragma endregion

    std::wstring GetFormattedValue() const;

private:
    float  m_min, m_max, m_value;
    bool   m_dragging = false;
    bool   m_wasDragging = false;
    float* m_linkedFloat = nullptr;
};