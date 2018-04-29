#include "parser.h"
#include "util.h"
#include "type.h"
#include "type_table.h"
#include <stdarg.h>
#include <ho_system.h>

void Parser::report_error_location(Token* tok) {
	fprintf(stderr, "%.*s:%d:%d ", tok->filename.length, tok->filename.data, tok->line, tok->column);
}

void Parser::report_syntax_error(Token* error_token, char* msg, ...) {
	va_list args;
	va_start(args, msg);
	report_error_location(error_token);
	fprintf(stderr, "Syntax Error: ");
	vfprintf(stderr, msg, args);
	va_end(args);
	system_exit(-1);
}

Token* Parser::require_and_eat(Token_Type tt) {
	Token* t = lexer->eat_token();
	if(t->type == TOKEN_END_OF_STREAM && tt != TOKEN_END_OF_STREAM)
		report_syntax_error(t, "unexpected end of stream, expected '%s'\n", lexer->get_token_string(tt));
	if (t->type != tt) 
		report_syntax_error(t, "expected '%s', but got '%.*s'\n", lexer->get_token_string(tt), TOKEN_STR(t));
	return t;
}
Token* Parser::require_and_eat(char t) {
	return require_and_eat((Token_Type)t);
}

Ast** Parser::parse_top_level() {
	// Empty file check
	if (lexer->token_count == 1 && lexer->peek_token_type() == TOKEN_END_OF_STREAM)
		return 0;
	Parser_Error perr = PARSER_OK;

	Ast** ast_top_level = array_create(Ast*, 64);

	while (perr == PARSER_OK && lexer->peek_token_type() != TOKEN_END_OF_STREAM) {
		Ast* decl = parse_declaration(global_scope);
		switch (decl->node_type) {
			case AST_DECL_VARIABLE:
			case AST_DECL_CONSTANT:
				require_and_eat(';');
				break;
			default:
				break;
		}
		if (!decl) break;
		array_push(ast_top_level, &decl);
	}

	if (perr == PARSER_ERROR_FATAL) {
		fprintf(stderr, "There were errors, exiting...\n");
		return 0;
	}
	return ast_top_level;
}

// -------------------------------------------
// ------------- Declarations ----------------
// -------------------------------------------

Ast* Parser::parse_declaration(Scope* scope) {
	Token* name = lexer->eat_token();

	if (name->type != TOKEN_IDENTIFIER)
		report_syntax_error(name, "invalid identifier %.*s on declaration.\n", TOKEN_STR(name));

	Token* decl = lexer->eat_token();

	if (decl->type == ':')
	{
		Token* next = lexer->peek_token();
		if (next->type == ':') {
			// struct, procedure and constant declaration
			lexer->eat_token(); // :
			next = lexer->peek_token();
			if (next->type == '(') {
				return parse_decl_proc(name, scope);
			} else if (next->type == TOKEN_KEYWORD_STRUCT) {
				return parse_decl_struct(name, scope);
			} else if (next->type == TOKEN_KEYWORD_ENUM) {
				return parse_decl_enum(name, scope, 0);
			} else {
				return parse_decl_constant(name, scope, 0);
			}
		} else {
			// type for a variable, enum or constant declaration
			Type_Instance* declaration_type = 0;
			if (lexer->peek_token_type() != '=')
				declaration_type = parse_type();
			next = lexer->peek_token();
			if (next->type == ':') {
				// typed constant
				lexer->eat_token(); // ':'
				if (lexer->peek_token_type() == TOKEN_KEYWORD_ENUM)
					return parse_decl_enum(name, scope, declaration_type);
				else
					return parse_decl_constant(name, scope, declaration_type);
			} else {
				// variable declaration
				return parse_decl_variable(name, scope, declaration_type);
			}
		}
	} else {
		report_syntax_error(decl, "invalid declaration of '%.*s', declaration requires ':'\n", TOKEN_STR(name));
	}
	return 0;
}

Ast* Parser::parse_decl_proc(Token* name, Scope* scope) {
	require_and_eat('(');

	Ast**          arguments = 0;
	Scope*         arguments_scope = 0;
	Type_Instance* return_type = 0;
	s32 nargs = 0;

	if (lexer->peek_token_type() != ')') {
		arguments_scope = scope_create(0, scope, SCOPE_PROCEDURE_ARGUMENTS);
		arguments = array_create(Ast*, 4);

		for (;;) {
			if (nargs != 0)
				require_and_eat(',');
			Token* name = require_and_eat(TOKEN_IDENTIFIER);
			Ast* arg = parse_decl_variable(name, arguments_scope);
			array_push(arguments, &arg);
			++nargs;
			if (lexer->peek_token_type() != ',')
				break;
		}
	}
	require_and_eat(')');

	if (lexer->peek_token_type() == '{') {
		return_type = type_primitive_get(TYPE_PRIMITIVE_VOID);
	} else {
		require_and_eat(TOKEN_ARROW);
		return_type = parse_type();
	}

	Type_Instance* proc_type = type_new_temporary();
	proc_type->kind = KIND_FUNCTION;
	proc_type->function_desc.arguments_names = array_create(string, nargs);
	proc_type->function_desc.arguments_type  = array_create(Type_Instance*, nargs);
	proc_type->function_desc.num_arguments = nargs;
	proc_type->function_desc.return_type = return_type;
	for (s32 i = 0; i < nargs; ++i) {
		array_push(proc_type->function_desc.arguments_names, &arguments[i]->decl_variable.name->value);
		array_push(proc_type->function_desc.arguments_type, &arguments[i]->decl_variable.variable_type);
	}

	scope->decl_count += 1;
	Ast* body = 0;
	u32  flags = 0;
	Token* extern_library_name = 0;

	// TODO(psv): forward declared procs (foreign)
	if (lexer->peek_token_type() == '#') {
		lexer->eat_token();
		Token* tag = lexer->eat_token();
		if (compiler_tags[COMPILER_TAG_FOREIGN].data == tag->value.data) {
			require_and_eat('(');
			Ast* libname = parse_expr_literal(scope);
			if (libname->expr_literal.flags & LITERAL_FLAG_STRING) {
				extern_library_name = libname->expr_literal.token;
			} else {
				report_syntax_error(tag, "foreign compiler tag requires string literal as library path\n");
			}
			require_and_eat(')');
		} else {
			report_syntax_error(tag, "expected compiler tag but got '%.*s'\n", TOKEN_STR(tag));
		}
		require_and_eat(';');
		flags |= DECL_PROC_FLAG_FOREIGN;
	} else {
		if (arguments_scope)
			body = parse_comm_block(arguments_scope, 0);
		else
			body = parse_comm_block(scope, 0);
		if(body->comm_block.block_scope){
			body->comm_block.block_scope->flags |= SCOPE_PROCEDURE_BODY;
		}
	}

	Ast* node = ast_create_decl_proc(name, scope, arguments_scope, proc_type, arguments, body, return_type, flags, nargs);
	node->decl_procedure.extern_library_name = extern_library_name;
	if (body) {
		body->comm_block.creator = node;
	}

	if(arguments_scope)
		arguments_scope->creator_node = node;
	return node;
}

Ast* Parser::parse_decl_variable(Token* name, Scope* scope, Type_Instance* type) {
	Ast* assignment = 0;

	Token* next = lexer->peek_token();
	if (next->type == '=') {
		next = lexer->eat_token();
		assignment = parse_expression(scope);
	}
	scope->decl_count += 1;
	return ast_create_decl_variable(name, scope, assignment, type, 0);
}

Ast* Parser::parse_decl_variable(Token* name, Scope* scope) {
	require_and_eat(':');

	Type_Instance* var_type = 0;

	Token* next = lexer->peek_token();
	if (next->type != '=')
		var_type = parse_type();

	scope->decl_count += 1;
	return parse_decl_variable(name, scope, var_type);
}

Ast* Parser::parse_decl_struct(Token* name, Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_STRUCT);

	s32    fields_count = 0;
	Ast**  fields = array_create(Ast*, 8);
	Scope* scope_struct = scope_create(0, scope, SCOPE_STRUCTURE);

	require_and_eat('{');

	for (;;) {
		Token* field_name = lexer->eat_token();
		if (field_name->type != TOKEN_IDENTIFIER) {
			report_syntax_error(field_name, "expected struct field declaration, but got '%.*s'\n", TOKEN_STR(field_name));
		}
		Ast* field = parse_decl_variable(field_name, scope_struct);
		require_and_eat(';');
		array_push(fields, &field);
		++fields_count;
		if (lexer->peek_token_type() == '}')
			break;
	}

	require_and_eat('}');

	Type_Instance* struct_type = type_new_temporary();
	struct_type->kind = KIND_STRUCT;
	struct_type->struct_desc.fields_names = array_create(string, fields_count);
	struct_type->struct_desc.fields_types = array_create(Type_Instance*, fields_count);
	struct_type->struct_desc.name = name;
	array_allocate(struct_type->struct_desc.fields_names, fields_count);
	array_allocate(struct_type->struct_desc.fields_types, fields_count);
	for (s32 i = 0; i < fields_count; ++i) {
		struct_type->struct_desc.fields_names[i] = fields[i]->decl_variable.name->value;
		struct_type->struct_desc.fields_types[i] = fields[i]->decl_variable.variable_type;
	}

	scope->decl_count += 1;

	Ast* node = ast_create_decl_struct(name, scope, scope_struct, struct_type, fields, 0, fields_count);
	scope_struct->creator_node = node;
	return node;
}

Ast* Parser::parse_decl_enum(Token* name, Scope* scope, Type_Instance* hint_type) {

	require_and_eat(':');
	require_and_eat(TOKEN_KEYWORD_ENUM);
	require_and_eat('{');

	if (!hint_type) {
		// TODO(psv): decide default value
		hint_type = type_primitive_get(TYPE_PRIMITIVE_U32);
	}

	s32    fields_count = 0;
	Ast**  fields = 0;
	Scope* enum_scope = scope_create(0, scope, SCOPE_ENUM);

	for (;;) {
		if (fields_count != 0)
			require_and_eat(',');
		Token* const_name = lexer->eat_token();
		if (const_name->type != TOKEN_IDENTIFIER)
			report_syntax_error(const_name, "expected enum field constant declaration, but got '%.*s'\n", TOKEN_STR(const_name));
		Ast* field = parse_decl_constant(const_name, enum_scope, hint_type);
		array_push(fields, &field);
		++fields_count;
		if (lexer->peek_token_type() != ',')
			break;
	}

	require_and_eat('}');

	scope->decl_count += 1;

	Ast* node = ast_create_decl_enum(name, scope, enum_scope, fields, hint_type, 0, fields_count);
	enum_scope->creator_node = node;
	return node;
}

Ast* Parser::parse_decl_constant(Token* name, Scope* scope, Type_Instance* type) {
	// either literal or another constant
	Ast* value = 0;

	Token* next = lexer->peek_token();
	if (next->type == TOKEN_IDENTIFIER) {
		value = ast_create_expr_variable(name, scope, type);
	} else {
		value = parse_expr_literal(scope);
	}

	scope->decl_count += 1;

	return ast_create_decl_constant(name, scope, value, type, 0);
}

// -------------------------------------------
// ------------- Expressions -----------------
// -------------------------------------------

Ast* Parser::parse_expr_proc_call(Scope* scope) {
	Token* name = lexer->eat_token();
	Ast**  arguments = 0;
	s32    args_count = 0;
	
	require_and_eat('(');
	if (lexer->peek_token_type() != ')') {
		arguments = array_create(Ast*, 4);
		for (;;) {
			if (args_count != 0)
				require_and_eat(',');
			Ast* argument = parse_expression(scope);
			array_push(arguments, &argument);
			++args_count;
			if (lexer->peek_token_type() != ',')
				break;
		}
	}
	require_and_eat(')');

	return ast_create_expr_proc_call(scope, name, arguments, args_count);
}

Ast* Parser::parse_expression_precedence10(Scope* scope) {
	Token* t = lexer->peek_token();
	if (t->flags & TOKEN_FLAG_LITERAL) {
		return parse_expr_literal(scope);
	} else if (t->type == TOKEN_IDENTIFIER) {
		if (lexer->peek_token_type(1) == '(') {
			return parse_expr_proc_call(scope);
		} else {
			lexer->eat_token();
			return ast_create_expr_variable(t, scope, 0);
		}
	} else if(t->type == '(') {
		lexer->eat_token();
		Ast* expr = parse_expression(scope);
		require_and_eat(')');
		return expr;
	}
	return 0;
}

Ast* Parser::parse_expression_precedence9(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_precedence10(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '['){
			lexer->eat_token();
			Ast* r = parse_expression(scope);
			expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
			require_and_eat(']');
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence8(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_precedence9(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '.'){
			lexer->eat_token();
			Ast* r = parse_expression_precedence9(scope);
			expr = ast_create_expr_binary(scope, expr, r, OP_BINARY_DOT, op);
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence7(Scope* scope) {
	Token_Type next = lexer->peek_token_type();
	if (next == TOKEN_KEYWORD_CAST) {
		// parse cast
		Token* cast = lexer->eat_token();
		require_and_eat('(');
		Type_Instance* ttc = parse_type();
		require_and_eat(')');
		Ast* operand = parse_expression_precedence7(scope);
		return ast_create_expr_unary(scope, operand, OP_UNARY_CAST, cast, ttc, UNARY_EXPR_FLAG_PREFIXED);
	}
	return parse_expression_precedence8(scope);
}

Ast* Parser::parse_expression_precedence6(Scope* scope) {
	Token_Type next = lexer->peek_token_type();
	if (next == '~' || next == '!' || next == '&' || next == '*' || next == '-' || next == '+') {
		Token* op = lexer->eat_token();
		Ast* operand = parse_expression_precedence6(scope);
		return ast_create_expr_unary(scope, operand, token_to_unary_op(op), op, 0, UNARY_EXPR_FLAG_PREFIXED);
	}
	return parse_expression_precedence7(scope);
}

Ast* Parser::parse_expression_precedence5(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_precedence6(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '*' || op->type == '/' || op->type == '%'){
			lexer->eat_token();
			Ast* r = parse_expression_precedence6(scope);
			expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence4(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_precedence5(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '+' || op->type == '-'){
			lexer->eat_token();
			Ast* r = parse_expression_precedence5(scope);
			expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence3(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_precedence4(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '^' || op->type == '|' || op->type == '&' || op->type == TOKEN_BITSHIFT_LEFT || op->type == TOKEN_BITSHIFT_RIGHT){
			lexer->eat_token();
			Ast* r = parse_expression_precedence4(scope);
			expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence2(Scope* scope) {
	Ast* expr = parse_expression_precedence3(scope);
	Token_Type next = lexer->peek_token_type();
	if (next == TOKEN_EQUAL_COMPARISON || next == TOKEN_GREATER_EQUAL || next == TOKEN_NOT_EQUAL ||
		next == TOKEN_LESS_EQUAL || next == '<' || next == '>') 
	{
		Token* op = lexer->eat_token();
		Ast* right = parse_expression_precedence2(scope);
		return ast_create_expr_binary(scope, expr, right, token_to_binary_op(op), op);
	}
	return expr;
}

Ast* Parser::parse_expression_precedence1(Scope* scope) {
	Ast* expr = parse_expression_precedence2(scope);
	Token_Type next = lexer->peek_token_type();
	if (next == TOKEN_LOGIC_OR || next == TOKEN_LOGIC_AND) {
		Token* op = lexer->eat_token();
		Ast* right = parse_expression_precedence1(scope);
		return ast_create_expr_binary(scope, expr, right, token_to_binary_op(op), op);
	}
	return expr;
}

Ast* Parser::parse_expression(Scope* scope) {
	return parse_expression_precedence1(scope);
}

Ast* Parser::parse_expr_literal(Scope* scope) {
	Token* first = lexer->eat_token();
	Ast* node = ast_create_expr_literal(scope, LITERAL_UNKNOWN, first, 0, 0);
	bool negate = false;
	bool prefixed = false;
	switch (first->type) {
		case '-':
			negate = true;
		case '+':
			first = lexer->eat_token();
			prefixed = true;
		break;
		default: break;
	}
	if (prefixed && first->type == TOKEN_LITERAL_INT) {
		node->expr_literal.type = LITERAL_SINT;
		node->expr_literal.value_s64 = str_to_s64((char*)first->value.data, first->value.length);
		if (negate)
			node->expr_literal.value_s64 = -node->expr_literal.value_s64;
	} else if (prefixed) {
		report_syntax_error(first, "expected integer literal after %c operator but got '%.*s'\n", (char)first->type, TOKEN_STR(first));
	} else {
		switch (first->type) {
		case TOKEN_LITERAL_INT: {
			node->expr_literal.type = LITERAL_SINT;
			node->expr_literal.value_s64 = str_to_s64((char*)first->value.data, first->value.length);
		} break;
		case TOKEN_LITERAL_HEX_INT: {
			node->expr_literal.type = LITERAL_HEX_INT;
			node->expr_literal.value_u64 = literal_integer_to_u64(first);
		} break;
		case TOKEN_LITERAL_BIN_INT: {
			node->expr_literal.type = LITERAL_BIN_INT;
			node->expr_literal.value_u64 = literal_integer_to_u64(first);
		} break;
		case TOKEN_LITERAL_BOOL_FALSE: {
			node->expr_literal.type = LITERAL_BOOL;
			node->expr_literal.value_bool = false;
		} break;
		case TOKEN_LITERAL_BOOL_TRUE: {
			node->expr_literal.type = LITERAL_BOOL;
			node->expr_literal.value_bool = true;
		} break;
		case TOKEN_LITERAL_CHAR:
			// TODO(psv): utf8 encoding
			node->expr_literal.type = LITERAL_UINT;
			node->type_return = type_primitive_get(TYPE_PRIMITIVE_U32);
			break;
		case TOKEN_LITERAL_FLOAT:
			node->expr_literal.type = LITERAL_FLOAT;
			node->expr_literal.value_r64 = literal_float_to_r64(first);
			break;
		case TOKEN_LITERAL_STRING:
			node->expr_literal.type = LITERAL_STRUCT;
			node->expr_literal.flags |= LITERAL_FLAG_STRING;
			// TODO(psv): get type string here already
			break;
		default: {
			// TODO(psv):
			// struct literal
			// array literal
		}break;
		}
	}

	return node;
}

// -------------------------------------------
// --------------- Commands ------------------
// -------------------------------------------

Ast* Parser::parse_command(Scope* scope) {
	Token* next = lexer->peek_token();
	Ast* command = 0;
	switch (next->type) {
		case '{':
			command = parse_comm_block(scope, 0);
			break;
		case TOKEN_KEYWORD_IF:
			command = parse_comm_if(scope); 
			break;
		case TOKEN_KEYWORD_FOR:
			command = parse_comm_for(scope);
			break;
		case TOKEN_KEYWORD_BREAK:
			command = parse_comm_break(scope);
			require_and_eat(';');
			break;
		case TOKEN_KEYWORD_CONTINUE:
			command = parse_comm_continue(scope);
			require_and_eat(';');
			break;
		case TOKEN_KEYWORD_RETURN:
			command = parse_comm_return(scope);
			require_and_eat(';');
			break;
		case TOKEN_IDENTIFIER:{
			Token_Type t = lexer->peek_token_type(1);
			if (t == ':') {
				command = parse_declaration(scope);
			} else if (t == '(') {
				// syntatic sugar void proc call
				Ast* pcall = parse_expr_proc_call(scope);
				command = ast_create_comm_variable_assignment(scope, 0, pcall);
			} else {
				command = parse_comm_variable_assignment(scope);
			}
			require_and_eat(';');
		}break;
		default: {
			command = parse_comm_variable_assignment(scope);
			require_and_eat(';');
		}break;
	}
	return command;
}

Ast* Parser::parse_comm_variable_assignment(Scope* scope) {
	Ast* lvalue = parse_expression(scope);

	Token* op = lexer->eat_token();
	if (!(op->flags & TOKEN_FLAG_ASSIGNMENT_OPERATOR))
		report_syntax_error(op, "expected assignment operator on variable assignment, but got '%.*s'\n", TOKEN_STR(op));

	Ast* rvalue = parse_expression(scope);
	Operator_Binary binop = OP_BINARY_UNKNOWN;

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
			report_internal_compiler_error(__FILE__, __LINE__, "Token '%.*s' flag assignment operator expected to be set but apparently it is not.\n", TOKEN_STR(op));
		}break;
	}
	if(binop != OP_BINARY_UNKNOWN)
		rvalue = ast_create_expr_binary(scope, lvalue, rvalue, binop, op);

	return ast_create_comm_variable_assignment(scope, lvalue, rvalue);
}

Ast* Parser::parse_comm_return(Scope* scope) {
	Token* t = require_and_eat(TOKEN_KEYWORD_RETURN);
	Ast* expr = 0;
	if (lexer->peek_token_type() != ';')
		expr = parse_expression(scope);
	return ast_create_comm_return(scope, expr, t);
}

Ast* Parser::parse_comm_continue(Scope* scope) {
	Token* t = require_and_eat(TOKEN_KEYWORD_CONTINUE);
	return ast_create_comm_continue(scope, t);
}

Ast* Parser::parse_comm_break(Scope* scope) {
	Token* t = require_and_eat(TOKEN_KEYWORD_BREAK);
	Ast* lit = 0;
	if(lexer->peek_token_type() != ';')
		lit = parse_expr_literal(scope);
	return ast_create_comm_break(scope, lit, t);
}

Ast* Parser::parse_comm_for(Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_FOR);
	
	Ast* condition = parse_expression(scope);

	Ast* body = parse_command(scope);
	if(body->node_type == AST_COMMAND_BLOCK) {
		if(body->comm_block.block_scope)
			body->comm_block.block_scope->flags |= SCOPE_LOOP;
		return ast_create_comm_for(scope, condition, body);
	} else {
		Ast** commands = array_create(Ast*, 1);
		array_push(commands, &body);
		Ast* inner = ast_create_comm_block(scope, scope_create(0, scope, SCOPE_LOOP), commands, 0, 1);
		
		Ast* for_cmd = ast_create_comm_for(scope, condition, inner);
		inner->comm_block.creator = for_cmd;

		inner->comm_block.block_scope->creator_node = for_cmd;
		inner->comm_block.block_scope->decl_count = 1;
		return inner;
	}
}

Ast* Parser::parse_comm_if(Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_IF);
	Ast* condition = parse_expression(scope);
	Ast* command_true = parse_command(scope);
	Ast* command_false = 0;
	if (lexer->peek_token_type() == TOKEN_KEYWORD_ELSE) {
		lexer->eat_token();
		command_false = parse_command(scope);
	}
	return ast_create_comm_if(scope, condition, command_true, command_false);
}

Ast* Parser::parse_comm_block(Scope* scope, Ast* creator) {
	require_and_eat('{');
	
	if (lexer->peek_token_type() == '}') {
		lexer->eat_token();
		return ast_create_comm_block(scope, 0, 0, creator, 0);
	}

	Ast** commands = array_create(Ast*, 4);
	Scope* block_scope = scope_create(0, scope, SCOPE_BLOCK);

	s32 command_count = 0;
	for (;;) {
		Ast* command = parse_command(block_scope);
		array_push(commands, &command);
		++command_count;
		if (lexer->peek_token_type() == '}')
			break;
	}

	require_and_eat('}');

	Ast* node = ast_create_comm_block(scope, block_scope, commands, creator, command_count);
	node->comm_block.block_scope->creator_node = node;
	return node;
}


// -------------------------------------------
// ---------------- Types --------------------
// -------------------------------------------
// Parse Type, doest not internalize types yet

Type_Instance* Parser::parse_type() {
	Token* tok = lexer->eat_token();
	switch (tok->type) {
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
			return parse_type_primitive();

		case '^':
			return parse_type_pointer();

		case TOKEN_IDENTIFIER:
			return parse_type_struct(tok);

		case '[':
			return parse_type_array();

		case '(':
			return parse_type_function();

		default:
			report_syntax_error(tok, "expected type declaration, but got '%.*s'\n", TOKEN_STR(tok));
		break;
	}
	return 0;
}

Type_Instance* Parser::parse_type_primitive() {
	switch (lexer->last_token()->type) {
		case TOKEN_SINT64:	return type_primitive_get(TYPE_PRIMITIVE_S64);
		case TOKEN_SINT32:  return type_primitive_get(TYPE_PRIMITIVE_S32);
		case TOKEN_SINT16:  return type_primitive_get(TYPE_PRIMITIVE_S16);
		case TOKEN_SINT8:   return type_primitive_get(TYPE_PRIMITIVE_S8);
		case TOKEN_UINT64:  return type_primitive_get(TYPE_PRIMITIVE_U64);
		case TOKEN_UINT32:  return type_primitive_get(TYPE_PRIMITIVE_U32);
		case TOKEN_UINT16:  return type_primitive_get(TYPE_PRIMITIVE_U16);
		case TOKEN_UINT8:   return type_primitive_get(TYPE_PRIMITIVE_U8);
		case TOKEN_REAL32:  return type_primitive_get(TYPE_PRIMITIVE_R32);
		case TOKEN_REAL64:  return type_primitive_get(TYPE_PRIMITIVE_R64);
		case TOKEN_BOOL:    return type_primitive_get(TYPE_PRIMITIVE_BOOL);
		case TOKEN_VOID:    return type_primitive_get(TYPE_PRIMITIVE_VOID);
		default: report_internal_compiler_error(__FILE__, __LINE__, "tried to parse unknown primitive type\n"); break;
	}
}
Type_Instance* Parser::parse_type_pointer() {
	Type_Instance* t = type_new_temporary();
	t->kind  = KIND_POINTER;
	t->flags = TYPE_FLAG_SIZE_RESOLVED;
	t->type_size_bits = type_pointer_size();
	t->pointer_to = parse_type();
	return t;
}
Type_Instance* Parser::parse_type_struct(Token* name) {
	Type_Instance* t = type_new_temporary();
	t->kind = KIND_STRUCT;
	t->flags = 0;
	t->type_size_bits = 0;
	t->struct_desc.fields_names = 0;
	t->struct_desc.fields_types = 0;
	t->struct_desc.name = name;
	return t;
}
Type_Instance* Parser::parse_type_array() {
	Token* dimension = lexer->eat_token();
	Type_Instance* t = type_new_temporary();
	t->kind = KIND_ARRAY;
	t->flags = 0;
	t->type_size_bits = 0;
	t->array_desc.dimension = 0;

	if (dimension->flags & TOKEN_FLAG_NUMERIC_LITERAL) {
		// numeric literal
		t->array_desc.dimension = literal_integer_to_u64(dimension);
		t->array_desc.dimension_evaluated = true;
	} else if (dimension->type == TOKEN_IDENTIFIER) {
		// TODO: constant or enum constant
		t->array_desc.constant_dimension_name = require_and_eat(TOKEN_IDENTIFIER);
		t->array_desc.dimension_evaluated = false;
	}
	require_and_eat(']');
	t->array_desc.array_of = parse_type();
	return t;
}
Type_Instance* Parser::parse_type_function() {
	Type_Instance* t = type_new_temporary();
	t->kind = KIND_FUNCTION;
	t->flags = 0;
	t->type_size_bits = 0;
	if (lexer->peek_token_type() == ')') {
		t->function_desc.arguments_names = 0;
		t->function_desc.arguments_type = 0;
		t->function_desc.num_arguments = 0;
		require_and_eat(')');
	} else {
		t->function_desc.arguments_type = array_create(Type_Instance*, 4);
		t->function_desc.num_arguments = 0;
		t->function_desc.arguments_names = 0;
		for(s32 i = 0; ; ++i) {
			if (i != 0)
				require_and_eat(',');
			Type_Instance* argtype = parse_type();
			array_push(t->function_desc.arguments_type, &argtype);
			t->function_desc.num_arguments += 1;
			if (lexer->peek_token_type() != ',')
				break;
		}
		require_and_eat(')');
		require_and_eat(TOKEN_ARROW);
		t->function_desc.return_type = parse_type();
	}
	return t;
}

// Expressions Precedence

Precedence Parser::unary_op_precedence_level(Operator_Unary unop, bool prefixed)
{
	if (prefixed) {
		if (unop == OP_UNARY_PLUS)			return PRECEDENCE_7;
		if (unop == OP_UNARY_DEREFERENCE)	return PRECEDENCE_7;	// dereference
		if (unop == OP_UNARY_ADDRESSOF)		return PRECEDENCE_7;	// address of
		if (unop == OP_UNARY_BITWISE_NOT)	return PRECEDENCE_7;	// not
		if (unop == OP_UNARY_MINUS)			return PRECEDENCE_7;	// minus
		if (unop == OP_UNARY_LOGIC_NOT)		return PRECEDENCE_7;	// not logic
		if (unop == OP_UNARY_CAST)			return PRECEDENCE_7;	// cast
	} else {
		return PRECEDENCE_7;
	}
	assert(0);
	return PRECEDENCE_0;
}

Precedence Parser::binary_op_precedence_level(Operator_Binary bo)
{
	Precedence p = PRECEDENCE_0;
	switch (bo) {
	case OP_BINARY_PLUS:			p = PRECEDENCE_4; break;
	case OP_BINARY_MINUS:			p = PRECEDENCE_4; break;
	case OP_BINARY_MULT:			p = PRECEDENCE_5; break;
	case OP_BINARY_DIV:				p = PRECEDENCE_5; break;
	case OP_BINARY_AND:				p = PRECEDENCE_3; break;
	case OP_BINARY_OR:				p = PRECEDENCE_3; break;
	case OP_BINARY_XOR:				p = PRECEDENCE_3; break;
	case OP_BINARY_MOD:				p = PRECEDENCE_5; break;
	case OP_BINARY_LOGIC_AND:		p = PRECEDENCE_1; break;
	case OP_BINARY_LOGIC_OR:		p = PRECEDENCE_1; break;
	case OP_BINARY_SHL:				p = PRECEDENCE_3; break;
	case OP_BINARY_SHR:				p = PRECEDENCE_3; break;
	case OP_BINARY_LT:				p = PRECEDENCE_2; break;
	case OP_BINARY_GT:				p = PRECEDENCE_2; break;
	case OP_BINARY_LE:				p = PRECEDENCE_2; break;
	case OP_BINARY_GE:				p = PRECEDENCE_2; break;
	case OP_BINARY_EQUAL:			p = PRECEDENCE_2; break;
	case OP_BINARY_NOT_EQUAL:		p = PRECEDENCE_2; break;
	case OP_BINARY_DOT:				p = PRECEDENCE_8; break;
	default: assert(0); break;
	}
	return p;
}