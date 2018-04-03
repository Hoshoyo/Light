#include "type_infer.h"
#include "decl_check.h"

Decl_Error report_type_mismatch(Type_Instance* t1, Type_Instance* t2) {
	report_semantic_error(DECL_ERROR_FATAL, "type mismatch ");
	DEBUG_print_type(stderr, t1, true);
	fprintf(stderr, " vs ");
	DEBUG_print_type(stderr, t2, true);
	return DECL_ERROR_FATAL;
}

Type_Instance* type_strength_resolve(Type_Instance* t1, Type_Instance* t2, Ast* expr1, Ast* expr2, Decl_Error* error);
Type_Instance* infer_from_binary_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval = false);
Type_Instance* infer_from_unary_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval = false);
Type_Instance* infer_from_literal_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval = false);
Type_Instance* infer_from_variable_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval = false);
Type_Instance* infer_from_proc_call_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval = false);

Type_Instance* infer_from_binary_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval) {
	Type_Instance* left  = infer_from_expression(expr->expr_binary.left, error, rep_undeclared, lval);
	Type_Instance* right = infer_from_expression(expr->expr_binary.right, error, rep_undeclared, lval);

	if (!left || !right)
		return 0;

	switch (expr->expr_binary.op) {
		case OP_BINARY_MOD:{
			if (type_primitive_int(left) && type_primitive_int(right)) {
				Type_Instance* inferred = type_strength_resolve(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
				if (*error & DECL_ERROR_FATAL) return 0;
				return inferred;
			}
			report_error_location(expr);
			*error |= report_type_error(DECL_ERROR_FATAL, "binary '%' operator is not defined for the type '");
			DEBUG_print_type(stderr, left, true);
			fprintf(stderr, "'\n");
		}break;
		case OP_BINARY_PLUS:
		case OP_BINARY_MINUS: {
			if (left->kind == KIND_POINTER && type_primitive_int(right)) {
				assert(left->flags & TYPE_FLAG_INTERNALIZED);
				return left;
			}
			
			if ((left->kind == KIND_POINTER) && (right->kind == KIND_POINTER) && (left == right)) {
				if (expr->expr_binary.op == OP_BINARY_MINUS) {
					// difference of pointers gives an u64
					return type_primitive_get(TYPE_PRIMITIVE_U64);
				} else {
					report_error_location(expr);
					*error |= report_type_error(DECL_ERROR_FATAL, "cannot add two pointer types\n");
					return 0;
				}
			}
		}
		case OP_BINARY_MULT:
		case OP_BINARY_DIV: {
			if ((type_primitive_int(left) && type_primitive_int(right)) ||
				(type_primitive_float(left) && type_primitive_float(right))) 
			{
				Type_Instance* inferred = type_strength_resolve(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
				if (*error & DECL_ERROR_FATAL) return 0;
				return inferred;
			}
			report_error_location(expr);
			if (type_hash(left) == type_hash(right)) {
				*error |= report_type_error(DECL_ERROR_FATAL, "binary arithmetic expressions are not defined for the type '");
				DEBUG_print_type(stderr, left, true);
				fprintf(stderr, "'\n");
			} else {
				*error |= report_type_mismatch(left, right);
				fprintf(stderr, "\n");
			}
		}break;
		case OP_BINARY_XOR: {
			if (type_primitive_bool(left) && type_primitive_bool(right)) {
				return left;
			}
		}
		case OP_BINARY_AND:
		case OP_BINARY_OR:
		case OP_BINARY_SHL:
		case OP_BINARY_SHR: {
			if (type_primitive_int(left) && type_primitive_int(right)) 
			{
				Type_Instance* inferred = type_strength_resolve(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
				if (*error & DECL_ERROR_FATAL) return 0;
				return inferred;
			}
			report_error_location(expr);
			if (type_hash(left) == type_hash(right)) {
				*error |= report_type_error(DECL_ERROR_FATAL, "binary bitwise expressions are not defined for the type '");
				DEBUG_print_type(stderr, left, true);
				fprintf(stderr, "'\n");
			} else {
				*error |= report_type_mismatch(left, right);
				fprintf(stderr, "\n");
			}
		}break;

		case OP_BINARY_LOGIC_AND:
		case OP_BINARY_LOGIC_OR: {
			return type_primitive_get(TYPE_PRIMITIVE_BOOL);
		}break;

		case OP_BINARY_EQUAL:
		case OP_BINARY_NOT_EQUAL: {
			if (type_primitive_bool(left) && type_primitive_bool(right)) {
				return left;
			}
		}
		case OP_BINARY_GE:
		case OP_BINARY_GT:
		case OP_BINARY_LE:
		case OP_BINARY_LT: {
			Type_Instance* booltype = type_primitive_get(TYPE_PRIMITIVE_BOOL);
			if ((type_primitive_int(left) && type_primitive_int(right)) ||
				(type_primitive_float(left) && type_primitive_float(right))) 
			{
				Type_Instance* inferred = type_strength_resolve(left, right, expr->expr_binary.left, expr->expr_binary.right, error);
				if (*error & DECL_ERROR_FATAL) return 0;
				expr->type_return = booltype;
				return booltype;
			} else {
				if (type_hash(left) == type_hash(right)) {
					expr->type_return = booltype;
					return booltype;
				} else {
					report_error_location(expr);
					*error = report_type_mismatch(left, right);
					fprintf(stderr, "\n");
				}
			}
		}break;
		case OP_BINARY_VECTOR_ACCESS: {
			// right is index
			Type_Instance* index_type = infer_from_expression(expr->expr_binary.right, error, rep_undeclared);
			if (*error & DECL_ERROR_FATAL) return 0;
			expr->expr_binary.right->type_return = index_type;

			Type_Instance* operand_type = infer_from_expression(expr->expr_binary.left, error, rep_undeclared, lval);
			// left is indexed
			if (*error & DECL_ERROR_FATAL) return 0;
			if (operand_type->kind == KIND_POINTER) {
				Type_Instance* res = operand_type->pointer_to;
				assert(res->flags & TYPE_FLAG_INTERNALIZED);
				if (res->kind == KIND_ARRAY) {
					res = res->array_desc.array_of;
				}
				return res;
			} else {
				report_error_location(expr);
				*error |= report_type_error(DECL_ERROR_FATAL, "cannot dereference a non pointer type '");
				DEBUG_print_type(stderr, operand_type, true);
				fprintf(stderr, "'\n");
				return 0;
			}
		}break;

		case OP_BINARY_DOT: {
			// TODO(psv): implement
			assert(0);
		}break;
	}
	return 0;
}

Type_Instance* infer_from_unary_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval) {
	assert(expr->node_type == AST_EXPRESSION_UNARY);

	switch (expr->expr_unary.op) {
		case OP_UNARY_CAST:{
			Type_Instance* res = resolve_type(expr->scope, expr->expr_unary.type_to_cast, rep_undeclared);
			if (!res) {
				*error |= report_type_error(DECL_ERROR_FATAL, "could not infer type of cast, type '");
				DEBUG_print_type(stderr, expr->expr_unary.type_to_cast, true);
				fprintf(stderr, "' is invalid\n");
			}
			return res;
		}break;
		case OP_UNARY_ADDRESSOF: {
			// TODO(psv): check if it can only ask for address of lvalue (strong type)
			Type_Instance* res = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if (*error & DECL_ERROR_FATAL) return 0;
			if (!res) return 0;

			Type_Instance* newtype = type_new_temporary();
			newtype->kind = KIND_POINTER;
			newtype->pointer_to = res;
			newtype->type_size_bits = type_pointer_size() * 8;
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
			if (*error & DECL_ERROR_FATAL) return 0;
			if (!operand_type) return 0;

			if (operand_type->kind != KIND_POINTER) {
				report_error_location(expr);
				*error |= report_type_error(DECL_ERROR_FATAL, "cannot dereference a non pointer type '");
				DEBUG_print_type(stderr, operand_type, true);
				fprintf(stderr, "'\n");
			}
			Type_Instance* res = operand_type->pointer_to;
			assert(res->flags & TYPE_FLAG_INTERNALIZED);
			return res;
		}break;
		case OP_UNARY_LOGIC_NOT: {
			Type_Instance* operand_type = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if (*error & DECL_ERROR_FATAL) return 0;
			if (!operand_type) return 0;

			if (type_primitive_bool(operand_type)) {
				assert(operand_type->flags & TYPE_FLAG_INTERNALIZED);
				return operand_type;
			} else {
				*error |= report_type_error(DECL_ERROR_FATAL, "unary operator '!' can only be used in a boolean type\n");
				return 0;
			}
		}break;
		case OP_UNARY_BITWISE_NOT: {
			Type_Instance* operand_type = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if (*error & DECL_ERROR_FATAL) return 0;
			if (!operand_type) return 0;

			if (type_primitive_int(operand_type)) {
				return operand_type;
			} else {
				*error |= report_type_error(DECL_ERROR_FATAL, "unary operator not '~' can only be used in integer types\n");
				return 0;
			}
		}break;
		case OP_UNARY_PLUS:
		case OP_UNARY_MINUS: {
			Type_Instance* operand_type = infer_from_expression(expr->expr_unary.operand, error, rep_undeclared, lval);
			if (*error & DECL_ERROR_FATAL) return 0;
			if (!operand_type) return 0;

			if (type_primitive_int(operand_type) || type_primitive_float(operand_type)) {
				return operand_type;
			} else {
				*error |= report_type_error(DECL_ERROR_FATAL, "unary operators '+' or '-' can only be used in numeric types\n");
				return 0;
			}
		}break;
	}
	return 0;
}

Type_Instance* infer_from_literal_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval) {
	Type_Instance* result = type_new_temporary();
	result->flags = 0 | TYPE_FLAG_WEAK;
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

Type_Instance* infer_from_variable_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval) {
	assert(expr->node_type == AST_EXPRESSION_VARIABLE);

	Ast* decl = decl_from_name(expr->scope, expr->expr_variable.name);
	if (!decl) {
		if(rep_undeclared)
			*error |= report_undeclared(expr->expr_variable.name);
		return 0;
	}
	if (decl->node_type != AST_DECL_VARIABLE && decl->node_type != AST_DECL_CONSTANT) {
		*error |= report_semantic_error(DECL_ERROR_FATAL, 
			"could not infer type from name '%.*s', it is not a variable nor constant\n", TOKEN_STR(expr->expr_variable.name));
		return 0;
	}
	Type_Instance* type = decl->decl_variable.variable_type;
	if (lval) {
		// transform this in the pointer type
		Type_Instance* ptrtype = type_new_temporary();
		ptrtype->kind = KIND_POINTER;
		ptrtype->type_size_bits = type_pointer_size() * 8;
		ptrtype->flags = TYPE_FLAG_SIZE_RESOLVED | TYPE_FLAG_RESOLVED;
		ptrtype->pointer_to = type;
		return internalize_type(&ptrtype, true);
	}

	return type;
}

Type_Instance* infer_from_proc_call_expr(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval) {
	Ast* decl = decl_from_name(expr->scope, expr->expr_proc_call.name);
	if (!decl) {
		*error |= report_undeclared(expr->expr_proc_call.name);
		return 0;
	}
	if (decl->node_type != AST_DECL_PROCEDURE) {
		*error |= report_semantic_error(DECL_ERROR_FATAL, 
			"'%.*s' used in a procedure call, but is not a procedure\n", TOKEN_STR(expr->expr_proc_call.name));
		return 0;
	}
	Type_Instance* type = decl->decl_procedure.type_return;
	assert(type->flags & TYPE_FLAG_INTERNALIZED);
	return type;
}

Type_Instance* infer_from_expression(Ast* expr, Decl_Error* error, bool rep_undeclared, bool lval) {
	assert(expr->flags & AST_FLAG_IS_EXPRESSION);
	
	Type_Instance* type = 0;
	
	switch (expr->node_type) {
		case AST_EXPRESSION_BINARY:				type = infer_from_binary_expr(expr, error, rep_undeclared, lval); break;
		case AST_EXPRESSION_UNARY:				type = infer_from_unary_expr(expr, error, rep_undeclared, lval); break;
		case AST_EXPRESSION_LITERAL:			type = infer_from_literal_expr(expr, error, rep_undeclared, lval); break;
		case AST_EXPRESSION_VARIABLE:			type = infer_from_variable_expr(expr, error, rep_undeclared, lval); break;
		case AST_EXPRESSION_PROCEDURE_CALL:		type = infer_from_proc_call_expr(expr, error, rep_undeclared, lval); break;
	}
	expr->type_return = type;
	return type;
}

Type_Instance* type_transform_weak_to_strong(Type_Instance* weak, Type_Instance* strong, Ast* expr, Decl_Error* error);

Decl_Error type_update_weak(Ast* expr, Type_Instance* strong) {
	assert(expr->flags & AST_FLAG_IS_EXPRESSION);
	Decl_Error error = DECL_OK;
	
	switch (expr->node_type) {
		case AST_EXPRESSION_BINARY:{
			expr->type_return = strong;

			// This expression is weak, meaning it can't have pointer, struct '.' nor boolean type
			// and so both sides of a binary expression must be of the same type
			//														- PSV 1 Apr 2018
			error |= type_update_weak(expr->expr_binary.left, strong);
			error |= type_update_weak(expr->expr_binary.left, strong);
		}break;
		case AST_EXPRESSION_LITERAL: {
			expr->type_return = type_transform_weak_to_strong(expr->type_return, strong, expr, &error);
		}break;
		case AST_EXPRESSION_UNARY: {
			switch (expr->expr_unary.op) {
			case OP_UNARY_BITWISE_NOT:
			case OP_UNARY_MINUS:
			case OP_UNARY_PLUS: {
				error |= type_update_weak(expr->expr_unary.operand, strong);
			}break;

			case OP_UNARY_ADDRESSOF:		// cannot be weak, because a variable operand has strong type
			case OP_UNARY_CAST:				// cannot be weak by definition
			case OP_UNARY_DEREFERENCE:		// cannot be weak, because a variable operand has strong type
			case OP_UNARY_LOGIC_NOT:		// cannot be weak, because bool is always strong
				assert(0); break;
			}
		}break;

		case AST_EXPRESSION_VARIABLE:
		case AST_EXPRESSION_PROCEDURE_CALL: {
			assert(0); break; // a procedure call or variable can never be weak, and so it should not get here.
		}break;
	}

	return error;
}

Type_Instance* type_transform_weak_to_strong(Type_Instance* weak, Type_Instance* strong, Ast* expr, Decl_Error* error) {
	assert(weak->flags & TYPE_FLAG_WEAK);
	assert(type_hash(weak) != type_hash(strong));

	switch (weak->kind) {
		case KIND_PRIMITIVE: {
			if (type_primitive_int(weak) || type_primitive_float(weak))
				return strong;
			report_error_location(expr);
			*error |= report_type_mismatch(weak, strong);
		}break;
		case KIND_STRUCT:
		case KIND_ARRAY:
		case KIND_POINTER: {
			report_error_location(expr);
			*error |= report_type_mismatch(weak, strong);
		}break;
		case KIND_FUNCTION: assert(0); break; // internal compiler error
	}
	return 0;
}

// Resolve the strength of a type TODO(psv): "and updated its children when necessary."
// the returned type is always the correct type, unless an error occurred, then this function returns 0
Type_Instance* type_strength_resolve(Type_Instance* t1, Type_Instance* t2, Ast* expr1, Ast* expr2, Decl_Error* error) {
	if (t1->flags & TYPE_FLAG_WEAK && t2->flags & TYPE_FLAG_WEAK) {
		if (type_hash(t1) == type_hash(t2)) {
			return t1;
		} else {
			report_error_location(expr1);
			*error |= report_semantic_error(DECL_ERROR_FATAL, "could not infer type from two different weak types: ");
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

		report_error_location(expr1);
		*error |= report_semantic_error(DECL_ERROR_FATAL, "type mismatch ");
		DEBUG_print_type(stderr, t1, true);
		fprintf(stderr, " vs ");
		DEBUG_print_type(stderr, t2, true);
		fprintf(stderr, "\n");
		return 0;
	}
}


/*	-------------------------------------------------------------
	---------------------- Type checking ------------------------
	------------------------------------------------------------- */

Type_Error report_type_check_error(Type_Error e, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Type Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}

Type_Error report_type_mismatch(Ast* node, Type_Instance* t1, Type_Instance* t2) {
	report_error_location(node);
	Type_Error err = report_type_error(TYPE_ERROR_MISMATCH, "type mismatch '");
	DEBUG_print_type(stderr, t1, true);
	fprintf(stderr, "' vs '");
	DEBUG_print_type(stderr, t2, true);
	fprintf(stderr, "'\n");
	return err;
}

Type_Instance* scope_get_function_type(Scope* scope) {
	while (scope) {
		if (scope->flags & SCOPE_PROCEDURE_BODY) {
			assert(scope->creator_node->node_type == AST_COMMAND_BLOCK);
			Ast* decl = scope->creator_node->comm_block.creator;
			assert(decl);
			return decl->decl_procedure.type_return;
		}
		scope = scope->parent;
	}
	return 0;
}

Type_Error type_check(Ast* node) {
	Type_Error error = TYPE_OK;

	switch (node->node_type) {
		// Declarations
		case AST_DECL_CONSTANT:{
			if (node->decl_constant.type_info != node->decl_constant.value->type_return) {
				error |= type_check(node->decl_constant.value);
				error |= report_type_mismatch(node, node->decl_constant.type_info, node->decl_constant.value->type_return);
			}
		} break;
		case AST_DECL_VARIABLE: {
			if (node->decl_variable.assignment && (node->decl_variable.variable_type != node->decl_variable.assignment->type_return)) {
				error |= type_check(node->decl_variable.assignment);
				error |= report_type_mismatch(node, node->decl_variable.variable_type, node->decl_variable.assignment->type_return);
			}
		}break;
		case AST_DECL_ENUM: {
			for (size_t i = 0; i < node->decl_enum.fields_count; ++i) {
				error |= type_check(node->decl_enum.fields[i]);
			}
		}break;
		case AST_DECL_PROCEDURE: {
			for (size_t i = 0; i < node->decl_procedure.arguments_count; ++i) {
				error |= type_check(node->decl_procedure.arguments[i]);
			}
			error |= type_check(node->decl_procedure.body);
		}break;
		case AST_DECL_STRUCT: {
			for (size_t i = 0; i < node->decl_struct.fields_count; ++i) {
				error |= type_check(node->decl_struct.fields[i]);
			}
		}break;
		case AST_DECL_UNION: {
			for (size_t i = 0; i < node->decl_union.fields_count; ++i) {
				error |= type_check(node->decl_union.fields[i]);
			}
		}break;

		// Commands
		case AST_COMMAND_BLOCK: {
			for (size_t i = 0; i < node->comm_block.command_count; ++i) {
				error |= type_check(node->comm_block.commands[i]);
			}
		}break;
		case AST_COMMAND_FOR: {
			if (node->comm_for.condition->type_return != type_primitive_get(TYPE_PRIMITIVE_BOOL)) {
				report_error_location(node->comm_for.condition);
				error |= report_type_check_error(TYPE_ERROR_FATAL, "for condition must have boolean type\n");
			}
			error |= type_check(node->comm_for.condition);
			error |= type_check(node->comm_for.body);
		}break;
		case AST_COMMAND_IF: {
			if (node->comm_if.condition->type_return != type_primitive_get(TYPE_PRIMITIVE_BOOL)) {
				assert(node->comm_if.condition->type_return);
				report_error_location(node->comm_if.condition);
				error |= report_type_check_error(TYPE_ERROR_FATAL, "if condition must have boolean type but expression type is '");
				DEBUG_print_type(stderr, node->comm_if.condition->type_return, true);
				fprintf(stderr, "'\n");
			}
			error |= type_check(node->comm_if.body_true);
			if (node->comm_if.body_false) {
				error |= type_check(node->comm_if.body_false);
			}
		}break;
		case AST_COMMAND_RETURN: {
			Type_Instance* rettype = scope_get_function_type(node->scope);
			if (!rettype) {
				report_error_location(node);
				error |= report_type_check_error(TYPE_ERROR_FATAL, "command return is not inside a procedure body\n");
			} else {
				if (rettype == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
					if (node->comm_return.expression) {
						report_error_location(node);
						error |= report_type_check_error(TYPE_ERROR_FATAL, "non-void return statement of procedure returning void\n");
					}
				} else if (rettype != node->comm_return.expression->type_return) {
					if (node->comm_return.expression->type_return->flags & TYPE_FLAG_WEAK) {
						Type_Instance* t = type_transform_weak_to_strong(node->comm_return.expression->type_return, rettype, node->comm_return.expression, &error);
						if (error & TYPE_ERROR_FATAL) return error;
						assert(t);
						node->comm_return.expression->type_return = t;
					} else {
						error |= report_type_mismatch(node->comm_return.expression, rettype, node->comm_return.expression->type_return);
					}
				}
			}
			error |= type_check(node->comm_return.expression);
		}break;
		case AST_COMMAND_VARIABLE_ASSIGNMENT: {

		}break;
		case AST_COMMAND_BREAK:
		case AST_COMMAND_CONTINUE:

		case AST_EXPRESSION_BINARY:
		case AST_EXPRESSION_LITERAL:
		case AST_EXPRESSION_PROCEDURE_CALL:
		case AST_EXPRESSION_UNARY:
		case AST_EXPRESSION_VARIABLE:
			break;
	}

	return error;
}

Type_Error type_check(Scope* scope, Ast** ast) {
	size_t ndecl = array_get_length(ast);
	Type_Error error = TYPE_OK;

	for (size_t i = 0; i < ndecl; ++i) {
		Ast* node = ast[i];
		error |= type_check(node);
	}
	return error;
}