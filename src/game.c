#include "game.h"

#include <math.h>
#include "math.h"
#include "defines.h"

#define min(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a < _b ? _a : _b; })
#define max(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a > _b ? _a : _b; })
#define sign(a) ({                                       \
        __typeof__(a) _a = (a);                          \
        (__typeof__(a))(_a < 0 ? -1 : (_a > 0 ? 1 : 0)); \
    })

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
  0xFF000000,
  0xFFa83240,
  0xFFa8327f
};

const static u32 PLAYER_COLOUR = 0xFF0000FF;
static f32 PLAYER_SPEED = 1.f;

State state;
b8 topdown_view;

b8 rendering_visible;

u32 map_to_viewport_x(f32 world) 
{ 
  return (u32)(world / MAP_W * VIEWPORT_W);
}

u32 map_to_viewport_y(f32 world) 
{ 
  return (u32)(world / MAP_H * VIEWPORT_H);
}

u32 viewport_to_map_x(f32 viewport) 
{ 
  return (u32)(viewport / VIEWPORT_W * MAP_W);
}

u32 viewport_to_map_y(f32 viewport) 
{ 
  return (u32)(viewport / VIEWPORT_H * MAP_H);
}

State* game_get(void)
{
  return &state;
}

void game_init(void)
{
  state.pos = (v2f){VIEWPORT_W / 2, VIEWPORT_H / 2};
  state.look_dir = (v2f){0, -15};
  state.plane = (v2f){15, 15};
}

void verline(u32 x, u32 y0, u32 y1, u32 colour) {
  for (u32 y = y0; y <= y1; y++)
    SetPixel(x, y, colour);
}

void game_update(void)
{
  if (GetKeyPressed(GLFW_KEY_F3))
    state.show_debug_ui = !state.show_debug_ui;
  
  if (GetKeyDown(GLFW_KEY_A))
    state.pos.x -= PLAYER_SPEED;
  if (GetKeyDown(GLFW_KEY_D))
    state.pos.x += PLAYER_SPEED;
  if (GetKeyDown(GLFW_KEY_W))
    state.pos.y -= PLAYER_SPEED;
  if (GetKeyDown(GLFW_KEY_S))
    state.pos.y += PLAYER_SPEED;
  
  if (state.pos.x < 0.f)
    state.pos.x = 0.f;
  if (state.pos.y < 0.f)
    state.pos.y = 0.f;
  if (state.pos.x >= VIEWPORT_W)
    state.pos.x = VIEWPORT_W - 0.001f;
  if (state.pos.y >= VIEWPORT_H)
    state.pos.y = VIEWPORT_H - 0.001f;
  
  ClearPixels();
  if (topdown_view)
  {
    for (u32 y = 0; y < VIEWPORT_H; ++y)
    {
      for (u32 x = 0; x < VIEWPORT_W; ++x)
      {
        u32 idx = viewport_to_map_y(y) * MAP_W + viewport_to_map_x(x);
        u32 col = MAP_DATA[idx];
        SetPixel(x, y, COLOUR_DATA[col]);
      }
    }

    // Draw player
    SetPixel(state.pos.x, state.pos.y, PLAYER_COLOUR);
  }
  else 
  {
    // for (u32 x = 0; x < VIEWPORT_W; ++x)
    // {
    // }
  }
}

void game_debug_ui(void)
{
  igBegin("Debug Window", NULL, ImGuiWindowFlags_AlwaysAutoResize);

  igText("Sample debugging menu. \nWill be updated later.");
  
  ImGuiTreeNodeFlags tflags = ImGuiTreeNodeFlags_UpsideDownArrow 
    | ImGuiTreeNodeFlags_DefaultOpen 
    | ImGuiTreeNodeFlags_FramePadding;
  if (igCollapsingHeader_TreeNodeFlags("Rendering Info", tflags))
  {
    igIndent(0);

    {
      igBeginColumns("r_header", 2, ImGuiOldColumnFlags_NoResize | ImGuiOldColumnFlags_NoBorder | ImGuiOldColumnFlags_GrowParentContentsSize);
      igText("Framerate");
      igSameLine(0, -1);
      igText("%.2f", 1.f / (state.curr_time - state.prev_time));

      igNextColumn();
      igText("Time Passed");
      igSameLine(0, -1);
      igText("%.2f", state.curr_time);
      igEndColumns();
    }

    // Render options
    {
      igCheckbox("Top Down View", &topdown_view);
    }

    igUnindent(0);
  }

  if (igCollapsingHeader_TreeNodeFlags("Player Info", tflags))
  {
    igIndent(0);
    {
      igInputFloat("Move Speed", &PLAYER_SPEED, 0.1f, 1.f, "%.2f", ImGuiInputTextFlags_None);
      igInputFloat2("Look Direction", state.look_dir.v, "%.2f", ImGuiInputTextFlags_None);
      igInputFloat2("Screen Plane Offset", state.plane.v, "%.2f", ImGuiInputTextFlags_None);
    }
    igUnindent(0);
  }

  igEnd();
}

void game_free(void) 
{

}

void ClearPixels(void)
{
  for (u32 i = 0; i < VIEWPORT_W * VIEWPORT_H; ++i)
    state.pixels[i] = 0x00000000;
}
void SetPixel(u32 x, u32 y, u32 colour)
{
  if (x < 0 || y < 0 || x >= VIEWPORT_W || y >= VIEWPORT_H)
    return;

  // Invert y
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