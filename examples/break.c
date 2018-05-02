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
s32 __main();

s32 __main(){
s64 i = 0;
s64 j = 0;
while((i<10)){
j = 0;
while((j<10)){
print_s64(((s64)j));
if(((i==4)&&(j==5)))goto loop_0;
j = (j+1);
}
loop_1:;

i = (i+1);
}
loop_0:;

return 0;
}


int __entry() {
	return __main();
}