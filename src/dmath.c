#include "dmath.h"

#include <math.h>

v2f v2f_rotate(v2f vec, f32 angle)
{
  return (v2f) {
    vec.x * cosf(angle) - vec.y * sinf(angle),
    vec.x * sinf(angle) + vec.y * cosf(angle)
  };
}

f32 f_sign(f32 n)
{
  return n > 0.f ? 1.f : -1.f;
}