#include "decl_check.h"
#include "symbol_table.h"
#include "type_table.h"
#include "type_infer.h"

void report_error_location(Token* tok) {
	fprintf(stderr, "%.*s (%d:%d) ", tok->filename.length, tok->filename.data, tok->line, tok->column);
}
void report_error_location(Ast* node) {
	switch (node->node_type) {
	case AST_DECL_CONSTANT:				report_error_location(node->decl_constant.name); break;
	case AST_DECL_VARIABLE:				report_error_location(node->decl_variable.name); break;
	case AST_DECL_PROCEDURE:			report_error_location(node->decl_procedure.name); break;
	case AST_DECL_ENUM:					report_error_location(node->decl_enum.name); break;
	case AST_DECL_STRUCT:				report_error_location(node->decl_struct.name); break;
	case AST_DECL_UNION:				report_error_location(node->decl_union.name); break;

	case AST_EXPRESSION_BINARY:			report_error_location(node->expr_binary.token_op); break;
	case AST_EXPRESSION_LITERAL:		report_error_location(node->expr_literal.token); break;
	case AST_EXPRESSION_PROCEDURE_CALL:	report_error_location(node->expr_proc_call.name); break;
	case AST_EXPRESSION_UNARY:			report_error_location(node->expr_unary.token_op); break;
	case AST_EXPRESSION_VARIABLE:		report_error_location(node->expr_variable.name); break;

	case AST_COMMAND_BLOCK:					break;// TODO(psv): put token for block start in the ast
	case AST_COMMAND_BREAK:					break;//report_error_location(node->comm_break); break;
	case AST_COMMAND_CONTINUE:				break;//report_error_location(node->comm_cont); break;
	case AST_COMMAND_FOR:					break;//report_error_location(node->comm_); break;
	case AST_COMMAND_IF:					break;//report_error_location(node->comm_); break;
	case AST_COMMAND_RETURN:				break;//report_error_location(node->comm_); break;
	case AST_COMMAND_VARIABLE_ASSIGNMENT:	break;//report_error_location(node->comm_); break;
	}
}

Decl_Error report_type_error(Decl_Error e, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Type Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}

Decl_Error report_semantic_error(Decl_Error e, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Semantic Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}

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

static void print_name_of_decl(FILE* out, Ast* decl) {
	switch (decl->node_type) {
		case AST_DECL_CONSTANT: {
			fprintf(out, "constant %.*s", TOKEN_STR(decl->decl_constant.name));
		}break;
		case AST_DECL_VARIABLE: {
			fprintf(out, "variable %.*s", TOKEN_STR(decl->decl_variable.name));
		}break;
		case AST_DECL_PROCEDURE: {
			fprintf(out, "procedure %.*s", TOKEN_STR(decl->decl_procedure.name));
		}break;
		case AST_DECL_ENUM: {
			fprintf(out, "enum %.*s", TOKEN_STR(decl->decl_enum.name));
		}break;
		case AST_DECL_UNION: {
			fprintf(out, "union %.*s", TOKEN_STR(decl->decl_union.name));
		}break;
		case AST_DECL_STRUCT: {
			fprintf(out, "struct %.*s", TOKEN_STR(decl->decl_struct.name));
		}break;
	}
}

static void print_dependency(FILE* out, Ast* decl) {

}

static void report_dependencies_error() {
	fprintf(stderr, "Semantic Error: dependencies unresolved:\n");
	size_t n = array_get_length(infer_queue);
	for (size_t i = 0; i < n; ++i) {
		print_name_of_decl(stderr, infer_queue[i]);
		fprintf(stderr, " is depending on ");
		print_dependency(stderr, infer_queue[i]);
		fprintf(stderr, "\n");
	}
}

// ---------------------------------------------
// -------------- Declarations -----------------
// ---------------------------------------------

Decl_Error report_redeclaration(char* node_type, Token* redeclared, Token* existing) {
	report_error_location(redeclared);
	fprintf(stderr, "Semantic Error: redeclaration of %s %.*s \n", node_type, TOKEN_STR(redeclared));
	fprintf(stderr, " - previously defined at\n");
	report_error_location(existing);
	fprintf(stderr, "\n");
	return DECL_ERROR_FATAL;
}

Decl_Error decl_check_redefinition(Scope* scope, Ast* node) {
	Decl_Error error = DECL_OK;

	Token* decl_name = 0;
	char*  decl_string_desc = 0;

	switch (node->node_type) {
		// TODO(psv): check forward declarations, they must not raise an error
		case AST_DECL_PROCEDURE:	decl_string_desc = "procedure"; decl_name = node->decl_procedure.name; break;
		case AST_DECL_CONSTANT:		decl_string_desc = "constant";  decl_name = node->decl_constant.name; break;
		case AST_DECL_ENUM:			decl_string_desc = "enum";      decl_name = node->decl_enum.name; break;
		case AST_DECL_STRUCT:		decl_string_desc = "struct";    decl_name = node->decl_struct.name; break;
		case AST_DECL_VARIABLE:		decl_string_desc = "variable";  decl_name = node->decl_variable.name; break;

		// TODO(psv): unions
		case AST_DECL_UNION: assert(0); break;
		default:             assert(0); break;
	}
	// check for redefinition of the symbol
	s64 index = symbol_table_entry_exist(&scope->symb_table, node->decl_constant.name);
	if (index != -1) {
		error |= report_redeclaration(decl_string_desc, node->decl_constant.name, symbol_table_get_entry(&scope->symb_table, index));
	} else {
		symbol_table_add(&scope->symb_table, node->decl_constant.name, node);
	}
	return error;
}

// resolves and internalizes type if it can, if the type was resolved set TYPE_FLAG_INTERNALIZE and TYPE_FLAG_RESOLVED flags
Type_Instance* resolve_type(Scope* scope, Type_Instance* type) {
	if (type->flags & TYPE_FLAG_RESOLVED)
		return type;

	switch (type->kind) {
		case KIND_PRIMITIVE:
			return type;
		case KIND_POINTER: {
			type->flags |= TYPE_FLAG_SIZE_RESOLVED;
			type->type_size_bits = type_pointer_size();
			type->pointer_to = resolve_type(scope, type->pointer_to);
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
			type->array_desc.array_of = resolve_type(scope, type->array_desc.array_of);
			if (type->array_desc.array_of->flags & TYPE_FLAG_INTERNALIZED) {
				type->flags |= TYPE_FLAG_RESOLVED | TYPE_FLAG_SIZE_RESOLVED;
				type->type_size_bits = type->array_desc.dimension * type->array_desc.array_of->type_size_bits;
				return internalize_type(&type, true);
			} else {
				return type;
			}
		} break;
		case KIND_FUNCTION: {
			Type_Instance* ret_type = resolve_type(scope, type->function_desc.return_type);
			if (!(ret_type->flags & TYPE_FLAG_INTERNALIZED)) {
				return type;
			} else {
				size_t nargs = type->function_desc.num_arguments;
				for (size_t i = 0; i < nargs; ++i) {
					Type_Instance* t = type->function_desc.arguments_type[i];
					t = resolve_type(scope, type->function_desc.arguments_type[i]);
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

Decl_Error resolve_types_decls(Scope* scope, Ast* node) {
	if (!(node->flags & AST_FLAG_IS_DECLARATION)) {
		return DECL_OK;
	}

	Decl_Error error = DECL_OK;

	switch (node->node_type) {
		case AST_DECL_VARIABLE:{
			if (!node->decl_variable.variable_type) {
				// infer from expr
				if (!node->decl_variable.assignment) {
					error |= report_semantic_error(DECL_ERROR_FATAL, "cannot infer variable type if no assignment expression is given\n");
					return error;
				}
				Type_Instance* type = infer_from_expression(node->decl_variable.assignment, &error);
				if (error & DECL_ERROR_FATAL) return error;
				if (!type) {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
					return error;
				}
				type->flags |= TYPE_FLAG_RESOLVED;
				node->decl_variable.variable_type = internalize_type(&type, true);
				return error;
			}
			if (node->decl_variable.variable_type->flags & TYPE_FLAG_RESOLVED) {
				return error;
			} else {
				Type_Instance* type = resolve_type(scope, node->decl_variable.variable_type);
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
					Decl_Error e = resolve_types_decls(node->decl_struct.struct_scope, node->decl_struct.fields[i]);
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
				node->decl_procedure.type_return = resolve_type(scope, node->decl_procedure.type_return);
				if (!(node->decl_procedure.type_return->flags & TYPE_FLAG_INTERNALIZED)) {
					infer_queue_push(node);
					error |= DECL_QUEUED_TYPE;
					return error;
				}
			}
			size_t nargs = node->decl_procedure.arguments_count;
			for (size_t i = 0; i < nargs; ++i) {
				error |= resolve_types_decls(scope, node->decl_procedure.arguments[i]);
			}
			if (error & DECL_QUEUED_TYPE) {
				infer_queue_push(node);
				return error;
			}

			if (!(node->decl_procedure.type_procedure->flags & TYPE_FLAG_RESOLVED)) {
				// return type must be resolved if we are here, so check only arguments				
				for (size_t i = 0; i < nargs; ++i) {
					Type_Instance* t = resolve_type(scope, node->decl_procedure.type_procedure->function_desc.arguments_type[i]);
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
		error |= resolve_types_decls(global_scope, node);
	}
	
	if (error & DECL_ERROR_FATAL) {
		return DECL_ERROR_FATAL;
	}

	size_t n = array_get_length(infer_queue);
	while (error) {
		error = DECL_OK;
		for (size_t i = 0; i < n; ++i) {
			error |= resolve_types_decls(global_scope, infer_queue[i]);
		}
		if (error & DECL_ERROR_FATAL) break;
		size_t infer_queue_end_size = array_get_length(infer_queue);
		if (infer_queue_end_size == n) {
			report_dependencies_error();
			return DECL_ERROR_FATAL;
		}
	}

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
	static s32 indent_level = 1;

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

	printf("(id:%d) %s scope [%d]:\n", scope->id, description, scope->level);

	for (s32 i = 0; i < scope->symb_table.entries_capacity; ++i) {
		if (scope->symb_table.entries[i].occupied) {
			DEBUG_print_indent_level(indent_level);
			printf("%.*s\n", TOKEN_STR(scope->symb_table.entries[i].identifier));
			
			Ast* n = scope->symb_table.entries[i].decl_node;

			switch (n->node_type) {
				case AST_DECL_CONSTANT:
				case AST_DECL_VARIABLE:
					break;
				case AST_DECL_PROCEDURE:
					indent_level += 1;
					DEBUG_print_scope_decls(n->decl_procedure.arguments_scope);
					indent_level -= 1;
					break;
				case AST_DECL_ENUM:
					indent_level += 1;
					DEBUG_print_scope_decls(n->decl_enum.enum_scope);
					indent_level -= 1;
					break;
				case AST_DECL_STRUCT:
					indent_level += 1;
					DEBUG_print_scope_decls(n->decl_struct.struct_scope);
					indent_level -= 1;
					break;
				case AST_DECL_UNION: assert(0); break;
				default:             assert(0); break;
			}
		}
	}
}