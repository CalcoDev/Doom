#ifndef GAME_H
#define GAME_H

#include "cimgui_include.h"
#include "defines.h"
#include "math.h"

#define FRAMERATE 0.016667f
#define WINDOW_W 1280
#define WINDOW_H 720

#define VIEWPORT_W 320
#define VIEWPORT_H 180

#define MAP_W 16
#define MAP_H 9

typedef struct State
{
  // Rendering
  GLFWwindow* window;
  u32 glfw_texture;
  u32 pixels[WINDOW_W * WINDOW_H];

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
  struct Player {
    v2f pos;
    v2f forward;
    f32 fov;
  } player;
} State;

State* game_get(void);

void game_init(void);
void game_update(void);
void game_debug_ui(void);
void game_free(void);

b8 GetKeyPressed(u16 key);
b8 GetKeyReleased(u16 key);
b8 GetKeyDown(u16 key);
b8 GetKeyUp(u16 key);

void ClearPixels(void);
void SetPixel(u32 x, u32 y, u32 colour);

#endif
