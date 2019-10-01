#pragma once
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#if __WORDSIZE == 64 && defined(__linux__)
typedef unsigned long int u64;
typedef long int s64;
#else
typedef unsigned long long int u64;
typedef long long int s64;
#endif

typedef char s8;
typedef short s16;
typedef int s32;

typedef int bool;
typedef float r32;
typedef double r64;

#define true 1
#define false 0