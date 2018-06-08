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
typedef void* __func_type_0x686ca0(u64, u64);
typedef void __func_type_0x686d08(void*);
typedef r32 __func_type_0x686d68(r32);
typedef void __func_type_0x686e70(struct Complex*, s64);
typedef struct Complex __func_type_0x686ed8(struct Complex, struct Complex);
typedef struct Complex* __func_type_0x686f40(struct Complex*, s64, s64);
typedef s32 __func_type_0x686fb0();
typedef void* __func_type_0x687008(void*, void*, u64);
typedef u64 __func_type_0x687078(s32, void*, u64);
typedef s64 __func_type_0x6870e8(u8*);
typedef s64 __func_type_0x687188(s32, u8*);
typedef s64 __func_type_0x687230(s64, u8*);
typedef s64 __func_type_0x687298(r32, u8*);
typedef s64 __func_type_0x687340(r64, u8*);
typedef s64 __func_type_0x6873a8(bool);
typedef s64 __func_type_0x687408(s32);
typedef s64 __func_type_0x687468(s64);
typedef s64 __func_type_0x6874c8(r32);
typedef s64 __func_type_0x687528(r64);
typedef s64 __func_type_0x687588(u8*, s64);
typedef s64 __func_type_0x6875f0(struct string);
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
struct Complex* dft(struct Complex* data, s64 width, s64 height);
char* __string_data_0 = "\n";
char* __string_data_1 = "\n";
char* __string_data_2 = "\n";
char* __string_data_3 = "\n";
char* __string_data_4 = "\n";
char* __string_data_5 = "\n";
char* __string_data_6 = "\n";
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
*(struct Complex*)((char*)b + i *  8 ) = *(struct Complex*)((char*)a + ((((i)*0x2))+0x1) *  8 );
i = ((i)+0x1);
}
loop_0:;

}
{
s64 i;
i = 0x0;
while(((i)<((n)/0x2))){
*(struct Complex*)((char*)a + i *  8 ) = *(struct Complex*)((char*)a + ((i)*0x2) *  8 );
i = ((i)+0x1);
}
loop_1:;

}
{
s64 i;
i = 0x0;
while(((i)<((n)/0x2))){
*(struct Complex*)((char*)a + ((i)+((n)/0x2)) *  8 ) = *(struct Complex*)((char*)b + i *  8 );
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
e = *(struct Complex*)((char*)data + k *  8 );

struct Complex o;
o = *(struct Complex*)((char*)data + ((k)+((length)/0x2)) *  8 );

struct Complex w = {0};

((w).r) = (cosf)(((((2.000000)*3.141593))*((r32)((k)/length))));
((w).i) = (-(sinf)(((((2.000000)*3.141593))*((r32)((k)/length)))));
*(struct Complex*)((char*)data + k *  8 ) = (complex_sum)(e,(complex_mul)(w,o));
*(struct Complex*)((char*)data + ((k)+((length)/0x2)) *  8 ) = (complex_sub)(e,(complex_mul)(w,o));
k = ((k)+0x1);
}
loop_3:;

}
}
}

struct Complex* dft(struct Complex* data, s64 width, s64 height){
struct Complex* result;
result = ((struct Complex*)(calloc)(((u64)((width)*height)),0x8));
{
s64 v;
v = 0x0;
while(((v)<height)){
{
s64 u;
u = 0x0;
while(((u)<width)){
{
s64 y;
y = 0x0;
while(((y)<height)){
{
s64 x;
x = 0x0;
while(((x)<width)){
struct Complex f;
f = *(struct Complex*)((char*)data + ((((y)*width))+x) *  8 );

r32 c;
c = ((6.283185)*((r32)((((((r32)((x)*u)))/((r32)width)))+((((r32)((y)*v)))/((r32)height)))));
((*(struct Complex*)((char*)result + ((((v)*width))+u) *  8 )).r) = ((((*(struct Complex*)((char*)result + ((((v)*width))+u) *  8 )).r))+1.000000);
((*(struct Complex*)((char*)result + ((((v)*width))+u) *  8 )).i) = ((((*(struct Complex*)((char*)result + ((((v)*width))+u) *  8 )).i))+1.000000);
x = ((x)+0x1);
}
loop_7:;

}
y = ((y)+0x1);
}
loop_6:;

}
u = ((u)+0x1);
}
loop_5:;

}
v = ((v)+0x1);
}
loop_4:;

}
return result;
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
r32* x;
x = ((r32*)matrix);
(print_r32)(*(r32*)((char*)x + 0x0 *  4 ));
struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
(print_string)(__temp_v_0);
(print_r32)(*(r32*)((char*)x + 0x1 *  4 ));
struct string __temp_v_1;
{
char* __t_base = (char*)&(__temp_v_1);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_1;
}
(print_string)(__temp_v_1);
(print_r32)(*(r32*)((char*)x + 0x2 *  4 ));
struct string __temp_v_2;
{
char* __t_base = (char*)&(__temp_v_2);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_2;
}
(print_string)(__temp_v_2);
(print_r32)(*(r32*)((char*)x + 0x3 *  4 ));
struct string __temp_v_3;
{
char* __t_base = (char*)&(__temp_v_3);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_3;
}
(print_string)(__temp_v_3);
(print_r32)(*(r32*)((char*)x + 0x4 *  4 ));
struct string __temp_v_4;
{
char* __t_base = (char*)&(__temp_v_4);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_4;
}
(print_string)(__temp_v_4);
(print_r32)(*(r32*)((char*)x + 0x5 *  4 ));
struct string __temp_v_5;
{
char* __t_base = (char*)&(__temp_v_5);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_5;
}
(print_string)(__temp_v_5);
(print_r32)(*(r32*)((char*)x + 0x6 *  4 ));
struct string __temp_v_6;
{
char* __t_base = (char*)&(__temp_v_6);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_6;
}
(print_string)(__temp_v_6);
(print_r32)(*(r32*)((char*)x + 0x7 *  4 ));
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
start = auxbuffer;
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
start = auxbuffer;
while(((val)!=0x0)){
s64 rem;
rem = ((val)%0xa);
val = ((val)/0xa);
(*auxbuffer) = ((0x30)+((u8)rem));
auxbuffer = ((auxbuffer)-0x1);
}
loop_10:;

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
loop_11:;

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
loop_12:;

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