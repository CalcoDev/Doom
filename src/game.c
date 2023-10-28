#include "game.h"

#include <math.h>
#include <memory.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "os_utils.h"

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

void concat(char* str, char* suffix, i32 offset, i32 length)
{
  memcpy(str + offset, suffix, length);
  *(str + offset + length) = '\0';
}

void game_init(void)
{
  state.player.pos = (v2f) {4, 2};
  state.player.dir = (v2f) {0, -1};
  state.player.plane = (v2f) {0.66, 0};

  i32 x, y, comp;
  char path[1024];
  get_asset_path(path, ".\\assets\\wall_tex.png", 1024, 20);
  stbi_set_flip_vertically_on_load(1);
  unsigned char* data = stbi_load(path, &x, &y, &comp, STBI_rgb_alpha);
  memcpy(state.textures[0].data, data, TEX_W * TEX_H * 4);
  stbi_image_free(data);
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

void DrawVLine(i32 x, i32 y0, i32 y1, u32 colour)
{
  for (int y = y0; y <= y1; y++)
    SetPixel(x, y, colour);
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
      v2f pos;
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
        hit.pos = (v2f) { pos.x + side_dist.x, pos.y + side_dist.y };
      }
    }

    // if (hit.yside)
    // {
    //   u32 br = ((hit.colour & 0xFF00FF) * 0xC0) >> 8;
    //   u32 g  = ((hit.colour & 0x00FF00) * 0xC0) >> 8;
    //   hit.colour = 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
    // }

    f32 dist = !hit.yside ? 
      (side_dist.x - delta_dist.x) : (side_dist.y - delta_dist.y);

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
      SetPixel(x, dy, colour);
    }

    DrawVLine(x, 0, y0, 0xFF202020);
    DrawVLine(x, y1, VIEWPORT_H - 1, 0xFF505050);
  }
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