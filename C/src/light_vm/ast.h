#pragma once
#define FLAG(X) (1 << (X))

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef int bool;
typedef float r32;
typedef double r64;

typedef struct {
	s64 length;
	s64 capacity;
	u8* data;
} string;

typedef enum
{
	TOKEN_END_OF_STREAM = 0,
	TOKEN_UNKNOWN = 1,

	// Symbols
	TOKEN_SYMBOL_NOT = '!',				// 33
	TOKEN_SYMBOL_POUND = '#',			// 35
	TOKEN_SYMBOL_DOLLAR = '$',			// 36
	TOKEN_SYMBOL_MOD = '%',				// 37
	TOKEN_SYMBOL_AND = '&',				// 38
	TOKEN_SYMBOL_OPEN_PAREN = '(',		// 40
	TOKEN_SYMBOL_CLOSE_PAREN = ')',		// 41
	TOKEN_SYMBOL_TIMES = '*',			// 42
	TOKEN_SYMBOL_PLUS = '+',			// 43
	TOKEN_SYMBOL_COMMA = ',',			// 44
	TOKEN_SYMBOL_MINUS = '-',			// 45
	TOKEN_SYMBOL_DOT = '.',				// 46
	TOKEN_SYMBOL_DIV = '/',				// 47
	TOKEN_SYMBOL_COLON = ':',			// 58
	TOKEN_SYMBOL_SEMICOLON = ';',		// 59
	TOKEN_SYMBOL_LESS = '<',			// 60
	TOKEN_SYMBOL_EQUAL = '=',			// 61
	TOKEN_SYMBOL_GREATER = '>',			// 62
	TOKEN_SYMBOL_INTERROGATION = '?',	// 63
	TOKEN_SYMBOL_AT = '@',				// 64
	TOKEN_SYMBOL_OPEN_BRACE = '{',		// 91
	TOKEN_SYMBOL_CLOSE_BRACE = '}',		// 93
	TOKEN_SYMBOL_CARAT = '^',			// 94
	TOKEN_SYMBOL_BACK_TICK = '`',		// 96
	TOKEN_SYMBOL_OPEN_BRACKET = '[',	// 123
	TOKEN_SYMBOL_PIPE = '|',			// 124
	TOKEN_SYMBOL_CLOSE_BRACKET = ']',	// 125
	TOKEN_SYMBOL_TILDE = '~',			// 126

	// Literal tokens
	TOKEN_LITERAL_INT = 127,
	TOKEN_LITERAL_HEX_INT,
	TOKEN_LITERAL_BIN_INT,
	TOKEN_LITERAL_FLOAT,
	TOKEN_LITERAL_CHAR,
	TOKEN_LITERAL_STRING,
	TOKEN_LITERAL_BOOL_TRUE,
	TOKEN_LITERAL_BOOL_FALSE,

	TOKEN_IDENTIFIER,

	// Operation tokens
	TOKEN_SYMBOL,
	TOKEN_ARROW,
	TOKEN_EQUAL_COMPARISON,
	TOKEN_LESS_EQUAL,
	TOKEN_GREATER_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_LOGIC_OR,
	TOKEN_LOGIC_AND,
	TOKEN_BITSHIFT_LEFT,
	TOKEN_BITSHIFT_RIGHT,

	TOKEN_PLUS_EQUAL,
	TOKEN_MINUS_EQUAL,
	TOKEN_TIMES_EQUAL,
	TOKEN_DIV_EQUAL,
	TOKEN_MOD_EQUAL,
	TOKEN_AND_EQUAL,
	TOKEN_OR_EQUAL,
	TOKEN_XOR_EQUAL,
	TOKEN_SHL_EQUAL,
	TOKEN_SHR_EQUAL,

	//TOKEN_KEYWORD_CAST,

	// Type tokens
	TOKEN_SINT64,
	TOKEN_SINT32,
	TOKEN_SINT16,
	TOKEN_SINT8,
	TOKEN_UINT64,
	TOKEN_UINT32,
	TOKEN_UINT16,
	TOKEN_UINT8,
	TOKEN_REAL32,
	TOKEN_REAL64,
	TOKEN_BOOL,
	TOKEN_VOID,

	// Reserved words
	TOKEN_KEYWORD_IF,
	TOKEN_KEYWORD_ELSE,
	TOKEN_KEYWORD_FOR,
	TOKEN_KEYWORD_WHILE,
	TOKEN_KEYWORD_BREAK,
	TOKEN_KEYWORD_CONTINUE,
	TOKEN_KEYWORD_RETURN,
	TOKEN_KEYWORD_ENUM,
	TOKEN_KEYWORD_STRUCT,
	TOKEN_KEYWORD_UNION,
	TOKEN_KEYWORD_ARRAY,
	TOKEN_KEYWORD_NULL,
} Token_Type;

enum {
	TOKEN_FLAG_RESERVED_WORD = FLAG(0),
	TOKEN_FLAG_NUMERIC_LITERAL = FLAG(1),
	TOKEN_FLAG_LITERAL = FLAG(2),
	TOKEN_FLAG_BINARY_OPERATOR = FLAG(3),
	TOKEN_FLAG_UNARY_OPERATOR = FLAG(4),
	TOKEN_FLAG_UNARY_PREFIXED = FLAG(5),
	TOKEN_FLAG_UNARY_POSTFIXED = FLAG(6),
	TOKEN_FLAG_PRIMITIVE_TYPE = FLAG(7),
	TOKEN_FLAG_ASSIGNMENT_OPERATOR = FLAG(8),
	TOKEN_FLAG_INTEGER_LITERAL = FLAG(9),
};

typedef struct {
	Token_Type type;

	s32 line;
	s32 column;
	s32 offset_in_file;
	s32 real_string_length;
	u32 flags;

	string filename;
	string value;
} Token;

// Symbol Table
typedef struct {
	Token* identifier;
	struct Ast_t*   decl_node;
	bool   occupied;
} Symbol_Table_Entry;
typedef struct {
	s64 entries_count;
	s64 entries_capacity;
	Symbol_Table_Entry *entries;
} Symbol_Table;


// Type information

typedef enum {
	KIND_UNKNOWN = 0,

	KIND_PRIMITIVE,
	KIND_POINTER,
	KIND_STRUCT,
	KIND_UNION,
	KIND_ARRAY,
	KIND_FUNCTION,

	KIND_ALIAS,
} Type_Kind;

typedef enum {
	TYPE_UNKNOWN = 0,

	TYPE_PRIMITIVE_S8,
	TYPE_PRIMITIVE_S16,
	TYPE_PRIMITIVE_S32,
	TYPE_PRIMITIVE_S64,
	TYPE_PRIMITIVE_U8,
	TYPE_PRIMITIVE_U16,
	TYPE_PRIMITIVE_U32,
	TYPE_PRIMITIVE_U64,
	TYPE_PRIMITIVE_R32,
	TYPE_PRIMITIVE_R64,
	TYPE_PRIMITIVE_BOOL,
	TYPE_PRIMITIVE_VOID,
} Type_Primitive;

typedef struct {
	struct Type_Instance_t* array_of;
	bool dimension_evaluated;
	union {
		u64    dimension;
		Token* constant_dimension_name;
	};
} Type_Array;

typedef struct {
	Token*          name;
	struct Type_Instance_t** fields_types;
	string*         fields_names;
	s32             fields_count;
	s32             alignment;
} Type_Union;

typedef struct {
	Token*          name;
	struct Type_Instance_t** fields_types;
	string*         fields_names;
	s64*            offset_bits;
	s32             fields_count;
	s32             alignment;
} Type_Struct;

typedef struct {
	struct Type_Instance_t*  return_type;
	struct Type_Instance_t** arguments_type;
	string*         arguments_names;
	s32             num_arguments;
} Type_Function;

typedef struct {
	Token* name;
} Type_Alias;

enum {
	// Expected state of node from the outside, another node can only infer its type from this
	// instance if the type is internalized, otherwise wait and depend on it in the infer queue.
	TYPE_FLAG_INTERNALIZED = FLAG(1),

	// When the type size is resolved this flag is set, can only be set by the internalizer node
	TYPE_FLAG_SIZE_RESOLVED = FLAG(2),

	// Indicates if a type is lvalue, meaning it has an address linked to it
	//const u32 TYPE_FLAG_LVALUE = FLAG(3);
	TYPE_FLAG_WEAK   = FLAG(4),
	TYPE_FLAG_STRONG = FLAG(5),
};
typedef struct Type_Instance_t {
	Type_Kind kind;
	u32 flags;
	union {
		s32 type_queue_index;
		s32 type_table_index;
	};
	s64 type_size_bits;
	union {
		Type_Primitive primitive;
		struct Type_Instance_t* pointer_to;
		Type_Array     array_desc;
		Type_Struct    struct_desc;
		Type_Union     union_desc;
		Type_Function  function_desc;
		Type_Alias     alias;
	};
	u8* offset_datasegment;
} Type_Instance;


// AST

typedef enum {
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

} Ast_NodeType;

typedef enum {
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
} Literal_Type;

typedef enum {
	OP_UNARY_UNKNOWN = 0,
	
	OP_UNARY_PLUS,
	OP_UNARY_MINUS,
	OP_UNARY_DEREFERENCE,
	OP_UNARY_ADDRESSOF,
	OP_UNARY_BITWISE_NOT,
	OP_UNARY_CAST,

	OP_UNARY_LOGIC_NOT,
} Operator_Unary;

typedef enum {
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
} Operator_Binary;

typedef enum {
	GLOBAL_STRING,
} Data_Type;

enum {
	SCOPE_PROCEDURE_ARGUMENTS = FLAG(0),
	SCOPE_PROCEDURE_BODY      = FLAG(1),
	SCOPE_STRUCTURE           = FLAG(2),
	SCOPE_UNION               = FLAG(3),
	SCOPE_ENUM                = FLAG(4),
	SCOPE_FILESCOPE           = FLAG(5),
	SCOPE_BLOCK               = FLAG(6),
	SCOPE_LOOP                = FLAG(7),
};

typedef struct Scope_t {
	s32           id;
	s32           level;
	s32           decl_count;
	u32           flags;
	s32           stack_allocation_offset;
	Symbol_Table  symb_table;
	struct Scope_t*        parent;
	union {
		struct Ast_t*   creator_node;
		string filename;
	};
} Scope;

typedef struct {
	string filename;
	s32    line;
	s32    column;
} Site;

// ----------------------------------------
// ------------ Declarations --------------
// ----------------------------------------

enum {
	DECL_PROC_FLAG_FOREIGN = FLAG(0),
	DECL_PROC_FLAG_MAIN = FLAG(1),
};

typedef struct {
	Token*         name;
	struct Ast_t**          arguments;		// DECL_VARIABLE
	struct Ast_t*           body;			// COMMAND_BLOCK
	Type_Instance* type_return;
	Type_Instance* type_procedure;
	Scope*         arguments_scope;

	Site   site;

	u32    flags;
	s32    arguments_count;

	u64*   proc_runtime_address;
	
	Token* extern_library_name;
} Ast_Decl_Procedure;

enum {
	DECL_VARIABLE_STACK = FLAG(0),
	DECL_VARIABLE_DATA_SEGMENT = FLAG(1),
	DECL_VARIABLE_STRUCT_FIELD = FLAG(2),
};
typedef struct {
	Token*         name;
	struct Ast_t*           assignment;		// EXPRESSION
	Type_Instance* variable_type;

	Site site;

	u32 flags;
	s32 size_bytes;
	s32 alignment;
	u32 temporary_register;
	s32 stack_offset;
	s32 field_index;
} Ast_Decl_Variable;

enum {
	STRUCT_FLAG_PACKED = FLAG(1),
};
typedef struct {
	Token*         name;
	struct Ast_t**          fields;			// DECL_VARIABLE
	Type_Instance* type_info;
	Scope*         struct_scope;

	Site site;

	u32 flags;
	s32 fields_count;
	s32 alignment;
	s64 size_bytes;
} Ast_Decl_Struct;

typedef struct {
	Token* name;
	struct Ast_t** fields;
	Type_Instance* type_info;
	Scope* union_scope;

	Site site;

	u32 flags;
	s32 fields_count;
	s32 alignment;
	s64 size_bytes;
} Ast_Decl_Union;

typedef struct {
	Token*         name;
	struct Ast_t**          fields;			// DECL_CONSTANT
	Type_Instance* type_hint;
	Scope*         enum_scope;

	Site site;

	u32 flags;
	s32 fields_count;
} Ast_Decl_Enum;

typedef struct {
	Token*         name;
	struct Ast_t*           value;		// LITERAL | CONSTANT
	Type_Instance* type_info;

	Site site;

	u32 flags;
} Ast_Decl_Constant;

typedef struct {
	Token*         name;
	Type_Instance* type;

	Site site;
} Ast_Decl_Typedef;

// ----------------------------------------
// -------------- Commands ----------------
// ----------------------------------------

typedef struct {
	struct Ast_t**  commands;	// COMMANDS
	Scope* block_scope;
	struct Ast_t*   creator;
	s32    command_count;
} Ast_Comm_Block;

typedef struct {
	struct Ast_t*   lvalue;	// EXPRESSION
	struct Ast_t*   rvalue;	// EXPRESSION
} Ast_Comm_VariableAssign;

typedef struct {
	struct Ast_t* condition;		// EXPRESSION (boolean)
	struct Ast_t* body_true;		// COMMAND
	struct Ast_t* body_false;	// COMMAND
} Ast_Comm_If;

typedef struct {
	struct Ast_t* condition;		// EXPRESSION (boolean)
	struct Ast_t* body;			// COMMAND
	s64  id;
	s64  deferred_commands;
} Ast_Comm_For;

typedef struct {
	struct Ast_t*   level;			// INT LITERAL [0, MAX_INT]
	Token* token_break;
} Ast_Comm_Break;

typedef struct {
	Token* token_continue;
} Ast_Comm_Continue;

typedef struct {
	struct Ast_t*   expression;	// EXPRESSION
	Token* token_return;
} Ast_Comm_Return;

// ----------------------------------------
// ------------- Expressions --------------
// ----------------------------------------

typedef struct {
	struct Ast_t* left;
	struct Ast_t* right;
	Token*          token_op;
	Operator_Binary op;
} Ast_Expr_Binary;

enum {
	UNARY_EXPR_FLAG_PREFIXED  = FLAG(0),
	UNARY_EXPR_FLAG_POSTFIXED = FLAG(1),
};

typedef struct {
	struct Ast_t*  operand;
	Token*         token_op;
	Operator_Unary op;
	Type_Instance* type_to_cast;
	u32            flags;
} Ast_Expr_Unary;

enum {
	LITERAL_FLAG_STRING = FLAG(0),
};

typedef struct {
	Token*       token;
	Literal_Type type;
	u32 flags;
	union {
		u64 value_u64;
		s64 value_s64;

		r32 value_r32;
		r64 value_r64;

		bool value_bool;

		struct Ast_t**  struct_exprs;
		struct {
			struct Ast_t** array_exprs;
			Type_Instance* array_strong_type;
		};
	};
} Ast_Expr_Literal;

typedef struct {
	Token* name;
	struct Ast_t*   decl;
} Ast_Expr_Variable;

typedef struct {
	struct Ast_t*   caller;
	struct Ast_t**  args;		// EXPRESSIONS
	s32    args_count;
} Ast_Expr_ProcCall;

typedef struct  {
	Data_Type type;
	u8*       data;
	s64       length_bytes;
	Token*    location;
	Type_Instance* data_type;
	s32       id;
} Ast_Data;

typedef enum  {
	EXPR_DIRECTIVE_SIZEOF,	// #sizeof type
	EXPR_DIRECTIVE_TYPEOF,  // #typeof expr
	EXPR_DIRECTIVE_RUN,		// #run expr
} Expr_Directive_Type;

typedef struct {
	Expr_Directive_Type type;
	Token* token;
	union {
		struct Ast_t*  expr;
		Type_Instance* type_expr;
	};
} Ast_Expr_Directive;

enum {
	AST_FLAG_IS_DECLARATION = FLAG(0),
	AST_FLAG_IS_COMMAND     = FLAG(1),
	AST_FLAG_IS_EXPRESSION  = FLAG(2),
	AST_FLAG_IS_DATA        = FLAG(3),
	AST_FLAG_QUEUED         = FLAG(4),
	AST_FLAG_LVALUE         = FLAG(5),
	AST_FLAG_ENUM_ACCESSOR  = FLAG(6),
	AST_FLAG_IS_DIRECTIVE   = FLAG(7),
	AST_FLAG_LEFT_ASSIGN    = FLAG(8),
};

typedef struct {
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
} Ast;