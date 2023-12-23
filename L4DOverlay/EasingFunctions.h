#pragma once

#include <cmath>

class EasingFunctions final {
public:
    static float Linear(float x) {
        return x;
    }

    static float EaseInCubic(float x) {
        return x * x * x;
    }

    static float EaseOutCubic(float x) {
        return 1 - powf(1 - x, 3);
    }

    static float EaseInOutCubic(float x) {
        return x < 0.5 ? 4 * x * x * x : 1 - powf(-2 * x + 2, 3) / 2;
    }
};
