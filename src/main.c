#include <stdio.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#define WINDOW_W 1280
#define WINDOW_H 720

#define VIEWPORT_W 320
#define VIEWPORT_H 180

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

int main()
{
  AssertTrue(glfwInit() != 0, "Failed to initialize glfw.", "");

  GLFWwindow* window = glfwCreateWindow(WINDOW_W, WINDOW_H, "Doom", NULL, NULL);
  AssertTrue(window != NULL, "Failed to create window!", "");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // Mac

  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwMakeContextCurrent(window);

  AssertTrue(
      gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0,
      "Failed to load glad.", ""
  );

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(window, 1);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
}