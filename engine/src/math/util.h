#pragma once

static inline float maxf(float a, float b) {
    return (a > b) ? a : b;
}

static inline float minf(float a, float b) {
    return (a < b) ? a : b;
}

static inline float lerpf(float v0, float v1, float t) {
    return (1.f - t) * v0 + t * v1;
}