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

Type_Instance* type_strength_resolve_binary(Type_Instance* t1, Type_Instance* t2, Ast* expr1, Ast* expr2, Type_Error* error);
Type_Error     type_update_weak(Ast* expr, Type_Instance* strong);
Type_Error     type_update_weak(Ast* expr);
Type_Instance* type_transform_weak_to_strong(Type_Instance* weak, Type_Instance* strong, Ast* expr, Type_Error* error);

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

Type_Instance* infer_from_binary_expression(Ast* expr, Type_Error* error, u32 flags) 
{
	auto strength_dependency = [](Ast* expr) -> u32 {
		if (expr->expr_binary.left->flags & AST_FLAG_TYPE_STRENGTH_DEP || expr->expr_binary.right->flags & AST_FLAG_TYPE_STRENGTH_DEP)
			return AST_FLAG_TYPE_STRENGTH_DEP;
		return 0;
	};

	Type_Instance* left  = infer_from_expression(expr->expr_binary.left, error, flags);
	Type_Instance* right = infer_from_expression(expr->expr_binary.right, error, flags);
	if (*error & TYPE_ERROR_FATAL) return 0;

	switch (expr->expr_binary.op) {
		case OP_BINARY_MOD: {
			expr->type_return = type_strength_resolve_binary(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
			expr->flags |= strength_dependency(expr);
			return expr->type_return;
		}break;
		case OP_BINARY_PLUS:
		case OP_BINARY_MINUS: {
			if (left->kind == KIND_POINTER && type_primitive_int(right)) {
				*error |= type_update_weak(expr->expr_binary.left);
				*error |= type_update_weak(expr->expr_binary.right);
				expr->flags |= strength_dependency(expr);
				return left;
			}

			if ((left->kind == KIND_POINTER) && (right->kind == KIND_POINTER)) {
				expr->type_return = type_strength_resolve_binary(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
				expr->flags |= strength_dependency(expr);
				if (expr->expr_binary.op == OP_BINARY_MINUS) {
					// difference of pointers gives an s64
					return type_primitive_get(TYPE_PRIMITIVE_S64);
				} else {
					*error |= report_type_error(TYPE_ERROR_FATAL, expr, "pointer arithmetic between pointers is only defined for the unary operator '-'\n");
					return 0;
				}
			}
		}
		case OP_BINARY_MULT:
		case OP_BINARY_DIV: {
			if ((type_primitive_int(left) && type_primitive_int(right)) ||
				(type_primitive_float(left) && type_primitive_float(right)))
			{
				expr->type_return = type_strength_resolve_binary(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
				expr->flags |= strength_dependency(expr);
				return expr->type_return;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' requires numeric type\n", binop_op_to_string(expr->expr_binary.op));
				return 0;
			}
		}break;
		case OP_BINARY_XOR: {
			expr->type_return = type_strength_resolve_binary(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
			expr->flags |= strength_dependency(expr);

			if (type_primitive_bool(left) && type_primitive_bool(right)) {
				return left;
			} else if (type_primitive_int(left) && type_primitive_int(right)) {
				return expr->type_return;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' requires boolean or integer type\n", binop_op_to_string(expr->expr_binary.op));
				return 0;
			}
		} break;
		case OP_BINARY_AND:
		case OP_BINARY_OR:
		case OP_BINARY_SHL:
		case OP_BINARY_SHR: {
			if (type_primitive_int(left) && type_primitive_int(right)) {
				expr->type_return = type_strength_resolve_binary(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
				expr->flags |= strength_dependency(expr);
				return expr->type_return;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' requires boolean or integer type\n", binop_op_to_string(expr->expr_binary.op));
				return 0;
			}
		}break;

		case OP_BINARY_LOGIC_AND:
		case OP_BINARY_LOGIC_OR: {
			if(type_primitive_bool(left) && type_primitive_bool(right)){
				type_strength_resolve_binary(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
				expr->flags |= strength_dependency(expr);
				expr->type_return = type_primitive_get(TYPE_PRIMITIVE_BOOL);
				return expr->type_return;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' requires boolean type\n", binop_op_to_string(expr->expr_binary.op));
				return 0;
			}
		}break;

		case OP_BINARY_EQUAL:
		case OP_BINARY_NOT_EQUAL: {
			type_strength_resolve_binary(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
			expr->flags |= strength_dependency(expr);
			expr->type_return = type_primitive_get(TYPE_PRIMITIVE_BOOL);
			return expr->type_return;
		}
		case OP_BINARY_GE:
		case OP_BINARY_GT:
		case OP_BINARY_LE:
		case OP_BINARY_LT: 
		{
			if ((type_primitive_int(left) && type_primitive_int(right)) ||
				(type_primitive_float(left) && type_primitive_float(right)))
			{
				type_strength_resolve_binary(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
				expr->flags |= strength_dependency(expr);
				expr->type_return = type_primitive_get(TYPE_PRIMITIVE_BOOL);
				return expr->type_return;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "binary operator '%s' requires numeric type\n", binop_op_to_string(expr->expr_binary.op));
				return 0;
			}
		}break;
		case OP_BINARY_VECTOR_ACCESS: {
			// right is index
			// left  is indexed
			if (expr->expr_binary.right->type_return->kind == KIND_POINTER) {
				Type_Instance* index_type   = type_update_weak(expr->expr_binary.right);
				Type_Instance* indexed_type = type_update_weak(expr->expr_binary.left);
				expr->flags |= strength_dependency(expr);
				if(indexed_type->kind == KIND_POINTER){
					return indexed_type->pointer_to;
				}
			}
			*error |= report_type_error(TYPE_ERROR_FATAL, expr, "vector accessing requires an lvalue type\n");
			return 0;
		}break;

		case OP_BINARY_DOT: {
			// TODO: incomplete
			type_strength_resolve_binary(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
			expr->flags |= strength_dependency(expr);
			expr->type_return = right;
			return right;
			//assert_msg(0, "OP_BINARY_DOT not implemented yet");
		}break;
	}
}

Type_Instance* infer_from_literal_expression(Ast* expr, Type_Error* error, u32 flags) {
	Type_Instance* result = type_new_temporary();
	result->flags = TYPE_FLAG_WEAK;

	switch (expr->expr_literal.type) {
		case LITERAL_BIN_INT:
		case LITERAL_HEX_INT: {
			expr->flags |= AST_FLAG_TYPE_STRENGTH_DEP;
			result->kind = KIND_PRIMITIVE;
			result->primitive = TYPE_PRIMITIVE_U64;
		}break;
		case LITERAL_SINT: {
			expr->flags |= AST_FLAG_TYPE_STRENGTH_DEP;
			result->kind = KIND_PRIMITIVE;
			result->primitive = TYPE_PRIMITIVE_S64;
		}break;
		case LITERAL_BOOL: {
			return type_primitive_get(TYPE_PRIMITIVE_BOOL);
		}break;
		case LITERAL_FLOAT: {
			expr->flags |= AST_FLAG_TYPE_STRENGTH_DEP;
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

Type_Instance* infer_from_unary_expression(Ast* expr, Type_Error* error, u32 flags) {
	assert(expr->node_type == AST_EXPRESSION_UNARY);
	/*
	switch (expr->expr_unary.op) {
		case OP_UNARY_CAST:{
			Type_Instance* res = resolve_type(expr->scope, expr->expr_unary.type_to_cast, rep_undeclared);
			if (!res) {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "could not infer type of cast, type '");
				DEBUG_print_type(stderr, expr->expr_unary.type_to_cast, true);
				fprintf(stderr, "' is invalid\n");
			}
			// @IMPORTANT instead of resolving types here, make them collapse to something, give preference to the type being cast to!
			expr->expr_unary.operand->type_return = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if(expr->expr_unary.operand->type_return && expr->expr_unary.operand->type_return->flags & TYPE_FLAG_WEAK){
				expr->expr_unary.operand->type_return = type_transform_weak_to_strong(expr->expr_unary.operand->type_return, res, expr->expr_unary.operand, error);
				//Type_Instance* opt = resolve_type(expr->scope, expr->expr_unary.operand->type_return, report_undeclared);
				//if(!opt){
				//	*error |= report_type_error(TYPE_ERROR_FATAL, expr, "could not infer type of cast operand\n");
				//} else {
				//	opt->flags |= TYPE_FLAG_RESOLVED;
				//	opt = internalize_type(&opt, true);
				//	*error |= type_update_weak(expr->expr_unary.operand, opt);
				//}
			}
			return res;
		}break;
		case OP_UNARY_ADDRESSOF: {
			// TODO(psv): check if it can only ask for address of lvalue (strong type)
			Type_Instance* res = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if (*error & TYPE_ERROR_FATAL) return 0;
			if (!res) return 0;

			Type_Instance* newtype = type_new_temporary();
			newtype->kind = KIND_POINTER;
			newtype->pointer_to = res;
			newtype->type_size_bits = type_pointer_size_bits();
			newtype->flags = TYPE_FLAG_SIZE_RESOLVED;
			if (res->flags & TYPE_FLAG_INTERNALIZED) {
				newtype->flags |= TYPE_FLAG_RESOLVED;
				internalize_type(&newtype, true);
			} else {
				newtype->flags |= TYPE_FLAG_WEAK;
			}
			return newtype;
		}break;
		case OP_UNARY_DEREFERENCE: {
			Type_Instance* operand_type = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if (*error & TYPE_ERROR_FATAL) return 0;
			if (!operand_type) return 0;

			if (operand_type->kind != KIND_POINTER) {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "cannot dereference a non pointer type '");
				DEBUG_print_type(stderr, operand_type, true);
				fprintf(stderr, "'\n");
				return 0;
			} else {
				Type_Instance* res = operand_type->pointer_to;
				assert(res->flags & TYPE_FLAG_INTERNALIZED);
				return res;
			}
		}break;
		case OP_UNARY_LOGIC_NOT: {
			Type_Instance* operand_type = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if (*error & TYPE_ERROR_FATAL) return 0;
			if (!operand_type) return 0;

			if (type_primitive_bool(operand_type)) {
				assert(operand_type->flags & TYPE_FLAG_INTERNALIZED);
				return operand_type;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "unary operator '!' can only be used in a boolean type\n");
				return 0;
			}
		}break;
		case OP_UNARY_BITWISE_NOT: {
			Type_Instance* operand_type = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if (*error & TYPE_ERROR_FATAL) return 0;
			if (!operand_type) return 0;

			if (type_primitive_int(operand_type)) {
				return operand_type;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "unary operator not '~' can only be used in integer types\n");
				return 0;
			}
		}break;
		case OP_UNARY_PLUS:
		case OP_UNARY_MINUS: {
			Type_Instance* operand_type = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if (*error & TYPE_ERROR_FATAL) return 0;
			if (!operand_type) return 0;

			if (type_primitive_int(operand_type) || type_primitive_float(operand_type)) {
				return operand_type;
			} else {
				*error |= report_type_error(TYPE_ERROR_FATAL, expr, "unary operators '+' or '-' can only be used in numeric types\n");
				return 0;
			}
		}break;
	}
	*/
	return 0;
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


// Resolve the strength of a type TODO(psv): "and updated its children when necessary."
// the returned type is always the correct type, unless an error occurred, then this function returns 0
Type_Instance* type_strength_resolve_binary(Type_Instance* t1, Type_Instance* t2, Ast* expr1, Ast* expr2, Type_Error* error) {
	if (t1->flags & TYPE_FLAG_WEAK && t2->flags & TYPE_FLAG_WEAK) {
		if (type_hash(t1) == type_hash(t2)) {
			return t1;
		} else {
			*error |= report_type_error(TYPE_ERROR_FATAL, expr1, "could not infer type from two different weak types: ");
			DEBUG_print_type(stderr, t1, true);
			fprintf(stderr, " and ");
			DEBUG_print_type(stderr, t2, true);
			fprintf(stderr, "\n");
			return 0;
		}
	}

	if (t1->flags & TYPE_FLAG_WEAK && t2->flags & TYPE_FLAG_STRONG) {
		if (type_hash(t1) == type_hash(t2)) return t2;
		// transform t1 into t2 and update expr1
		Type_Instance* transformed = type_transform_weak_to_strong(t1, t2, expr1, error);
		if (transformed)
			*error |= type_update_weak(expr1, transformed);
		return transformed;
	}
	if (t1->flags & TYPE_FLAG_STRONG && t2->flags & TYPE_FLAG_WEAK) {
		if (type_hash(t1) == type_hash(t2)) return t1;

		// transform t2 into t1 and update expr2
		Type_Instance* transformed = type_transform_weak_to_strong(t2, t1, expr2, error);
		if(transformed)
			*error |= type_update_weak(expr2, transformed);
		return transformed;
	}

	if (t1->flags & TYPE_FLAG_STRONG && t2->flags & TYPE_FLAG_STRONG) {
		if (t1 == t2) return t1;

		*error |= report_type_error(TYPE_ERROR_FATAL, expr1, "type mismatch ");
		DEBUG_print_type(stderr, t1, true);
		fprintf(stderr, " vs ");
		DEBUG_print_type(stderr, t2, true);
		fprintf(stderr, "\n");
		return 0;
	}
}

Type_Error type_update_weak(Ast* expr) {
	Type_Error error = TYPE_OK;
	Type_Instance* type = resolve_type(expr->scope, expr->type_return, true);
	if(!type) {
		return (error |= TYPE_ERROR_FATAL);
	}
	return type_update_weak(expr, type);
}

Type_Error type_update_weak(Ast* expr, Type_Instance* strong) {
	assert(expr->flags & AST_FLAG_IS_EXPRESSION);
	Type_Error error = TYPE_OK;
	
	switch (expr->node_type) {
		case AST_EXPRESSION_BINARY:{
			expr->type_return = strong;

			// This expression is weak, meaning it can't have pointer, struct '.' nor boolean type
			// and so both sides of a binary expression must be of the same type
			//														- PSV 1 Apr 2018
			error |= type_update_weak(expr->expr_binary.left, strong);
			error |= type_update_weak(expr->expr_binary.right, strong);
		}break;
		case AST_EXPRESSION_LITERAL: {
			if(expr->type_return->flags & TYPE_FLAG_WEAK)
				expr->type_return = type_transform_weak_to_strong(expr->type_return, strong, expr, &error);
		}break;
		case AST_EXPRESSION_UNARY: {
			switch (expr->expr_unary.op) {
			case OP_UNARY_BITWISE_NOT:
			case OP_UNARY_MINUS:
			case OP_UNARY_PLUS: {
				error |= type_update_weak(expr->expr_unary.operand, strong);
			}break;

			case OP_UNARY_ADDRESSOF: {			// cannot be weak, because a variable operand has strong type
				error |= report_type_error(TYPE_ERROR_FATAL, expr, "type infer failed, tried to address an rvalue\n");
			}break;
			case OP_UNARY_DEREFERENCE: {		// cannot be weak, because a variable operand has strong type
				error |= report_type_error(TYPE_ERROR_FATAL, expr, "type infer failed, tried to dereference an rvalue\n");
			}break;
			case OP_UNARY_LOGIC_NOT: {		// cannot be weak, because bool is always strong
				error |= report_type_error(TYPE_ERROR_FATAL, expr, "type infer failed, logic '!' operator can only be used on booleans\n");
			}break;
			case OP_UNARY_CAST:				// cannot be weak by definition
				break;
			default: assert(0); break;
			}
		}break;

		case AST_EXPRESSION_VARIABLE:
		case AST_EXPRESSION_PROCEDURE_CALL: {
			// a type cast got here most likely, so do nothing, in case of error, type_check will catch
		}break;
	}

	return error;
}

Type_Instance* type_transform_weak_to_strong(Type_Instance* weak, Type_Instance* strong, Ast* expr, Type_Error* error) {
	assert(weak->flags & TYPE_FLAG_WEAK);
	//assert(type_hash(weak) != type_hash(strong));

	switch (weak->kind) {
		case KIND_PRIMITIVE: {
			if(strong->kind != KIND_PRIMITIVE){
				// cannot infer from expression the type of this weak node, so strengthen it
				weak->flags |= TYPE_FLAG_RESOLVED;
				return internalize_type(&weak, true);
			}
			if (type_primitive_int(weak) && type_primitive_int(strong)) {
				return strong;
			}
			if (type_primitive_float(weak) && type_primitive_float(strong)) {
				return strong;
			}
			// types are not compatible, strenghten weak type and let the type check error out
			weak->flags |= TYPE_FLAG_RESOLVED;
			return internalize_type(&weak, true);
		}break;
		case KIND_STRUCT:
		case KIND_ARRAY:
		case KIND_POINTER: {
			return weak;
			//report_error_location(expr);
			//*error |= report_type_mismatch(weak, strong);
		}break;
		case KIND_FUNCTION: assert(0); break; // internal compiler error
	}
	return 0;
}