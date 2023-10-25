#ifndef MATH_H
#define MATH_H

typedef struct v2f
{
  f32 x;
  f32 y;
} v2f;

typedef struct v2i
{
  i32 x;
  i32 y;
} v2i;

#define v2_sqr_mag(a) ((a.x) * (a.x) + (a.y) * (a.y))
#define v2_mag(a)     sqrtf(v2_sqr_mag(a))
#define v2_norm(a)                                                             \
  (v2f) { a.x / v2_mag(a), a.y / v2_mag(a) }
#define v2_dot(a, b) ((a.x) * (b.x) + (a.y) * (b.y))

#endif
