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

static bool scope_inside_loop(Scope* scope) {
	while (scope) {
		if (scope->flags & SCOPE_LOOP) return true;
		if (scope->flags & SCOPE_PROCEDURE_BODY) return false;
		if (scope->flags & SCOPE_ENUM) return false;
		if (scope->flags & SCOPE_STRUCTURE) return false;
		scope = scope->parent;
	}
	return false;
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
		} else {
			if (rettype == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
				if (node->comm_return.expression) {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "non-void return statement of procedure returning void\n");
					return error;
				}
			} else {
				if (!node->comm_return.expression) {
					if (rettype != type_primitive_get(TYPE_PRIMITIVE_VOID)) {
						error |= report_type_error(TYPE_ERROR_FATAL, node, "expected return value of type '");
						DEBUG_print_type(stderr, rettype, true);
						fprintf(stderr, "'\n");
						return error;
					}
				} else if (node->comm_return.expression->type_return->flags & TYPE_FLAG_WEAK) {
					type_propagate(rettype, node->comm_return.expression);
					if (error & TYPE_ERROR_FATAL) return error;
				}
				if (rettype != node->comm_return.expression->type_return) {
					error |= report_type_mismatch(node->comm_return.expression, rettype, node->comm_return.expression->type_return);
				}
			}
		}
	}break;
	case AST_COMMAND_VARIABLE_ASSIGNMENT: {

	}break;
	case AST_COMMAND_BREAK: {
		Type_Instance* break_lvl_type = node->comm_break.level->type_return;
		if (!type_primitive_int(break_lvl_type)) {
			error |= report_type_error(TYPE_ERROR_FATAL, node->comm_break.level, "break statement argument must be an integer literal > 0\n");
		}
	}break;
	case AST_COMMAND_CONTINUE: {
		if (!scope_inside_loop(node->scope)) {
			error |= report_type_error(TYPE_ERROR_FATAL, node, "continue statement must be inside a loop\n");
		}
	}break;

	default: {
	}break;
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
