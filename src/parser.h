#pragma once
#include "util.h"
#include "ast.h"
#include "memory.h"

const u32 PARSER_NO_ERROR = 0;
const u32 PARSER_ERROR_FATAL = 1;
const u32 PARSER_ERROR_WARNING = 2;
const u32 PARSER_ERROR_END = 3;
const u32 PARSER_INTERNAL_COMPILER_ERROR = 4;

struct Parser {
	int parser_error;

	string filename;
	Ast** top_level;
	Lexer* lexer;
	Memory_Arena arena;

	Parser(Lexer* lexer);

	Ast** parse_top_level(Scope* global_scope);
	Ast* parse_declaration(Scope* scope);
	Ast* parse_expression(Scope* scope, Precedence caller_prec = PRECEDENCE_0, bool quit_on_precedence = false);
	Ast* parse_block(Scope* scope);
	Ast* parse_variable_decl(Token* name, Scope* scope);
	Ast* parse_proc_decl(Token* name, Scope* scope);
	Ast* parse_literal();
	Ast* parse_command(Scope* scope);
	Ast* parse_proc_call(Scope* scope);
	Ast* parse_struct(Token* name, Scope* scope);
	Ast* parse_directive(Scope* scope);

	Precedence get_precedence_level(Token_Type type, bool postfixed, bool unary);
	Precedence get_precedence_level(UnaryOperation uo, bool prefixed);
	Precedence get_precedence_level(BinaryOperation bo);
	bool is_loop_control_flow_command(Token_Type tt);
	bool is_control_flow_statement(Token_Type tt);

	Type_Instance* parse_type();

	Token* require_and_eat(Token_Type t);
	
	// returns -1 on error, 0 on success
	int require_token_type(Token* tok, Token_Type type);
	int report_syntax_error(char* msg, ...);
};

#define TOKEN_STR(T) T->value.length, T->value.data