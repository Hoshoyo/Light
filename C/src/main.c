#define LIGHT_ARENA_IMPLEMENT
#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "utils/os.h"
#include "global_tables.h"
#include "top_typecheck.h"
#include <light_array.h>

int main(int argc, char** argv) {
    light_set_global_tables(argv[0]);

    if(argc < 2) {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        return 1;
    }

    Light_Lexer lexer = {0};
    //Light_Token* tokens = lexer_file(&lexer, argv[1], LIGHT_LEXER_PRINT_TOKENS);
    Light_Token* tokens = lexer_file(&lexer, argv[1], 0);

    u32 parser_error = 0;
    Light_Parser parser = {0};

    Light_Scope global_scope = {0};
    Light_Ast** ast = parse_top_level(&parser, &lexer, &global_scope, &parser_error);

    if(parser_error & PARSER_ERROR_FATAL)
        return 1;

    
    Light_Type_Check_Error type_error = top_typecheck(ast, &global_scope);
    if(type_error & TYPE_ERROR) {
        return 1;
    }
    ast_print(ast, LIGHT_AST_PRINT_STDOUT|LIGHT_AST_PRINT_EXPR_TYPES, 0);

    //type_table_print();

    return 0;
}