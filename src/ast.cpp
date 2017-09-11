#include "ast.h"
#include "parser.h"
#include "type.h"
#include "symbol_table.h"

#define ALLOC_AST(A) (Ast*)A->allocate(sizeof(Ast))
//#define ALLOC_AST(A) (Ast*)malloc(sizeof(Ast))

UnaryOperation get_unary_op(Token* token)
{
	Token_Type t = token->type;
	switch (t) {
	case '-':			return UNARY_OP_MINUS;
	case '+':			return UNARY_OP_PLUS;
	case '*':			return UNARY_OP_DEREFERENCE;
	case '&':			return UNARY_OP_ADDRESS_OF;
	case '[':			return UNARY_OP_VECTOR_ACCESS;
	case '~':			return UNARY_OP_NOT_BITWISE;
	case '!':			return UNARY_OP_NOT_LOGICAL;
	case TOKEN_CAST:	return UNARY_OP_CAST;
	}
}

BinaryOperation get_binary_op(Token* token)
{
	Token_Type t = token->type;
	switch (t) {
	case TOKEN_LOGIC_AND:			return BINARY_OP_LOGIC_AND;
	case TOKEN_LOGIC_OR:			return BINARY_OP_LOGIC_OR;
	case TOKEN_EQUAL_COMPARISON:	return BINARY_OP_EQUAL_EQUAL;
	case TOKEN_LESS_EQUAL:			return BINARY_OP_LESS_EQUAL;
	case TOKEN_GREATER_EQUAL:		return BINARY_OP_GREATER_EQUAL;
	case TOKEN_NOT_EQUAL:			return BINARY_OP_NOT_EQUAL;
	case '>':						return BINARY_OP_GREATER_THAN;
	case '<':						return BINARY_OP_LESS_THAN;
	case '^':						return BINARY_OP_XOR;
	case '|':						return BINARY_OP_OR;
	case '&':						return BINARY_OP_AND;
	case TOKEN_BITSHIFT_LEFT:		return BINARY_OP_BITSHIFT_LEFT;
	case TOKEN_BITSHIFT_RIGHT:		return BINARY_OP_BITSHIFT_RIGHT;
	case '+':						return BINARY_OP_PLUS;
	case '-':						return BINARY_OP_MINUS;
	case '*':						return BINARY_OP_MULT;
	case '/':						return BINARY_OP_DIV;
	case '%':						return BINARY_OP_MOD;
	case '.':						return BINARY_OP_DOT;
	case '=':						return ASSIGNMENT_OPERATION_EQUAL;
	case TOKEN_PLUS_EQUAL:			return ASSIGNMENT_OPERATION_PLUS_EQUAL;
	case TOKEN_MINUS_EQUAL:			return ASSIGNMENT_OPERATION_MINUS_EQUAL;
	case TOKEN_TIMES_EQUAL:			return ASSIGNMENT_OPERATION_TIMES_EQUAL;
	case TOKEN_DIV_EQUAL:			return ASSIGNMENT_OPERATION_DIVIDE_EQUAL;
	case TOKEN_AND_EQUAL:			return ASSIGNMENT_OPERATION_AND_EQUAL;
	case TOKEN_OR_EQUAL:			return ASSIGNMENT_OPERATION_OR_EQUAL;
	case TOKEN_XOR_EQUAL:			return ASSIGNMENT_OPERATION_XOR_EQUAL;
	case TOKEN_SHL_EQUAL:			return ASSIGNMENT_OPERATION_SHL_EQUAL;
	case TOKEN_SHR_EQUAL:			return ASSIGNMENT_OPERATION_SHR_EQUAL;
	}
}

Ast* create_proc(Memory_Arena* arena, Token* name, Type_Instance* return_type, Ast** arguments, int nargs, Ast* body, Scope* scope, Decl_Site* site) {
	Ast* proc = ALLOC_AST(arena);

	if (body)
		proc->node = AST_NODE_PROC_DECLARATION;
	else
		proc->node = AST_NODE_PROC_FORWARD_DECL;
	proc->is_decl = true;
	proc->return_type = 0;
	proc->type_checked = false;

	proc->proc_decl.scope = scope;
	proc->proc_decl.name = name;
	proc->proc_decl.proc_ret_type = return_type;
	proc->proc_decl.proc_type = 0;
	proc->proc_decl.arguments = arguments;
	proc->proc_decl.body = body;
	proc->proc_decl.num_args = nargs;

	proc->proc_decl.site.filename = site->filename;
	proc->proc_decl.site.line = site->line;
	proc->proc_decl.site.column = site->column;

	scope->decl_node = proc;

	return proc;
}

Ast* create_named_argument(Memory_Arena* arena, Token* name, Type_Instance* type, Ast* default_val, int index, Scope* scope, Decl_Site* site)
{
	Ast* narg = ALLOC_AST(arena);

	narg->node = AST_NODE_NAMED_ARGUMENT;
	narg->is_decl = true;
	narg->return_type = 0;
	narg->type_checked = false;

	narg->named_arg.arg_name = name;
	narg->named_arg.arg_type = type;
	narg->named_arg.index = index;
	narg->named_arg.default_value = default_val;
	narg->named_arg.scope = scope;

	narg->named_arg.site.filename = site->filename;
	narg->named_arg.site.line = site->line;
	narg->named_arg.site.column = site->column;

	return narg;
}

Ast* create_variable_decl(Memory_Arena* arena, Token* name, Type_Instance* type, Ast* assign_val, Scope* scope, Decl_Site* site)
{
	Ast* vdecl = ALLOC_AST(arena);

	vdecl->node = AST_NODE_VARIABLE_DECL;
	vdecl->is_decl = true;
	vdecl->return_type = 0;
	vdecl->type_checked = false;

	vdecl->var_decl.scope = scope;
	vdecl->var_decl.name = name;
	vdecl->var_decl.alignment = 4;
	vdecl->var_decl.assignment = assign_val;
	vdecl->var_decl.type = type;
	
	vdecl->var_decl.site.filename = site->filename;
	vdecl->var_decl.site.line = site->line;
	vdecl->var_decl.site.column = site->column;

	return vdecl;
}

Ast* create_literal(Memory_Arena* arena, u32 flags, Token* lit_tok)
{
	Ast* lit = ALLOC_AST(arena);

	lit->node = AST_NODE_LITERAL_EXPRESSION;
	lit->is_decl = false;
	lit->return_type = 0;
	lit->type_checked = false;

	lit->expression.expression_type = EXPRESSION_TYPE_LITERAL;
	lit->expression.literal_exp.flags = flags;
	lit->expression.literal_exp.lit_tok = lit_tok;
	lit->expression.literal_exp.type = 0;
	return lit;
}

Ast* create_binary_operation(Memory_Arena* arena, Ast* left_op, Ast *right_op, Token* op, Precedence precedence, Scope* scope)
{
	Ast* binop = ALLOC_AST(arena);

	binop->node = AST_NODE_BINARY_EXPRESSION;
	binop->is_decl = false;
	binop->return_type = 0;
	binop->type_checked = false;

	binop->expression.expression_type = EXPRESSION_TYPE_BINARY;
	binop->expression.binary_exp.left = left_op;
	binop->expression.binary_exp.right = right_op;
	binop->expression.binary_exp.op = get_binary_op(op);
	binop->expression.binary_exp.op_token = op;
	binop->expression.binary_exp.scope = scope;
	binop->expression.binary_exp.precedence = precedence;

	return binop;
}

Ast* create_variable(Memory_Arena* arena, Token* var_token, Scope* scope)
{
	Ast* var = ALLOC_AST(arena);

	var->node = AST_NODE_VARIABLE_EXPRESSION;
	var->is_decl = false;
	var->return_type = 0;
	var->type_checked = false;

	var->expression.expression_type = EXPRESSION_TYPE_VARIABLE;

	var->expression.variable_exp.name = var_token;
	var->expression.variable_exp.type = 0;
	var->expression.variable_exp.scope = scope;
	
	return var;
}

Ast* create_block(Memory_Arena* arena, Scope* scope)
{
	Ast* block = ALLOC_AST(arena);

	block->node = AST_NODE_BLOCK;
	block->is_decl = false;
	block->return_type = 0;
	block->type_checked = false;

	block->block.scope = create_scope(scope->level + 1, scope, SCOPE_FLAG_BLOCK_SCOPE);
	block->block.commands = create_array(Ast*, 16);

	scope->decl_node = block;

	return block;
}

Ast* create_proc_call(Memory_Arena* arena, Token* name, Ast** args, Scope* scope)
{
	Ast* proc_call = ALLOC_AST(arena);

	proc_call->node = AST_NODE_PROCEDURE_CALL;
	proc_call->is_decl = false;
	proc_call->return_type = 0;
	proc_call->type_checked = false;

	proc_call->expression.expression_type = EXPRESSION_TYPE_PROC_CALL;

	proc_call->expression.proc_call.name = name;
	proc_call->expression.proc_call.args = args;
	proc_call->expression.proc_call.scope = scope;

	return proc_call;
}

Ast* create_if(Memory_Arena* arena, Ast* bool_exp, Ast* body, Ast* else_stmt, Scope* scope)
{
	Ast* if_stmt = ALLOC_AST(arena);
	if_stmt->node = AST_NODE_IF_STATEMENT;
	if_stmt->is_decl = false;
	if_stmt->return_type = 0;
	if_stmt->type_checked = false;

	if_stmt->if_stmt.bool_exp = bool_exp;
	if_stmt->if_stmt.body = body;
	if_stmt->if_stmt.scope = scope;
	if_stmt->if_stmt.else_exp = else_stmt;

	return if_stmt;
}

Ast* create_while(Memory_Arena* arena, Ast* bool_exp, Ast* body, Scope* scope)
{
	Ast* while_stmt = ALLOC_AST(arena);
	while_stmt->node = AST_NODE_WHILE_STATEMENT;
	while_stmt->is_decl = false;
	while_stmt->return_type = 0;
	while_stmt->type_checked = false;

	while_stmt->while_stmt.bool_exp = bool_exp;
	while_stmt->while_stmt.body = body;
	while_stmt->while_stmt.scope = scope;

	return while_stmt;
}

Ast* create_return(Memory_Arena* arena, Ast* exp, Scope* scope, Token* token)
{
	Ast* ret_stmt = ALLOC_AST(arena);
	ret_stmt->node = AST_NODE_RETURN_STATEMENT;
	ret_stmt->is_decl = false;
	ret_stmt->return_type = 0;
	ret_stmt->type_checked = false;

	ret_stmt->ret_stmt.expr = exp;
	ret_stmt->ret_stmt.scope = scope;

	ret_stmt->ret_stmt.site.filename = token->filename;
	ret_stmt->ret_stmt.site.line = token->line;
	ret_stmt->ret_stmt.site.column = token->column;
	
	return ret_stmt;
}

Ast* create_unary_expression(Memory_Arena* arena, Ast* operand, UnaryOperation op, u32 flags, Type_Instance* cast_type, Precedence precedence, Scope* scope)
{
	Ast* unop = ALLOC_AST(arena);

	unop->node = AST_NODE_UNARY_EXPRESSION;
	unop->is_decl = false;
	unop->return_type = 0;
	unop->type_checked = false;

	unop->expression.expression_type = EXPRESSION_TYPE_UNARY;
	unop->expression.unary_exp.op = op;
	unop->expression.unary_exp.operand = operand;
	unop->expression.unary_exp.cast_type = cast_type;
	unop->expression.unary_exp.precedence = precedence;
	unop->expression.unary_exp.flags = flags;
	unop->expression.unary_exp.scope = scope;

	return unop;
}

Ast* create_break(Memory_Arena* arena, Scope* scope, Token* token)
{
	Ast* break_stmt = ALLOC_AST(arena);

	break_stmt->is_decl = false;
	break_stmt->node = AST_NODE_BREAK_STATEMENT;
	break_stmt->return_type = 0;
	break_stmt->type_checked = false;

	break_stmt->break_stmt.scope = scope;

	break_stmt->break_stmt.site.filename = token->filename;
	break_stmt->break_stmt.site.line = token->line;
	break_stmt->break_stmt.site.column = token->column;

	return break_stmt;
}

Ast* create_continue(Memory_Arena* arena, Scope* scope, Token* token)
{
	Ast* continue_stmt = ALLOC_AST(arena);

	continue_stmt->is_decl = false;
	continue_stmt->node = AST_NODE_CONTINUE_STATEMENT;
	continue_stmt->return_type = 0;
	continue_stmt->type_checked = false;

	continue_stmt->continue_stmt.scope = scope;

	continue_stmt->continue_stmt.site.filename = token->filename;
	continue_stmt->continue_stmt.site.line = token->line;
	continue_stmt->continue_stmt.site.column = token->column;

	return continue_stmt;
}

Ast* create_struct_decl(Memory_Arena* arena, Token* name, Ast** fields, int num_fields, Scope* struct_scope, Decl_Site* site)
{
	Ast* struct_decl = ALLOC_AST(arena);

	struct_decl->is_decl = true;
	struct_decl->node = AST_NODE_STRUCT_DECLARATION;
	struct_decl->return_type = 0;
	struct_decl->type_checked = false;

	struct_decl->struct_decl.alignment = 4;
	struct_decl->struct_decl.fields = fields;
	struct_decl->struct_decl.name = name;
	struct_decl->struct_decl.num_fields = num_fields;
	struct_decl->struct_decl.size_bytes = 0;
	struct_decl->struct_decl.type_info = 0;
	struct_decl->struct_decl.scope = struct_scope;

	struct_decl->struct_decl.site.filename = site->filename;
	struct_decl->struct_decl.site.line = site->line;
	struct_decl->struct_decl.site.column = site->column;

	struct_scope->decl_node = struct_decl;

	return struct_decl;
}

void block_push_command(Ast* block, Ast* command)
{
	push_array(block->block.commands, &command);
}

void push_ast_list(Ast** list, Ast* arg)
{
	push_array(list, &arg);
}

s64 generate_scope_id()
{
	scope_manager.current_id++;
	return scope_manager.current_id - 1;
}

Scope* create_scope(s32 level, Scope* parent, u32 flags)
{
	Scope* res = (Scope*)malloc(sizeof(Scope));
	res->num_declarations = 0;
	res->symb_table = 0;
	res->id = generate_scope_id();
	res->level = level;
	res->parent = parent;
	res->flags = flags;
	res->decl_node = 0;
	return res;
}







/***************************************

				DEBUG

****************************************/


static int DEBUG_indent_level = 0;
void DEBUG_print_indent_level() {
	for (int i = 0; i < DEBUG_indent_level; ++i) {
		fprintf(stdout, "   ");
	}
}

void DEBUG_print_type(FILE* out, Type_Instance* type) {
	if (!type) {
		fprintf(out, "(TYPE_IS_NULL)");
		return;
	}
	if (type->type == TYPE_PRIMITIVE) {
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
	} else if (type->type == TYPE_POINTER) {
		fprintf(out, "^");
		DEBUG_print_type(out, type->pointer_to);
	} else if (type->type == TYPE_STRUCT) {
		fprintf(out, "%.*s struct {\n", type->type_struct.name_length, type->type_struct.name);
		int num_fields = get_arr_length(type->type_struct.fields_types);
		for (int i = 0; i < num_fields; ++i) {
			Type_Instance* field_type = type->type_struct.fields_types[i];
			string name = type->type_struct.fields_names[i];
			fprintf(out, "%.*s : ", name.length, name.data);
			DEBUG_print_type(out, field_type);
			fprintf(out, ";\n");
		}
		fprintf(out, "}\n");
	} else if (type->type == TYPE_FUNCTION) {
		fprintf(out, "(");
		int num_args = type->type_function.num_arguments;
		for (int i = 0; i < num_args; ++i) {
			DEBUG_print_type(out, type->type_function.arguments_type[i]);
			if(i + 1 < num_args) fprintf(out, ",");
		}
		fprintf(out, ") -> ");
		DEBUG_print_type(out, type->type_function.return_type);
	}

}

void DEBUG_print_block(FILE* out, Ast* block)
{
	size_t num_commands = get_arr_length(block->block.commands);
	fprintf(out, "{\n");
	DEBUG_indent_level += 1;
	for (size_t i = 0; i < num_commands; ++i) {
		DEBUG_print_indent_level();
		DEBUG_print_node(out, block->block.commands[i]);
		fprintf(out, ";\n");
	}
	DEBUG_indent_level -= 1;
	DEBUG_print_indent_level();
	fprintf(out, "}");
}

void DEBUG_print_proc(FILE* out, Ast* proc_node) {
	fprintf(out, "%.*s :: (", TOKEN_STR(proc_node->proc_decl.name));
	for (int i = 0; i < proc_node->proc_decl.num_args; ++i) {
		Ast* narg = proc_node->proc_decl.arguments[i];
		fprintf(out, "%.*s : ", TOKEN_STR(narg->named_arg.arg_name));
		DEBUG_print_type(out, narg->named_arg.arg_type);
		if (i + 1 != proc_node->proc_decl.num_args) fprintf(out, ", ");
	}
	fprintf(out, ") -> ");
	DEBUG_print_type(out, proc_node->proc_decl.proc_ret_type);
	if (proc_node->proc_decl.body == 0) {
		fprintf(out, ";");
	} else {
		DEBUG_print_block(out, proc_node->proc_decl.body);
	}
}

void DEBUG_print_expression(FILE* out, Ast* node) {
	switch (node->expression.expression_type) {
	case EXPRESSION_TYPE_LITERAL: {
		fprintf(out, "%.*s", TOKEN_STR(node->expression.literal_exp.lit_tok));
	} break;
	case EXPRESSION_TYPE_BINARY: {
		fprintf(out, "(");
		DEBUG_print_expression(out, node->expression.binary_exp.left);
		switch (node->expression.binary_exp.op) {
		case BINARY_OP_PLUS:			 fprintf(out, "+"); break;
		case BINARY_OP_MINUS:			 fprintf(out, "-"); break;
		case BINARY_OP_MULT:			 fprintf(out, "*"); break;
		case BINARY_OP_DIV:				 fprintf(out, "/"); break;
		case BINARY_OP_AND:				 fprintf(out, "&"); break;
		case BINARY_OP_OR:				 fprintf(out, "|"); break;
		case BINARY_OP_XOR:				 fprintf(out, "^"); break;
		case BINARY_OP_MOD:				 fprintf(out, "%"); break;
		case BINARY_OP_LOGIC_AND:		 fprintf(out, "&&"); break;
		case BINARY_OP_LOGIC_OR:		 fprintf(out, "||"); break;
		case BINARY_OP_BITSHIFT_LEFT:	 fprintf(out, "<<"); break;
		case BINARY_OP_BITSHIFT_RIGHT:	 fprintf(out, ">>"); break;
		case BINARY_OP_LESS_THAN:		 fprintf(out, "<"); break;
		case BINARY_OP_GREATER_THAN:	 fprintf(out, ">"); break;
		case BINARY_OP_LESS_EQUAL:		 fprintf(out, "<="); break;
		case BINARY_OP_GREATER_EQUAL:	 fprintf(out, ">="); break;
		case BINARY_OP_EQUAL_EQUAL:		 fprintf(out, "=="); break;
		case BINARY_OP_NOT_EQUAL:		 fprintf(out, "!="); break;
		case BINARY_OP_DOT:				 fprintf(out, "."); break;
		case ASSIGNMENT_OPERATION_EQUAL:		fprintf(out, "="); break;
		case ASSIGNMENT_OPERATION_PLUS_EQUAL:	fprintf(out, "+="); break;
		case ASSIGNMENT_OPERATION_MINUS_EQUAL:	fprintf(out, "-="); break;
		case ASSIGNMENT_OPERATION_TIMES_EQUAL:	fprintf(out, "*="); break;
		case ASSIGNMENT_OPERATION_DIVIDE_EQUAL:	fprintf(out, "/="); break;
		case ASSIGNMENT_OPERATION_AND_EQUAL:	fprintf(out, "&="); break;
		case ASSIGNMENT_OPERATION_OR_EQUAL:		fprintf(out, "|="); break;
		case ASSIGNMENT_OPERATION_XOR_EQUAL:	fprintf(out, "^="); break;
		case ASSIGNMENT_OPERATION_SHL_EQUAL:	fprintf(out, "<<="); break;
		case ASSIGNMENT_OPERATION_SHR_EQUAL:	fprintf(out, ">>="); break;
		}
		DEBUG_print_expression(out, node->expression.binary_exp.right);
		fprintf(out, ")");
	} break;
	case EXPRESSION_TYPE_UNARY: {
		if (node->expression.unary_exp.flags & UNARY_EXP_FLAG_PREFIXED) {
			switch (node->expression.unary_exp.op) {
			case UNARY_OP_ADDRESS_OF:	fprintf(out, "&"); break;
			case UNARY_OP_MINUS:		fprintf(out, "-"); break;
			case UNARY_OP_DEREFERENCE:	fprintf(out, "*"); break;
			case UNARY_OP_NOT_BITWISE:	fprintf(out, "~"); break;
			case UNARY_OP_NOT_LOGICAL:	fprintf(out, "!"); break;
			case UNARY_OP_PLUS:			fprintf(out, "+"); break;
			case UNARY_OP_CAST: {
				fprintf(out, "cast (");
				DEBUG_print_type(out, node->expression.unary_exp.cast_type);
				fprintf(out, ")");
			}break;
			}
			DEBUG_print_expression(out, node->expression.unary_exp.operand);
		}
		else {
			fprintf(out, "ERROR POSTFIXED");
		}
	}break;
	case EXPRESSION_TYPE_VARIABLE: {
		fprintf(out, "%.*s", TOKEN_STR(node->expression.variable_exp.name));
	} break;
	case EXPRESSION_TYPE_PROC_CALL: {
		int num_args = 0;
		if (node->expression.proc_call.args) {
			num_args = get_arr_length(node->expression.proc_call.args);
		}
		fprintf(out, "%.*s(", TOKEN_STR(node->expression.proc_call.name));
		for (int i = 0; i < num_args; ++i) {
			DEBUG_print_expression(out, node->expression.proc_call.args[i]);
			if(i + 1 != num_args) fprintf(out, ", ");
		}
		fprintf(out, ")");
	} break;
	}
}

void DEBUG_print_var_decl(FILE* out, Ast* node) {
	fprintf(out, "%.*s : ", TOKEN_STR(node->var_decl.name));
	DEBUG_print_type(out, node->var_decl.type);
	if (node->var_decl.assignment) {
		fprintf(out, " = ");
		DEBUG_print_expression(out, node->var_decl.assignment);
	}
}

void DEBUG_print_if_statement(FILE* out, Ast* node)
{
	fprintf(out, "if (");
	DEBUG_print_expression(out, node->if_stmt.bool_exp);
	fprintf(out, ")");
	if (node->if_stmt.body->node == AST_NODE_BLOCK) {
		DEBUG_print_node(out, node->if_stmt.body);
	} else {
		fprintf(out, " ");
		DEBUG_print_node(out, node->if_stmt.body);
		fprintf(out, ";");
	}
	if (node->if_stmt.else_exp) {
		fprintf(out, "\n");
		DEBUG_print_indent_level();
		fprintf(out, "else");
		if (node->if_stmt.body->node == AST_NODE_BLOCK) {
			DEBUG_print_node(out, node->if_stmt.else_exp);
		} else {
			fprintf(out, " ");
			DEBUG_print_node(out, node->if_stmt.else_exp);
			fprintf(out, ";");
		}
	}
}

void DEBUG_print_while_statement(FILE* out, Ast* node)
{
	fprintf(out, "while (");
	DEBUG_print_expression(out, node->while_stmt.bool_exp);
	fprintf(out, ")");
	DEBUG_print_node(out, node->while_stmt.body);
}

void DEBUG_print_return_statement(FILE* out, Ast* node)
{
	fprintf(out, "return ");
	if (node->ret_stmt.expr) {
		DEBUG_print_expression(out, node->ret_stmt.expr);
	}
}

void DEBUG_print_break_statement(FILE* out, Ast* node) 
{
	fprintf(out, "break");
}

void DEBUG_print_continue_statement(FILE* out, Ast* node)
{
	fprintf(out, "continue");
}

void DEBUG_print_struct_declaration(FILE* out, Ast* node)
{

	Ast_StructDecl* sd = &node->struct_decl;
	fprintf(out, "%.*s :: struct{\n", TOKEN_STR(sd->name));
	DEBUG_indent_level += 1;
	for (int i = 0; i < sd->num_fields; ++i) {
		DEBUG_print_var_decl(out, sd->fields[i]);
		fprintf(out, ";\n");
	}
	DEBUG_indent_level -= 1;
	fprintf(out, "}\n");
}

void DEBUG_print_node(FILE* out, Ast* node) {
	switch (node->node) {
	case AST_NODE_PROC_FORWARD_DECL:
	case AST_NODE_PROC_DECLARATION:		DEBUG_print_proc(out, node); break;
	case AST_NODE_VARIABLE_DECL:		DEBUG_print_var_decl(out, node); break;
	case AST_NODE_UNARY_EXPRESSION:		
	case AST_NODE_BINARY_EXPRESSION:	DEBUG_print_expression(out, node); break;
	case AST_NODE_BLOCK:				DEBUG_print_block(out, node); break;
	case AST_NODE_IF_STATEMENT:			DEBUG_print_if_statement(out, node); break;
	case AST_NODE_WHILE_STATEMENT:		DEBUG_print_while_statement(out, node); break;
	case AST_NODE_PROCEDURE_CALL:		DEBUG_print_expression(out, node); break;
	case AST_NODE_RETURN_STATEMENT:		DEBUG_print_return_statement(out, node); break;
	case AST_NODE_BREAK_STATEMENT:		DEBUG_print_break_statement(out, node); break;
	case AST_NODE_CONTINUE_STATEMENT:	DEBUG_print_continue_statement(out, node); break;
	case AST_NODE_STRUCT_DECLARATION:	DEBUG_print_struct_declaration(out, node); break;
	}
}

void DEBUG_print_ast(FILE* out, Ast** ast) {
	int l = get_arr_length(ast);
	for (int i = 0; i < get_arr_length(ast); ++i) {
		DEBUG_print_node(out, ast[i]);
		fprintf(out, "\n");
	}
}
