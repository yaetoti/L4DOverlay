#include "EasingFunctions.h"

#include <cmath>

float EasingFunctions::Linear(float x) {
    return x;
}

float EasingFunctions::EaseInCubic(float x) {
    return x * x * x;
}

float EasingFunctions::EaseOutCubic(float x) {
    return 1 - powf(1 - x, 3);
}

float EasingFunctions::EaseInOutCubic(float x) {
    return x < 0.5 ? 4 * x * x * x : 1 - powf(-2 * x + 2, 3) / 2;
}
