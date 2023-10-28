#ifndef MATH_H
#define MATH_H

#include "defines.h"

typedef union v2f
{
  f32 v[2];
  struct 
  {
    f32 x;
    f32 y;
  };
} v2f;

typedef struct v2i
{
  i32 x;
  i32 y;
} v2i;

typedef struct v2u
{
  u32 x;
  u32 y;
} v2u;

#define v2_sqr_mag(a) ((a.x) * (a.x) + (a.y) * (a.y))
#define v2_mag(a)     sqrtf(v2_sqr_mag(a))
#define v2_norm(a)                                                             \
  (v2f) { a.x / v2_mag(a), a.y / v2_mag(a) }
#define v2_dot(a, b) ((a.x) * (b.x) + (a.y) * (b.y))

#define normalize(u) ({              \
        const v2f _u = (u);           \
        const f32 l = v2_mag(_u);    \
        (v2f) { _u.x / l, _u.y / l }; \
    })

#define v2_i(a) (v2i){(i32)(a.x), (i32)(a.y)}
#define v2_u(a) (v2u){(u32)(a.x), (u32)(a.y)}
#define v2_f(a) (v2f){(f32)(a.x), (f32)(a.y)}

#define f_PI 3.1415926535f
#define f_deg_to_rad(degrees) ((degrees) * (f_PI / 180.f))
#define f_rad_to_deg(radians) ((radians) * (180.f/ f_PI))

#define sign(a) ({                                       \
        __typeof__(a) _a = (a);                          \
        (__typeof__(a))(_a < 0 ? -1 : (_a > 0 ? 1 : 0)); \
    })

v2f v2f_rotate(v2f vec, f32 angle);

#endif
