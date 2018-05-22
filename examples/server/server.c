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
typedef struct in_addr in_addr;
typedef struct sockaddr_in sockaddr_in;
typedef s32 __func_type_0x15fcdb8(s32, s32, s32);
typedef s32 __func_type_0x15fce68(s32, struct sockaddr_in*, u32);
typedef s32 __func_type_0x15fced8(s32, s32);
typedef s32 __func_type_0x15fcf40(s32, struct sockaddr_in*, u32*);
typedef s64 __func_type_0x15fcfb0(s32, void*, u64, s32);
typedef u16 __func_type_0x15fd028(u16);
typedef s32 __func_type_0x15fd088();
typedef void* __func_type_0x15fd0e0(void*, void*, u64);
typedef u64 __func_type_0x15fd150(s32, void*, u64);
typedef s64 __func_type_0x15fd1c0(u8*);
typedef s64 __func_type_0x15fd260(s32, u8*);
typedef s64 __func_type_0x15fd308(s64, u8*);
typedef s64 __func_type_0x15fd370(r32);
typedef s64 __func_type_0x15fd3d0(bool);
typedef s64 __func_type_0x15fd430(s32);
typedef s64 __func_type_0x15fd490(s64);
typedef s64 __func_type_0x15fd4f0(u8*, s64);
typedef s64 __func_type_0x15fd558(struct string);
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct in_addr{u32 s_addr;} in_addr;
typedef struct sockaddr_in{u16 sin_family;u16 sin_port;struct in_addr sin_addr;char sin_zero[8];} sockaddr_in;

s32 socket(s32 domain, s32 type, s32 protocol);
s32 bind(s32 fd, struct sockaddr_in* addr, u32 len);
s32 listen(s32 fd, s32 n);
s32 accept(s32 fd, struct sockaddr_in* addr, u32* addr_len);
s64 recv(s32 fd, void* buf, u64 n, s32 flags);
s64 send(s32 fd, void* buf, u64 n, s32 flags);
u16 htons(u16 x);
char* __string_data_0 = "Could not create socket";
char* __string_data_1 = "Socket created";
char* __string_data_2 = "bind failed\n";
char* __string_data_3 = "Waiting for connections...\n";
char* __string_data_4 = "accept failed\n";
char* __string_data_5 = "Connection accepted!\n";
char* __string_data_6 = "Client disconnected\n";
char* __string_data_7 = "recv failed";
s32 __main();
void* memcpy(void* dest, void* src, u64 size);
u64 write(s32 fd, void* buf, u64 count);
s64 string_length(u8* s);
s64 s32_to_str(s32 val, char buffer[16]);
s64 s64_to_str(s64 val, char buffer[32]);
s64 r32_to_str(r32 v);
s64 print_bool(bool v);
s64 print_s32(s32 v);
s64 print_s64(s64 v);
s64 print_string_c(u8* s);
s64 print_string_l(u8* s, s64 length);
s64 print_string(struct string s);

u16 htons(u16 x){
u16 result;
result = ((u16)((((((x)>>0x8))&0xff))|((((x)&0xff))<<0x8)));
return result;
}

s32 __main(){
s32 socket_desc = 0;

s32 client_sock = 0;

s32 c;
c = 0x10;
s64 read_size;
read_size = 0x0;
struct sockaddr_in server = {0};

struct sockaddr_in client = {0};

char client_message[2048] = {0};

socket_desc = (socket)(0x2,0x1,0x0);
if(((socket_desc)==(-0x1))){
struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x17;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
(print_string)(__temp_v_0);
return (-0x1);
} else {
struct string __temp_v_1;
{
char* __t_base = (char*)&(__temp_v_1);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xe;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_1;
}
(print_string)(__temp_v_1);
}
((server).sin_family) = ((u16)0x2);
((((server).sin_addr)).s_addr) = 0x0;
((server).sin_port) = (htons)(0x22b8);
if((((bind)(socket_desc,(&server),0x10))<0x0)){
struct string __temp_v_2;
{
char* __t_base = (char*)&(__temp_v_2);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xc;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_2;
}
(print_string)(__temp_v_2);
return (-0x1);
}
(listen)(socket_desc,0x3);
struct string __temp_v_3;
{
char* __t_base = (char*)&(__temp_v_3);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1b;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_3;
}
(print_string)(__temp_v_3);
client_sock = (accept)(socket_desc,(&client),((u32*)(&c)));
if(((client_sock)<0x0)){
{
struct string __temp_v_4;
{
char* __t_base = (char*)&(__temp_v_4);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xe;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_4;
}
(print_string)(__temp_v_4);
return (-0x1);
}
}
struct string __temp_v_5;
{
char* __t_base = (char*)&(__temp_v_5);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x15;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_5;
}
(print_string)(__temp_v_5);
read_size = (recv)(client_sock,((void*)client_message),0x7d0,0x0);
while(((read_size)>0x0)){
(write)(client_sock,((void*)client_message),((u64)(string_length)(((u8*)client_message))));
read_size = (recv)(client_sock,((void*)client_message),0x7d0,0x0);
}
loop_0:;

if(((read_size)==0x0)){
{
struct string __temp_v_6;
{
char* __t_base = (char*)&(__temp_v_6);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x14;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_6;
}
(print_string)(__temp_v_6);
}
} else {
if(((read_size)==(-0x1))){
{
struct string __temp_v_7;
{
char* __t_base = (char*)&(__temp_v_7);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xb;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_7;
}
(print_string)(__temp_v_7);
}
}
}
return 0x0;
}

s64 string_length(u8* s){
s64 c;
c = 0x0;
while((((*s))!=0x0)){
c = ((c)+0x1);
s = ((s)+0x1);
}
loop_1:;

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
loop_2:;

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
loop_3:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)(((char*)buffer)+ 1 * (sum)))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
}

s64 r32_to_str(r32 v){
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