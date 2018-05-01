#include "ast.h"
#include "memory.h"
#include "type_table.h"

static Memory_Arena arena_scope(65536);
static Memory_Arena arena_ast(65536);

#define ALLOC_AST() (Ast*)arena_ast.allocate(sizeof(Ast))
#define ALLOC_SCOPE() (Scope*)arena_scope.allocate(sizeof(Scope))

Scope* scope_create(Ast* creator, Scope* parent, u32 flags) {
	static s64 id = 1;

	Scope* scope = ALLOC_SCOPE();
	scope->id = id++;
	scope->level = parent->level + 1;
	scope->creator_node = creator;
	scope->decl_count = 0;
	scope->flags = flags;
	scope->parent = parent;
	scope->symb_table = { 0 };

	return scope;
}

Ast* ast_create_decl_proc(Token* name, Scope* scope, Scope* arguments_scope, Type_Instance* ptype, Ast** arguments, Ast* body, Type_Instance* type_return, u32 flags, s32 arguments_count) {
	Ast* dp = ALLOC_AST();

	dp->node_type = AST_DECL_PROCEDURE;
	dp->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	dp->scope = scope;
	dp->flags = AST_FLAG_IS_DECLARATION;
	dp->infer_queue_index = -1;

	dp->decl_procedure.name = name;
	dp->decl_procedure.arguments = arguments;
	dp->decl_procedure.body = body;
	dp->decl_procedure.type_return = type_return;
	dp->decl_procedure.flags = flags;
	dp->decl_procedure.arguments_count = arguments_count;
	dp->decl_procedure.extern_library_name = 0;
	dp->decl_procedure.type_procedure = ptype;
	dp->decl_procedure.arguments_scope = arguments_scope;

	dp->decl_procedure.site.filename = name->filename;
	dp->decl_procedure.site.line = name->line;
	dp->decl_procedure.site.column = name->column;

	return dp;
}

Ast* ast_create_decl_variable(Token* name, Scope* scope, Ast* assignment, Type_Instance* var_type, u32 flags) {
	Ast* dv = ALLOC_AST();

	dv->node_type = AST_DECL_VARIABLE;
	dv->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	dv->scope = scope;
	dv->flags = AST_FLAG_IS_DECLARATION;
	dv->infer_queue_index = -1;

	dv->decl_variable.name = name;
	dv->decl_variable.flags = flags;
	dv->decl_variable.size_bytes = 0;
	dv->decl_variable.temporary_register = -1;
	dv->decl_variable.variable_type = var_type;
	dv->decl_variable.assignment = assignment;
	dv->decl_variable.alignment = 8;		// @TEMPORARY

	dv->decl_variable.site.filename = name->filename;
	dv->decl_variable.site.line = name->line;
	dv->decl_variable.site.column = name->column;

	return dv;
}

Ast* ast_create_decl_struct(Token* name, Scope* scope, Scope* struct_scope, Type_Instance* stype, Ast** fields, u32 flags, s32 field_count) {
	Ast* ds = ALLOC_AST();

	ds->node_type = AST_DECL_STRUCT;
	ds->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	ds->scope = scope;
	ds->flags = AST_FLAG_IS_DECLARATION;
	ds->infer_queue_index = -1;

	ds->decl_struct.name = name;
	ds->decl_struct.fields = fields;
	ds->decl_struct.fields_count = field_count;
	ds->decl_struct.flags = flags;
	ds->decl_struct.size_bytes = 0;
	ds->decl_struct.alignment = 8;		// @TEMPORARY
	ds->decl_struct.type_info = stype;
	ds->decl_struct.struct_scope = struct_scope;

	ds->decl_struct.site.filename = name->filename;
	ds->decl_struct.site.line = name->line;
	ds->decl_struct.site.column = name->column;

	return ds;
}

Ast* ast_create_decl_enum(Token* name, Scope* scope, Scope* enum_scope, Ast** fields, Type_Instance* type_hint, u32 flags, s32 field_count) {
	Ast* de = ALLOC_AST();

	de->node_type = AST_DECL_ENUM;
	de->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	de->scope = scope;
	de->flags = AST_FLAG_IS_DECLARATION;
	de->infer_queue_index = -1;

	de->decl_enum.name = name;
	de->decl_enum.fields = fields;
	de->decl_enum.fields_count = field_count;
	de->decl_enum.flags = flags;
	de->decl_enum.type_hint = type_hint;
	de->decl_enum.enum_scope = enum_scope;

	de->decl_enum.site.filename = name->filename;
	de->decl_enum.site.line = name->line;
	de->decl_enum.site.column = name->column;

	return de;
}

Ast* ast_create_decl_constant(Token* name, Scope* scope, Ast* value, Type_Instance* type, u32 flags) {
	Ast* dc = ALLOC_AST();

	dc->node_type = AST_DECL_CONSTANT;
	dc->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	dc->scope = scope;
	dc->flags = AST_FLAG_IS_DECLARATION;
	dc->infer_queue_index = -1;

	dc->decl_constant.name = name;
	dc->decl_constant.flags = flags;
	dc->decl_constant.type_info = type;
	dc->decl_constant.value = value;

	dc->decl_constant.site.filename = name->filename;
	dc->decl_constant.site.line = name->line;
	dc->decl_constant.site.column = name->column;

	return dc;
}

// Expressions
Ast* ast_create_expr_proc_call(Scope* scope, Token* name, Ast** arguments, s32 args_count) {
	Ast* epc = ALLOC_AST();

	epc->node_type = AST_EXPRESSION_PROCEDURE_CALL;
	epc->type_return = 0;
	epc->scope = scope;
	epc->flags = AST_FLAG_IS_EXPRESSION;
	epc->infer_queue_index = -1;

	epc->expr_proc_call.decl = 0;
	epc->expr_proc_call.name = name;
	epc->expr_proc_call.args = arguments;
	epc->expr_proc_call.args_count = args_count;

	return epc;
}

Ast* ast_create_expr_unary(Scope* scope, Ast* operand, Operator_Unary op, Token* op_token, Type_Instance* type_to_cast, u32 flags) {
	Ast* eu = ALLOC_AST();

	eu->node_type = AST_EXPRESSION_UNARY;
	eu->type_return = 0;
	eu->scope = scope;
	eu->flags = AST_FLAG_IS_EXPRESSION;
	eu->infer_queue_index = -1;

	eu->expr_unary.flags = flags;
	eu->expr_unary.op = op;
	eu->expr_unary.operand = operand;

	eu->expr_unary.token_op = op_token;
	eu->expr_unary.type_to_cast = type_to_cast;

	return eu;
}

Ast* ast_create_expr_binary(Scope* scope, Ast* left, Ast* right, Operator_Binary op, Token* op_token) {
	Ast* eb = ALLOC_AST();

	eb->node_type = AST_EXPRESSION_BINARY;
	eb->type_return = 0;
	eb->scope = scope;
	eb->flags = AST_FLAG_IS_EXPRESSION;
	eb->infer_queue_index = -1;

	eb->expr_binary.left  = left;
	eb->expr_binary.right = right;
	eb->expr_binary.op    = op;

	eb->expr_binary.token_op = op_token;

	return eb;
}

Ast* ast_create_expr_variable(Token* name, Scope* scope, Type_Instance* type) {
	Ast* ev = ALLOC_AST();

	ev->node_type = AST_EXPRESSION_VARIABLE;
	ev->type_return = type;
	ev->scope = scope;
	ev->flags = AST_FLAG_IS_EXPRESSION;
	ev->infer_queue_index = -1;

	ev->expr_variable.name = name;
	ev->expr_variable.decl = 0;

	return ev;
}

Ast* ast_create_expr_literal(Scope* scope, Literal_Type literal_type, Token* token, u32 flags, Type_Instance* type) {
	Ast* el = ALLOC_AST();

	el->node_type = AST_EXPRESSION_LITERAL;
	el->type_return = type;
	el->scope = scope;
	el->flags = AST_FLAG_IS_EXPRESSION;
	el->infer_queue_index = -1;

	el->expr_literal.flags = flags;
	el->expr_literal.type = literal_type;
	el->expr_literal.token = token;

	return el;
}

// Commands
Ast* ast_create_comm_block(Scope* parent_scope, Scope* block_scope, Ast** commands, Ast* creator, s32 command_count) {
	Ast* cb = ALLOC_AST();

	cb->node_type = AST_COMMAND_BLOCK;
	cb->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	cb->scope = parent_scope;
	cb->flags = AST_FLAG_IS_COMMAND;
	cb->infer_queue_index = -1;

	cb->comm_block.block_scope = block_scope;
	cb->comm_block.commands = commands;
	cb->comm_block.command_count = command_count;
	cb->comm_block.creator = creator;

	return cb;
}

Ast* ast_create_comm_variable_assignment(Scope* scope, Ast* lvalue, Ast* rvalue) {
	Ast* cva = ALLOC_AST();

	cva->node_type = AST_COMMAND_VARIABLE_ASSIGNMENT;
	cva->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	cva->scope = scope;
	cva->flags = AST_FLAG_IS_COMMAND;
	cva->infer_queue_index = -1;

	cva->comm_var_assign.lvalue = lvalue;
	cva->comm_var_assign.rvalue = rvalue;

	return cva;
}

Ast* ast_create_comm_if(Scope* scope, Ast* condition, Ast* command_true, Ast* command_false) {
	Ast* ci = ALLOC_AST();

	ci->node_type = AST_COMMAND_IF;
	ci->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	ci->scope = scope;
	ci->flags = AST_FLAG_IS_COMMAND;
	ci->infer_queue_index = -1;

	ci->comm_if.condition = condition;
	ci->comm_if.body_true = command_true;
	ci->comm_if.body_false = command_false;

	return ci;
}

Ast* ast_create_comm_for(Scope* scope, Ast* condition, Ast* body) {
	Ast* cf = ALLOC_AST();

	cf->node_type = AST_COMMAND_FOR;
	cf->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	cf->scope = scope;
	cf->flags = AST_FLAG_IS_COMMAND;
	cf->infer_queue_index = -1;

	cf->comm_for.condition = condition;
	cf->comm_for.body = body;
	cf->comm_for.id = -1;

	return cf;
}

Ast* ast_create_comm_break(Scope* scope, Ast* lit, Token* token) {
	Ast* cb = ALLOC_AST();

	cb->node_type = AST_COMMAND_BREAK;
	cb->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	cb->scope = scope;
	cb->flags = AST_FLAG_IS_COMMAND;
	cb->infer_queue_index = -1;

	cb->comm_break.level = lit;
	cb->comm_break.token_break = token;

	return cb;
}

Ast* ast_create_comm_continue(Scope* scope, Token* token) {
	Ast* cc = ALLOC_AST();

	cc->node_type = AST_COMMAND_CONTINUE;
	cc->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	cc->scope = scope;
	cc->flags = AST_FLAG_IS_COMMAND;
	cc->infer_queue_index = -1;

	cc->comm_continue.token_continue = token;

	return cc;
}

Ast* ast_create_comm_return(Scope* scope, Ast* expr, Token* token) {
	Ast* cr = ALLOC_AST();

	cr->node_type = AST_COMMAND_RETURN;
	cr->type_return = type_primitive_get(TYPE_PRIMITIVE_VOID);
	cr->scope = scope;
	cr->flags = AST_FLAG_IS_COMMAND;
	cr->infer_queue_index = -1;

	cr->comm_return.expression = expr;
	cr->comm_return.token_return = token;

	return cr;
}












char* binop_op_to_string(Operator_Binary binop) {
	char* str = 0;
	switch (binop) {
		case OP_BINARY_AND:				str = "&"; break;
		case OP_BINARY_OR:				str = "|"; break;
		case OP_BINARY_PLUS:			str = "+"; break;
		case OP_BINARY_MINUS:			str = "-"; break;
		case OP_BINARY_DIV:				str = "/"; break;
		case OP_BINARY_MULT:			str = "*"; break;
		case OP_BINARY_MOD:				str = "%%"; break;
		case OP_BINARY_EQUAL:			str = "=="; break;
		case OP_BINARY_NOT_EQUAL:		str = "!="; break;
		case OP_BINARY_GE:				str = ">="; break;
		case OP_BINARY_GT:				str = ">"; break;
		case OP_BINARY_LE:				str = "<="; break;
		case OP_BINARY_LT:				str = "<"; break;
		case OP_BINARY_LOGIC_AND:		str = "&&"; break;
		case OP_BINARY_LOGIC_OR:		str = "||"; break;
		case OP_BINARY_SHL:				str = "<<"; break;
		case OP_BINARY_SHR:				str = ">>"; break;
		case OP_BINARY_XOR:				str = "^"; break;
		case OP_BINARY_VECTOR_ACCESS:	str = "[]"; break;
		case OP_BINARY_DOT:				str = "."; break;
		default: str = ""; break;
	}
	return str;
}


#if 1

/***************************************

				DEBUG

****************************************/
static bool print_types = false;

static int DEBUG_indent_level = 0;
void DEBUG_print_indent_level() {
	for (int i = 0; i < DEBUG_indent_level; ++i) {
		fprintf(stdout, "   ");
	}
}

void quick_type(FILE* out, Type_Instance* type) {
	if (!print_types)  return;
	fprintf(out, "<");
	DEBUG_print_type(out, type, true);
	fprintf(out, ">");
}

void DEBUG_print_type(FILE* out, Type_Instance* type, bool short_) {
	if (!type) {
		fprintf(out, "(TYPE_IS_NULL)");
		return;
	}
	if (type->kind == KIND_PRIMITIVE) {
		switch (type->primitive) {
		case TYPE_PRIMITIVE_S64:	fprintf(out, "s64"); break;
		case TYPE_PRIMITIVE_S32:	fprintf(out, "s32"); break;
		case TYPE_PRIMITIVE_S16:	fprintf(out, "s16"); break;
		case TYPE_PRIMITIVE_S8:		fprintf(out, "s8"); break;
		case TYPE_PRIMITIVE_U64:	fprintf(out, "u64"); break;
		case TYPE_PRIMITIVE_U32:	fprintf(out, "u32"); break;
		case TYPE_PRIMITIVE_U16:	fprintf(out, "u16"); break;
		case TYPE_PRIMITIVE_U8:		fprintf(out, "u8"); break;
		case TYPE_PRIMITIVE_BOOL:	fprintf(out, "bool"); break;
		case TYPE_PRIMITIVE_R64:	fprintf(out, "r64"); break;
		case TYPE_PRIMITIVE_R32:	fprintf(out, "r32"); break;
		case TYPE_PRIMITIVE_VOID:	fprintf(out, "void"); break;
		}
	} else if (type->kind == KIND_POINTER) {
		fprintf(out, "^");
		DEBUG_print_type(out, type->pointer_to);
	} else if (type->kind == KIND_STRUCT) {
		if (short_) {
			fprintf(out, "%.*s", TOKEN_STR(type->struct_desc.name));
		} else {
			fprintf(out, "%.*s struct {\n", TOKEN_STR(type->struct_desc.name));
			int num_fields = array_get_length(type->struct_desc.fields_types);
			for (int i = 0; i < num_fields; ++i) {
				Type_Instance* field_type = type->struct_desc.fields_types[i];
				string name = type->struct_desc.fields_names[i];
				fprintf(out, "%.*s : ", name.length, name.data);
				DEBUG_print_type(out, field_type);
				fprintf(out, ";\n");
			}
			fprintf(out, "}\n");
		}
	} else if (type->kind == KIND_FUNCTION) {
		fprintf(out, "(");
		int num_args = type->function_desc.num_arguments;
		for (int i = 0; i < num_args; ++i) {
			DEBUG_print_type(out, type->function_desc.arguments_type[i]);
			if(i + 1 < num_args) fprintf(out, ",");
		}
		fprintf(out, ") -> ");
		DEBUG_print_type(out, type->function_desc.return_type);
	} else if (type->kind == KIND_ARRAY) {
		fprintf(out, "[%llu", type->array_desc.dimension);
		fprintf(out, "]");
		DEBUG_print_type(out, type->array_desc.array_of);
	}

}

void DEBUG_print_block(FILE* out, Ast* block)
{
	size_t num_commands = block->comm_block.command_count;
	quick_type(out, block->type_return);
	fprintf(out, "{\n");
	DEBUG_indent_level += 1;
	for (size_t i = 0; i < num_commands; ++i) {
		DEBUG_print_indent_level();
		if (block->comm_block.commands) {
			DEBUG_print_node(out, block->comm_block.commands[i]);
			Ast_NodeType cmd_type = block->comm_block.commands[i]->node_type;
			if (cmd_type == AST_COMMAND_BREAK || cmd_type == AST_COMMAND_CONTINUE ||
				cmd_type == AST_COMMAND_RETURN || cmd_type == AST_COMMAND_VARIABLE_ASSIGNMENT) {
				fprintf(out, ";");
			}
			fprintf(out, "\n");
		}
	}
	DEBUG_indent_level -= 1;
	DEBUG_print_indent_level();
	fprintf(out, "}");
}

void DEBUG_print_proc_decl(FILE* out, Ast* proc_node) {
	quick_type(out, proc_node->type_return);
	quick_type(out, proc_node->decl_procedure.type_procedure);
	fprintf(out, "%.*s :: (", TOKEN_STR(proc_node->decl_procedure.name));
	for (int i = 0; i < proc_node->decl_procedure.arguments_count; ++i) {
		if(i != 0)
			fprintf(out, ", ");
		Ast* narg = proc_node->decl_procedure.arguments[i];
		fprintf(out, "%.*s : ", TOKEN_STR(narg->decl_variable.name));
		DEBUG_print_type(out, narg->decl_variable.variable_type);
	}
	fprintf(out, ") -> ");
	DEBUG_print_type(out, proc_node->decl_procedure.type_return);
	if (proc_node->decl_procedure.body == 0) {
		fprintf(out, ";");
	} else {
		DEBUG_print_block(out, proc_node->decl_procedure.body);
	}
}

void DEBUG_print_expression(FILE* out, Ast* node) {
	quick_type(out, node->type_return);
	switch (node->node_type) {
	case AST_EXPRESSION_LITERAL: {
		if(!node->type_return) {
			fprintf(out, "<nil>");
			break;
		}
		if(node->type_return->kind == KIND_PRIMITIVE){
			switch(node->type_return->primitive){
				case TYPE_PRIMITIVE_BOOL:	(node->expr_literal.value_bool) ? fprintf(out, "true") : fprintf(out, "false"); break;
				case TYPE_PRIMITIVE_R32:
				case TYPE_PRIMITIVE_R64:	fprintf(out, "%f", node->expr_literal.value_r64); break;
				case TYPE_PRIMITIVE_S8:		fprintf(out, "%lld", (s8)node->expr_literal.value_s64); break;
				case TYPE_PRIMITIVE_S16:	fprintf(out, "%lld", (s16)node->expr_literal.value_s64); break;
				case TYPE_PRIMITIVE_S32:	fprintf(out, "%lld", (s32)node->expr_literal.value_s64); break;
				case TYPE_PRIMITIVE_S64:	fprintf(out, "%lld", (s64)node->expr_literal.value_s64); break;

				case TYPE_PRIMITIVE_U8:		fprintf(out, "%llu", (u8)node->expr_literal.value_u64); break;
				case TYPE_PRIMITIVE_U16:	fprintf(out, "%llu", (u16)node->expr_literal.value_u64); break;
				case TYPE_PRIMITIVE_U32:	fprintf(out, "%llu", (u32)node->expr_literal.value_u64); break;
				case TYPE_PRIMITIVE_U64:	fprintf(out, "%llu", (u64)node->expr_literal.value_u64); break;
				default:					fprintf(out, "<UNSUPPORTED literal>"); break;
			}
		} else {
			switch(node->expr_literal.type){
				case LITERAL_ARRAY:		fprintf(out, "<UNSUPPORTED array literal>"); break;
				case LITERAL_STRUCT:	fprintf(out, "<UNSUPPORTED struct literal>"); break;
				default:				fprintf(out, "<UNSUPPORTED literal>"); break;
			}
		}
		
	} break;
	case AST_EXPRESSION_BINARY: {
		fprintf(out, "(");
		DEBUG_print_expression(out, node->expr_binary.left);
		switch (node->expr_binary.op) {
			case OP_BINARY_PLUS:					fprintf(out, " + "); break;
			case OP_BINARY_MINUS:					fprintf(out, " - "); break;
			case OP_BINARY_MULT:					fprintf(out, " * "); break;
			case OP_BINARY_DIV:						fprintf(out, " / "); break;
			case OP_BINARY_AND:						fprintf(out, " & "); break;
			case OP_BINARY_OR:						fprintf(out, " | "); break;
			case OP_BINARY_XOR:						fprintf(out, " ^ "); break;
			case OP_BINARY_MOD:						fprintf(out, " %% "); break;
			case OP_BINARY_LOGIC_AND:				fprintf(out, " && "); break;
			case OP_BINARY_LOGIC_OR:				fprintf(out, " || "); break;
			case OP_BINARY_SHL:						fprintf(out, " << "); break;
			case OP_BINARY_SHR:						fprintf(out, " >> "); break;
			case OP_BINARY_LT:						fprintf(out, " < "); break;
			case OP_BINARY_GT:						fprintf(out, " > "); break;
			case OP_BINARY_LE:						fprintf(out, " <= "); break;
			case OP_BINARY_GE:						fprintf(out, " >= "); break;
			case OP_BINARY_EQUAL:					fprintf(out, " == "); break;
			case OP_BINARY_NOT_EQUAL:				fprintf(out, " != "); break;
			case OP_BINARY_DOT:						fprintf(out, "."); break;
			case OP_BINARY_VECTOR_ACCESS:			fprintf(out, "["); break;
		}
		DEBUG_print_expression(out, node->expr_binary.right);
		if (node->expr_binary.op == OP_BINARY_VECTOR_ACCESS)
			fprintf(out, "]");
		fprintf(out, ")");
	} break;
	case AST_EXPRESSION_UNARY: {
		fprintf(out, "(");
		if (node->expr_unary.flags & UNARY_EXPR_FLAG_PREFIXED) {
			switch (node->expr_unary.op) {
			case OP_UNARY_CAST: {
				fprintf(out, "cast(");
				DEBUG_print_type(out, node->expr_unary.type_to_cast, true);
				fprintf(out, ")");
			}break;
			case OP_UNARY_ADDRESSOF:	fprintf(out, "&"); break;
			case OP_UNARY_MINUS:		fprintf(out, "-"); break;
			case OP_UNARY_DEREFERENCE:	fprintf(out, "*"); break;
			case OP_UNARY_BITWISE_NOT:	fprintf(out, "~"); break;
			case OP_UNARY_LOGIC_NOT:	fprintf(out, "!"); break;
			case OP_UNARY_PLUS:			fprintf(out, "+"); break;
			}
		} else {
			fprintf(out, "<Unsupported Unary>");
		}
		DEBUG_print_expression(out, node->expr_unary.operand);
		fprintf(out, ")");
	}break;
	case AST_EXPRESSION_VARIABLE: {
		fprintf(out, "%.*s", TOKEN_STR(node->expr_variable.name));
	} break;
	case AST_EXPRESSION_PROCEDURE_CALL: {
		int num_args = 0;
		if (node->expr_proc_call.args) {
			num_args = array_get_length(node->expr_proc_call.args);
		}
		fprintf(out, "%.*s(", TOKEN_STR(node->expr_proc_call.name));
		for (int i = 0; i < num_args; ++i) {
			DEBUG_print_expression(out, node->expr_proc_call.args[i]);
			if(i + 1 != num_args) fprintf(out, ", ");
		}
		fprintf(out, ")");
	} break;
	}
}

void DEBUG_print_expr_cast(FILE* out, Ast* node) {
	assert(0);
}

void DEBUG_print_constant_decl(FILE* out, Ast* node) {
	quick_type(out, node->type_return);
	fprintf(out, "%.*s : ", TOKEN_STR(node->decl_constant.name));
	DEBUG_print_type(out, node->decl_constant.type_info);
	if (node->decl_constant.value) {
		fprintf(out, " : ");
		DEBUG_print_expression(out, node->decl_constant.value);
	}
	fprintf(out, ";\n");
}

void DEBUG_print_var_decl(FILE* out, Ast* node) {
	quick_type(out, node->type_return);
	fprintf(out, "%.*s : ", TOKEN_STR(node->decl_variable.name));
	DEBUG_print_type(out, node->decl_variable.variable_type);
	if (node->decl_variable.assignment) {
		fprintf(out, " = ");
		DEBUG_print_expression(out, node->decl_variable.assignment);
	}
	fprintf(out, ";\n");
}

void DEBUG_print_if_command(FILE* out, Ast* node)
{
	fprintf(out, "if (");
	DEBUG_print_expression(out, node->comm_if.condition);
	fprintf(out, ")");
	if (node->comm_if.body_true->node_type == AST_COMMAND_BLOCK) {
		DEBUG_print_node(out, node->comm_if.body_true);
	} else {
		fprintf(out, "\n");
		DEBUG_indent_level += 1;
		DEBUG_print_indent_level();
		DEBUG_print_node(out, node->comm_if.body_true);
		DEBUG_indent_level -= 1;
	}
	if (node->comm_if.body_false) {
		fprintf(out, "\n");
		DEBUG_print_indent_level();
		fprintf(out, "else");
		if (node->comm_if.body_false->node_type == AST_COMMAND_BLOCK) {
			DEBUG_print_node(out, node->comm_if.body_false);
		} else {
			fprintf(out, "\n");
			DEBUG_indent_level += 1;
			DEBUG_print_indent_level();
			DEBUG_print_node(out, node->comm_if.body_false);
			DEBUG_indent_level -= 1;
		}
	}
}

void DEBUG_print_for_command(FILE* out, Ast* node)
{
	fprintf(out, "for (");
	DEBUG_print_expression(out, node->comm_for.condition);
	fprintf(out, ")");
	DEBUG_print_node(out, node->comm_for.body);
}

void DEBUG_print_return_command(FILE* out, Ast* node)
{
	fprintf(out, "return ");
	if (node->comm_return.expression) {
		DEBUG_print_expression(out, node->comm_return.expression);
	}
}

void DEBUG_print_break_command(FILE* out, Ast* node)
{
	fprintf(out, "break ");
	if (node->comm_break.level > 0)
		DEBUG_print_expression(out, node->comm_break.level);
}

void DEBUG_print_continue_command(FILE* out, Ast* node)
{
	if (print_types) {
		fprintf(out, "<");
		DEBUG_print_type(out, node->type_return, true);
		fprintf(out, ">");
	}
	fprintf(out, "continue");
}

void DEBUG_print_variable_assignment(FILE* out, Ast* node) {
	if (node->comm_var_assign.lvalue) {
		DEBUG_print_expression(out, node->comm_var_assign.lvalue);
		fprintf(out, " = ");
	}
	DEBUG_print_expression(out, node->comm_var_assign.rvalue);
}

void DEBUG_print_struct_decl(FILE* out, Ast* node)
{
	Ast_Decl_Struct* sd = &node->decl_struct;
	fprintf(out, "%.*s :: struct{\n", TOKEN_STR(sd->name));
	DEBUG_indent_level += 1;
	for (int i = 0; i < sd->fields_count; ++i) {
		DEBUG_print_indent_level();
		DEBUG_print_var_decl(out, sd->fields[i]);
	}
	DEBUG_indent_level -= 1;
	fprintf(out, "}\n");
}

void DEBUG_print_enum_decl(FILE* out, Ast* node) {
	Ast_Decl_Enum* ed = &node->decl_enum;
	fprintf(out, "%.*s :", TOKEN_STR(ed->name));
	DEBUG_print_type(out, ed->type_hint);
	fprintf(out, ": enum {\n");
	DEBUG_indent_level += 1;
	for (int i = 0; i < ed->fields_count; ++i) {
		DEBUG_print_constant_decl(out, ed->fields[i]);
		fprintf(out, ",\n");
	}
	DEBUG_indent_level -= 1;
	fprintf(out, "}\n");
}

void DEBUG_print_node(FILE* out, Ast* node) {
	switch (node->node_type) {
		case AST_DECL_PROCEDURE:				DEBUG_print_proc_decl(out, node); break;
		case AST_DECL_VARIABLE:					DEBUG_print_var_decl(out, node); break;
		case AST_DECL_STRUCT:					DEBUG_print_struct_decl(out, node); break;
		case AST_DECL_UNION:					assert(0);
		case AST_DECL_ENUM:						DEBUG_print_enum_decl(out, node); break;
		case AST_DECL_CONSTANT:					DEBUG_print_constant_decl(out, node); break;

		// Commands
		case AST_COMMAND_BLOCK:					DEBUG_print_block(out, node); break;
		case AST_COMMAND_VARIABLE_ASSIGNMENT:	DEBUG_print_variable_assignment(out, node); break;
		case AST_COMMAND_IF:					DEBUG_print_if_command(out, node); break;
		case AST_COMMAND_FOR:					DEBUG_print_for_command(out, node); break;
		case AST_COMMAND_BREAK:					DEBUG_print_break_command(out, node); break;
		case AST_COMMAND_CONTINUE:				DEBUG_print_continue_command(out, node); break;
		case AST_COMMAND_RETURN:				DEBUG_print_return_command(out, node); break;

		// Expressions
		case AST_EXPRESSION_BINARY:
		case AST_EXPRESSION_UNARY:
		case AST_EXPRESSION_LITERAL:
		case AST_EXPRESSION_VARIABLE:
		case AST_EXPRESSION_PROCEDURE_CALL:		DEBUG_print_expression(out, node); break;
	default: {
		fprintf(out, "<Unsupported AST Node>\n");
	}break;
	}
}

void DEBUG_print_ast(FILE* out, Ast** ast, bool print_ts) {
	print_types = print_ts;
	int l = array_get_length(ast);
	for (int i = 0; i < array_get_length(ast); ++i) {
		DEBUG_print_node(out, ast[i]);
		fprintf(out, "\n");
	}
}

#endif
