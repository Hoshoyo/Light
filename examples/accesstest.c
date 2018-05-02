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
typedef struct {r32 x;r32 y;r32 z;r32 w;} vec4;
typedef struct {s64 length;s64 capacity;u8* data;} string;
typedef struct {vec4 pos;s32 age;string name;} Person;

Person foo();
s32 __main();

Person foo(){
Person Pedro = {0};
r32 a = ((Pedro.pos).y);
r32 b = ((Pedro.pos).y);
(Pedro.age) = 26;
s32 c = (Pedro.age);
string d = (Pedro.name);
return Pedro;
}

s32 __main(){
Person x = foo();
return (x.age);
}


void __entry() {
	ExitProcess(__main());
}