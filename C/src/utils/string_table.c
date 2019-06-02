#include "string_table.h"
#include "allocator.h"

u64 string_hash(string s) {
	return fnv_1_hash(s.data, s.length);
}

GENERATE_HASH_TABLE_IMPLEMENTATION(String, string, string, string_hash, light_alloc, light_free)

// Hash functions
u64
fnv_1_hash(const u8* s, u64 length) {
	u64 hash = 14695981039346656037ULL;
	u64 fnv_prime = 1099511628211ULL;
	for (u64 i = 0; i < length; ++i) {
		hash = hash * fnv_prime;
		hash = hash ^ s[i];
	}
	return hash;
}

u64 
fnv_1_hash_from_start(u64 hash, const u8* s, u64 length) {
	u64 fnv_prime = 1099511628211;
	for (u64 i = 0; i < length; ++i) {
		hash = hash * fnv_prime;
		hash = hash ^ s[i];
	}
	return hash;
}

u64
fnv_1_hash_combine(u64 hash1, u64 hash2) {
	u64 fnv_prime = 1099511628211;
	u64 hash = hash1;
	hash = hash * fnv_prime;
	hash = hash ^ hash2;
	return hash;
}