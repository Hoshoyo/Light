#include "parser.h"
#include "error.h"
#include "global_tables.h"
#include "utils/os.h"
#include "utils/allocator.h"
#include <light_array.h>
#include <stdarg.h>
#include <assert.h>

//#include "hash_tables.h"

#define ReturnIfError() if(*error & PARSER_ERROR_FATAL) return 0
#define TOKEN_STR(T) (T)->length, (T)->data

// Forward declarations
static Light_Ast* parse_decl_variable(Light_Parser* parser, Light_Token* name, Light_Type* type, Light_Scope* scope, u32* error, bool require_expr);

static s32
parser_error_location(Light_Parser* parser, Light_Token* t) {
    if(!t) {
        return fprintf(stderr, "%s: ", parser->lexer->filepath);
    } else {
        return fprintf(stderr, "%s:%d:%d: ", parser->lexer->filepath, t->line + 1, t->column + 1);
    }
}

static u32
parser_error_fatal(Light_Parser* parser, Light_Token* loc, const char* fmt, ...) {
    va_list args;
	va_start(args, fmt);
    parser_error_location(parser, loc);
	fprintf(stderr, "Syntax Error: ");
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
parse_directive(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, '#');
    ReturnIfError();

    Light_Token* tag = lexer_next(parser->lexer);

    if(tag->type == TOKEN_IDENTIFIER && tag->data == (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_IMPORT].data) {
        Light_Token* filename_token = lexer_next(parser->lexer);

        if(filename_token->type != TOKEN_LITERAL_STRING) {
            *error |= parser_error_fatal(parser, filename_token, "Expected filename as a string literal\n");
            return 0;
        }

        // Directive #import "filename"
        const char* current_filepath_absolute = parser->lexer->filepath_absolute;

        char* full_imported_filepath = light_filepath_relative_to(
            (char*)filename_token->data + 1, filename_token->length - 2, 
            current_filepath_absolute);

        if(!full_imported_filepath) {
            // TODO(psv): maybe to not fatal here
            *error |= parser_error_fatal(parser, filename_token, "Could not import %.*s: file not found\n", TOKEN_STR(filename_token));
            ReturnIfError();
        }

        string src_str = {0};
        src_str.data = full_imported_filepath;
        src_str.length = strlen(full_imported_filepath);

        int index = 0;
        if(!string_table_entry_exist(&parser->parse_queue, src_str, &index, 0)) 
        {
            string_table_add(&parser->parse_queue, src_str, &index);
            array_push(parser->parse_queue_array, src_str);
        }
    } else if(tag->type == TOKEN_IDENTIFIER && tag->data == (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_EXTERN].data) {
        // TODO(psv): Implement extern
        assert(0);        
    } else if(tag->type == TOKEN_IDENTIFIER) {
        *error |= parser_error_fatal(parser, tag, "Unrecognized directive '%.*s'\n", tag->length, tag->data);
    } else if(tag->type != TOKEN_END_OF_STREAM) {
        *error |= parser_error_fatal(parser, tag, "Expected directive identifier but got '%.*s'\n", tag->length, tag->data);
    } else {
        *error |= parser_error_fatal(parser, tag, "Unexpected end of file in directive declaration\n");
    }

    return 0;
}

static Light_Ast*
parse_comm_block(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;

    *error |= parser_require_and_eat(parser, '{');
    ReturnIfError();
    Light_Token* first = lexer_peek_n(lexer, -1);
	
	if (lexer_peek(lexer)->type == '}') {
		Light_Token* last = lexer_next(lexer); // eat '{'
		return ast_new_comm_block(scope, 0, 0, 0, (Lexical_Range){first, last});
	}

	Light_Ast** commands = array_new(Light_Ast*);
	Light_Scope* block_scope = light_scope_new(0, scope, SCOPE_BLOCK);

	s32 command_count = 0;
	for (;;) {
		Light_Ast* command = parse_command(parser, block_scope, error, true);
        ReturnIfError();

		array_push(commands, command);

		++command_count;
        if(command->flags & AST_FLAG_DECLARATION) {
            block_scope->decl_count++;
            array_push(block_scope->decls, command);
        }

		if (lexer_peek(lexer)->type == '}')
			break;
	}

	Light_Token* last = lexer_next(lexer); // eat '}'

	Light_Ast* result = ast_new_comm_block(scope, commands, command_count, block_scope, (Lexical_Range){first, last});
	result->comm_block.block_scope->creator_node = result;

	return result;
}

static Light_Ast*
parse_comm_if(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;

    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_IF);
    ReturnIfError();
    Light_Token* if_token = lexer_peek_n(lexer, -1);

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
    Lexical_Range lrange = {if_token, lexer_peek_n(parser->lexer, -1)};
	return ast_new_comm_if(scope, condition, command_true, command_false, if_token, lrange);
}

static Light_Ast*
parse_comm_while(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_WHILE);
    ReturnIfError();
    Light_Token* while_token = lexer_peek_n(parser->lexer, -1);

	Light_Ast* condition = parse_expression(parser, scope, error);
    ReturnIfError();

	Light_Ast* body = parse_command(parser, scope, error, true);
    ReturnIfError();

	if (body->kind == AST_COMMAND_BLOCK) {
		if (body->comm_block.block_scope)
			body->comm_block.block_scope->flags |= SCOPE_LOOP;
	} else {
        Light_Scope* while_scope = light_scope_new(0, scope, SCOPE_LOOP);
        Light_Ast** while_commands = array_new(Light_Ast*);
        array_push(while_commands, body);

        body = ast_new_comm_block(scope, while_commands, 1, while_scope, body->lexical_range);
    }
	return ast_new_comm_while(scope, condition, body, while_token, (Lexical_Range){while_token, body->lexical_range.end});
}

static Light_Ast** 
parse_comm_comma_list(Light_Parser* parser, Light_Scope* scope, u32* error, s32* decl_count) {
	Light_Ast** commands = array_new(Light_Ast*);
    s32 dc = 0;
	while(true) {
		Light_Ast* comm = parse_command(parser, scope, error, false);
		array_push(commands, comm);
        if(comm->flags & AST_FLAG_DECLARATION) {
            dc++;
            array_push(scope->decls, comm);
        }
        
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
    Light_Token* for_token = lexer_peek_n(lexer, -1);

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
    
    Lexical_Range lrange = {for_token, body->lexical_range.end};
    Light_Ast* result = ast_new_comm_for(scope, for_scope, condition, body, prologue, epilogue, for_token, lrange);
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
        case TOKEN_LITERAL_HEX_INT: {
            Light_Token* toki = lexer_next(parser->lexer); // integer
            lit = ast_new_expr_literal_primitive(scope, next, (Lexical_Range){toki, toki});
        } break;
        case ';': break;
        default:
            *error |= parser_error_fatal(parser, next, "break expects either a positive integer literal or no argument, given '%.*s' instead\n", TOKEN_STR(next));
            return 0;
    }
    Lexical_Range lrange = {break_token};
    if(lit) {
        lrange.end = lit->lexical_range.end;
    }
	return ast_new_comm_break(scope, break_token, lit, lrange);
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
        case TOKEN_LITERAL_HEX_INT: {
            Light_Token* toki = lexer_next(parser->lexer); // eat integer
            lit = ast_new_expr_literal_primitive(scope, next, (Lexical_Range){toki, toki});
        } break;
        case ';': break;
        default:
            *error |= parser_error_fatal(parser, next, "break expects either a positive integer literal or no argument, given '%.*s' instead\n", TOKEN_STR(next));
            return 0;
    }
    Lexical_Range lrange = {continue_token, continue_token};
    if(lit) {
        lrange.end = lit->lexical_range.end;
    }
	return ast_new_comm_continue(scope, continue_token, lit, lrange);
}

static Light_Ast*
parse_comm_return(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, TOKEN_KEYWORD_RETURN);
    ReturnIfError();
    Light_Token* return_token = lexer_peek_n(parser->lexer, -1);

    Lexical_Range lrange = (Lexical_Range){return_token, return_token};
	Light_Ast* expr = 0;
	if (lexer_peek(parser->lexer)->type != ';'){
		expr = parse_expression(parser, scope, error);
        lrange.end = expr->lexical_range.end;
    }
	return ast_new_comm_return(scope, expr, return_token, lrange);
}

static Light_Ast*
parse_comm_assignment(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;

    Light_Ast* lvalue = parse_expression(parser, scope, error);
    ReturnIfError();

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
        rvalue = ast_new_expr_binary(scope, lvalue, rvalue, op, binop, rvalue->lexical_range);
    }

    Lexical_Range lrange = (Lexical_Range){lvalue->lexical_range.start, rvalue->lexical_range.end};
    return ast_new_comm_assignment(scope, lvalue, rvalue, op, lrange);
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
            ReturnIfError();
            if(require_semicolon) {
                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }
			break;
		case TOKEN_KEYWORD_CONTINUE:
			command = parse_comm_continue(parser, scope, error);
            ReturnIfError();
            if(require_semicolon) {
                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }
			break;
		case TOKEN_KEYWORD_RETURN:
			command = parse_comm_return(parser, scope, error);
            ReturnIfError();
            if(require_semicolon) {
                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }
			break;
		case TOKEN_IDENTIFIER:{
            Light_Token* t = lexer_peek_n(lexer, 1);
			if (t->type == '.' || t->flags & TOKEN_FLAG_ASSIGNMENT_OPERATOR) {
				command = parse_comm_assignment(parser, scope, error);
			} else if (t->type == '(') {
				// Syntatic sugar void proc call
				Light_Ast* pcall = parse_expression(parser, scope, error);
                ReturnIfError();
                command = ast_new_comm_assignment(scope, 0, pcall, next, (Lexical_Range){next, pcall->lexical_range.end});
            } else if(t->type == '[') {
                command = parse_comm_assignment(parser, scope, error);
            } else {
				command = parse_declaration(parser, scope, false, error);
			}
            if(require_semicolon) {
                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }
		}break;
		default: {
			command = parse_comm_assignment(parser, scope, error);
            ReturnIfError();
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
    Light_Token* proc_name = name;

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

    bool is_variable_declaration = false;
    // Check if the pattern 'name :' appears next, that indicates
    // a normal procedure declaration instead of a type declaration.
    if( lexer_peek(lexer)->type == '(' &&
        lexer_peek_n(lexer, 1)->type == TOKEN_IDENTIFIER && 
        (lexer_peek_n(lexer, 2)->type == ':' /* || lexer_peek_n(lexer, 2)->type == ','*/))
    {
        // Normal declaration
    } else if(
        lexer_peek(lexer)->type == '(' &&
        lexer_peek_n(lexer, 1)->type == ')')
    {
        // Normal declaration
        s32 current_lexer_index = lexer->index;

        parse_type(parser, scope, error);   // discard

        if(lexer_peek(lexer)->type == ';')
            is_variable_declaration = true;
        lexer->index = current_lexer_index;
    } else {
        is_variable_declaration = true;
    }

    if(is_variable_declaration) {
        Light_Type* function_type = parse_type(parser, scope, error);
        ReturnIfError();

        // This is in fact a variable declaration of procedure type.
        Light_Ast* decl_var = ast_new_decl_variable(scope, name, function_type, 0, STORAGE_CLASS_DATA_SEGMENT, 0, (Lexical_Range){name, name});

        if(scope->level == 0) {
            // global scope
            *error |= parser_require_and_eat(parser, ';');
            ReturnIfError();
        }

        return decl_var;
    }

    *error |= parser_require_and_eat(parser, '(');
    ReturnIfError();

    Light_Scope* args_scope = light_scope_new(0, scope, SCOPE_PROCEDURE_ARGUMENTS);
    Light_Ast**  arguments = 0;
    s32 args_count = 0;
    Light_Type** args_types = 0;
    bool all_args_internalized = true;
    bool variable_declaration = false;
    u32 function_flags = 0;
    bool variadic = false;

    if(lexer_peek(lexer)->type != ')') {
        arguments = array_new(Light_Ast*);
        args_types = array_new(Light_Type*);
        for (;;) {
			if (args_count != 0) {
                *error |= parser_require_and_eat(parser, ',');
                ReturnIfError();
            }

            bool multiple_args = false;
            int multiple_count = 0;
            Light_Token* name = 0;
            do {
                name = lexer_next(lexer);
                if (name->type != TOKEN_IDENTIFIER) {
                    *error |= parser_error_fatal(parser, name, "expected argument #%d declaration identifier but got '%.*s'\n", args_count + 1, TOKEN_STR(name));
                    return 0;
                }

                Light_Ast* arg_decl = 0;
                // multiple arguments declarations
                if(lexer_peek(lexer)->type == ',') {
                    multiple_args = true;
                    lexer_next(lexer);
                    arg_decl = ast_new_decl_variable(args_scope, name, 0, 0, STORAGE_CLASS_STACK, 0, (Lexical_Range){name, name});
                    arg_decl->decl_variable.flags |= DECL_VARIABLE_FLAG_PROC_ARGUMENT;
                    array_push(arguments, arg_decl);   // defer filling type
                    array_push(args_types, 0);         // defer filling this
                } else {
                    multiple_args = false;
                }

                multiple_count++;
            } while(multiple_args);

            *error |= parser_require_and_eat(parser, ':');
            ReturnIfError();

            Light_Type* arg_type = 0;

            // TODO(psv): refactor token to be ...
            if(lexer_peek(parser->lexer)->type == '.') {
                if(multiple_count > 1) {
                    *error |= parser_error_fatal(parser, proc_name, "cannot have two or more arguments as variadic\n");
                    ReturnIfError();
                }
                *error |= parser_require_and_eat(parser, '.');
                ReturnIfError();
                *error |= parser_require_and_eat(parser, '.');
                ReturnIfError();
                *error |= parser_require_and_eat(parser, '.');
                ReturnIfError();
                function_flags |= TYPE_FUNCTION_VARIADIC;
                variadic = true;

                // TODO(psv): make it ^User_Type_Info when variadic
                // this will probably be done in the type checking phase
                arg_type = type_new_pointer(type_primitive_get(TYPE_PRIMITIVE_VOID));
            } else {
                arg_type = parse_type(parser, scope, error);
                ReturnIfError();
            }

            Light_Ast* arg = parse_decl_variable(parser, name, arg_type, args_scope, error, false);
            arg->decl_variable.flags |= DECL_VARIABLE_FLAG_PROC_ARGUMENT;
            array_push(arguments, arg);
            array_push(args_types, arg_type);

            for(int aa = 1; aa < multiple_count; ++aa) {
                arguments[array_length(arguments) - 1 - aa]->decl_variable.type = arg_type;
                args_types[array_length(args_types) - 1 - aa] = arg_type;
                args_scope->decl_count++;
                array_push(args_scope->decls, arguments[array_length(arguments) - 1 - aa]);
                ++args_count;
            }

            args_scope->decl_count++;
            array_push(args_scope->decls, arg);
            ++args_count;

            if(!(arg_type->flags & TYPE_FLAG_INTERNALIZED))
                all_args_internalized = false;

            if(variadic)
                break; // must be the last one

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
    u32 flags = 0;
    Light_Token* external_library_name = 0;

    // compiler parameters
    if(lexer_peek(lexer)->type == '#') {
        lexer_next(lexer); // eat #
        Light_Token* tag = lexer_next(lexer);

        // extern
        if(tag->data == (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_EXTERN].data) {
            
            if (lexer_peek(lexer)->type == '(')
            {
                *error |= parser_require_and_eat(parser, '(');
                ReturnIfError();
                flags |= DECL_PROC_FLAG_EXTERN;
            
                external_library_name = lexer_next(lexer);

                if(external_library_name->type != TOKEN_LITERAL_STRING) {
                    *error |= parser_error_fatal(parser, external_library_name, "expected library name as a string literal\n");
                    ReturnIfError();
                }

                *error |= parser_require_and_eat(parser, ')');
                ReturnIfError();

                *error |= parser_require_and_eat(parser, ';');
                ReturnIfError();
            }

            function_flags |= TYPE_FUNCTION_STDCALL;
        }
    }

    Light_Type* proc_type = type_new_function(args_types, return_type, args_count, all_args_internalized, function_flags);

    if(lexer_peek(lexer)->type == ';') {
        // Variable with procedure type
        Light_Ast* decl_var = ast_new_decl_variable(scope, name, proc_type, 0, STORAGE_CLASS_DATA_SEGMENT, 0, (Lexical_Range){name, name});
        return decl_var;
    }

    Lexical_Range decl_range = {
        .start = name,
        .end = lexer_peek_n(parser->lexer, -1),
    };

    if(!(flags & DECL_PROC_FLAG_EXTERN)) {
        body = parse_comm_block(parser, args_scope, error);
        ReturnIfError();
    }

    if(variadic) {
        flags |= DECL_PROC_FLAG_VARIADIC;
    }

    Light_Ast* result = ast_new_decl_procedure(scope, name, body, return_type, args_scope, arguments, args_count, flags, (Lexical_Range){0});
    result->decl_proc.proc_type = proc_type;
    result->decl_proc.extern_library_name = external_library_name;
    args_scope->creator_node = result;

    if(body && body->comm_block.block_scope) {
        body->comm_block.block_scope->creator_node = result;
        body->comm_block.block_scope->flags |= SCOPE_PROCEDURE_BODY;
    }

    result->lexical_range.start = name;
    result->lexical_range.end = lexer_peek_n(parser->lexer, -1);

    result->decl_proc.decl_lexical_range = decl_range;

    return result;
}

static Light_Ast*
parse_decl_variable(Light_Parser* parser, Light_Token* name, Light_Type* type, Light_Scope* scope, u32* error, bool require_expr) {
    Light_Token* name_in = name;
    Light_Lexer* lexer = parser->lexer;
    if(!name) {
        name = lexer_next(lexer);
        if(name->type != TOKEN_IDENTIFIER) {
            *error |= parser_error_fatal(parser, name, "expected identifier name in variable declaration, but got '%.*s'\n", TOKEN_STR(name));
            ReturnIfError();
        }
    }

    if(!type && !name_in) {
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

    Lexical_Range lrange = {name, lexer_peek_n(lexer, -1)};
    return ast_new_decl_variable(scope, name, type, expr, STORAGE_CLASS_STACK, 0, lrange);
}

static void
parse_push_internal_file(Light_Parser* parser, const char* filepath, const char* compiler_path, u64 compiler_path_size) {
    // Find internal files based on compiler path
    uint64_t internal_file_size = 0;
    const char* internal_file = light_real_path_from(compiler_path, compiler_path_size, filepath, &internal_file_size);
    string ifile = {internal_file_size, 0, (char*)internal_file};
    array_push(parser->parse_queue_array, ifile);
    string_table_add(&parser->parse_queue, ifile, 0);
}

void
parse_init(Light_Parser* parser, Light_Lexer* lexer, Light_Scope* global_scope, const char* compiler_path, u64 compiler_path_size, const char* main_file) {
    parser->scope_global = global_scope;
    parser->lexer = lexer;
    parser->top_level = array_new_len(Light_Ast*, 1024);

    // Create hash table for files to parse
    parser->parse_queue_array = array_new_len(string, 2048);
    string_table_new(&parser->parse_queue, 1024 * 1024);

    // TODO(psv): Figure out if this is going to be done this way
#if 0
    parse_push_internal_file(parser, "/../modules/base.li", compiler_path, compiler_path_size); // base must be first
    parse_push_internal_file(parser, "/../modules/reflect.li", compiler_path, compiler_path_size);
#endif
    string mf = {strlen(main_file), 0, (char*)main_file};
    array_push(parser->parse_queue_array, mf);
    string_table_add(&parser->parse_queue, mf, 0);
}

Light_Ast** 
parse_top_level(Light_Parser* parser, Light_Lexer* lexer, Light_Scope* global_scope, u32* error) {
    // Check empty file
    if(lexer_peek(lexer)->type == TOKEN_END_OF_STREAM){
        return 0;
    } else {
        parser->lexer = lexer;
    }

    bool parsing = true;
    while(!(*error & PARSER_ERROR_FATAL) && parsing) {
        Light_Token* t = lexer_peek(lexer);

        switch(t->type) {
            case TOKEN_END_OF_STREAM: parsing = false; break;
            case '#':{
                Light_Ast* directive = parse_directive(parser, global_scope, error);
                if(directive) {
                    array_push(parser->top_level, directive);
                }
            }break;
            case TOKEN_IDENTIFIER:{
                Light_Ast* decl = parse_declaration(parser, global_scope, true, error);
                if(*error & PARSER_ERROR_FATAL) break;
                if(decl) {
                    array_push(parser->top_level, decl);
                    global_scope->decl_count++;
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
parse_declaration(Light_Parser* parser, Light_Scope* scope, bool require_semicolon, u32* error) {
    Light_Ast* result = 0;

    Light_Lexer* lexer = parser->lexer;
    Light_Token* name = lexer_next(lexer);

    if(name->type != TOKEN_IDENTIFIER) {
        *error |= parser_error_fatal(parser, name, "expected declaration name identifier, but got %s '%.*s'\n", 
            token_type_to_str(name->type), TOKEN_STR(name));
        return 0;
    }

    Light_Token* next = lexer_peek(lexer);

    if(next->type == ':') {
        // Might be a constant or variable declaration
        // Procedure declaration is tecnically a constant
        lexer_next(lexer);

        Light_Type* decl_type = 0;
        Light_Token* n = lexer_peek(lexer);
        if(n->type != ':' && n->type != '=') {
            if(n->type == '(') {
                // procedure declaration
                result = parse_decl_procedure(parser, name, scope, error);
                return result;
            }
            decl_type = parse_type(parser, scope, error);
        }

        n = lexer_peek(lexer);

        switch(n->type) {
            case ';': {
                if(require_semicolon) lexer_next(lexer); // eat ';'
                // variable declaration without expression assignment
                result = ast_new_decl_variable(scope, name, decl_type, 0, STORAGE_CLASS_STACK, 0, (Lexical_Range){0});
            } break;
            case ':':
                lexer_next(lexer); // eat ':'
            default: { // : is optional
                // inferred constant declaration
                Light_Ast* expr = parse_expression(parser, scope, error);
                ReturnIfError();
                if(require_semicolon) {
                    *error |= parser_require_and_eat(parser, ';');
                    ReturnIfError();
                }
                result = ast_new_decl_constant(scope, name, decl_type, expr, 0, (Lexical_Range){0});
            }break;
            case '=': {
                lexer_next(lexer); // eat '='
                // inferred variable declaration
                Light_Ast* expr = parse_expression(parser, scope, error);
                ReturnIfError();
                if(require_semicolon) {
                    *error |= parser_require_and_eat(parser, ';');
                    ReturnIfError();
                }
                result = ast_new_decl_variable(scope, name, decl_type, expr, STORAGE_CLASS_STACK, 0, (Lexical_Range){0});
            }break;
        }
    } else {
        // typedef
        Light_Type* type = parse_type(parser, scope, error);
        ReturnIfError();
        if(require_semicolon && type && 
            type->kind != TYPE_KIND_ENUM && type->kind != TYPE_KIND_STRUCT && type->kind != TYPE_KIND_UNION) 
        {
            *error |= parser_require_and_eat(parser, ';');
        }
        type = type_new_alias(scope, name, type);
        result = ast_new_decl_typedef(scope, type, name, (Lexical_Range){0});
    }

    result->lexical_range.start = name;
    result->lexical_range.end = lexer_peek_n(lexer, -1);

    return result;
}

static Light_Operator_Binary
token_to_binary_op(Light_Token* token) {
    switch(token->type) {
        case '+':                   return OP_BINARY_PLUS; break;
        case '-':                   return OP_BINARY_MINUS; break;
        case '*':                   return OP_BINARY_MULT; break;
        case '/':                   return OP_BINARY_DIV; break;
        case '%':                   return OP_BINARY_MOD; break;
        case '&':                   return OP_BINARY_AND; break;
        case '|':                   return OP_BINARY_OR; break;
        case '^':                   return OP_BINARY_XOR; break;
        case TOKEN_BITSHIFT_LEFT:   return OP_BINARY_SHL; break;
        case TOKEN_BITSHIFT_RIGHT:  return OP_BINARY_SHR; break;
        case '<':                   return OP_BINARY_LT; break;
        case '>':                   return OP_BINARY_GT; break;
        case TOKEN_LESS_EQUAL:      return OP_BINARY_LE; break;
        case TOKEN_GREATER_EQUAL:   return OP_BINARY_GE; break;
        case TOKEN_EQUAL_EQUAL:     return OP_BINARY_EQUAL; break;
        case TOKEN_NOT_EQUAL:       return OP_BINARY_NOT_EQUAL; break;
        case TOKEN_LOGIC_AND:       return OP_BINARY_LOGIC_AND; break;
        case TOKEN_LOGIC_OR:        return OP_BINARY_LOGIC_OR; break;
        case '[':                   return OP_BINARY_VECTOR_ACCESS; break;
        default: return OP_BINARY_UNKNOWN; break;
    }
}

static Light_Operator_Unary
token_to_unary_op(Light_Token* token) {
    switch(token->type) {
        case '+': return OP_UNARY_PLUS;
        case '-': return OP_UNARY_MINUS;
        case '*': return OP_UNARY_DEREFERENCE;
        case '&': return OP_UNARY_ADDRESSOF;
        case '~': return OP_UNARY_BITWISE_NOT;
        case '[': return OP_UNARY_CAST;
        case '!': return OP_UNARY_LOGIC_NOT;
        default: return OP_UNARY_UNKNOWN;
    }
}

Light_Ast* parse_expr_literal(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;
	Light_Token* first = lexer_next(lexer);

    Light_Ast* result = 0;

    switch (first->type) {
        case TOKEN_LITERAL_DEC_INT:
        case TOKEN_LITERAL_DEC_UINT:
        case TOKEN_LITERAL_HEX_INT:
        case TOKEN_LITERAL_BIN_INT:
        case TOKEN_LITERAL_BOOL_FALSE:
        case TOKEN_LITERAL_BOOL_TRUE:
        case TOKEN_LITERAL_FLOAT:
        case TOKEN_LITERAL_CHAR:
        case TOKEN_KEYWORD_NULL:
            result = ast_new_expr_literal_primitive(scope, first, (Lexical_Range){first, first});
            break;
        case TOKEN_LITERAL_STRING: {
            // Convert string literal to struct string
            // string -> struct { u64 length, u64 capacity, u8* data }
            // Create a token for the string
            Light_Token* string_token = light_alloc(sizeof(Light_Token));
            string_token->line = first->line;
            string_token->column = first->column;
            string_token->filepath = first->filepath;
            string_token->flags = first->flags;
            string_token->type = TOKEN_IDENTIFIER;
            string_token->data = (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_STRING].data;
            string_token->length = light_special_idents_table[LIGHT_SPECIAL_IDENT_STRING].length;

            Light_Ast* arr = ast_new_expr_literal_array(scope, first, 0, (Lexical_Range){0});
            arr->flags |= AST_FLAG_COMPILER_GENERATED;
            arr->expr_literal_array.raw_data = true;
            arr->expr_literal_array.array_strong_type = 0;
            arr->expr_literal_array.data = first->raw_data;
            arr->expr_literal_array.data_length_bytes = (u64)first->raw_data_length;

            Light_Ast* cast = ast_new_expr_unary(scope, arr, string_token, OP_UNARY_CAST, (Lexical_Range){0});
            cast->flags |= AST_FLAG_COMPILER_GENERATED;
            cast->expr_unary.type_to_cast = type_new_pointer(type_primitive_get(TYPE_PRIMITIVE_U8));

            result = ast_new_expr_literal_struct(scope, string_token, first, 0, false, 0, (Lexical_Range) { .start = string_token, .end = string_token });
            result->expr_literal_struct.struct_exprs = array_new(Light_Ast*);
            result->flags |= AST_FLAG_COMPILER_GENERATED;

            // @String
            // length
            u64 len = first->raw_data_length;
            Light_Ast* ll0 = ast_new_expr_literal_primitive_u32(scope, 0, (Lexical_Range){0});
            ll0->flags |= AST_FLAG_COMPILER_GENERATED;
            array_push(result->expr_literal_struct.struct_exprs, ll0);
            // capacity
            Light_Ast* ll1 = ast_new_expr_literal_primitive_u32(scope, len, (Lexical_Range){0});
            ll1->flags |= AST_FLAG_COMPILER_GENERATED;
            array_push(result->expr_literal_struct.struct_exprs, ll1);
            // data
            array_push(result->expr_literal_struct.struct_exprs, cast);

            // @Syntactic Sugar
            // final result is equivalent to struct string { 0, 0, array "data" }
        } break;
        default: {
            *error |= parser_error_fatal(parser, first, "expected literal expression but got '%.*s'\n", TOKEN_STR(first));
        }break;
    }

	return result;
}

Light_Ast* 
parse_expr_literal_struct(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Lexical_Range lrange = {0};

    Light_Token* name = 0;
    if(lexer_peek(parser->lexer)->type == TOKEN_IDENTIFIER) {
        name = lexer_next(parser->lexer);
        *error |= parser_require_and_eat(parser, ':');
        ReturnIfError();
        lrange.start = name;
    }

	*error |= parser_require_and_eat(parser, '{');
    ReturnIfError();
    Light_Token* struct_token = lexer_peek_n(parser->lexer, -1);
    if (!lrange.start) {
        lrange.start = struct_token;
    }

    Light_Ast* result = 0;
    bool named = false;
    if(!name) named = true;
	
    Light_Scope* struct_scope = 0;
    // Check if it is named
    if( lexer_peek(parser->lexer)->type == TOKEN_IDENTIFIER && 
        lexer_peek_n(parser->lexer, 1)->type == ':' &&
        lexer_peek_n(parser->lexer, 2)->type != '{') 
    {
        named = true;
        struct_scope = light_scope_new(0, scope, SCOPE_STRUCTURE);
    }

	Light_Ast** exprs_or_decls = array_new(Light_Ast*);
	if(lexer_peek(parser->lexer)->type != '}'){
        u32 named_error = PARSER_OK;
		while(true) {
            if(named) {
                // Require full declaration
                Light_Ast* decl = parse_decl_variable(parser, 0, 0, struct_scope, error, true);
                ReturnIfError();
                if(!decl->decl_variable.assignment) {
                    // Do not allow named struct literal to have no assignment
                    named_error |= parser_error_fatal(parser, lexer_peek(parser->lexer), 
                        "expected assignment expression at struct literal declaration\n");

                    // We can keep going to maybe report more errors
                } else {
                    // Expression scope is the expression scope, not the struct,
                    // since we are parsing a struct literal.
                    decl->decl_variable.assignment->scope_at = scope;
                }
                struct_scope->decl_count++;

                array_push(exprs_or_decls, decl);
            } else {
                Light_Ast* expr = parse_expression(parser, scope, error);
                ReturnIfError();

                array_push(exprs_or_decls, expr);
            }

			if(lexer_peek(parser->lexer)->type != ','){
				break;
			} else {
				lexer_next(parser->lexer);
			}
		}
        *error |= named_error;
        ReturnIfError();
        
        result = ast_new_expr_literal_struct(scope, name, struct_token, exprs_or_decls, named, struct_scope, (Lexical_Range){0});
	}

	*error |= parser_require_and_eat(parser, '}');
    ReturnIfError();
    lrange.end = lexer_peek_n(parser->lexer, -1);

    if(!result) {
        result = ast_new_expr_literal_struct(scope, name, struct_token, 0, false, 0, lrange);
    } else {
        result->lexical_range = lrange;
    }

	return result;
}

Light_Ast* 
parse_expr_literal_array(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Lexical_Range lrange = {0};

    *error |= parser_require_and_eat(parser, '[');
    ReturnIfError();
    Light_Token* array_token = lexer_peek_n(parser->lexer, -1);
    lrange.start = array_token;

    Light_Ast* result = 0;
    Light_Ast** exprs = array_new(Light_Ast*);
	if(lexer_peek(parser->lexer)->type != ']') {
		while(true) {
			Light_Ast* expr = parse_expression(parser, scope, error);
			array_push(exprs, expr);

			if(lexer_peek(parser->lexer)->type != ','){
				break;
			} else {
				lexer_next(parser->lexer);
			}
		}
	}

	*error |= parser_require_and_eat(parser, ']');
    ReturnIfError();
    lrange.end = lexer_peek_n(parser->lexer, -1);

    result = ast_new_expr_literal_array(scope, array_token, exprs, lrange);

	return result;
}

Light_Ast*
parse_expr_directive(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Lexer* lexer = parser->lexer;
    Light_Token* pound = lexer_next(lexer); // eat #

    Light_Token* directive = lexer_next(lexer);

    if(directive->type != TOKEN_IDENTIFIER) {
        if(error) *error |= PARSER_ERROR_FATAL;
        *error |= parser_error_fatal(parser, directive, "expected directive but got '%.*s'\n", TOKEN_STR(directive));
        return 0;
    }

    Lexical_Range lrange = {0};
    lrange.start = pound;
    if(directive->data == (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_TYPEOF].data) {
        Light_Ast* expression = parse_expression(parser, scope, error);
        ReturnIfError();
        lrange.end = expression->lexical_range.end;
        return ast_new_expr_directive(scope, EXPR_DIRECTIVE_TYPEOF, directive, expression, 0, lrange);
    } else if(directive->data == (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_TYPEVALUE].data) {
        Light_Type* type = parse_type(parser, scope, error);
        ReturnIfError();
        lrange.end = directive;
        return ast_new_expr_directive(scope, EXPR_DIRECTIVE_TYPEVALUE, directive, 0, type, lrange);
    } else if(directive->data == (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_SIZEOF].data) {
        // can't have optional parantheses because of functional types
        Light_Type* type = parse_type(parser, scope, error);
        ReturnIfError();
        lrange.end = directive;
        return ast_new_expr_directive(scope, EXPR_DIRECTIVE_SIZEOF, directive, 0, type, lrange);
    } else if(directive->data == (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_RUN].data) {
        Light_Ast* expression = parse_expression(parser, scope, error);
        ReturnIfError();
        lrange.end = expression->lexical_range.end;
        return ast_new_expr_directive(scope, EXPR_DIRECTIVE_RUN, directive, expression, 0, lrange);
    } else {
        *error |= parser_error_fatal(parser, directive, "invalid directive expression '%.*s'\n", TOKEN_STR(directive));
        ReturnIfError();
    }

    // TODO(psv): #code #run
    return 0;
}

Light_Ast* 
parse_expression_precedence10(Light_Parser* parser, Light_Scope* scope, u32* error) {
	Light_Token* t = lexer_peek(parser->lexer);
	if (t->flags & TOKEN_FLAG_LITERAL || t->type == TOKEN_KEYWORD_NULL) {
		// literal
		return parse_expr_literal(parser, scope, error);
	} else if(t->type == '[') {
		// array literal
		return parse_expr_literal_array(parser, scope, error);
    } else if(t->type == '{') {
        // struct literal
        return parse_expr_literal_struct(parser, scope, error);
	} else if (t->type == TOKEN_IDENTIFIER) {
		// variable
        if(lexer_peek_n(parser->lexer, 1)->type == ':') {
            // struct literal
            return parse_expr_literal_struct(parser, scope, error);
        } else {
            lexer_next(parser->lexer); // eat identifier
            Lexical_Range lrange = {t, t};
            return ast_new_expr_variable(scope, t, lrange);
        }
	} else if(t->type == '#') {
		return parse_expr_directive(parser, scope, error);
	} else if(t->type == '(') {
		// ( expr )
		lexer_next(parser->lexer);
		Light_Ast* expr = parse_expression(parser, scope, error);
		*error |= parser_require_and_eat(parser, ')');
        ReturnIfError();
		return expr;
	} else {
        *error |= parser_error_fatal(parser, t, "expected expression but got '%.*s'\n", TOKEN_STR(t));
	}
	return 0;
}


Light_Ast* parse_expression_precedence9(Light_Parser* parser, Light_Scope* scope, u32* error) {
	Light_Token* op = 0;
	Light_Ast* expr = parse_expression_precedence10(parser, scope, error);
    ReturnIfError();
	while(true) {
		op = lexer_peek(parser->lexer);
		if(op->type == '['){
			lexer_next(parser->lexer);
			Light_Ast* r = parse_expression(parser, scope, error);
            ReturnIfError();

			*error |= parser_require_and_eat(parser, ']');
            ReturnIfError();
            
            Lexical_Range lrange = {0};
            lrange.start = expr->lexical_range.start;
            lrange.end = lexer_peek_n(parser->lexer, -1);
			
            expr = ast_new_expr_binary(scope, expr, r, op, token_to_binary_op(op), lrange);
		} else if(op->type == '(') {
			// procedure call
			Light_Ast**  arguments = 0;
			s32          args_count = 0;
			
			*error |= parser_require_and_eat(parser, '(');
            ReturnIfError();
			if (lexer_peek(parser->lexer)->type != ')') {
				arguments = array_new(Light_Ast*);
				for (;;) {
					if (args_count != 0) {
                        *error |= parser_require_and_eat(parser, ',');
                        ReturnIfError();
                    }
					Light_Ast* argument = parse_expression(parser, scope, error);
					array_push(arguments, argument);
					++args_count;
					if (lexer_peek(parser->lexer)->type != ',')
						break;
				}
			}
			*error |= parser_require_and_eat(parser, ')');
            ReturnIfError();

            Lexical_Range lrange = {0};
            lrange.start = expr->lexical_range.start;
            lrange.end = lexer_peek_n(parser->lexer, -1);

			return ast_new_expr_proc_call(scope, expr, arguments, args_count, op, lrange);
		} else {
			break;
		}
	}
	return expr;
}

Light_Ast* parse_expression_precedence8(Light_Parser* parser, Light_Scope* scope, u32* error) {
	Light_Token* op = 0;
	Light_Ast* expr = parse_expression_precedence9(parser, scope, error);
    ReturnIfError();
	while(true) {
		op = lexer_peek(parser->lexer);
		if(op->type == '.'){
			lexer_next(parser->lexer);
            Light_Token* ident = lexer_next(parser->lexer);
            if(ident->type != TOKEN_IDENTIFIER) {
                *error |= parser_error_fatal(parser, ident, "expected identifier after '.' operator, but got '%.*s'\n", TOKEN_STR(ident));
                return 0;
            }
            Lexical_Range dot_range = {0};
            dot_range.start = expr->lexical_range.start;
            dot_range.end = ident;
            expr = ast_new_expr_dot(scope, expr, ident, dot_range);

            while(true) {
				op = lexer_peek(parser->lexer);
				if(op->type == '['){
					lexer_next(parser->lexer);
					Light_Ast* r = parse_expression(parser, scope, error);
					*error |= parser_require_and_eat(parser, ']');
                    ReturnIfError();
                    
                    Lexical_Range lrange = {0};
                    lrange.start = expr->lexical_range.start;
                    lrange.end = lexer_peek_n(parser->lexer, -1);

					expr = ast_new_expr_binary(scope, expr, r, op, token_to_binary_op(op), lrange);
                    ReturnIfError();
				} else {
					break;
				}
			}
		} else {
			break;
		}
	}
	return expr;
}

Light_Ast* parse_expression_precedence7(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Token_Type next = lexer_peek(parser->lexer)->type;
	if (next == '~' || next == '!' || next == '&' || next == '*' || next == '-' || next == '+') {
		Light_Token* op = lexer_next(parser->lexer);
		Light_Ast* operand = parse_expression_precedence7(parser, scope, error);
        ReturnIfError();
        Lexical_Range lrange = {0};
        lrange.start = op;
        lrange.end = operand->lexical_range.end;
		return ast_new_expr_unary(scope, operand, op, token_to_unary_op(op), lrange);
	}
    return parse_expression_precedence8(parser, scope, error);
}

Light_Ast* parse_expression_precedence6(Light_Parser* parser, Light_Scope* scope, u32* error) {
	Light_Ast* expr = parse_expression_precedence7(parser, scope, error);
    if(lexer_peek(parser->lexer)->type == TOKEN_ARROW) {
        // cast operator
        Light_Token* op_token = lexer_next(parser->lexer);
        Light_Type* type = parse_type(parser, scope, error);
        ReturnIfError();
        expr = ast_new_expr_unary(scope, expr, op_token, OP_UNARY_CAST, expr->lexical_range);
        expr->expr_unary.type_to_cast = type;
    }
    return expr;
}

Light_Ast* parse_expression_precedence5(Light_Parser* parser, Light_Scope* scope, u32* error) {
	Light_Token* op = 0;
	Light_Ast* expr = parse_expression_precedence6(parser, scope, error);
    ReturnIfError();
	while(true) {
		op = lexer_peek(parser->lexer);
		if(op->type == '*' || op->type == '/' || op->type == '%'){
			lexer_next(parser->lexer);
			Light_Ast* r = parse_expression_precedence6(parser, scope, error);
            ReturnIfError();
            Lexical_Range lrange = {0};
            lrange.start = expr->lexical_range.start;
            lrange.end = r->lexical_range.end;
			expr = ast_new_expr_binary(scope, expr, r, op, token_to_binary_op(op), lrange);
		} else {
			break;
		}
	}
	return expr;
}

Light_Ast* parse_expression_precedence4(Light_Parser* parser, Light_Scope* scope, u32* error) {
	Light_Token* op = 0;
	Light_Ast* expr = parse_expression_precedence5(parser, scope, error);
    ReturnIfError();
	while(true) {
		op = lexer_peek(parser->lexer);
		if(op->type == '+' || op->type == '-'){
			lexer_next(parser->lexer);
			Light_Ast* r = parse_expression_precedence5(parser, scope, error);
            ReturnIfError();
            Lexical_Range lrange = {0};
            lrange.start = expr->lexical_range.start;
            lrange.end = r->lexical_range.end;
			expr = ast_new_expr_binary(scope, expr, r, op, token_to_binary_op(op), lrange);
		} else {
			break;
		}
	}
	return expr;
}

Light_Ast* parse_expression_precedence3(Light_Parser* parser, Light_Scope* scope, u32* error) {
	Light_Token* op = 0;
	Light_Ast* expr = parse_expression_precedence4(parser, scope, error);
    ReturnIfError();
	while(true) {
		op = lexer_peek(parser->lexer);
		if(op->type == '^' || op->type == '|' || op->type == '&' || op->type == TOKEN_BITSHIFT_LEFT || op->type == TOKEN_BITSHIFT_RIGHT){
			lexer_next(parser->lexer);
			Light_Ast* r = parse_expression_precedence4(parser, scope, error);
            ReturnIfError();
            Lexical_Range lrange = {0};
            lrange.start = expr->lexical_range.start;
            lrange.end = r->lexical_range.end;
			expr = ast_new_expr_binary(scope, expr, r, op, token_to_binary_op(op), lrange);
		} else {
			break;
		}
	}
	return expr;
}

Light_Ast* parse_expression_precedence2(Light_Parser* parser, Light_Scope* scope, u32* error) {
	Light_Ast* expr = parse_expression_precedence3(parser, scope, error);
    ReturnIfError();
	Light_Token_Type next = lexer_peek(parser->lexer)->type;
	if (next == TOKEN_EQUAL_EQUAL || next == TOKEN_GREATER_EQUAL || next == TOKEN_NOT_EQUAL ||
		next == TOKEN_LESS_EQUAL || next == '<' || next == '>') 
	{
		Light_Token* op = lexer_next(parser->lexer);
		Light_Ast* right = parse_expression_precedence2(parser, scope, error);
        ReturnIfError();
        Lexical_Range lrange = {0};
        lrange.start = expr->lexical_range.start;
        lrange.end = right->lexical_range.end;
		return ast_new_expr_binary(scope, expr, right, op, token_to_binary_op(op), lrange);
	}
	return expr;
}

Light_Ast* parse_expression_precedence1(Light_Parser* parser, Light_Scope* scope, u32* error) {
	Light_Ast* expr = parse_expression_precedence2(parser, scope, error);
    ReturnIfError();
	Light_Token_Type next = lexer_peek(parser->lexer)->type;
	if (next == TOKEN_LOGIC_OR || next == TOKEN_LOGIC_AND) {
		Light_Token* op = lexer_next(parser->lexer);
		Light_Ast* right = parse_expression_precedence1(parser, scope, error);
        ReturnIfError();
        Lexical_Range lrange = {0};
        lrange.start = expr->lexical_range.start;
        lrange.end = right->lexical_range.end;
		return ast_new_expr_binary(scope, expr, right, op, token_to_binary_op(op), lrange);
	}
	return expr;
}

Light_Ast* 
parse_expression(Light_Parser* parser, Light_Scope* scope, u32* error) {
    return parse_expression_precedence1(parser, scope, error);
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

    Light_Token* token_array = lexer_peek_n(parser->lexer, -1);

    Light_Ast* dimension = parse_expression(parser, scope, error);
    ReturnIfError();

	*error |= parser_require_and_eat(parser, ']');
    ReturnIfError();

	Light_Type* array_type = parse_type(parser, scope, error);
    ReturnIfError();

	return type_new_array(dimension, array_type, token_array);
}
static Light_Type*
parse_type_procedure(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, '('); 
    ReturnIfError();

    bool all_args_internalized = true;
    u32 flags = 0;
    Light_Type** arguments_types = 0;
    if(lexer_peek(parser->lexer)->type != ')') {
        arguments_types = array_new(Light_Type*);
        for(s32 i = 0; ; ++i) {
            if(i != 0) {
                *error |= parser_require_and_eat(parser, ',');
                ReturnIfError();
            }
            // TODO(psv): refactor token to be ...
            if(lexer_peek(parser->lexer)->type == '.') {
                *error |= parser_require_and_eat(parser, '.');
                ReturnIfError();
                *error |= parser_require_and_eat(parser, '.');
                ReturnIfError();
                *error |= parser_require_and_eat(parser, '.');
                ReturnIfError();
                flags |= TYPE_FUNCTION_VARIADIC;
                break; // must be the last one
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

    if(lexer_peek(parser->lexer)->type == '#' && 
        lexer_peek_n(parser->lexer, 1)->data == (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_EXTERN].data && 
        lexer_peek_n(parser->lexer, 2)->type != '(') 
    {
        lexer_next(parser->lexer); // eat #
        lexer_next(parser->lexer); // eat extern
        flags |= TYPE_FUNCTION_STDCALL;
    }

    return type_new_function(arguments_types, return_type, args_count, all_args_internalized, flags);
}
static Light_Type*
parse_type_alias(Light_Parser* parser, Light_Scope* scope, u32* error) {
    Light_Token* name = lexer_next(parser->lexer);
    if(name->type != TOKEN_IDENTIFIER) {
        *error |= parser_error_fatal(parser, name, "expected identifier but got '%.*s'\n", TOKEN_STR(name));
        return 0;
    }

    return type_new_alias(scope, name, 0);
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

    Light_Ast** fields = array_new(Light_Ast*);
    Light_Scope*  enum_scope = light_scope_new(0, scope, SCOPE_ENUM);

    if(lexer_peek(parser->lexer)->type != '}') {
        for(s32 i = 0; ; ++i) {
            if(i != 0) {
                *error |= parser_require_and_eat(parser, ',');
                ReturnIfError();

                // Optional comma at the end
                if(lexer_peek(parser->lexer)->type == '}')
                    break;
            }
            Light_Token* name = lexer_next(parser->lexer);
            if(name->type != TOKEN_IDENTIFIER) {
                *error |= parser_error_fatal(parser, name, "expected identifier in enum field declaration, but got '%.*s'\n", TOKEN_STR(name));
                return 0;
            }

            Light_Ast* field_value = 0;
            if(lexer_peek(parser->lexer)->type == ':') {
                lexer_next(parser->lexer); // skip ':'
                *error |= parser_require_and_eat(parser, ':');
                ReturnIfError();

                field_value = parse_expression(parser, enum_scope, error);
                ReturnIfError();
            } else {
                field_value = 0;
            }

            enum_scope->decl_count++;

            Lexical_Range lrange = {0};
            lrange.start = name;
            lrange.end = (field_value) ? field_value->lexical_range.end : name;

            Light_Ast* field = ast_new_decl_constant(enum_scope, name, 0, field_value, 0, lrange);
            array_push(fields, field);

            if(lexer_peek(parser->lexer)->type != ',') break;
        }
    }

    *error |= parser_require_and_eat(parser, '}');
    ReturnIfError();


    s32 fields_count = array_length(fields);
    Light_Type* result = type_new_enum(fields, fields_count, type_hint, enum_scope);

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
    if(lexer_peek(parser->lexer)->type != '}') {
        for(;;) {
            Light_Ast* field = parse_decl_variable(parser, 0, 0, struct_scope, error, false);
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
    if(lexer_peek(parser->lexer)->type != '}') {
        for(;;) {
            Light_Ast* field = parse_decl_variable(parser, 0, 0, union_scope, error, false);
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
    }

    *error |= parser_require_and_eat(parser, '}');
    ReturnIfError();

    Light_Type* result = type_new_union(fields, field_count, union_scope);
    union_scope->creator_type = result;

    return result;
}

Light_Type*
parse_directive_typeof(Light_Parser* parser, Light_Scope* scope, u32* error) {
    *error |= parser_require_and_eat(parser, '#');
    ReturnIfError();
    
    Light_Token* first = lexer_peek_n(parser->lexer, -1);

    Light_Token* directive = lexer_next(parser->lexer);
    if(directive->type != TOKEN_IDENTIFIER || directive->data != (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_TYPEOF].data) {
        *error |= parser_error_fatal(parser, directive, "expected 'type_of' but got '%.*s'\n", TOKEN_STR(directive));
        return 0;
    }

    Light_Ast* expr = parse_expression(parser, scope, error);
    ReturnIfError();

    Lexical_Range lrange = {0};
    lrange.start = first;
    lrange.end = expr->lexical_range.end;

    Light_Ast* expr_directive = ast_new_expr_directive(scope, EXPR_DIRECTIVE_TYPEOF, directive, expr, 0, lrange);
    ReturnIfError();

    return type_new_directive(expr_directive);
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
        case '#':
            // a type can be returned from #type_of
            return parse_directive_typeof(parser, scope, error);
        default:
            *error |= parser_error_fatal(parser, t, "invalid token '%.*s' in type declaration\n", TOKEN_STR(t));
            break;
    }

    return 0;
}