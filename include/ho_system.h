#if !defined(HO_SYSTEM_H)
#define HO_SYSTEM_H
#include <stdint.h>

typedef int32_t b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float r32;
typedef double r64;

#if defined(_WIN32) || defined(_WIN64)
#include "ho_windows.h"
#elif defined(__linux__)
#define HO_API
#include <stdlib.h>
#include <string.h>
#include "ho_linux.h"
#endif
#ifdef HO_SYSTEM_IMPLEMENT
#define DYNAMIC_ARRAY_IMPLEMENT
#undef HO_SYSTEM_IMPLEMENT
#endif	// HO_SYSTEM_IMPLEMENT
#include "dynamic_array.h"
#endif	// HO_SYSTEM_H