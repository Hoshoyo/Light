#define HO_SYSTEM_IMPLEMENT
#include <ho_system.h>
#include "lexer.h"
#include "parser.h"
#include "type.h"
#include "symbol_table.h"
#include "semantic.h"
#include "interpreter.h"
#include "code_generator.h"
#include "llvm_backend.h"

int main(int argc, char** argv) {
	if (argc <= 1) {
		fprintf(stderr, "No files were given.\nusage: %s [option ...] filename\n", argv[0]);
		return -1;
	}
	
	Lexer::init();

	Lexer lexer;
	if (lexer.start(argv[1]) != LEXER_SUCCESS) {
		return -1;
	}

	Scope* global_scope = create_scope(0, 0, 0);
	Ast** ast = array_create(Ast*, 2048);	// @TODO estimate size here
	initialize_types(global_scope, ast);

	Parser parser(&lexer);
	ast = parser.parse_top_level(global_scope, ast);
	if (ast == 0) {
		// file is empty
		return -1; 
	}
	if (check_and_submit_declarations(ast, global_scope) != DECL_CHECK_PASSED) {
		return -1;
	}
	if (do_type_inference(ast, global_scope, &type_table) != 0) {
		return -1;
	}

#if 0
	init_interpreter();
	generate_interpreter_code(ast);
	run_interpreter();
#endif

#if 1
	//printf("\n\n");
	//DEBUG_print_type_table();
	//DEBUG_print_node_type(stdout, ast, true);
	printf("\n\nNumber of values in the infer queue = %d\n", array_get_length(infer_queue));
	printf("\n\n");
	DEBUG_print_ast(stdout, ast);
#endif

	// LLVM backend
#if defined (_WIN32) || defined(_WIN64)
	//llvm_generate_ir(ast);
#endif
	return 0;
}