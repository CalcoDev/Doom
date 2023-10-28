#include "game.h"

#include <math.h>

const static u8 MAP_DATA[MAP_W * MAP_H] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 3, 0, 0, 0, 2, 2, 2, 0, 0, 0, 3, 0, 0, 1,
    1, 0, 3, 0, 0, 0, 2, 0, 2, 0, 3, 3, 3, 0, 0, 1,
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
b8 debug_view;

void game_init(void)
{
  state.player.pos = (v2f) {4, 2};
  state.player.dir = (v2f) {0, -1};
  state.player.plane = (v2f) {0.66, 0};
}

void move(f32 amount)
{
  f32 x = state.player.dir.x * amount;
  f32 y = state.player.dir.y * amount;

  v2f npos = { state.player.pos.x + x, state.player.pos.y + y };
  v2i mpos = { (i32)state.player.pos.x, (i32)state.player.pos.y };
  v2i mnpos = { (i32)npos.x, (i32)npos.y };

  if (MAP_DATA[mpos.y * MAP_W + mnpos.x] == 0)
    state.player.pos.x += x;
  if (MAP_DATA[mnpos.y * MAP_W + mpos.x] == 0)
    state.player.pos.y += y;
}

void rotate(f32 angle)
{
  v2f d = state.player.dir;
  v2f p = state.player.plane;

  state.player.dir.x = d.x * cosf(angle) - d.y * sinf(angle);
  state.player.dir.y = d.x * sinf(angle) + d.y * cosf(angle);
  state.player.plane.x = p.x * cosf(angle) - p.y * sinf(angle);
  state.player.plane.y = p.x * sinf(angle) + p.y * cosf(angle);
}

v2i viewport_to_map(v2f p)
{
  return (v2i) {
    (i32)(p.x / VIEWPORT_W * MAP_W),
    (i32)(p.y / VIEWPORT_H * MAP_H)
  };
}

v2i map_to_viewport(v2f p)
{
  return (v2i) {
    (i32)(p.x / MAP_W * VIEWPORT_W),
    (i32)(p.y / MAP_H * VIEWPORT_H)
  };
}

void DrawLine(v2i p, v2i d, u32 colour)
{
  i32 steps = abs(d.x) > abs(d.y) ? abs(d.x) : abs(d.y);
  f32 xIncrement = (f32)d.x / (f32)steps;
  f32 yIncrement = (f32)d.y / (f32)steps;
  f32 x = (f32) p.x;
  f32 y = (f32) p.y;

  for (i32 i = 0; i <= steps; i++) {
      SetPixel((i32)x, (i32)y, colour);
      x += xIncrement;
      y += yIncrement;
  }
}

void render_debug()
{
  // Draw map
  for (i32 y = 0; y < VIEWPORT_H; ++y)
  {
    for (i32 x = 0; x < VIEWPORT_W; ++x)
    {
      v2i pos = viewport_to_map((v2f) {x, y});
      u8 wall = MAP_DATA[pos.y * MAP_W + pos.x];
      u32 colour = COLOUR_DATA[wall];
      SetPixel(x, y, colour);
    }
  }
  
  v2i player_pos = map_to_viewport(state.player.pos);

  // Draw look dir
  const f32 scale = 10.f;
  v2i dir_off = {
    (i32)(state.player.dir.x * scale),
    (i32)(state.player.dir.y * scale)
  };
  DrawLine(player_pos, dir_off, 0xFF00FF00);

  // Draw look plane
  v2i plane_off = {
    (i32)(state.player.plane.x * scale),
    (i32)(state.player.plane.y * scale)
  };
  v2i plane_pos = {
    player_pos.x + dir_off.x - plane_off.x / 2,
    player_pos.y + dir_off.y - plane_off.y / 2
  };
  DrawLine(plane_pos, plane_off, 0xFFFF0000);

  // Draw player
  SetPixel(player_pos.x, player_pos.y, 0xFF0000FF);
}

void render_raycast()
{

}

void game_update(void)
{
  if (GetKeyPressed(GLFW_KEY_F3))
    state.show_debug_ui = !state.show_debug_ui;
  
  if (GetKeyDown(GLFW_KEY_A))
    rotate(-PLAYER_SENS);
  if (GetKeyDown(GLFW_KEY_D))
    rotate(+PLAYER_SENS);
  
  if (GetKeyDown(GLFW_KEY_W))
    move(+PLAYER_SPEED);
  if (GetKeyDown(GLFW_KEY_S))
    move(-PLAYER_SPEED);
  
  ClearPixels();
  if (debug_view)
    render_debug();
  else
    render_raycast();
}

void game_debug_ui(void)
{
  igBegin("Debug Window", NULL, ImGuiWindowFlags_AlwaysAutoResize);

  igCheckbox("Debug View", &debug_view);

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

State* game_get(void)
{
  return &state;
}