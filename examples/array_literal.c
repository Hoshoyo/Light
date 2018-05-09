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


// Forward declarations
typedef struct arr{char a[16];} arr;
typedef struct vec2{r32 x;r32 y;} vec2;

s64 print_s64(s64 value);
s64 print_s32(s32 value);
s64 print_r32(r32 value);
s64 print_r64(r64 value);
s64 println();
s64 print_string(s64 length, u8* str);
s32 __main();

s32 __main(){
char a[16];
{
char* __t_base = (char*)a;
char* __struct_base = __t_base;
__struct_base = __t_base;
{
char* __t_base = __struct_base;
*(r32*)((char*)__t_base + 0) = 1.000000;
*(r32*)((char*)__t_base + 4) = 2.000000;
}
__t_base += 8;
__struct_base = __t_base;
{
char* __t_base = __struct_base;
*(r32*)((char*)__t_base + 0) = 3.000000;
*(r32*)((char*)__t_base + 4) = 4.000000;
}
__t_base += 8;
}

print_r32(((*(struct vec2*)(((char*)a)+ 8 * (1))).x));
return 0;
}


int __entry() {
	return __main();
}