#ifndef CETECH_VEC3F_H
#define CETECH_VEC3F_H

#include <math.h>

static inline void vec3f_set(float* restrict result, const float* restrict a) {
    result[0] = a[0];
    result[1] = a[1];
    result[2] = a[2];
}

static inline void vec3f_add(float* restrict result, const float* restrict a, const float* restrict b) {
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
}

static inline void vec3f_sub(float* restrict result, const float* restrict a, const float* restrict b) {
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
}

static inline void vec3f_mul(float* restrict result, const float* restrict a, const float s) {
    result[0] = a[0] * s;
    result[1] = a[1] * s;
    result[2] = a[2] * s;
}

static inline void vec3f_div(float* restrict result, const float* restrict a, const float s) {
    result[0] = a[0] / s;
    result[1] = a[1] / s;
    result[2] = a[2] / s;
}

static inline float vec3f_dot(const float* restrict a, const float* restrict b) {
    return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

static inline float vec3f_length_squared(const float* restrict a) {
    return (a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]);
}

static inline float vec3f_length(const float* restrict a) {
    return sqrtf(vec3f_length_squared(a));
}

static inline void vec3f_normalized(float* restrict result, const float* restrict a) {
    const float inv_length = 1.0f/vec3f_length(a);

    vec3f_mul(result, a, inv_length);
}

#endif //CETECH_VEC3F_H
