#ifndef CETECH_MATH_TYPES_H
#define CETECH_MATH_TYPES_H

//==============================================================================
// Includes
//==============================================================================

#include "../types.h"


//==============================================================================
// Vectors
//==============================================================================

typedef struct {
    union {
        f32 f[2];
        struct {
            f32 x;
            f32 y;
        };
    };
} vec2f_t;


typedef struct {
    union {
        f32 f[3];
        struct {
            f32 x;
            f32 y;
            f32 z;
        };
    };
} vec3f_t;

typedef struct {
    union {
        f32 f[4];
        struct {
            f32 x;
            f32 y;
            f32 z;
            f32 w;
        };
    };
} vec4f_t;

//==============================================================================
// Quaternion
//==============================================================================

typedef vec4f_t quatf_t;


//==============================================================================
// Matrix
//==============================================================================

typedef struct {
    union {
        f32 f[3 * 3];
        struct {
            vec3f_t x;
            vec3f_t y;
            vec3f_t z;
        };
    };
} mat33f_t;


typedef struct {
    union {
        f32 f[4 * 4];
        struct {
            vec4f_t x;
            vec4f_t y;
            vec4f_t z;
            vec4f_t w;
        };
    };
} mat44f_t;

#endif //CETECH_MATH_TYPES_H