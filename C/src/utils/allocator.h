#pragma once
#include <common.h>

void* light_alloc(u64 size_bytes);
void  light_free(void* block);
void* light_realloc(void* block, u64 size_bytes);