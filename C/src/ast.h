#pragma once
#include <stdint.h>
#include "lexer.h"

typedef enum {
	AST_UNKNOWN = 0,

	// Declarations
	AST_DECL_PROCEDURE,
	AST_DECL_VARIABLE,
	AST_DECL_CONSTANT,
	AST_DECL_TYPEDEF,

	// Commands
	AST_COMMAND_BLOCK,
	AST_COMMAND_ASSIGNMENT,
	AST_COMMAND_IF,
	AST_COMMAND_FOR,
	AST_COMMAND_WHILE,
	AST_COMMAND_BREAK,
	AST_COMMAND_CONTINUE,
	AST_COMMAND_RETURN,

	// Expressions
	AST_EXPRESSION_BINARY,
	AST_EXPRESSION_UNARY,
	AST_EXPRESSION_LITERAL_PRIMITIVE,
	AST_EXPRESSION_LITERAL_ARRAY,
	AST_EXPRESSION_LITERAL_STRUCT,
	AST_EXPRESSION_VARIABLE,
	AST_EXPRESSION_PROCEDURE_CALL,
	AST_EXPRESSION_DOT,
	AST_EXPRESSION_DIRECTIVE,
} Light_Ast_Type;

typedef enum {
	LITERAL_UNKNOWN = 0,
	LITERAL_DEC_SINT,
	LITERAL_DEC_UINT,
	LITERAL_HEX_INT,
	LITERAL_BIN_INT,
	LITERAL_FLOAT,
	LITERAL_BOOL,
	LITERAL_CHAR,
	LITERAL_POINTER,
} Light_Literal_Type;

typedef enum {
	OP_UNARY_UNKNOWN = 0,
	
	OP_UNARY_PLUS,
	OP_UNARY_MINUS,
	OP_UNARY_DEREFERENCE,
	OP_UNARY_ADDRESSOF,
	OP_UNARY_BITWISE_NOT,
	OP_UNARY_CAST,
	OP_UNARY_LOGIC_NOT,
} Light_Operator_Unary;

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

	OP_BINARY_VECTOR_ACCESS, // []
} Light_Operator_Binary;

// -------------- ----- ----------------
// -------------- Scope ----------------
// -------------- ----- ----------------
typedef enum {
    SCOPE_PROCEDURE_ARGUMENTS = (1 << 0),
    SCOPE_PROCEDURE_BODY      = (1 << 1),
    SCOPE_STRUCTURE           = (1 << 2),
    SCOPE_UNION               = (1 << 3),
    SCOPE_ENUM                = (1 << 4),
    SCOPE_FILESCOPE           = (1 << 5),
    SCOPE_BLOCK               = (1 << 6),
    SCOPE_LOOP                = (1 << 7),
} Light_Scope_Flags;

typedef struct Light_Scope_t {
	struct Light_Scope_t* parent;
	struct Light_Ast_t**  decls;
	union {
		struct Light_Ast_t*   creator_node;
		struct Light_Type_t*  creator_type;
	};

	uint32_t          flags;
	int32_t           id;
	int32_t           level;
	int32_t           decl_count;

	struct Symbol_Table_t*      symb_table;

    // Necessary data for code generation
	int32_t           stack_allocation_offset;
	int32_t           stack_current_offset;
} Light_Scope;

// -------------- ----- ----------------
// --------------  Ast  ----------------
// -------------- ----- ----------------

typedef enum {
	STORAGE_CLASS_REGISTER = 0,
    STORAGE_CLASS_STACK,
    STORAGE_CLASS_DATA_SEGMENT,
} Light_Storage_Class;

// Expressions
typedef struct {
	struct Light_Ast_t* left;
	Light_Token*        identifier;
} Light_Ast_Expr_Dot;

typedef struct {
	struct Light_Ast_t*   left;
	struct Light_Ast_t*   right;
	Light_Token*          token_op;
	Light_Operator_Binary op;
} Light_Ast_Expr_Binary;

typedef struct {
	struct Light_Ast_t*  operand;
	Light_Token*         token_op;
	struct Light_Type_t* type_to_cast;
	Light_Operator_Unary op;
	uint32_t             flags;
} Light_Ast_Expr_Unary;

typedef struct {
	Light_Token*        token;
	Light_Literal_Type  type;
	uint32_t            flags;
	Light_Storage_Class storage_class;
	union {
		uint8_t  value_u8;
		uint16_t value_u16;
		uint32_t value_u32;
		uint64_t value_u64;
		int8_t   value_s8;
		int16_t  value_s16;
		int32_t  value_s32;
		int64_t  value_s64;
		float    value_r32;
		double   value_r64;
		bool     value_bool;
	};
} Light_Ast_Expr_Literal_Primitive;

typedef struct  {
	Light_Token*         token_array;
	struct Light_Ast_t** array_exprs;
	struct Light_Type_t* array_strong_type;
	u8*                  data;
	uint64_t             data_length_bytes;
	bool                 raw_data;
	Light_Storage_Class  storage_class;
} Light_Ast_Expr_Literal_Array;

typedef struct {
	Light_Token*         name;
	Light_Token*         token_struct;
	Light_Scope*         struct_scope;		// Only relevant in nameless structs
	bool                 named;
	union {
		struct Light_Ast_t** struct_exprs;
		struct Light_Ast_t** struct_decls;
	};
	Light_Storage_Class  storage_class;
} Light_Ast_Expr_Literal_Struct;

typedef struct {
	Light_Token*        name;
	struct Light_Ast_t* decl;
} Light_Ast_Expr_Variable;

typedef enum {
	CALLING_CONVENTION_LIGHT,
	CALLING_CONVENTION_C,
} Light_Calling_Convention;

typedef struct {
	struct Light_Ast_t*      caller_expr;
	struct Light_Ast_t**     args;
	int32_t                  arg_count;
	Light_Token*             token;
} Light_Ast_Expr_Proc_Call;

typedef enum {
	EXPR_DIRECTIVE_SIZEOF,	// #sizeof type
	EXPR_DIRECTIVE_TYPEOF,  // #typeof expr
	EXPR_DIRECTIVE_RUN,		// #run expr
	EXPR_DIRECTIVE_COMPILE, // #compile `string`
} Light_Expr_Directive_Type;

typedef struct {
	Light_Expr_Directive_Type type;
	Light_Token*              directive_token;
	union {
		struct Light_Ast_t*  expr;
		struct Light_Type_t* type_expr;
	};
} Light_Ast_Expr_Directive;

// Commands
typedef struct {
	struct Light_Ast_t** commands;
	struct Light_Ast_t** defer_stack;
	Light_Scope*         block_scope;
	int32_t              command_count;
} Light_Ast_Comm_Block;

typedef struct {
	struct Light_Ast_t* lvalue;	// Must be an expression
	struct Light_Ast_t* rvalue;	// Must be an expression
	Light_Token*        op_token;
} Light_Ast_Comm_Assignment;

typedef struct {
	struct Light_Ast_t* condition;	// Must be a (boolean) expression
	struct Light_Ast_t* body_true;	// Must be a command
	struct Light_Ast_t* body_false;	// Must be a command
	Light_Token*        if_token;
} Light_Ast_Comm_If;

typedef struct {
	struct Light_Ast_t* condition;		// Must be a (boolean) expression
	struct Light_Ast_t* body;			// Must be a command
	Light_Token*        while_token;
} Light_Ast_Comm_While;

typedef struct {
	struct Light_Ast_t*  condition;		// Must be a (boolean) expression
	struct Light_Ast_t*  body;			// Must be a command
	struct Light_Ast_t** prologue;		// Array of commands
	struct Light_Ast_t** epilogue;		// Array of commands
	Light_Scope*         for_scope;
	Light_Token*         for_token;
} Light_Ast_Comm_For;

typedef struct {
	struct Light_Ast_t*   level;	    // Must be an int literal [0, MAX_INT]
	s64                   level_value;
	bool                  level_evaluated;
	Light_Token*          token_break;
} Light_Ast_Comm_Break;
typedef struct {
	struct Light_Ast_t*   level;
	s64                   level_value;
	bool                  level_evaluated;
	Light_Token*          token_continue;
} Light_Ast_Comm_Continue;
typedef struct {
	struct Light_Ast_t* expression;		// Must be an expression
	Light_Token*        token_return;
} Light_Ast_Comm_Return;

// Declarations
typedef enum {
    DECL_VARIABLE_FLAG_EXPORTED     = (1 << 0),
    DECL_VARIABLE_FLAG_STRUCT_FIELD = (1 << 1),
	DECL_VARIABLE_FLAG_UNION_FIELD  = (1 << 2),
	DECL_VARIABLE_FLAG_RESOLVED     = (1 << 3),
} Light_Decl_Variable_Flags;

typedef struct {
    Light_Token*         name;
	struct Light_Ast_t*  assignment;		// Must be Expression
    Light_Storage_Class  storage_class;
	struct Light_Type_t* type;

	uint32_t flags;
	int32_t  alignment_bytes;
	int32_t  stack_offset; // Offset from Stack base
	int32_t  field_index;  // Only relevant when DECL_VARIABLE_FLAG_STRUCT_FIELD is set
} Light_Ast_Decl_Variable;

typedef enum {
    DECL_PROC_FLAG_FOREIGN = (1 << 0),
    DECL_PROC_FLAG_MAIN    = (1 << 1),
	DECL_PROC_FLAG_EXTERN  = (1 << 2),
} Light_Decl_Procedure_Flags;

typedef struct {
	Light_Token*              name;
	struct Light_Ast_t**      arguments;       // Must be DECL_VARIABLE
	struct Light_Ast_t*       body;			   // Must be a Light_Command_Block
	struct Light_Type_t*      return_type;     // Type of the procedure return
	struct Light_Type_t*      proc_type;       // Type of the procedure
	Light_Scope*              arguments_scope;

	uint32_t           flags;
	int32_t            argument_count;
	
	Light_Token*       extern_library_name;
} Light_Ast_Decl_Procedure;

typedef struct {
	Light_Token*         name;
	struct Light_Ast_t*  value;
	struct Light_Type_t* type_info;

	uint32_t flags;
} Light_Ast_Decl_Constant;

typedef struct {
	Light_Token*          name;
	struct Light_Type_t*  type_referenced;
} Light_Ast_Decl_Typedef;

typedef struct {
	struct Light_Ast_t** inside_nodes_true;
	struct Light_Ast_t** inside_nodes_false;
	
	bool evaluated;
	bool evaluated_to_bool_value;
} Light_Ast_Directive_Static_If;

typedef struct {
	struct Light_Ast_t** inside_decls;
} Light_Ast_Directive_Foreign;

typedef struct {
	Light_Token*         literal_string;
	struct Light_Ast_t*  result_node;
} Light_Ast_Directive_Compile;

typedef struct {
	Light_Token* import_string;
	bool         is_relative_path;
	const char*  absolute_path;
} Light_Ast_Directive_Import;

typedef enum {
    AST_FLAG_EXPRESSION   = (1 << 0),
    AST_FLAG_COMMAND      = (1 << 1),
    AST_FLAG_DECLARATION  = (1 << 2),
	AST_FLAG_DIRECTIVE    = (1 << 3),
	AST_FLAG_INFER_QUEUED = (1 << 4),
	AST_FLAG_ALLOW_BASE_ENUM = (1 << 5), // This flags allows type inference to not error out if a variable with enum type is seen
	AST_FLAG_EXPRESSION_LVALUE = (1 << 6),
} Light_Ast_Flags;

typedef struct Light_Ast_t {
    Light_Ast_Type kind;

    int32_t  id;
	int32_t  infer_queue_index;
    uint32_t flags;

    struct Light_Type_t* type;
    Light_Scope*         scope_at;

    union {
		// Declarations
        Light_Ast_Decl_Procedure decl_proc;
        Light_Ast_Decl_Variable  decl_variable;
		Light_Ast_Decl_Constant  decl_constant;
		Light_Ast_Decl_Typedef   decl_typedef;

		// Commands
		Light_Ast_Comm_Assignment comm_assignment;
		Light_Ast_Comm_Block      comm_block;
		Light_Ast_Comm_Break      comm_break;
		Light_Ast_Comm_Continue   comm_continue;
		Light_Ast_Comm_Return     comm_return;
		Light_Ast_Comm_For        comm_for;
		Light_Ast_Comm_If         comm_if;
		Light_Ast_Comm_While      comm_while;

		// Expressions
		Light_Ast_Expr_Binary            expr_binary;
		Light_Ast_Expr_Unary             expr_unary;
		Light_Ast_Expr_Literal_Primitive expr_literal_primitive;
		Light_Ast_Expr_Literal_Array     expr_literal_array;
		Light_Ast_Expr_Literal_Struct    expr_literal_struct;
		Light_Ast_Expr_Variable          expr_variable;
		Light_Ast_Expr_Proc_Call         expr_proc_call;
		Light_Ast_Expr_Directive         expr_directive;
		Light_Ast_Expr_Dot               expr_dot;

		// Directives
		Light_Ast_Directive_Foreign   directive_foreign;
		Light_Ast_Directive_Static_If directive_static_if;
		Light_Ast_Directive_Compile   directive_compile;
    };
} Light_Ast;

// -------------- ------- ----------------
// --------------  Types  ----------------
// -------------- ------- ----------------

typedef enum {
    TYPE_KIND_NONE = 0,
    TYPE_KIND_PRIMITIVE,
    TYPE_KIND_POINTER,
    TYPE_KIND_STRUCT,
    TYPE_KIND_UNION,
    TYPE_KIND_ARRAY,
    TYPE_KIND_FUNCTION,
	TYPE_KIND_ENUM,
    TYPE_KIND_ALIAS,
} Light_Type_Kind;

typedef enum {
    TYPE_PRIMITIVE_VOID = 0,
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

    TYPE_PRIMITIVE_COUNT,
} Light_Type_Primitive;

typedef struct {
    struct Light_Type_t* array_of;
	bool                 dimension_evaluated;
	uint64_t             dimension;
	Light_Token*         token_array;
	union {
		struct Light_Ast_t*   const_expr;
	};
} Light_Type_Array;

typedef enum {
    LIGHT_STRUCT_FLAG_PACKED = (1 << 0),
} Light_Struct_Flags;

typedef struct {
	Light_Ast**   fields;			// Must be AST_DECL_VARIABLE
	Light_Scope*  struct_scope;
	int64_t*      offset_bits;
	uint32_t      flags;
	int32_t       size_bits;
	int32_t       fields_count;
	int32_t       alignment_bytes;
} Light_Type_Struct;

typedef struct {
	Light_Ast**   fields;
	Light_Scope*  union_scope;
	uint32_t      flags;
	int32_t       size_bits;
	int32_t       fields_count;
	int32_t       alignment_bytes;
} Light_Type_Union;

typedef struct {
    struct Light_Type_t*  return_type;
	struct Light_Type_t** arguments_type;
    struct {
	    char**   arguments_names;
        int32_t* arguments_names_length;
    };
	int32_t      arguments_count;
} Light_Type_Function;

typedef struct {
	struct Light_Type_t*  type_hint;
	Light_Scope*          enum_scope;
	Light_Ast**           fields;			// Must be CONSTANT_DECL
	s32                   field_count;
	uint32_t              flags;
	int64_t*              evaluated_values;
} Light_Type_Enum;

typedef struct {
    Light_Token* name;
	Light_Scope*          scope;
    struct Light_Type_t*  alias_to;
} Light_Type_Alias;

typedef enum {
    TYPE_FLAG_WEAK          = (1 << 1),
    TYPE_FLAG_INTERNALIZED  = (1 << 2),
    TYPE_FLAG_SIZE_RESOLVED = (1 << 3),
	TYPE_FLAG_UNRESOLVED    = (1 << 4),
} Light_Type_Flags;

typedef struct Light_Type_t{
    Light_Type_Kind kind;
    uint32_t        flags;
    uint32_t        size_bits;

    union {
        Light_Type_Primitive primitive;
        struct Light_Type_t* pointer_to;
        Light_Type_Array     array_info;
        Light_Type_Struct    struct_info;
        Light_Type_Union     union_info;
        Light_Type_Function  function;
		Light_Type_Enum      enumerator;
        Light_Type_Alias     alias;
    };
} Light_Type;

// -------------- --------- ----------------
// -------------- Functions ----------------
// -------------- --------- ----------------

// Scope
Light_Scope* light_scope_new(Light_Ast* creator_node, Light_Scope* parent, uint32_t flags);

// Ast
// Declarations
Light_Ast* ast_new_decl_typedef(Light_Scope* scope, Light_Type* type, Light_Token* name);
Light_Ast* ast_new_decl_variable(Light_Scope* scope, Light_Token* name, Light_Type* type, Light_Ast* expr, Light_Storage_Class storage, u32 flags);
Light_Ast* ast_new_decl_constant(Light_Scope* scope, Light_Token* name, Light_Type* type, Light_Ast* expr, u32 flags);
Light_Ast* ast_new_decl_procedure(Light_Scope* scope, Light_Token* name, Light_Ast* body, Light_Type* return_type, Light_Scope* args_scope, Light_Ast** args, s32 args_count, u32 flags);

// Commands
Light_Ast* ast_new_comm_block(Light_Scope* scope, Light_Ast** commands, s32 command_count, Light_Scope* block_scope);
Light_Ast* ast_new_comm_if(Light_Scope* scope, Light_Ast* condition, Light_Ast* if_true, Light_Ast* if_false, Light_Token* if_token);
Light_Ast* ast_new_comm_while(Light_Scope* scope, Light_Ast* condition, Light_Ast* body, Light_Token* while_token);
Light_Ast* ast_new_comm_for(Light_Scope* scope, Light_Scope* for_scope, Light_Ast* condition, Light_Ast* body, Light_Ast** prologue, Light_Ast** epilogue, Light_Token* for_token);
Light_Ast* ast_new_comm_break(Light_Scope* scope, Light_Token* break_keyword, Light_Ast* level);
Light_Ast* ast_new_comm_continue(Light_Scope* scope, Light_Token* continue_keyword, Light_Ast* level);
Light_Ast* ast_new_comm_return(Light_Scope* scope, Light_Ast* expr, Light_Token* return_token);
Light_Ast* ast_new_comm_assignment(Light_Scope* scope, Light_Ast* lvalue, Light_Ast* rvalue, Light_Token* op_token);

// Expressions
Light_Ast* ast_new_expr_literal_primitive(Light_Scope* scope, Light_Token* token);
Light_Ast* ast_new_expr_literal_primitive_u64(Light_Scope* scope, u64 val);
Light_Ast* ast_new_expr_literal_array(Light_Scope* scope, Light_Token* token, Light_Ast** array_exprs);
Light_Ast* ast_new_expr_literal_struct(Light_Scope* scope, Light_Token* name, Light_Token* token, Light_Ast** struct_exprs, bool named, Light_Scope* struct_scope);
Light_Ast* ast_new_expr_unary(Light_Scope* scope, Light_Ast* operand, Light_Token* op_token, Light_Operator_Unary op);
Light_Ast* ast_new_expr_binary(Light_Scope* scope, Light_Ast* left, Light_Ast* right, Light_Token* op_token, Light_Operator_Binary op);
Light_Ast* ast_new_expr_dot(Light_Scope* scope, Light_Ast* left, Light_Token* identifier);
Light_Ast* ast_new_expr_proc_call(Light_Scope* scope, Light_Ast* caller, Light_Ast** arguments, s32 args_count, Light_Token* op);
Light_Ast* ast_new_expr_variable(Light_Scope* scope, Light_Token* name);

// Utils
bool literal_primitive_evaluate(Light_Ast* p);

// -------------- --------- ----------------
// --------------   Print   ----------------
// -------------- --------- ----------------

typedef enum {
	LIGHT_AST_PRINT_STDOUT = (1 << 0),
	LIGHT_AST_PRINT_STDERR = (1 << 1),
	LIGHT_AST_PRINT_BUFFER = (1 << 2),
	LIGHT_AST_PRINT_EXPR_TYPES = (1 << 3),
} Light_Ast_Print_Flags;

s32 ast_print_node(Light_Ast* node, u32 flags, s32 indent_level);
s32 ast_print_type(Light_Type* type, u32 flags, s32 indent_level);
s32 ast_print_expression(Light_Ast* expr, u32 flags, s32 indent_level);
s32 ast_print(Light_Ast** ast, u32 flags, s32 indent_level);