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
typedef struct Complex Complex;
typedef void* __func_type_0x1a74ca0(u64, u64);
typedef void __func_type_0x1a74d08(void*);
typedef r32 __func_type_0x1a74d68(r32);
typedef void __func_type_0x1a74e70(struct Complex*, s64);
typedef struct Complex __func_type_0x1a74ed8(struct Complex, struct Complex);
typedef s32 __func_type_0x1a74f40();
typedef void* __func_type_0x1a74f98(void*, void*, u64);
typedef u64 __func_type_0x1a75008(s32, void*, u64);
typedef s64 __func_type_0x1a75078(u8*);
typedef s64 __func_type_0x1a75118(s32, u8*);
typedef s64 __func_type_0x1a751c0(s64, u8*);
typedef s64 __func_type_0x1a75228(r32, u8*);
typedef s64 __func_type_0x1a752d0(r64, u8*);
typedef s64 __func_type_0x1a75338(bool);
typedef s64 __func_type_0x1a75398(s32);
typedef s64 __func_type_0x1a753f8(s64);
typedef s64 __func_type_0x1a75458(r32);
typedef s64 __func_type_0x1a754b8(r64);
typedef s64 __func_type_0x1a75518(u8*, s64);
typedef s64 __func_type_0x1a75580(struct string);
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct Complex{r32 r;r32 i;} Complex;

void* calloc(u64 count, u64 size);
void free(void* block);
r32 cosf(r32 v);
r32 sinf(r32 v);
void separate(struct Complex* a, s64 n);
struct Complex complex_mul(struct Complex l, struct Complex r);
struct Complex complex_sum(struct Complex l, struct Complex r);
struct Complex complex_sub(struct Complex l, struct Complex r);
void fft2(struct Complex* data, s64 length);
char* __string_data_0 = "\n";
char* __string_data_1 = "\n";
char* __string_data_2 = "\n";
s32 __main();
void* memcpy(void* dest, void* src, u64 size);
u64 write(s32 fd, void* buf, u64 count);
s64 string_length(u8* s);
s64 s32_to_str(s32 val, char buffer[16]);
s64 s64_to_str(s64 val, char buffer[32]);
s64 r32_to_str(r32 v, char buffer[32]);
s64 r64_to_str(r64 v, char buffer[64]);
s64 print_bool(bool v);
s64 print_s32(s32 v);
s64 print_s64(s64 v);
s64 print_r32(r32 v);
s64 print_r64(r64 v);
s64 print_string_c(u8* s);
s64 print_string_l(u8* s, s64 length);
s64 print_string(struct string s);

void separate(struct Complex* a, s64 n){
struct Complex* b;
b = ((struct Complex*)(calloc)(((u64)((n)/0x2)),0x8));
{
s64 i;
i = 0x0;
while(((i)<((n)/0x2))){
*(struct Complex*)(((char*)b)+ 8 * (i)) = *(struct Complex*)(((char*)a)+ 8 * (((((i)*0x2))+0x1)));
i = ((i)+0x1);
}
loop_0:;

}
{
s64 i;
i = 0x0;
while(((i)<((n)/0x2))){
*(struct Complex*)(((char*)a)+ 8 * (i)) = *(struct Complex*)(((char*)a)+ 8 * (((i)*0x2)));
i = ((i)+0x1);
}
loop_1:;

}
{
s64 i;
i = 0x0;
while(((i)<((n)/0x2))){
*(struct Complex*)(((char*)a)+ 8 * (((i)+((n)/0x2)))) = *(struct Complex*)(((char*)b)+ 8 * (i));
i = ((i)+0x1);
}
loop_2:;

}
(free)(((void*)b));
}

struct Complex complex_mul(struct Complex l, struct Complex r){
struct Complex result = {0};

((result).r) = ((((((l).r))*((r).r)))-((((l).i))*((r).i)));
((result).i) = ((((((l).r))*((r).i)))+((((l).i))*((r).r)));
return result;
}

struct Complex complex_sum(struct Complex l, struct Complex r){
struct Complex result = {0};

((result).r) = ((((l).r))+((r).r));
((result).i) = ((((l).i))+((r).i));
return result;
}

struct Complex complex_sub(struct Complex l, struct Complex r){
struct Complex result = {0};

((result).r) = ((((l).r))-((r).r));
((result).i) = ((((l).i))-((r).i));
return result;
}

void fft2(struct Complex* data, s64 length){

if(((length)>=0x2)){
(separate)(data,length);
(fft2)(data,((length)/0x2));
(fft2)(((data)+((length)/0x2)),((length)/0x2));
{
s64 k;
k = 0x0;
while(((k)<((length)/0x2))){
struct Complex e;
e = *(struct Complex*)(((char*)data)+ 8 * (k));

struct Complex o;
o = *(struct Complex*)(((char*)data)+ 8 * (((k)+((length)/0x2))));

struct Complex w = {0};

((w).r) = (cosf)(((((2.000000)*3.141593))*((r32)((k)/length))));
((w).i) = (-(sinf)(((((2.000000)*3.141593))*((r32)((k)/length)))));
*(struct Complex*)(((char*)data)+ 8 * (k)) = (complex_sum)(e,(complex_mul)(w,o));
*(struct Complex*)(((char*)data)+ 8 * (((k)+((length)/0x2)))) = (complex_sub)(e,(complex_mul)(w,o));
k = ((k)+0x1);
}
loop_3:;

}
}
}

s32 __main(){
char matrix[32];
{
char* __t_base = (char*)matrix;
char* __struct_base = __t_base;
__struct_base = __t_base;
{
char* __t_base = __struct_base;
*(r32*)((char*)__t_base + 0) = 2.000000;
*(r32*)((char*)__t_base + 4) = 0.000000;
}
__t_base += 8;
__struct_base = __t_base;
{
char* __t_base = __struct_base;
*(r32*)((char*)__t_base + 0) = 3.000000;
*(r32*)((char*)__t_base + 4) = 0.000000;
}
__t_base += 8;
__struct_base = __t_base;
{
char* __t_base = __struct_base;
*(r32*)((char*)__t_base + 0) = 4.000000;
*(r32*)((char*)__t_base + 4) = 0.000000;
}
__t_base += 8;
__struct_base = __t_base;
{
char* __t_base = __struct_base;
*(r32*)((char*)__t_base + 0) = 5.000000;
*(r32*)((char*)__t_base + 4) = 0.000000;
}
__t_base += 8;
}

(fft2)(((struct Complex*)matrix),((0x2)*0x2));
struct Complex* x;
x = ((struct Complex*)matrix);
(print_r32)(((*(struct Complex*)(((char*)x)+ 8 * (0x0))).r));
struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
(print_string)(__temp_v_0);
(print_r32)(((*(struct Complex*)(((char*)x)+ 8 * (0x1))).r));
struct string __temp_v_1;
{
char* __t_base = (char*)&(__temp_v_1);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_1;
}
(print_string)(__temp_v_1);
(print_r32)(((*(struct Complex*)(((char*)x)+ 8 * (0x2))).r));
struct string __temp_v_2;
{
char* __t_base = (char*)&(__temp_v_2);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_2;
}
(print_string)(__temp_v_2);
(print_r32)(((*(struct Complex*)(((char*)x)+ 8 * (0x3))).r));
return 0x0;
}

s64 string_length(u8* s){
s64 c;
c = 0x0;
while((((*s))!=0x0)){
c = ((c)+0x1);
s = ((s)+0x1);
}
loop_4:;

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
loop_5:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)(((char*)buffer)+ 1 * (sum)))),((void*)((auxbuffer)+0x1)),((u64)size));
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
loop_6:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)(((char*)buffer)+ 1 * (sum)))),((void*)((auxbuffer)+0x1)),((u64)size));
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
*(u8*)(((char*)buffer)+ 1 * (l)) = 0x2e;
l = ((l)+0x1);
if(((fractional_part)==0.000000)){
*(u8*)(((char*)buffer)+ 1 * (l)) = 0x30;
l = ((l)+0x1);
return l;
}
while(((((precision)>0x0))&&((fractional_part)>0.000000))){
fractional_part = ((fractional_part)*10.000000);
*(u8*)(((char*)buffer)+ 1 * (l)) = ((((u8)fractional_part))+0x30);
fractional_part = ((fractional_part)-((r32)((s32)fractional_part)));
l = ((l)+0x1);
}
loop_7:;

return l;
}

s64 r64_to_str(r64 v, char buffer[64]){
s64 l;
l = (s64_to_str)(((s64)v),((u8*)((u8*)buffer)));
s64 precision;
precision = 0x1f;
r64 fractional_part;
fractional_part = ((v)-((r64)((s64)v)));
*(u8*)(((char*)buffer)+ 1 * (l)) = 0x2e;
l = ((l)+0x1);
if(((fractional_part)==0.000000)){
*(u8*)(((char*)buffer)+ 1 * (l)) = 0x30;
l = ((l)+0x1);
return l;
}
while(((((precision)>0x0))&&((fractional_part)>0.000000))){
fractional_part = ((fractional_part)*10.000000);
*(u8*)(((char*)buffer)+ 1 * (l)) = ((((u8)fractional_part))+0x30);
fractional_part = ((fractional_part)-((r64)((s64)fractional_part)));
l = ((l)+0x1);
}
loop_8:;

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


int __entry() {
	return __main();
}