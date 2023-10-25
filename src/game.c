#include "game.h"

#include <math.h>

const static u8 MAP_DATA[MAP_W * MAP_H] = {
    1, 1, 1, 1, 1, 1, 1, 1, //
    1, 0, 0, 0, 5, 0, 0, 1, //
    1, 0, 2, 2, 0, 3, 0, 1, //
    1, 0, 3, 0, 0, 4, 0, 1, //
    1, 0, 0, 0, 0, 0, 0, 1, //
    1, 0, 0, 0, 0, 0, 0, 1, //
    1, 0, 0, 0, 0, 0, 0, 1, //
    1, 1, 1, 1, 1, 1, 1, 1, //
};

void game_init(void)
{
  state.pos = (v2f){MAP_W / 2, MAP_H / 2};
  state.look_dir = (v2f){-1, 0};
  state.plane = (v2f){0, 0.66};
}

void game_update(void)
{
  if (GetKeyPressed(GLFW_KEY_F3))
    state.show_debug_ui = !state.show_debug_ui;

  // raycast
  // for (u32 x = 0; x < VIEWPORT_W; ++x)
  // {
  //   f32 cam_x = 2 * (x / ((f32)VIEWPORT_W)) - 1;
  //   v2f dir = (v2f){
  //       (state.look_dir.x + state.plane.x * cam_x),
  //       (state.look_dir.y + state.plane.y * cam_x),
  //   };
  //   v2f delta_dist = (v2f) {(dir.x == 0 ? 1e30 : (f32)fabs(1 / dir.x)),
  //     (dir.y == 0 ? 1e30 : (f32)fabs(1 / dir.y))};

  //   v2f pos = state.pos;
  //   v2i ipos = (v2i){(i32)pos.x, (i32)pos.y};
  // }

  // draw
  ClearPixels();
}

void game_debug_ui(void)
{
  igBegin("Debug Window", NULL, 0);
  igEnd();
}

void game_free(void) 
{

}
