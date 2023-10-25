#include "game.h"

b8 GetKeyPressed(u16 key)
{
  return state.curr_keys[key] && !state.prev_keys[key];
}
b8 GetKeyReleased(u16 key)
{
  return !state.curr_keys[key] && state.prev_keys[key];
}
b8 GetKeyDown(u16 key) { return state.curr_keys[key]; }
b8 GetKeyUp(u16 key) { return !state.curr_keys[key]; }

void ClearPixels(void)
{
  for (u32 i = 0; i < VIEWPORT_W * VIEWPORT_H; ++i)
    state.pixels[i] = 0x00000000;
}

void SetPixel(u32 x, u32 y, u32 colour)
{
  state.pixels[y * VIEWPORT_W + x] = colour;
}
