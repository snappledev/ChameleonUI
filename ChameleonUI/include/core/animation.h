#pragma once
#include <cmath>
#include <algorithm>

// all take t in [0,1] and return [0,1]
using EasingFn = float(*)(float);

#pragma region easing functions
namespace Easing
{
    inline float Linear(float t) { return t; }

    inline float QuadIn(float t) { return t * t; }
    inline float QuadOut(float t) { return t * (2.f - t); }
    inline float QuadInOut(float t) {
        return t < 0.5f ? 2.f * t * t : -1.f + (4.f - 2.f * t) * t;
    }

    inline float CubicIn(float t) { return t * t * t; }
    inline float CubicOut(float t) { float u = t - 1.f; return u * u * u + 1.f; }
    inline float CubicInOut(float t) {
        return t < 0.5f ? 4.f * t * t * t : 1.f + (t - 1.f) * (2.f * t - 2.f) * (2.f * t - 2.f);
    }

    inline float ExpoOut(float t) {
        return t >= 1.f ? 1.f : 1.f - std::pow(2.f, -10.f * t);
    }

    inline float BackOut(float t) {
        const float c = 1.70158f;
        float u = t - 1.f;
        return 1.f + (c + 1.f) * u * u * u + c * u * u;
    }

    inline float ElasticOut(float t) {
        if (t <= 0.f) return 0.f;
        if (t >= 1.f) return 1.f;
        return std::pow(2.f, -10.f * t) * std::sin((t - 0.075f) * (6.2831853f / 0.3f)) + 1.f;
    }

    inline float SmoothStep(float t) { return t * t * (3.f - 2.f * t); }

    inline float OvershootOut(float t) {
        float u = t - 1.f;
        return 1.f + 2.5f * u * u * u + 1.5f * u * u;
    }
}
#pragma endregion

#pragma region anim value
template<typename T>
struct AnimValue
{
    T        current{};      // interpolated value
    T        from{};         // start of current animation
    T        target{};       // end of current animation
    float    elapsed  = 0.f; // seconds since animation started
    float    duration = 0.f; // total animation duration in seconds
    EasingFn easing   = Easing::QuadOut;

    AnimValue() = default;
    explicit AnimValue(T initial, EasingFn ease = Easing::QuadOut)
        : current(initial), from(initial), target(initial), easing(ease) {}

    bool IsAnimating() const { return elapsed < duration; }

    void TransitionTo(T newTarget, float dur) {
        if (dur <= 0.f) {
            current = from = target = newTarget;
            elapsed = duration = 0.f;
            return;
        }
        from     = current;
        target   = newTarget;
        duration = dur;
        elapsed  = 0.f;
    }

    void TransitionToIfChanged(T newTarget, float dur) {
        if (target == newTarget) return;
        TransitionTo(newTarget, dur);
    }

    void Set(T value) {
        current = from = target = value;
        elapsed = duration = 0.f;
    }

    bool Tick(float dt) {
        if (elapsed >= duration) return false;
        elapsed += dt;
        if (elapsed >= duration) {
            elapsed = duration;
            current = target;
            return true;
        }
        float t = elapsed / duration;
        float e = easing ? easing(t) : t;
        current = Lerp(from, target, e);
        return true;
    }

    const T& Value() const { return current; }
    operator const T& () const { return current; }

private:
    static float Lerp(float a, float b, float t) { return a + (b - a) * t; }
};
#pragma endregion

#pragma region anim state
struct AnimState
{
    AnimValue<float> hover{ 0.f };
    AnimValue<float> press{ 0.f };
    AnimValue<float> checked{ 0.f };
    AnimValue<float> focus{ 0.f };
    AnimValue<float> active{ 0.f };
    AnimValue<float> opacity{ 1.f };
    AnimValue<float> glow{ 0.f };

    bool IsAnimating() const {
        return hover.IsAnimating() || press.IsAnimating() ||
            checked.IsAnimating() || focus.IsAnimating() ||
            active.IsAnimating() || opacity.IsAnimating() ||
            glow.IsAnimating();
    }

    bool Tick(float dt) {
        bool changed = false;
        changed |= hover.Tick(dt);
        changed |= press.Tick(dt);
        changed |= checked.Tick(dt);
        changed |= focus.Tick(dt);
        changed |= active.Tick(dt);
        changed |= opacity.Tick(dt);
        changed |= glow.Tick(dt);
        return changed;
    }
};
#pragma endregion

#pragma region anim config
struct AnimConfig
{
    float    hoverIn       = 0.15f;
    float    hoverOut      = 0.20f;
    float    pressIn       = 0.06f;
    float    pressOut      = 0.12f;
    float    checkToggle   = 0.20f;
    float    tabSwitch     = 0.25f;
    float    fadeIn        = 0.20f;
    float    fadeOut       = 0.15f;
    float    glowIn        = 0.20f;
    float    glowOut       = 0.30f;
    float    tabTransition = 0.30f;

    EasingFn hoverEasing         = Easing::QuadOut;
    EasingFn pressEasing         = Easing::QuadIn;
    EasingFn checkEasing         = Easing::CubicOut;
    EasingFn tabEasing           = Easing::CubicInOut;
    EasingFn fadeEasing          = Easing::QuadOut;
    EasingFn glowEasing          = Easing::ExpoOut;
    EasingFn tabTransitionEasing = Easing::CubicInOut;
};
#pragma endregion