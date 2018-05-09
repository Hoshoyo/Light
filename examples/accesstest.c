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
typedef struct vec4{r32 x;r32 y;r32 z;r32 w;} vec4;
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct Person{struct vec4 pos;s32 age;struct string name;} Person;

struct Person foo();
s32 __main();

struct Person foo(){
struct Person Pedro = {0};

r32 a;
a = ((((Pedro).pos)).y);
r32 b;
b = ((((Pedro).pos)).y);
((Pedro).age) = 26;
s32 c;
c = ((Pedro).age);
struct string d;
d = ((Pedro).name);

return Pedro;
}

s32 __main(){
struct Person x;
x = foo();

return ((x).age);
}


int __entry() {
	return __main();
}