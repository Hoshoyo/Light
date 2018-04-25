#include "type_check.h"
#include "type_table.h"
#include "decl_check.h"
#include "type_infer.h"

/*	-------------------------------------------------------------
---------------------- Type checking ------------------------
------------------------------------------------------------- */

static Type_Error report_type_mismatch(Ast* node, Type_Instance* t1, Type_Instance* t2) {
	report_error_location(node);
	Type_Error err = report_type_error(TYPE_ERROR_FATAL, "type mismatch '");
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
	case AST_DECL_CONSTANT: {
		if (node->decl_constant.type_info != node->decl_constant.value->type_return) {
			error |= type_check(node->decl_constant.value);
			error |= report_type_mismatch(node, node->decl_constant.type_info, node->decl_constant.value->type_return);
		}
	} break;
	case AST_DECL_VARIABLE: {
		if (node->decl_variable.assignment) {
			error |= type_check(node->decl_variable.assignment);
		}
		if (node->decl_variable.assignment && (node->decl_variable.variable_type != node->decl_variable.assignment->type_return)) {
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
		if (node->decl_procedure.body) {
			error |= type_check(node->decl_procedure.body);
		}
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
			error |= report_type_error(TYPE_ERROR_FATAL, node->comm_for.condition, "for condition must have boolean type\n");
		}
		error |= type_check(node->comm_for.condition);
		error |= type_check(node->comm_for.body);
	}break;
	case AST_COMMAND_IF: {
		if (node->comm_if.condition->type_return != type_primitive_get(TYPE_PRIMITIVE_BOOL)) {
			assert(node->comm_if.condition->type_return);
			error |= report_type_error(TYPE_ERROR_FATAL, node->comm_if.condition, "if condition must have boolean type but expression type is '");
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
			error |= report_type_error(TYPE_ERROR_FATAL, node, "command return is not inside a procedure body\n");
		}
		else {
			if (rettype == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
				if (node->comm_return.expression) {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "non-void return statement of procedure returning void\n");
				}
			}
			else if (rettype != node->comm_return.expression->type_return) {
				if (node->comm_return.expression->type_return->flags & TYPE_FLAG_WEAK) {
					Type_Instance* t = 0;//type_transform_weak_to_strong(node->comm_return.expression->type_return, rettype, node->comm_return.expression, &error);
					if (error & TYPE_ERROR_FATAL) return error;
					assert(t);
					node->comm_return.expression->type_return = t;
				}
				else {
					error |= report_type_mismatch(node->comm_return.expression, rettype, node->comm_return.expression->type_return);
				}
			}
		}
		if (node->comm_return.expression)
			error |= type_check(node->comm_return.expression);
	}break;
	case AST_COMMAND_VARIABLE_ASSIGNMENT: {
		Type_Instance* lvalue_type = 0;
		if (node->comm_var_assign.lvalue) {
			lvalue_type = node->comm_var_assign.lvalue->type_return;
			Type_Instance* rvalue_type = node->comm_var_assign.rvalue->type_return;

			if (lvalue_type->kind != KIND_POINTER || (lvalue_type->flags & TYPE_FLAG_WEAK)) {
				error |= report_type_error(TYPE_ERROR_FATAL, node, "left side of assignment is not an addressable value\n");
			}
			else {
				if (lvalue_type->pointer_to != rvalue_type) {
					error |= report_type_mismatch(node, lvalue_type->pointer_to, rvalue_type);
				}
			}
			error |= type_check(node->comm_var_assign.lvalue);

			error |= type_check(node->comm_var_assign.rvalue);
			if (error & TYPE_ERROR_FATAL) break;
			if (node->comm_var_assign.rvalue->type_return != lvalue_type->pointer_to) {
				error |= report_type_mismatch(node, lvalue_type->pointer_to, node->comm_var_assign.rvalue->type_return);
			}
		}
		else {
			lvalue_type = type_primitive_get(TYPE_PRIMITIVE_VOID);
			error |= type_check(node->comm_var_assign.rvalue);
			if (error & TYPE_ERROR_FATAL) break;
			if (node->comm_var_assign.rvalue->type_return != lvalue_type) {
				error |= report_type_error(TYPE_ERROR_FATAL, node, "expected void type in rvalue expression\n");
			}
		}

	}break;
	case AST_COMMAND_BREAK: {
		Type_Instance* break_lvl_type = node->comm_break.level->type_return;
		if (!type_primitive_int(break_lvl_type)) {
			report_error_location(node->comm_break.level);
			error |= report_type_error(TYPE_ERROR_FATAL, node->comm_break.level, "break statement argument must be an integer literal > 0\n");
		}
	}break;
	case AST_COMMAND_CONTINUE: break;

	case AST_EXPRESSION_BINARY: {
		error |= type_check(node->expr_binary.left);
		error |= type_check(node->expr_binary.right);

		if (error & TYPE_ERROR_FATAL) return error;

		Type_Instance* left_type = node->expr_binary.left->type_return;
		Type_Instance* right_type = node->expr_binary.right->type_return;
		if (left_type->flags & TYPE_FLAG_WEAK) {
			left_type->flags |= TYPE_FLAG_RESOLVED;
			left_type = resolve_type(node->scope, left_type, false);
			left_type = internalize_type(&left_type);
		}
		if (right_type->flags & TYPE_FLAG_WEAK) {
			right_type->flags |= TYPE_FLAG_RESOLVED;
			right_type = resolve_type(node->scope, right_type, false);
			right_type = internalize_type(&right_type);
		}
		switch (node->expr_binary.op) {
		case OP_BINARY_PLUS:
		case OP_BINARY_MINUS: {
			if (left_type->kind == KIND_POINTER) {
				if (right_type->kind == KIND_POINTER) {
					if (node->expr_binary.op == OP_BINARY_PLUS) {
						// ^T + ^T |-> error
						error |= report_type_error(TYPE_ERROR_FATAL, node, "pointer type cannot be added to another pointer type\n");
					}
					else {
						// ^T - ^T |-> s64
						assert(node->type_return == type_primitive_get(TYPE_PRIMITIVE_S64));
						if (left_type != right_type) {
							error |= report_type_error(TYPE_ERROR_FATAL, node, "type mismatch in pointer arithmetic '-', '");
							DEBUG_print_type(stderr, left_type, true);
							fprintf(stderr, "' vs '");
							DEBUG_print_type(stderr, right_type, true);
							fprintf(stderr, "\n");
						}
					}
				}
				else if (type_primitive_int(right_type)) {
					// ^T + INT |-> ^T
					assert(node->type_return == left_type);
				}
				else {
					if (node->expr_binary.op == OP_BINARY_PLUS) {
						error |= report_type_error(TYPE_ERROR_FATAL, node, "pointer arithmetic type mismatch, trying to add '");
						DEBUG_print_type(stderr, left_type, true);
						fprintf(stderr, "' to ");
						DEBUG_print_type(stderr, right_type, true);
					}
					else {
						error |= report_type_error(TYPE_ERROR_FATAL, node, "pointer arithmetic type mismatch, trying to subtract '");
						DEBUG_print_type(stderr, right_type, true);
						fprintf(stderr, "' from ");
						DEBUG_print_type(stderr, left_type, true);
					}
					fprintf(stderr, "\n");
				}
			}
			else {
				// TODO(psv): add coercions of integer and floating point types to bigger versions of themselves
				if (left_type != right_type) {
					error |= report_type_mismatch(node, left_type, right_type);
				}
			}
		}break;
		case OP_BINARY_MULT:
		case OP_BINARY_DIV: {
			// TODO(psv): add coercions of integer and floating point types to bigger versions of themselves
			if (left_type != right_type) {
				error |= report_type_mismatch(node, left_type, right_type);
			} else {
				if (type_primitive_int(left_type) || type_primitive_float(left_type)) {
					// NUMERIC * NUMERIC |-> NUMERIC
					assert(node->type_return == left_type);
				} else {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "binary operators '*' and '/' can only be used on numeric types\n");
				}
			}
		}break;
		case OP_BINARY_MOD: {
			if (left_type != right_type) {
				error |= report_type_mismatch(node, left_type, right_type);
			}
			else {
				if (type_primitive_int(left_type)) {
					// INT % INT |-> INT
				}
				else {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "binary operator '%%' requires integer types\n");
				}
			}
		}break;
		case OP_BINARY_AND:
		case OP_BINARY_OR: {
			if (left_type != right_type) {
				error |= report_type_mismatch(node, left_type, right_type);
			}
			else {
				// TODO(psv): allow only unsigned here? maybe signed?
				if (type_primitive_int_unsigned(left_type)) {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "binary operators '&' and '|' require unsigned integer types\n");
				}
			}
		}break;
		case OP_BINARY_XOR: {
			if (left_type != right_type) {
				error |= report_type_mismatch(node, left_type, right_type);
			}
			else {
				if (type_primitive_int(left_type) || type_primitive_bool(left_type)) {
					// UINT ^ UINT |-> UINT
					// BOOL ^ BOOL |-> BOOL
				}
				else {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "binary operator 'xor' can only be used on boolean or integer types\n");
				}
			}
		}break;
		case OP_BINARY_EQUAL:
		case OP_BINARY_NOT_EQUAL:
		case OP_BINARY_LE:
		case OP_BINARY_LT:
		case OP_BINARY_GE:
		case OP_BINARY_GT: {
			if (left_type != right_type) {
				error |= report_type_mismatch(node, left_type, right_type);
			}
			else {
				if (type_primitive_int(left_type) || type_primitive_float(left_type)) {
					// INT compare INT     |-> BOOL
					// FLOAT compare FLOAT |-> BOOL
					assert(node->type_return == type_primitive_get(TYPE_PRIMITIVE_BOOL));
				}
				else {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "binary comparison operators can only be used on numeric types\n");
				}
			}
		}break;
		case OP_BINARY_LOGIC_AND:
		case OP_BINARY_LOGIC_OR: {
			if ((left_type != right_type) || left_type != type_primitive_get(TYPE_PRIMITIVE_BOOL)) {
				error |= report_type_error(TYPE_ERROR_FATAL, node, "binary logic operators '&&' and '||' can only be used on boolean type\n");
			}
		}break;
		case OP_BINARY_DOT: {
			// TODO(psv): implement
		}break;
		case OP_BINARY_VECTOR_ACCESS: {
			// TODO(psv): implement
		}break;
		}
	}break;
	case AST_EXPRESSION_LITERAL: break;

	case AST_EXPRESSION_PROCEDURE_CALL: {
		Ast* decl = decl_from_name(node->scope, node->expr_proc_call.name);
		size_t nargs = node->expr_proc_call.args_count;
		size_t nargs_expected = decl->decl_procedure.arguments_count;

		if (nargs < nargs_expected) {
			error |= report_type_error(TYPE_ERROR_FATAL, node, "procedure call missing arguments, expected '%lld' got '%lld'\n",
				nargs_expected, nargs);
		}
		else if (nargs > nargs_expected) {
			error |= report_type_error(TYPE_ERROR_FATAL, node, "too many arguments for procedure call, expected '%lld' got '%lld'\n",
				nargs_expected, nargs);
		}
		else {
			// Type check each argument
			for (size_t i = 0; i < nargs; ++i) {
				Ast* expr = node->expr_proc_call.args[i];
				if (expr->type_return->flags & TYPE_FLAG_WEAK) {
					error |= type_update_weak(expr, decl->decl_procedure.type_procedure->function_desc.arguments_type[i]);
					if (error & TYPE_ERROR_FATAL) continue;
				}
				if (expr->type_return != decl->decl_procedure.type_procedure->function_desc.arguments_type[i]) {
					error |= report_type_error(TYPE_ERROR_FATAL, "type mismatch in argument #%lld of '%.*s' procedure call. '",
						i + 1, TOKEN_STR(node->expr_proc_call.name));
					DEBUG_print_type(stderr, expr->type_return);
					fprintf(stderr, "' vs '");
					DEBUG_print_type(stderr, decl->decl_procedure.type_procedure->function_desc.arguments_type[i]);
					fprintf(stderr, "'\n");
				}
			}
		}
	}break;
	case AST_EXPRESSION_UNARY: {
		//assert_msg(0, "unary expression type check not implemented");
		assert(!(node->type_return->flags & TYPE_FLAG_WEAK));
		Ast* operand = node->expr_unary.operand;
		error |= type_check(operand);
		if (error & TYPE_ERROR_FATAL) return error;

		switch (node->expr_unary.op) {
		case OP_UNARY_ADDRESSOF: {
			assert(!(node->type_return->flags & TYPE_FLAG_WEAK));
			assert(node->type_return->kind == KIND_POINTER);
			assert(operand->type_return == node->type_return->pointer_to);
			// check addressable operand
			//if (!(node->flags & )) {
			//	error |= report_type_error(TYPE_ERROR_FATAL, node, "cannot address an rvalue\n");
			//}
		}break;

		case OP_UNARY_DEREFERENCE: {
			assert(!(node->type_return->flags & TYPE_FLAG_WEAK));
			assert(node->type_return == operand->type_return->pointer_to);
			if (operand->type_return->kind != KIND_POINTER) {
				error |= report_type_error(TYPE_ERROR_FATAL, node, "dereference operator requires a pointer type\n");
			}
		}break;

		case OP_UNARY_LOGIC_NOT: {
			assert(!(node->type_return->flags & TYPE_FLAG_WEAK));
			if (operand->type_return != type_primitive_get(TYPE_PRIMITIVE_BOOL)) {
				error |= report_type_error(TYPE_ERROR_FATAL, node, "unary operator '!' is only defined for the boolean type\n");
			}
		}break;

		case OP_UNARY_BITWISE_NOT: {
			if (!type_primitive_int(operand->type_return)) {
				error |= report_type_error(TYPE_ERROR_FATAL, node, "unary operator '~' is only defined for integer types\n");
			}
		}break;

		case OP_UNARY_MINUS:
		case OP_UNARY_PLUS: {
			if (!type_primitive_numeric(operand->type_return)) {
				error |= report_type_error(TYPE_ERROR_FATAL, node, "unary operators '+' and '-' are only defined for numeric types\n");
			}
		}break;
		case OP_UNARY_CAST: {
			Type_Instance* ttc = node->expr_unary.type_to_cast;
			Type_Instance* opt = operand->type_return;

			if (opt->flags & TYPE_FLAG_WEAK) {
				opt->flags |= TYPE_FLAG_RESOLVED;
				opt = resolve_type(node->scope, opt, false);
				operand->type_return = internalize_type(&opt);
				error |= type_update_weak(operand, opt);
			}
			error |= type_check(operand);

			if (ttc->kind == KIND_POINTER) {
				// cast(^T) ^V
				// cast(^T) INT
				if (!(opt->kind == KIND_POINTER || type_primitive_int(opt))) {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "unary cast to pointer type can only cast pointer or integer types\n");
				}
			}
			else if (ttc->kind == KIND_PRIMITIVE) {
				if (ttc->primitive == TYPE_PRIMITIVE_VOID) {
					// cast(void) T
					error |= report_type_error(TYPE_ERROR_FATAL, node, "unary cast operator does not allow casting to type void\n");
				}
				else if (operand->type_return->kind == KIND_POINTER) {
					if (!type_primitive_int(ttc)) {
						// cast(!INT) ^T
						error |= report_type_error(TYPE_ERROR_FATAL, node, "unary cast of pointer types to '");
						DEBUG_print_type(stderr, ttc, true);
						fprintf(stderr, "' is undefined\n");
					}
					else {
						// cast(INT) ^T
					}
				}
				else if (operand->type_return->kind == KIND_PRIMITIVE) {
					// cast(PRIM) PRIM
					switch (operand->type_return->primitive) {
					case TYPE_PRIMITIVE_BOOL: {// cast(INT) bool
					}break;
					case TYPE_PRIMITIVE_R32:
					case TYPE_PRIMITIVE_R64: {
						if (!type_primitive_numeric(ttc)) {
							error |= report_type_error(TYPE_ERROR_FATAL, node, "unary cast of floating point can only cast to numeric types\n");
						}
					}break;
					case TYPE_PRIMITIVE_S8:
					case TYPE_PRIMITIVE_S16:
					case TYPE_PRIMITIVE_S32:
					case TYPE_PRIMITIVE_S64:
					case TYPE_PRIMITIVE_U8:
					case TYPE_PRIMITIVE_U16:
					case TYPE_PRIMITIVE_U32:
					case TYPE_PRIMITIVE_U64: {
						if (!type_primitive_numeric(ttc)) {
							error |= report_type_error(TYPE_ERROR_FATAL, node, "unary cast of integer types can only cast to numeric types\n");
						}
					}break;
					case TYPE_PRIMITIVE_VOID: {
						error |= report_type_error(TYPE_ERROR_FATAL, node, "unary cast of void primitive is undefined\n");
					}break;
					}
				}
				else {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "unary cast operator cannot cast composite types to primitive\n");
				}
			}
			else {
				error |= report_type_error(TYPE_ERROR_FATAL, node, "unary cast operator cannot cast to type '");
				DEBUG_print_type(stderr, ttc, true);
			}
		}break;
		}
	}break;
	case AST_EXPRESSION_VARIABLE:
		//assert_msg(0, "expression variable type check not implemented");
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
