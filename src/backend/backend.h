#pragma once
#include "../ast.h"

typedef enum {
    BACKEND_X86_PECOFF = 1,
    BACKEND_X86_ELF    = 2,
    BACKEND_X86_RAWX   = 3,
    BACKEND_C          = 4,
    BACKEND_LIGHT_VM   = 5,
} Light_Backend;

int         backend_generate(Light_Backend backend, Light_Ast** ast, Light_Scope* global_scope, const char* main_file_directory, const char* compiler_path, const char* output_filename, double* gen_time_us);
const char* backend_to_string(Light_Backend backend);

int lvm_generate(Light_Ast** ast, Light_Scope* global_scope);