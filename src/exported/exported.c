#include "exported.h"
#include "common.h"
#include "../parser.h"
#include "../top_typecheck.h"
#include <stdlib.h>

void light_set_global_tables(const char* compiler_path);
Light_Ast** load_internal_modules(Light_Parser* parser, Light_Scope* global_scope);

Light_Ast**
light_lex_parse_string(const char* str, int length_bytes)
{
    Light_Parser* parser = calloc(1, sizeof(Light_Parser));
    Light_Scope* global_scope = calloc(1, sizeof(Light_Scope));

    light_set_global_tables("light_internal");
    initialize_global_identifiers_table();

    Light_Lexer lexer = {0};
    parse_init(parser, &lexer, global_scope, "light_internal", sizeof("light_internal") - 1, "light_internal_main");

    Light_Ast** ast = load_internal_modules(parser, global_scope);

    Light_Lexer user_lexer = {0};
    Light_Token* user_tokens = lexer_cstr(&user_lexer, (char*)str, length_bytes, 0);

    u32 parser_error = 0;
    ast = parse_top_level(parser, &user_lexer, global_scope, &parser_error);
    if(parser_error & PARSER_ERROR_FATAL)
        return 0;

    return ast;
}

Light_Ast* light_lex

Light_Ast**
light_typeinfer_check(Light_Ast** ast)
{
    Light_Type_Error type_error = top_typecheck(ast, ast[0]->scope_at);
    if(type_error & TYPE_ERROR) {
        return 0;
    }
    return ast;
}