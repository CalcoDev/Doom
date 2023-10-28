#include "game.h"

#include <math.h>

const static u8 MAP_DATA[MAP_W * MAP_H] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 3, 0, 0, 0, 2, 2, 2, 0, 0, 0, 3, 0, 0, 1,
    1, 0, 3, 0, 0, 2, 2, 0, 2, 0, 3, 3, 3, 0, 0, 1,
    1, 0, 3, 0, 0, 0, 2, 0, 2, 0, 0, 0, 3, 0, 0, 1,
    1, 0, 3, 0, 0, 0, 0, 0, 2, 0, 0, 3, 3, 3, 0, 1,
    1, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

const static u32 COLOUR_DATA[4] = {
  0xFF2b2b2b,
  0xFF03bafc,
  0xFFa83240,
  0xFFa8327f
};

const static u32 PLAYER_COLOUR = 0xFF0000FF;
static f32 PLAYER_SPEED = 0.05f;
static f32 PLAYER_SENS = 0.05f;

State state;

void move_player(v2f movement)
{
  v2f npos = {
    state.player.pos.x + movement.x,
    state.player.pos.y + movement.y,
  };

  v2i mpos = {
    (i32)state.player.pos.x,
    (i32)state.player.pos.y
  };
  v2i mnpos = {
    (i32)npos.x,
    (i32)npos.y
  };

  // Try move X
  if (MAP_DATA[mpos.y * MAP_W + mnpos.x] == 0)
    state.player.pos.x += movement.x;
  
  // Try move Y
  if (MAP_DATA[mnpos.y * MAP_W + mpos.x] == 0)
    state.player.pos.y += movement.y;
}

State* game_get(void)
{
  return &state;
}

void game_init(void)
{
}

void game_update(void)
{
  if (GetKeyPressed(GLFW_KEY_F3))
    state.show_debug_ui = !state.show_debug_ui;
  
  ClearPixels();
}

void game_debug_ui(void)
{
  igBegin("Debug Window", NULL, ImGuiWindowFlags_AlwaysAutoResize);
  igEnd();
}

void game_free(void) 
{

}

void ClearPixels(void)
{
  for (i32 i = 0; i < VIEWPORT_W * VIEWPORT_H; ++i)
    state.pixels[i] = 0x00000000;
}
void SetPixel(i32 x, i32 y, u32 colour)
{
  if (x < 0 || y < 0 || x >= VIEWPORT_W || y >= VIEWPORT_H)
    return;

  y = VIEWPORT_H - 1 - y;
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