#include "parser.h"
#include "ast.h"

Parser::Parser(Lexer* lexer) 
	: arena(65536), lexer(lexer)
{
	filename = lexer->filename;
}

void print_error_loc(FILE* out, string filename, int line, int column) {
	fprintf(out, "%.*s (%d:%d) ", filename.length, filename.data, line, column);
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

Ast** Parser::parse_top_level() 
{
	top_level = create_array(Ast*, 512);	// @todo estimate size here
	Scope* global_scope = create_scope(0, 0);

	int error = PARSER_NO_ERROR;
	while (error == PARSER_NO_ERROR && lexer->peek_token_type() != TOKEN_END_OF_STREAM) {
		Ast* decl = parse_declaration(global_scope);
		if (!decl) break;
		push_array(top_level, &decl);
	}
	if (error == PARSER_ERROR_FATAL) {
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
				return PRECEDENCE_5;
			if (type == TOKEN_MINUS_MINUS)
				return PRECEDENCE_5;
		}
		else {
			if (type == '*') return PRECEDENCE_6;	// dereference
			if (type == '&') return PRECEDENCE_6;	// address of
			if (type == '!') return PRECEDENCE_6;	// not
			if (type == '~') return PRECEDENCE_5;	// xor
			if (type == '-') return PRECEDENCE_6;	// minus
			if (type == TOKEN_PLUS_PLUS)
				return PRECEDENCE_6;
			if (type == TOKEN_MINUS_MINUS)
				return PRECEDENCE_6;
		}
	}

	switch (type)
	{
	case TOKEN_LOGIC_AND:
	case TOKEN_LOGIC_OR:
		return PRECEDENCE_0;
	case TOKEN_EQUAL_COMPARISON:
	case TOKEN_LESS_EQUAL:
	case TOKEN_GREATER_EQUAL:
	case TOKEN_NOT_EQUAL:
	case '>':
	case '<':
		return PRECEDENCE_1;
	case '^':
	case '|':
	case '&':
	case TOKEN_BITSHIFT_LEFT:
	case TOKEN_BITSHIFT_RIGHT:
		return PRECEDENCE_2;
	case '+':
	case '-':
		return PRECEDENCE_3;
	case '*':
	case '/':
	case '%':
		return PRECEDENCE_4;
	case '.':
		return PRECEDENCE_7;
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
		if (unop == UNARY_OP_DEREFERENCE)	return PRECEDENCE_6;	// dereference
		if (unop == UNARY_OP_ADDRESS_OF)	return PRECEDENCE_6;	// address of
		if (unop == UNARY_OP_NOT_BITWISE)	return PRECEDENCE_6;	// not
		if (unop == UNARY_OP_MINUS)			return PRECEDENCE_6;	// minus
		if (unop == UNARY_OP_NOT_LOGICAL)	return PRECEDENCE_6;	// not logic
		if (unop == UNARY_OP_CAST)			return PRECEDENCE_6;	// cast
	}
	else {
		assert(0);
	}
}

Precedence Parser::get_precedence_level(BinaryOperation bo) 
{
	Precedence p = PRECEDENCE_0;
	switch (bo) {
	case BINARY_OP_PLUS:			 p = PRECEDENCE_3; break;
	case BINARY_OP_MINUS:			 p = PRECEDENCE_3; break;
	case BINARY_OP_MULT:			 p = PRECEDENCE_4; break;
	case BINARY_OP_DIV:				 p = PRECEDENCE_4; break;
	case BINARY_OP_AND:				 p = PRECEDENCE_2; break;
	case BINARY_OP_OR:				 p = PRECEDENCE_2; break;
	case BINARY_OP_XOR:				 p = PRECEDENCE_2; break;
	case BINARY_OP_MOD:				 p = PRECEDENCE_4; break;
	case BINARY_OP_LOGIC_AND:		 p = PRECEDENCE_0; break;
	case BINARY_OP_LOGIC_OR:		 p = PRECEDENCE_0; break;
	case BINARY_OP_BITSHIFT_LEFT:	 p = PRECEDENCE_2; break;
	case BINARY_OP_BITSHIFT_RIGHT:	 p = PRECEDENCE_2; break;
	case BINARY_OP_LESS_THAN:		 p = PRECEDENCE_1; break;
	case BINARY_OP_GREATER_THAN:	 p = PRECEDENCE_1; break;
	case BINARY_OP_LESS_EQUAL:		 p = PRECEDENCE_1; break;
	case BINARY_OP_GREATER_EQUAL:	 p = PRECEDENCE_1; break;
	case BINARY_OP_EQUAL_EQUAL:		 p = PRECEDENCE_1; break;
	case BINARY_OP_NOT_EQUAL:		 p = PRECEDENCE_1; break;
	case BINARY_OP_DOT:				 p = PRECEDENCE_7; break;
	}
	return p;
}

AssignmentOperation Parser::get_assignment_op(Token_Type tt)
{
	AssignmentOperation assop = ASSIGNMENT_OPERATION_UNKNOWN;
	switch (tt) {
	case '=':				assop = ASSIGNMENT_OPERATION_EQUAL; break;
	case TOKEN_PLUS_EQUAL:	assop = ASSIGNMENT_OPERATION_PLUS_EQUAL; break;
	case TOKEN_MINUS_EQUAL:	assop = ASSIGNMENT_OPERATION_MINUS_EQUAL; break;
	case TOKEN_TIMES_EQUAL:	assop = ASSIGNMENT_OPERATION_TIMES_EQUAL; break;
	case TOKEN_DIV_EQUAL:	assop = ASSIGNMENT_OPERATION_DIVIDE_EQUAL; break;
	case TOKEN_AND_EQUAL:	assop = ASSIGNMENT_OPERATION_AND_EQUAL; break;
	case TOKEN_OR_EQUAL:	assop = ASSIGNMENT_OPERATION_OR_EQUAL; break;
	case TOKEN_XOR_EQUAL:	assop = ASSIGNMENT_OPERATION_XOR_EQUAL; break;
	case TOKEN_SHL_EQUAL:	assop = ASSIGNMENT_OPERATION_SHL_EQUAL; break;
	case TOKEN_SHR_EQUAL:	assop = ASSIGNMENT_OPERATION_SHR_EQUAL; break;
	}
	return assop;
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
			left_op = create_variable(&arena, first, 0);
		}
	} else if (first->type == (Token_Type)'(') {
		lexer->eat_token();
		left_op = parse_expression(scope, PRECEDENCE_0);
		require_and_eat((Token_Type)')');
	} else if (first->flags & TOKEN_FLAG_UNARY_OPERATOR) {
		UnaryOperation uop = get_unary_op(lexer->eat_token());
		Precedence unop_precedence = get_precedence_level(uop, true);
		Type* cast_type = 0;
		if (uop == UNARY_OP_CAST) {
			require_and_eat((Token_Type)'(');
			cast_type = parse_type();
			require_and_eat((Token_Type)')');
		}
		Ast* operand = parse_expression(scope, unop_precedence, true);
		left_op = create_unary_expression(&arena, operand, uop, UNARY_EXP_FLAG_PREFIXED, cast_type, unop_precedence, scope);
	}

	Token* optor = lexer->eat_token();
	if (optor->flags & TOKEN_FLAG_BINARY_OPERATOR) {
		Precedence bop_precedence = get_precedence_level(get_binary_op(optor));
		if (bop_precedence < caller_prec && quit_on_precedence) {
			lexer->rewind();
			return left_op;
		}
		Ast* right_op = parse_expression(scope, bop_precedence, quit_on_precedence);
		Ast* binop = create_binary_operation(&arena, left_op, right_op, optor, bop_precedence, scope);
		if (right_op->node == AST_NODE_BINARY_EXPRESSION) {
			if (get_precedence_level(optor->type, false, false) >= get_precedence_level(right_op->expression.binary_exp.op)) {
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

Ast* Parser::parse_variable_assignment(Scope* scope)
{
	Ast* vassign = 0;
	Ast* lvalue = parse_expression(scope);
	Token* eqsign = lexer->eat_token();
	AssignmentOperation assign_op = ASSIGNMENT_OPERATION_UNKNOWN;
	assign_op = get_assignment_op(eqsign->type);
	
	if (assign_op == ASSIGNMENT_OPERATION_UNKNOWN) {
		print_error_loc(stderr, filename, eqsign->line, eqsign->column);
		fprintf(stderr, "Syntax error: '%.*s' assignment operation is invalid.\n", TOKEN_STR(eqsign));
		return 0;
	}

	Ast* rvalue = parse_expression(scope);
	
	vassign = create_var_assignment(&arena, lvalue, rvalue, assign_op, scope);

	return vassign;
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
		}
		// proc call
		else if (lexer->peek_token_type(1) == '(') {
			Ast* call = parse_expression(scope);
			require_and_eat((Token_Type)';');
			command = call;
		}
		// variable assignment
		else if (get_assignment_op(lexer->peek_token_type(1)) != ASSIGNMENT_OPERATION_UNKNOWN) {
			Ast* assign_op = parse_variable_assignment(scope);
			require_and_eat((Token_Type)';');
			command = assign_op;
		}
	}
	else if (first->type == TOKEN_IF_STATEMENT) {
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
		lexer->eat_token();
		command = create_break(&arena, scope);
		require_and_eat((Token_Type)';');
	}
	else if (first->type == TOKEN_CONTINUE_STATEMENT) {
		lexer->eat_token();
		command = create_continue(&arena, scope);
		require_and_eat((Token_Type)';');
	}
	else if (first->type == TOKEN_RETURN_STATEMENT) {
		lexer->eat_token();
		Ast* exp = 0;
		if (lexer->peek_token_type() != ';') {
			exp = parse_expression(scope);
		}
		command = create_return(&arena, exp, scope);
		require_and_eat((Token_Type)';');
	}
	else if (first->type == (Token_Type)'}') {
		command = create_return(&arena, 0, scope);
	}
	else {
		command = parse_expression(scope);
	}

	if (command == 0) {
		//assert(0);
	}
	return command;
}

Ast* Parser::parse_block(Scope* scope)
{
	require_and_eat((Token_Type)'{');
	Ast* block = create_block(&arena, scope);

	Token* next = 0;
	do {
		Ast* command = parse_command(scope);
		block_push_command(block, command);
		next = lexer->peek_token();
	} while (next->type != '}');

	require_and_eat((Token_Type)'}');
	return block;
}

Ast* Parser::parse_proc(Token* name, Scope* scope) 
{
	if (name == 0) {
		name = lexer->eat_token();
		if (require_token_type(name, TOKEN_IDENTIFIER) == -1) return 0;
	}

	// (name1 : type1, name2 : type2)
	Token* curr = require_and_eat(TOKEN_COLON_COLON);
	curr = require_and_eat((Token_Type)'(');

	Ast* args = 0;
	Ast* first_arg = 0;
	int n_args = 0;
	while (true) {
		curr = lexer->eat_token();
		if (curr->type == TOKEN_IDENTIFIER) {
			require_and_eat((Token_Type)':');
			Type* arg_type = parse_type();

			// possible default value
			Ast* arg_def_value = 0;
			if (lexer->peek_token_type() == (Token_Type)'=') {
				arg_def_value = parse_expression(scope);
			}

			// create the argument node
			Ast* argument = create_named_argument(&arena, curr, arg_type, arg_def_value, n_args);
			if (args == 0) {
				args = argument;
				first_arg = args;
			} else {
				args->named_arg.next = argument;
				args = argument;
			}
			n_args++;
			if (lexer->peek_token_type() == (Token_Type)',') lexer->eat_token();
		} else if (curr->type == (Token_Type)')') {
			break;
		} else {
			print_error_loc(stderr, filename, curr->line, curr->column);
			fprintf(stderr, "Syntax error: argument #%d of proc %.*s cannot be nameless.\n",
				n_args + 1, TOKEN_STR(name));
			parser_error = PARSER_ERROR_FATAL;
			return 0;
		}
	}

	Type* ret_type = 0;
	if (lexer->peek_token_type() == TOKEN_ARROW) {
		lexer->eat_token();
		ret_type = parse_type();
	} else {
		ret_type = get_type(TOKEN_VOID);
	}

	Ast* body = 0;
	if (lexer->peek_token_type() == (Token_Type)'{') {
		Scope* proc_scope = create_scope(scope->level + 1, scope);
		body = parse_block(proc_scope);
	}

	return create_proc(&arena, name, ret_type, first_arg, n_args, body, scope);
}

Ast* Parser::parse_variable_decl(Token* name, Scope* scope) 
{
	if (name == 0) {
		name = lexer->eat_token();
		if (require_token_type(name, TOKEN_IDENTIFIER) == -1) return 0;
	}
	require_and_eat((Token_Type)':');

	Type* type = 0;
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

	Ast* vardecl = create_variable_decl(&arena, name, type, assign_exp, scope);
	return vardecl;
}

Ast* Parser::parse_declaration(Scope* scope)
{
	Token* name = lexer->eat_token();
	if (name->type != TOKEN_IDENTIFIER) {
		print_error_loc(stderr, filename, name->line, name->column);
		fprintf(stderr, "Syntax error: invalid identifier %.*s on declaration", TOKEN_STR(name));
		parser_error = PARSER_ERROR_FATAL;
		return 0;
	}

	Token* decl = lexer->peek_token();
	
	if (decl->type == TOKEN_COLON_COLON) {
		if (lexer->peek_token_type(1) == TOKEN_STRUCT_WORD) {
			// id :: struct {}
			//return parse_struct();
			return 0;
		} else if (lexer->peek_token_type(1) == (Token_Type)'(') {
			// id :: () {}
			return parse_proc(name, scope);
		}
	} 
	else if (decl->type == (Token_Type)':') {
		// id :: type;
		Ast* vardecl = parse_variable_decl(name, scope);
		require_and_eat((Token_Type)';');
		return vardecl;
	}
}

Type* Parser::parse_type()
{
	Type* type = 0;
	Token* t = lexer->eat_token();

	// if is primitive
	if (t->flags & TOKEN_FLAG_PRIMITIVE_TYPE) {
		type = get_type(t->type);
		return type;
	} else if (t->type == (Token_Type)'^') {
		Type* t = (Type*)malloc(sizeof(Type));
		t->primitive = TYPE_NOT_PRIMITIVE;
		t->pointer_to = parse_type();
		return t;
	}
	else {
		// not implemented
		assert(0);
	}
	return 0;
}