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
u32 ExitProcess(u32 ret);
s64 print_s64(s64 value);
s64 print_s32(s32 value);
s64 print_r32(r32 value);
s64 print_r64(r64 value);
s64 println();
s64 print_string(s64 length, u8* str);
s32 __main();

s32 __main(){
return 0;
}


void __entry() {
	ExitProcess(__main());
}