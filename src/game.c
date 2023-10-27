#include "game.h"

const static u8 MAP_DATA[MAP_W * MAP_H] = {
    1, 1, 1, 1, 1, 1, 1, 1, //
    1, 0, 0, 0, 5, 0, 0, 1, //
    1, 0, 2, 2, 0, 3, 0, 1, //
    1, 0, 3, 0, 0, 4, 0, 1, //
    1, 0, 0, 0, 0, 0, 0, 1, //
    1, 0, 0, 0, 0, 0, 0, 1, //
    1, 0, 0, 0, 0, 0, 0, 1, //
    1, 1, 1, 1, 1, 1, 1, 1, //
};

State state;

State* game_get(void)
{
  return &state;
}

void game_init()
{
  state.pos = (v2f){MAP_W / 2, MAP_H / 2};
  state.look_dir = (v2f){-1, 0};
  state.plane = (v2f){0, 0.66};
}

void game_update()
{
  if (GetKeyPressed(GLFW_KEY_F3))
    state.show_debug_ui = !state.show_debug_ui;

  ClearPixels();
}

void game_debug_ui()
{
  igBegin("Debug Window", NULL, 0);
  igEnd();
}

void game_free() 
{

}

void ClearPixels()
{
  for (u32 i = 0; i < VIEWPORT_W * VIEWPORT_H; ++i)
    state.pixels[i] = 0x00000000;
}
void SetPixel(u32 x, u32 y, u32 colour)
{
  state.pixels[y * VIEWPORT_W + x] = colour;
}

b8 GetKeyPressed(u16 key)
{
  return state.curr_keys[key] && !state.prev_keys[key];
}
b8 GetKeyReleased(u16 key)
{
  return !state.curr_keys[key] && state.prev_keys[key];
}
b8 GetKeyDown(u16 key) 
{ 
  return state.curr_keys[key]; 
}
b8 GetKeyUp(u16 key) 
{ 
  return !state.curr_keys[key]; 
}