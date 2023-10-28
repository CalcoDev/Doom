#ifndef DEFINES_H
#define DEFINES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#define tn_debug 1

#define bytes(n)     n
#define kilobytes(n) n << 10
#define megabytes(n) n << 20
#define gigabytes(n) n << 30

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

typedef float f32;
typedef double f64;

#define statement(s) do {s} while (0)

#ifdef COMPILER_CLANG
#  define FILE_NAME __FILE_NAME__
#else
#  define FILE_NAME __FILE__
#endif


#ifdef tn_debug
#define tn_assert(expr, format, ...) do { \
    if (!(expr)) { \
        fprintf(stderr, "Assertion failed: %s, File: %s, Line: %d\n", #expr, __FILE__, __LINE__); \
        fprintf(stderr, format, ##__VA_ARGS__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)
#define tn_log(format, ...) do { \
    fprintf(stdout, "Info: %s, File: %s, Line: %d\n", __func__, __FILE__, __LINE__); \
    fprintf(stdout, format, ##__VA_ARGS__); \
    fprintf(stdout, "\n"); \
} while (0)
#define tn_logerr(format, ...) do { \
    fprintf(stderr, "Error: %s, File: %s, Line: %d\n", __func__, __FILE__, __LINE__); \
    fprintf(stderr, format, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
} while (0)
#define tn_logfatal(format, ...) do { \
    fprintf(stderr, "Fatal Error: %s, File: %s, Line: %d\n", __func__, __FILE__, __LINE__); \
    fprintf(stderr, format, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
    exit(EXIT_FAILURE); \
} while (0)
#else
#define tn_assert(expr, format, ...) do { (void)0; } while (0)
#define tn_log(format, ...) do { (void)0; } while (0)
#define tn_logerr(format, ...) do { (void)0; } while (0)
#define tn_logfatal(format, ...) do { (void)0; } while (0)
#endif
#endif
