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

	Ast** parse_top_level();
	Ast* parse_declaration();
	Ast* parse_expression(Precedence caller_prec = PRECEDENCE_0);
	Ast* parse_block();
	Ast* parse_variable_decl(Token* name);
	Ast* parse_proc(Token* name);
	Ast* parse_literal();

	Precedence get_precedence_level(Token_Type tt, bool postfixed, bool unary);
	Precedence get_precedence_level(BinaryOperation bo);

	Type* parse_type();

	Token* require_and_eat(Token_Type t);
	
	// returns -1 on error, 0 on success
	int require_token_type(Token* tok, Token_Type type);
};

#define TOKEN_STR(T) T->value.length, T->value.data