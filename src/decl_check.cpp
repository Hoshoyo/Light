#include "decl_check.h"
#include "symbol_table.h"
#include "type_table.h"
#include "type_infer.h"
#include "lexer.h"
#include "error.h"

static Ast** type_decl_array;
inline void type_decl_push(Ast* node) {
	array_push(type_decl_array, &node);
}
Ast** type_decl_array_get() {
	return type_decl_array;
}

static Ast** g_infer_queue;
inline void infer_queue_push(Ast* node) {
	if (node->flags & AST_FLAG_QUEUED)
		return;
	node->flags |= AST_FLAG_QUEUED;
	node->infer_queue_index = array_push(g_infer_queue, &node);
}
inline void infer_queue_remove(Ast* node) {
	if (node->flags & AST_FLAG_QUEUED) {
		if(node->infer_queue_index != -1)
			array_remove(g_infer_queue, node->infer_queue_index);
	}
}

s32 get_alignment_from_type(Type_Instance* type){
	switch(type->kind) {
		case KIND_PRIMITIVE: return type->type_size_bits / 8;
		case KIND_POINTER:   return type_pointer_size_bits() / 8;
		case KIND_FUNCTION:  return type_pointer_size_bits() / 8;
		case KIND_ARRAY:     return get_alignment_from_type(type->array_desc.array_of);
		case KIND_STRUCT:{
			if(type->struct_desc.fields_count > 0){
				return get_alignment_from_type(type->struct_desc.fields_types[0]);
			} else {
				return 0;
			}
		}   
	}
	return 0;
}

bool expr_is_constant(Ast* node, Type_Error* error, bool rep_undeclared, bool rep_error);

// ---------------------------------------------
// -------------- Declarations -----------------
// ---------------------------------------------

Type_Error report_redeclaration(char* node_type, Token* redeclared, Token* existing) {
	report_decl_error(TYPE_ERROR_FATAL, redeclared, "redeclaration of %s %.*s \n", node_type, TOKEN_STR(redeclared));
	fprintf(stderr, " - previously defined at: ");
	report_error_location(existing);
	fprintf(stderr, "\n");
	return TYPE_ERROR_FATAL;
}
Type_Error report_undeclared(Token* name){
	report_decl_error(TYPE_ERROR_FATAL, name, "Undeclared identifier '%.*s'\n", TOKEN_STR(name));
	return TYPE_ERROR_FATAL;
}

Type_Error decl_insert_into_symbol_table(Ast* node, Token* name, char* descriptor) {
	Type_Error error = TYPE_OK;
	if (node->scope->symb_table.entries_capacity == 0) {
		symbol_table_init(&node->scope->symb_table, (node->scope->decl_count + 4) * 8);
	}
	s64 index = symbol_table_entry_exist(&node->scope->symb_table, name);
	if (index != -1) {
		error |= report_redeclaration(descriptor, name, symbol_table_get_entry(&node->scope->symb_table, index));
	}
	else {
		symbol_table_add(&node->scope->symb_table, name, node);
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

Type_Error decl_check_redefinition(Scope* scope, Ast* node) {
	Type_Error error = TYPE_OK;

	Token* decl_name = 0;
	char*  decl_string_desc = 0;

	switch (node->node_type) {
		case AST_DECL_TYPEDEF: {
			error |= decl_insert_into_symbol_table(node, node->decl_typedef.name, "type alias");
		}break;

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

Type_Instance* type_from_alias(Scope* scope, Type_Instance* type) {
	assert(type->kind == KIND_ALIAS);

	return 0;
}

// resolves and internalizes type if it can, if the type was resolved set TYPE_FLAG_INTERNALIZE
// if undeclared error return 0
// when not resolved and rep_undeclared is false, return the type as it were do it can be queued
Type_Instance* resolve_type(Scope* scope, Type_Instance* type, bool rep_undeclared) {
	if (type->flags & TYPE_FLAG_INTERNALIZED)
		return type;

	switch (type->kind) {
	case KIND_ALIAS: {
		Type_Instance* t = type_from_alias(scope, type);
		if (!t) {
			if (rep_undeclared) {
				report_undeclared(type->alias.name);
				return 0;
			}
			return type;
		} else {
			if (t->flags & TYPE_FLAG_INTERNALIZED) {
				return t;
			} else {
				return resolve_type(scope, t, rep_undeclared);
			}
		}
	}break;
	case KIND_PRIMITIVE: {
		type->flags |= TYPE_FLAG_SIZE_RESOLVED;
		type->type_size_bits = type_size_primitive(type->primitive) * 8;
		return internalize_type(&type, scope, true);
	}
	case KIND_POINTER: {
		type->flags |= TYPE_FLAG_SIZE_RESOLVED;
		type->type_size_bits = type_pointer_size_bits();
		if (type->pointer_to->kind == KIND_STRUCT) {
			Ast* struct_decl = decl_from_name(scope, type->pointer_to->struct_desc.name);
			if (!struct_decl) {
				if (rep_undeclared) {
					report_undeclared(type->pointer_to->struct_desc.name);
				}
				return 0;
			}
			type->pointer_to = resolve_type(scope, type->pointer_to, report_undeclared);
			return internalize_type(&type, scope, true);
		}
		else {
			type->pointer_to = resolve_type(scope, type->pointer_to, rep_undeclared);
			if (!type->pointer_to) return 0;
			if (type->pointer_to->flags & TYPE_FLAG_INTERNALIZED) {
				return internalize_type(&type, scope, true);
			} else {
				return type;
			}
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
			type->flags |= TYPE_FLAG_SIZE_RESOLVED;
			type->type_size_bits = type->array_desc.dimension * type->array_desc.array_of->type_size_bits;
			return internalize_type(&type, scope, true);
		}
		else {
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
			type->flags |= TYPE_FLAG_SIZE_RESOLVED;
			type->type_size_bits = type_pointer_size_bits();
			type = internalize_type(&type, scope, true);
			return type;
		}
	} break;
	}
	return 0;
}

Type_Error type_information_pass(Scope* scope, Ast* node) {
	Type_Error error_code = TYPE_OK;
	switch (node->node_type) 
	{
		case AST_DECL_TYPEDEF: {
			Type_Instance* type = resolve_type(node->scope, node->decl_typedef.type, false);
			if (!type || !(type->flags & TYPE_FLAG_INTERNALIZED)) {
				error_code |= TYPE_QUEUED;
				return error_code;
			}
			node->decl_typedef.type = type;
		}break;

		case AST_DECL_PROCEDURE: {
			// If it is the main procedure, tag it as so.
			if (node->decl_procedure.name->value.data == compiler_tags[COMPILER_TAG_MAIN_PROC].data) {
				node->decl_procedure.flags |= DECL_PROC_FLAG_MAIN;
			}

			// Try to internalize the return type of the procedure
			if (!(node->decl_procedure.type_return->flags & TYPE_FLAG_INTERNALIZED)) {
				Type_Instance* type = resolve_type(scope, node->decl_procedure.type_return, false);
				// The type could not be resolved, that means it some name could not be resolved to a type, so queue this node
				if (!type || !(type->flags & TYPE_FLAG_INTERNALIZED)) {
					infer_queue_push(node);
					error_code |= TYPE_QUEUED;
					return error_code;
				}
				// this means the type is internalized so just apply it
				node->decl_procedure.type_return = type;
			}

			// type info pass on the arguments
			size_t nargs = node->decl_procedure.arguments_count;
			for (size_t i = 0; i < nargs; ++i) {
				error_code |= type_information_pass(scope, node->decl_procedure.arguments[i]);
			}

			// If any of the arguments caused a fatal error, no point in continuing
			if (error_code & TYPE_ERROR_FATAL) {
				return error_code;
			}

			// If any of the arguments was queued, queue this node too
			if (error_code & TYPE_QUEUED) {
				infer_queue_push(node);
				return error_code;
			}

			if (!(node->decl_procedure.type_procedure->flags & TYPE_FLAG_INTERNALIZED)) {
				// return type must be resolved if we are here, the arguments too
				for (size_t i = 0; i < nargs; ++i) {
					Type_Instance* type_arg = node->decl_procedure.arguments[i]->decl_variable.variable_type;
					assert(type_arg->flags & TYPE_FLAG_INTERNALIZED);
					assert(type_arg);

					node->decl_procedure.type_procedure->function_desc.arguments_type[i] = type_arg;
				}

				// Internalize procedure type
				node->decl_procedure.type_procedure->function_desc.return_type = node->decl_procedure.type_return;
				node->decl_procedure.type_procedure = resolve_type(scope, node->decl_procedure.type_procedure, false);
				node->decl_procedure.type_procedure = internalize_type(&node->decl_procedure.type_procedure, scope, true);
				assert(node->decl_procedure.type_procedure->flags & TYPE_FLAG_INTERNALIZED);
				infer_queue_remove(node);
			}
		}break;

		case AST_DECL_STRUCT: {
			if (node->decl_struct.type_info->flags & TYPE_FLAG_INTERNALIZED) {
				return error_code;
			} else {
				bool           packed = false;
				size_t         nfields = node->decl_struct.fields_count;
				Type_Instance* tinfo = node->decl_struct.type_info;
				s64            offset = 0;
				s32            alignment = 0;
				size_t         type_size_bits = 0;

				for (size_t i = 0; i < nfields; ++i) {
					error_code |= type_information_pass(node->decl_struct.struct_scope, node->decl_struct.fields[i]);
					if (error_code & TYPE_ERROR_FATAL) {
						return error_code;
					} else if (error_code & TYPE_QUEUED) {
						infer_queue_push(node);
						return error_code;
					} else {
						tinfo->struct_desc.fields_types[i] = node->decl_struct.fields[i]->decl_variable.variable_type;
						s64 field_type_size = tinfo->struct_desc.fields_types[i]->type_size_bits;
						tinfo->struct_desc.offset_bits[i] = offset;

						type_size_bits += field_type_size;
						if (field_type_size == 0) continue;

						if (!packed) {
							// align type to its boundary
							s64 offset_bytes = offset / 8;
							s64 field_type_size_bytes = field_type_size / 8;
							alignment = get_alignment_from_type(tinfo->struct_desc.fields_types[i]);

							if (offset_bytes % alignment != 0) {
								s64 delta = align_delta(offset_bytes, alignment);
								delta *= 8; // delta in bits
								tinfo->struct_desc.offset_bits[i] += delta;
								type_size_bits += delta;
								offset += delta;
							}
						}
						offset += field_type_size;
						if (i == 0 || alignment == 0) {
							tinfo->struct_desc.alignment = alignment;
						}
					}
				}

				if (!packed) {
					// adjust for the alignment of the first element
					s64 first_alignment = tinfo->struct_desc.alignment;
					s64 delta = align_delta(type_size_bits / 8, first_alignment);
					type_size_bits += delta * 8;
				}
				node->decl_struct.type_info->type_size_bits = type_size_bits;
				node->decl_struct.type_info->flags |= TYPE_FLAG_SIZE_RESOLVED;
				node->decl_struct.type_info = internalize_type(&tinfo, scope, true);
				infer_queue_remove(node);
				type_decl_push(node);
			}
		}break;
		case AST_DECL_UNION:
			assert(0);
			break;
		case AST_DECL_VARIABLE: {
			Type_Instance* expr_type = 0;
			if (node->decl_variable.assignment) {
				expr_type = infer_from_expression(node->decl_variable.assignment, &error_code, 0);
				if (error_code & TYPE_ERROR_FATAL) return error_code;
				if (error_code & TYPE_QUEUED) {
					infer_queue_push(node);
					error_code |= TYPE_QUEUED;
					return error_code;
				}
				if (expr_type == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
					error_code |= report_type_error(TYPE_ERROR_FATAL, node, "cannot assign a variable to an expression returning void\n");
				}
			} else if(!node->decl_variable.variable_type) {
				error_code |= report_decl_error(TYPE_ERROR_FATAL, node, "cannot infer variable type if no assignment expression is given\n");
				return error_code;
			}
			
			if (node->decl_variable.variable_type) {
				Type_Instance* type_var = resolve_type(node->scope, node->decl_variable.variable_type, false);
				if (!type_var || !(type_var->flags & TYPE_FLAG_INTERNALIZED)) {
					error_code |= TYPE_QUEUED;
					return error_code;
				}

				node->decl_variable.variable_type = type_var;

				// Cannot declare a variable void
				if (node->decl_variable.variable_type == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
					error_code |= report_type_error(TYPE_ERROR_FATAL, node, "cannot declare a variable of type void\n");
					return error_code;
				}

				// Propagate the variable type to the expression, type checking is performed here
				// TODO(psv): type check
				if (node->decl_variable.assignment) {
					error_code |= type_propagate(type_var, node->decl_variable.assignment);
					if (error_code & TYPE_ERROR_FATAL) return error_code;
				}

			} else {
				if (expr_type->flags & TYPE_FLAG_INTERNALIZED) {
					node->decl_variable.variable_type = expr_type;
				} else {
					error_code |= type_propagate(0, node->decl_variable.assignment);
					if (error_code & TYPE_ERROR_FATAL) return error_code;
					node->decl_variable.variable_type = expr_type;
				}

				if (expr_type == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
					error_code |= report_type_error(TYPE_ERROR_FATAL, node, "cannot assign a variable to an expression returning void\n");
					return error_code;
				}
			}

			infer_queue_remove(node);

			assert(node->decl_variable.variable_type->flags & TYPE_FLAG_INTERNALIZED);
		}break;
		case AST_DECL_CONSTANT: {
			//assert_msg(0, "constant type information pass not implemented");
		}break;
		case AST_DECL_ENUM: {
			assert_msg(0, "enum type information pass not implemented");
		}break;
		default: {
			assert_msg(0, "invalid declaration in top level type information pass");
		}break;
	}
	return error_code;
}

Type_Error decl_check_top_level(Scope* global_scope, Ast** ast_top_level) {
	Type_Error error_code = TYPE_OK;

	// Initialize the global scope symbol table
	symbol_table_init(&global_scope->symb_table, (global_scope->decl_count + 4) * 8);

	// TODO: figure it out why this is here
	type_decl_array = array_create(Ast*, 16);
		
	// infer queue for nodes that dont have their type inferred yet.
	g_infer_queue = array_create(Ast*, 16);

	size_t ndecls = array_get_length(ast_top_level);
	for (size_t i = 0; i < ndecls; ++i) 
	{
		Ast* node = ast_top_level[i];
		assert(node->flags & AST_FLAG_IS_DECLARATION);
		if (node->node_type == AST_DATA) continue;

		error_code |= decl_check_redefinition(global_scope, node);
		if (error_code & TYPE_ERROR_FATAL) continue;
		error_code |= type_information_pass(global_scope, node);
	}
	return error_code;
}

#if 0

Type_Error resolve_types_decls(Scope* scope, Ast* node, bool rep_undeclared) {
	if (!(node->flags & AST_FLAG_IS_DECLARATION)) {
		return DECL_OK;
	}

	Type_Error error = DECL_OK;

	switch (node->node_type) {
		case AST_DECL_VARIABLE:{
			if (!node->decl_variable.variable_type) {
				// infer from expr
				if (!node->decl_variable.assignment) {
					error |= report_decl_error(TYPE_ERROR_FATAL, node, "cannot infer variable type if no assignment expression is given\n");
					return error;
				}
				Type_Instance* type = infer_from_expression(node->decl_variable.assignment, &error, rep_undeclared);
				if (error & TYPE_ERROR_FATAL) return error;
				if (!type || error & DECL_QUEUED_TYPE) {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
					return error;
				}
				type->flags |= TYPE_FLAG_RESOLVED;
				infer_queue_remove(node);
				if (type == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "cannot assign a variable to an expression returning void\n");
				}
				return error;
			}
			if (node->decl_variable.variable_type->flags & TYPE_FLAG_RESOLVED) {
				return error;
			} else {
				Type_Instance* type = resolve_type(scope, node->decl_variable.variable_type, rep_undeclared);

				if (type && type->flags & TYPE_FLAG_RESOLVED) {
					node->decl_variable.variable_type = type;
					if (type == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
						error |= report_type_error(TYPE_ERROR_FATAL, node, "cannot declare a variable of type void\n");
					}

					infer_queue_remove(node);
				} else {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
				}
			}
		} break;
		case AST_DECL_CONSTANT: {
			Type_Error const_err = TYPE_OK;
			bool is_constant = expr_is_constant(node->decl_constant.value, &const_err, false, true);
			if(!is_constant) {
				error |= const_err;
				return error;
			}
			if (!node->decl_constant.type_info) {
				// infer from expr
				if (!node->decl_constant.value) {
					error |= report_decl_error(TYPE_ERROR_FATAL, node, "cannot infer constant type if value expression is given\n");
					return error;
				}
				Type_Instance* type = infer_from_expression(node->decl_constant.value, &error, rep_undeclared);
				if (error & TYPE_ERROR_FATAL) return error;
				if (!type || error & DECL_QUEUED_TYPE) {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
					return error;
				}
				type->flags |= TYPE_FLAG_RESOLVED;
				infer_queue_remove(node);
				if (type == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "cannot assign a constant to an expression returning void\n");
				}
				return error;
			}
			if (node->decl_variable.variable_type->flags & TYPE_FLAG_RESOLVED) {
				return error;
			} else {
				Type_Instance* type = resolve_type(scope, node->decl_constant.type_info, rep_undeclared);

				if (type && type->flags & TYPE_FLAG_RESOLVED) {
					node->decl_constant.type_info = type;
					if (type == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
						error |= report_type_error(TYPE_ERROR_FATAL, node, "cannot declare a constant of type void\n");
					}

					infer_queue_remove(node);
				} else {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
				}
			}
		}break;
		case AST_DECL_STRUCT:{
			if (node->decl_struct.type_info->flags & TYPE_FLAG_RESOLVED) {
				return error;
			} else {
				bool packed = false;
				size_t nfields = node->decl_struct.fields_count;
				Type_Instance* tinfo = node->decl_struct.type_info;
				s64 offset = 0;
				s32 alignment = 0;
				size_t type_size_bits = 0;


				for (size_t i = 0; i < nfields; ++i) {
					Type_Error e = resolve_types_decls(node->decl_struct.struct_scope, node->decl_struct.fields[i], rep_undeclared);
					error |= e;
					if (e & DECL_QUEUED_TYPE) {
						break;
					} else {
						tinfo->struct_desc.fields_types[i] = node->decl_struct.fields[i]->decl_variable.variable_type;
						s64 field_type_size = tinfo->struct_desc.fields_types[i]->type_size_bits;
						tinfo->struct_desc.offset_bits[i] = offset;

						type_size_bits += field_type_size;
						if(field_type_size == 0) continue;

						if(!packed){
							// align type to its boundary
							s64 offset_bytes = offset / 8;
							s64 field_type_size_bytes = field_type_size / 8;
							alignment = get_alignment_from_type(tinfo->struct_desc.fields_types[i]);

							if(offset_bytes % alignment != 0) {
								s64 delta = align_delta(offset_bytes, alignment);
								delta *= 8; // delta in bits
								tinfo->struct_desc.offset_bits[i] += delta;
								type_size_bits += delta;
								offset += delta;
							}
						}
						offset += field_type_size;
						if(i == 0 || alignment == 0){
							tinfo->struct_desc.alignment = alignment;
						}
					}
				}

				if (error & DECL_QUEUED_TYPE) {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
				} else {
					if(!packed){
						// adjust for the alignment of the first element
						s64 first_alignment = tinfo->struct_desc.alignment;
						s64 delta = align_delta(type_size_bits / 8, first_alignment);
						type_size_bits += delta * 8;
					}
					node->decl_struct.type_info->type_size_bits = type_size_bits;
					node->decl_struct.type_info->flags |= TYPE_FLAG_RESOLVED | TYPE_FLAG_SIZE_RESOLVED;
					node->decl_struct.type_info = internalize_type(&tinfo, scope, true);
					infer_queue_remove(node);
					//printf("struct %.*s done\n", TOKEN_STR(node->decl_struct.name));
					type_decl_push(node);
				}
			}
		} break;
		case AST_DECL_PROCEDURE: {
			if (node->decl_procedure.name->value.data == compiler_tags[COMPILER_TAG_MAIN_PROC].data) {
				node->decl_procedure.flags |= DECL_PROC_FLAG_MAIN;
			}

			if (!(node->decl_procedure.type_return->flags & TYPE_FLAG_RESOLVED)) {
				Type_Instance* type = resolve_type(scope, node->decl_procedure.type_return, rep_undeclared);
				if (error & TYPE_ERROR_FATAL) {
					return error;
				}
				if (type) node->decl_procedure.type_return = type;
				if (!node->decl_procedure.type_return || !(node->decl_procedure.type_return->flags & TYPE_FLAG_INTERNALIZED)) {
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
						error |= TYPE_ERROR_FATAL;
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
				node->decl_procedure.type_procedure = resolve_type(scope, node->decl_procedure.type_procedure, rep_undeclared);
				node->decl_procedure.type_procedure = internalize_type(&node->decl_procedure.type_procedure, scope, true);
				infer_queue_remove(node);
			}
		} break;
		case AST_DECL_ENUM:		// TODO(psv): types in namespaces
			break;
		default: // TODO(psv): internal error here
			break;
	}
	return error;
}



Type_Error decl_check_inner(Scope* global_scope, Ast** ast_top_level);
Type_Error decl_check_inner_decl(Ast* node);
Type_Error decl_check_inner_expr(Ast* node);
Type_Error decl_check_inner_expr_lassign(Ast* node);
Type_Error decl_check_inner_command(Ast* node);



Type_Error decl_check_inner_decl(Ast* node) {
	assert(node->flags & AST_FLAG_IS_DECLARATION);
	Type_Error error = DECL_OK;

	switch (node->node_type) {

		// @INNER AST_DECL_VARIABLE
		case AST_DECL_VARIABLE:{
			if (node->decl_variable.variable_type) {
				node->decl_variable.variable_type = resolve_type(node->scope, node->decl_variable.variable_type, true);
				if (node->decl_variable.variable_type == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "cannot declare a variable of type void\n");
				}
				if (error & TYPE_ERROR_FATAL) return error;
			}

			if (node->decl_variable.assignment) {
				Type_Error type_error = TYPE_OK;
				Type_Instance* infered = infer_from_expression(node->decl_variable.assignment, &type_error, TYPE_INFER_REPORT_UNDECLARED);
				if (infered == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "cannot declare a variable of type void\n");
				}
				if (type_error & TYPE_ERROR_FATAL) return type_error | error;
				bool typechecked = false;
				if (infered && infered->flags & TYPE_FLAG_WEAK) {
					type_propagate(node->decl_variable.variable_type, node->decl_variable.assignment);
					if(!node->decl_variable.assignment->type_return){
						type_error |= report_type_error(TYPE_ERROR_FATAL, node, "could not infer variable type from assignment expression\n");
						return type_error | error;
					}
					if(infered->kind == KIND_ARRAY || infered->kind == KIND_STRUCT) {
						infered = type_check_expr(node->decl_variable.assignment->type_return, node->decl_variable.assignment, &error);
						typechecked = true;
					} else {
						infered = node->decl_variable.assignment->type_return;
					}
				}
				if(!node->decl_variable.variable_type){
					if(!infered){
						type_error |= report_type_error(TYPE_ERROR_FATAL, node, "could not infer variable type from assignment expression\n");
						return type_error | error;
					}
					node->decl_variable.variable_type = infered;
				} else {
					if(!typechecked)
						node->decl_variable.assignment->type_return = type_check_expr(node->decl_variable.variable_type, node->decl_variable.assignment, &type_error);
				}
				error |= type_error;
			}
			
			if (node->scope->level > 0)
				error |= decl_insert_into_symbol_table(node, node->decl_variable.name, "variable");
		}break;
		case AST_DECL_CONSTANT: {
			Type_Error type_error = TYPE_OK;

			if(!expr_is_constant(node->decl_constant.value, &type_error, true, true)){
				return type_error | error;
			}

			if (node->decl_constant.type_info) {
				node->decl_constant.type_info = resolve_type(node->scope, node->decl_constant.type_info, true);
				if (node->decl_constant.type_info == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
					error |= report_type_error(TYPE_ERROR_FATAL, node, "cannot declare a constant of type void\n");
				}
				if (error & TYPE_ERROR_FATAL) return error;
			}

			assert (node->decl_constant.value);

			Type_Instance* infered = infer_from_expression(node->decl_constant.value, &type_error, TYPE_INFER_REPORT_UNDECLARED);
			if (infered == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
				error |= report_type_error(TYPE_ERROR_FATAL, node, "cannot declare a constant of type void\n");
			}
			if (type_error & TYPE_ERROR_FATAL) return type_error | error;
			bool typechecked = false;
			if (infered && infered->flags & TYPE_FLAG_WEAK) {
				type_propagate(node->decl_constant.type_info, node->decl_constant.value);
				if(!node->decl_constant.value->type_return){
					type_error |= report_type_error(TYPE_ERROR_FATAL, node, "could not infer constant type from value expression\n");
					return type_error | error;
				}
				if(infered->kind == KIND_ARRAY || infered->kind == KIND_STRUCT) {
					infered = type_check_expr(node->decl_constant.value->type_return, node->decl_constant.value, &error);
					typechecked = true;
				} else {
					infered = node->decl_constant.value->type_return;
				}
			}
			if(!node->decl_constant.type_info){
				if(!infered){
					type_error |= report_type_error(TYPE_ERROR_FATAL, node, "could not infer constant type from value expression\n");
					return type_error | error;
				}
				node->decl_constant.type_info = infered;
			} else {
				if(!typechecked)
					node->decl_constant.value->type_return = type_check_expr(node->decl_constant.type_info, node->decl_constant.value, &type_error);
			}
			error |= type_error;
			
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
		case AST_DATA: break;		// no need to check is just raw data
		default: assert(0); break; // TODO(psv): internal error
	}

	return error;
}

Type_Error decl_check_inner_command(Ast* node) {
	assert(node->flags & AST_FLAG_IS_COMMAND);
	Type_Error error = DECL_OK;

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
				node->comm_break.level->type_return = infer_from_expression(node->comm_break.level, &error, TYPE_INFER_REPORT_UNDECLARED);
				if (error & TYPE_ERROR_FATAL) return error;
				if(node->comm_break.level->type_return->flags & TYPE_FLAG_WEAK){
					type_propagate(0, node->comm_break.level);
				}

				if (type_primitive_int(node->comm_break.level->type_return)) {
					// check if it is inside a loop
					u64 loop_level = node->comm_break.level->expr_literal.value_u64;
					if (scope_inside_loop(node->scope, loop_level)) {
						return error;
					} else {
						report_error_location(node);
						error |= report_semantic_error(TYPE_ERROR_FATAL, "break command is not inside an iterative loop nested '%lld' deep\n", loop_level);
					}
				} else {
					report_error_location(node);
					error |= report_semantic_error(TYPE_ERROR_FATAL, "break command must have an integer as a target\n");
				}
			} else {
				report_error_location(node);
				error |= report_semantic_error(TYPE_ERROR_FATAL, "break command must have an integer literal as a target\n");
			}
		} break;
		case AST_COMMAND_CONTINUE: {
			if (scope_inside_loop(node->scope)) {
				return error;
			} else {
				report_error_location(node);
				error |= report_semantic_error(TYPE_ERROR_FATAL, "continue command is not inside an iterative loop\n");
			}
		}break;
		case AST_COMMAND_RETURN: {
			if (!scope_inside_proc(node->scope)) {
				report_error_location(node);
				error |= report_semantic_error(TYPE_ERROR_FATAL, "return command is not inside a procedure\n");
			}
			if(node->comm_return.expression){
				//decl_check_inner_expr(node->comm_return.expression);
				node->comm_return.expression->type_return = infer_from_expression(node->comm_return.expression, &error, TYPE_INFER_REPORT_UNDECLARED);
			}
		}break;
		case AST_COMMAND_FOR: {
			if (node->comm_for.body->comm_block.block_scope) {
				assert(node->comm_for.body->comm_block.block_scope->flags & SCOPE_LOOP);
			}
			node->comm_for.condition->type_return = infer_from_expression(node->comm_for.condition, &error, TYPE_INFER_REPORT_UNDECLARED);
			error |= decl_check_inner_command(node->comm_for.body);
		}break;
		case AST_COMMAND_IF: {
			node->comm_if.condition->type_return = infer_from_expression(node->comm_if.condition, &error, TYPE_INFER_REPORT_UNDECLARED);
			error |= decl_check_inner_command(node->comm_if.body_true);
			if (node->comm_if.body_false)
				error |= decl_check_inner_command(node->comm_if.body_false);
		}break;
		case AST_COMMAND_VARIABLE_ASSIGNMENT: {
			// TODO(psv): lvalue not distinguished
			Type_Instance* ltype = 0;
			if(node->comm_var_assign.lvalue){
				//error |= decl_check_inner_expr_lassign(node->comm_var_assign.lvalue);
				ltype = infer_from_expression(node->comm_var_assign.lvalue, &error, TYPE_INFER_REPORT_UNDECLARED | TYPE_INFER_LVALUE);
				if(error & TYPE_ERROR_FATAL) return error;
				assert(ltype->flags & TYPE_FLAG_STRONG);
			} else if (node->comm_var_assign.lvalue && node->comm_var_assign.lvalue->type_return != type_primitive_get(TYPE_PRIMITIVE_VOID)) {
				error |= report_type_error(TYPE_ERROR_FATAL, "left side of assignment is not an addressable value\n");
			}
			//error |= decl_check_inner_expr(node->comm_var_assign.rvalue);
			Type_Instance* rtype = infer_from_expression(node->comm_var_assign.rvalue, &error, TYPE_INFER_REPORT_UNDECLARED);
			if(error & TYPE_ERROR_FATAL) return error;
			if(rtype->flags & TYPE_FLAG_WEAK){
				type_propagate(ltype, node->comm_var_assign.rvalue);
			}
			node->comm_var_assign.rvalue->type_return = type_check_expr(ltype, node->comm_var_assign.rvalue, &error);

			if (node->comm_var_assign.rvalue->type_return) {
				// if the rvalue is weak, transform it
				if (node->comm_var_assign.rvalue->type_return && node->comm_var_assign.rvalue->type_return->flags & TYPE_FLAG_WEAK) {
					//error |= type_update_weak(node->comm_var_assign.rvalue, node->comm_var_assign.lvalue->type_return->pointer_to);
					type_propagate(node->comm_var_assign.lvalue->type_return, node->comm_var_assign.rvalue);
					node->comm_var_assign.rvalue->type_return = type_check_expr(node->comm_var_assign.lvalue->type_return, node->comm_var_assign.rvalue, &error);
				}
			}
		}break;
		default: assert(0); break;	// TODO(psv): report internal compiler error
	}

	return error;
}

Type_Error decl_check_inner(Scope* global_scope, Ast** ast_top_level) {
	Type_Error error = DECL_OK;
	size_t ndecls = array_get_length(ast_top_level);

	for (size_t i = 0; i < ndecls; ++i) {
		Ast* node = ast_top_level[i];
		if (node->flags & AST_FLAG_IS_DECLARATION) {
			error |= decl_check_inner_decl(node);
		} else if(node->flags & AST_FLAG_IS_COMMAND) {
			error |= decl_check_inner_command(node);
		} else if (node->flags & AST_FLAG_IS_EXPRESSION) {
			assert(0); // @DEPRECATED
			//error |= decl_check_inner_expr(node);
		}
	}

	return error;
}

Type_Error decl_check_top_level(Scope* global_scope, Ast** ast_top_level) {
	Type_Error error = DECL_OK;

	type_decl_array = array_create(Ast*, 16);
	infer_queue = array_create(Ast*, 16);
	// Initialize the global scope symbol table
	symbol_table_init(&global_scope->symb_table, (global_scope->decl_count + 4) * 8);

	size_t ndecls = array_get_length(ast_top_level);
	for (size_t i = 0; i < ndecls; ++i) {
		Ast* node = ast_top_level[i];
		assert(node->flags & AST_FLAG_IS_DECLARATION);
		if(node->node_type == AST_DATA) continue;

		error |= decl_check_redefinition(global_scope, node);
		if (error & TYPE_ERROR_FATAL) continue;
		error |= resolve_types_decls(global_scope, node, false);
	}

	if (error & TYPE_ERROR_FATAL) {
		return TYPE_ERROR_FATAL;
	}

	// infer queue of top level
	size_t n = array_get_length(infer_queue);
	while (error) {
		error = DECL_OK;
		size_t xx = array_get_length(infer_queue);
		for (size_t i = 0; i < n; ++i) {
			error |= resolve_types_decls(global_scope, infer_queue[i], true);
			if (error & TYPE_ERROR_FATAL) break;
		}
		if (error & TYPE_ERROR_FATAL) break;
		size_t infer_queue_end_size = array_get_length(infer_queue);
		if (infer_queue_end_size == n) {
			report_dependencies_error(infer_queue);
			return TYPE_ERROR_FATAL;
		} else {
			n = infer_queue_end_size;
		}
	}

	error |= resolve_type_internalize_queue();

	if (error & TYPE_ERROR_FATAL) {
		return TYPE_ERROR_FATAL;
	}

	error |= decl_check_inner(global_scope, ast_top_level);

	return error;
}
#endif


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

bool expr_is_constant(Ast* node, Type_Error* error, bool rep_undeclared, bool rep_error) {
	assert(node->flags & AST_FLAG_IS_EXPRESSION);

	switch(node->node_type) {
		case AST_EXPRESSION_BINARY:{
			return(expr_is_constant(node->expr_binary.left, error, rep_undeclared, rep_error) && 
				expr_is_constant(node->expr_binary.right, error, rep_undeclared, rep_error));
		}break;
		case AST_EXPRESSION_LITERAL:{
			if(node->expr_literal.type == LITERAL_ARRAY) {
				Ast** a = node->expr_literal.array_exprs;
				size_t length = 0;
				if(a) {
					length = array_get_length(a);
				}
				bool res = true;
				for(size_t i = 0; i < length; ++i) {
					if(!expr_is_constant(a[i], error, rep_undeclared, rep_error)){
						res = false;
					}
				}
				return res;
			} else if(node->expr_literal.type == LITERAL_STRUCT) {
				Ast** a = node->expr_literal.struct_exprs;
				size_t length = 0;
				if(a) {
					length = array_get_length(a);
				}
				bool res = true;
				for(size_t i = 0; i < length; ++i) {
					if(!expr_is_constant(a[i], error, rep_undeclared, rep_error)){
						res = false;
					}
				}
				return res;
			}
			return true;
		}break;
		case AST_EXPRESSION_PROCEDURE_CALL:{
			if(rep_error){
				*error |= report_type_error(TYPE_ERROR_FATAL, node, "procedure call expression is not a constant value\n");
			}
			return false;
		}break;
		case AST_EXPRESSION_UNARY:{
			Operator_Unary op = node->expr_unary.op;
			switch(op){
				case OP_UNARY_DEREFERENCE:
				case OP_UNARY_ADDRESSOF:{
					return false;
				}break;
				case OP_UNARY_BITWISE_NOT:
				case OP_UNARY_CAST:
				case OP_UNARY_LOGIC_NOT:
				case OP_UNARY_MINUS:
				case OP_UNARY_PLUS:{
					return expr_is_constant(node->expr_unary.operand, error, rep_undeclared, rep_error);
				}break;
			}
		}break;
		case AST_EXPRESSION_VARIABLE:{
			Ast* decl = decl_from_name(node->scope, node->expr_variable.name);
			if(!decl){
				if(rep_undeclared){
					*error |= report_undeclared(node->expr_variable.name);
				} else {
					*error |= TYPE_QUEUED;
				}
				return false;
			} else {
				if(decl->node_type == AST_DECL_CONSTANT || decl->node_type == AST_DECL_PROCEDURE){
					return true;
				} else {
					if(rep_error){
						*error |= report_type_error(TYPE_ERROR_FATAL, node, "'%.*s' is not a constant value\n", TOKEN_STR(node->expr_variable.name));
					}
					return false;
				}
			}
		}break;
	}
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
