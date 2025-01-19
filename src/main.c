#define LIGHT_ARENA_IMPLEMENT
#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "utils/os.h"
#include "utils/utils.h"
#include "global_tables.h"
#include "top_typecheck.h"
#include "bytecode.h"
#include "backend/c/toplevel.h"
#include <light_array.h>
#include "ir.h"
#define HOHT_IMPLEMENTATION
#include <hoht.h>
#include "backend/backend.h"

Light_Ast** load_internal_modules(Light_Parser* parser, Light_Scope* global_scope, Light_Backend backend);

Compilation_Settings g_compilation_settings = {.bitsize = 32};

int main(int argc, char** argv) {
    double start = os_time_us();

    light_set_global_tables(argv[0]);

    if(argc < 2) {
        fprintf(stderr, "usage: %s filename <backend>\n\n", argv[0]);
        fprintf(stderr, "choosing a backend\n");
        fprintf(stderr, "  - c        Generates a C file and compiles using the current platform C compiler\n");
        fprintf(stderr, "  - lightvm  Compiles into the internal VM bytecode and runs it immediately\n");
        return 1;
    }
    s32 backend = BACKEND_C;
    g_compilation_settings.bitsize = 32;
    if(argc >= 3) {
        char* backend_str = argv[2];
        if(strcmp(backend_str, "-x86coff") == 0) {
            backend = BACKEND_X86_PECOFF;
            g_compilation_settings.bitsize = 32;
        } else if(strcmp(backend_str, "-x86elf") == 0) {
            backend = BACKEND_X86_ELF;
            g_compilation_settings.bitsize = 32;
        } else if(strcmp(backend_str, "-x86rawx") == 0) {
            backend = BACKEND_X86_RAWX;
            g_compilation_settings.bitsize = 32;
        } else if(strcmp(backend_str, "-c") == 0) {
            backend = BACKEND_C;
            g_compilation_settings.bitsize = 64;
        } else if(strcmp(backend_str, "-lightvm") == 0) {
            backend = BACKEND_LIGHT_VM;
            g_compilation_settings.bitsize = 64;
        } else {
            fprintf(stderr, "invalid backend '%s'\n", argv[2]);
            return 1;
        }
    }
    char* output_filename = 0;
    if(argc >= 5) {
        if(strcmp(argv[3], "-o") == 0){
            output_filename = argv[4];
        }
    }
    bool quiet = false;
    if(argc >= 6) {
        if(strcmp(argv[5], "-q") == 0) {
            quiet = true;
        }
    }

    size_t compiler_path_size = 0;
    const char* compiler_path = light_path_from_filename(argv[0], &compiler_path_size);

    size_t real_path_size = 0;
    const char* main_file_directory = light_path_from_filename(argv[1], &real_path_size);

    Light_Lexer  lexer = {0};
    Light_Parser parser = {0};
    Light_Scope  global_scope = {0};

    initialize_global_identifiers_table();

    u32 parser_error = 0;
    parse_init(&parser, &lexer, &global_scope, compiler_path, compiler_path_size, argv[1]);

    Light_Ast** ast = load_internal_modules(&parser, &global_scope, backend);

    double parse_start = os_time_us();
    double lexing_elapsed = 0.0;
    // Parse all other files included in the main file
    while(array_length(parser.parse_queue_array) > 0) {
        Light_Lexer lexer = {0};
        double lexer_start = os_time_us();
        Light_Token* tokens = lexer_file(&lexer, parser.parse_queue_array[0].data, 0);
        lexing_elapsed += (os_time_us() - lexer_start) / 1000.0;

        if(tokens == 0) {
            // File does not exist
            return 1;
        }

        ast = parse_top_level(&parser, &lexer, &global_scope, &parser_error);

        if(parser_error & PARSER_ERROR_FATAL)
            return 1;

        array_remove(parser.parse_queue_array, 0);
    }
    double parse_elapsed = (os_time_us() - parse_start) / 1000.0;
    
    // Type checking
    double tcheck_start = os_time_us();
    Light_Type_Error type_error = top_typecheck(ast, &global_scope);
    if(type_error & TYPE_ERROR) {
        return 1;
    }
    double tcheck_elapsed = (os_time_us() - tcheck_start) / 1000.0;
    
#if 0
    ast_print(ast, LIGHT_AST_PRINT_STDOUT, 0);
    //ast_print(ast, LIGHT_AST_PRINT_STDOUT, 0);
    //type_table_print();
#endif

    if(!output_filename)
        output_filename = (char*)light_extensionless_filename(light_filename_from_path(argv[1]));

    double backend_gen_time = 0.0;
    backend_generate(backend, ast, &global_scope, main_file_directory, compiler_path, output_filename, &backend_gen_time);

    double total_elapsed = (os_time_us() - start) / 1000.0;
    if(!quiet) {
        printf("\n- elapsed time: (backend: %s)\n\n", backend_to_string(backend));
        printf("  lexing:          %.2f ms\n", lexing_elapsed);
        printf("  parse:           %.2f ms\n", parse_elapsed);
        printf("  type check:      %.2f ms\n", tcheck_elapsed);
        printf("  backend gen:     %.2f ms\n", backend_gen_time);
        printf("  total:           %.2f ms\n", total_elapsed);
        printf("\n");
    }

    return 0;
}