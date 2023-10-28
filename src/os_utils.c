#include "os_utils.h"

#include <Windows.h>
#include <Shlwapi.h>
#include <UserEnv.h>

#include <string.h>

void windowsify_path(char* buffer, i32 size)
{
  for (i32 c = 0; c < size; ++c)
  {
    if (buffer[c] == '/')
      buffer[c] = '\\';
  }
}

void unixify_path(char* buffer, i32 size)
{
  for (i32 c = 0; c < size; ++c)
  {
    if (buffer[c] == '\\')
      buffer[c] = '//';
  }
}

i32 get_exe_path(char* buffer, i32 size)
{
  i32 l = GetModuleFileNameA(NULL, buffer, size);
  i32 c = l;
  for (;buffer[c] != '\\'; --c);
  buffer[c] = '\0';
  return c;
}

i32 get_asset_path(char* buffer, char* rel_path, i32 size, i32 rel_size)
{
  get_exe_path(buffer, size);

  char* b = (char*)malloc(sizeof(char) * size);
  PathCombineA(b, buffer, rel_path);
  memcpy(buffer, b, size);

  free(b);
}