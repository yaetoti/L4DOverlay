#pragma once

class EasingFunctions final {
public:
    static float Linear(float x);
    static float EaseInCubic(float x);
    static float EaseOutCubic(float x);
    static float EaseInOutCubic(float x);
};
