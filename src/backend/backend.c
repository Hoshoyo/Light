#include "backend.h"
#include "../ir.h"
#include "../global_tables.h"
#include "../utils/os.h"
#include "x86/x86.h"
#include "c/toplevel.h"

const char*
backend_to_string(Light_Backend backend)
{
    switch(backend) {
        case BACKEND_X86_ELF:       return "x86 elf";
        case BACKEND_X86_PECOFF:    return "x86 pecoff";
        case BACKEND_X86_RAWX:      return "x86 rawx";
        case BACKEND_C:             return "C";
        case BACKEND_LIGHT_VM:      return "Light VM";
        default: return "invalid backend";
    }
}

int
backend_generate(Light_Backend backend, Light_Ast** ast, Light_Scope* global_scope, 
    const char* main_file_directory, const char* compiler_path,
    const char* output_filename, double* gen_time_us)
{
    switch(backend) {
        case BACKEND_X86_ELF:
        case BACKEND_X86_RAWX:
        case BACKEND_X86_PECOFF: {
            double generate_start = os_time_us();
            IR_Generator irgen = {0};
            ir_generate(&irgen, ast);
            X86_generate(&irgen, output_filename, backend);
            double generate_elapsed = (os_time_us() - generate_start) / 1000.0;
            if(gen_time_us) *gen_time_us = generate_elapsed;
        } break;
        case BACKEND_C: {
            double generate_start = os_time_us();
            backend_c_generate_top_level(ast, global_type_table, global_scope, main_file_directory, output_filename, compiler_path);
            double generate_elapsed = (os_time_us() - generate_start) / 1000.0;

            double gcc_start = os_time_us();
            backend_c_compile_with_cl(ast, output_filename, main_file_directory);
            double gcc_elapsed = (os_time_us() - gcc_start) / 1000.0;
            if(gen_time_us) *gen_time_us = gcc_elapsed + generate_elapsed;
        } break;
        case BACKEND_LIGHT_VM: {
            double lvm_start = os_time_us();
            lvm_generate(ast, global_scope);
            double generate_elapsed = (os_time_us() - lvm_start) / 1000.0;
            if(gen_time_us) *gen_time_us = generate_elapsed;
        } break;
        default: return 1;
    }
    return 0;
}