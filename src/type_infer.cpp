#include "type_infer.h"
#include "decl_check.h"
#include "ast.h"

Type_Error report_type_mismatch(Ast* node, Type_Instance* t1, Type_Instance* t2) {
	report_error_location(node);
	Type_Error err = report_type_error(TYPE_ERROR_FATAL, "type mismatch '");
	DEBUG_print_type(stderr, t1, true);
	fprintf(stderr, "' vs '");
	DEBUG_print_type(stderr, t2, true);
	fprintf(stderr, "'\n");
	return err;
}

Type_Instance* type_propagate(Type_Instance* strong, Ast* expr);
Type_Instance* type_check_expr(Ast* expr);

/* --------------------------------------------------------------
   -------------------- Type Inference --------------------------
   -------------------------------------------------------------- */

Type_Instance* infer_from_expression(Ast* expr, Type_Error* error, u32 flags) {
	assert(expr->flags & AST_FLAG_IS_EXPRESSION);

	Type_Instance* type_infered = 0;

	switch (expr->node_type) {
		case AST_EXPRESSION_BINARY:				type_infered = infer_from_binary_expression(expr, error, flags); break;
		case AST_EXPRESSION_LITERAL:			type_infered = infer_from_literal_expression(expr, error, flags); break;
		case AST_EXPRESSION_PROCEDURE_CALL:		type_infered = infer_from_procedure_call(expr, error, flags); break;
		case AST_EXPRESSION_UNARY:				type_infered = infer_from_unary_expression(expr, error, flags); break;
		case AST_EXPRESSION_VARIABLE:			type_infered = infer_from_variable_expression(expr, error, flags); break;
	}
	return type_infered;
}

Type_Instance* infer_from_binary_expression(Ast* expr, Type_Error* error, u32 flags)  {
	assert(expr->node_type == AST_EXPRESSION_BINARY);

	Type_Instance* left  = infer_from_expression(expr->expr_binary.left, error, flags);
	Type_Instance* right = infer_from_expression(expr->expr_binary.right, error, flags);
	
	if (left->flags & TYPE_FLAG_STRONG || right->flags & TYPE_FLAG_STRONG) {
		if (left->flags & TYPE_FLAG_WEAK && right->flags & TYPE_FLAG_STRONG)
			left = type_propagate(right, expr->expr_binary.left);
		else if (left->flags & TYPE_FLAG_STRONG && right->flags & TYPE_FLAG_WEAK)
			right = type_propagate(left, expr->expr_binary.right);
		else if (left->flags & TYPE_FLAG_STRONG && right->flags & TYPE_FLAG_STRONG) {
			assert(left == right);
			expr->type_return = left;
		}
		return type_check_expr(expr);
	}

	return 0;
}

Type_Instance* infer_from_unary_expression(Ast* expr, Type_Error* error, u32 flags) {
	assert(expr->node_type == AST_EXPRESSION_UNARY);
	Type_Instance* infered = infer_from_expression(expr->expr_unary.operand, error, flags);
	if (infered->flags & TYPE_FLAG_STRONG) {
		expr->type_return = infered;
		return infered;
	}
	return 0;
}

Type_Instance* infer_from_literal_expression(Ast* expr, Type_Error* error, u32 flags) {
	Type_Instance* result = type_new_temporary();
	result->flags = TYPE_FLAG_WEAK;

	switch (expr->expr_literal.type) {
		case LITERAL_BIN_INT:
		case LITERAL_HEX_INT: {
			result->kind = KIND_PRIMITIVE;
			result->primitive = TYPE_PRIMITIVE_U64;
		}break;
		case LITERAL_SINT: {
			result->kind = KIND_PRIMITIVE;
			result->primitive = TYPE_PRIMITIVE_S64;
		}break;
		case LITERAL_BOOL: {
			return type_primitive_get(TYPE_PRIMITIVE_BOOL);
		}break;
		case LITERAL_FLOAT: {
			result->kind = KIND_PRIMITIVE;
			result->primitive = TYPE_PRIMITIVE_R64;
		}break;
		case LITERAL_STRUCT:
		case LITERAL_ARRAY: assert(0); break;	// TODO(psv): not implemented yet
	}
	return result;
}

Type_Instance* infer_from_procedure_call(Ast* expr, Type_Error* error, u32 flags) {
	Ast* decl = decl_from_name(expr->scope, expr->expr_proc_call.name);
	if (!decl) {
		return 0;
	}
	if (decl->node_type != AST_DECL_PROCEDURE) {
		*error |= report_type_error(TYPE_ERROR_FATAL, expr, "'%.*s' is not a procedure\n", TOKEN_STR(expr->expr_proc_call.name));
		return 0;
	}
	Type_Instance* type = decl->decl_procedure.type_return;
	assert(type->flags & TYPE_FLAG_INTERNALIZED);
	size_t nargs = expr->expr_proc_call.args_count;
	for (size_t i = 0; i < nargs; ++i) {
		// lval is always false in case of procedure arguments.
		Type_Instance* type = infer_from_expression(expr->expr_proc_call.args[i], error, false);
		if(expr->expr_proc_call.args[i]->flags & AST_FLAG_TYPE_STRENGTH_DEP){
			expr->flags |= AST_FLAG_TYPE_STRENGTH_DEP;
			expr->expr_proc_call.args[i]->flags |= AST_FLAG_TYPE_STRENGTH_DEP;
		}
		expr->expr_proc_call.args[i]->type_return = type;
	}

	return type;
}

Type_Instance* infer_from_variable_expression(Ast* expr, Type_Error* error, u32 flags) {
	assert(expr->node_type == AST_EXPRESSION_VARIABLE);

	Ast* decl = decl_from_name(expr->scope, expr->expr_variable.name);
	if (!decl) {
		return 0;
	}
	expr->expr_variable.decl = decl;
	Type_Instance* type = 0;

	if (decl->node_type != AST_DECL_VARIABLE && decl->node_type != AST_DECL_CONSTANT) {
		assert(0); // implement function ptr
	} else if(decl->node_type == AST_DECL_VARIABLE){
		type = decl->decl_variable.variable_type;
		if (flags & TYPE_INFER_LVALUE) {
			// transform this in the pointer type
			Type_Instance* ptrtype = type_new_temporary();
			ptrtype->kind = KIND_POINTER;
			ptrtype->type_size_bits = type_pointer_size_bits();
			ptrtype->flags = TYPE_FLAG_SIZE_RESOLVED | TYPE_FLAG_RESOLVED | TYPE_FLAG_LVALUE;
			ptrtype->pointer_to = type;
			expr->expr_variable.flags |= EXPR_VARIABLE_LVALUE;
			return internalize_type(&ptrtype, true);
		}
	} else if(decl->node_type == AST_DECL_CONSTANT) {
		type = decl->decl_constant.type_info;
	}
	return type;
}

Type_Instance* type_propagate(Type_Instance* strong, Ast* expr) {
	assert(expr->flags & AST_FLAG_IS_EXPRESSION);

	if (expr->flags & TYPE_FLAG_STRONG)
		return expr->type_return;

	switch(expr->node_type){
		case AST_EXPRESSION_BINARY:{
			Type_Instance* left_type  = type_propagate(strong, expr->expr_binary.left->type_return);
			Type_Instance* right_type = type_propagate(strong, expr->expr_binary.right->type_return);

			expr->type_return = type_check_expr(strong, expr, error);
		}break;
	}
}

Type_Instance* type_check_expr(Type_Instance* check_against, Ast* expr, Type_Error* error){
	switch (expr->node_type) {
		case AST_EXPRESSION_BINARY:{
			switch (expr->expr_binary.op) {
				case OP_BINARY_PLUS:
				case OP_BINARY_MINUS:{
				} break;

				case OP_BINARY_AND:
				case OP_BINARY_OR:
				case OP_BINARY_XOR:
				case OP_BINARY_DIV:
				case OP_BINARY_MULT:
				case OP_BINARY_MOD:
				case OP_BINARY_SHL:
				case OP_BINARY_SHR:
				case OP_BINARY_LOGIC_AND:
				case OP_BINARY_LOGIC_OR:

				case OP_BINARY_EQUAL:
				case OP_BINARY_NOT_EQUAL:
				case OP_BINARY_GE:
				case OP_BINARY_GT:
				case OP_BINARY_LE:
				case OP_BINARY_LT:

				case OP_BINARY_VECTOR_ACCESS:

				case OP_BINARY_DOT:
			}
		} break;
	}
}