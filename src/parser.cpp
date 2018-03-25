#include "parser.h"
#include "ast.h"
#include "util.h"
#include "type.h"
#include <ho_system.h>

void Parser::report_error_location(Token* tok) {
	fprintf(stderr, "%.*s (%d:%d) ", tok->filename.length, tok->filename.data, tok->line, tok->column);
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
	scope->decl_count += 1;
	// TODO(psv): forward declared procs (foreign)
	Ast* body = parse_comm_block(scope);

	Ast* node = ast_create_decl_proc(name, scope, arguments_scope, arguments, body, return_type, 0, nargs);
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
		if (fields_count != 0)
			require_and_eat(';');
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

	scope->decl_count += 1;

	Ast* node = ast_create_decl_struct(name, scope, scope_struct, fields, 0, fields_count);
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
	require_and_eat(':');
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
	if (lexer->peek_token_type() == ')') {
		lexer->eat_token();
	} else {
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

Ast* Parser::parse_expression(Scope* scope, Precedence caller_prec, bool quit_on_precedence)
{
	Ast* left_op = 0;
	Token* first = lexer->peek_token();

	if (first->flags & TOKEN_FLAG_LITERAL) {
		left_op = parse_expr_literal(scope);
	}
	else if (first->type == TOKEN_IDENTIFIER) {
		if (lexer->peek_token_type(1) == '(') {
			// proc call
			left_op = parse_expr_proc_call(scope);
		} else {
			first = lexer->eat_token();
			left_op = ast_create_expr_variable(first, scope, 0);
		}
	} else if (first->type == (Token_Type)'(') {
		lexer->eat_token();
		left_op = parse_expression(scope, PRECEDENCE_0);
		require_and_eat((Token_Type)')');
	} else if (first->flags & TOKEN_FLAG_UNARY_OPERATOR) {
		Token* tok = lexer->eat_token();
		Operator_Unary uop = token_to_unary_op(tok);
		Precedence unop_precedence = unary_op_precedence_level(uop, true);
		Type_Instance* cast_type = 0;
		if (uop == OP_UNARY_CAST) {
			require_and_eat((Token_Type)'(');
			cast_type = parse_type();
			require_and_eat((Token_Type)')');
		}
		Ast* operand = parse_expression(scope, unop_precedence, true);
		left_op = ast_create_expr_unary(scope, operand, uop, UNARY_EXPR_FLAG_PREFIXED);
	}
	else {
		if (first->type == TOKEN_END_OF_STREAM) {
			report_syntax_error(0, "unexpected end of stream at expression.\n", TOKEN_STR(first));
		}
		report_syntax_error(0, "expected expression but got '%.*s'.\n", TOKEN_STR(first));
	}

	Token* optor = lexer->eat_token();
	if (optor->flags & TOKEN_FLAG_BINARY_OPERATOR) {
		if (optor->type == '.' && lexer->peek_token_type() != TOKEN_IDENTIFIER) {
			Token* t = lexer->peek_token();
			report_syntax_error(0, "expected member variable, got '%.*s'.\n", TOKEN_STR(t));
		}
		Precedence bop_precedence = binary_op_precedence_level(token_to_binary_op(optor));
		if (bop_precedence < caller_prec && quit_on_precedence) {
			lexer->rewind();
			return left_op;
		}
		Ast* right_op = parse_expression(scope, bop_precedence, quit_on_precedence);
		Ast* binop = ast_create_expr_binary(scope, left_op, right_op, token_to_binary_op(optor));
		if (right_op->node_type == AST_EXPRESSION_BINARY) {
			if (optor->type == '.') {
				if (binary_op_precedence_level(token_to_binary_op(optor)) > binary_op_precedence_level(right_op->expr_binary.op)) {
					binop->expr_binary.right = right_op->expr_binary.left;
					right_op->expr_binary.left = binop;
					return right_op;
				}
			} else {
				if (binary_op_precedence_level(token_to_binary_op(optor)) >= binary_op_precedence_level(right_op->expr_binary.op)) {
					binop->expr_binary.right = right_op->expr_binary.left;
					right_op->expr_binary.left = binop;
					return right_op;
				}
			}
		}
		return binop;
	} else {
		lexer->rewind();
		return left_op;
	}
}

Ast* Parser::parse_expr_literal(Scope* scope) {
	Token* first = lexer->eat_token();
	Ast* node = ast_create_expr_literal(scope, LITERAL_UNKNOWN, 0, 0);
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
	}
	switch(first->type) {
		case TOKEN_LITERAL_INT: {
			node->expr_literal.type = LITERAL_SINT;
			node->expr_literal.value_s64 = str_to_s64((char*)first->value.data, first->value.length);
		} break;
		case TOKEN_LITERAL_HEX_INT: {
			node->expr_literal.type = LITERAL_HEX_INT;
			node->expr_literal.value_u64 = lexer->literal_integer_to_u64(first);
		} break;
		case TOKEN_LITERAL_BIN_INT: {
			node->expr_literal.type = LITERAL_BIN_INT;
			node->expr_literal.value_u64 = lexer->literal_integer_to_u64(first);
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
		case TOKEN_LITERAL_STRING:
			node->expr_literal.flags |= LITERAL_FLAG_STRING;
			// TODO(psv): get type string here already
		default: {
			// TODO(psv):
			// struct literal
			// array literal
		}break;
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
			command = parse_comm_block(scope);
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
		default: {
			if (lexer->peek_token_type(1) == ':') {
				command = parse_declaration(scope);
			} else {
				command = parse_comm_variable_assignment(scope);
			}
			require_and_eat(';');
		}
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
		rvalue = ast_create_expr_binary(scope, lvalue, rvalue, binop);

	return ast_create_comm_variable_assignment(scope, lvalue, rvalue);
}

Ast* Parser::parse_comm_return(Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_RETURN);
	Ast* expr = 0;
	if (lexer->peek_token_type() != ';')
		expr = parse_expression(scope);
	return ast_create_comm_return(scope, expr);
}

Ast* Parser::parse_comm_continue(Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_CONTINUE);
	return ast_create_comm_continue(scope);
}

Ast* Parser::parse_comm_break(Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_BREAK);
	Ast* lit = 0;
	if(lexer->peek_token_type() != ';')
		lit = parse_expr_literal(scope);
	return ast_create_comm_break(scope, lit);
}

Ast* Parser::parse_comm_for(Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_FOR);
	
	Ast* condition = parse_expression(scope);

	Ast* body = parse_command(scope);
	if (body->scope)
		body->scope->flags |= SCOPE_LOOP;

	return ast_create_comm_for(scope, condition, body);
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

Ast* Parser::parse_comm_block(Scope* scope) {
	require_and_eat('{');
	
	if (lexer->peek_token_type() == '}') {
		lexer->eat_token();
		return ast_create_comm_block(scope, 0, 0, 0);
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

	Ast* node = ast_create_comm_block(scope, block_scope, commands, command_count);
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
	t->flags = TYPE_INSTANCE_SIZE_RESOLVED;
	t->type_size_bits = type_pointer_size();
	t->pointer_to = parse_type();
	return t;
}
Type_Instance* Parser::parse_type_struct(Token* name) {
	Type_Instance* t = type_new_temporary();
	t->kind = KIND_STRUCT;
	t->flags = 0;
	t->type_size_bits = 0;

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
		t->array_desc.dimension = lexer->literal_integer_to_u64(dimension);
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
			t->function_desc.arguments_type[i] = parse_type();
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

#if 0
#include <stdarg.h>
#include "symbol_table.h"

Parser::Parser(Lexer* lexer) 
	: arena(65536), lexer(lexer)
{
	filename = lexer->filename;
}

Ast** Parser::parse_top_level(Scope* global_scope, Ast** top_level)
{
	// Empty file check
	if (lexer->token_count == 1 && lexer->peek_token_type() == TOKEN_END_OF_STREAM) {
		return 0;
	}
	
	parser_error = PARSER_OK;

	while (parser_error == PARSER_OK && lexer->peek_token_type() != TOKEN_END_OF_STREAM) {
		Ast* decl = parse_declaration(global_scope);
		if (!decl) break;
		array_push(top_level, &decl);
	}

	if (parser_error == PARSER_ERROR_FATAL) {
		fprintf(stderr, "There were errors, exiting...\n");
		return 0;
	}
	return top_level;
}

void print_error_loc(FILE* out, string filename, int line, int column) {
	fprintf(out, "%.*s (%d:%d) ", filename.length, filename.data, line, column);
}

int Parser::report_syntax_error(Token* error_token, char* msg, ...)
{
	if(error_token)
		print_error_loc(stderr, error_token->filename, error_token->line, error_token->column);
	va_list args;
	va_start(args, msg);
	fprintf(stderr, "Syntax Error: ");
	vfprintf(stderr, msg, args);
	va_end(args);
	parser_error = PARSER_ERROR_FATAL;
	return system_exit(-1);
}

Token* Parser::require_and_eat(char c) {
	return require_and_eat((Token_Type)c);
}

Token* Parser::require_and_eat(Token_Type t)
{
	Token* tok = lexer->eat_token();
	if (tok->type != t) {
		print_error_loc(stderr, filename, tok->line, tok->column);

		fprintf(stderr, "Syntax error: expected '%s' got '%.*s'.\n", lexer->get_token_string(t), TOKEN_STR(tok));
		parser_error = PARSER_ERROR_FATAL;
		system_exit(-1);
	}
	return tok;
}

int Parser::require_token_type(Token* tok, Token_Type type)
{
	if (tok->type != type) {
		print_error_loc(stderr, filename, tok->line, tok->column);
		fprintf(stderr, "Syntax error: expected :: got '%.*s'.\n", TOKEN_STR(tok));
		parser_error = PARSER_ERROR_FATAL;
		return -1;
	}
	return 0;
}

Ast* Parser::parse_literal()
{
	Token* lit_token = lexer->eat_token();
	Ast* literal_ast = 0;

	u32 flags = 0;
	if (lit_token->type == TOKEN_INT_LITERAL) {
		flags |= LITERAL_FLAG_IS_REGSIZE | LITERAL_FLAG_NUMERIC;
	} else if (lit_token->type == TOKEN_FLOAT_LITERAL) {
		flags |= LITERAL_FLAG_IS_REGSIZE | LITERAL_FLAG_NUMERIC;
	} else if (lit_token->type == TOKEN_CHAR_LITERAL) {
		flags |= LITERAL_FLAG_IS_REGSIZE | LITERAL_FLAG_NUMERIC;
	} else if (lit_token->type == TOKEN_BOOL_LITERAL) {
		flags |= LITERAL_FLAG_IS_REGSIZE | LITERAL_FLAG_NUMERIC;
	} else if (lit_token->type == TOKEN_STRING_LITERAL) {
		flags |= LITERAL_FLAG_STRING;
	} else {
		assert(0);
	}

	literal_ast = create_literal(&arena, flags, lit_token);
	return literal_ast;
}

Precedence Parser::get_precedence_level(Token_Type type, bool postfixed, bool unary)
{
	if (unary) {
		if (postfixed) {
			// Note(psv): include postfixed operators here
		} else {
			if (type == '*') return PRECEDENCE_7;	// dereference
			if (type == '&') return PRECEDENCE_7;	// address of
			if (type == '!') return PRECEDENCE_7;	// not
			if (type == '~') return PRECEDENCE_6;	// xor
			if (type == '-') return PRECEDENCE_7;	// minus
		}
	}

	switch (type)
	{
	case TOKEN_LOGIC_AND:
	case TOKEN_LOGIC_OR:
		return PRECEDENCE_1;
	case TOKEN_EQUAL_COMPARISON:
	case TOKEN_LESS_EQUAL:
	case TOKEN_GREATER_EQUAL:
	case TOKEN_NOT_EQUAL:
	case '>':
	case '<':
		return PRECEDENCE_2;
	case '^':
	case '|':
	case '&':
	case TOKEN_BITSHIFT_LEFT:
	case TOKEN_BITSHIFT_RIGHT:
		return PRECEDENCE_3;
	case '+':
	case '-':
		return PRECEDENCE_4;
	case '*':
	case '/':
	case '%':
		return PRECEDENCE_5;
	case '.':
		return PRECEDENCE_8;
	case '=':
	case TOKEN_PLUS_EQUAL:
	case TOKEN_MINUS_EQUAL:
	case TOKEN_TIMES_EQUAL:
	case TOKEN_DIV_EQUAL:
	case TOKEN_AND_EQUAL:
	case TOKEN_OR_EQUAL:
	case TOKEN_XOR_EQUAL:
	case TOKEN_SHL_EQUAL:
	case TOKEN_SHR_EQUAL:
		return PRECEDENCE_0;
	default:
		return PRECEDENCE_0;
	}
}

Precedence Parser::get_precedence_level(UnaryOperation unop, bool prefixed)
{
	/*
	UNARY_OP_MINUS,
	UNARY_OP_PLUS,
	UNARY_OP_DEREFERENCE,
	UNARY_OP_ADDRESS_OF,
	UNARY_OP_VECTOR_ACCESS,
	UNARY_OP_NOT_LOGICAL,
	UNARY_OP_NOT_BITWISE,
	UNARY_OP_CAST,
	*/
	if (prefixed)
	{
		if (unop == UNARY_OP_DEREFERENCE)	return PRECEDENCE_7;	// dereference
		if (unop == UNARY_OP_ADDRESS_OF)	return PRECEDENCE_7;	// address of
		if (unop == UNARY_OP_NOT_BITWISE)	return PRECEDENCE_7;	// not
		if (unop == UNARY_OP_MINUS)			return PRECEDENCE_7;	// minus
		if (unop == UNARY_OP_NOT_LOGICAL)	return PRECEDENCE_7;	// not logic
		if (unop == UNARY_OP_CAST)			return PRECEDENCE_7;	// cast
	}
	else {
		assert(0);
	}
}

Precedence Parser::get_precedence_level(BinaryOperation bo) 
{
	Precedence p = PRECEDENCE_0;
	switch (bo) {
	case BINARY_OP_PLUS:			 p = PRECEDENCE_4; break;
	case BINARY_OP_MINUS:			 p = PRECEDENCE_4; break;
	case BINARY_OP_MULT:			 p = PRECEDENCE_5; break;
	case BINARY_OP_DIV:				 p = PRECEDENCE_5; break;
	case BINARY_OP_AND:				 p = PRECEDENCE_3; break;
	case BINARY_OP_OR:				 p = PRECEDENCE_3; break;
	case BINARY_OP_XOR:				 p = PRECEDENCE_3; break;
	case BINARY_OP_MOD:				 p = PRECEDENCE_5; break;
	case BINARY_OP_LOGIC_AND:		 p = PRECEDENCE_1; break;
	case BINARY_OP_LOGIC_OR:		 p = PRECEDENCE_1; break;
	case BINARY_OP_BITSHIFT_LEFT:	 p = PRECEDENCE_3; break;
	case BINARY_OP_BITSHIFT_RIGHT:	 p = PRECEDENCE_3; break;
	case BINARY_OP_LESS_THAN:		 p = PRECEDENCE_2; break;
	case BINARY_OP_GREATER_THAN:	 p = PRECEDENCE_2; break;
	case BINARY_OP_LESS_EQUAL:		 p = PRECEDENCE_2; break;
	case BINARY_OP_GREATER_EQUAL:	 p = PRECEDENCE_2; break;
	case BINARY_OP_EQUAL_EQUAL:		 p = PRECEDENCE_2; break;
	case BINARY_OP_NOT_EQUAL:		 p = PRECEDENCE_2; break;
	case BINARY_OP_DOT:				 p = PRECEDENCE_8; break;

	case ASSIGNMENT_OPERATION_EQUAL:		p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_PLUS_EQUAL:	p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_MINUS_EQUAL:	p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_TIMES_EQUAL:	p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_DIVIDE_EQUAL:	p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_MOD_EQUAL:	p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_AND_EQUAL:	p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_OR_EQUAL:		p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_XOR_EQUAL:	p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_SHL_EQUAL:	p = PRECEDENCE_0; break;
	case ASSIGNMENT_OPERATION_SHR_EQUAL:	p = PRECEDENCE_0; break;
	}
	return p;
}

Type_Primitive get_primitive_type(Token* tok)
{
	switch (tok->type) {
	case TOKEN_BOOL:	return Type_Primitive::TYPE_PRIMITIVE_BOOL; break;
	case TOKEN_SINT8:	return Type_Primitive::TYPE_PRIMITIVE_S8; break;
	case TOKEN_SINT16:	return Type_Primitive::TYPE_PRIMITIVE_S16; break;
	case TOKEN_SINT32:	return Type_Primitive::TYPE_PRIMITIVE_S32; break;
	case TOKEN_SINT64:	return Type_Primitive::TYPE_PRIMITIVE_S64; break;
	case TOKEN_UINT8:	return Type_Primitive::TYPE_PRIMITIVE_U8; break;
	case TOKEN_UINT16:	return Type_Primitive::TYPE_PRIMITIVE_U16; break;
	case TOKEN_UINT32:	return Type_Primitive::TYPE_PRIMITIVE_U32; break;
	case TOKEN_UINT64:	return Type_Primitive::TYPE_PRIMITIVE_U64; break;
	case TOKEN_REAL32:	return Type_Primitive::TYPE_PRIMITIVE_R32; break;
	case TOKEN_REAL64:	return Type_Primitive::TYPE_PRIMITIVE_R64; break;
	case TOKEN_VOID:	return Type_Primitive::TYPE_PRIMITIVE_VOID; break;
	default: return Type_Primitive::TYPE_PRIMITIVE_UNKNOWN; break;
	}
}

Ast* Parser::parse_expression(Scope* scope, Precedence caller_prec, bool quit_on_precedence)
{
	Ast* left_op = 0;
	Token* first = lexer->peek_token();

	if (first->flags & TOKEN_FLAG_LITERAL) {
		left_op = parse_literal();
	} else if(first->type == TOKEN_IDENTIFIER) {
		if (lexer->peek_token_type(1) == '(') {
			// proc call
			left_op = parse_proc_call(scope);
		} else {
			first = lexer->eat_token();
			left_op = create_variable(&arena, first, scope);
		}
	} else if (first->type == (Token_Type)'(') {
		lexer->eat_token();
		left_op = parse_expression(scope, PRECEDENCE_0);
		require_and_eat((Token_Type)')');
	} else if (first->flags & TOKEN_FLAG_UNARY_OPERATOR) {
		Token* tok = lexer->eat_token();
		UnaryOperation uop = get_unary_op(tok);
		Precedence unop_precedence = get_precedence_level(uop, true);
		Type_Instance* cast_type = 0;
		if (uop == UNARY_OP_CAST) {
			require_and_eat((Token_Type)'(');
			cast_type = parse_type();
			require_and_eat((Token_Type)')');
		}
		Ast* operand = parse_expression(scope, unop_precedence, true);
		left_op = create_unary_expression(&arena, operand, uop, tok, UNARY_EXP_FLAG_PREFIXED, cast_type, unop_precedence, scope);
	} else {
		if (first->type == TOKEN_END_OF_STREAM) {
			report_syntax_error(0, "unexpected end of stream at expression.\n", TOKEN_STR(first));
		}
		report_syntax_error(0, "expected expression but got '%.*s'.\n", TOKEN_STR(first));
	}

	Token* optor = lexer->eat_token();
	if (optor->flags & TOKEN_FLAG_BINARY_OPERATOR || optor->flags & TOKEN_FLAG_ASSIGNMENT_OPERATOR) {
		if (optor->type == '.' && lexer->peek_token_type() != TOKEN_IDENTIFIER) {
			Token* t = lexer->peek_token();
			report_syntax_error(0, "expected member variable, got '%.*s'.\n", TOKEN_STR(t));
		}
		Precedence bop_precedence = get_precedence_level(get_binary_op(optor));
		if (bop_precedence < caller_prec && quit_on_precedence) {
			lexer->rewind();
			return left_op;
		}
		Ast* right_op = parse_expression(scope, bop_precedence, quit_on_precedence);
		Ast* binop = create_binary_operation(&arena, left_op, right_op, optor, bop_precedence, scope);
		if (right_op->node == AST_NODE_BINARY_EXPRESSION) {
			//
			//	if it is an assignment operator, be right associative
			//
			// @FIX
			if (optor->type == '.') {
				if (get_precedence_level(optor->type, false, false) > get_precedence_level(right_op->expression.binary_exp.op) &&
					!(optor->flags & TOKEN_FLAG_ASSIGNMENT_OPERATOR)) {
					binop->expression.binary_exp.right = right_op->expression.binary_exp.left;
					right_op->expression.binary_exp.left = binop;
					return right_op;
				}
			} else {
				if (get_precedence_level(optor->type, false, false) >= get_precedence_level(right_op->expression.binary_exp.op) &&
					!(optor->flags & TOKEN_FLAG_ASSIGNMENT_OPERATOR)) {
					binop->expression.binary_exp.right = right_op->expression.binary_exp.left;
					right_op->expression.binary_exp.left = binop;
					return right_op;
				}
			}
		}
		return binop;
	} else {
		lexer->rewind();
		return left_op;
	}
}

bool Parser::is_loop_control_flow_command(Token_Type tt)
{
	if (tt == TOKEN_RETURN_STATEMENT ||
		tt == TOKEN_CONTINUE_STATEMENT ||
		tt == TOKEN_BREAK_STATEMENT)
	{
		return true;
	}
	return false;
}

bool Parser::is_control_flow_statement(Token_Type tt)
{
	if (tt == TOKEN_IF_STATEMENT ||
		tt == TOKEN_ELSE_STATEMENT ||
		tt == TOKEN_WHILE_STATEMENT ||
		tt == TOKEN_FOR_STATEMENT ||
		tt == TOKEN_DO_STATEMENT ||
		tt == TOKEN_SWITCH_STATEMENT)
	{
		return true;
	}
	return false;
}

Ast* Parser::parse_proc_call(Scope* scope)
{
	Token* name = lexer->eat_token();
	Token* next = 0;
	Ast** args = 0;
	int num_args = 0;
	require_and_eat((Token_Type)'(');
	if (lexer->peek_token_type() == (Token_Type)')') {
		require_and_eat((Token_Type)')');
	}
	else {
		args = array_create(Ast*, 4);
		while(true) {
			Ast* arg = parse_expression(scope);
			push_ast_list(&args, arg);
			num_args++;
			next = lexer->peek_token();
			if (next->type == (Token_Type)',') {
				lexer->eat_token();
			} else if (next->type != (Token_Type)')') {
				print_error_loc(stderr, filename, next->line, next->column);
				fprintf(stderr, "Syntax error: unexpected '%.*s' on argument #%d of %.*s proc call", TOKEN_STR(next), num_args, TOKEN_STR(name));
				return 0;
			} else {
				require_and_eat((Token_Type)')');
				break;
			}
		}
	}

	Ast* proc_call = create_proc_call(&arena, name, args, scope);
	return proc_call;
}

Ast* Parser::parse_command(Scope* scope)
{
	Ast* command = 0;
	Token* first = lexer->peek_token();
	if (first->type == TOKEN_IDENTIFIER) {
		// declaration or procedure call
		if (lexer->peek_token_type(1) == ':') {
			return parse_declaration(scope);
		} else if (lexer->peek_token_type(1) == '(') {
			// proc call
			Ast* call = parse_expression(scope);
			require_and_eat((Token_Type)';');
			command = call;
			return command;
		}
	}
	if (first->type == TOKEN_IF_STATEMENT) {
		lexer->eat_token();
		Ast* bool_exp = parse_expression(scope);
		Ast* body = parse_command(scope);
		Ast* else_stmt = 0;
		if (lexer->peek_token_type() == TOKEN_ELSE_STATEMENT) {
			lexer->eat_token();
			else_stmt = parse_command(scope);
		}
		command = create_if(&arena, bool_exp, body, else_stmt, scope);
	}
	else if (first->type == TOKEN_WHILE_STATEMENT) {
		lexer->eat_token();
		Ast* bool_exp = parse_expression(scope);
		Ast* body = parse_command(scope);
		
		command = create_while(&arena, bool_exp, body, scope);
	}
	else if (first->type == TOKEN_FOR_STATEMENT) {
		assert(0);
	}
	else if (first->type == TOKEN_SWITCH_STATEMENT) {
		assert(0);
	}
	else if (first->type == TOKEN_DO_STATEMENT) {
		assert(0);
	}
	else if (first->type == (Token_Type)'{') {
		command = parse_block(scope);
	}
	else if (first->type == TOKEN_BREAK_STATEMENT) {
		Token* tok = lexer->eat_token();
		command = create_break(&arena, scope, tok);
		require_and_eat((Token_Type)';');
	}
	else if (first->type == TOKEN_CONTINUE_STATEMENT) {
		Token* tok = lexer->eat_token();
		command = create_continue(&arena, scope, tok);
		require_and_eat((Token_Type)';');
	}
	else if (first->type == TOKEN_RETURN_STATEMENT) {
		Token* tok = lexer->eat_token();
		Ast* exp = 0;
		if (lexer->peek_token_type() != ';') {
			exp = parse_expression(scope);
		}
		command = create_return(&arena, exp, scope, tok);
		require_and_eat((Token_Type)';');
	}
	else if (first->type == (Token_Type)'}') {
		return 0;
	}
	else {
		command = parse_expression(scope);
		require_and_eat((Token_Type)';');
	}

	if (command == 0) {
		assert(0);
	}
	return command;
}

Ast* Parser::parse_block(Scope* scope)
{
	require_and_eat((Token_Type)'{');
	Ast* block = create_block(&arena, scope);

	Token* next = 0;
	do {
		Ast* command = parse_command(block->block.scope);
		if (!command) break;
		block_push_command(block, command);
		next = lexer->peek_token();
	} while (next->type != '}');

	require_and_eat((Token_Type)'}');
	return block;
}

Ast* Parser::parse_proc_decl(Token* name, Scope* scope) 
{
	Token* curr = require_and_eat((Token_Type)'(');

	Scope* proc_scope = create_scope(scope->level + 1, scope, SCOPE_FLAG_PROC_SCOPE);

	Ast** args = array_create(Ast*, 4);
	int n_args = 0;
	for (;;) {
		curr = lexer->eat_token();
		if (curr->type == TOKEN_IDENTIFIER) {
			require_and_eat((Token_Type)':');
			Type_Instance* arg_type = parse_type();

			// possible default value
			Ast* arg_def_value = 0;
			if (lexer->peek_token_type() == (Token_Type)'=') {
				arg_def_value = parse_expression(scope);
			}

			Decl_Site site;
			site.filename = filename;
			site.line = curr->line;
			site.column = curr->column;

			// create the argument node
			Ast* argument = create_named_argument(&arena, curr, arg_type, arg_def_value, n_args, proc_scope, &site);
			n_args++;
			array_push(args, &argument);
			if (lexer->peek_token_type() == (Token_Type)',') lexer->eat_token();
		} else if (curr->type == (Token_Type)')') {
			break;
		} else {
			report_syntax_error(curr, "argument #%d of proc %.*s cannot be nameless.\n", n_args + 1, TOKEN_STR(name));
			return 0;
		}
	}

	Type_Instance* ret_type = 0;
	if (lexer->peek_token_type() == TOKEN_ARROW) {
		lexer->eat_token();
		ret_type = parse_type();
	} else {
		ret_type = get_primitive_type(TYPE_PRIMITIVE_VOID);
	}

	Ast* body = 0;
	Token* extern_name = 0;
	if (lexer->peek_token_type() == TOKEN_SYMBOL_OPEN_BRACE) {
		body = parse_block(proc_scope);
	} else if(lexer->peek_token_type() == '#'){
		lexer->eat_token(); // #
		Token* tag = lexer->eat_token();
		// @todo check internalized string
		if (str_equal("foreign", sizeof("foreign") - 1, tag->value.data, tag->value.length)) {
			require_and_eat((Token_Type)'(');
			extern_name = lexer->eat_token();
			require_and_eat((Token_Type)')');
		} else {
			report_syntax_error(tag, "#%.*s is not recognized as procedure tag.\n", TOKEN_STR(tag));
			return 0;
		}
		require_and_eat((Token_Type)';');
	}

	Decl_Site site;
	site.filename = filename;
	site.line = name->line;
	site.column = name->column;

	scope->num_declarations += 1;
	proc_scope->num_declarations += n_args;
	return create_proc(&arena, name, extern_name, ret_type, args, n_args, body, proc_scope, &site);
}

Ast* Parser::parse_variable_decl(Token* name, Scope* scope, Type_Instance* type) 
{
	Ast* assign_exp = 0;

	if (lexer->peek_token_type() == (Token_Type)'=') {
		lexer->eat_token();
		assign_exp = parse_expression(scope);
	} else if (!type) {
		report_syntax_error(name, "expected type definition or assignment after '%.*s' variable declaration.\n", TOKEN_STR(name));
		return 0;
	}

	require_and_eat(';');

	Decl_Site site;
	site.filename = filename;
	site.line = name->line;
	site.column = name->column;

	scope->num_declarations += 1;
	Ast* vardecl = create_variable_decl(&arena, name, type, assign_exp, scope, &site);
	return vardecl;
}

Ast* Parser::parse_struct(Token* name, Scope* scope)
{
	if (name && name->type != TOKEN_IDENTIFIER) {
		print_error_loc(stderr, filename, name->line, name->column);
		report_syntax_error(name, "invalid identifier %.*s on struct declaration.\n", TOKEN_STR(name));
		return 0;
	}

	require_and_eat(TOKEN_STRUCT_WORD);
	require_and_eat(TOKEN_SYMBOL_OPEN_BRACE);

	Ast** fields = array_create(Ast*, 8);
	int num_fields = 0;
	Scope* struct_scope = create_scope(1, scope, SCOPE_FLAG_STRUCT_SCOPE);
	while (true) {
		Ast* field = parse_declaration(struct_scope);
		if (field == 0) return 0;
		array_push(fields, &field);
		num_fields++;
		if (lexer->peek_token_type() == TOKEN_SYMBOL_CLOSE_BRACE) break;
	}

	Decl_Site site;
	site.filename = filename;
	site.line = name->line;
	site.column = name->column;

	scope->num_declarations += 1;
	Ast* struct_decl = create_struct_decl(&arena, name, fields, num_fields, struct_scope, &site);
	require_and_eat(TOKEN_SYMBOL_CLOSE_BRACE);
	return struct_decl;
}

Ast* Parser::parse_enum(Token* name, Scope* scope, Type_Instance* type) {
	if (name && name->type != TOKEN_IDENTIFIER) {
		print_error_loc(stderr, filename, name->line, name->column);
		report_syntax_error(name, "invalid identifier %.*s on enum declaration.\n", TOKEN_STR(name));
		return 0;
	}

	require_and_eat(TOKEN_ENUM_WORD);
	require_and_eat(TOKEN_SYMBOL_OPEN_BRACE);

	Ast_EnumField* fields = array_create(Ast_EnumField, 4);
	int num_fields = 0;

	u64 field_number = 0;
	while (true) {
		Ast_EnumField field;
		Token* field_name = lexer->eat_token();
		field.name = field_name;
		if (field_name->type != TOKEN_IDENTIFIER) {
			report_syntax_error(field_name, "expected enum field name, but got '%.*s'.\n", TOKEN_STR(field_name));
			return 0;
		}
		Token* next = lexer->peek_token();
		if (next->type == '=') {
			lexer->eat_token(); // =
			field.expression = parse_expression(scope);
		} else {
			field.expression = create_literal(&arena, LITERAL_FLAG_NUMERIC | LITERAL_FLAG_IS_REGSIZE, 0, field_number);
		}
		next = lexer->eat_token();
		
		array_push(fields, &field);

		field_number += 1;
		num_fields += 1;

		if (next->type == ',') {
			continue;
		} else if (next->type == '}') {
			break;
		} else {
			report_syntax_error(next, "unexpected '%.*s' after enum field declaration.\n", TOKEN_STR(next));
			return 0;
		}
	}

	if (!type) type = get_primitive_type(TYPE_PRIMITIVE_U64);

	Decl_Site site = SITE_FROM_TOKEN(name);

	scope->num_declarations += 1;

	return create_enum_decl(&arena, name, fields, num_fields, type, scope, &site);
}

Ast* Parser::parse_directive(Scope* scope)
{
	Token* directive = lexer->eat_token();
	if (directive->type == TOKEN_IDENTIFIER) {
		if (str_equal(directive->value.data, directive->value.length, "include", sizeof("include") - 1)) {
			// parse include directive
			require_and_eat((Token_Type)'(');
			Ast* lit = parse_literal();
			if (lit->node != AST_NODE_LITERAL_EXPRESSION || lit->expression.literal_exp.lit_tok->type != TOKEN_STRING_LITERAL) {
				report_syntax_error(directive, "directive '%.*s' requires library name as string literal as argument.\n", TOKEN_STR(directive));
				return 0;
			}
			require_and_eat((Token_Type)')');
			Ast* decl = parse_declaration(scope);
			if (decl->node == AST_NODE_PROC_FORWARD_DECL) {
				decl->proc_decl.flags |= PROC_DECL_FLAG_IS_EXTERNAL;
			}
			return create_directive(&arena, directive, lit, decl);
		}
	} else {
		report_syntax_error(directive, "directive '%.*s' doest not exist.\n", TOKEN_STR(directive));
		return 0;
	}
}

Ast* Parser::parse_constant_decl(Token* name, Scope* scope, Type_Instance* type) {
	Ast* expr = parse_expression(scope);
	
	require_and_eat(';');

	Decl_Site site = SITE_FROM_TOKEN(name);

	scope->num_declarations += 1;

	return create_constant(&arena, name, expr, type, scope);
}

Ast* Parser::parse_declaration(Scope* scope)
{
	Token* name = lexer->eat_token();

	if (name->type == '#') {
		return parse_directive(scope);
	}

	if (name->type != TOKEN_IDENTIFIER) {
		report_syntax_error(name, "invalid identifier %.*s on declaration.\n", TOKEN_STR(name));
		return 0;
	}

	Token* decl = lexer->eat_token();
	
	if (decl->type == ':') 
	{
		Token* next = lexer->peek_token();

		if (next->type == ':') {
			// struct, procedure and constant declaration
			lexer->eat_token(); // :
			next = lexer->peek_token();
			if (next->type == '(') {
				// procedure declaration
				return parse_proc_decl(name, scope);
			} else if(next->type == TOKEN_STRUCT_WORD) {
				// struct declaration
				return parse_struct(name, scope);
			} else {
				// constant with type inference or enum
				if (lexer->peek_token_type() == TOKEN_ENUM_WORD)
					return parse_enum(name, scope, 0);
				else
					return parse_constant_decl(name, scope, 0);
				//report_syntax_error(next, "invalid declaration of '%.*s', declaration requires ':'\n", TOKEN_STR(name));
			}
		} else {
			// type for a variable, enum or constant declaration
			Type_Instance* declaration_type = 0;
			if (lexer->peek_token_type() != '=') {
				declaration_type = parse_type();
			}
			next = lexer->peek_token();
			if (next->type == ':') {
				// typed constant
				lexer->eat_token(); // ':'
				if (lexer->peek_token_type() == TOKEN_ENUM_WORD)
					return parse_enum(name, scope, declaration_type);
				else
					return parse_constant_decl(name, scope, declaration_type);
			} else {
				// variable declaration
				return parse_variable_decl(name, scope, declaration_type);
			}
		}
	} else {
		report_syntax_error(decl, "invalid declaration of '%.*s', declaration requires ':'\n", TOKEN_STR(name));
	}
	return 0;
}

Type_Instance* Parser::parse_type()
{
	Token* tok = lexer->eat_token();
	Type_Instance* ti = new Type_Instance();
	if (tok->flags & TOKEN_FLAG_PRIMITIVE_TYPE) {
		// primitive type
		Type_Primitive primitive = get_primitive_type(tok);
		ti->flags = 0 | TYPE_FLAG_IS_REGISTER_SIZE | TYPE_FLAG_IS_RESOLVED | TYPE_FLAG_IS_SIZE_RESOLVED;
		ti->type = TYPE_PRIMITIVE;
		ti->primitive = primitive;
		ti->type_size = get_size_of_primitive_type(primitive);
	} else if (tok->type == TOKEN_SYMBOL_CARAT) {
		// pointer type
		ti->flags = 0 | TYPE_FLAG_IS_REGISTER_SIZE | TYPE_FLAG_IS_SIZE_RESOLVED;
		ti->type = TYPE_POINTER;
		ti->type_size = get_size_of_pointer();
		ti->pointer_to = parse_type();
	} else if (tok->type == TOKEN_SYMBOL_OPEN_BRACKET) {
		// array type
		ti->flags = 0;
		ti->type = TYPE_ARRAY;
		ti->type_size = -1;

		int num_dimensions = 0;
		Token* f = lexer->peek_token();
		if (f->type == TOKEN_DOUBLE_DOT) {
			// dynamic array
			lexer->eat_token();
			num_dimensions = 1;
			ti->flags |= TYPE_FLAG_ARRAY_DYNAMIC;
			require_and_eat(TOKEN_SYMBOL_CLOSE_BRACKET);
		} else {
			ti->flags |= TYPE_FLAG_ARRAY_STATIC;
			// static array
			Ast** dimension_sizes = array_create(Ast*, 2);
			for (int i = 0;; ++i) {
				Ast* value = parse_expression(0);
				array_push(dimension_sizes, &value);
				num_dimensions++;
				f = lexer->eat_token();
				if (f->type == TOKEN_SYMBOL_COMMA) continue;
				else if (f->type == TOKEN_SYMBOL_CLOSE_BRACKET) break;
				else {
					parser_error = PARSER_ERROR_FATAL;
					fprintf(stderr, "Syntax error: expected end of array declaration with ']', got %.*s instead.\n", TOKEN_STR(f));
					return 0;
				}
			}
			ti->type_array.dimensions_sizes = dimension_sizes;
		}

		ti->type_array.num_dimensions = num_dimensions;
		ti->type_array.array_of = parse_type();
	} else if (tok->type == TOKEN_SYMBOL_OPEN_PAREN) {
		// function type
		ti->type = TYPE_FUNCTION;
		ti->flags = TYPE_FLAG_IS_REGISTER_SIZE | TYPE_FLAG_IS_SIZE_RESOLVED;
		ti->type_size = get_size_of_pointer();

		int num_arguments = 0;
		if (lexer->peek_token_type() != TOKEN_SYMBOL_CLOSE_PAREN) {
			Type_Instance** arg_types = array_create(Type_Instance*, 8);
			for (int i = 0;; ++i) {
				Type_Instance* value = parse_type();
				array_push(arg_types, &value);
				num_arguments++;
				if (lexer->peek_token_type() == ',') {
					lexer->eat_token();
					continue;
				} else {
					break;
				}
			}
			ti->type_function.arguments_type = arg_types;
		} else {
			// no arguments
			ti->type_function.arguments_type = 0;	
		}
		require_and_eat(TOKEN_SYMBOL_CLOSE_PAREN);
		ti->type_function.num_arguments = num_arguments;
		if (lexer->peek_token_type() == TOKEN_ARROW) {
			lexer->eat_token();
			ti->type_function.return_type = parse_type();
		} else if (lexer->peek_token_type() == TOKEN_SYMBOL_OPEN_BRACE) {
			// initiate function body, so return type is void
			Type_Instance* void_type = ti;
			void_type->flags = 0 | TYPE_FLAG_IS_REGISTER_SIZE | TYPE_FLAG_IS_RESOLVED | TYPE_FLAG_IS_SIZE_RESOLVED;
			void_type->type = TYPE_PRIMITIVE;
			void_type->primitive = TYPE_PRIMITIVE_VOID;
			void_type->type_size = get_size_of_primitive_type(TYPE_PRIMITIVE_VOID);

			ti->type_function.return_type = void_type;
		}
	} else if (tok->type == TOKEN_IDENTIFIER) {
		// struct type
		Type_Instance* struct_type = ti;

		struct_type->flags = 0;
		struct_type->type = TYPE_STRUCT;
		struct_type->type_size = 0;
		struct_type->type_struct.name = tok->value.data;
		struct_type->type_struct.name_length = tok->value.length;
		struct_type->type_struct.fields_types = 0;
	} else {
		return 0;
	}
	return ti;
}
#endif