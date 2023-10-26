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

#endif
