#define HO_SYSTEM_IMPLEMENT
#include <ho_system.h>
#include "lexer.h"
#include "parser.h"
#include "type.h"
#include "symbol_table.h"
#include "semantic.h"
#include "interpreter.h"
#include "code_generator.h"

int main(int argc, char** argv) {
	if (argc <= 1) {
		fprintf(stderr, "No files were given.\nusage: prism [option ...] filename\n");
		return -1;
	}
	
	Lexer lexer;
	
	if (lexer.start(argv[1]) != LEXER_SUCCESS) {
		fprintf(stderr, "There were errors, exiting.\n");
		return -1;
	}

	Scope* global_scope = create_scope(0, 0, 0);
	Ast** ast = create_array(Ast*, 2048);	// @TODO estimate size here @IMPORTANT
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
	printf("\n\n");
	//DEBUG_print_type_table();
	//DEBUG_print_node_type(stdout, ast, true);
	printf("\n\nNumber of values in the infer queue = %d\n", get_arr_length(infer_queue));
	printf("\n\n");
	DEBUG_print_ast(stdout, ast);
#endif
	return 0;
}