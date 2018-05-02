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

s64 print_s32(s32 val);
s64 print_string(s64 length, u8* str);
s32 foo(r32 v);
typedef s32 __ret_make_proc(r32);
__ret_make_proc* make_proc();
void map(s32* arr, s64 length, s64(*f)(s32));
s32 __main();

s32 foo(r32 v){
return ((s32)v);
}

__ret_make_proc* make_proc(){
return foo;
}

void map(s32* arr, s64 length, s64(*f)(s32)){
char delimiter[2] = {0};
*(u8*)(((char*)delimiter)+ 1 * (0)) = 0x2c;
*(u8*)(((char*)delimiter)+ 1 * (1)) = 0x20;
s64 i = 0;
while((i<length)){
if((i!=0)){
print_string(2,((u8*)delimiter));
}
f(*(s32*)(((char*)arr)+ 4 * (i)));
i = (i+1);
}
loop_0:;

}

s32 __main(){
s32(*a)(r32) = make_proc();
char v[20] = {0};
*(s32*)(((char*)v)+ 4 * (0)) = 0;
*(s32*)(((char*)v)+ 4 * (1)) = 1;
*(s32*)(((char*)v)+ 4 * (2)) = 2;
*(s32*)(((char*)v)+ 4 * (3)) = 3;
*(s32*)(((char*)v)+ 4 * (4)) = 4;
map(((s32*)v),5,print_s32);
return a(32.000000);
}


int __entry() {
	return __main();
}