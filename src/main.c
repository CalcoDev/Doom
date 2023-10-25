#include <stdio.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_glfw_gl3.h>

#define WINDOW_W 1280
#define WINDOW_H 720

#define VIEWPORT_W 32
#define VIEWPORT_H 18

#define Bytes(n)     n
#define Kilobytes(n) n << 10
#define Megabytes(n) n << 20
#define Gigabytes(n) n << 30

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef s8 b8;
typedef s16 b16;
typedef s32 b32;
typedef s64 b64;

typedef float f32;
typedef double f64;

#define Statement(s)                                                           \
  do                                                                           \
  {                                                                            \
    s                                                                          \
  } while (0)

#ifdef COMPILER_CLANG
#  define FILE_NAME __FILE_NAME__
#else
#  define FILE_NAME __FILE__
#endif

#define DOOM_DEBUG 1
#ifdef DOOM_DEBUG

#  define flush fflush(stdout)
#  define trace Statement(printf("%s:%d: Trace\n", FILE_NAME, __LINE__); flush;)
#  define unreachable Statement(\
printf("How did we get here? In %s on line %d\n", FILE_NAME, __LINE__);\
flush;
#  define Log(format, ...)                                                     \
    Statement(printf("Info: "); printf(format, ##__VA_ARGS__); printf("\n");   \
              flush;)
#  define LogError(format, ...)                                                \
    Statement(printf("%s:%d: Error: ", FILE_NAME, __LINE__);                   \
              printf(format, ##__VA_ARGS__); printf("\n"); flush;)
#  define LogReturn(ret, format, ...)                                          \
    Statement(printf("%s:%d: Error: ", FILE_NAME, __LINE__);                   \
              printf(format, ##__VA_ARGS__); printf("\n"); flush; return ret;)
#  define LogFatal(format, ...)                                                \
    Statement(printf("%s:%d: Error: ", FILE_NAME, __LINE__);                   \
              printf(format, ##__VA_ARGS__); printf("\n"); flush; exit(-1);)
#  define AssertTrue(c, format, ...)                                           \
    Statement(if (!(c)) {                                                      \
      printf("%s:%d: Error: ", FILE_NAME, __LINE__);                           \
      printf("Assertion Failure: ");                                           \
      printf(format, ##__VA_ARGS__);                                           \
      printf("\n");                                                            \
    })
#else
#  define Log(format, ...)            Statement()
#  define LogError(format, ...)       Statement()
#  define LogReturn(ret, format, ...) Statement()
#  define LogFatal(format, ...)       Statement()
#  define AssertTrue(c, format, ...)  Statement()
#endif

#define D_MAX_NK_VERTEX  65536
#define D_MAX_NK_ELEMENT 262144
#define FRAMERATE        0.016667f

typedef struct State
{
  // Rendering
  GLFWwindow* window;
  u32 glfw_texture;
  u32 pixels[WINDOW_W * WINDOW_H];
  b8 dirty;

  // Time
  f32 prev_time;
  f32 curr_time;

  // UI
  struct nk_glfw nk_glfw;
  struct nk_context* nk_ctx;
  b8 show_debug_ui;

  // Input
  b8 prev_keys[GLFW_KEY_LAST];
  b8 curr_keys[GLFW_KEY_LAST];

  f32 lifespan;
} State;
State state;

b8 GetKeyPressed(u16 key)
{
  return state.curr_keys[key] && !state.prev_keys[key];
}
b8 GetKeyReleased(u16 key)
{
  return !state.curr_keys[key] && state.prev_keys[key];
}
b8 GetKeyDown(u16 key) { return state.curr_keys[key]; }
b8 GetKeyUp(u16 key) { return !state.curr_keys[key]; }

void SetPixel(u32 x, u32 y, u32 colour)
{
  state.pixels[x * VIEWPORT_W + y] = colour;
  state.dirty = 1;
}

void GLFWKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods
)
{
  state.curr_keys[key] = action == GLFW_PRESS;
}

int main()
{
  AssertTrue(glfwInit() != 0, "Failed to initialize glfw.", "");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // Mac

  state.window = glfwCreateWindow(WINDOW_W, WINDOW_H, "Doom", NULL, NULL);
  AssertTrue(state.window != NULL, "Failed to create window!", "");
  glfwSetKeyCallback(state.window, GLFWKeyCallback);

  glfwMakeContextCurrent(state.window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  state.nk_ctx =
      nk_glfw3_init(&state.nk_glfw, state.window, NK_GLFW3_INSTALL_CALLBACKS);
  {
    struct nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&state.nk_glfw, &atlas);
    nk_glfw3_font_stash_end(&state.nk_glfw);
  }

  static const f32 verts[] = {-1, 1, -1, -1, 1, -1, 1, -1, 1, 1, -1, 1};
  static const char* vs_s =
      "#version 330 core\nlayout(location = 0) in vec2 p;out vec2 tc;void "
      "main(){tc=p;gl_Position=vec4(p.xy, 1, 1);}";
  static const char* fs_s =
      "#version 330 core\nout vec4 FC;in vec2 tc;uniform sampler2D "
      "t;void main(){FC=texture(t, vec2((tc.x+1)/2,(tc.y+1)/2));}";

  u32 vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vs_s, NULL);
  glCompileShader(vs);
  u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fs_s, NULL);
  glCompileShader(fs);
  u32 program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  u32 vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  u32 vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)0);
  glEnableVertexAttribArray(0);

  glGenTextures(1, &state.glfw_texture);
  glBindTexture(GL_TEXTURE_2D, state.glfw_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGBA, VIEWPORT_W, VIEWPORT_H, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &state.pixels[0]
  );

  glUseProgram(program);
  while (!glfwWindowShouldClose(state.window))
  {
    state.curr_time = glfwGetTime();

    // Limit update FPS
    if (state.curr_time - state.prev_time > FRAMERATE)
    {
      for (u32 i = 0; i < GLFW_KEY_LAST; ++i)
        state.prev_keys[i] = state.curr_keys[i];

      glfwPollEvents();
      nk_glfw3_new_frame(&state.nk_glfw);

      if (GetKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(state.window, 1);
      if (GetKeyPressed(GLFW_KEY_F3))
      {
        Log("Pressed F3", "");
        state.show_debug_ui = !state.show_debug_ui;
      }

      if (state.dirty)
      {
        state.dirty = 0;
        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0, VIEWPORT_W, VIEWPORT_H, GL_RGBA,
            GL_UNSIGNED_BYTE, &state.pixels[0]
        );
      }
      glViewport(0, 0, WINDOW_W, WINDOW_H);
      glBlendFunc(GL_ONE, GL_ZERO);
      glDisable(GL_SCISSOR_TEST);
      glScissor(0, 0, 1280, 720);
      glCullFace(GL_BACK);
      glDepthFunc(GL_LESS);
      glDisable(GL_DEPTH_TEST);
      glBindVertexArray(vao);
      glBindTexture(GL_TEXTURE_2D, state.glfw_texture);
      glUseProgram(program);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      // gui
      if (state.show_debug_ui)
      {
        if (nk_begin(
                state.nk_ctx, "Debug UI", nk_rect(WINDOW_W - 320, 0, 320, 360),
                NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                    NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE
            ))
        {
          nk_layout_row_static(state.nk_ctx, 200, 300, 1);
          nk_group_begin(
              state.nk_ctx, "Time", NK_WINDOW_BORDER | NK_WINDOW_DYNAMIC
          );
          char buf[64];

          nk_layout_row_static(state.nk_ctx, 12, 120, 2);
          snprintf(buf, sizeof(buf), "Time: %f", state.curr_time);
          nk_label_colored(
              state.nk_ctx, buf, NK_TEXT_ALIGN_LEFT,
              (struct nk_color){255, 0, 0, 255}
          );
          snprintf(
              buf, sizeof(buf), "FPS: %f",
              1.0f / (state.curr_time - state.prev_time)
          );
          nk_label(state.nk_ctx, buf, NK_TEXT_ALIGN_LEFT);

          nk_label(state.nk_ctx, "Timescale: 1", NK_TEXT_ALIGN_LEFT);
          nk_label(state.nk_ctx, "Ooga booga more height", NK_TEXT_ALIGN_LEFT);
          nk_group_end(state.nk_ctx);

          if (nk_tree_push(state.nk_ctx, NK_TREE_TAB, "Tre", NK_MAXIMIZED))
          {
            nk_label(
                state.nk_ctx, "This is a funky thing hmm yes yes",
                NK_TEXT_CENTERED
            );
            nk_property_float(
                state.nk_ctx, "#Lifespan", 0, &state.lifespan, 2000, 0.01f,
                0.01f
            );
            nk_tree_pop(state.nk_ctx);
          }

          nk_layout_row_static(state.nk_ctx, 20, 120, 2);
          snprintf(buf, sizeof(buf), "Lifespan viewer: %f", state.curr_time);
          nk_label_colored(
              state.nk_ctx, buf, NK_TEXT_ALIGN_LEFT,
              (struct nk_color){255, 255, 0, 255}
          );
        }
        nk_end(state.nk_ctx);
      }

      nk_glfw3_render(
          &state.nk_glfw, NK_ANTI_ALIASING_OFF, D_MAX_NK_VERTEX,
          D_MAX_NK_ELEMENT
      );
      glfwSwapBuffers(state.window);

      state.prev_time = state.curr_time;
    }
  }

  glDeleteShader(vs);
  glDeleteShader(fs);
  glDeleteProgram(program);

  glfwTerminate();
}