#include "parser.h"
#include "ast.h"
#include <stdarg.h>
#include "symbol_table.h"

Parser::Parser(Lexer* lexer) 
	: arena(65536), lexer(lexer)
{
	filename = lexer->filename;
}

void print_error_loc(FILE* out, string filename, int line, int column) {
	fprintf(out, "%.*s (%d:%d) ", filename.length, filename.data, line, column);
}

int Parser::report_syntax_error(char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	fprintf(stderr, "Syntax Error: ");
	vfprintf(stderr, msg, args);
	va_end(args);
	parser_error = PARSER_ERROR_FATAL;
	return 0;
}

Token* Parser::require_and_eat(Token_Type t)
{
	Token* tok = lexer->eat_token();
	if (tok->type != t) {
		print_error_loc(stderr, filename, tok->line, tok->column);

		fprintf(stderr, "Syntax error: expected %s got '%.*s'.\n", lexer->get_token_string(t), TOKEN_STR(tok));
		parser_error = PARSER_ERROR_FATAL;
		exit(-1);
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

Ast** Parser::parse_top_level(Scope* global_scope) 
{
	if (lexer->token_count == 1 && lexer->peek_token_type() == TOKEN_END_OF_STREAM) {
		return 0;
	}
	top_level = create_array(Ast*, 2048);	// @todo estimate size here @important

	parser_error = PARSER_NO_ERROR;
	while (parser_error == PARSER_NO_ERROR && lexer->peek_token_type() != TOKEN_END_OF_STREAM) {
		Ast* decl = parse_declaration(global_scope);
		if (!decl) break;
		push_array(top_level, &decl);
	}
	if (parser_error == PARSER_ERROR_FATAL) {
		fprintf(stderr, "There were errors, exiting...\n");
		return 0;
	}
	return top_level;
}

Ast* Parser::parse_literal()
{
	Token* lit_token = lexer->eat_token();
	Ast* literal_ast = 0;

	u32 flags = 0;
	if (lit_token->type == TOKEN_INT_LITERAL) {
		flags |= LITERAL_FLAG_IS_REGSIZE;
	} else if (lit_token->type == TOKEN_FLOAT_LITERAL) {
		flags |= LITERAL_FLAG_IS_REGSIZE;
	} else if (lit_token->type == TOKEN_CHAR_LITERAL) {
		flags |= LITERAL_FLAG_IS_REGSIZE;
	} else if (lit_token->type == TOKEN_BOOL_LITERAL) {
		flags |= LITERAL_FLAG_IS_REGSIZE;
	} else if (lit_token->type == TOKEN_STRING_LITERAL) {
		flags = 0;
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
			if (type == TOKEN_PLUS_PLUS)
				return PRECEDENCE_6;
			if (type == TOKEN_MINUS_MINUS)
				return PRECEDENCE_6;
		}
		else {
			if (type == '*') return PRECEDENCE_7;	// dereference
			if (type == '&') return PRECEDENCE_7;	// address of
			if (type == '!') return PRECEDENCE_7;	// not
			if (type == '~') return PRECEDENCE_6;	// xor
			if (type == '-') return PRECEDENCE_7;	// minus
			if (type == TOKEN_PLUS_PLUS)
				return PRECEDENCE_7;
			if (type == TOKEN_MINUS_MINUS)
				return PRECEDENCE_7;
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
	case TOKEN_INT:		return Type_Primitive::TYPE_PRIMITIVE_S32; break;
	case TOKEN_CHAR:	return Type_Primitive::TYPE_PRIMITIVE_S8; break;
	case TOKEN_FLOAT:	return Type_Primitive::TYPE_PRIMITIVE_R32; break;
	case TOKEN_REAL32:	return Type_Primitive::TYPE_PRIMITIVE_R32; break;
	case TOKEN_DOUBLE:	return Type_Primitive::TYPE_PRIMITIVE_R64; break;
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
		UnaryOperation uop = get_unary_op(lexer->eat_token());
		Precedence unop_precedence = get_precedence_level(uop, true);
		Type_Instance* cast_type = 0;
		if (uop == UNARY_OP_CAST) {
			require_and_eat((Token_Type)'(');
			cast_type = parse_type();
			require_and_eat((Token_Type)')');
		}
		Ast* operand = parse_expression(scope, unop_precedence, true);
		left_op = create_unary_expression(&arena, operand, uop, UNARY_EXP_FLAG_PREFIXED, cast_type, unop_precedence, scope);
	}

	Token* optor = lexer->eat_token();
	if (optor->flags & TOKEN_FLAG_BINARY_OPERATOR || optor->flags & TOKEN_FLAG_ASSIGNMENT_OPERATOR) {
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
			if (get_precedence_level(optor->type, false, false) >= get_precedence_level(right_op->expression.binary_exp.op) &&
				!(optor->flags & TOKEN_FLAG_ASSIGNMENT_OPERATOR)) {
				binop->expression.binary_exp.right = right_op->expression.binary_exp.left;
				right_op->expression.binary_exp.left = binop;
				return right_op;
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
		args = create_array(Ast*, 4);
		while(true) {
			Ast* arg = parse_expression(scope);
			push_ast_list(args, arg);
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
		// variable decl
		if (lexer->peek_token_type(1) == ':') {
			Ast* vdecl = parse_variable_decl(lexer->eat_token(), scope);
			require_and_eat((Token_Type)';');
			command = vdecl;
			return command;
		}
		// proc call
		else if (lexer->peek_token_type(1) == '(') {
			Ast* call = parse_expression(scope);
			require_and_eat((Token_Type)';');
			command = call;
			return command;
		}
		else if (lexer->peek_token_type(1) == TOKEN_COLON_COLON) {
			// proc declaration
			Token* name = lexer->eat_token();
			command = parse_proc_decl(name, scope);
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
		command = create_return(&arena, 0, scope, first);
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
		if (!command) return 0;
		block_push_command(block, command);
		next = lexer->peek_token();
	} while (next->type != '}');

	require_and_eat((Token_Type)'}');
	return block;
}

Ast* Parser::parse_proc_decl(Token* name, Scope* scope) 
{
	if (name == 0) {
		name = lexer->eat_token();
		if (require_token_type(name, TOKEN_IDENTIFIER) == -1) return 0;
	}

	// (name1 : type1, name2 : type2)
	Token* curr = require_and_eat(TOKEN_COLON_COLON);
	curr = require_and_eat((Token_Type)'(');

	Scope* proc_scope = create_scope(scope->level + 1, scope, SCOPE_FLAG_PROC_SCOPE);

	Ast** args = create_array(Ast*, 4);
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
			push_array(args, &argument);
			if (lexer->peek_token_type() == (Token_Type)',') lexer->eat_token();
		} else if (curr->type == (Token_Type)')') {
			break;
		} else {
			print_error_loc(stderr, filename, curr->line, curr->column);
			report_syntax_error("argument #%d of proc %.*s cannot be nameless.\n", n_args + 1, TOKEN_STR(name));
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
	if (lexer->peek_token_type() == TOKEN_SYMBOL_OPEN_BRACE) {
		body = parse_block(proc_scope);
	} else if(lexer->peek_token_type() == ';'){
		lexer->eat_token();
		body = 0;
	}

	Decl_Site site;
	site.filename = filename;
	site.line = name->line;
	site.column = name->column;

	scope->num_declarations += 1;
	proc_scope->num_declarations += n_args;
	return create_proc(&arena, name, ret_type, args, n_args, body, proc_scope, &site);
}

Ast* Parser::parse_variable_decl(Token* name, Scope* scope) 
{
	if (name == 0) {
		name = lexer->eat_token();
		if (require_token_type(name, TOKEN_IDENTIFIER) == -1) return 0;
	}
	require_and_eat((Token_Type)':');

	Type_Instance* type = 0;
	Ast* assign_exp = 0;

	if (lexer->peek_token_type() == (Token_Type)'=') {
		lexer->eat_token();
		assign_exp = parse_expression(scope);
	} else {
		type = parse_type();
		if (lexer->peek_token_type() == (Token_Type)'=') {
			lexer->eat_token();
			assign_exp = parse_expression(scope);
		}
	}

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
		report_syntax_error("invalid identifier %.*s on struct declaration.\n", TOKEN_STR(name));
		return 0;
	}
	require_and_eat(TOKEN_COLON_COLON);
	require_and_eat(TOKEN_STRUCT_WORD);
	require_and_eat(TOKEN_SYMBOL_OPEN_BRACE);

	Ast** fields = create_array(Ast*, 8);
	int num_fields = 0;
	Scope* struct_scope = create_scope(1, scope, SCOPE_FLAG_STRUCT_SCOPE);
	while (true) {
		Ast* field = parse_declaration(struct_scope);
		if (field == 0) return 0;
		push_array(fields, &field);
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

Ast* Parser::parse_declaration(Scope* scope)
{
	Token* name = lexer->eat_token();
	if (name->type != TOKEN_IDENTIFIER) {
		print_error_loc(stderr, filename, name->line, name->column);
		report_syntax_error("invalid identifier %.*s on declaration.\n", TOKEN_STR(name));
		return 0;
	}

	Token* decl = lexer->peek_token();
	
	if (decl->type == TOKEN_COLON_COLON) {
		if (lexer->peek_token_type(1) == TOKEN_STRUCT_WORD) {
			// id :: struct {}
			return parse_struct(name, scope);
		} else if (lexer->peek_token_type(1) == (Token_Type)'(') {
			// id :: () {}
			return parse_proc_decl(name, scope);
		} else {
			Token* t = lexer->peek_token(1);
			print_error_loc(stderr, filename, t->line, t->column);
			report_syntax_error("invalid token '%.*s' on declaration site of %.*s.\n", TOKEN_STR(t), TOKEN_STR(name));
			return 0;
		}
	} 
	else if (decl->type == (Token_Type)':') {
		// id :: type;
		Ast* vardecl = parse_variable_decl(name, scope);
		require_and_eat((Token_Type)';');
		return vardecl;
	}
	else {
		report_syntax_error("invalid declaration of '%.*s'\n", TOKEN_STR(name));
		return 0;
	}
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
			Ast** dimension_sizes = create_array(Ast*, 2);
			for (int i = 0;; ++i) {
				Ast* value = parse_expression(0);
				push_array(dimension_sizes, &value);
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
			Type_Instance** arg_types = create_array(Type_Instance*, 8);
			for (int i = 0;; ++i) {
				Type_Instance* value = parse_type();
				push_array(arg_types, &value);
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
	}
	return ti;
}