#include "decl_check.h"
#include "symbol_table.h"
#include "type_table.h"
#include "type_infer.h"
#include "lexer.h"
#include "error.h"

static Ast** infer_queue;
inline void infer_queue_push(Ast* node) {
	if (node->flags & AST_FLAG_QUEUED)
		return;
	node->flags |= AST_FLAG_QUEUED;
	node->infer_queue_index = array_push(infer_queue, &node);
}
inline void infer_queue_remove(Ast* node) {
	if (node->flags & AST_FLAG_QUEUED) {
		if(node->infer_queue_index != -1)
			array_remove(infer_queue, node->infer_queue_index);
	}
}

// ---------------------------------------------
// -------------- Declarations -----------------
// ---------------------------------------------

Decl_Error report_redeclaration(char* node_type, Token* redeclared, Token* existing) {
	report_decl_error(DECL_ERROR_FATAL, redeclared, "redeclaration of %s %.*s \n", node_type, TOKEN_STR(redeclared));
	fprintf(stderr, " - previously defined at: ");
	report_error_location(existing);
	fprintf(stderr, "\n");
	return DECL_ERROR_FATAL;
}
Decl_Error report_undeclared(Token* name){
	report_decl_error(DECL_ERROR_FATAL, name, "Undeclared identifier '%.*s'\n", TOKEN_STR(name));
	return DECL_ERROR_FATAL;
}

Decl_Error decl_check_redefinition(Scope* scope, Ast* node) {
	Decl_Error error = DECL_OK;

	Token* decl_name = 0;
	char*  decl_string_desc = 0;

	switch (node->node_type) {
		// TODO(psv): check forward declarations, they must not raise an error
		case AST_DECL_PROCEDURE: {
			error |= decl_insert_into_symbol_table(node, node->decl_procedure.name, "procedure");

			for (size_t i = 0; i < node->decl_procedure.arguments_count; ++i) {
				error |= decl_insert_into_symbol_table(node->decl_procedure.arguments[i],
					node->decl_procedure.arguments[i]->decl_variable.name, "procedure argument");
			}
		}break;
		case AST_DECL_ENUM: {
			error |= decl_insert_into_symbol_table(node, node->decl_enum.name, "enum");

			for (size_t i = 0; i < node->decl_enum.fields_count; ++i) {
				error |= decl_insert_into_symbol_table(node->decl_enum.fields[i],
					node->decl_enum.fields[i]->decl_constant.name, "enum field");
			}
		}break;
		case AST_DECL_CONSTANT: {
			error |= decl_insert_into_symbol_table(node, node->decl_constant.name, "constant");
		}break;
		case AST_DECL_STRUCT: {
			error |= decl_insert_into_symbol_table(node, node->decl_struct.name, "struct");

			for (size_t i = 0; i < node->decl_struct.fields_count; ++i) {
				error |= decl_insert_into_symbol_table(node->decl_struct.fields[i],
					node->decl_struct.fields[i]->decl_variable.name, "struct field");
			}
		}break;
		case AST_DECL_VARIABLE: {
			error |= decl_insert_into_symbol_table(node, node->decl_variable.name, "variable");
		}break;

		// TODO(psv): unions
		case AST_DECL_UNION: assert(0); break;
		default:             assert(0); break;
	}
	return error;
}

// resolves and internalizes type if it can, if the type was resolved set TYPE_FLAG_INTERNALIZE and TYPE_FLAG_RESOLVED flags
// if undeclared error return 0
Type_Instance* resolve_type(Scope* scope, Type_Instance* type, bool rep_undeclared) {
	if (type->flags & TYPE_FLAG_INTERNALIZED)
		return type;

	switch (type->kind) {
		case KIND_PRIMITIVE:{
			type->flags |= TYPE_FLAG_SIZE_RESOLVED;
			type->type_size_bits = type_size_primitive(type->primitive) * 8;
			return type;
		}
		case KIND_POINTER: {
			type->flags |= TYPE_FLAG_SIZE_RESOLVED;
			type->type_size_bits = type_pointer_size_bits();
			type->pointer_to = resolve_type(scope, type->pointer_to, rep_undeclared);
			if (!type->pointer_to) return 0;
			if (type->pointer_to->flags & TYPE_FLAG_INTERNALIZED) {
				type->flags |= TYPE_FLAG_RESOLVED;
				return internalize_type(&type, true);
			} else {
				return type;
			}
		} break;
		case KIND_STRUCT: {
			Ast* struct_decl = decl_from_name(scope, type->struct_desc.name);
			if (!struct_decl) {
				if (rep_undeclared) {
					report_undeclared(type->struct_desc.name);
					return 0;
				}
				return type;
			}
			Type_Instance* t = struct_decl->decl_struct.type_info;
			if (t->flags & TYPE_FLAG_INTERNALIZED) {
				return t;
			} else {
				return type;
			}
		} break;
		case KIND_ARRAY: {
			type->array_desc.array_of = resolve_type(scope, type->array_desc.array_of, rep_undeclared);
			if (!type->array_desc.array_of) return 0;
			if (type->array_desc.array_of->flags & TYPE_FLAG_INTERNALIZED) {
				type->flags |= TYPE_FLAG_RESOLVED | TYPE_FLAG_SIZE_RESOLVED;
				type->type_size_bits = type->array_desc.dimension * type->array_desc.array_of->type_size_bits;
				return internalize_type(&type, true);
			} else {
				return type;
			}
		} break;
		case KIND_FUNCTION: {
			Type_Instance* ret_type = resolve_type(scope, type->function_desc.return_type, rep_undeclared);
			if (!ret_type) return 0;
			if (!(ret_type->flags & TYPE_FLAG_INTERNALIZED)) {
				return type;
			} else {
				size_t nargs = type->function_desc.num_arguments;
				for (size_t i = 0; i < nargs; ++i) {
					Type_Instance* t = type->function_desc.arguments_type[i];
					t = resolve_type(scope, type->function_desc.arguments_type[i], rep_undeclared);
					type->function_desc.arguments_type[i] = t;
					if (!(t->flags & TYPE_FLAG_INTERNALIZED)) {
						return type;
					}
				}
				type->flags |= TYPE_FLAG_RESOLVED;
				type = internalize_type(&type, true);
				return type;
			}
		} break;
	}
	return 0;
}

Decl_Error resolve_types_decls(Scope* scope, Ast* node, bool rep_undeclared) {
	if (!(node->flags & AST_FLAG_IS_DECLARATION)) {
		return DECL_OK;
	}

	Decl_Error error = DECL_OK;

	switch (node->node_type) {
		case AST_DECL_VARIABLE:{
			if (!node->decl_variable.variable_type) {
				// infer from expr
				if (!node->decl_variable.assignment) {
					error |= report_decl_error(DECL_ERROR_FATAL, node, "cannot infer variable type if no assignment expression is given\n");
					return error;
				}
				Type_Instance* type = infer_from_expression(node->decl_variable.assignment, &error, rep_undeclared);
				if (error & DECL_ERROR_FATAL) return error;
				if (!type) {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
					return error;
				}
				type->flags |= TYPE_FLAG_RESOLVED;
				infer_queue_remove(node);
				node->decl_variable.variable_type = internalize_type(&type, true);
				return error;
			}
			if (node->decl_variable.variable_type->flags & TYPE_FLAG_RESOLVED) {
				return error;
			} else {
				Type_Instance* type = resolve_type(scope, node->decl_variable.variable_type, rep_undeclared);
				if (!type) {
					error |= DECL_ERROR_FATAL;
					return error;
				}
				if (type->flags & TYPE_FLAG_RESOLVED) {
					node->decl_variable.variable_type = type;
					infer_queue_remove(node);
				} else {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
				}
			}
		} break;
		case AST_DECL_STRUCT:{
			if (node->decl_struct.type_info->flags & TYPE_FLAG_RESOLVED) {
				return error;
			} else {
				size_t nfields = node->decl_struct.fields_count;
				Type_Instance* tinfo = node->decl_struct.type_info;

				size_t type_size_bits = 0;
				for (size_t i = 0; i < nfields; ++i) {
					Decl_Error e = resolve_types_decls(node->decl_struct.struct_scope, node->decl_struct.fields[i], rep_undeclared);
					error |= e;
					if (e & DECL_QUEUED_TYPE) {
						continue;
					} else {
						tinfo->struct_desc.fields_types[i] = node->decl_struct.fields[i]->decl_variable.variable_type;
						type_size_bits += tinfo->struct_desc.fields_types[i]->type_size_bits;
					}
				}
				if (error & DECL_QUEUED_TYPE) {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
				} else {
					node->decl_struct.type_info->type_size_bits = type_size_bits;
					node->decl_struct.type_info->flags |= TYPE_FLAG_RESOLVED;
					node->decl_struct.type_info->flags |= TYPE_FLAG_SIZE_RESOLVED;
					node->decl_struct.type_info->flags |= TYPE_FLAG_LVALUE;
					node->decl_struct.type_info = internalize_type(&tinfo, true);
					infer_queue_remove(node);
				}
			}
		} break;
		case AST_DECL_PROCEDURE: {
			if (!(node->decl_procedure.type_return->flags & TYPE_FLAG_RESOLVED)) {
				node->decl_procedure.type_return = resolve_type(scope, node->decl_procedure.type_return, rep_undeclared);
				if (!node->decl_procedure.type_return) {
					// undeclared ident
					error |= DECL_ERROR_FATAL;
					return error;
				}
				if (!(node->decl_procedure.type_return->flags & TYPE_FLAG_INTERNALIZED)) {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
					return error;
				}
			}
			size_t nargs = node->decl_procedure.arguments_count;
			for (size_t i = 0; i < nargs; ++i) {
				error |= resolve_types_decls(scope, node->decl_procedure.arguments[i], rep_undeclared);
			}
			if (error & DECL_QUEUED_TYPE) {
				infer_queue_push(node);
				return error;
			}

			if (!(node->decl_procedure.type_procedure->flags & TYPE_FLAG_RESOLVED)) {
				// return type must be resolved if we are here, so check only arguments				
				for (size_t i = 0; i < nargs; ++i) {
					Type_Instance* t = resolve_type(scope, node->decl_procedure.type_procedure->function_desc.arguments_type[i], rep_undeclared);
					if (!t) {
						error |= DECL_ERROR_FATAL;
						return error;
					}
					if (t->flags & TYPE_FLAG_INTERNALIZED) {
						node->decl_procedure.type_procedure->function_desc.arguments_type[i] = t;
					} else {
						infer_queue_push(node);
						error |= DECL_QUEUED_TYPE;
						return error;
					}
				}
				node->decl_procedure.type_procedure->flags |= TYPE_FLAG_RESOLVED;
				node->decl_procedure.type_procedure->function_desc.return_type = node->decl_procedure.type_return;
				node->decl_procedure.type_procedure = internalize_type(&node->decl_procedure.type_procedure, true);
				infer_queue_remove(node);
			}
		} break;
		case AST_DECL_ENUM:		// TODO(psv): types in namespaces
		case AST_DECL_CONSTANT:	// TODO(psv): only type aliases
		default: // TODO(psv): internal error here
			break;
	}
	return error;
}

static bool scope_inside_proc(Scope* scope) {
	while (scope) {
		if (scope->flags & SCOPE_PROCEDURE_BODY) return true;
		if (scope->flags & SCOPE_ENUM) return false;
		if (scope->flags & SCOPE_STRUCTURE) return false;
		scope = scope->parent;
	}
	return false;
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

static bool scope_inside_loop(Scope* scope, u64 level) {
	while (scope) {
		if (scope->flags & SCOPE_LOOP) --level;
		if (level <= 0) return true;
		if (scope->flags & SCOPE_PROCEDURE_BODY) return false;
		if (scope->flags & SCOPE_ENUM) return false;
		if (scope->flags & SCOPE_STRUCTURE) return false;
		scope = scope->parent;
	}
	return false;
}

Decl_Error decl_check_inner(Scope* global_scope, Ast** ast_top_level);
Decl_Error decl_check_inner_decl(Ast* node);
Decl_Error decl_check_inner_expr(Ast* node);
Decl_Error decl_check_inner_expr_lassign(Ast* node);
Decl_Error decl_check_inner_command(Ast* node);

Decl_Error decl_insert_into_symbol_table(Ast* node, Token* name, char* descriptor) {
	Decl_Error error = DECL_OK;
	if (node->scope->symb_table.entries_capacity == 0) {
		symbol_table_init(&node->scope->symb_table, (node->scope->decl_count + 4) * 8);
	}
	s64 index = symbol_table_entry_exist(&node->scope->symb_table, name);
	if (index != -1) {
		error |= report_redeclaration(descriptor, name, symbol_table_get_entry(&node->scope->symb_table, index));
	} else {
		symbol_table_add(&node->scope->symb_table, name, node);
	}
	return error;
}

Decl_Error decl_check_inner_decl(Ast* node) {
	assert(node->flags & AST_FLAG_IS_DECLARATION);
	Decl_Error error = DECL_OK;

	switch (node->node_type) {
		case AST_DECL_VARIABLE:{
			Type_Instance* var_type = 0;
			if (node->decl_variable.variable_type) {
				var_type = resolve_type(node->scope, node->decl_variable.variable_type, true);
				node->decl_variable.variable_type = var_type;
				if(node->decl_variable.assignment){
					Type_Instance* rtype = infer_from_expression(node->decl_variable.assignment, &error, true);
					if(error & DECL_ERROR_FATAL) return error;
					if(rtype->flags & TYPE_FLAG_WEAK){
						rtype = type_strength_resolve(rtype, var_type, node->decl_variable.assignment, node, &error);
						if(error & DECL_ERROR_FATAL) return error;
						node->decl_variable.assignment->type_return = rtype;
					}
				}
			}
			if (!var_type) {
				if (node->decl_variable.assignment) {
					// infer from expression
 					var_type = infer_from_expression(node->decl_variable.assignment, &error, true);
					if (error & TYPE_ERROR_FATAL) return error;
					assert(var_type);
					if(var_type->flags & TYPE_FLAG_WEAK){
						var_type->flags |= TYPE_FLAG_RESOLVED;	// if inferred and is weak, keep it as strong
						error |= type_update_weak(node->decl_variable.assignment, var_type);
						if (error & DECL_ERROR_FATAL) return error;
					}
					node->decl_variable.variable_type = internalize_type(&var_type, true);
					node->decl_variable.assignment->type_return = var_type;
				} else {
					report_error_location(node);
					error |= report_type_error(DECL_ERROR_FATAL, "variable declaration cannot be type inferred without an assignment expression\n");
				}
			}
			if (node->scope->level > 0)
				error |= decl_insert_into_symbol_table(node, node->decl_variable.name, "variable");
		}break;
		case AST_DECL_CONSTANT: {
			// TODO(psv): could be type alias, not implemented yet
			Type_Instance* const_type = 0;
			if (node->decl_constant.type_info) {
				//const_type = resolve_type(node->scope, node->decl_constant.type_info, true);
				//node->decl_constant.type_info = const_type;
				const_type = resolve_type(node->scope, node->decl_constant.type_info, true);
				node->decl_constant.type_info = const_type;
				if (node->decl_constant.value) {
					Type_Instance* rtype = infer_from_expression(node->decl_constant.value, &error, true);
					if (error & DECL_ERROR_FATAL) return error;
					if (rtype->flags & TYPE_FLAG_WEAK) {
						rtype = type_strength_resolve(rtype, const_type, node->decl_constant.value, node, &error);
						if (error & DECL_ERROR_FATAL) return error;
						node->decl_constant.value->type_return = rtype;
					}
				}
			}
			if (!const_type) {
				if (node->decl_constant.value) {
					// infer from expression
					const_type = infer_from_expression(node->decl_constant.value, &error, true);
					if (error & DECL_ERROR_FATAL) return error;
					assert(const_type);
					const_type->flags |= TYPE_FLAG_RESOLVED;	// if inferred and is weak, keep it as strong
					node->decl_constant.type_info = internalize_type(&const_type, true);
					node->decl_constant.value->type_return = const_type;
				} else {
					report_error_location(node);
					error |= report_type_error(DECL_ERROR_FATAL, "constant declaration cannot be type inferred without an assignment expression\n");
				}
			}
			if (node->scope->level > 0)
				error |= decl_insert_into_symbol_table(node, node->decl_constant.name, "constant");
		}break;
		case AST_DECL_PROCEDURE: {
			//for (size_t i = 0; i < node->decl_procedure.arguments_count; ++i) {
			//	Ast* arg = node->decl_procedure.arguments[i];
			//	error |= decl_check_inner_decl(arg);
			//}
			if (!(node->decl_procedure.flags & DECL_PROC_FLAG_FOREIGN)) {
				error |= decl_check_inner_command(node->decl_procedure.body);
				if (node->scope->level > 0)
					error |= decl_insert_into_symbol_table(node, node->decl_procedure.name, "procedure");
			}
		}break;
		case AST_DECL_STRUCT: break; // should not have struct inner decl yet
		case AST_DECL_ENUM: break; // should not have enum inner decl yet
		case AST_DECL_UNION: break; // should not have union inner decl yet
		default: assert(0); break; // TODO(psv): internal error
	}

	return error;
}

Decl_Error decl_check_inner_command(Ast* node) {
	assert(node->flags & AST_FLAG_IS_COMMAND);
	Decl_Error error = DECL_OK;

	switch (node->node_type) {
		case AST_COMMAND_BLOCK:	{
			if(node->comm_block.commands)
				error |= decl_check_inner(node->comm_block.block_scope, node->comm_block.commands); 
		}break;
		case AST_COMMAND_BREAK: {
			if (!node->comm_break.level) {
				node->comm_break.level = ast_create_expr_literal(node->scope, LITERAL_HEX_INT, 0, 0, type_primitive_get(TYPE_PRIMITIVE_U64));
				node->comm_break.level->expr_literal.value_u64 = 1;
			}
			if (node->comm_break.level->node_type == AST_EXPRESSION_LITERAL) {
				error |= decl_check_inner_expr(node->comm_break.level);
				if (error & DECL_ERROR_FATAL) return error;
				if (type_primitive_int(node->comm_break.level->type_return)) {
					// check if it is inside a loop
					u64 loop_level = node->comm_break.level->expr_literal.value_u64;
					if (scope_inside_loop(node->scope, loop_level)) {
						return error;
					} else {
						report_error_location(node);
						error |= report_semantic_error(DECL_ERROR_FATAL, "break command is not inside an iterative loop nested '%llu' deep\n", loop_level);
					}
				} else {
					report_error_location(node);
					error |= report_semantic_error(DECL_ERROR_FATAL, "break command must have an integer as a target\n");
				}
			} else {
				report_error_location(node);
				error |= report_semantic_error(DECL_ERROR_FATAL, "break command must have an integer literal as a target\n");
			}
		} break;
		case AST_COMMAND_CONTINUE: {
			if (scope_inside_loop(node->scope)) {
				return error;
			} else {
				report_error_location(node);
				error |= report_semantic_error(DECL_ERROR_FATAL, "continue command is not inside an iterative loop\n");
			}
		}break;
		case AST_COMMAND_RETURN: {
			if (!scope_inside_proc(node->scope)) {
				report_error_location(node);
				error |= report_semantic_error(DECL_ERROR_FATAL, "return command is not inside a procedure\n");
			}
			if(node->comm_return.expression)
				error |= decl_check_inner_expr(node->comm_return.expression);
		}break;
		case AST_COMMAND_FOR: {
			assert(node->comm_for.body->comm_block.block_scope->flags & SCOPE_LOOP);
			error |= decl_check_inner_expr(node->comm_for.condition);
			error |= decl_check_inner_command(node->comm_for.body);
		}break;
		case AST_COMMAND_IF: {
			error |= decl_check_inner_expr(node->comm_if.condition);
			error |= decl_check_inner_command(node->comm_if.body_true);
			if (node->comm_if.body_false)
				error |= decl_check_inner_command(node->comm_if.body_false);
		}break;
		case AST_COMMAND_VARIABLE_ASSIGNMENT: {
			// TODO(psv): lvalue not distinguished
			if(node->comm_var_assign.lvalue)
				error |= decl_check_inner_expr_lassign(node->comm_var_assign.lvalue);
			else if (node->comm_var_assign.lvalue && node->comm_var_assign.lvalue->type_return != type_primitive_get(TYPE_PRIMITIVE_VOID)) {
				error |= report_type_error(TYPE_ERROR_FATAL, "left side of assignment is not an addressable value\n");
			}
			error |= decl_check_inner_expr(node->comm_var_assign.rvalue);
			if (node->comm_var_assign.rvalue) {
				// if the rvalue is weak, transform it
				if (node->comm_var_assign.rvalue->type_return && node->comm_var_assign.rvalue->type_return->flags & TYPE_FLAG_WEAK) {
					error |= type_update_weak(node->comm_var_assign.rvalue, node->comm_var_assign.lvalue->type_return->pointer_to);
				}
			}
		}break;
		default: assert(0); break;	// TODO(psv): report internal compiler error
	}

	return error;
}

Decl_Error decl_check_inner_expr_lassign(Ast* node) {
	assert(node->flags & AST_FLAG_IS_EXPRESSION);
	Decl_Error error = DECL_OK;
	Type_Instance* t = infer_from_expression(node, &error, true, true);
	node->type_return = t;

	return error;
}

Decl_Error decl_check_inner_expr(Ast* node) {
	assert(node->flags & AST_FLAG_IS_EXPRESSION);
	Decl_Error error = DECL_OK;
	Type_Instance* t = infer_from_expression(node, &error, true);
	node->type_return = t;

	switch(node->node_type){
		case AST_EXPRESSION_BINARY:{
			error |= decl_check_inner_expr(node->expr_binary.left);
			error |= decl_check_inner_expr(node->expr_binary.right);
		}break;
		case AST_EXPRESSION_PROCEDURE_CALL:{
			for(s32 i = 0; i < node->expr_proc_call.args_count; ++i){
				error |= decl_check_inner_expr(node->expr_proc_call.args[i]);
			}
		}break;
		case AST_EXPRESSION_UNARY:{
			error |= decl_check_inner_expr(node->expr_unary.operand);
		}break;
		case AST_EXPRESSION_LITERAL:
		case AST_EXPRESSION_VARIABLE:
		break;
	}

	return error;
}

Decl_Error decl_check_inner(Scope* global_scope, Ast** ast_top_level) {
	Decl_Error error = DECL_OK;
	size_t ndecls = array_get_length(ast_top_level);

	for (size_t i = 0; i < ndecls; ++i) {
		Ast* node = ast_top_level[i];
		if (node->flags & AST_FLAG_IS_DECLARATION) {
			error |= decl_check_inner_decl(node);
		} else if(node->flags & AST_FLAG_IS_COMMAND) {
			error |= decl_check_inner_command(node);
		} else if (node->flags & AST_FLAG_IS_EXPRESSION) {
			error |= decl_check_inner_expr(node);
		}
	}

	return error;
}

Decl_Error decl_check_top_level(Scope* global_scope, Ast** ast_top_level) {
	Decl_Error error = DECL_OK;

	infer_queue = array_create(Ast*, 16);
	// Initialize the global scope symbol table
	symbol_table_init(&global_scope->symb_table, (global_scope->decl_count + 4) * 8);

	size_t ndecls = array_get_length(ast_top_level);
	for (size_t i = 0; i < ndecls; ++i) {
		Ast* node = ast_top_level[i];
		assert(node->flags & AST_FLAG_IS_DECLARATION);

		error |= decl_check_redefinition(global_scope, node);
		if (error & DECL_ERROR_FATAL) continue;
		error |= resolve_types_decls(global_scope, node, false);
	}

	if (error & DECL_ERROR_FATAL) {
		return DECL_ERROR_FATAL;
	}

	// infer queue of top level
	size_t n = array_get_length(infer_queue);
	while (error) {
		error = DECL_OK;
		for (size_t i = 0; i < n; ++i) {
			error |= resolve_types_decls(global_scope, infer_queue[i], true);
			if (error & DECL_ERROR_FATAL) break;
		}
		if (error & DECL_ERROR_FATAL) break;
		size_t infer_queue_end_size = array_get_length(infer_queue);
		if (infer_queue_end_size == n) {
			report_dependencies_error(infer_queue);
			return DECL_ERROR_FATAL;
		}
	}

	if (error & DECL_ERROR_FATAL) {
		return DECL_ERROR_FATAL;
	}

	error |= decl_check_inner(global_scope, ast_top_level);

	return error;
}

Ast* decl_from_name(Scope* scope, Token* name) {
	while (scope) {
		if (scope->symb_table.entries_count > 0) {
			s64 index = symbol_table_entry_exist(&scope->symb_table, name);
			if (index != -1) {
				return scope->symb_table.entries[index].decl_node;
			}
		}
		scope = scope->parent;
	}
	return 0;
}










void DEBUG_print_indent_level(s32 il) {
	for (s32 i = 0; i < il; ++i)
		printf(" ");
}

void DEBUG_print_scope_decls(Scope* scope) {
	if (!scope) return;
	
	char* description = 0;
	if (scope->flags & SCOPE_BLOCK)
		description = "block";
	else if (scope->flags & SCOPE_PROCEDURE_BODY)
		description = "procedure";
	else if (scope->flags & SCOPE_ENUM)
		description = "enum";
	else if (scope->flags & SCOPE_PROCEDURE_ARGUMENTS)
		description = "arguments";
	else if (scope->flags & SCOPE_STRUCTURE)
		description = "struct";
	else if (scope->id == 0)
		description = "global";

	DEBUG_print_indent_level(scope->level);
	printf("(id:%d) %s scope [%d]:\n", scope->id, description, scope->level);

	for (s32 i = 0; i < scope->symb_table.entries_capacity; ++i) {
		if (scope->symb_table.entries[i].occupied) {
			DEBUG_print_indent_level(scope->level + 1);
			printf("%d: %.*s\n", i, TOKEN_STR(scope->symb_table.entries[i].identifier));
		}
	}
}

void DEBUG_print_scope_decls(Ast** top) {
	for (size_t i = 0; i < array_get_length(top); ++i) {
		Ast* node = top[i];
		switch (node->node_type) {
			case AST_DECL_PROCEDURE: {
				DEBUG_print_scope_decls(node->decl_procedure.arguments_scope);
				DEBUG_print_scope_decls(node->decl_procedure.body->comm_block.block_scope);
				DEBUG_print_scope_decls(node->decl_procedure.body->comm_block.commands);
			}break;
			case AST_COMMAND_BLOCK: {
				DEBUG_print_scope_decls(node->comm_block.block_scope);
				DEBUG_print_scope_decls(node->comm_block.commands);
			}break;
			case AST_COMMAND_FOR: {
			}break;
			case AST_COMMAND_IF: {

			}break;
		}
	}
}