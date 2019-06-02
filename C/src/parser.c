#include "parser.h"
#include "global_tables.h"
#include "utils/os.h"
#include <light_array.h>
#include <stdarg.h>
#include <assert.h>

#define ReturnIfError() if(*error & PARSER_ERROR_FATAL) return 0
#define TOKEN_STR(T) (T)->length, (T)->data

// Forward declarations
static Light_Ast* parse_decl_variable(Light_Parser* parser, Light_Token* name, Light_Type* type, Light_Scope* scope, u32* error);

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
parse_comm_block(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;

    *error |= parser_require_and_eat(parser, '{');
    ReturnIfError();
	
	if (lexer_peek(lexer)->type == '}') {
		lexer_next(lexer); // eat '{'
		return ast_new_comm_block(scope, 0, 0, 0);
	}

	Light_Ast** commands = array_new(Light_Ast*);
	Light_Scope* block_scope = light_scope_new(0, scope, SCOPE_BLOCK);

	s32 command_count = 0;
	for (;;) {
		Light_Ast* command = parse_command(parser, block_scope, error, true);
        ReturnIfError();

		array_push(commands, command);

		++command_count;
        if(command->flags & AST_FLAG_DECLARATION)
            block_scope->decl_count++;

		if (lexer_peek(lexer)->type == '}')
			break;
	}

	lexer_next(lexer); // eat '}'

	Light_Ast* result = ast_new_comm_block(scope, commands, command_count, block_scope);
	result->comm_block.block_scope->creator_node = result;

	return result;
}

static Light_Ast*
parse_comm_if(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;

    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_IF);
    ReturnIfError();

	Light_Ast* condition = parse_expression(parser, scope, error);
    ReturnIfError();

	Light_Ast* command_true  = 0;
	Light_Ast* command_false = 0;

	command_true = parse_command(parser, scope, error, true);
    ReturnIfError();

	if (lexer_peek(lexer)->type == TOKEN_KEYWORD_ELSE) {
		lexer_next(lexer); // eat 'else'

        command_false = parse_command(parser, scope, error, true);
        ReturnIfError();
	}
	return ast_new_comm_if(scope, condition, command_true, command_false);
}

static Light_Ast*
parse_comm_while(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_WHILE);
    ReturnIfError();

	Light_Ast* condition = parse_expression(parser, scope, error);
    ReturnIfError();

	Light_Ast* body = parse_command(parser, scope, error, true);
    ReturnIfError();

	if (body->kind == AST_COMMAND_BLOCK) {
		if (body->comm_block.block_scope)
			body->comm_block.block_scope->flags |= SCOPE_LOOP;
	}
	return ast_new_comm_while(scope, condition, body);
}

static Light_Ast** 
parse_comm_comma_list(Light_Parser* parser, Light_Scope* scope, u32* error, s32* decl_count) {
	Light_Ast** commands = array_new(Light_Ast*);
    s32 dc = 0;
	while(true) {
		Light_Ast* comm = parse_command(parser, scope, error, false);
		array_push(commands, comm);
        if(comm->flags & AST_FLAG_DECLARATION)
            dc++;
        
		if (lexer_peek(parser->lexer)->type != ',') {
			break;
		} else {
			lexer_next(parser->lexer); // eat ','
		}
	}
    if(decl_count) *decl_count = dc;
	return commands;
}

static Light_Ast*
parse_comm_for(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;

    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_FOR);
    ReturnIfError();

    Light_Ast** prologue = 0;
    Light_Ast** epilogue = 0;
    Light_Ast* condition = 0;

    Light_Scope* for_scope = light_scope_new(0, scope, SCOPE_LOOP);

    // Parse prologue command
    if(lexer_peek(lexer)->type != ';') {
        s32 decl_count = 0;
        prologue = parse_comm_comma_list(parser, for_scope, error, &decl_count);
        ReturnIfError();

        for_scope->decl_count += decl_count;
    }
    *error |= parser_require_and_eat(parser, ';');
    ReturnIfError();

    // Parse condition
    if(lexer_peek(lexer)->type != ';') {
        condition = parse_expression(parser, for_scope, error);
        ReturnIfError();
    }

    *error |= parser_require_and_eat(parser, ';');
    ReturnIfError();

    // Parse epiligue command
    s32 epilogue_decl_count = 0;
    epilogue = parse_comm_comma_list(parser, for_scope, error, &epilogue_decl_count);
    ReturnIfError();

    for_scope->decl_count += epilogue_decl_count;

    // Parse Body
    Light_Ast* body = parse_command(parser, for_scope, error, true);
    ReturnIfError();

    Light_Ast* result = ast_new_comm_for(scope, for_scope, condition, body, prologue, epilogue);
    for_scope->creator_node = result;

    return result;
}

static Light_Ast*
parse_comm_break(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_BREAK);
    ReturnIfError();
    Light_Token* break_token = lexer_peek_n(parser->lexer, -1);
    
	Light_Ast* lit = 0;
	Light_Token* next = lexer_peek(parser->lexer);
	switch(next->type) {
        case '#': assert(0); break; // TODO(psv): directive
        case TOKEN_LITERAL_DEC_INT:
        case TOKEN_LITERAL_BIN_INT:
        case TOKEN_LITERAL_HEX_INT:
            lit = ast_new_expr_literal_primitive(scope, next);
            break;
        default:
            *error |= parser_error_fatal(parser, next, "break expects either a positive integer literal or no argument, given '%.*s' instead\n", TOKEN_STR(next));
            return 0;
    }
	return ast_new_comm_break(scope, break_token, lit);
}

static Light_Ast*
parse_comm_continue(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_CONTINUE);
    ReturnIfError();
    Light_Token* continue_token = lexer_peek_n(parser->lexer, -1);
    
	Light_Ast* lit = 0;
	Light_Token* next = lexer_peek(parser->lexer);
	switch(next->type) {
        case '#': assert(0); break; // TODO(psv): directive
        case TOKEN_LITERAL_DEC_INT:
        case TOKEN_LITERAL_BIN_INT:
        case TOKEN_LITERAL_HEX_INT:
            lit = ast_new_expr_literal_primitive(scope, next);
            break;
        default:
            *error |= parser_error_fatal(parser, next, "break expects either a positive integer literal or no argument, given '%.*s' instead\n", TOKEN_STR(next));
            return 0;
    }
	return ast_new_comm_continue(scope, continue_token, lit);
}

static Light_Ast*
parse_comm_return(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_RETURN);
    ReturnIfError();
    Light_Token* return_token = lexer_peek_n(parser->lexer, -1);

	Light_Ast* expr = 0;
	if (lexer_peek(parser->lexer)->type != ';')
		expr = parse_expression(parser, scope, error);
	return ast_new_comm_return(scope, expr, return_token);
}

static Light_Ast*
parse_comm_assignment(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;

    Light_Ast* lvalue = parse_expression(parser, scope, error);
    ReturnIfError();
    Light_Ast* dereferenced_lvalue = 0;

    if(lvalue->kind == AST_EXPRESSION_VARIABLE) {
        // We must dereference automatically, so as to an expression like:
        // a = ...  will be transformed into *a = ...
        dereferenced_lvalue = ast_new_expr_unary(scope, lvalue, 0, OP_UNARY_DEREFERENCE);
    } else {
        dereferenced_lvalue = lvalue;
    }

    Light_Operator_Binary binop = OP_BINARY_UNKNOWN;
    Light_Token* op = lexer_next(lexer);
    switch (op->type) {
		case '=':
			break;
		case TOKEN_PLUS_EQUAL:	binop = OP_BINARY_PLUS; break;
		case TOKEN_MINUS_EQUAL:	binop = OP_BINARY_MINUS; break;
		case TOKEN_TIMES_EQUAL:	binop = OP_BINARY_MULT; break;
		case TOKEN_DIV_EQUAL:	binop = OP_BINARY_DIV; break;
		case TOKEN_MOD_EQUAL:	binop = OP_BINARY_MOD; break;
		case TOKEN_SHL_EQUAL:	binop = OP_BINARY_SHL; break;
		case TOKEN_SHR_EQUAL:	binop = OP_BINARY_SHR; break;
		case TOKEN_XOR_EQUAL:	binop = OP_BINARY_XOR; break;
		case TOKEN_AND_EQUAL:	binop = OP_BINARY_AND; break;
		case TOKEN_OR_EQUAL:	binop = OP_BINARY_OR; break;
		default: {
			*error |= parser_error_fatal(parser, op, "expected assignment operator, but got '%.*s'\n", TOKEN_STR(op));
            return 0;
		}break;
	}

    Light_Ast* rvalue = parse_expression(parser, scope, error);
    ReturnIfError();

    if(binop != OP_BINARY_UNKNOWN) {
        // Syntax sugar, transform a += b into a = a + b
        rvalue = ast_new_expr_binary(scope, lvalue, rvalue, op, binop);
    }

    return ast_new_comm_assignment(scope, lvalue, rvalue);
}

Light_Ast*
parse_command(Light_Parser* parser, Light_Scope* scope, u32* error, bool require_semicolon) {
    Light_Lexer* lexer = parser->lexer;

    Light_Token* next = lexer_peek(lexer);
	Light_Ast* command = 0;
	switch (next->type) {
		case '{':
			command = parse_comm_block(parser, scope, error);
			break;
		case TOKEN_KEYWORD_IF:
			command = parse_comm_if(parser, scope, error); 
			break;
		case TOKEN_KEYWORD_WHILE:
			command = parse_comm_while(parser, scope, error);
			break;
		case TOKEN_KEYWORD_FOR:
			command = parse_comm_for(parser, scope, error);
			break;
		case TOKEN_KEYWORD_BREAK:
			command = parse_comm_break(parser, scope, error);
            if(require_semicolon) {
                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }
			break;
		case TOKEN_KEYWORD_CONTINUE:
			command = parse_comm_continue(parser, scope, error);
            if(require_semicolon) {
                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }
			break;
		case TOKEN_KEYWORD_RETURN:
			command = parse_comm_return(parser, scope, error);
            if(require_semicolon) {
                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }
			break;
		case TOKEN_IDENTIFIER:{
			Light_Token_Type t = lexer_peek_n(lexer, 1)->type;
			if (t == ':') {
				command = parse_declaration(parser, scope, error);
			} else if (t == '(') {
                // TODO(psv):
				// syntatic sugar void proc call
				Light_Ast* pcall = parse_expression(parser, scope, error);
				//command = ast_create_comm_variable_assignment(scope, 0, pcall);
			} else {
				command = parse_comm_assignment(parser, scope, error);
			}
            if(require_semicolon) {
                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }
		}break;
		default: {
			command = parse_comm_assignment(parser, scope, error);
            if(require_semicolon) {
                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }
		}break;
	}
	return command;
}

static Light_Ast* 
parse_decl_procedure(Light_Parser* parser, Light_Token* name, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;

    if(!name) {
        name = lexer_next(lexer);
        if(name->type != TOKEN_IDENTIFIER) {
            *error |= parser_error_fatal(parser, name, "expected identifier name in procedure declaration, but got '%.*s'\n", TOKEN_STR(name));
            return 0;
        }
        *error |= parser_require_and_eat(parser, ':');
        ReturnIfError();
        *error |= parser_require_and_eat(parser, ':');
        ReturnIfError();
    }

    *error |= parser_require_and_eat(parser, '(');
    ReturnIfError();

    Light_Scope* args_scope = light_scope_new(0, scope, SCOPE_PROCEDURE_ARGUMENTS);
    Light_Ast**  arguments = 0;
    s32 args_count = 0;

    if(lexer_peek(lexer)->type != ')') {
        arguments = array_new(Light_Ast*);
        for (;;) {
			if (args_count != 0) {
                *error |= parser_require_and_eat(parser, ',');
                ReturnIfError();
            }
			Light_Token* name = lexer_next(lexer);
			if (name->type != TOKEN_IDENTIFIER) {
				*error |= parser_error_fatal(parser, name, "expected argument #%d declaration identifier but got '%.*s'\n", args_count + 1, TOKEN_STR(name));
                return 0;
			}
			Light_Ast* arg = parse_decl_variable(parser, name, 0, args_scope, error);
			array_push(arguments, arg);

            args_scope->decl_count++;
			++args_count;

			if (lexer_peek(lexer)->type != ',') break;
		}        
    }

    *error |= parser_require_and_eat(parser, ')');
    ReturnIfError();

    Light_Type* return_type = 0;
	if (lexer_peek(lexer)->type == '{') {
		return_type = type_primitive_get(TYPE_PRIMITIVE_VOID);
	} else {
		*error |= parser_require_and_eat(parser, TOKEN_ARROW);
        ReturnIfError();
		return_type = parse_type(parser, scope, error);
        ReturnIfError();
	}

    Light_Ast* body = 0;

    // TODO(psv): foreign declaration and forward declaration
    body = parse_comm_block(parser, scope, error);
    ReturnIfError();

    Light_Ast* result = ast_new_decl_procedure(scope, name, body, return_type, args_scope, (Light_Ast_Decl_Variable**)arguments, args_count, 0);

    if(body) {
        body->comm_block.block_scope->creator_node = result;
    }

    return result;
}

static Light_Ast*
parse_decl_variable(Light_Parser* parser, Light_Token* name, Light_Type* type, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;
    if(!name) {
        name = lexer_next(lexer);
        if(name->type != TOKEN_IDENTIFIER) {
            *error |= parser_error_fatal(parser, name, "expected identifier name in variable declaration, but got '%.*s'\n", TOKEN_STR(name));
            ReturnIfError();
        }
    }

    if(!type) {
        *error |= parser_require_and_eat(parser, ':');
        ReturnIfError();
        
        if(lexer_peek(lexer)->type != '=') {
            type = parse_type(parser, scope, error);
            ReturnIfError();
        }
    }

    Light_Ast* expr = 0;
    if(lexer_peek(lexer)->type == '=') {
        lexer_next(lexer); // eat '='
        expr = parse_expression(parser, scope, error);
        ReturnIfError();
    }

    return ast_new_decl_variable(scope, name, type, expr, STORAGE_CLASS_STACK, 0);
}

static Light_Ast*
parse_decl_constant(Light_Parser* parser, Light_Token* name, Light_Type* type, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;
    if(!name) {
        name = lexer_next(lexer);
        if(name->type != TOKEN_IDENTIFIER) {
            *error |= parser_error_fatal(parser, name, "expected identifier name in constant declaration, but got '%.*s'\n", TOKEN_STR(name));
            ReturnIfError();
        }
    }

    if(!type) {
        *error |= parser_require_and_eat(parser, ':');
        ReturnIfError();
        
        if(lexer_peek(lexer)->type != ':') {
            type = parse_type(parser, scope, error);
            ReturnIfError();
        }
    }

    Light_Ast* expr = 0;
    if(lexer_peek(lexer)->type == ':') {
        lexer_next(lexer); // eat ':'
        expr = parse_expression(parser, scope, error);
        ReturnIfError();
    }

    return ast_new_decl_constant(scope, name, type, expr, 0);
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
                lexer_next(lexer); // eat ':'
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
            result = ast_new_decl_typedef(scope, type, name);
        } break;
        default: {
            *error |= parser_error_fatal(parser, next, "invalid token '%.*s' after declaration name\n", TOKEN_STR(next));
        }break;
    }

    return result;
}

Light_Ast* 
parse_expression(Light_Parser* parser, Light_Scope* scope, u32* error) {
    // TODO(psv):
    return 0;
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

        enum_scope->decl_count++;

        if(lexer_peek(parser->lexer)->type != ',') break;
    }

    *error |= parser_require_and_eat(parser, '}');
    ReturnIfError();


    s32 fields_count = array_length(fields_names);
    Light_Type* result = type_new_enum(fields_names, fields_values, fields_count, type_hint, enum_scope);

    enum_scope->creator_type = result;

    return result;
}

static Light_Type*
parse_type_struct(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_STRUCT);
    ReturnIfError();
    *error |= parser_require_and_eat(parser, '{');
    ReturnIfError();

    Light_Ast** fields = array_new(Light_Ast*);
    Light_Scope* struct_scope = light_scope_new(0, scope, SCOPE_STRUCTURE);

    s32 field_count = 0;
    for(;;) {
        Light_Ast* field = parse_decl_variable(parser, 0, 0, struct_scope, error);
        ReturnIfError();
        field->decl_variable.flags |= DECL_VARIABLE_FLAG_STRUCT_FIELD;
        field->decl_variable.field_index = field_count;
        array_push(fields, field);

        *error |= parser_require_and_eat(parser, ';');
        ReturnIfError();

        struct_scope->decl_count++;

        ++field_count;
        if(lexer_peek(parser->lexer)->type == '}') break;
    }

    *error |= parser_require_and_eat(parser, '}');
    ReturnIfError();

    Light_Type* result = type_new_struct(fields, field_count, struct_scope);
    struct_scope->creator_type = result;

    return result;
}

static Light_Type*
parse_type_union(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_UNION);
    ReturnIfError();
    *error |= parser_require_and_eat(parser, '{');
    ReturnIfError();

    Light_Ast** fields = array_new(Light_Ast*);
    Light_Scope* union_scope = light_scope_new(0, scope, SCOPE_UNION);

    s32 field_count = 0;
    for(;;) {
        Light_Ast* field = parse_decl_variable(parser, 0, 0, union_scope, error);
        ReturnIfError();
        field->decl_variable.flags |= DECL_VARIABLE_FLAG_UNION_FIELD;
        field->decl_variable.field_index = field_count;
        array_push(fields, field);

        *error |= parser_require_and_eat(parser, ';');
        ReturnIfError();

        union_scope->decl_count++;

        ++field_count;
        if(lexer_peek(parser->lexer)->type == '}') break;
    }

    *error |= parser_require_and_eat(parser, '}');
    ReturnIfError();

    Light_Type* result = type_new_union(fields, field_count, union_scope);
    union_scope->creator_type = result;

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
            return parse_type_struct(parser, scope, error);
        case TOKEN_KEYWORD_UNION:
            return parse_type_union(parser, scope, error);
        default:
            *error |= parser_error_fatal(parser, t, "invalid token '%.*s' in type declaration\n", TOKEN_STR(t));
            break;
    }

    return 0;
}