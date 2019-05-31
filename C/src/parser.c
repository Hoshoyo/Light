#include "parser.h"
#include "global_tables.h"
#include "utils/os.h"
#include <light_array.h>

// Forward declarations
Light_Ast* parse_decl_procedure(Light_Parser* parser, Light_Token* name, Light_Scope* scope);

static s32
parser_error_location(Light_Parser* parser, Light_Token* t) {
    return fprintf(stderr, "%s:%d:%d: ", parser->lexer->filename, t->line, t->column);
}

static u32
parser_require_and_eat(Light_Parser* parser, Light_Token_Type type) {
    Light_Token* t = lexer_next(parser->lexer);
    if(t->type != type) {
        parser_error_location(parser, t);
        fprintf(stderr, "Syntax error: expected '%s' but got '%s'\n", token_type_to_str(type), token_type_to_str(t->type));
        return PARSER_ERROR_FATAL;
    }
    return PARSER_OK;
}

Light_Ast** 
parse_top_level(Light_Parser* parser, Light_Lexer* lexer, Light_Scope* global_scope, u32* error) {
    // Check empty file
    if(lexer_peek(lexer)->type == TOKEN_END_OF_STREAM){
        return 0;
    }

    parser->scope_global = global_scope;
    parser->lexer = lexer;
    parser->top_level = array_new_len(Light_Ast*, 1024);

    bool parsing = true;
    while(!(*error & PARSER_ERROR_FATAL) && parsing) {
        Light_Token* t = lexer_peek(lexer);

        switch(t->type) {
            case TOKEN_END_OF_STREAM: parsing = false; break;
            case '#':{
                // TODO(psv):
                // parse_directive
            }break;
            case TOKEN_IDENTIFIER:{
                Light_Ast* decl = parse_declaration(parser, global_scope);
                if(decl->kind == AST_DECL_CONSTANT || decl->kind == AST_DECL_VARIABLE){
                    *error |= parser_require_and_eat(parser, ';');
                }
                if(decl) {
                    array_push(parser->top_level, decl);
                }
            }break;
            default:{
                parser_error_location(parser, t);
                fprintf(stderr, "Syntax error: expected declaration, but got '%s'\n", token_type_to_str(t->type));
                *error |= PARSER_ERROR_FATAL;
            }break;
        }
    }

    return parser->top_level;
}

Light_Ast*
parse_declaration(Light_Parser* parser, Light_Scope* scope) {
    
}