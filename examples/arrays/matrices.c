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
void __memory_copy(void* dest, void* src, u64 size) {
	for(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];
}
typedef struct string string;
typedef struct M128A M128A;
typedef struct CONTEXT CONTEXT;
typedef struct EXCEPTION_RECORD EXCEPTION_RECORD;
typedef struct EXCEPTION_POINTERS EXCEPTION_POINTERS;
typedef struct OVERLAPPED OVERLAPPED;
typedef struct SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;
typedef struct OFSTRUCT OFSTRUCT;
typedef void* __func_type_00000146CAF93D70(u64, u64);
typedef void __func_type_00000146CAF93DD8();
typedef s32 __func_type_00000146CAF93E30();
typedef s64 __func_type_00000146CAF93E88(u8*);
typedef void* __func_type_00000146CAF93EE8(void*, void*, u64);
typedef s64 __func_type_00000146CAF93F98(s64, u8*);
typedef s64 __func_type_00000146CAF94040(s32, u8*);
typedef s64 __func_type_00000146CAF940A8(r32, u8*);
typedef s64 __func_type_00000146CAF94150(r64, u8*);
typedef s64 __func_type_00000146CAF941B8(r32);
typedef s64 __func_type_00000146CAF94218(r64);
typedef s64 __func_type_00000146CAF94278(s32);
typedef s64 __func_type_00000146CAF942D8(s64);
typedef s64 __func_type_00000146CAF94338(u8*, s64);
typedef s32 __func_type_00000146CAF943A0(struct string);
typedef void* __func_type_00000146CAF94AD8(u32);
typedef bool __func_type_00000146CAF94B38(u32, void*);
typedef bool __func_type_00000146CAF94BA0(void*, void*, u32, u32*, void*);
typedef u8* __func_type_00000146CAF94C20();
typedef u32 __func_type_00000146CAF94C78();
typedef bool __func_type_00000146CAF94D10(void*, void*, u32, u32*, struct OVERLAPPED*);
typedef s32 __func_type_00000146CAF94DD0(u8*, struct OFSTRUCT*, u32);
typedef bool __func_type_00000146CAF94E40(void*);
typedef void* __func_type_00000146CAF94EE0(u8*, u32, u32, struct SECURITY_ATTRIBUTES*, u32, u32, void*);
typedef bool __func_type_00000146CAF94F70(u8*, struct SECURITY_ATTRIBUTES*);
typedef bool __func_type_00000146CAF94FD8(u8*, u8*, struct SECURITY_ATTRIBUTES*);
typedef void* __func_type_00000146CAF95048(void*, u64, u32, u32);
typedef bool __func_type_00000146CAF950C0(void*, u64, u32);
typedef void __func_type_00000146CAF95130(void*, void*, u64);
typedef void* __func_type_00000146CAF951A0(void*, u64);
typedef s64 __func_type_00000146CAF95208(void*);
typedef __func_type_00000146CAF95208* __func_type_00000146CAF95268(__func_type_00000146CAF95208* );
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct M128A{u64 Low;s64 High;} M128A;
typedef struct CONTEXT{u64 P1Home;u64 P2Home;u64 P3Home;u64 P4Home;u64 P5Home;u64 P6Home;u32 ContextFlags;u32 MxCsr;u16 SegCs;u16 SegDs;u16 SegEs;u16 SegFs;u16 SegGs;u16 SegSs;u32 EFlags;u64 Dr0;u64 Dr1;u64 Dr2;u64 Dr3;u64 Dr6;u64 Dr7;u64 Rax;u64 Rcx;u64 Rdx;u64 Rbx;u64 Rsp;u64 Rbp;u64 Rsi;u64 Rdi;u64 R8;u64 R9;u64 R10;u64 R11;u64 R12;u64 R13;u64 R14;u64 R15;u64 Rip;char Header[32];char Legacy[128];struct M128A Xmm0;struct M128A Xmm1;struct M128A Xmm2;struct M128A Xmm3;struct M128A Xmm4;struct M128A Xmm5;struct M128A Xmm6;struct M128A Xmm7;struct M128A Xmm8;struct M128A Xmm9;struct M128A Xmm10;struct M128A Xmm11;struct M128A Xmm12;struct M128A Xmm13;struct M128A Xmm14;struct M128A Xmm15;char VectorRegister[416];u64 VectorControl;u64 DebugControl;u64 LastBranchToRip;u64 LastBranchFromRip;u64 LastExceptionToRip;u64 LastExceptionFromRip;} CONTEXT;
typedef struct EXCEPTION_RECORD{u32 ExceptionCode;u32 ExceptionFlags;struct EXCEPTION_RECORD* ExceptionRecord;void* ExceptionAddress;u32 NumberParameters;char ExceptionInformation[120];} EXCEPTION_RECORD;
typedef struct EXCEPTION_POINTERS{struct EXCEPTION_RECORD* ExceptionRecord;struct CONTEXT* ContextRecord;} EXCEPTION_POINTERS;
typedef struct OVERLAPPED{u64* Internal;u64* InternalHigh;void* Pointer;void* hEvent;} OVERLAPPED;
typedef struct SECURITY_ATTRIBUTES{u32 nLength;void* lpSecurityDescriptor;bool bInheritHandle;} SECURITY_ATTRIBUTES;
typedef struct OFSTRUCT{u8 cBytes;u8 fFixedDisk;u16 nErrCode;u16 Reserved1;u16 Reserved2;char szPathName[128];} OFSTRUCT;

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
void* stdout;
s64 string_length(u8* s);
void* memcpy(void* dest, void* src, u64 size);
s64 s64_to_str(s64 val, char buffer[32]);
s64 s32_to_str(s32 val, char buffer[16]);
s64 r32_to_str(r32 v, char buffer[32]);
s64 r64_to_str(r64 v, char buffer[64]);
s64 print_r32(r32 v);
s64 print_r64(r64 v);
s64 print_s32(s32 v);
s64 print_s64(s64 v);
s64 print_string_l(u8* data, s64 length);
s64 print_cstr(u8* data);
s32 print_string(struct string s);
void* GetStdHandle(u32 std_handle);
bool SetStdHandle(u32 std_handle, void* handle);
bool WriteConsoleA(void* handle_console_output, void* buffer, u32 number_chars_to_write, u32* number_chars_written, void* reserved);
bool WriteConsoleW(void* handle_console_output, void* buffer, u32 number_chars_to_write, u32* number_chars_written, void* reserved);
bool ReadConsole(void* console_input, void* buffer, u32 number_chars_to_read, u32* number_chars_read, void* input_control);
u8* GetCommandLineA();
u32 GetLastError();
bool ReadFile(void* handle_file, void* buffer, u32 number_bytes_to_read, u32* number_bytes_read, struct OVERLAPPED* overlapped);
bool WriteFile(void* handle_file, void* buffer, u32 number_bytes_to_write, u32* number_bytes_written, struct OVERLAPPED* overlapped);
s32 OpenFileA(u8* filename, struct OFSTRUCT* reopen_buff, u32 ustyle);
bool CloseHandle(void* handle_object);
void* CreateFileA(u8* filename, u32 desired_access, u32 share_mode, struct SECURITY_ATTRIBUTES* security_attribs, u32 creation_disposition, u32 flags_and_attribs, void* handle_template_file);
bool CreateDirectory(u8* pathname, struct SECURITY_ATTRIBUTES* security_attribs);
bool CreateDirectoryEx(u8* template_directory, u8* new_directory, struct SECURITY_ATTRIBUTES* security_attribs);
void* VirtualAlloc(void* address, u64 size, u32 allocation_type, u32 protect);
bool VirtualFree(void* address, u64 size, u32 free_type);
void MoveMemory(void* destination, void* source, u64 length);
void CopyMemory(void* destination, void* source, u64 length);
void* SecureZeroMemory(void* ptr, u64 count);
typedef s64 __ret_SetUnhandledExceptionFilter(void*);
__ret_SetUnhandledExceptionFilter* SetUnhandledExceptionFilter(__func_type_00000146CAF95208* top_level_exception_filter);

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

void* memcpy(void* dest, void* src, u64 size){
u64 i;
i = 0x0;
while(((i)<size)){
(*((u8*)((dest)+i))) = (*((u8*)((src)+i)));
i = ((i)+0x1);
}
loop_9:;

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
loop_10:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)((char*)buffer + sum *  1 ))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
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
*(u8*)((char*)buffer + 0x0 *  1 ) = 0x2d;
sum = 0x1;
}
u8* auxbuffer;
auxbuffer = (&*(u8*)((char*)b + 0xf *  1 ));
u8* start;
start = ((auxbuffer)+sum);
while(((val)!=0x0)){
s32 rem;
rem = ((val)%0xa);
val = ((val)/0xa);
(*auxbuffer) = ((0x30)+((u8)rem));
auxbuffer = ((auxbuffer)-0x1);
}
loop_11:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)((char*)buffer + sum *  1 ))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
}

s64 r32_to_str(r32 v, char buffer[32]){
s64 l;
l = (s32_to_str)(((s32)v),((u8*)((u8*)buffer)));
s64 precision;
precision = 0x4;
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
precision = ((precision)-0x1);
}
loop_12:;

return l;
}

s64 r64_to_str(r64 v, char buffer[64]){
s64 l;
l = (s64_to_str)(((s64)v),((u8*)((u8*)buffer)));
s64 precision;
precision = 0x1f;
if(((v)<0.000000)){
v = (-v);
}
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
loop_13:;

return l;
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

s64 print_string_l(u8* data, s64 length){
u32 written = 0;

(WriteConsoleA)(stdout,((void*)data),((u32)length),(&written),((void*)0x0));
return ((s64)written);
}

s64 print_cstr(u8* data){
u32 length;
length = ((u32)(string_length)(data));
u32 written = 0;

(WriteConsoleA)(stdout,((void*)data),length,(&written),((void*)0x0));
return ((s64)written);
}

s32 print_string(struct string s){
u32 written = 0;

(WriteConsoleA)(stdout,((void*)((s).data)),((u32)((s).length)),(&written),((void*)0x0));
return ((s32)written);
}


void __entry() {
	stdout = (GetStdHandle)((-0xb));
	ExitProcess(__main());
}