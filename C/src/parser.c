#include "parser.h"
#include "global_tables.h"
#include "utils/os.h"
#include <light_array.h>
#include <stdarg.h>

#define ReturnIfError() if(*error & PARSER_ERROR_FATAL) return 0
#define TOKEN_STR(T) (T)->length, (T)->data

// Forward declarations

static s32
parser_error_location(Light_Parser* parser, Light_Token* t) {
    if(!t) {
        return fprintf(stderr, "%s: ", parser->lexer->filepath);
    } else {
        return fprintf(stderr, "%s:%d:%d: ", parser->lexer->filepath, t->line, t->column);
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
parse_decl_variable(Light_Parser* parser, Light_Token* name, Light_Type* type, Light_Scope* scope, u32* error) {
    return 0;
}

static Light_Ast*
parse_decl_constant(Light_Parser* parser, Light_Token* name, Light_Type* type, Light_Scope* scope, u32* error) {
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
                if(*error & PARSER_ERROR_FATAL) break;
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
        *error |= parser_error_fatal(parser, name, "expected declaration name identifier, but got %s '%.*s'\n", 
            token_type_to_str(name->type), TOKEN_STR(name));
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
                    result = parse_decl_constant(parser, name, 0, scope, error);
                }
            } else {
                // type for a variable or constant declaration
                Light_Type* type = parse_type(parser, scope, error);
                if(*error & PARSER_ERROR_FATAL) break;
                Light_Token* n = lexer_peek(lexer);
                switch(n->type) {
                    case '=':
                        lexer_next(lexer);
                        result = parse_decl_variable(parser, name, type, scope, error);
                        break;
                    case ':':
                        lexer_next(lexer);
                        result = parse_decl_constant(parser, name, type, scope, error);
                        break;
                    default:{
                        *error |= parser_error_fatal(parser, next, "invalid token '%.*s' in declaration\n", TOKEN_STR(n));
                    } break;
                }
            }
        } break;
        case TOKEN_ARROW:{
            // typedef
            Light_Type* type = parse_type(parser, scope, error);
            result = ast_new_typedef(scope, type, name);
        } break;
        default: {
            *error |= parser_error_fatal(parser, next, "invalid token '%.*s' after declaration name\n", TOKEN_STR(next));
        }break;
    }

    return result;
}

static Light_Type*
parse_type_pointer(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, '^');
    ReturnIfError();

    return type_new_pointer(parse_type(parser, scope, error));
}
static Light_Type*
parse_type_array(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, '[');
    ReturnIfError();

    Light_Ast* dimension = parse_expression(parser, scope, error);
    ReturnIfError();

	*error |= parser_require_and_eat(parser, ']');
    ReturnIfError();

	Light_Type* array_type = parse_type(parser, scope, error);
    ReturnIfError();

	return type_new_array(dimension, array_type);
}
static Light_Type*
parse_type_procedure(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, '('); 
    ReturnIfError();

    bool all_args_internalized = true;
    Light_Type** arguments_types = 0;
    if(lexer_peek(parser->lexer)->type != ')') {
        arguments_types = array_new(Light_Type*);
        for(s32 i = 0; ; ++i) {
            if(i != 0) {
                *error |= parser_require_and_eat(parser, ',');
                ReturnIfError();
            }
            Light_Type* arg_type = parse_type(parser, scope, error);
            array_push(arguments_types, arg_type);
            if(!(arg_type->flags & TYPE_FLAG_INTERNALIZED)) all_args_internalized = false;

            if(lexer_peek(parser->lexer)->type != ',') break;
        }
    }

    *error |= parser_require_and_eat(parser, ')'); 
    ReturnIfError();

    Light_Type* return_type = 0;
    if(lexer_peek(parser->lexer)->type == TOKEN_ARROW) {
        *error |= parser_require_and_eat(parser, TOKEN_ARROW); 
        ReturnIfError();

        return_type = parse_type(parser, scope, error);
        ReturnIfError();
    } else {
        return_type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    }

    s32 args_count = (arguments_types) ? array_length(arguments_types) : 0;
    return type_new_function(arguments_types, return_type, args_count, all_args_internalized);
}
static Light_Type*
parse_type_alias(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Token* name = lexer_next(parser->lexer);
    if(name->type != TOKEN_IDENTIFIER) {
        *error |= parser_error_fatal(parser, name, "expected identifier but got '%.*s'\n", TOKEN_STR(name));
        return 0;
    }

    return type_new_alias(name, 0);
}

static Light_Type*
parse_type_enum(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_ENUM);
    ReturnIfError();

    Light_Type* type_hint = 0;
    if(lexer_peek(parser->lexer)->type != '{') {
        type_hint = parse_type(parser, scope, error);
        ReturnIfError();
    }

    *error |= parser_require_and_eat(parser, '{');
    ReturnIfError();

    Light_Token** fields_names = array_new(Light_Token*);
    Light_Ast**   fields_values = array_new(Light_Ast*);
    Light_Scope*  enum_scope = light_scope_new(0, scope, SCOPE_ENUM);
    for(s32 i = 0; ; ++i) {
        if(i != 0) {
            *error |= parser_require_and_eat(parser, ',');
            ReturnIfError();
        }
        Light_Token* name = lexer_next(parser->lexer);
        if(name->type != TOKEN_IDENTIFIER) {
            *error |= parser_error_fatal(parser, name, "expected identifier in enum field declaration, but got '%.*s'\n", TOKEN_STR(name));
            return 0;
        }

        array_push(fields_names, name);

        if(lexer_peek(parser->lexer)->type == ':') {
            lexer_next(parser->lexer); // skip ':'
            *error |= parser_require_and_eat(parser, ':');
            ReturnIfError();

            Light_Ast* val = parse_expression(parser, enum_scope, error);
            ReturnIfError();

            array_push(fields_values, val);
        } else {
            array_push(fields_values, 0);
        }

        if(lexer_peek(parser->lexer)->type != ',') break;
    }

    *error |= parser_require_and_eat(parser, '}');
    ReturnIfError();

    s32 fields_count = array_length(fields_names);
    return type_new_enum(fields_names, fields_values, fields_count, type_hint);
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
            lexer_next(parser->lexer);
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
            return parse_type_enum(parser, scope, error);
        case TOKEN_KEYWORD_STRUCT:
        case TOKEN_KEYWORD_UNION:
        default:
            // TODO(psv): error
            break;
    }

    return 0;
}