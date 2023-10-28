#include "game.h"

#include <math.h>
#include "math.h"
#include "defines.h"

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
  0xFF03bafc,
  0xFFa83240,
  0xFFa8327f
};

const static u32 PLAYER_COLOUR = 0xFF0000FF;
static f32 PLAYER_SPEED = 1.f;
static f32 PLAYER_SENS = 0.05f;

State state;
b8 topdown_view = 1;
b8 topdown_raycast = 1;
b8 draw_view_ray = 1;
b8 topdown_wasd_movement = 0;
f32 view_ray_scale = 10.f;

void draw_line_dda(u32 x0, u32 y0, i32 dx, i32 dy, u32 colour)
{
  i32 steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
  f64 xIncrement = (f64)dx / (f64)steps;
  f64 yIncrement = (f64)dy / (f64)steps;
  f64 x = (f64)x0;
  f64 y = (f64)y0;

  for (i32 i = 0; i <= steps; i++) {
      SetPixel((u32)x, (u32)y, colour);
      x += xIncrement;
      y += yIncrement;
  }
}

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

void draw_vline(u32 x, u32 y0, u32 y1, u32 colour)
{
  for (u32 y = y0; y <= y1; ++y)
    SetPixel(x, y, colour);
}

void move_player(v2f movement)
{
  v2f npos = {
    state.player.pos.x + movement.x,
    state.player.pos.y + movement.y,
  };

  v2u mpos = {
    viewport_to_map_x(state.player.pos.x),
    viewport_to_map_y(state.player.pos.y),
  };
  v2u mnpos = {
    viewport_to_map_x(npos.x),
    viewport_to_map_y(npos.y),
  };

  // Try move X
  if (MAP_DATA[mpos.y * MAP_W + mnpos.x] == 0)
    state.player.pos.x += movement.x;
  
  // Try move Y
  if (MAP_DATA[mnpos.y * MAP_W + mpos.x] == 0)
    state.player.pos.y += movement.y;
}

State* game_get(void)
{
  return &state;
}

void game_init(void)
{
  state.player.pos = (v2f){VIEWPORT_W / 2, VIEWPORT_H / 2};
  state.player.forward_angle = f_deg_to_rad(90);
  state.player.fov = f_deg_to_rad(90);
}

void game_update(void)
{
  if (GetKeyPressed(GLFW_KEY_F3))
    state.show_debug_ui = !state.show_debug_ui;
  
  if (topdown_view)
  {
    if (topdown_wasd_movement)
    {
      if (GetKeyDown(GLFW_KEY_A))
        move_player((v2f) {-PLAYER_SPEED, 0});
      if (GetKeyDown(GLFW_KEY_D))
        move_player((v2f) {PLAYER_SPEED, 0});
      if (GetKeyDown(GLFW_KEY_W))
        move_player((v2f) {0, -PLAYER_SPEED});
      if (GetKeyDown(GLFW_KEY_S))
        move_player((v2f) {0, +PLAYER_SPEED});
    }
    
    // INVERTED BECAUSE -Y IS +Y
    if (GetKeyDown(GLFW_KEY_LEFT))
      state.player.forward_angle -= PLAYER_SENS;
    if (GetKeyDown(GLFW_KEY_RIGHT))
      state.player.forward_angle += PLAYER_SENS;
  }

  if (!topdown_view || (topdown_view && !topdown_wasd_movement))
  {
    // INVERTED BECAUSE -Y IS +Y
    if (GetKeyDown(GLFW_KEY_A))
      state.player.forward_angle -= PLAYER_SENS;
    if (GetKeyDown(GLFW_KEY_D))
      state.player.forward_angle += PLAYER_SENS;
    
    if (GetKeyDown(GLFW_KEY_W))
    {
      v2f forward = {
        cosf(state.player.forward_angle),
        sinf(state.player.forward_angle)
      };

      move_player((v2f) {PLAYER_SPEED * forward.x, PLAYER_SPEED * forward.y});
    }
    if (GetKeyDown(GLFW_KEY_S))
    {
      v2f forward = {
        cosf(state.player.forward_angle),
        sinf(state.player.forward_angle)
      };
      move_player((v2f) {-PLAYER_SPEED * forward.x, -PLAYER_SPEED * forward.y});
    }
  }
  
  ClearPixels();

  // Draw map
  if (topdown_view)
  {
    if (!topdown_raycast)
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
    }

    if (draw_view_ray)
    {
      v2f dir = {
        cosf(state.player.forward_angle),
        sinf(state.player.forward_angle)
      };

      draw_line_dda(
        state.player.pos.x, state.player.pos.y, 
        dir.x * view_ray_scale, dir.y * view_ray_scale,
        0xFF00FF00
      );
    }

    // Draw player
    SetPixel(state.player.pos.x, state.player.pos.y, PLAYER_COLOUR);
  }

  // Raycasting stuff, 2d if 2d, 2d if pseudo 3d
  {
    v2f pos = state.player.pos;
    v2f forward = {
      cosf(state.player.forward_angle),
      sinf(state.player.forward_angle)
    };
    f32 fov = state.player.fov;

    f32 half_fov = fov * 0.5f;
    for (u32 x = 0; x < VIEWPORT_W; ++x)
    {
      f32 norm_x = ((f32)x / (f32)VIEWPORT_W * 2.f) - 1.f;
      f32 ray_angle = state.player.forward_angle + (half_fov * norm_x);
      v2f ray_dir = {cosf(ray_angle), sinf(ray_angle)};

      struct Hit {
        v2f pos;
        u32 wall_colour;
        b8 hit_vline;
        b8 hit;
      } hit = {0};

      v2f rp = pos;

      // TODO(calco): Innefficient, but works.
      b8 alternate = 0;
      while (!hit.hit)
      {
        u32 mx = viewport_to_map_x(rp.x);
        u32 my = viewport_to_map_y(rp.y);

        u32 wall = MAP_DATA[my * MAP_W + mx];
        if (wall != 0) {
          hit.hit = 1;
          hit.pos = rp;
          hit.hit_vline = alternate;
          hit.wall_colour = COLOUR_DATA[wall];
        }

        if (alternate)
          rp.x += ray_dir.x;
        else
          rp.y += ray_dir.y;
        
        alternate = !alternate;
      }

      if (hit.hit)
      {
        v2u upos = v2_u(hit.pos);
        if (hit.hit_vline)
        {
          u32 br = ((hit.wall_colour & 0xFF00FF) * 0xC0) >> 8;
          u32 g  = ((hit.wall_colour & 0x00FF00) * 0xC0) >> 8;
          hit.wall_colour = 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
        }
        
        if (topdown_view)
        {
          if (topdown_raycast) 
          {
            SetPixel(upos.x, upos.y, hit.wall_colour);
          }
        }
        else
        {
          // v2f _d = {pos.x - hit.pos.x, pos.y - hit.pos.y};
          // f32 ray_dist = v2_sqr_mag(_d);
          // f64 ray_dist = (_d.x) * (_d.x) + (_d.y) * (_d.y);
          // ray_dist = sqrt(ray_dist);
          // f32 dist = ray_dist;

          // i32 h = (int) (VIEWPORT_H / dist);
          // i32 y0 = max((VIEWPORT_H / 2) - (h / 2), 0);
          // i32 y1 = min((VIEWPORT_H / 2) + (h / 2), VIEWPORT_H - 1);

          SetPixel(x, upos.y, hit.wall_colour);

          // v2f _d = {pos.x - hit.pos.x, pos.y - hit.pos.y};
          // f32 ray_dist = v2_mag(_d);
          // f32 theta = ray_angle;
          // f32 dist = cosf(theta) * ray_dist;

          // i32 h = (int) (VIEWPORT_H / dist);
          // i32 y0 = max((VIEWPORT_H / 2) - (h / 2), 0);
          // i32 y1 = min((VIEWPORT_H / 2) + (h / 2), VIEWPORT_H - 1);

          // if (y0 < 1)
          //   y0 = 1;
          // if (y0 > VIEWPORT_H - 2)
          //   y0 = VIEWPORT_H - 2;
          // if (y1 < 1)
          //   y1 = 1;
          // if (y1 > VIEWPORT_H - 2)
          //   y1 = VIEWPORT_H - 2;

          // draw_vline(x, 0, y0-1, 0xFF000000);
          // draw_vline(x, y0, y1, hit.wall_colour);
          // draw_vline(x, y1+1, VIEWPORT_H-1, 0xFF000000);
        }
      }
    }
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
      
      if (topdown_view)
        igCheckbox("Raycast 2D", &topdown_raycast);
    }

    igUnindent(0);
  }

  if (igCollapsingHeader_TreeNodeFlags("Player Info", tflags))
  {
    igIndent(0);
    {
      igInputFloat("Move Speed", &PLAYER_SPEED, 0.1f, 1.f, "%.2f", 
        ImGuiInputTextFlags_None);

      igBeginColumns("r_header", 2, ImGuiOldColumnFlags_NoResize | ImGuiOldColumnFlags_NoBorder | ImGuiOldColumnFlags_GrowParentContentsSize);
      igSliderFloat("FOV", &state.player.fov, 
        0.f, f_PI * 2.f, "%.2f", ImGuiSliderFlags_None);
      igNextColumn();
      igSliderFloat("Forward Angle", &state.player.forward_angle, 
        0.f, f_PI * 2.f, "%.2f", ImGuiSliderFlags_None);
      igEndColumns();

      if (topdown_view) 
      {
        igCheckbox("Draw View Ray", &draw_view_ray);
        if (draw_view_ray)
        {
          igSliderFloat("View Ray Scale", &view_ray_scale, 
            0.f, 32.f, "%.2f", ImGuiSliderFlags_None);
        }
        igCheckbox("WASD Movement", &topdown_wasd_movement);
      }
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