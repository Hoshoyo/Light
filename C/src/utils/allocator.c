#include "allocator.h"
#include <stdlib.h>

void* 
light_alloc(u64 size_bytes) {
    return calloc(1, size_bytes);
}

void 
light_free(void* block) {
    free(block);
}

void* 
light_realloc(void* block, u64 size_bytes) {
    return realloc(block, size_bytes);
}