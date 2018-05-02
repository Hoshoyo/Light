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
void fill_array();
s64 __main();

void fill_array(){
char a[12] = {0};
s64 i = 0;
while((i<3)){
*(s32*)(((char*)a)+ 4 * (i)) = ((s32)i);
i = (i+1);
}
loop_0:;

i = 0;
while((i<3)){
print_s64(((s64)*(s32*)(((char*)a)+ 4 * (i))));
i = (i+1);
}
loop_1:;

}

s64 __main(){
fill_array();
return 0;
}


int __entry() {
	return __main();
}