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
#include "c_backend.h"
#include "file_table.h"

void initialize() {
	type_table_init();
	file_table_init();
	Lexer::init();
	Parser::init();
}

int main(int argc, char** argv) {
	bool verbose = false;

	if (argc < 2) {
		fprintf(stderr, "usage: %s filename", argv[0]);
		return -1;
	}

	// @Temporary: flag -v for verbose print out.
	if(argc >= 3) {
		if(c_str_equal(argv[2], "-v")) {
			verbose = true;
		}
	}

	Timer timer;
	double start = timer.GetTime();

	initialize();

	// Global scope
	Scope  global_scope = { 0 };

	// TODO(psv): multiple input files
	queue_file_for_parsing("internal/definitions.li");
	queue_file_for_parsing(argv[1]);

	Ast** ast_top_level = parse_files_in_queue(&global_scope);
	if(ast_top_level == 0) {
		return -1;
	}
	DEBUG_print_ast(stdout, ast_top_level, false);

	// TODO(psv): Fuse type checking, and also refactor it
	Decl_Error decl_err = decl_check_top_level(&global_scope, ast_top_level);
	
	if (decl_err & (~DECL_ERROR_WARNING)) {
		return -1;
	}
	Type_Error type_error = type_check(&global_scope, ast_top_level);
	if (type_error) {
		return -1;
	}

	double end = timer.GetTime();
	printf("Compiler     elapsed: %fms\n", (end - start));

	if(verbose){
		// TODO(psv): make compiler options/flags to print this
		DEBUG_print_ast(stdout, ast_top_level, true);
		//DEBUG_print_scope_decls(&global_scope);
		//DEBUG_print_type_table();
		DEBUG_print_type_table_structs();
	}
	
	double bend_start = timer.GetTime();
	c_generate(ast_top_level, g_type_table, argv[1], argv[0], g_lib_table);
	double bend_end = timer.GetTime();
	
	printf("Backend      elapsed: %fms\n", (bend_end - bend_start));
	printf("Total        elapsed: %fms\n", ((end - start) + (bend_end - bend_start)));
	return 0;
}
