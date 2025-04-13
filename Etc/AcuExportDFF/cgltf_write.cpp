#define CGLTF_NO_STDINT
#define CGLTF_NO_STDBOOL
#define CGLTF_ATOLL _atoi64
#define CGLTF_WRITE_IMPLEMENTATION
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
//{
//    int r = -1;
//
//    if (size != 0)
//    {
//        va_list args_copy;
//        va_copy(args_copy, args);
//        r = _vsnprintf_s(buf, size, _TRUNCATE, fmt, args_copy);
//        va_end(args_copy);
//    }
//
//    if (r == -1)
//    {
//        r = _vscprintf(fmt, args);
//    }
//
//    return r;
//}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int r = vsnprintf(buf, size, fmt, args);
    va_end(args);
    return r;
}
#include "cgltf_write.h"