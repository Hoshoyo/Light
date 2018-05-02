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

s64 print_string(s64 length, u8* str);
s64 print_s64(s64 val);
s32 foo();
typedef s32 __ret_make_proc();
__ret_make_proc* make_proc();
s32 __main();

s32 foo(){
return 42;
}

__ret_make_proc* make_proc(){
return foo;
}

s32 __main(){
s32(*a)() = make_proc();
return a();
}


int __entry() {
	return __main();
}