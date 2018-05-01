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
s64 print_string(s64 length, u8* str);
s32 factorial(s32 v);
void fill_array();
void multi_array();
typedef struct {r32 x;r32 y;} vec2;
void fill_struct();
void pointer_arithmetic();
void matrix();
s64 __main();

s32 factorial(s32 v){
if(((v==0)||(v==1)))return 1;
return (v*factorial((v-1)));
}

void fill_array(){
char a[40] = {0};
s64 i = 0;
while((i<10)){
*(s32*)(((char*)a)+ 4 * (i)) = factorial(((s32)i));
i = (i+1);
}
loop_0:;

i = 0;
while((i<10)){
print_s64(((s64)*(s32*)(((char*)a)+ 4 * (i))));
i = (i+1);
}
loop_1:;

}

void multi_array(){
char matrix[64] = {0};
{
s64 x = 0;
s64 y = 0;
while((y<4)){
x = 0;
while((x<4)){
*(r32*)(((char*)(r32**)(((char*)matrix)+ 16 * (y)))+ 4 * (x)) = ((r32)(x*y));
print_r32(*(r32*)(((char*)(r32**)(((char*)matrix)+ 16 * (y)))+ 4 * (x)));
x = (x+1);
}
loop_3:;

y = (y+1);
}
loop_2:;

}
}

void fill_struct(){
vec2 pos = {0};
(pos.x) = 1.000000;
(pos.y) = 2.000000;
print_s64((pos.x));
print_s64((pos.y));
}

void pointer_arithmetic(){
char arr[10] = {0};
u8* ptr = ((u8*)(&arr));
(*ptr) = 0x48;
(*(ptr+1)) = 0x49;
print_string(2,ptr);
}

void matrix(){
char m[64] = {0};
char x[16] = (r32**)(((char*)m)+ 16 * (0));
}

s64 __main(){
return 0;
}


void __entry() {
	ExitProcess(__main());
}