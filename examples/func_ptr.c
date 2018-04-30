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
typedef struct {char name[32];s32 age;} Person;
void print_array(char a[32]);
s32 __main();

void print_array(char a[32]){
s64 i = 0;
while((i<32)){
if((*(u8*)(((char*)a)+ 1 * (i))==0)){
break;
}
print_s64(((s64)*(u8*)(((char*)a)+ 1 * (i))));
i = (i+1);
}
}

s32 __main(){
Person p = {0};
*(u8*)(((char*)(p.name))+ 1 * (0)) = 0x50;
*(u8*)(((char*)(p.name))+ 1 * (1)) = 0x65;
*(u8*)(((char*)(p.name))+ 1 * (2)) = 0x64;
*(u8*)(((char*)(p.name))+ 1 * (3)) = 0x72;
*(u8*)(((char*)(p.name))+ 1 * (4)) = 0x6f;
(p.age) = 26;
print_array((p.name));
return 0;
}


int __entry() {
	return __main();
}