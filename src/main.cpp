#define HO_SYSTEM_IMPLEMENT
#include <ho_system.h>
#include "lexer.h"
#include "parser.h"
#include "type.h"
#include "symbol_table.h"
#include "semantic.h"
#include "interpreter.h"

int main(int argc, char** argv) {
	//init_interpreter();
	//run_interpreter();

	if (argc <= 1) {
		fprintf(stderr, "No files were given.\nusage: prism [option ...] filename\n");
		return -1;
	}
	
	Lexer lexer;
	
	if (lexer.start(argv[1]) != LEXER_SUCCESS) {
		fprintf(stderr, "There were errors, exiting.\n");
		return -1;
	}

	Parser parser(&lexer);
	Scope* global_scope = create_scope(0, 0, 0);
	Ast** ast = parser.parse_top_level(global_scope);
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
	//DEBUG_print_node_type(stdout, ast, true);
	//if (do_type_check(ast, &type_table) == 0) {
	//	return -1;
	//}
	printf("\n\nNumber of values in the infer queue = %d\n", get_arr_length(infer_queue));
#if 1
	//printf("\n\n");
	//DEBUG_print_type_table();
	printf("\n\n");
	DEBUG_print_ast(stdout, ast);
#endif
	return 0;
}
