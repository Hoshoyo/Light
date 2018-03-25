#include "decl_check.h"
#include "symbol_table.h"
#include "type_table.h"

void report_error_location(Token* tok) {
	fprintf(stderr, "%.*s (%d:%d) ", tok->filename.length, tok->filename.data, tok->line, tok->column);
}

Decl_Error report_declaration_error(Decl_Error e, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Semantic Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}

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

Decl_Error decl_check_top_level(Scope* global_scope, Ast** ast_top_level) {
	Decl_Error error = DECL_OK;

	// Initialize the global scope symbol table
	symbol_table_init(&global_scope->symb_table, (global_scope->decl_count + 4) * 8);

	size_t ndecls = array_get_length(ast_top_level);
	for (size_t i = 0; i < ndecls; ++i) {
		Ast* node = ast_top_level[i];
		assert(node->flags & AST_FLAG_IS_DECLARATION);

		error |= decl_check_redefinition(global_scope, node);
	}
	return error;
}


static Ast** infer_queue;

Decl_Error infer_type_decl(Ast* node) {
	Decl_Error error = DECL_OK;
	Scope* scope = node->scope;

	switch (node->node_type) {
		case AST_DECL_CONSTANT:{
			if (!(node->decl_constant.type_info->flags & TYPE_INSTANCE_RESOLVED)) {
				// try to resolve
			}
		} break;
		case AST_DECL_VARIABLE: {
			if (!(node->decl_variable.variable_type->flags & TYPE_INSTANCE_RESOLVED)) {
				// try to resolve
			} else {
				// is resolved but not internalized
				if (!(node->decl_variable.variable_type->flags & TYPE_INSTANCE_INTERNALIZED)) {
					internalize_type(&node->decl_variable.variable_type);
				}
			}
		}break;
		case AST_DECL_PROCEDURE:
		case AST_DECL_STRUCT:
		case AST_DECL_ENUM:
		case AST_DECL_UNION: assert(0);
	}
	return error;
}

Decl_Error infer_type_command(Ast* node) {
	return 0;
}

Decl_Error infer_type_expression(Ast* node) {
	return 0;
}

Decl_Error infer_type_node(Ast* node) {
	Decl_Error error = DECL_OK;

	if (node->flags & AST_FLAG_IS_DECLARATION) {
		error |= infer_type_decl(node);
	} else if (node->flags & AST_FLAG_IS_COMMAND) {

	} else if (node->flags & AST_FLAG_IS_EXPRESSION) {

	} else {
		// internal compiler error
		assert(0);
	}
	return error;
}

Decl_Error infer_types(Scope* global_scope, Ast** ast_top_level) {
	Decl_Error error = DECL_OK;
	size_t n = array_get_length(ast_top_level);

	infer_queue = array_create(Ast*, n);

	for (size_t i = 0; i < n; ++i) {
		Ast* node = ast_top_level[i];
		error |= infer_type_node(node);
	}

	return error;
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