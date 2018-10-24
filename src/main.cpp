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
#include "interpreter.h"
#include "bytecode_gen.h"

void initialize() {
	TIME_FUNC();
	type_table_init();
	file_table_init();
	Lexer::init();
	Parser::init();
}

extern double total_file_load_time;

int main(int argc, char** argv) {
	bool verbose = false;

	if (argc < 2) {
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		return -1;
	}

	// @Temporary: flag -v for verbose print out.
	if(argc >= 3) {
		if(c_str_equal(argv[2], "-v")) {
			verbose = true;
		}
		if(c_str_equal(argv[2], "-d")) {
			print_debug_c = true;
		}
	}

	Timer timer;
	double start = timer.GetTime();

	initialize();

	// Global scope
	Scope  global_scope = { 0 };

	// TODO(psv): multiple input files
	queue_file_for_parsing(argv[1]);

	Ast** ast_top_level = parse_files_in_queue(&global_scope);
	if(ast_top_level == 0) {
		return -1;
	}

	{
		//TIME_FUNC_NAMED("decl_checking");
		// TODO(psv): Fuse type checking, and also refactor it
		Type_Error decl_err = decl_check_top_level(&global_scope, ast_top_level);
		if (decl_err & TYPE_ERROR_FATAL) {
			return -1;
		}
	}

	{
		//TIME_FUNC_NAMED("type check");
		Type_Error type_error = type_check(&global_scope, ast_top_level);
		if (type_error) {
			return -1;
		}
	}

	double end = timer.GetTime();
	printf("File load time:     %f ms\n", total_file_load_time);
	printf("Compiler elapsed:   %f ms\n", (end - start));
	printf("Lexer processed:    %d LoC\n", global_lexer_line_count);

	if(verbose){
		// TODO(psv): make compiler options/flags to print this
		DEBUG_print_ast(stdout, ast_top_level, true);
		//DEBUG_print_scope_decls(&global_scope);
		//DEBUG_print_type_table();
		DEBUG_print_type_table_structs();
	}

#if 1
	double bend_start = timer.GetTime();
	c_generate(ast_top_level, g_type_table, argv[1], (char*)ho_current_exe_full_path(), g_lib_table);
	double bend_end = timer.GetTime();

	printf("Backend      elapsed: %f ms\n", (bend_end - bend_start));
	printf("Total        elapsed: %f ms\n", ((end - start) + (bend_end - bend_start)));
	print_profile();
#else
	{
		Interpreter interp = init_interpreter();
		bytecode_generate(&interp, ast_top_level);
		run_interpreter(&interp);
		//print_profile();
	}
#endif
	return 0;
}