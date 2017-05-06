#include "ast.h"
#include "parser.h"

#define ALLOC_AST(A) (Ast*)A->allocate(sizeof(Ast))


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
	}
}

Ast* create_proc(Memory_Arena* arena, Token* name, Type* return_type, Ast* arguments, int nargs, Ast* body) {
	Ast* proc = ALLOC_AST(arena);

	proc->node = AST_NODE_PROC_DECLARATION;
	proc->is_decl = true;
	proc->return_type = 0;

	proc->proc_decl.name = name;
	proc->proc_decl.proc_ret_type = return_type;
	proc->proc_decl.arguments = arguments;
	proc->proc_decl.body = body;
	proc->proc_decl.num_args = nargs;

	return proc;
}

Ast* create_named_argument(Memory_Arena* arena, Token* name, Type* type, Ast* default_val, int index) 
{
	Ast* narg = ALLOC_AST(arena);

	narg->node = AST_NODE_NAMED_ARGUMENT;
	narg->is_decl = true;
	narg->return_type = 0;

	narg->named_arg.arg_name = name;
	narg->named_arg.arg_type = type;
	narg->named_arg.index = index;
	narg->named_arg.default_value = default_val;

	return narg;
}

Ast* create_variable_decl(Memory_Arena* arena, Token* name, Type* type, Ast* assign_val)
{
	Ast* vdecl = ALLOC_AST(arena);

	vdecl->node = AST_NODE_VARIABLE_DECL;
	vdecl->is_decl = true;
	vdecl->return_type = 0;

	vdecl->var_decl.name = name;
	vdecl->var_decl.alignment = 4;
	vdecl->var_decl.assignment = assign_val;
	vdecl->var_decl.type = type;
	vdecl->var_decl.size_bytes = get_type_size_bytes(type);

	return vdecl;
}

Ast* create_literal(Memory_Arena* arena, u32 flags, Token* lit_tok)
{
	Ast* lit = ALLOC_AST(arena);

	lit->node = AST_NODE_LITERAL_EXPRESSION;
	lit->is_decl = false;
	lit->return_type = 0;

	lit->expression.expression_type = EXPRESSION_TYPE_LITERAL;
	lit->expression.literal_exp.flags = flags;
	lit->expression.literal_exp.lit_tok = lit_tok;
	lit->expression.literal_exp.type = 0;

	return lit;
}

Ast* create_binary_operation(Memory_Arena* arena, Ast* left_op, Ast *right_op, Token* op)
{
	Ast* binop = ALLOC_AST(arena);

	binop->node = AST_NODE_BINARY_EXPRESSION;
	binop->is_decl = false;
	binop->return_type = 0;

	binop->expression.expression_type = EXPRESSION_TYPE_BINARY;
	binop->expression.binary_exp.left = left_op;
	binop->expression.binary_exp.right = right_op;
	binop->expression.binary_exp.op = get_binary_op(op);

	return binop;
}

Ast* create_variable(Memory_Arena* arena, Token* var_token, Scope* scope)
{
	Ast* var = ALLOC_AST(arena);

	var->node = AST_NODE_VARIABLE_EXPRESSION;
	var->is_decl = false;
	var->return_type = 0;

	var->expression.expression_type = EXPRESSION_TYPE_VARIABLE;

	var->expression.variable_exp.name = var_token;
	var->expression.variable_exp.type = 0;
	var->expression.variable_exp.scope = scope;
	
	return var;
}








/***************************************

				DEBUG

****************************************/

void DEBUG_print_type(FILE* out, Type* type) {
	if (type->flags & TYPE_FLAG_PRIMITIVE) {
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
	} else if (type->flags & TYPE_FLAG_POINTER) {
		fprintf(out, "^");
		DEBUG_print_type(out, type->pointer_to);
	}
}

void DEBUG_print_proc(FILE* out, Ast* proc_node) {
	fprintf(out, "%.*s :: (", TOKEN_STR(proc_node->proc_decl.name));
	for (int i = 0; i < proc_node->proc_decl.num_args; ++i) {
		Ast* narg = proc_node->proc_decl.arguments;
		fprintf(out, "%.*s : ", TOKEN_STR(narg->named_arg.arg_name));
		DEBUG_print_type(out, narg->named_arg.arg_type);
		if (i + 1 != proc_node->proc_decl.num_args) fprintf(out, ",");
	}
	fprintf(out, ")");
	if (proc_node->proc_decl.body == 0) {
		fprintf(out, "{(empty)}");
	} else {
		fprintf(out, "{...}");
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
		}
		DEBUG_print_expression(out, node->expression.binary_exp.right);
		fprintf(out, ")");
	} break;
	case EXPRESSION_TYPE_UNARY: {
		fprintf(out, "ERROR_UNARY_EXP");
	}break;
	case EXPRESSION_TYPE_VARIABLE: {
		fprintf(out, "%.*s", TOKEN_STR(node->expression.variable_exp.name));
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

void DEBUG_print_node(FILE* out, Ast* node) {
	switch (node->node) {
	case AST_NODE_PROC_DECLARATION:		fprintf(stdout, "PROCEDURE DECLARATION: "); DEBUG_print_proc(out, node); break;
	case AST_NODE_NAMED_ARGUMENT:		fprintf(stdout, "NAMED ARGUMENT"); break;
	case AST_NODE_VARIABLE_DECL:		fprintf(stdout, "VARIABLE DECLARATION: "); DEBUG_print_var_decl(out, node); break;
	case AST_NODE_BINARY_EXPRESSION:	DEBUG_print_expression(out, node); break;
	}
}

void DEBUG_print_ast(FILE* out, Ast** ast) {
	int l = get_arr_length(ast);
	for (int i = 0; i < get_arr_length(ast); ++i) {
		DEBUG_print_node(out, ast[i]);
		fprintf(out, "\n");
	}
}