#pragma once
#include "util.h"
#include "ast.h"
#include "memory.h"

enum Parser_Error {
	PARSER_OK = 0,
	PARSER_ERROR_FATAL,
	PARSER_ERROR_WARNING,
	PARSER_INTERNAL_COMPILER_ERROR,
};

#if 0
struct Parser {
	int parser_error;

	string filename;
	Ast**  ast_top_level;
	Lexer* lexer;

	Parser(Lexer* lexer);

	Ast** parse_top_level(Scope* global_scope, Ast** ast);
	Ast* parse_declaration(Scope* scope);
	Ast* parse_expression(Scope* scope, Precedence caller_prec = PRECEDENCE_0, bool quit_on_precedence = false);
	Ast* parse_block(Scope* scope);
	Ast* parse_variable_decl(Token* name, Scope* scope, Type_Instance* type);
	Ast* parse_proc_decl(Token* name, Scope* scope);
	Ast* parse_literal();
	Ast* parse_command(Scope* scope);
	Ast* parse_proc_call(Scope* scope);
	Ast* parse_struct(Token* name, Scope* scope);
	Ast* parse_enum(Token* name, Scope* scope, Type_Instance* type);
	Ast* parse_directive(Scope* scope);
	Ast* parse_constant_decl(Token* name, Scope* scope, Type_Instance* type);

	Precedence get_precedence_level(Token_Type type, bool postfixed, bool unary);
	Precedence get_precedence_level(UnaryOperation uo, bool prefixed);
	Precedence get_precedence_level(BinaryOperation bo);
	bool is_loop_control_flow_command(Token_Type tt);
	bool is_control_flow_statement(Token_Type tt);

	Type_Instance* parse_type();

	Token* require_and_eat(Token_Type t);
	Token* require_and_eat(char t);
	
	// returns -1 on error, 0 on success
	int require_token_type(Token* tok, Token_Type type);
	int report_syntax_error(Token* error_token, char* msg, ...);
};
#endif