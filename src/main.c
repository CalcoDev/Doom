#include <stdio.h>
#include <math.h>

#include "cimgui_include.h"

#include "defines.h"
#include "math.h"
#include "game.h"

void GLFWKeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mods
)
{
  State* state = (State*) glfwGetWindowUserPointer(window);
  state->curr_keys[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
}

int main(void)
{
  tn_assert(glfwInit() != 0, "Failed to initialize glfw.");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // Mac

  State* state = game_get();

  state->window = glfwCreateWindow(WINDOW_W, WINDOW_H, "Doom", NULL, NULL);
  tn_assert(state->window != NULL, "Failed to create window!");
  glfwSetKeyCallback(state->window, GLFWKeyCallback);

  glfwSetWindowUserPointer(state->window, state);

  glfwMakeContextCurrent(state->window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  state->imgui_ctx = igCreateContext(NULL);

  ImGuiIO* io = igGetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io->IniFilename = NULL;
  io->LogFilename = NULL;

  tn_assert(
      ImGui_ImplGlfw_InitForOpenGL(state->window, 1) != 0,
      "Failed to initialize imgui for glfw & opengl."
  );

  tn_assert(
      ImGui_ImplOpenGL3_Init("#version 330 core") != 0,
      "Failed to initialize imgui for opengl."
  );

  igStyleColorsDark(NULL);

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

  glGenTextures(1, &state->glfw_texture);
  glBindTexture(GL_TEXTURE_2D, state->glfw_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGBA, VIEWPORT_W, VIEWPORT_H, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &state->pixels[0]
  );

  glUseProgram(program);

  game_init();
  while (!glfwWindowShouldClose(state->window))
  {
    state->curr_time = glfwGetTime();

    // Limit update FPS
    if (state->curr_time - state->prev_time > FRAMERATE)
    {
      for (u32 i = 0; i < GLFW_KEY_LAST; ++i)
        state->prev_keys[i] = state->curr_keys[i];

      glfwPollEvents();
      if (GetKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(state->window, 1);

      game_update();
      glTexSubImage2D(
          GL_TEXTURE_2D, 0, 0, 0, VIEWPORT_W, VIEWPORT_H, GL_RGBA,
          GL_UNSIGNED_BYTE, &state->pixels[0]
      );

      glDrawArrays(GL_TRIANGLES, 0, 6);

      // gui
      ImGui_ImplGlfw_NewFrame();
      ImGui_ImplOpenGL3_NewFrame();
      igNewFrame();

      if (state->show_debug_ui)
      {
        game_debug_ui();
      }

      igRender();
      ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

      glfwSwapBuffers(state->window);

      state->prev_time = state->curr_time;
    }
  }

  game_free();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(NULL);

  glDeleteShader(vs);
  glDeleteShader(fs);
  glDeleteProgram(program);

  glfwTerminate();
}
