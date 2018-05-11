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

char* __string_data_0 = "Hello";
s32 __main();

s32 __main(){
s64 a;
struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x5;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
a = ((__temp_v_0).length);
return ((s32)a);
}


void __entry() {
	ExitProcess(__main());
}