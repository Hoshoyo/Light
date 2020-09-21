#pragma once
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef int bool;
typedef float r32;
typedef double r64;

#define true 1
#define false 0

#if defined(_WIN32) || defined(_WIN64)
#define PRINTF_U64 "%llu"
#define PRINTF_S64 "%lld"
#elif defined(__linux__)
#define PRINTF_U64 "%lu"
#define PRINTF_S64 "%ld"
#endif