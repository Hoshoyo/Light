#pragma once
#include "util.h"
#include "lexer.h"
#include "type.h"
#include "memory.h"
#include <stdio.h>

struct Ast_ProcDecl;
struct Ast_VarDecl;
struct Ast_StructDecl;
struct Ast_Binary_Exp;
struct Ast_Unary_Exp;
struct Ast_ProcDecl;
struct Ast;

struct Scope;

enum Node_Type {
	AST_NODE_PROC_DECLARATION,
	AST_NODE_NAMED_ARGUMENT,
	AST_NODE_VARIABLE_DECL,
	AST_NODE_BINARY_EXPRESSION,
	AST_NODE_UNARY_EXPRESSION,
	AST_NODE_LITERAL_EXPRESSION,
	AST_NODE_VARIABLE_EXPRESSION,
	AST_NODE_BLOCK,
	AST_NODE_VARIABLE_ASSIGNMENT,
	AST_NODE_PROCEDURE_CALL,
	AST_NODE_IF_STATEMENT,
	AST_NODE_WHILE_STATEMENT,
	AST_NODE_RETURN_STATEMENT,
	AST_NODE_BREAK_STATEMENT,
	AST_NODE_CONTINUE_STATEMENT,
};

enum Precedence
{
	PRECEDENCE_0 = 0,	//	 || &&
	PRECEDENCE_1 = 1,	//	 == >= <= != > <
	PRECEDENCE_2 = 2,	//	 ^ | & >> <<
	PRECEDENCE_3 = 3,	//	 + -
	PRECEDENCE_4 = 4,	//	 * / %
	PRECEDENCE_5 = 5,	//	 &(addressof) ~ ++POST --POST
	PRECEDENCE_6 = 6,	//	 *(dereference)	cast ! ++PRE --PRE
	PRECEDENCE_7 = 7,	//	 .
	PRECEDENCE_MAX = 8,
};

enum AssignmentOperation 
{
	ASSIGNMENT_OPERATION_UNKNOWN,

	ASSIGNMENT_OPERATION_EQUAL,
	ASSIGNMENT_OPERATION_PLUS_EQUAL,
	ASSIGNMENT_OPERATION_MINUS_EQUAL,
	ASSIGNMENT_OPERATION_TIMES_EQUAL,
	ASSIGNMENT_OPERATION_DIVIDE_EQUAL,
	ASSIGNMENT_OPERATION_AND_EQUAL,
	ASSIGNMENT_OPERATION_OR_EQUAL,
	ASSIGNMENT_OPERATION_XOR_EQUAL,
	ASSIGNMENT_OPERATION_SHL_EQUAL,
	ASSIGNMENT_OPERATION_SHR_EQUAL,
};

enum BinaryOperation
{
	BINARY_OP_UNKNOWN = 0,

	BINARY_OP_PLUS = 1,
	BINARY_OP_MINUS = 2,
	BINARY_OP_MULT = 3,
	BINARY_OP_DIV = 4,
	BINARY_OP_AND = 5,
	BINARY_OP_OR = 6,
	BINARY_OP_XOR = 7,
	BINARY_OP_MOD = 8,
	BINARY_OP_LOGIC_AND = 9,
	BINARY_OP_LOGIC_OR = 10,
	BINARY_OP_BITSHIFT_LEFT = 11,
	BINARY_OP_BITSHIFT_RIGHT = 12,

	BINARY_OP_LESS_THAN = 13,
	BINARY_OP_GREATER_THAN = 14,
	BINARY_OP_LESS_EQUAL = 15,
	BINARY_OP_GREATER_EQUAL = 16,
	BINARY_OP_EQUAL_EQUAL = 17,
	BINARY_OP_NOT_EQUAL = 18,

	BINARY_OP_DOT = 19,
};

enum UnaryOperation
{
	UNARY_OP_UNKNOWN = 0,

	UNARY_OP_MINUS,
	UNARY_OP_PLUS,
	UNARY_OP_DEREFERENCE,
	UNARY_OP_ADDRESS_OF,
	UNARY_OP_VECTOR_ACCESS,
	UNARY_OP_NOT_LOGICAL,
	UNARY_OP_NOT_BITWISE,
	UNARY_OP_CAST,
};

// declarations
struct Ast_ProcDecl {
	Token* name;
	Ast* arguments;
	Ast* body;
	int num_args;
	Type* proc_ret_type;
	Scope* scope;
};
struct Ast_VarDecl {
	Token* name;
	Ast* assignment;
	Type* type;
	Scope* scope;
	s32 size_bytes;
	s32 alignment;
};
struct Ast_StructDecl {
	Token* name;
	Ast_VarDecl** fields;
	s64 size_bytes;
	s32 alignment;
};

struct Ast_Binary_Exp {
	BinaryOperation op;
	Precedence precedence;
	Ast* left;
	Ast* right;
	Scope* scope;
};

const u32 UNARY_EXP_FLAG_PREFIXED = FLAG(0);
const u32 UNARY_EXP_FLAG_POSTFIXED = FLAG(1);
struct Ast_Unary_Exp {
	Precedence precedence;
	u32 flags;
	UnaryOperation op;
	Ast* operand;
	Type* cast_type;
	Scope* scope;
};

const u32 LITERAL_FLAG_IS_REGSIZE = FLAG(0);
struct Ast_Literal {
	u32 flags;
	Token* lit_tok;
	Type* type;
};

struct Ast_Variable {
	Token* name;
	Type* type;
	Scope* scope;
};

struct Ast_ProcCall {
	Scope* scope;
	Token* name;
	Ast** args;
};

const u32 EXPRESSION_TYPE_LITERAL = 1;
const u32 EXPRESSION_TYPE_BINARY = 2;
const u32 EXPRESSION_TYPE_UNARY = 3;
const u32 EXPRESSION_TYPE_VARIABLE = 4;
const u32 EXPRESSION_TYPE_PROC_CALL = 5;

struct Ast_Expression {
	u32 expression_type;
	union {
		Ast_Binary_Exp binary_exp;
		Ast_Unary_Exp unary_exp;
		Ast_Literal literal_exp;
		Ast_Variable variable_exp;
		Ast_ProcCall proc_call;
	};
};

struct Ast_NamedArgument {
	Token* arg_name;
	Type* arg_type;
	Ast* default_value;
	int index;
	Ast* next;
};

struct Ast_Block {
	Scope* scope;
	Ast** commands;
};

struct Ast_VarAssign {
	Scope* scope;
	Ast* lvalue;
	Ast* rvalue;
	AssignmentOperation assign_op;
};

struct Ast_If {
	Ast* bool_exp;
	Ast* body;
	Ast* else_exp;
	Scope* scope;
};

struct Ast_While {
	Ast* bool_exp;
	Ast* body;
	Scope* scope;
};

struct Ast_Return {
	Ast* expr;
	Scope* scope;
};

struct Ast_Break {
	Scope* scope;
};

struct Ast_Continue {
	Scope* scope;
};

struct Ast {
	Node_Type node;
	Type* return_type;
	bool is_decl;
	union {
		Ast_ProcDecl proc_decl;
		Ast_VarDecl var_decl;
		Ast_StructDecl struct_decl;
		Ast_NamedArgument named_arg;
		Ast_Block block;
		Ast_VarAssign var_assign;
		Ast_If if_stmt;
		Ast_While while_stmt;
		Ast_Return ret_stmt;
		Ast_Break break_stmt;
		Ast_Continue continue_stmt;

		Ast_Expression expression;
	};
};

struct Scope {
	s64 id;
	s32 level;
	Scope* parent;
};

struct Scope_Manager {
	s64 current_id = 0;
};

static Scope_Manager scope_manager = { };

s64 generate_scope_id();
Scope* create_scope(s32 level, Scope* parent);

Ast* create_proc(Memory_Arena* arena, Token* name, Type* return_type, Ast* arguments, int nargs, Ast* body, Scope* scope);
Ast* create_named_argument(Memory_Arena* arena, Token* name, Type* type, Ast* default_val, int index);
Ast* create_variable_decl(Memory_Arena* arena, Token* name, Type* type, Ast* assign_val, Scope* scope);
Ast* create_literal(Memory_Arena* arena, u32 flags, Token* lit_tok);
Ast* create_variable(Memory_Arena* arena, Token* var_token, Scope* scope);
Ast* create_block(Memory_Arena* arena, Scope* scope);
Ast* create_var_assignment(Memory_Arena* arena, Ast* lvalue, Ast* rvalue, AssignmentOperation assign_op, Scope* scope);
Ast* create_proc_call(Memory_Arena* arena, Token* name, Ast** args, Scope* scope);
Ast* create_if(Memory_Arena* arena, Ast* bool_exp, Ast* body, Ast* else_stmt, Scope* scope);
Ast* create_while(Memory_Arena* arena, Ast* bool_exp, Ast* body, Scope* scope);
Ast* create_return(Memory_Arena* arena, Ast* exp, Scope* scope);
Ast* create_unary_expression(Memory_Arena* arena, Ast* operand, UnaryOperation op, u32 flags, Type* cast_type, Precedence precedence, Scope* scope);
Ast* create_binary_operation(Memory_Arena* arena, Ast* left_op, Ast *right_op, Token* op, Precedence precedence, Scope* scope);
Ast* create_break(Memory_Arena* arena, Scope* scope);
Ast* create_continue(Memory_Arena* arena, Scope* scope);

UnaryOperation get_unary_op(Token* token);
BinaryOperation get_binary_op(Token* token);

void block_push_command(Ast* block, Ast* command);
void push_ast_list(Ast** list, Ast* arg);


void DEBUG_print_node(FILE* out, Ast* node);
void DEBUG_print_ast(FILE* out, Ast** ast);