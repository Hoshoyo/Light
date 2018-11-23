// ----------------------------------------
// ------------ Declarations --------------
// ----------------------------------------

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

struct Ast_Expr_Directive {
	Expr_Directive_Type type;
	Token* token;
	union {
		Ast*           expr;
		Type_Instance* type_expr;
	};
};

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