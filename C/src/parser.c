#include "parser.h"
#include "global_tables.h"
#include "utils/os.h"
#include <light_array.h>
#include <stdarg.h>

// Forward declarations

static s32
parser_error_location(Light_Parser* parser, Light_Token* t) {
    if(!t) {
        return fprintf(stderr, "%s: ", parser->lexer->filename);
    } else {
        return fprintf(stderr, "%s:%d:%d: ", parser->lexer->filename, t->line, t->column);
    }
}

static u32
parser_error_fatal(Light_Parser* parser, Light_Token* loc, const char* fmt, ...) {
    va_list args;
	va_start(args, fmt);
    parser_error_location(parser, loc);
	fprintf(stderr, "Syntax error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);

    return PARSER_ERROR_FATAL;
}

static u32
parser_require_and_eat(Light_Parser* parser, Light_Token_Type type) {
    Light_Token* t = lexer_next(parser->lexer);
    if(t->type != type) {
        return parser_error_fatal(parser, t, "expected '%s' but got '%s'\n", token_type_to_str(type), token_type_to_str(t->type));
    }
    return PARSER_OK;
}

static Light_Ast* 
parse_decl_procedure(Light_Parser* parser, Light_Token* name, Light_Scope* scope, u32* error) {
    return 0;
}

static Light_Ast*
parse_decl_variable(Light_Parser* parser, Light_Token* name, Light_Scope* scope, u32* error) {
    return 0;
}

static Light_Ast*
parse_decl_constant(Light_Parser* parser, Light_Token* name, Light_Scope* scope, u32* error) {
    return 0;
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
                Light_Ast* decl = parse_declaration(parser, global_scope, error);
                if(decl->kind == AST_DECL_CONSTANT || decl->kind == AST_DECL_VARIABLE){
                    *error |= parser_require_and_eat(parser, ';');
                }
                if(decl) {
                    array_push(parser->top_level, decl);
                }
            }break;
            default:{
                *error |= parser_error_fatal(parser, t, "expected declaration, but got '%s'\n", token_type_to_str(t->type));
            }break;
        }
    }

    return parser->top_level;
}

Light_Ast*
parse_declaration(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Ast* result = 0;

    Light_Lexer* lexer = parser->lexer;
    Light_Token* name = lexer_next(lexer);

    if(name->type != TOKEN_IDENTIFIER) {
        *error |= parser_error_fatal(parser, name, "expected declaration name identifier, but got '%s'\n", token_type_to_str(name->type));
        return 0;
    }

    Light_Token* next = lexer_next(lexer);

    switch(next->type) {
        case ':':{
            if(lexer_peek(lexer)->type == ':') {
                // constant declaration
                if(lexer_peek(lexer)->type == '(') {
                    // procedure declaration
                    result = parse_decl_procedure(parser, name, scope, error);
                } else {
                    // constant declaration
                    result = parse_decl_constant(parser, name, scope, error);
                }
            } else {
                // type for a variable or constant declaration
                Light_Type* type = parse_type(parser, scope, error);
                if(*error & PARSER_ERROR_FATAL) break;
                Light_Token* n = lexer_peek(lexer);
                switch(n->type) {
                    case '=':
                        result = parse_decl_variable(parser, name, scope, error);
                        break;
                    case ':':
                        result = parse_decl_constant(parser, name, scope, error);
                        break;
                    default:
                        *error |= parser_error_fatal(parser, n, "unexpected '%s' on declaration\n", token_type_to_str(n->type));
                        break;
                }
            }
        } break;
        case TOKEN_ARROW:{
            // typedef
            Light_Type* type = parse_type(parser, scope, error);
            result = ast_new_typedef(scope, type, name);
        } break;
        default: {
            *error |= parser_error_fatal(parser, next, "invalid token '%s' after declaration name\n", token_type_to_str(next->type));
        }break;
    }

    return result;
}

Light_Type*
parse_type(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Token* t = lexer_peek(parser->lexer);

    switch(t->type) {
        case TOKEN_SINT64:
        case TOKEN_SINT32:
        case TOKEN_SINT16:
        case TOKEN_SINT8:
        case TOKEN_UINT64:
        case TOKEN_UINT32:
        case TOKEN_UINT16:
        case TOKEN_UINT8:
        case TOKEN_REAL32:
        case TOKEN_REAL64:
        case TOKEN_BOOL:
        case TOKEN_VOID:
            return type_primitive_from_token(t->type);
        case '^':
            return parse_type_pointer(parser, scope, error);
        case '[':
            return parse_type_array(parser, scope, error);
        case '(':
            return parse_type_procedure(parser, scope, error);
        case TOKEN_IDENTIFIER:
            return parse_type_alias(parser, scope, error);
        case TOKEN_KEYWORD_ENUM:
        case TOKEN_KEYWORD_STRUCT:
        case TOKEN_KEYWORD_UNION:
        default:
            // TODO(psv): error
            break;
    }

    return 0;
}