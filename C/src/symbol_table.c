#include "symbol_table.h"
#include "utils/allocator.h"

u64 symbol_hash(Light_Symbol s) {
    return fnv_1_hash(s.token->data, s.token->length);
}

int symbol_equal(Light_Symbol s1, Light_Symbol s2) {
    return s1.token->data == s2.token->data;
}

GENERATE_HASH_TABLE_IMPLEMENTATION(Symbol, symbol, Light_Symbol, symbol_hash, light_alloc, light_free, symbol_equal)