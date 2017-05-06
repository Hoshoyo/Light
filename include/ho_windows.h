#pragma comment(lib, "User32.lib")
#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <string.h>

#ifdef _DEBUG
#define assert(x) if(!(x)) { DebugBreak(); }
#else
#define assert(x)
#endif

#if defined(HO_DLL_EXPORT)
#define HO_API __stdcall __declspec(dllexport)
#elif defined(HO_DLL_IMPORT)
#define HO_API __stdcall __declspec(dllimport)
#else
#define HO_API __stdcall
#endif
extern "C" {
int HO_API ho_list_currdir(WIN32_FIND_DATA* data, int num_entries);
int HO_API ho_list_dir(WIN32_FIND_DATA* data, int num_entries, const char* path);
void HO_API ho_getfileinfo(const char* filename, WIN32_FIND_DATA* info);
int HO_API ho_mkdir(const char* path);
int HO_API ho_wmkdir(const wchar_t* dirname);
int HO_API ho_rmdir(const char* path);
int HO_API ho_wrmdir(const wchar_t* dirname);
int HO_API ho_chdir(const char* dirname);
int HO_API ho_wchdir(const wchar_t* dirname);

/* Allocations */
void* HO_API ho_bigalloc_rw(size_t size);
void* HO_API ho_bigalloc_r(size_t size);
void HO_API ho_bigfree(void* block, size_t size);


/* Files */
#define FILE_READ GENERIC_READ
#define FILE_WRITE GENERIC_WRITE
/*
	CREATE_ALWAYS = 2
	CREATE_NEW = 1
	OPEN_ALWAYS = 4
	OPEN_EXISTING = 3
	TRUNCATE_EXISTING = 5
*/
HANDLE HO_API ho_createfile(const char* filename, int access_flags, int action_flags);
HANDLE HO_API ho_openfile(const char* filename, int access_flags);
void HO_API ho_closefile(HANDLE file);
u64 HO_API ho_getfilesize(const char* filename);

/*
	If mem is 0 this functions allocates file_size bytes for the file
	the memory is returned on mem and must be freed by the caller
*/
#define HO_ALLOC ((void*)0)
void* HO_API ho_readentirefile(HANDLE file, size_t file_size, void* mem);
u32 HO_API ho_writefile(HANDLE file, u32 bytes_to_write, u8* bytes);
} // extern C

struct Timer
{
	double g_freq;
	Timer();
	double GetTime();
};

#if defined(HO_SYSTEM_IMPLEMENT)

Timer::Timer()
{
	LARGE_INTEGER li = {};
	QueryPerformanceFrequency(&li);
	g_freq = double(li.QuadPart);
}

double Timer::GetTime()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart) / this->g_freq;
}

// return the number of directories found
int HO_API ho_list_currdir(WIN32_FIND_DATA* data, int num_entries)
{
	HANDLE search_handle = FindFirstFileEx(".\\*", FindExInfoStandard, &data[0], FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
	if (search_handle == INVALID_HANDLE_VALUE)
		return 0;
	int i = 1;
	for (; i < num_entries && FindNextFile(search_handle, &data[i]); i++);

	FindClose(search_handle);
	return i;
}

int HO_API ho_list_dir(WIN32_FIND_DATA* data, int num_entries, const char* path)
{
	HANDLE search_handle = FindFirstFileEx(path, FindExInfoStandard, &data[0], FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
	if (search_handle == INVALID_HANDLE_VALUE)
		return 0;
	int i = 1;
	for (; i < num_entries && FindNextFile(search_handle, &data[i]); i++);

	FindClose(search_handle);
	return i;
}

void HO_API ho_getfileinfo(const char* filename, WIN32_FIND_DATA* info)
{
	HANDLE search_handle = FindFirstFileEx(filename, FindExInfoStandard, info, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
	if (search_handle == INVALID_HANDLE_VALUE)
		return;
	FindClose(search_handle);
}

int HO_API ho_mkdir(const char* path)
{
	return _mkdir(path);
}

int HO_API ho_wmkdir(const wchar_t* dirname)
{
	return _wmkdir(dirname);
}

int HO_API ho_rmdir(const char* path)
{
	return _rmdir(path);
}

int HO_API ho_wrmdir(const wchar_t* dirname)
{
	return _wrmdir(dirname);
}

int HO_API ho_chdir(const char* dirname)
{
	return _chdir(dirname);
}

int HO_API ho_wchdir(const wchar_t* dirname)
{
	return _wchdir(dirname);
}

/* Allocations */
void* HO_API ho_bigalloc_rw(size_t size)
{
	return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}
void* HO_API ho_bigalloc_r(size_t size)
{
	return VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READONLY);
}

void HO_API ho_bigfree(void* block, size_t size)
{
	VirtualFree(block, size, MEM_RELEASE);
}

/* Files */
#define FILE_READ GENERIC_READ
#define FILE_WRITE GENERIC_WRITE

/*
	CREATE_ALWAYS = 2
	CREATE_NEW = 1
	OPEN_ALWAYS = 4
	OPEN_EXISTING = 3
	TRUNCATE_EXISTING = 5
*/
HANDLE HO_API ho_createfile(const char* filename, int access_flags, int action_flags)
{
	HANDLE handle = CreateFile(filename, access_flags, 0, 0, action_flags, FILE_ATTRIBUTE_NORMAL, 0);
	return handle;
}

HANDLE HO_API ho_openfile(const char* filename, int access_flags)
{
	HANDLE handle = CreateFile(filename, access_flags, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	return handle;
}

void HO_API ho_closefile(HANDLE file)
{
	CloseHandle(file);
}

u64 HO_API ho_getfilesize(const char* filename)
{
	WIN32_FIND_DATA info;
	ho_getfileinfo(filename, &info);
	u64 size = 0;
	size = (u64)info.nFileSizeLow | ((u64)info.nFileSizeHigh << 32);
	return size;
}

/*
	If mem is 0 this functions allocates file_size bytes for the file
	the memory is returned on mem and must be freed by the caller
*/
#define HO_ALLOC ((void*)0)
void* HO_API ho_readentirefile(HANDLE file, size_t file_size, void* mem)
{
	u64 maxs32 = 0x7fffffff;
	DWORD bytes_read = 0;
	void* memory = mem;
	if (file_size == 0)
		return 0;
	if (!mem)
		memory = ho_bigalloc_rw(file_size);
	if (INVALID_HANDLE_VALUE == file)
		return 0;

	if (file_size > maxs32)
	{
		void* mem_aux = memory;
		s64 num_reads = file_size / maxs32;
		s64 rest = file_size % maxs32;
		DWORD bytes_read;
		for (s64 i = 0; i < num_reads; ++i)
		{
			ReadFile(file, mem_aux, (DWORD)maxs32, &bytes_read, 0);
			mem_aux = (char*)mem_aux + maxs32;
		}
		ReadFile(file, mem_aux, (DWORD)rest, &bytes_read, 0);
	}
	else
	{
		ReadFile(file, memory, (DWORD)file_size, &bytes_read, 0);
	}

	return memory;
}

u32 HO_API ho_writefile(HANDLE file, u32 bytes_to_write, u8* bytes)
{
	DWORD written = 0;
	WriteFile(file, bytes, bytes_to_write, &written, 0);
	return written;
}

#endif // HO_SYSTEM_IMPLEMENT