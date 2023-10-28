#ifndef OS_UTILS
#define OS_UTILS

#include "defines.h"

void windowsify_path(char* buffer, i32 size);
void unixify_path(char* buffer, i32 size);

i32 get_exe_path(char* buffer, i32 size);
i32 get_asset_path(char* buffer, char* rel_path, i32 size, i32 rel_size);

#endif