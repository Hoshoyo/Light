#pragma once
#include "ast.h"

GENERATE_HASH_TABLE(Type, type, Light_Type*)
//GENERATE_HASH_TABLE_IMPLEMENTATION(Type, type, Light_Type*, type_hash, light_alloc, light_free)

GENERATE_HASH_TABLE(String, string, string)
//GENERATE_HASH_TABLE_IMPLEMENTATION(String, string, string, string_hash, light_alloc, light_free)
