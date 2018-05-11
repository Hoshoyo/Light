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
typedef struct string{s64 length;s64 capacity;u8* data;} string;

char* __string_data_0 = "";
s32 __main();
s32 STDIN_FILENO = 0x0;
s32 STDOUT_FILENO = 0x1;
s32 STDERR_FILENO = 0x2;
void* memcpy(void* dest, void* src, u64 size);
u64 write(s32 fd, void* buf, u64 count);
s64 string_length(u8* s);
s64 s32_to_str(s32 val, char buffer[16]);
s64 s64_to_str(s64 val, char buffer[32]);
s64 print_bool(bool v);
s64 print_s32(s32 v);
s64 print_s64(s64 v);
s64 print_cstr(u8* s);
s64 print_cstr_length(u8* s, s64 length);
s64 print_str(struct string s);
char* __string_data_1 = "\n";
s64 println(struct string s);

s32 __main(){
s64 i;
i = 0x0;
s64 j;
j = 0x0;
while(((i)<0xa)){
j = 0x0;
while(((j)<0xa)){
print_s64(((s64)j));
if(((((i)==0x4))&&((j)==0x5)))goto loop_0;
j = ((j)+0x1);
}
loop_1:;

i = ((i)+0x1);
struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x0;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
println(__temp_v_0);
}
loop_0:;

return 0x0;
}

s64 string_length(u8* s){
s64 c;
c = 0x0;
while((((*s))!=0x0)){
c = ((c)+0x1);
s = ((s)+0x1);
}
loop_2:;

return c;
}

s64 s32_to_str(s32 val, char buffer[16]){
char b[16] = {0};

s32 sum = 0;

if(((val)==0x0)){
*(u8*)(((char*)buffer)+ 1 * (0x0)) = 0x30;
return 0x1;
}
if(((val)<0x0)){
val = (-val);
*(u8*)(((char*)buffer)+ 1 * (0x0)) = ((u8)0x2d);
sum = 0x1;
}
u8* auxbuffer;
auxbuffer = (&*(u8*)(((char*)b)+ 1 * (0x10)));
u8* start;
start = auxbuffer;
while(((val)!=0x0)){
s32 rem;
rem = ((val)%0xa);
val = ((val)/0xa);
(*auxbuffer) = ((0x30)+((u8)rem));
auxbuffer = ((auxbuffer)-0x1);
}
loop_3:;

s64 size;
size = ((start)-auxbuffer);
memcpy(((void*)(&*(u8*)(((char*)buffer)+ 1 * (sum)))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
}

s64 s64_to_str(s64 val, char buffer[32]){
char b[32] = {0};

s32 sum = 0;

if(((val)==0x0)){
*(u8*)(((char*)buffer)+ 1 * (0x0)) = 0x30;
return 0x1;
}
if(((val)<0x0)){
val = (-val);
*(u8*)(((char*)buffer)+ 1 * (0x0)) = ((u8)0x2d);
sum = 0x1;
}
u8* auxbuffer;
auxbuffer = (&*(u8*)(((char*)b)+ 1 * (0x20)));
u8* start;
start = auxbuffer;
while(((val)!=0x0)){
s64 rem;
rem = ((val)%0xa);
val = ((val)/0xa);
(*auxbuffer) = ((0x30)+((u8)rem));
auxbuffer = ((auxbuffer)-0x1);
}
loop_4:;

s64 size;
size = ((start)-auxbuffer);
memcpy(((void*)(&*(u8*)(((char*)buffer)+ 1 * (sum)))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
}

s64 print_bool(bool v){
char buffer[5] = {0};

s64 len = 0;

if(v){
{
char* __t_base = (char*)(buffer);
*(u8*)__t_base = 0x74;
__t_base += 1;
*(u8*)__t_base = 0x72;
__t_base += 1;
*(u8*)__t_base = 0x75;
__t_base += 1;
*(u8*)__t_base = 0x65;
__t_base += 1;
*(u8*)__t_base = 0x0;
__t_base += 1;
}

len = 0x4;
} else {
{
char* __t_base = (char*)(buffer);
*(u8*)__t_base = 0x66;
__t_base += 1;
*(u8*)__t_base = 0x61;
__t_base += 1;
*(u8*)__t_base = 0x6c;
__t_base += 1;
*(u8*)__t_base = 0x73;
__t_base += 1;
*(u8*)__t_base = 0x65;
__t_base += 1;
}

len = 0x5;
}
return print_cstr_length(((u8*)buffer),len);
}

s64 print_s32(s32 v){
char buffer[16] = {0};

s64 len;
len = s32_to_str(v,buffer);
return print_cstr_length(((u8*)buffer),len);
}

s64 print_s64(s64 v){
char buffer[32] = {0};

s64 len;
len = s64_to_str(v,buffer);
return print_cstr_length(((u8*)buffer),len);
}

s64 print_cstr(u8* s){
write(STDOUT_FILENO,((void*)s),((u64)string_length(s)));
}

s64 print_cstr_length(u8* s, s64 length){
write(STDOUT_FILENO,((void*)s),((u64)length));
}

s64 print_str(struct string s){
write(STDOUT_FILENO,((void*)((s).data)),((u64)((s).length)));
}

s64 println(struct string s){
print_str(s);
struct string nl;
{
char* __t_base = (char*)&(nl);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x2;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_1;
}

print_str(nl);
}


int __entry() {
	return __main();
}