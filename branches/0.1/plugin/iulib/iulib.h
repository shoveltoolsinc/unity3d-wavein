#ifndef IULIB_H__
#define IULIB_H__

#if (defined _WINDOWS)
#   define IU_WINDOWS 1
#elif (defined __APPLE__ && defined __MACH__)
#   define IU_MACOS 1
#endif

#if (IU_WINDOWS)
#   define WIN32_LEAN_AND_MEAN
#   define NOMINMAX
#   include <windows.h>
#   if (defined _MSC_VER)
typedef short int16_t;
#   endif
#elif (IU_MACOS)
#   include <stdint.h>
#endif

#include <cassert>
#include <algorithm>

#endif
