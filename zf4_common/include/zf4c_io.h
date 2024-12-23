#ifndef ZF4C_IO_H
#define ZF4C_IO_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void zf4_log(const char* const format, ...);
void zf4_log_error(const char* const format, ...);

char* zf4_get_file_contents(const char* const filename);

#endif
