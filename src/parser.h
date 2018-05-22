#pragma once
#include "util.h"
#include "ast.h"
#include "lexer.h"
#include "memory.h"
#include "file_table.h"

enum Parser_Error {
	PARSER_OK = 0,
	PARSER_ERROR_FATAL,
	PARSER_ERROR_WARNING,
	PARSER_INTERNAL_COMPILER_ERROR,
};

struct Parser {
	Lexer* lexer;
	Scope* global_scope;
	Ast**  top_level;

	Token** current_foreign = 0;

	Parser(Lexer* lexer, Scope* global_scope) : lexer(lexer), global_scope(global_scope) {}

	Ast** parse_top_level();

	void parse_directive(Scope* scope);

	// String data
	Ast* data_global_string_push(Token* s);

	// Type parsing
	Type_Instance* parse_type();
	Type_Instance* parse_type_primitive();
	Type_Instance* parse_type_pointer();
	Type_Instance* parse_type_struct(Token* name);
	Type_Instance* parse_type_array();
	Type_Instance* parse_type_function();

	// Declaration parsing
	Ast* parse_declaration(Scope* scope);
	Ast* parse_decl_proc(Token* name, Scope* scope);
	Ast* parse_decl_variable(Token* name, Scope* scope);
	Ast* parse_decl_variable(Token* name, Scope* scope, Type_Instance* type);
	Ast* parse_decl_struct(Token* name, Scope* scope, bool is_union = false);
	Ast* parse_decl_enum(Token* name, Scope* scope, Type_Instance* hint);
	Ast* parse_decl_constant(Token* name, Scope* scope, Type_Instance* type);

	// Expression parsing
	Ast* parse_expression(Scope* scope);
	Ast* parse_expression_precedence1(Scope* scope);
	Ast* parse_expression_precedence2(Scope* scope);
	Ast* parse_expression_precedence3(Scope* scope);
	Ast* parse_expression_precedence4(Scope* scope);
	Ast* parse_expression_precedence5(Scope* scope);
	Ast* parse_expression_precedence6(Scope* scope);
	Ast* parse_expression_precedence7(Scope* scope);
	Ast* parse_expression_precedence8(Scope* scope);
	Ast* parse_expression_precedence9(Scope* scope);
	Ast* parse_expression_precedence10(Scope* scope);
	Ast* parse_expression_left_dot(Scope* scope);

	Ast* parse_expr_literal_struct(Token* name, Scope* scope);
	Ast* parse_expr_literal_array(Scope* scope);
	Ast* parse_expr_literal(Scope* scope);
	//Ast* parse_expr_proc_call(Scope* scope);

	// Expression auxiliary
	Precedence unary_op_precedence_level(Operator_Unary unop, bool prefixed);
	Precedence binary_op_precedence_level(Operator_Binary bo);

	// Command parsing
	Ast** parse_command_comma_list(Scope* scope);
	Ast*  parse_command(Scope* scope, bool eat_semicolon = true);
	Ast*  parse_comm_block(Scope* scope, Ast* creator);
	Ast*  parse_comm_if(Scope* scope);
	Ast*  parse_comm_for(Scope* scope);
	Ast*  parse_comm_while(Scope* scope);
	Ast*  parse_comm_break(Scope* scope);
	Ast*  parse_comm_continue(Scope* scope);
	Ast*  parse_comm_return(Scope* scope);
	Ast*  parse_comm_variable_assignment(Scope* scope);

	// Error report
	void   report_fatal_error(Token* error_token, char* msg, ...);
	void   report_syntax_error(Token* error_token, char* msg, ...);
	void   report_error_location(Token* tok);
	Token* require_and_eat(Token_Type t);
	Token* require_and_eat(char c);

	static void init();
};

struct Parse_Queue {
	char**  queue_main;
	Token** queue_imports;	// array of filenames (fullpath) of files to be parsed
	Ast***  files_toplevels;
};
extern string* g_lib_table;

Ast** parse_files_in_queue(Scope* global_scope);
void queue_file_for_parsing(Token* token);
void queue_file_for_parsing(char* filename);