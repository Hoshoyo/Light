#pragma once
#include "symbol_table.h"

#define SITE_FROM_TOKEN(T) {(T)->filename, (T)->line, (T)->column }

enum Ast_NodeType {
	AST_UNKNOWN = 0,
	
	// Data
	AST_DATA,

	// Declarations
	AST_DECL_PROCEDURE,
	AST_DECL_VARIABLE,
	AST_DECL_STRUCT,
	AST_DECL_ENUM,
	AST_DECL_UNION,
	AST_DECL_CONSTANT,
	AST_DECL_TYPEDEF,

	// Commands
	AST_COMMAND_BLOCK,
	AST_COMMAND_VARIABLE_ASSIGNMENT,
	AST_COMMAND_IF,
	AST_COMMAND_FOR,
	AST_COMMAND_BREAK,
	AST_COMMAND_CONTINUE,
	AST_COMMAND_RETURN,

	// Expressions
	AST_EXPRESSION_BINARY,
	AST_EXPRESSION_UNARY,
	AST_EXPRESSION_LITERAL,
	AST_EXPRESSION_VARIABLE,
	AST_EXPRESSION_PROCEDURE_CALL,
	AST_EXPRESSION_DIRECTIVE,

};

enum Literal_Type {
	LITERAL_UNKNOWN = 0,

	LITERAL_SINT,
	LITERAL_UINT,
	LITERAL_HEX_INT,
	LITERAL_BIN_INT,
	LITERAL_FLOAT,
	LITERAL_BOOL,
	LITERAL_CHAR,
	LITERAL_POINTER,

	LITERAL_STRUCT,
	LITERAL_ARRAY,
};

enum Operator_Unary {
	OP_UNARY_UNKNOWN = 0,
	
	OP_UNARY_PLUS,
	OP_UNARY_MINUS,
	OP_UNARY_DEREFERENCE,
	OP_UNARY_ADDRESSOF,
	OP_UNARY_BITWISE_NOT,
	OP_UNARY_CAST,

	OP_UNARY_LOGIC_NOT,
};

enum Operator_Binary {
	OP_BINARY_UNKNOWN = 0,

	OP_BINARY_PLUS,			// +
	OP_BINARY_MINUS,		// -
	OP_BINARY_MULT,			// *
	OP_BINARY_DIV,			// /
	OP_BINARY_MOD,			// %
	OP_BINARY_AND,			// &
	OP_BINARY_OR,			// |
	OP_BINARY_XOR,			// ^
	OP_BINARY_SHL,			// <<
	OP_BINARY_SHR,			// >>

	OP_BINARY_LT,			// <
	OP_BINARY_GT,			// >
	OP_BINARY_LE,			// <=
	OP_BINARY_GE,			// >=
	OP_BINARY_EQUAL,		// ==
	OP_BINARY_NOT_EQUAL,	// !=

	OP_BINARY_LOGIC_AND,	// &&
	OP_BINARY_LOGIC_OR,		// ||

	OP_BINARY_DOT,			 // .
	OP_BINARY_VECTOR_ACCESS, // []
};

enum Data_Type {
	GLOBAL_STRING,
};

enum Precedence {
	PRECEDENCE_0 = 0,	//
	PRECEDENCE_1 = 1,	//	 || &&
	PRECEDENCE_2 = 2,	//	 == >= <= != > <
	PRECEDENCE_3 = 3,	//	 ^ | & >> <<
	PRECEDENCE_4 = 4,	//	 + -
	PRECEDENCE_5 = 5,	//	 * / %
	PRECEDENCE_6 = 6,	//	 &(addressof) ~ 
	PRECEDENCE_7 = 7,	//	 *(dereference)	cast !
	PRECEDENCE_8 = 8,	//   []
	PRECEDENCE_9 = 9,	//	 .
	PRECEDENCE_MAX,		//   lit, variables, proc_calls
};

struct Symbol_Table;
struct Ast;
struct Token;
struct Type_Instance;

const u32 SCOPE_PROCEDURE_ARGUMENTS = FLAG(0);
const u32 SCOPE_PROCEDURE_BODY      = FLAG(1);
const u32 SCOPE_STRUCTURE           = FLAG(2);
const u32 SCOPE_UNION               = FLAG(3);
const u32 SCOPE_ENUM                = FLAG(4);
const u32 SCOPE_FILESCOPE           = FLAG(5);
const u32 SCOPE_BLOCK               = FLAG(6);
const u32 SCOPE_LOOP                = FLAG(7);
struct Scope {
	s32           id;
	s32           level;
	s32           decl_count;
	u32           flags;
	s32           stack_allocation_offset;
	Symbol_Table  symb_table;
	Scope*        parent;
	union {
		Ast*   creator_node;
		string filename;
	};
};

struct Site {
	string filename;
	s32    line;
	s32    column;
};

// ----------------------------------------
// ------------ Declarations --------------
// ----------------------------------------

const u32 DECL_PROC_FLAG_FOREIGN = FLAG(0);
const u32 DECL_PROC_FLAG_MAIN = FLAG(1);
struct Ast_Decl_Procedure {
	Token*         name;
	Ast**          arguments;		// DECL_VARIABLE
	Ast*           body;			// COMMAND_BLOCK
	Type_Instance* type_return;
	Type_Instance* type_procedure;
	Scope*         arguments_scope;

	Site   site;

	u32    flags;
	s32    arguments_count;

	u64*   proc_runtime_address;
	
	Token* extern_library_name;
};

const u32 DECL_VARIABLE_STACK = FLAG(0);
const u32 DECL_VARIABLE_DATA_SEGMENT = FLAG(1);
const u32 DECL_VARIABLE_STRUCT_FIELD = FLAG(2);
struct Ast_Decl_Variable {
	Token*         name;
	Ast*           assignment;		// EXPRESSION
	Type_Instance* variable_type;

	Site site;

	u32 flags;
	s32 size_bytes;
	s32 alignment;
	u32 temporary_register;
	s32 stack_offset;
	s32 field_index;
};

const u32 STRUCT_FLAG_PACKED = FLAG(1);
struct Ast_Decl_Struct {
	Token*         name;
	Ast**          fields;			// DECL_VARIABLE
	Type_Instance* type_info;
	Scope*         struct_scope;

	Site site;

	u32 flags;
	s32 fields_count;
	s32 alignment;
	s64 size_bytes;
};
struct Ast_Decl_Union {
	Token* name;
	Ast** fields;
	Type_Instance* type_info;
	Scope* union_scope;

	Site site;

	u32 flags;
	s32 fields_count;
	s32 alignment;
	s64 size_bytes;
};

struct Ast_Decl_Enum {
	Token*         name;
	Ast**          fields;			// DECL_CONSTANT
	Type_Instance* type_hint;
	Scope*         enum_scope;

	Site site;

	u32 flags;
	s32 fields_count;
};
struct Ast_Decl_Constant {
	Token*         name;
	Ast*           value;		// LITERAL | CONSTANT
	Type_Instance* type_info;

	Site site;

	u32 flags;
};

struct Ast_Decl_Typedef {
	Token*         name;
	Type_Instance* type;

	Site site;
};

// ----------------------------------------
// -------------- Commands ----------------
// ----------------------------------------

struct Ast_Comm_Block {
	Ast**  commands;	// COMMANDS
	Scope* block_scope;
	Ast*   creator;
	s32    command_count;
};
struct Ast_Comm_VariableAssign {
	Ast*   lvalue;	// EXPRESSION
	Ast*   rvalue;	// EXPRESSION
};
struct Ast_Comm_If {
	Ast* condition;		// EXPRESSION (boolean)
	Ast* body_true;		// COMMAND
	Ast* body_false;	// COMMAND
};
struct Ast_Comm_For {
	Ast* condition;		// EXPRESSION (boolean)
	Ast* body;			// COMMAND
	s64  id;
	s64  deferred_commands;
};
struct Ast_Comm_Break {
	Ast*   level;			// INT LITERAL [0, MAX_INT]
	Token* token_break;
};
struct Ast_Comm_Continue {
	Token* token_continue;
};
struct Ast_Comm_Return {
	Ast*   expression;	// EXPRESSION
	Token* token_return;
};

// ----------------------------------------
// ------------- Expressions --------------
// ----------------------------------------

struct Ast_Expr_Binary {
	Ast* left;
	Ast* right;
	Token*          token_op;
	Operator_Binary op;
};

const u32 UNARY_EXPR_FLAG_PREFIXED  = FLAG(0);
const u32 UNARY_EXPR_FLAG_POSTFIXED = FLAG(1);
struct Ast_Expr_Unary {
	Ast*           operand;
	Token*         token_op;
	Operator_Unary op;
	Type_Instance* type_to_cast;
	u32            flags;
};

const u32 LITERAL_FLAG_STRING = FLAG(0);
struct Ast_Expr_Literal {
	Token*       token;
	Literal_Type type;
	u32 flags;
	union {
		u64 value_u64;
		s64 value_s64;

		r32 value_r32;
		r64 value_r64;

		bool value_bool;

		Ast**  struct_exprs;
		struct {
			Ast** array_exprs;
			Type_Instance* array_strong_type;
		};
	};
};

struct Ast_Expr_Variable {
	Token* name;
	Ast*   decl;
};
struct Ast_Expr_ProcCall {
	Ast*   caller;
	Ast**  args;		// EXPRESSIONS
	s32    args_count;
};

struct Ast_Data {
	Data_Type type;
	u8*       data;
	s64       length_bytes;
	Token*    location;
	Type_Instance* data_type;
	s32       id;
};

enum Expr_Directive_Type {
	EXPR_DIRECTIVE_SIZEOF,	// #sizeof type
	EXPR_DIRECTIVE_TYPEOF,  // #typeof expr
	EXPR_DIRECTIVE_RUN,		// #run expr
};
struct Ast_Expr_Directive {
	Expr_Directive_Type type;
	Token* token;
	union {
		Ast*           expr;
		Type_Instance* type_expr;
	};
};

const u32 AST_FLAG_IS_DECLARATION = FLAG(0);
const u32 AST_FLAG_IS_COMMAND     = FLAG(1);
const u32 AST_FLAG_IS_EXPRESSION  = FLAG(2);
const u32 AST_FLAG_IS_DATA        = FLAG(3);
const u32 AST_FLAG_QUEUED         = FLAG(4);
const u32 AST_FLAG_LVALUE         = FLAG(5);
const u32 AST_FLAG_ENUM_ACCESSOR  = FLAG(6);
const u32 AST_FLAG_IS_DIRECTIVE   = FLAG(7);
const u32 AST_FLAG_LEFT_ASSIGN    = FLAG(8);
//const u32 AST_FLAG_FAILED_TYPE_CHECK = FLAG(6);

struct Ast {
	Ast_NodeType   node_type;
	Type_Instance* type_return;
	Scope*         scope;
	
	s64 infer_queue_index;
	u32 flags;

	union {
		Ast_Decl_Procedure      decl_procedure;
		Ast_Decl_Variable       decl_variable;
		Ast_Decl_Struct         decl_struct;
		Ast_Decl_Union			decl_union;
		Ast_Decl_Enum           decl_enum;
		Ast_Decl_Constant       decl_constant;
		Ast_Decl_Typedef		decl_typedef;

		Ast_Comm_Block          comm_block;
		Ast_Comm_VariableAssign comm_var_assign;
		Ast_Comm_If             comm_if;
		Ast_Comm_For            comm_for;
		Ast_Comm_Break          comm_break;
		Ast_Comm_Continue		comm_continue;
		Ast_Comm_Return         comm_return;

		Ast_Expr_Binary         expr_binary;
		Ast_Expr_Unary          expr_unary;
		Ast_Expr_Literal        expr_literal;
		Ast_Expr_Variable       expr_variable;
		Ast_Expr_ProcCall       expr_proc_call;

		Ast_Expr_Directive      expr_directive;

		Ast_Data                data_global;
	};

	s32 unique_id;
};

Scope* scope_create(Ast* creator, Scope* parent, u32 flags);

Ast* ast_create_expr_sizeof(Type_Instance* type, Scope* scope, Token* directive_token);
Ast* ast_create_expr_typeof(Ast* expr, Scope* scope, Token* directive_token);
Ast* ast_create_expr_run(Scope* scope, Token* directive_token, Ast* expr);

Ast* ast_create_data(Data_Type type, Scope* scope, Token* location, u8* data, s64 length_bytes, Type_Instance* data_type);

Ast* ast_create_decl_proc(Token* name, Scope* scope, Scope* arguments_scope, Type_Instance* ptype, Ast** arguments, Ast* body, Type_Instance* type_return, u32 flags, s32 arguments_count);
Ast* ast_create_decl_variable(Token* name, Scope* scope, Ast* assignment, Type_Instance* var_type, u32 flags);
Ast* ast_create_decl_struct(Token* name, Scope* scope, Scope* struct_scope, Type_Instance* stype, Ast** fields, u32 flags, s32 field_count);
Ast* ast_create_decl_union(Token* name, Scope* scope, Scope* union_scope, Type_Instance* utype, Ast** fields, u32 flags, s32 field_count);
Ast* ast_create_decl_enum(Token* name, Scope* scope, Scope* enum_scope, Ast** fields, Type_Instance* type_hint, u32 flags, s32 field_count);
Ast* ast_create_decl_constant(Token* name, Scope* scope, Ast* value, Type_Instance* type, u32 flags);
Ast* ast_create_decl_typedef(Token* name, Scope* scope, Type_Instance* type);

Ast* ast_create_expr_variable(Token* name, Scope* scope, Type_Instance* type);
Ast* ast_create_expr_literal(Scope* scope, Literal_Type literal_type, Token* token, u32 flags, Type_Instance* type);
Ast* ast_create_expr_binary(Scope* scope, Ast* left, Ast* right, Operator_Binary op, Token* token_op);
Ast* ast_create_expr_proc_call(Scope* scope, Ast* caller, Ast** arguments, s32 args_count);
Ast* ast_create_expr_unary(Scope* scope, Ast* operand, Operator_Unary op, Token* token_op, Type_Instance* type_to_cast, u32 flags);

Ast* ast_create_comm_block(Scope* parent_scope, Scope* block_scope, Ast** commands, Ast* creator, s32 command_count);
Ast* ast_create_comm_if(Scope* scope, Ast* condition, Ast* command_true, Ast* command_false);
Ast* ast_create_comm_for(Scope* scope, Ast* condition, Ast* body, s64 deferred_commands);
Ast* ast_create_comm_break(Scope* scope, Ast* lit, Token* token);
Ast* ast_create_comm_continue(Scope* scope, Token* token);
Ast* ast_create_comm_return(Scope* scope, Ast* expr, Token* token);
Ast* ast_create_comm_variable_assignment(Scope* scope, Ast* lvalue, Ast* rvalue);

char* binop_op_to_string(Operator_Binary binop);
void DEBUG_print_node(FILE* out, Ast* node);
void DEBUG_print_ast(FILE* out, Ast** ast, bool print_ts);
int  DEBUG_print_type(FILE* out, Type_Instance* type, bool short_ = true);
int  DEBUG_print_type_detailed(FILE* out, Type_Instance* type);