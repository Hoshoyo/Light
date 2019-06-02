#include "symbol_table.h"
#include "utils/allocator.h"

u64 symbol_hash(Light_Symbol s) {
    return fnv_1_hash(s.token->data, s.token->length);
}

GENERATE_HASH_TABLE_IMPLEMENTATION(Symbol, symbol, Light_Symbol, symbol_hash, light_alloc, light_free)