#pragma once
#include "utils/string_table.h"
#include "type_table.h"
#include <light_arena.h>

// Tables
extern String_Table global_identifiers_table;
extern String_Table global_imports_table;
extern string*      global_imports_queue;

// Paths
extern string global_compiler_path;

// Memory
extern Light_Arena* global_type_arena;

// Types
extern Type_Table global_type_table;