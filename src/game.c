#include "game.h"

#include <math.h>
#include <memory.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "os_utils.h"

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

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

// Map the indices ... to other indices
const static i32 TEXTURE_DATA[4] = {
  0,
  1,
  1,
  1,
};

const static u32 PLAYER_COLOUR = 0xFF0000FF;
static f32 PLAYER_SPEED = 0.05f;
static f32 PLAYER_SENS = 0.005f;

State state;
i32 entity_update_indices[ENTITY_COUNT];

b8 debug_view;
b8 topdown_raycast;

void concat(char* str, char* suffix, i32 offset, i32 length)
{
  memcpy(str + offset, suffix, length);
  *(str + offset + length) = '\0';
}

// TODO(calco): remove
void game_init(void)
{
  for (i32 e = 0; e < ENTITY_COUNT; ++e)
    entity_update_indices[e] = e;

  // Font
  load_texture_internal(".\\assets\\font.png", (v2i) {128, 128}, &state.font_tex);

  // Audio engine
  ma_result result;
  result = ma_engine_init(NULL, &state.audio_engine);

  i32 idx = load_sound("C:\\Users\\calco\\Documents\\Calcopod\\Development\\OpenGL\\doom\\assets\\intro.wav");

  // Player Data
  state.player.pos = (v2f) {4, 2};
  state.player.dir = (v2f) {0, -1};
  state.player.plane = (v2f) {0.66, 0};

  // Textures
  load_texture(".\\assets\\wall_tex.png");
  load_texture(".\\assets\\fiipestil.png");
  load_texture(".\\assets\\gun.png");

  // Entites
  Entity fiipestil = {0};
  fiipestil.flags = EntityFlag_Sprite;
  fiipestil.position = (v2f){5, 5};
  fiipestil.scale = (v2f){1, 1};
  fiipestil.z_transform = 10.f;
  fiipestil.sprite_idx = 1;
  fiipestil.tex_pos = (v2i) {0, 0};
  fiipestil.tex_off = (v2i) {64, 64};
  game_add_entity(fiipestil);

  load_texture(".\\assets\\win_text.png");
  Entity win_text = {0};
  win_text.flags = EntityFlag_Sprite;
  win_text.position = (v2f){4, 4};
  win_text.scale = (v2f){1, 1};
  win_text.z_transform = 10.f;
  win_text.sprite_idx = 3;
  win_text.tex_pos = (v2i) {0, 0};
  win_text.tex_off = (v2i) {64, 64};
  game_add_entity(win_text);

  state.show_debug_ui = 1;
}

void move(v2f dir, f32 amount)
{
  f32 x = dir.x * amount;
  f32 y = dir.y * amount;

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

void draw_line(v2i p, v2i d, u32 colour)
{
  i32 steps = abs(d.x) > abs(d.y) ? abs(d.x) : abs(d.y);
  f32 xIncrement = (f32)d.x / (f32)steps;
  f32 yIncrement = (f32)d.y / (f32)steps;
  f32 x = (f32) p.x;
  f32 y = (f32) p.y;

  for (i32 i = 0; i <= steps; i++) {
      set_pixel((i32)x, (i32)y, colour);
      x += xIncrement;
      y += yIncrement;
  }
}

void draw_vline(i32 x, i32 y0, i32 y1, u32 colour)
{
  for (int y = y0; y <= y1; y++)
    set_pixel(x, y, colour);
}

int entity_dist_sort(const void* v1, const void* v2)
{
  Entity* e1 = &state.entities[*((const i32*)v1)];
  Entity* e2 = &state.entities[*((const i32*)v2)];

  // TODO(calco): Should cache this but meh
  v2f dvec1 = {
    state.player.pos.x - e1->position.x,
    state.player.pos.y - e1->position.y,
  };
  v2f dvec2 = {
    state.player.pos.x - e2->position.x,
    state.player.pos.y - e2->position.y,
  };

  f32 d1 = v2_sqr_mag(dvec1);
  f32 d2 = v2_sqr_mag(dvec2);

  if (d1 > d2)
    return -1;
  if (d2 > d1)
    return 1;
  return 0;
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
      set_pixel(x, y, colour);
    }
  }
  
  v2i player_pos = map_to_viewport(state.player.pos);

  // Draw look dir
  const f32 scale = 10.f;
  v2i dir_off = {
    (i32)(state.player.dir.x * scale),
    (i32)(state.player.dir.y * scale)
  };
  draw_line(player_pos, dir_off, 0xFF00FF00);

  // Draw look plane
  v2i plane_off = {
    (i32)(state.player.plane.x * scale),
    (i32)(state.player.plane.y * scale)
  };
  v2i plane_pos = {
    player_pos.x + dir_off.x - plane_off.x / 2,
    player_pos.y + dir_off.y - plane_off.y / 2
  };
  draw_line(plane_pos, plane_off, 0xFFFF0000);

  // Draw player
  set_pixel(player_pos.x, player_pos.y, 0xFF0000FF);
}

void do_sprites()
{
  if (state.entity_idx < 0)
    return;

  qsort(&entity_update_indices, state.entity_idx, sizeof(entity_update_indices[0]), entity_dist_sort);

  for (i32 e = 0; e < state.entity_idx; ++e)
  {
    Entity* entity = &state.entities[entity_update_indices[e]];
    if (entity->flags & EntityFlag_Sprite == 0)
      continue;

    v2f ppos = state.player.pos;
    v2f pplane = state.player.plane;
    v2f pdir = state.player.dir;
    
    // Translate relative to camera
    v2f epos = {
      entity->position.x - ppos.x,
      entity->position.y - ppos.y,
    };
    // Projection matrix lol
    f32 det_inverse = 1.f / (pplane.x * pdir.y - pdir.x * pplane.y);
    
    v2f etransform = {
      det_inverse * (pdir.y * epos.x - pdir.x * epos.y),
      det_inverse * (-pplane.y * epos.x + pplane.x * epos.y)
    };

    i32 escreenx = (i32)((VIEWPORT_W / 2) * (1.f + etransform.x / etransform.y));

    i32 evertmove = (i32)(-entity->z_transform / etransform.y);
    
    v2i tex_off = entity->tex_pos;
    v2i tex_sth = entity->tex_off;

    i32 eheight = abs((int)(VIEWPORT_H / etransform.y)) * entity->scale.y;
    i32 y0 = max(0, (-eheight / 2 + VIEWPORT_H / 2 + evertmove));
    i32 y1 = min((eheight / 2 + VIEWPORT_H / 2 + evertmove), VIEWPORT_H - 1);

    i32 ewidth = abs((int)(VIEWPORT_H / etransform.y)) * entity->scale.x;
    i32 x0 = max(0, (-ewidth / 2 + escreenx));
    i32 x1 = min((ewidth / 2 + escreenx), VIEWPORT_W - 1);

    // Loop and render
    for (i32 stripe = x0; stripe < x1; ++stripe)
    {
      i32 tex_x = (int)(256 * ((stripe - (-ewidth / 2 + escreenx)) * tex_sth.x) / ewidth) / 256;
      if (etransform.y > 0 && stripe > 0 && stripe < VIEWPORT_W && etransform.y < state.z_buffer[stripe])
      {
        for (i32 sy = y0; sy < y1; ++sy)
        {
          i32 d = (sy-evertmove) * 256 - VIEWPORT_H * 128 + eheight * 128;
          i32 tex_y = ((d * tex_sth.y) / eheight) / 256;
          u32 colour = state.textures[entity->sprite_idx].data[(tex_y + tex_off.y) * TEX_W + (tex_x + tex_off.x)];
          if ((colour >> 24) & 0xFF != 0) // non transparent
            set_pixel(stripe, sy, colour);
        }
      }
    }
  }
}

void render_raycast()
{
  // Draw walls
  for (i32 x = 0; x < VIEWPORT_W; ++x)
  {
    f32 norm_x = ((f32)x / VIEWPORT_W) * 2.f - 1.f;
    v2f dir = {
      state.player.dir.x + state.player.plane.x * norm_x,
      state.player.dir.y + state.player.plane.y * norm_x,
    };

    v2f pos = state.player.pos;
    v2i ipos = v2_i(pos);
    v2i step = { (i32)f_sign(dir.x), (i32)f_sign(dir.y) };

    v2f delta_dist = {
      fabsf(dir.x) < 1e-20 ? 1e30 : fabsf(1.0f / dir.x),
      fabsf(dir.y) < 1e-20 ? 1e30 : fabsf(1.0f / dir.y)
    };
    v2f side_dist = {
      delta_dist.x * (dir.x < 0 ? (pos.x - ipos.x) : (ipos.x + 1 - pos.x)),
      delta_dist.y * (dir.y < 0 ? (pos.y - ipos.y) : (ipos.y + 1 - pos.y))
    };

    struct dda_hit
    {
      b8 hit;
      b8 yside;
      u32 wall;
    } hit = {0};

    while (!hit.hit)
    {
      if (side_dist.x < side_dist.y) 
      {
        side_dist.x += delta_dist.x;
        ipos.x += step.x;
        hit.yside = 0;
      }
      else 
      {
        side_dist.y += delta_dist.y;
        ipos.y += step.y;
        hit.yside = 1;
      }

      b8 wall = MAP_DATA[ipos.y * MAP_W + ipos.x];
      if (wall != 0)
      {
        hit.hit = 1;
        hit.wall = wall;
      }
    }

    f32 dist = !hit.yside ? 
      (side_dist.x - delta_dist.x) : (side_dist.y - delta_dist.y);
    
    state.z_buffer[x] = dist;

    i32 line_height = (i32)(VIEWPORT_H / dist);
    i32 y0 = max((VIEWPORT_H / 2) - (line_height / 2), 0);
    i32 y1 = min((VIEWPORT_H / 2) + (line_height / 2), VIEWPORT_H - 1);

    // unhardcode later
    i32 tex_num = 0;
    
    // the exact coords where the wall was hit
    f32 wall_x = hit.yside ? pos.x + dist * dir.x : pos.y + dist * dir.y;
    wall_x -= (f32)((i32)wall_x);

    // X texture coordinate
    i32 tex_x = (int)(wall_x * TEX_W);
    if(hit.yside == 0 && dir.x > 0 || hit.yside == 1 && dir.y < 0)
      tex_x = TEX_W - tex_x - 1;

    // Get the Y texture coord
    f32 tex_step = TEX_H / (f32)line_height;

    // TODO(calco): Maybe cast these to floats
    f32 tex_y_pos = (y0 - VIEWPORT_H / 2 + line_height / 2) * tex_step;
    for (i32 dy = y0; dy < y1; ++dy)
    {
      i32 tex_y = (int)tex_y_pos & (TEX_H - 1);
      tex_y_pos += tex_step;
      u32 colour = state.textures[tex_num].data[tex_y * TEX_W + tex_x];
      if (hit.yside)
      {
        u32 br = ((colour & 0xFF00FF) * 0xC0) >> 8;
        u32 g  = ((colour & 0x00FF00) * 0xC0) >> 8;
        colour = 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
      }
      set_pixel(x, dy, colour);
    }

    draw_vline(x, 0, y0, 0xFF202020);
    draw_vline(x, y1, VIEWPORT_H - 1, 0xFF505050);
  }

  do_sprites();

  draw_texture_idx(
    (v2i) {
      VIEWPORT_W - 1 - 64, 
      VIEWPORT_H - 1 - 64
    },
    (v2f) {1, 1},
    2, 0xFFFFFFFF
  );

  // draw_font_char((v2i) {20, 20}, 0.f, 0xFFA23F62, 'L');
  // draw_font_char((v2i) {40, 20}, 0.f, 0xFFFFFFFF, 'W');

  // draw_font_str(
  //   (v2i) {40, 40}, 0.f, 0xFF0000FF,
  //   "TEXT YOOO"
  // );

  // draw_font_str(
  //   (v2i) {120, 80}, 0.f, 0xFF0000FF,
  //   "Click here to buy!!!"
  // );
}

void game_update(void)
{
  if (GetKeyPressed(GLFW_KEY_F3))
  {
    state.show_debug_ui = !state.show_debug_ui;

    if (!state.show_debug_ui)
      glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
      glfwSetInputMode(state.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  
  if (GetKeyDown(GLFW_KEY_W))
    move(state.player.dir, +PLAYER_SPEED);
  if (GetKeyDown(GLFW_KEY_A))
    move(state.player.plane, -PLAYER_SPEED);
  if (GetKeyDown(GLFW_KEY_S))
    move(state.player.dir, -PLAYER_SPEED);
  if (GetKeyDown(GLFW_KEY_D))
    move(state.player.plane, +PLAYER_SPEED);
  
  if (!state.show_debug_ui)
    rotate((state.curr_mouse.x - state.prev_mouse.x) * PLAYER_SENS);
  
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
  
  if (!debug_view)
  {
    igText("Framerate: %.4f", 1.f / (state.curr_time - state.prev_time));

    igInputFloat("Player Sensitivity", &PLAYER_SENS, 0.01f, 0.1f, "%.4f", ImGuiTextFlags_None);

    igCheckbox("topdown Raycast", &topdown_raycast);

    igInputFloat2("Scale", state.entities[0].scale.v, "%.2f", ImGuiTextFlags_None);
    igInputFloat2("Position", state.entities[0].position.v, "%.2f", ImGuiTextFlags_None);
    igInputFloat("Z Transform", &state.entities[0].z_transform, 0.1f, 1.f, "%.2f", ImGuiTextFlags_None);

    igInputInt2("Tex Pos", state.entities[0].tex_pos.v, ImGuiTextFlags_None);
    igInputInt2("Tex Off", state.entities[0].tex_off.v, ImGuiTextFlags_None);
  }

  igEnd();
}

void game_free(void) 
{
  for (i32 s = 0; s < state.sound_source_idx; ++s)
    ma_sound_uninit(&state.sound_sources[s].source);
  
  // Technically unnecessary as all the sources were already removed.
  for (i32 s = 0; s < state.sound_idx; ++s)
    ma_sound_uninit(&state.sounds[s]);

  ma_engine_uninit(&state.audio_engine);
}

void game_add_entity(Entity entity)
{
  state.entities[state.entity_idx++] = entity;
}

void ClearPixels(void)
{
  for (i32 i = 0; i < VIEWPORT_W * VIEWPORT_H; ++i)
    state.pixels[i] = 0x00000000;
}
void set_pixel(i32 x, i32 y, u32 colour)
{
  state.pixels[y * VIEWPORT_W + x] = colour;
}

void load_texture_internal(char* path, v2i tex_size, u32* pixels)
{
  i32 x, y, comp;
  char pathh[1024];
  get_asset_path(pathh, path, 1024, 20);
  unsigned char* data = stbi_load(pathh, &x, &y, &comp, STBI_rgb_alpha);
  memcpy(pixels, data, tex_size.x * tex_size.y * 4);
  stbi_image_free(data);
}

i32 load_texture(char* path)
{
  i32 idx = state.texture_idx;
  if (idx >= TEX_COUNT)
    tn_logfatal("Ran out of texture slots to fill!");
  state.texture_idx += 1;
  
  load_texture_idx(path, idx);

  return idx;
}

u32 multiply_colors(u32 c1, u32 c2) {
  u8 a1 = (c1 >> 24) & 0xFF;
  u8 b1 = (c1 >> 16) & 0xFF;
  u8 g1 = (c1 >> 8) & 0xFF;
  u8 r1 = c1 & 0xFF;
  u8 a2 = (c2 >> 24) & 0xFF;
  u8 b2 = (c2 >> 16) & 0xFF;
  u8 g2 = (c2 >> 8) & 0xFF;
  u8 r2 = c2 & 0xFF;
  u8 ra = (u8)((a1 * a2) / 255);
  u8 rb = (u8)((b1 * b2) / 255);
  u8 rg = (u8)((g1 * g2) / 255);
  u8 rr = (u8)((r1 * r2) / 255);
  return (ra << 24) | (rb << 16) | (rg << 8) | rr;
}

void draw_texture_rect_src(u32* dest, v2i dest_size, v2i pos, v2i tex_a, v2i tex_b, v2i tex_size, v2f scale, u32* pixels, u32 modulate)
{
  v2f offset = {
    (tex_b.x - tex_a.x) * scale.x,
    (tex_b.y - tex_a.y) * scale.y
  };

  v2f step = {
    min(scale.x, 1.f),
    min(scale.y, 1.f)
  };
  for (f32 y = 0; y < offset.y; y += step.y)
  {
    for (f32 x = 0; x < offset.x; x += step.x)
    {
      u32 colour = pixels[(tex_a.y + (i32)(y / scale.y)) * tex_size.x + (tex_a.x + (i32)(x / scale.x))];
      if ((colour >> 24) & 0xFF != 0)
      {
        colour = multiply_colors(colour, modulate);
        // set_pixel(min(pos.x + (i32)x, VIEWPORT_W-1), min(pos.y + (i32)y, VIEWPORT_H-1), colour);
        i32 fx = min(pos.x + (i32)x, dest_size.x-1);
        i32 fy = min(pos.y + (i32)y, dest_size.y-1);
        dest[fy * dest_size.x + fx] = colour;
      }
    }
  }
}

void draw_texture_rect_screen(v2i pos, v2i tex_a, v2i tex_b, v2i tex_size, v2f scale, u32* pixels, u32 modulate)
{
  draw_texture_rect_src(&state.pixels, (v2i){VIEWPORT_W, VIEWPORT_H}, pos, tex_a, tex_b, tex_size, scale, pixels, modulate);
}

void draw_texture_idx(v2i pos, v2f scale, i32 idx, u32 modulate)
{
  draw_texture_rect_screen(pos, 
    (v2i) {0, 0},
    (v2i) {TEX_W, TEX_H},
    (v2i) {TEX_W, TEX_H},
    scale, &state.textures[idx].data, modulate
  );
}

// TODO(calco):
void draw_font_char(v2i pos, f32 z, u32 colour, char c)
{
  v2i tex_pos = {
    ((i32)(c) % 16) * FONT_GLYPH_W, 
    ((i32)(c) / 16) * FONT_GLYPH_H
  };

  draw_texture_rect_screen(
    pos, tex_pos, 
    (v2i) {
      tex_pos.x + FONT_GLYPH_W, 
      tex_pos.y + FONT_GLYPH_H
    }, 
    (v2i) {128, 128},
    (v2f) {1, 1}, &state.font_tex,
    colour
  );
}

void draw_font_str(v2i pos, f32 z, u32 colour, char* str)
{
  for (i32 c = 0; str[c] != '\0'; ++c)
  {
    draw_font_char((v2i) {
      pos.x + c * FONT_GLYPH_W,
      pos.y
    }, z, colour, str[c]);
  }
}

i32 get_font_width(char* str)
{
  return 1;
}

i32 load_sound(char* path)
{
  i32 idx = state.sound_source_idx;
  state.sound_sources[idx].path = path;
  ma_sound_init_from_file(&state.audio_engine, path, 0, NULL, NULL, &state.sound_sources[idx].source);
  state.sound_source_idx += 1;
}

void make_sound_available(void* user_data, ma_sound* sound)
{
  sound->endCallback = NULL;
}

b8 is_sound_available(i32 index)
{
  return state.sounds[index].endCallback == NULL;
}

i32 play_sound(i32 index, b8 spatial)
{
  i32 idx = state.sound_idx;
  if (!is_sound_available(idx))
  {
    i32 i = idx + 1;
    for (; i != idx && !is_sound_available(i); i = (i + 1) % SOUND_INSTANCE_COUNT);

    if (i == idx)
      tn_logfatal("Ran out of audio sources!");
    
    idx = i;
  }

  tn_log("Assigned instanced index %d", idx);

  SoundSource source = state.sound_sources[index];
  ma_sound_init_from_file(&state.audio_engine, source.path, 0, NULL, NULL, &state.sounds[idx]);
  ma_sound_start(&state.sounds[idx]);
  ma_sound_set_spatialization_enabled(&state.sounds[idx], spatial);
  ma_sound_set_end_callback(&state.sounds[idx], make_sound_available, NULL);

  // Scuffed as it guarantees literally nothing but hey, what can do.
  state.sound_idx = (idx + 1) % SOUND_INSTANCE_COUNT;
}

void set_sound_pos(i32 idx, v2f pos)
{
  ma_sound_set_position(&state.sounds[idx], 
    pos.x * SOUND_WORLD_SCALE, 0.f, pos.y * SOUND_WORLD_SCALE);
}

void set_sound_dir(i32 idx, v2f dir)
{
  // TODO(calco): Maybe multiply by scale?
  ma_sound_set_direction(&state.sounds[idx],
    dir.x, 0.f, dir.y);
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