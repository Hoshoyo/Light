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
char* base = (char*)a;
*(s32*)base = 1;
base += 4;
*(s32*)base = 2;
base += 4;
*(s32*)base = 3;
base += 4;
*(s32*)base = 4;
base += 4;
}

print_s32(*(s32*)(((char*)a)+ 4 * (3)));
return 0;
}


int __entry() {
	return __main();
}