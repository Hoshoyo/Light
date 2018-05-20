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
typedef s32 __func_type_00000149DEA87D80();
typedef s64 __func_type_00000149DEA87DD8(u8*);
typedef void* __func_type_00000149DEA87E38(void*, void*, u64);
typedef s64 __func_type_00000149DEA87EE8(s32, u8*);
typedef s64 __func_type_00000149DEA87F50(s32);
typedef s64 __func_type_00000149DEA87FB0(u8*, s64);
typedef s32 __func_type_00000149DEA88018(struct string);
typedef void* __func_type_00000149DEA88750(u32);
typedef bool __func_type_00000149DEA887B0(u32, void*);
typedef bool __func_type_00000149DEA88818(void*, void*, u32, u32*, void*);
typedef u8* __func_type_00000149DEA88898();
typedef u32 __func_type_00000149DEA888F0();
typedef bool __func_type_00000149DEA88988(void*, void*, u32, u32*, struct OVERLAPPED*);
typedef s32 __func_type_00000149DEA88A48(u8*, struct OFSTRUCT*, u32);
typedef bool __func_type_00000149DEA88AB8(void*);
typedef void* __func_type_00000149DEA88B58(u8*, u32, u32, struct SECURITY_ATTRIBUTES*, u32, u32, void*);
typedef bool __func_type_00000149DEA88BE8(u8*, struct SECURITY_ATTRIBUTES*);
typedef bool __func_type_00000149DEA88C50(u8*, u8*, struct SECURITY_ATTRIBUTES*);
typedef void* __func_type_00000149DEA88CC0(void*, u64, u32, u32);
typedef bool __func_type_00000149DEA88D38(void*, u64, u32);
typedef void __func_type_00000149DEA88DA8(void*, void*, u64);
typedef void* __func_type_00000149DEA88E18(void*, u64);
typedef s64 __func_type_00000149DEA88E80(void*);
typedef __func_type_00000149DEA88E80* __func_type_00000149DEA88EE0(__func_type_00000149DEA88E80* );
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct M128A{u64 Low;s64 High;} M128A;
typedef struct CONTEXT{u64 P1Home;u64 P2Home;u64 P3Home;u64 P4Home;u64 P5Home;u64 P6Home;u32 ContextFlags;u32 MxCsr;u16 SegCs;u16 SegDs;u16 SegEs;u16 SegFs;u16 SegGs;u16 SegSs;u32 EFlags;u64 Dr0;u64 Dr1;u64 Dr2;u64 Dr3;u64 Dr6;u64 Dr7;u64 Rax;u64 Rcx;u64 Rdx;u64 Rbx;u64 Rsp;u64 Rbp;u64 Rsi;u64 Rdi;u64 R8;u64 R9;u64 R10;u64 R11;u64 R12;u64 R13;u64 R14;u64 R15;u64 Rip;char Header[32];char Legacy[128];struct M128A Xmm0;struct M128A Xmm1;struct M128A Xmm2;struct M128A Xmm3;struct M128A Xmm4;struct M128A Xmm5;struct M128A Xmm6;struct M128A Xmm7;struct M128A Xmm8;struct M128A Xmm9;struct M128A Xmm10;struct M128A Xmm11;struct M128A Xmm12;struct M128A Xmm13;struct M128A Xmm14;struct M128A Xmm15;char VectorRegister[416];u64 VectorControl;u64 DebugControl;u64 LastBranchToRip;u64 LastBranchFromRip;u64 LastExceptionToRip;u64 LastExceptionFromRip;} CONTEXT;
typedef struct EXCEPTION_RECORD{u32 ExceptionCode;u32 ExceptionFlags;struct EXCEPTION_RECORD* ExceptionRecord;void* ExceptionAddress;u32 NumberParameters;char ExceptionInformation[120];} EXCEPTION_RECORD;
typedef struct EXCEPTION_POINTERS{struct EXCEPTION_RECORD* ExceptionRecord;struct CONTEXT* ContextRecord;} EXCEPTION_POINTERS;
typedef struct OVERLAPPED{u64* Internal;u64* InternalHigh;void* Pointer;void* hEvent;} OVERLAPPED;
typedef struct SECURITY_ATTRIBUTES{u32 nLength;void* lpSecurityDescriptor;bool bInheritHandle;} SECURITY_ATTRIBUTES;
typedef struct OFSTRUCT{u8 cBytes;u8 fFixedDisk;u16 nErrCode;u16 Reserved1;u16 Reserved2;char szPathName[128];} OFSTRUCT;

char* __string_data_0 = "Hello\xE3\x81\x82\"\n\n\n";
s32 __main();
void* stdout;
s64 string_length(u8* s);
void* memcpy(void* dest, void* src, u64 size);
s64 s32_to_str(s32 val, char buffer[16]);
s64 print_s32(s32 v);
s64 print_cstr_length(u8* data, s64 length);
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
s32 OpenFile(u8* filename, struct OFSTRUCT* reopen_buff, u32 ustyle);
bool CloseHandle(void* handle_object);
void* CreateFile(u8* filename, u32 desired_access, u32 share_mode, struct SECURITY_ATTRIBUTES* security_attribs, u32 creation_disposition, u32 flags_and_attribs, void* handle_template_file);
bool CreateDirectory(u8* pathname, struct SECURITY_ATTRIBUTES* security_attribs);
bool CreateDirectoryEx(u8* template_directory, u8* new_directory, struct SECURITY_ATTRIBUTES* security_attribs);
void* VirtualAlloc(void* address, u64 size, u32 allocation_type, u32 protect);
bool VirtualFree(void* address, u64 size, u32 free_type);
void MoveMemory(void* destination, void* source, u64 length);
void CopyMemory(void* destination, void* source, u64 length);
void* SecureZeroMemory(void* ptr, u64 count);
typedef s64 __ret_SetUnhandledExceptionFilter(void*);
__ret_SetUnhandledExceptionFilter* SetUnhandledExceptionFilter(__func_type_00000149DEA88E80* top_level_exception_filter);

s32 __main(){
struct string a;
{
char* __t_base = (char*)&(a);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xc;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}

(print_string)(a);
return 0x0;
}

s64 string_length(u8* s){
s64 c;
c = 0x0;
while((((*s))!=0x0)){
c = ((c)+0x1);
s = ((s)+0x1);
}
loop_0:;

return c;
}

void* memcpy(void* dest, void* src, u64 size){
u64 i;
i = 0x0;
while(((i)<size)){
(*((u8*)((dest)+i))) = (*((u8*)((src)+i)));
i = ((i)+0x1);
}
loop_1:;

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
*(u8*)(((char*)buffer)+ 1 * (0x0)) = 0x2d;
sum = 0x1;
}
u8* auxbuffer;
auxbuffer = (&*(u8*)(((char*)b)+ 1 * (0xf)));
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

s64 print_s32(s32 v){
char buffer[16] = {0};

s64 len;
len = (s32_to_str)(v,buffer);
return (print_cstr_length)(((u8*)buffer),len);
}

s64 print_cstr_length(u8* data, s64 length){
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