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

int main(int argc, char** argv) {
    double start = os_time_us();

    light_set_global_tables(argv[0]);

    if(argc < 2) {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        return 1;
    }

    size_t real_path_size = 0;
    //const char* main_file_path = light_real_path(argv[1], &real_path_size);
    const char* main_file_directory = light_path_from_filename(argv[1], &real_path_size);

    Light_Lexer  lexer = {0};
    Light_Parser parser = {0};
    Light_Scope  global_scope = {0};

    u32 parser_error = 0;
    parse_init(&parser, &lexer, &global_scope, argv[1]);

    Light_Ast** ast = 0;

    double parse_start = os_time_us();
    // Parse all other files included in the main file
    while(array_length(parser.parse_queue_array) > 0) {
        Light_Lexer lexer = {0};
        Light_Token* tokens = lexer_file(&lexer, parser.parse_queue_array[0].data, 0);

        ast = parse_top_level(&parser, &lexer, &global_scope, &parser_error);

        if(parser_error & PARSER_ERROR_FATAL)
            return 1;

        array_remove(parser.parse_queue_array, 0);
    }
    printf("Parse time: %fms\n", (os_time_us() - parse_start) / 1000.0);
    
    // Type checking
    double tcheck_start = os_time_us();
    Light_Type_Error type_error = top_typecheck(ast, &global_scope);
    if(type_error & TYPE_ERROR) {
        return 1;
    }
    printf("Typecheck time: %fms\n", (os_time_us() - tcheck_start) / 1000.0);
    
#if 0
    ast_print(ast, LIGHT_AST_PRINT_STDOUT|LIGHT_AST_PRINT_EXPR_TYPES, 0);
    type_table_print();
#endif

    double end = os_time_us();
    printf("Time elapsed: %fms\n", (end - start) / 1000.0);

#if 1
    const char* outfile = light_extensionless_filename(light_filename_from_path(argv[1]));

    double generate_start = os_time_us();
    backend_c_generate_top_level(ast, global_type_table, main_file_directory, outfile);
    printf("Generate time: %fms\n", (os_time_us() - generate_start) / 1000.0);

    double gcc_start = os_time_us();

    // Remove file extension
    backend_c_compile_with_gcc(ast, outfile, main_file_directory);
    printf("Backend time: %fms\n", (os_time_us() - gcc_start) / 1000.0);
#endif

#if 0
    Bytecode_State state = bytecode_gen_ast(ast);

    light_vm_debug_dump_code(stdout, state.vmstate);

    light_vm_execute(state.vmstate, 0, 0);
    light_vm_debug_dump_registers(stdout, state.vmstate, LVM_PRINT_FLOATING_POINT_REGISTERS|LVM_PRINT_DECIMAL);
#endif

    return 0;
}