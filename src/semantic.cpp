#include "semantic.h"

static int scope_error = 0;
static int declaration_ratio = 8;

static int report_semantic_error(Decl_Site* site, char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	if (site) {
		fprintf(stderr, "%.*s:%d:%d: Semantic Error: ", site->filename.length, site->filename.data, site->line, site->column);
	}
	vfprintf(stderr, msg, args);
	va_end(args);
	return 0;
}

static int report_semantic_warning(Decl_Site* site, char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	if (site) {
		fprintf(stderr, "%.*s:%d:%d: Warning: ", site->filename.length, site->filename.data, site->line, site->column);
	}
	vfprintf(stderr, msg, args);
	va_end(args);
	return 0;
}

static void report_declaration_site(Ast* node) {
	assert(node->is_decl);
	Decl_Site* site = 0;
	Token* name = 0;
	switch (node->node) {
		case AST_NODE_VARIABLE_DECL: {
			site = &node->var_decl.site;
			name = node->var_decl.name;
		}break;
		case AST_NODE_PROC_DECLARATION: {
			site = &node->proc_decl.site;
			name = node->proc_decl.name;
		}break;
		case AST_NODE_STRUCT_DECLARATION: {
			site = &node->struct_decl.site;
			name = node->struct_decl.name;
		}break;
	}
	int num = fprintf(stderr, "%.*s:%d:%d: ", site->filename.length, site->filename.data, site->line, site->column);
	fprintf(stderr, "%.*s\n", name->value.length, name->value.data);
	while (num > 0) {
		fprintf(stderr, "-");
		num -= 1;
	}
	fprintf(stderr, "^   Previously defined here.\n");
}

int check_declarations(Ast* node, Scope* scope) {
	if (node->is_decl) {
		switch (node->node) {
		case AST_NODE_VARIABLE_DECL: {
			assert(scope->symb_table);
			//
			// check if variable exist on this scope
			//
			s64 hash = scope->symb_table->entry_exist(node->var_decl.name);
			if (hash == -1) {
				hash = scope->symb_table->insert(scope, node->var_decl.name, node);
			} else {
				report_semantic_error(&node->var_decl.site, "Variable %.*s redefinition.\n", node->var_decl.name->value.length, node->var_decl.name->value.data);
				report_declaration_site(scope->symb_table->entries[hash].node);
				return 0;
			}
		}break;
		case AST_NODE_PROC_DECLARATION: {
			assert(scope->symb_table);
			//
			// check if the proc decl is not redefined in the same scope
			//
			s64 hash = scope->symb_table->entry_exist(node->proc_decl.name);
			if (hash == -1) {
				hash = scope->symb_table->insert(scope, node->proc_decl.name, node);
			} else {
				report_semantic_error(&node->proc_decl.site, "Variable %.*s redefinition.\n", node->proc_decl.name->value.length, node->proc_decl.name->value.data);
				report_declaration_site(scope->symb_table->entries[hash].node);
				return 0;
			}
			//
			// check for duplicate arguments
			//
			int num_args = node->proc_decl.num_args;
			int error = DECL_CHECK_PASSED;
			if (num_args > 0) {
				node->proc_decl.scope->symb_table = new Symbol_Table(num_args * declaration_ratio);
				printf("Proc (%d - level[%d]) has %d arguments.\n", node->proc_decl.scope->id, node->proc_decl.scope->level, node->proc_decl.scope->num_declarations);
				for (int i = 0; i < num_args; ++i) {
					int ret = check_declarations(node->proc_decl.arguments[i], node->proc_decl.scope);
					if (ret != DECL_CHECK_PASSED) error = DECL_CHECK_FAILED;
				}
			}
			int ret = check_declarations(node->proc_decl.body, node->proc_decl.scope);
			if (ret != DECL_CHECK_PASSED) error = DECL_CHECK_FAILED;
			if (error == DECL_CHECK_FAILED) return DECL_CHECK_FAILED;
		}break;
		case AST_NODE_STRUCT_DECLARATION: {
			Scope* struct_scope = node->struct_decl.scope;
			printf("Struct (%d - level[%d]) has %d declarations.\n", struct_scope->id, struct_scope->level, struct_scope->num_declarations);
		}break;

		case AST_NODE_NAMED_ARGUMENT: {
			//
			// check for duplicate named arguments from within a function
			//
			assert(scope->symb_table);
			s64 hash = scope->symb_table->entry_exist(node->named_arg.arg_name);
			if (hash == -1) {
				hash = scope->symb_table->insert(scope, node->named_arg.arg_name, node);
			} else {
				report_semantic_error(&node->named_arg.site, "Procedure argument %.*s redefinition.\n", node->named_arg.arg_name->value.length, node->named_arg.arg_name->value.data);
				fprintf(stderr, "Previously defined as argument #%d in the procedure.\n", scope->symb_table->entries[hash].node->named_arg.index + 1);
				return 0;
			}
		}break;

		default: {
			// should not be reached
			report_semantic_error(0, "Internal compiler error: unknown declaration node type.\n");
			scope_error = 1;
			return 0;
		}break;
		}
	}
	if (node->node == AST_NODE_BLOCK) {
		//
		// check recursively for all the declarations and commands of this block
		//
		Scope* block_scope = node->block.scope;
		printf("Block (%d - level[%d]) has %d declarations.\n", block_scope->id, block_scope->level, block_scope->num_declarations);

		if (block_scope->num_declarations) {
			// if there is at least 1 declaration, then create a symbol table (declaration ratio) times
			// bigger than the number of declarations

			block_scope->symb_table = new Symbol_Table(declaration_ratio * block_scope->num_declarations);
		}
		return check_declarations(node->block.commands, block_scope);
	}
	return 1;
}

int check_declarations(Ast** ast, Scope* global_scope)
{
	size_t num_nodes = get_arr_length(ast);

	global_scope->symb_table = new Symbol_Table(global_scope->num_declarations * declaration_ratio);

	for (size_t i = 0; i < num_nodes; ++i) {
		Ast* node = ast[i];
		if (check_declarations(node, global_scope) == 0) {
			return DECL_CHECK_FAILED;
		}
	}
	return DECL_CHECK_PASSED;
}

int infer_node_types(Ast* node, Scope* scope, Type_Table* table)
{
	if (node->is_decl) {
		switch (node->node)
		{
			case AST_NODE_PROC_DECLARATION: {
				// proc itself

				// arguments
				int num_args = node->proc_decl.num_args;
				for (int i = 0; i < num_args; ++i) {
					infer_node_types(node->proc_decl.arguments[i], node->proc_decl.scope, table);
				}
				// body
				infer_node_types(node->proc_decl.body, node->proc_decl.scope, table);
			}break;

			case AST_NODE_VARIABLE_DECL: {
				create_type(&node->var_decl.type, true);
			}break;
		}
	} else {
		switch (node->node)
		{
			case AST_NODE_PROCEDURE_CALL: {

			}break;

			case AST_NODE_BINARY_EXPRESSION: {

			}break;
				
			case AST_NODE_IF_STATEMENT: {

			}break;

			case AST_NODE_BLOCK: {
				type_inference(node->block.commands, scope, table);
			}break;
		}
	}
	return 0;
}

int type_inference(Ast** ast, Scope* global_scope, Type_Table* table)
{
	size_t num_nodes = get_arr_length(ast);

	for (size_t i = 0; i < num_nodes; ++i) {
		Ast* node = ast[i];
		infer_node_types(node, global_scope, table);
	}
	return DECL_CHECK_PASSED;
}