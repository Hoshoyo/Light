#pragma once
#include "util.h"
#include "lexer.h"
#include "type.h"
#include "memory.h"

struct Ast_ProcDecl;
struct Ast_VarDecl;
struct Ast_StructDecl;
struct Ast_Binary_Exp;
struct Ast_Unary_Exp;
struct Ast_ProcDecl;
struct Ast;

enum Node_Type {
	AST_NODE_PROC_DECLARATION,
	AST_NODE_NAMED_ARGUMENT,
	AST_NODE_VARIABLE_DECL,
	AST_NODE_BINARY_EXPRESSION,
	AST_NODE_UNARY_EXPRESSION,
	AST_NODE_LITERAL_EXPRESSION,
	AST_NODE_VARIABLE_EXPRESSION,
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
	UNARY_OP_DEREFERENCE,
	UNARY_OP_ADDRESS_OF,
	UNARY_OP_VECTOR_ACCESS,
	UNARY_OP_NOT_LOGICAL,
	UNARY_OP_NOT_BITWISE,
	UNARY_OP_CAST,
};

struct Scope {
	u32 scope_id;
};

// declarations
struct Ast_ProcDecl {
	Token* name;
	Ast* arguments;
	Ast* body;
	int num_args;
	Type* proc_ret_type;
};
struct Ast_VarDecl {
	Token* name;
	Ast* assignment;
	Type* type;
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
	Ast* left;
	Ast* right;
};

struct Ast_Unary_Exp {
	UnaryOperation op;
	Ast* operand;
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

const u32 EXPRESSION_TYPE_LITERAL = 1;
const u32 EXPRESSION_TYPE_BINARY = 2;
const u32 EXPRESSION_TYPE_UNARY = 3;
const u32 EXPRESSION_TYPE_VARIABLE = 4;

struct Ast_Expression {
	u32 expression_type;
	union {
		Ast_Binary_Exp binary_exp;
		Ast_Unary_Exp unary_exp;
		Ast_Literal literal_exp;
		Ast_Variable variable_exp;
	};
};

struct Ast_NamedArgument {
	Token* arg_name;
	Type* arg_type;
	Ast* default_value;
	int index;
	Ast* next;
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

		Ast_Expression expression;
	};
};

Ast* create_proc(Memory_Arena* arena, Token* name, Type* return_type, Ast* arguments, int nargs, Ast* body);
Ast* create_named_argument(Memory_Arena* arena, Token* name, Type* type, Ast* default_val, int index);
Ast* create_variable_decl(Memory_Arena* arena, Token* name, Type* type, Ast* assign_val);
Ast* create_literal(Memory_Arena* arena, u32 flags, Token* lit_tok);
Ast* create_variable(Memory_Arena* arena, Token* var_token, Scope* scope);

Ast *create_binary_operation(Memory_Arena* arena, Ast* left_op, Ast *right_op, Token* op);

void DEBUG_print_node(FILE* out, Ast* node);
void DEBUG_print_ast(FILE* out, Ast** ast);