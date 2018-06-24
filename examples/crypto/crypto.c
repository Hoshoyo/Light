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
typedef void __func_type_0x2058d20(struct string, u32*);
typedef void __func_type_0x2058d88(struct string);
typedef u32 __func_type_0x2058de8(u32, u32);
typedef u32 __func_type_0x2058e50(u32*, u32);
typedef void __func_type_0x2058eb8(u32*, u32, u32*, u32, u32, u32*, u64);
typedef void __func_type_0x2058f48(u32*, u32*, u64*);
typedef s32 __func_type_0x2058fb8();
typedef void* __func_type_0x2059010(void*, void*, u64);
typedef s64 __func_type_0x2059080(u8*);
typedef s64 __func_type_0x2059120(s32, u8*);
typedef s64 __func_type_0x2059188(u32, bool, s8*);
typedef s64 __func_type_0x2059238(s64, u8*);
typedef s64 __func_type_0x20592a0(r32, u8*);
typedef s64 __func_type_0x2059348(r64, u8*);
typedef s64 __func_type_0x20593b0(bool);
typedef s64 __func_type_0x2059410(u32);
typedef s64 __func_type_0x2059470(s32);
typedef s64 __func_type_0x20594d0(s64);
typedef s64 __func_type_0x2059530(r32);
typedef s64 __func_type_0x2059590(r64);
typedef s64 __func_type_0x20595f0(u8*, s64);
typedef s64 __func_type_0x2059658(struct string);
typedef u64 __func_type_0x2059828(s32, void*, u64);
typedef s64 __func_type_0x2059898(s32, void*, u64);
typedef s32 __func_type_0x2059908(u8*, s32);
typedef s32 __func_type_0x2059970(s32);
typedef s32 __func_type_0x2059a10(s32, struct stat*);
typedef void* __func_type_0x2059a78(u64);
typedef void* __func_type_0x2059ad8(struct string, __func_type_0x2059a78* , u64*);
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct timespec{s64 tv_sec;s64 tv_nsec;} timespec;
typedef struct stat{u64 st_dev;u64 st_ino;u64 st_nlink;u32 st_mode;u32 st_uid;u32 st_gid;u64 st_rdev;s64 st_size;s64 st_blksize;s64 st_blocks;struct timespec st_atim;struct timespec st_mtim;struct timespec st_ctim;char __glibc_reserved[24];} stat;

char digest[20];
u64 transforms;
void buffer_to_block(struct string buffer, char block[64]);
void sha1(struct string buffer);
u32 rol(u32 value, u32 bits);
u32 blk(char block[64], u32 i);
void R0(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i);
void R1(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i);
void R2(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i);
void R3(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i);
void R4(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i);
void transform(char digest[20], char block[64], u64* transforms);
s32 __main();
void* memcpy(void* dest, void* src, u64 size);
s64 string_length(u8* s);
s64 s32_to_str(s32 val, char buffer[16]);
s64 u32_to_str_base16(u32 val, bool leading_zeros, s8* buffer);
s64 s64_to_str(s64 val, char buffer[32]);
s64 r32_to_str(r32 v, char buffer[32]);
s64 r64_to_str(r64 v, char buffer[64]);
s64 print_bool(bool v);
char* __string_data_0 = "0x";
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
void* read_entire_file(struct string filename, __func_type_0x2059a78* allocator, u64* out_file_size);

void buffer_to_block(struct string buffer, char block[64]){
{
u64 i;
i = 0x0;
while(((i)<0x10)){
*(u32*)((char*)block + i *  4 ) = ((((((((u32)((*(u8*)((char*)((buffer).data) + ((((0x4)*i))+0x3) *  1 ))&0xff)))|((((u32)((*(u8*)((char*)((buffer).data) + ((((0x4)*i))+0x2) *  1 ))&0xff)))<<0x8)))|((((u32)((*(u8*)((char*)((buffer).data) + ((((0x4)*i))+0x1) *  1 ))&0xff)))<<0x10)))|((((u32)((*(u8*)((char*)((buffer).data) + ((((0x4)*i))+0x0) *  1 ))&0xff)))<<0x18));
i = ((i)+0x1);
}
loop_0:;

}
}

void sha1(struct string buffer){
u64 length;
length = 0x0;
u64 count;
count = 0x0;
u64 total_bits;
total_bits = ((((((transforms)*((0x10)*0x4)))+((u64)((buffer).length))))*0x8);
*(u8*)((char*)((buffer).data) + ((buffer).length) *  1 ) = 0x80;
((buffer).length) = ((((buffer).length))+0x1);
u64 orig_size;
orig_size = ((u64)((buffer).length));
while(((((u64)((buffer).length)))<((0x10)*0x4))){
*(u8*)((char*)((buffer).data) + ((buffer).length) *  1 ) = 0x0;
((buffer).length) = ((((buffer).length))+0x1);
}
loop_1:;

char block[64] = {0};

(buffer_to_block)(buffer,block);
if(((orig_size)>((((0x10)*0x4))-0x8))){
(transform)(digest,block,(&transforms));
{
u64 i;
i = 0x0;
while(((i)<((0x10)-0x2))){
*(u32*)((char*)block + i *  4 ) = 0x0;
i = ((i)+0x1);
}
loop_2:;

}
}
*(u32*)((char*)block + ((0x10)-0x1) *  4 ) = ((u32)total_bits);
*(u32*)((char*)block + ((0x10)-0x2) *  4 ) = ((u32)((total_bits)>>0x20));
(transform)(digest,block,(&transforms));
char result_buffer[8] = {0};

{
u64 i;
i = 0x0;
while(((i)<0x5)){
(u32_to_str_base16)(*(u32*)((char*)digest + i *  4 ),true,((s8*)result_buffer));
(print_string_l)(((u8*)result_buffer),0x8);
i = ((i)+0x1);
}
loop_3:;

}
}

u32 rol(u32 value, u32 bits){
return ((((value)<<bits))|((value)>>((0x20)-bits)));
}

u32 blk(char block[64], u32 i){
return (rol)(((((((*(u32*)((char*)block + ((((i)+0xd))&0xf) *  4 ))^*(u32*)((char*)block + ((((i)+0x8))&0xf) *  4 )))^*(u32*)((char*)block + ((((i)+0x2))&0xf) *  4 )))^*(u32*)((char*)block + i *  4 )),0x1);
}

void R0(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i){
(*z) = (((*z))+(((((((((((*w))&((x)^y)))^y))+*(u32*)((char*)block + i *  4 )))+0x5a827999))+(rol)(v,0x5)));
(*w) = (rol)((*w),0x1e);
}

void R1(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i){
*(u32*)((char*)block + i *  4 ) = (blk)(block,((u32)i));
(*z) = (((*z))+(((((((((((*w))&((x)^y)))^y))+*(u32*)((char*)block + i *  4 )))+0x5a827999))+(rol)(v,0x5)));
(*w) = (rol)((*w),0x1e);
}

void R2(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i){
*(u32*)((char*)block + i *  4 ) = (blk)(block,((u32)i));
(*z) = (((*z))+(((((((((((*w))^x))^y))+*(u32*)((char*)block + i *  4 )))+0x6ed9eba1))+(rol)(v,0x5)));
(*w) = (rol)((*w),0x1e);
}

void R3(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i){
*(u32*)((char*)block + i *  4 ) = (blk)(block,((u32)i));
(*z) = (((*z))+(((((((((((((*w))|x))&y))|(((*w))&x)))+*(u32*)((char*)block + i *  4 )))+0x8f1bbcdc))+(rol)(v,0x5)));
(*w) = (rol)((*w),0x1e);
}

void R4(char block[64], u32 v, u32* w, u32 x, u32 y, u32* z, u64 i){
*(u32*)((char*)block + i *  4 ) = (blk)(block,((u32)i));
(*z) = (((*z))+(((((((((((*w))^x))^y))+*(u32*)((char*)block + i *  4 )))+0xca62c1d6))+(rol)(v,0x5)));
(*w) = (rol)((*w),0x1e);
}

void transform(char digest[20], char block[64], u64* transforms){
u32 a;
a = *(u32*)((char*)digest + 0x0 *  4 );
u32 b;
b = *(u32*)((char*)digest + 0x1 *  4 );
u32 c;
c = *(u32*)((char*)digest + 0x2 *  4 );
u32 d;
d = *(u32*)((char*)digest + 0x3 *  4 );
u32 e;
e = *(u32*)((char*)digest + 0x4 *  4 );
(R0)(block,a,(&b),c,d,(&e),0x0);
(R0)(block,e,(&a),b,c,(&d),0x1);
(R0)(block,d,(&e),a,b,(&c),0x2);
(R0)(block,c,(&d),e,a,(&b),0x3);
(R0)(block,b,(&c),d,e,(&a),0x4);
(R0)(block,a,(&b),c,d,(&e),0x5);
(R0)(block,e,(&a),b,c,(&d),0x6);
(R0)(block,d,(&e),a,b,(&c),0x7);
(R0)(block,c,(&d),e,a,(&b),0x8);
(R0)(block,b,(&c),d,e,(&a),0x9);
(R0)(block,a,(&b),c,d,(&e),0xa);
(R0)(block,e,(&a),b,c,(&d),0xb);
(R0)(block,d,(&e),a,b,(&c),0xc);
(R0)(block,c,(&d),e,a,(&b),0xd);
(R0)(block,b,(&c),d,e,(&a),0xe);
(R0)(block,a,(&b),c,d,(&e),0xf);
(R1)(block,e,(&a),b,c,(&d),0x0);
(R1)(block,d,(&e),a,b,(&c),0x1);
(R1)(block,c,(&d),e,a,(&b),0x2);
(R1)(block,b,(&c),d,e,(&a),0x3);
(R2)(block,a,(&b),c,d,(&e),0x4);
(R2)(block,e,(&a),b,c,(&d),0x5);
(R2)(block,d,(&e),a,b,(&c),0x6);
(R2)(block,c,(&d),e,a,(&b),0x7);
(R2)(block,b,(&c),d,e,(&a),0x8);
(R2)(block,a,(&b),c,d,(&e),0x9);
(R2)(block,e,(&a),b,c,(&d),0xa);
(R2)(block,d,(&e),a,b,(&c),0xb);
(R2)(block,c,(&d),e,a,(&b),0xc);
(R2)(block,b,(&c),d,e,(&a),0xd);
(R2)(block,a,(&b),c,d,(&e),0xe);
(R2)(block,e,(&a),b,c,(&d),0xf);
(R2)(block,d,(&e),a,b,(&c),0x0);
(R2)(block,c,(&d),e,a,(&b),0x1);
(R2)(block,b,(&c),d,e,(&a),0x2);
(R2)(block,a,(&b),c,d,(&e),0x3);
(R2)(block,e,(&a),b,c,(&d),0x4);
(R2)(block,d,(&e),a,b,(&c),0x5);
(R2)(block,c,(&d),e,a,(&b),0x6);
(R2)(block,b,(&c),d,e,(&a),0x7);
(R3)(block,a,(&b),c,d,(&e),0x8);
(R3)(block,e,(&a),b,c,(&d),0x9);
(R3)(block,d,(&e),a,b,(&c),0xa);
(R3)(block,c,(&d),e,a,(&b),0xb);
(R3)(block,b,(&c),d,e,(&a),0xc);
(R3)(block,a,(&b),c,d,(&e),0xd);
(R3)(block,e,(&a),b,c,(&d),0xe);
(R3)(block,d,(&e),a,b,(&c),0xf);
(R3)(block,c,(&d),e,a,(&b),0x0);
(R3)(block,b,(&c),d,e,(&a),0x1);
(R3)(block,a,(&b),c,d,(&e),0x2);
(R3)(block,e,(&a),b,c,(&d),0x3);
(R3)(block,d,(&e),a,b,(&c),0x4);
(R3)(block,c,(&d),e,a,(&b),0x5);
(R3)(block,b,(&c),d,e,(&a),0x6);
(R3)(block,a,(&b),c,d,(&e),0x7);
(R3)(block,e,(&a),b,c,(&d),0x8);
(R3)(block,d,(&e),a,b,(&c),0x9);
(R3)(block,c,(&d),e,a,(&b),0xa);
(R3)(block,b,(&c),d,e,(&a),0xb);
(R4)(block,a,(&b),c,d,(&e),0xc);
(R4)(block,e,(&a),b,c,(&d),0xd);
(R4)(block,d,(&e),a,b,(&c),0xe);
(R4)(block,c,(&d),e,a,(&b),0xf);
(R4)(block,b,(&c),d,e,(&a),0x0);
(R4)(block,a,(&b),c,d,(&e),0x1);
(R4)(block,e,(&a),b,c,(&d),0x2);
(R4)(block,d,(&e),a,b,(&c),0x3);
(R4)(block,c,(&d),e,a,(&b),0x4);
(R4)(block,b,(&c),d,e,(&a),0x5);
(R4)(block,a,(&b),c,d,(&e),0x6);
(R4)(block,e,(&a),b,c,(&d),0x7);
(R4)(block,d,(&e),a,b,(&c),0x8);
(R4)(block,c,(&d),e,a,(&b),0x9);
(R4)(block,b,(&c),d,e,(&a),0xa);
(R4)(block,a,(&b),c,d,(&e),0xb);
(R4)(block,e,(&a),b,c,(&d),0xc);
(R4)(block,d,(&e),a,b,(&c),0xd);
(R4)(block,c,(&d),e,a,(&b),0xe);
(R4)(block,b,(&c),d,e,(&a),0xf);
*(u32*)((char*)digest + 0x0 *  4 ) = ((*(u32*)((char*)digest + 0x0 *  4 ))+a);
*(u32*)((char*)digest + 0x1 *  4 ) = ((*(u32*)((char*)digest + 0x1 *  4 ))+b);
*(u32*)((char*)digest + 0x2 *  4 ) = ((*(u32*)((char*)digest + 0x2 *  4 ))+c);
*(u32*)((char*)digest + 0x3 *  4 ) = ((*(u32*)((char*)digest + 0x3 *  4 ))+d);
*(u32*)((char*)digest + 0x4 *  4 ) = ((*(u32*)((char*)digest + 0x4 *  4 ))+e);
transforms = ((transforms)+0x1);
}

s32 __main(){
char sa[256] = {0};

*(u8*)((char*)sa + 0x0 *  1 ) = 0x61;
*(u8*)((char*)sa + 0x1 *  1 ) = 0x62;
*(u8*)((char*)sa + 0x2 *  1 ) = 0x63;
struct string s;
{
char* __t_base = (char*)&(s);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x3;
*(s64*)((char*)__t_base + 8) = (-0x1);
*(u8**)((char*)__t_base + 16) = sa;
}

(sha1)(s);
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
loop_5:;

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
if((((!((bool)((auxmask)&val))))&&(!leading_zeros)))goto loop_6;
u32 v;
v = ((((val)&((mask)<<((count)*0x4))))>>((count)*0x4));
if(((v)>=0xa))v = ((v)+0x37); else v = ((v)+0x30);
(*at) = ((s8)v);
at = ((at)-0x1);
auxmask = ((auxmask)&(((~mask))<<((count)*0x4)));
count = ((count)+0x1);
}
loop_6:;

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
loop_7:;

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
loop_8:;

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
loop_9:;

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
loop_10:;

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

struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x2;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
(print_string)(__temp_v_0);
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

void* read_entire_file(struct string filename, __func_type_0x2059a78* allocator, u64* out_file_size){
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
{
char* __t_base = (char*)digest;
char* __struct_base = __t_base;
*(u32*)__t_base = 0x67452301;
__t_base += 4;
*(u32*)__t_base = 0xefcdab89;
__t_base += 4;
*(u32*)__t_base = 0x98badcfe;
__t_base += 4;
*(u32*)__t_base = 0x10325476;
__t_base += 4;
*(u32*)__t_base = 0xc3d2e1f0;
__t_base += 4;
}
;
	transforms = 0x0;
	return __main();
}