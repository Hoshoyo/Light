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
void __memory_copy(void* dest, void* src, u64 size) {
	for(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];
}
typedef s32 __func_type_000001EA88CC0580();
typedef void* __func_type_000001EA88CC05D8(u32);

void* stdhandle;
s32 __main();
void* GetStdHandle(u32 stdhandle);

s32 __main(){
return 0x0;
}


void __entry() {
	stdhandle = (GetStdHandle)(((u32)(-0xb)));
	ExitProcess(__main());
}