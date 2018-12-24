#include <stdio.h>
#include <string.h>

#ifdef _DEBUG
//#define assert(x) if(!(x)) { __debugbreak(); }
#define assert(X) do{ if(!(X)) { fprintf(stderr, "assertion failed %s:%d\n", __FILE__, __LINE__); exit(-1); }} while(0)
#else
//#define assert(X)
#define assert(X) do{ if(!(X)) { fprintf(stderr, "assertion failed %s:%d\n", __FILE__, __LINE__); exit(-1); }} while(0)
#define assert_msg(X, MSG) do{ if(!(X)) { fprintf(stderr, "assertion failed %s:%d '%s'\n", __FILE__, __LINE__, MSG); exit(-1); }} while(0)
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>

typedef int HANDLE;
#define INVALID_HANDLE_VALUE -1
#define OPEN_EXISTING O_RDWR
#define CREATE_ALWAYS O_CREAT
/* Files */
#define FILE_READ O_RDONLY
#define FILE_WRITE O_WRONLY
#define FILE_READ_WRITE O_RDWR
#define FILE_APPEND O_APPEND
#define PATH_MAX 4096

/* Allocations */
extern "C" void* ho_bigalloc_rw(size_t size);
extern "C" void* ho_bigalloc_r(size_t size);
extern "C" void ho_bigfree(void* block, size_t size);

extern "C" int HO_API ho_createfile(const char* filename, int access_flags, int action_flags);
extern "C" int HO_API ho_openfile(const char* filename, int access_flags);
extern "C" void HO_API ho_closefile(int file);
extern "C" s64 HO_API ho_getfilesize(const char* filename);
extern "C" bool HO_API ho_file_exists(const char* filename);
extern "C" const char* HO_API ho_current_exe_path();
extern "C" const char* HO_API ho_current_exe_full_path();

/*
	If mem is 0 this functions allocates file_size bytes for the file
	the memory is returned on mem and must be freed by the caller
*/
#define HO_ALLOC ((void*)0)
extern "C" void* HO_API ho_readentirefile(int file, size_t file_size, void* mem);
extern "C" s64 HO_API ho_writefile(int file, s64 bytes_to_write, u8* bytes);
extern "C" char* HO_API ho_realpath(const char* path, size_t* size);

struct Timer
{
	Timer() {}
	double GetTime();
};

#if defined(HO_SYSTEM_IMPLEMENT)
#include <time.h>
double Timer::GetTime(){
	clockid_t clockid;
	struct timespec t_spec;
	int start = clock_gettime(CLOCK_MONOTONIC_RAW, &t_spec);
	u64 res = t_spec.tv_nsec + 1000000000 * t_spec.tv_sec;
	return (double)res / 1000000.0;
}

/*
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
}*/

bool HO_API ho_file_exists(const char* filepath) {
	int r = access(filepath, F_OK);
	return r == 0;
}

const char* HO_API ho_current_exe_path() {
	char result[PATH_MAX] = {0};
  ssize_t count;
#ifdef __linux__
	count = readlink("/proc/self/exe", result, PATH_MAX);
#else
  uint32_t size = sizeof(result);
  if (_NSGetExecutablePath(result, &size) != 0)
    printf("buffer too small; need size %u\n", size);
  count = strlen(result);
#endif

	const char *path;
	if (count != -1) {
		path = dirname(result);
		return path;
	}

	return 0;
}

const char* HO_API ho_current_exe_full_path() {
	char result[PATH_MAX] = {0};
  ssize_t count;
#ifdef __linux__
	count = readlink("/proc/self/exe", result, PATH_MAX);
#else
  uint32_t size = sizeof(result);
  if (_NSGetExecutablePath(result, &size) != 0)
    printf("buffer too small; need size %u\n", size);
  count = strlen(result);
#endif

	char* path = (char*)malloc(count + 1);
	path[count] = 0;
	memcpy(path, result, count);

	return path;
}

/* Allocations */
void* HO_API ho_bigalloc_rw(size_t size)
{
	return mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
}
void* HO_API ho_bigalloc_r(size_t size)
{
	return mmap(0, size, PROT_READ, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
}

void HO_API ho_bigfree(void* block, size_t size)
{
	munmap(block, size);
}

char* HO_API ho_realpath(const char* path, size_t* size)
{
	// this function uses malloc, should free after
	char* result = realpath(path, 0);
	if(result)
		*size = strlen(result);
	else
		*size = 0;
	return result;
}

int HO_API ho_createfile(const char* filename, int access_flags, int action_flags)
{
	int err =  open(filename, access_flags | action_flags | O_TRUNC, 0644);
	if (err == -1) {
		printf("ERROR CODE: %d %d\n",EACCES, errno);
	}
	return err;
}

int HO_API ho_openfile(const char* filename, int access_flags)
{
	return open(filename, access_flags);
}

void HO_API ho_closefile(int fd)
{
	close(fd);
}

s64 HO_API ho_getfilesize(const char* filename)
{
	int filedescriptor = open(filename, O_RDONLY);
	struct stat buf;
	fstat(filedescriptor, &buf);
	size_t size = buf.st_size;
	close(filedescriptor);
	return size;
}

/*
	If mem is 0 this functions allocates file_size bytes for the file
	the memory is returned on mem and must be freed by the caller
*/
#define HO_ALLOC ((void*)0)
void* HO_API ho_readentirefile(int file, size_t file_size, void* mem)
{
	void* memory = mem;
	if(file_size == 0) return 0;
	if(mem == 0){
		memory = ho_bigalloc_rw(file_size);
	}
	int error = read(file, memory, file_size);
	if(error == -1){
		return 0;
	}
	return memory;
}

s64 ho_writefile(int fd, s64 bytes_to_write, u8* bytes){
	s64 written = write(fd, bytes, bytes_to_write);
	return written;
}

#endif // HO_SYSTEM_IMPLEMENT
