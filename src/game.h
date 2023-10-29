#ifndef GAME_H
#define GAME_H

#include "miniaudio.h"

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
#define SOUND_SOURCE_COUNT 16
#define SOUND_INSTANCE_COUNT 128

#define SOUND_WORLD_SCALE 100

typedef struct SoundSource
{
  char* path;
  ma_sound source;
} SoundSource;

typedef struct State
{
  // Rendering
  GLFWwindow* window;
  u32 glfw_texture;
  u32 pixels[VIEWPORT_W * VIEWPORT_H];
  f32 z_buffer[VIEWPORT_W];

  // Audio
  ma_engine audio_engine;

  // Works because miniaudio auto refcounts the sound sources
  SoundSource sound_sources[SOUND_SOURCE_COUNT];
  ma_sound sounds[SOUND_INSTANCE_COUNT];
  i32 sound_source_idx;
  i32 sound_idx;

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
  i32 texture_idx;

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

i32 load_texture(char* path);
#define load_texture_idx(path, index) load_texture_internal(path, sizeof(path) / 1, index)
void load_texture_internal(char* path, i32 size, i32 idx);

void draw_texture(i32 x, i32 y, u32 w, u32 h, u32* pixels);

i32 load_sound(char* path);
i32 play_sound(i32 index, b8 spatial);

void set_sound_pos(i32 idx, v2f pos);
void set_sound_dir(i32 idx, v2f dir);

#endif
