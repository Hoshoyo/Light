#include "os.h"
#include "allocator.h"
#include <stdlib.h>
#include <string.h>

#if defined(__linux__)
#include <time.h>
const char* 
light_real_path(const char* path, uint64_t* size) {
	// this function uses malloc, should free after
	char* result = realpath(path, 0);
	if(result){
		if(size) *size = strlen(result);
	} else {
		if(size) *size = 0;
	}
	return result;
}

double os_time_us()
{
	clockid_t clockid;
	struct timespec t_spec;
	int start = clock_gettime(CLOCK_MONOTONIC_RAW, &t_spec);
	u64 res = t_spec.tv_nsec + 1000000000 * t_spec.tv_sec;
	return (double)res / 1000.0;
}

#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
static double perf_frequency;
static void os_set_query_frequency() {
	LARGE_INTEGER li = { 0 };
	QueryPerformanceFrequency(&li);
	perf_frequency = (double)(li.QuadPart);
}
double os_time_us() {
	static initialized = false;
	if (!initialized) {
		os_set_query_frequency();
		initialized = true;
	}

	LARGE_INTEGER li = { 0 };
	QueryPerformanceCounter(&li);
	return ((double)(li.QuadPart) / perf_frequency) * 1000000.0;
}

const char*
light_real_path(const char* path, uint64_t* size) {
	char* buffer = (char*)calloc(1, 4096);
	DWORD err = GetFullPathNameA(path, 4096, buffer, 0);
	if (err > 4096) {
		buffer = (char*)realloc(buffer, err);
		err = GetFullPathNameA(path, err, buffer, 0);
	} else if (err == 0) {
		return 0;
	}
	*size = err;
	return buffer;
}
#endif

const char*
light_real_path_from(const char* path_from, uint64_t from_size, const char* filename, uint64_t* out_size) {
	if (from_size == 0) {
		from_size = strlen(path_from);
	}
	size_t filename_length = strlen(filename);

	void* mem = light_alloc(from_size + filename_length + 1);
	memcpy(mem, path_from, from_size);
	memcpy((char*)mem + from_size, filename, filename_length);
	((char*)mem)[from_size + filename_length] = 0;

	const char* result = light_real_path(mem, out_size);
	light_free(mem);

	return result;
}

const char*
light_path_from_filename(const char* filename, uint64_t* size) {
	size_t fullpath_size;
	char* fullpath = (char*)light_real_path(filename, &fullpath_size);
	if (!fullpath) return 0;

	size_t pathsize = fullpath_size - 1;
	for (; pathsize >= 0; --pathsize) {
		if (fullpath[pathsize] == '/') {
			break;
		}
	}
	fullpath[pathsize + 1] = 0;
	if (size) *size = pathsize;
	return fullpath;
}

const char*
light_filename_from_path(const char* path) {
	size_t len = strlen(path);
	size_t i = len - 1;
	for (; i >= 0; --i) {
		if (path[i] == '/') {
			i++;
			break;
		}
	}
	return path + i;
}