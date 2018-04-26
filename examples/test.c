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


s32 ExitProcess(s32 exit_code);
void print_string(s64 length, s8* data);
void print_double(r64 v);
s32 factorial(s32 val);
s32 fibonacci(s32 val);
s32 main();

void __entry() {

	main();
}