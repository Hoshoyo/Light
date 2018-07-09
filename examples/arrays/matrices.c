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
void __memory_copy(void* dest, void* src, u64 size) {
	for(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];
}
typedef struct string string;
typedef struct timespec timespec;
typedef struct stat stat;
typedef void* __func_type_0x1a4fca0(u64, u64);
typedef void __func_type_0x1a4fd08();
typedef s32 __func_type_0x1a4fd60();
typedef void* __func_type_0x1a4fdb8(void*, void*, u64);
typedef s64 __func_type_0x1a4fe28(u8*);
typedef s64 __func_type_0x1a4fec8(s32, u8*);
typedef s64 __func_type_0x1a4ff30(u32, bool, s8*);
typedef s64 __func_type_0x1a4ffe0(s64, u8*);
typedef s64 __func_type_0x1a50048(r32, u8*);
typedef s64 __func_type_0x1a500f0(r64, u8*);
typedef s64 __func_type_0x1a50158(bool);
typedef s64 __func_type_0x1a501b8(u32);
typedef s64 __func_type_0x1a50218(s32);
typedef s64 __func_type_0x1a50278(s64);
typedef s64 __func_type_0x1a502d8(r32);
typedef s64 __func_type_0x1a50338(r64);
typedef s64 __func_type_0x1a50398(u8*, s64);
typedef s64 __func_type_0x1a50400(struct string);
typedef u64 __func_type_0x1a505d0(s32, void*, u64);
typedef s64 __func_type_0x1a50640(s32, void*, u64);
typedef s32 __func_type_0x1a506b0(u8*, s32);
typedef s32 __func_type_0x1a50718(s32);
typedef s32 __func_type_0x1a507b8(s32, struct stat*);
typedef void* __func_type_0x1a50820(u64);
typedef void* __func_type_0x1a50880(struct string, __func_type_0x1a50820* , u64*);
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct timespec{s64 tv_sec;s64 tv_nsec;} timespec;
typedef struct stat{u64 st_dev;u64 st_ino;u64 st_nlink;u32 st_mode;u32 st_uid;u32 st_gid;u64 st_rdev;s64 st_size;s64 st_blksize;s64 st_blocks;struct timespec st_atim;struct timespec st_mtim;struct timespec st_ctim;char __glibc_reserved[24];} stat;

void* calloc(u64 count, u64 size);
char* __string_data_0 = "Example1\n\n";
char* __string_data_1 = " ";
char* __string_data_2 = "\n";
char* __string_data_3 = " ";
char* __string_data_4 = "\n";
char* __string_data_5 = " ";
void example1();
char* __string_data_6 = "\n\nExample2\n\n";
char* __string_data_7 = " ";
char* __string_data_8 = "\n";
void example2();
s32 __main();
void* memcpy(void* dest, void* src, u64 size);
s64 string_length(u8* s);
s64 s32_to_str(s32 val, char buffer[16]);
s64 u32_to_str_base16(u32 val, bool leading_zeros, s8* buffer);
s64 s64_to_str(s64 val, char buffer[32]);
s64 r32_to_str(r32 v, char buffer[32]);
s64 r64_to_str(r64 v, char buffer[64]);
s64 print_bool(bool v);
char* __string_data_9 = "0x";
s64 print_u32(u32 v);
s64 print_s32(s32 v);
s64 print_s64(s64 v);
s64 print_r32(r32 v);
s64 print_r64(r64 v);
s64 print_string_c(u8* s);
s64 print_string_l(u8* s, s64 length);
s64 print_string(struct string s);
u64 write(s32 fd, void* buf, u64 count);
s64 read(s32 fd, void* buf, u64 count);
s32 open(u8* pathname, s32 flags);
s32 close(s32 fd);
s32 fstat(s32 fd, struct stat* buf);
void* read_entire_file(struct string filename, __func_type_0x1a50820* allocator, u64* out_file_size);

void example1(){
struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xa;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
(print_string)(__temp_v_0);
char matrix[64];
{
char* __t_base = (char*)matrix;
char* __struct_base = __t_base;
*(s32*)__t_base = 0x1;
__t_base += 4;
*(s32*)__t_base = 0x2;
__t_base += 4;
*(s32*)__t_base = 0x3;
__t_base += 4;
*(s32*)__t_base = 0x4;
__t_base += 4;
*(s32*)__t_base = 0x5;
__t_base += 4;
*(s32*)__t_base = 0x6;
__t_base += 4;
*(s32*)__t_base = 0x7;
__t_base += 4;
*(s32*)__t_base = 0x8;
__t_base += 4;
*(s32*)__t_base = 0x9;
__t_base += 4;
*(s32*)__t_base = 0xa;
__t_base += 4;
*(s32*)__t_base = 0xb;
__t_base += 4;
*(s32*)__t_base = 0xc;
__t_base += 4;
*(s32*)__t_base = 0xd;
__t_base += 4;
*(s32*)__t_base = 0xe;
__t_base += 4;
*(s32*)__t_base = 0xf;
__t_base += 4;
*(s32*)__t_base = 0x10;
__t_base += 4;
}

{
s64 y;
y = 0x0;
while(((y)<0x4)){
{
s64 x;
x = 0x0;
while(((x)<0x4)){
(print_s32)(*(s32*)((char*)((char*)matrix + y *  16 ) + x *  4 ));
struct string __temp_v_1;
{
char* __t_base = (char*)&(__temp_v_1);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_1;
}
(print_string)(__temp_v_1);
x = ((x)+0x1);
}
loop_1:;

}
struct string __temp_v_2;
{
char* __t_base = (char*)&(__temp_v_2);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_2;
}
(print_string)(__temp_v_2);
y = ((y)+0x1);
}
loop_0:;

}
{
s64 x;
x = 0x0;
while(((x)<((0x4)*0x4))){
(print_s32)(*(s32*)((char*)((s32*)matrix) + x *  4 ));
struct string __temp_v_3;
{
char* __t_base = (char*)&(__temp_v_3);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_3;
}
(print_string)(__temp_v_3);
x = ((x)+0x1);
}
loop_2:;

}
struct string __temp_v_4;
{
char* __t_base = (char*)&(__temp_v_4);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_4;
}
(print_string)(__temp_v_4);
char foo[16];
{
char* __arr_res = (char*)(((char*)matrix + 0x2 *  16 ));
__memory_copy(foo, __arr_res, 16);
}

{
s64 x;
x = 0x0;
while(((x)<0x4)){
(print_s32)(*(s32*)((char*)foo + x *  4 ));
struct string __temp_v_5;
{
char* __t_base = (char*)&(__temp_v_5);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_5;
}
(print_string)(__temp_v_5);
x = ((x)+0x1);
}
loop_3:;

}
}

void example2(){
struct string __temp_v_6;
{
char* __t_base = (char*)&(__temp_v_6);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xc;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_6;
}
(print_string)(__temp_v_6);
s32* ptr;
ptr = ((s32*)(calloc)(((0x4)*0x4),0x4));
{
s64 y;
y = 0x0;
while(((y)<0x4)){
{
s64 x;
x = 0x0;
while(((x)<0x4)){
*(s32*)((char*)ptr + ((((y)*0x4))+x) *  4 ) = ((s32)((y)*x));
x = ((x)+0x1);
}
loop_5:;

}
y = ((y)+0x1);
}
loop_4:;

}
char bar[64];
{
char* __arr_res = (char*)(((s32**)ptr));
__memory_copy(bar, __arr_res, 64);
}

{
s64 y;
y = 0x0;
while(((y)<0x4)){
{
s64 x;
x = 0x0;
while(((x)<0x4)){
(print_s32)(*(s32*)((char*)((char*)bar + y *  16 ) + x *  4 ));
struct string __temp_v_7;
{
char* __t_base = (char*)&(__temp_v_7);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_7;
}
(print_string)(__temp_v_7);
x = ((x)+0x1);
}
loop_7:;

}
struct string __temp_v_8;
{
char* __t_base = (char*)&(__temp_v_8);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_8;
}
(print_string)(__temp_v_8);
y = ((y)+0x1);
}
loop_6:;

}
}

s32 __main(){
(example1)();
(example2)();
return 0x0;
}

s64 string_length(u8* s){
s64 c;
c = 0x0;
while((((*s))!=0x0)){
c = ((c)+0x1);
s = ((s)+0x1);
}
loop_8:;

return c;
}

s64 s32_to_str(s32 val, char buffer[16]){
char b[16] = {0};

s32 sum = 0;

if(((val)==0x0)){
*(u8*)((char*)buffer + 0x0 *  1 ) = 0x30;
return 0x1;
}
if(((val)<0x0)){
val = (-val);
*(u8*)((char*)buffer + 0x0 *  1 ) = ((u8)0x2d);
sum = 0x1;
}
u8* auxbuffer;
auxbuffer = (&*(u8*)((char*)b + 0x10 *  1 ));
u8* start;
start = ((auxbuffer)+sum);
while(((val)!=0x0)){
s32 rem;
rem = ((val)%0xa);
val = ((val)/0xa);
(*auxbuffer) = ((0x30)+((u8)rem));
auxbuffer = ((auxbuffer)-0x1);
}
loop_9:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)((char*)buffer + sum *  1 ))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
}

s64 u32_to_str_base16(u32 val, bool leading_zeros, s8* buffer){
char temp_buffer[64] = {0};

s8* numbuffer;
numbuffer = ((s8*)temp_buffer);
s8* at;
at = ((numbuffer)+0x3f);
u32 count;
count = 0x0;
if(((val)==0x0)){
(*buffer) = 0x30;
count = ((count)+0x1);
return ((((s64)count))+0x1);
}
u32 mask;
mask = 0xf;
u32 auxmask;
auxmask = 0xffffffff;
while(((count)<0x8)){
if((((!((bool)((auxmask)&val))))&&(!leading_zeros)))goto loop_10;
u32 v;
v = ((((val)&((mask)<<((count)*0x4))))>>((count)*0x4));
if(((v)>=0xa))v = ((v)+0x37); else v = ((v)+0x30);
(*at) = ((s8)v);
at = ((at)-0x1);
auxmask = ((auxmask)&(((~mask))<<((count)*0x4)));
count = ((count)+0x1);
}
loop_10:;

u32 i;
i = 0x0;
{
u32 i;
i = 0x0;
while(((i)<count)){
at = ((at)+0x1);
(*buffer) = (*at);
buffer = ((buffer)+0x1);
i = ((i)+0x1);
}
loop_11:;

}
return ((s64)count);
}

s64 s64_to_str(s64 val, char buffer[32]){
char b[32] = {0};

s32 sum = 0;

if(((val)==0x0)){
*(u8*)((char*)buffer + 0x0 *  1 ) = 0x30;
return 0x1;
}
if(((val)<0x0)){
val = (-val);
*(u8*)((char*)buffer + 0x0 *  1 ) = ((u8)0x2d);
sum = 0x1;
}
u8* auxbuffer;
auxbuffer = (&*(u8*)((char*)b + 0x20 *  1 ));
u8* start;
start = ((auxbuffer)+sum);
while(((val)!=0x0)){
s64 rem;
rem = ((val)%0xa);
val = ((val)/0xa);
(*auxbuffer) = ((0x30)+((u8)rem));
auxbuffer = ((auxbuffer)-0x1);
}
loop_12:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)((char*)buffer + sum *  1 ))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
}

s64 r32_to_str(r32 v, char buffer[32]){
s64 l;
l = (s32_to_str)(((s32)v),((u8*)((u8*)buffer)));
s64 precision;
precision = 0xf;
if(((v)<0.000000)){
v = (-v);
}
r32 fractional_part;
fractional_part = ((v)-((r32)((s32)v)));
*(u8*)((char*)buffer + l *  1 ) = 0x2e;
l = ((l)+0x1);
if(((fractional_part)==0.000000)){
*(u8*)((char*)buffer + l *  1 ) = 0x30;
l = ((l)+0x1);
return l;
}
while(((((precision)>0x0))&&((fractional_part)>0.000000))){
fractional_part = ((fractional_part)*10.000000);
*(u8*)((char*)buffer + l *  1 ) = ((((u8)fractional_part))+0x30);
fractional_part = ((fractional_part)-((r32)((s32)fractional_part)));
l = ((l)+0x1);
}
loop_13:;

return l;
}

s64 r64_to_str(r64 v, char buffer[64]){
s64 l;
l = (s64_to_str)(((s64)v),((u8*)((u8*)buffer)));
s64 precision;
precision = 0x1f;
r64 fractional_part;
fractional_part = ((v)-((r64)((s64)v)));
*(u8*)((char*)buffer + l *  1 ) = 0x2e;
l = ((l)+0x1);
if(((fractional_part)==0.000000)){
*(u8*)((char*)buffer + l *  1 ) = 0x30;
l = ((l)+0x1);
return l;
}
while(((((precision)>0x0))&&((fractional_part)>0.000000))){
fractional_part = ((fractional_part)*10.000000);
*(u8*)((char*)buffer + l *  1 ) = ((((u8)fractional_part))+0x30);
fractional_part = ((fractional_part)-((r64)((s64)fractional_part)));
l = ((l)+0x1);
}
loop_14:;

return l;
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
return (print_string_l)(((u8*)buffer),len);
}

s64 print_u32(u32 v){
char buffer[16] = {0};

struct string __temp_v_9;
{
char* __t_base = (char*)&(__temp_v_9);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x2;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_9;
}
(print_string)(__temp_v_9);
s64 len;
len = (u32_to_str_base16)(v,true,((s8*)buffer));
return (print_string_l)(((u8*)buffer),len);
}

s64 print_s32(s32 v){
char buffer[16] = {0};

s64 len;
len = (s32_to_str)(v,buffer);
return (print_string_l)(((u8*)buffer),len);
}

s64 print_s64(s64 v){
char buffer[32] = {0};

s64 len;
len = (s64_to_str)(v,buffer);
return (print_string_l)(((u8*)buffer),len);
}

s64 print_r32(r32 v){
char buffer[32] = {0};

s64 len;
len = (r32_to_str)(v,buffer);
return (print_string_l)(((u8*)buffer),len);
}

s64 print_r64(r64 v){
char buffer[64] = {0};

s64 len;
len = (r64_to_str)(v,buffer);
return (print_string_l)(((u8*)buffer),len);
}

s64 print_string_c(u8* s){
(write)(0x1,((void*)s),((u64)(string_length)(s)));
}

s64 print_string_l(u8* s, s64 length){
(write)(0x1,((void*)s),((u64)length));
}

s64 print_string(struct string s){
(write)(0x1,((void*)((s).data)),((u64)((s).length)));
}

void* read_entire_file(struct string filename, __func_type_0x1a50820* allocator, u64* out_file_size){
s32 descriptor;
descriptor = (open)(((filename).data),0x0);
if(((descriptor)==(-0x1))){
{
return ((void*)0x0);
}
}
struct stat file_info = {0};

(fstat)(descriptor,(&file_info));
void* memory;
memory = (allocator)(((u64)((file_info).st_size)));
if(((memory)==((void*)0x0))){
{
return memory;
}
}
s64 rd;
rd = (read)(descriptor,memory,((u64)((file_info).st_size)));
(close)(descriptor);
if(((out_file_size)!=((u64*)0x0))){
{
(*out_file_size) = ((u64)((file_info).st_size));
}
}
return memory;
}


int __entry() {
	return __main();
}