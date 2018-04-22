#define HO_SYSTEM_IMPLEMENT
#include <ho_system.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "util.h"
#include "decl_check.h"
#include "type_table.h"
#include "type_infer.h"
#include "type_check.h"
#include "llvm_backend.h"

void initialize() {
	type_table_init();
	Lexer::init();
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s filename", argv[0]);
		return -1;
	}

	initialize();

	Lexer lexer;
	if (lexer.start(argv[1]) != LEXER_OK)
		return EXIT_FAILURE;

	// Global scope
	Scope  global_scope = { 0 };

	Parser parser(&lexer, &global_scope);
	Ast** ast_top_level = parser.parse_top_level();

	Decl_Error decl_err = decl_check_top_level(&global_scope, ast_top_level);
	if (decl_err & (~DECL_ERROR_WARNING)) {
		return -1;
	}

	DEBUG_print_ast(stdout, ast_top_level, true);

	Type_Error type_err = type_check(&global_scope, ast_top_level);
	if (type_err) {
		return -1;
	}

	DEBUG_print_ast(stdout, ast_top_level, true);
	//DEBUG_print_scope_decls(&global_scope);
	DEBUG_print_type_table();

	llvm_generate_ir(ast_top_level, g_type_table, argv[1]);
	return 0;
}
