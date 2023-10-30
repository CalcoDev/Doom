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

#define FONT_GLYPH_W 8
#define FONT_GLYPH_H 8

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

  u32 font_tex[128 * 128];

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
  v2f prev_mouse;
  v2f curr_mouse;

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
#define load_texture_idx(path, index) load_texture_internal(path, (v2i) {TEX_W, TEX_H}, &state.textures[index].data)
void load_texture_internal(char* path, v2i tex_size, u32* pixels);

void draw_texture_idx(v2i pos, v2f scale, i32 idx, u32 modulate);
void draw_texture_rect_screen(v2i pos, v2i tex_a, v2i tex_b, v2i tex_size, v2f scale, u32* pixels, u32 modulate);
void draw_texture_rect_src(u32* dest, v2i dest_size, v2i pos, v2i tex_a, v2i tex_b, v2i tex_size, v2f scale, u32* pixels, u32 modulate);

void draw_font_char(v2i pos, f32 z, u32 colour, char c);
void draw_font_str(v2i pos, f32 z, u32 colour, char* str);
i32 get_font_width(char* str);

i32 load_sound(char* path);
i32 play_sound(i32 index, b8 spatial);
void set_sound_pos(i32 idx, v2f pos);
void set_sound_dir(i32 idx, v2f dir);

#endif
