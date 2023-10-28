#ifndef GAME_H
#define GAME_H

#include "cimgui_include.h"
#include "defines.h"
#include "dmath.h"

#include "entity.h"

#define FRAMERATE 0.016667f
#define WINDOW_W 1280
#define WINDOW_H 720

#define VIEWPORT_W 320
#define VIEWPORT_H 180

#define MAP_W 16
#define MAP_H 9

#define TEX_COUNT 4
#define TEX_W 64
#define TEX_H 64

#define ENTITY_COUNT 64

typedef struct State
{
  // Rendering
  GLFWwindow* window;
  u32 glfw_texture;
  u32 pixels[VIEWPORT_W * VIEWPORT_H];
  f32 z_buffer[VIEWPORT_W];

  // Time
  f32 prev_time;
  f32 curr_time;

  // UI
  b8 show_debug_ui;
  ImGuiContext* imgui_ctx;

  // Input
  b8 prev_keys[GLFW_KEY_LAST];
  b8 curr_keys[GLFW_KEY_LAST];

  // Game
  struct Player
  {
    v2f pos;
    v2f dir;
    v2f plane;
  } player;

  struct Texture
  {
    u32 data[TEX_W * TEX_H];
  } textures[TEX_COUNT];

  Entity entities[ENTITY_COUNT];
  i32 entity_idx;
} State;

State* game_get(void);

void game_add_entity(Entity entity);

void game_init(void);
void game_update(void);
void game_debug_ui(void);
void game_free(void);

b8 GetKeyPressed(u16 key);
b8 GetKeyReleased(u16 key);
b8 GetKeyDown(u16 key);
b8 GetKeyUp(u16 key);

void ClearPixels(void);
void set_pixel(i32 x, i32 y, u32 colour);

#endif
