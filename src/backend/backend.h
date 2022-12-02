#pragma once
#include "../ast.h"

typedef enum {
    BACKEND_X86_PECOFF = 1,
    BACKEND_X64_PECOFF = 2,
    BACKEND_X86_ELF    = 3,
    BACKEND_X86_RAWX   = 4,
    BACKEND_C          = 5,
    BACKEND_LIGHT_VM   = 6,
} Light_Backend;

int         backend_generate(Light_Backend backend, Light_Ast** ast, Light_Scope* global_scope, const char* main_file_directory, const char* compiler_path, const char* output_filename, double* gen_time_us);
const char* backend_to_string(Light_Backend backend);

int lvm_generate(Light_Ast** ast, Light_Scope* global_scope);

Light_Ast* lvm_generate_and_run_directive(Light_Ast* expr, bool generate);