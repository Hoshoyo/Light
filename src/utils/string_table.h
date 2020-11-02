#pragma once
#include <common.h>
#include "hash.h"

typedef struct {
    s32   length;
    u32   value;
    char* data;
} string;

#define MAKE_STR(S, V) (string){ sizeof(S) - 1, V, (char*)(S) }
#define MAKE_STR_LEN(S, L) (string){ L, 0, (char*)(S) }

GENERATE_HASH_TABLE(String, string, string)

// Hash functions
u64 fnv_1_hash(const u8* s, u64 length);
u64 fnv_1_hash_from_start(u64 hash, const u8* s, u64 length);
u64 fnv_1_hash_combine(u64 hash1, u64 hash2);