#include "type_table.h"
#include "utils/allocator.h"
#include "type.h"

GENERATE_HASH_TABLE_IMPLEMENTATION(Type, type, Light_Type*, type_hash, light_alloc, light_free)