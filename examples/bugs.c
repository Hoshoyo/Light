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
s64 print_r32(r32 value);
s64 print_string(u8* str);
void* GetStdHandle(u32 std_handle);
bool WriteConsoleA(void* console_output, void* lp_buffer, u32 num_chars_to_write, u32* num_chars_written, void* reserved);
void pointer_arithmetic();
s64 __main();

void pointer_arithmetic(){
char arr[16] = {0};
u8* ptr = ((u8*)(&arr));
(*ptr) = 0x48;
(*(ptr+1)) = 0x65;
(*(ptr+2)) = 0x6c;
(*(ptr+3)) = 0x6c;
(*(ptr+4)) = 0x6f;
(*(ptr+5)) = 0x20;
(*(ptr+6)) = 0x57;
(*(ptr+7)) = 0x6f;
(*(ptr+8)) = 0x72;
(*(ptr+9)) = 0x6c;
(*(ptr+10)) = 0x64;
u32 written = 0;
void* stdhandle = GetStdHandle(((u32)(-11)));
bool err = WriteConsoleA(stdhandle,((void*)ptr),11,(&written),((void*)0));
}

s64 __main(){
pointer_arithmetic();
return 0;
}


void __entry() {
	ExitProcess(__main());
}