typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef u32 bool;
typedef float r32;
typedef double r64;
#define true 1
#define false 0


void __memory_copy(void* dest, void* src, u64 size) {
	for(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];
}
typedef struct {} type_0x144a320;
typedef struct {foo b;} tar;
typedef s32 hello;
typedef s32 type_0x144a588;
typedef struct foo* type_0x144a4a8;
typedef struct {type_0x144a4a8 a;} type_0x144a4e0;
typedef struct {type_0x144a4a8 a;} foo;
typedef union {} type_0x144a358;
typedef struct {foo b;} type_0x144a400;
