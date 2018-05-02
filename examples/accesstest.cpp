typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float r32;
typedef double r64;


// Forward declarations
u32 ExitProcess(u32 ret);
struct vec4;
struct Person;
struct string;

Person foo();
s32 __main();
struct vec4{r32 x;r32 y;r32 z;r32 w;};
struct Person{vec4 pos;s32 age;string name;};
struct string{s64 length;s64 capacity;u8* data;};

Person foo(){
Person Pedro = {0};
r32 a = ((Pedro.pos).y);
r32 b = ((Pedro.pos).y);
s32 c = (Pedro.age);
string d = (Pedro.name);
}

s32 __main(){
Person x = foo();
return 0;
}


void __entry() {
	ExitProcess(__main());
}