#ifndef GAME_H
#define GAME_H

#include <glfw/glfw3.h>
#include "defines.h"
#include "math.h"

#define FRAMERATE 0.016667f
#define WINDOW_W 1280
#define WINDOW_H 720

#define VIEWPORT_W 320
#define VIEWPORT_H 180

#define MAP_W 8
#define MAP_H 8

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
  v2f pos;
  v2f look_dir;
  v2f plane;
} State;
State state;

void game_init();
void game_update();
void game_debug_ui();
void game_free();

b8 GetKeyPressed(u16 key);
b8 GetKeyReleased(u16 key);
b8 GetKeyDown(u16 key);
b8 GetKeyUp(u16 key);

void ClearPixels();
void SetPixel(u32 x, u32 y, u32 colour);

#endif