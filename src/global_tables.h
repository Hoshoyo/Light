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
extern Type_Table   global_type_table;
extern Light_Type** global_type_array;

// Ast
extern Light_Ast**  global_infer_queue;

void light_set_global_tables(const char* compiler_path);

typedef struct {
    uint32_t bitsize;
} Compilation_Settings;

extern Compilation_Settings g_compilation_settings;