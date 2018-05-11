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
typedef struct string{s64 length;s64 capacity;u8* data;} string;

s32 __main();

s32 __main(){
r32 b;
char*  __temp_v_0;
{
char* __t_base = (char*)__temp_v_0;
char* __struct_base = __t_base;
*(r32*)__t_base = 0.000000;
__t_base += 0;
*(r32*)__t_base = 0.000000;
__t_base += 0;
*(r32*)__t_base = 0.000000;
__t_base += 0;
}
b = *(r32*)(((char*)__temp_v_0)+ 0 * (0x0));
return 0x0;
}


void __entry() {
	ExitProcess(__main());
}