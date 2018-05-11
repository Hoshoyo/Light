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
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct hey{char foo[12];} hey;
typedef struct M128A{u64 Low;s64 High;} M128A;
typedef struct CONTEXT{u64 P1Home;u64 P2Home;u64 P3Home;u64 P4Home;u64 P5Home;u64 P6Home;u32 ContextFlags;u32 MxCsr;u16 SegCs;u16 SegDs;u16 SegEs;u16 SegFs;u16 SegGs;u16 SegSs;u32 EFlags;u64 Dr0;u64 Dr1;u64 Dr2;u64 Dr3;u64 Dr6;u64 Dr7;u64 Rax;u64 Rcx;u64 Rdx;u64 Rbx;u64 Rsp;u64 Rbp;u64 Rsi;u64 Rdi;u64 R8;u64 R9;u64 R10;u64 R11;u64 R12;u64 R13;u64 R14;u64 R15;u64 Rip;char Header[32];char Legacy[128];struct M128A Xmm0;struct M128A Xmm1;struct M128A Xmm2;struct M128A Xmm3;struct M128A Xmm4;struct M128A Xmm5;struct M128A Xmm6;struct M128A Xmm7;struct M128A Xmm8;struct M128A Xmm9;struct M128A Xmm10;struct M128A Xmm11;struct M128A Xmm12;struct M128A Xmm13;struct M128A Xmm14;struct M128A Xmm15;char VectorRegister[416];u64 VectorControl;u64 DebugControl;u64 LastBranchToRip;u64 LastBranchFromRip;u64 LastExceptionToRip;u64 LastExceptionFromRip;} CONTEXT;
typedef struct EXCEPTION_RECORD{u32 ExceptionCode;u32 ExceptionFlags;struct EXCEPTION_RECORD* ExceptionRecord;void* ExceptionAddress;u32 NumberParameters;char ExceptionInformation[120];} EXCEPTION_RECORD;
typedef struct EXCEPTION_POINTERS{struct EXCEPTION_RECORD* ExceptionRecord;struct CONTEXT* ContextRecord;} EXCEPTION_POINTERS;
typedef struct OVERLAPPED{u64* Internal;u64* InternalHigh;void* Pointer;void* hEvent;} OVERLAPPED;
typedef struct SECURITY_ATTRIBUTES{u32 nLength;void* lpSecurityDescriptor;bool bInheritHandle;} SECURITY_ATTRIBUTES;
typedef struct OFSTRUCT{u8 cBytes;u8 fFixedDisk;u16 nErrCode;u16 Reserved1;u16 Reserved2;char szPathName[128];} OFSTRUCT;

s32 foo(struct string str, struct string str2);
s32 arr(char a[12]);
char* __string_data_0 = "Hello";
char* __string_data_1 = "bar";
s32 __main();
s32 EXIT_SUCCESS = 0x0;
s32 EXIT_FAILED = 0xffffffff;
u32 STD_INPUT_HANDLE = 0xfffffff6;
u32 STD_OUTPUT_HANDLE = 0xfffffff5;
u32 STD_ERROR_HANDLE = 0xfffffff4;
u32 OF_CANCEL = 0x800;
u32 OF_CREATE = 0x1000;
u32 OF_DELETE = 0x200;
u32 OF_EXIST = 0x4000;
u32 OF_PARSE = 0x100;
u32 OF_PROMPT = 0x2000;
u32 OF_READ = 0x0;
u32 OF_READWRITE = 0x2;
u32 OF_REOPEN = 0x8000;
u32 OF_SHARE_COMPAT = 0x0;
u32 OF_SHARE_DENY_NONE = 0x40;
u32 OF_SHARE_DENY_READ = 0x30;
u32 OF_SHARE_DENY_WRITE = 0x20;
u32 OF_SHARE_EXCLUSIVE = 0x10;
u32 OF_VERIFY = 0x400;
u32 OF_WRITE = 0x1;
u32 FILE_SHARE_DELETE = 0x4;
u32 FILE_SHARE_READ = 0x1;
u32 FILE_SHARE_WRITE = 0x2;
u32 FILE_ATTRIBUTE_READONLY = 0x1;
u32 FILE_ATTRIBUTE_HIDDEN = 0x2;
u32 FILE_ATTRIBUTE_SYSTEM = 0x4;
u32 FILE_ATTRIBUTE_DIRECTORY = 0x10;
u32 FILE_ATTRIBUTE_ARCHIVE = 0x20;
u32 FILE_ATTRIBUTE_DEVICE = 0x40;
u32 FILE_ATTRIBUTE_NORMAL = 0x80;
u32 FILE_ATTRIBUTE_TEMPORARY = 0x100;
u32 FILE_ATTRIBUTE_SPARSE_FILE = 0x200;
u32 FILE_ATTRIBUTE_REPARSE_POINT = 0x400;
u32 FILE_ATTRIBUTE_COMPRESSED = 0x800;
u32 FILE_ATTRIBUTE_OFFLINE = 0x1000;
u32 FILE_ATTRIBUTE_NOT_CONTENT_INDEXED = 0x2000;
u32 FILE_ATTRIBUTE_ENCRYPTED = 0x4000;
u32 FILE_ATTRIBUTE_INTEGRITY_STREAM = 0x8000;
u32 FILE_ATTRIBUTE_VIRTUAL = 0x10000;
u32 FILE_ATTRIBUTE_NO_SCRUB_DATA = 0x20000;
u32 FILE_ATTRIBUTE_EA = 0x40000;
u32 FILE_ATTRIBUTE_PINNED = 0x80000;
u32 FILE_ATTRIBUTE_UNPINNED = 0x100000;
u32 FILE_ATTRIBUTE_RECALL_ON_OPEN = 0x40000;
u32 FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS = 0x400000;
u32 FILE_FLAG_WRITE_THROUGH = 0x80000000;
u32 FILE_FLAG_OVERLAPPED = 0x40000000;
u32 FILE_FLAG_NO_BUFFERING = 0x20000000;
u32 FILE_FLAG_RANDOM_ACCESS = 0x10000000;
u32 FILE_FLAG_SEQUENTIAL_SCAN = 0x8000000;
u32 FILE_FLAG_DELETE_ON_CLOSE = 0x4000000;
u32 FILE_FLAG_BACKUP_SEMANTICS = 0x2000000;
u32 FILE_FLAG_POSIX_SEMANTICS = 0x1000000;
u32 FILE_FLAG_SESSION_AWARE = 0x800000;
u32 FILE_FLAG_OPEN_REPARSE_POINT = 0x200000;
u32 FILE_FLAG_OPEN_NO_RECALL = 0x100000;
u32 FILE_FLAG_FIRST_PIPE_INSTANCE = 0x80000;
u32 PAGE_NOACCESS = 0x1;
u32 PAGE_READONLY = 0x2;
u32 PAGE_READWRITE = 0x4;
u32 PAGE_WRITECOPY = 0x8;
u32 PAGE_EXECUTE = 0x10;
u32 PAGE_EXECUTE_READ = 0x20;
u32 PAGE_EXECUTE_READWRITE = 0x40;
u32 PAGE_EXECUTE_WRITECOPY = 0x80;
u32 PAGE_GUARD = 0x100;
u32 PAGE_NOCACHE = 0x200;
u32 PAGE_WRITECOMBINE = 0x400;
u32 PAGE_REVERT_TO_FILE_MAP = 0x80000000;
u32 PAGE_ENCLAVE_THREAD_CONTROL = 0x80000000;
u32 PAGE_TARGETS_NO_UPDATE = 0x40000000;
u32 PAGE_TARGETS_INVALID = 0x40000000;
u32 PAGE_ENCLAVE_UNVALIDATED = 0x20000000;
u32 MEM_COMMIT = 0x1000;
u32 MEM_RESERVE = 0x2000;
u32 MEM_DECOMMIT = 0x4000;
u32 MEM_RELEASE = 0x8000;
u32 MEM_FREE = 0x10000;
u32 MEM_PRIVATE = 0x20000;
u32 MEM_MAPPED = 0x40000;
u32 MEM_RESET = 0x80000;
u32 MEM_TOP_DOWN = 0x100000;
u32 MEM_WRITE_WATCH = 0x200000;
u32 MEM_PHYSICAL = 0x400000;
u32 MEM_ROTATE = 0x800000;
u32 MEM_DIFFERENT_IMAGE_BASE_OK = 0x800000;
u32 MEM_RESET_UNDO = 0x1000000;
u32 MEM_LARGE_PAGES = 0x20000000;
u32 MEM_4MB_PAGES = 0x80000000;
u32 SEC_64K_PAGES = 0x80000;
u32 SEC_FILE = 0x800000;
u32 SEC_IMAGE = 0x1000000;
u32 SEC_PROTECTED_IMAGE = 0x2000000;
u32 SEC_RESERVE = 0x4000000;
u32 SEC_COMMIT = 0x8000000;
u32 SEC_NOCACHE = 0x10000000;
u32 SEC_WRITECOMBINE = 0x40000000;
u32 SEC_LARGE_PAGES = 0x80000000;
u32 WRITE_WATCH_FLAG_RESET = 0x1;
u32 MEM_UNMAP_WITH_TRANSIENT_BOOST = 0x1;
u32 CREATE_ALWAYS = 0x2;
u32 CREATE_NEW = 0x1;
u32 OPEN_ALWAYS = 0x4;
u32 OPEN_EXISTING = 0x3;
u32 TRUNCATE_EXISTING = 0x5;
u32 ERROR_ALREADY_EXISTS = 0xb7;
u32 ERROR_PATH_NOT_FOUND = 0x3;
void* GetStdHandle(u32 std_handle);
bool SetStdHandle(u32 std_handle, void* handle);
bool WriteConsoleA(void* handle_console_output, void* buffer, u32 number_chars_to_write, u32* number_chars_written, void* reserved);
bool WriteConsoleW(void* handle_console_output, void* buffer, u32 number_chars_to_write, u32* number_chars_written, void* reserved);
bool ReadConsole(void* console_input, void* buffer, u32 number_chars_to_read, u32* number_chars_read, void* input_control);
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
__ret_SetUnhandledExceptionFilter* SetUnhandledExceptionFilter(s64(*top_level_exception_filter)(void*));

s32 foo(struct string str, struct string str2){
u32 written = 0;

void* stdhandle;
stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
WriteConsoleA(stdhandle,((void*)((str).data)),((u32)((str).length)),(&written),((void*)0x0));
WriteConsoleA(stdhandle,((void*)((str2).data)),((u32)((str2).length)),(&written),((void*)0x0));
return 0x17;
}

s32 arr(char a[12]){
s32 result;
result = 0x0;
s64 i;
i = 0x0;
while(((i)<0x3)){
result = ((result)+*(s32*)(((char*)a)+ 4 * (i)));
i = ((i)+0x1);
}
loop_0:;

return result;
}

s32 __main(){
s32 hello;
struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x5;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
struct string __temp_v_1;
{
char* __t_base = (char*)&(__temp_v_1);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x3;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_1;
}
hello = ((0x2)+foo(__temp_v_0,__temp_v_1));
s32 world;
char*  __temp_v_2;
{
char* __t_base = (char*)__temp_v_2;
char* __struct_base = __t_base;
*(s32*)__t_base = 0x1;
__t_base += 4;
*(s32*)__t_base = 0x2;
__t_base += 4;
*(s32*)__t_base = 0x3;
__t_base += 4;
}
world = ((0x4)*arr(__temp_v_2));
struct hey oi;
{
char* __t_base = (char*)&(oi);
char* __array_base = __t_base;
{
char* __t_base = __array_base;
*(s32*)__t_base = 0x1;
__t_base += 4;
*(s32*)__t_base = 0x2;
__t_base += 4;
*(s32*)__t_base = 0x3;
__t_base += 4;
}
}

return ((hello)+world);
}


void __entry() {
	ExitProcess(__main());
}