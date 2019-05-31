#pragma once
#include <stdint.h>

const char* light_real_path(const char* path, uint64_t* size);
const char* light_path_from_filename(const char* filename, uint64_t* size);
const char* light_real_path_from(const char* path_from, uint64_t from_size, const char* filename, uint64_t* out_size);
const char* light_filename_from_path(const char* path);