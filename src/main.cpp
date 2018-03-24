#define HO_SYSTEM_IMPLEMENT
#include <ho_system.h>
#include "lexer.h"
#include "ast.h"
#include "util.h"

void initialize() {
	Lexer::init();
}

int main(int argc, char** argv) {
	initialize();

	Lexer lexer;
	if (lexer.start(argv[1]) != LEXER_OK)
		return EXIT_FAILURE;

	// Global scope
	Scope  global_scope = { 0 };

	Scope* filescope = scope_create(0, &global_scope, SCOPE_FILESCOPE);
	filescope->filename = string_make(argv[1]);
	//Ast** ast_top_level = parse_top_level(filescope, &lexer);

	return 0;
}

#if 0
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
		fprintf(stderr, "There were errors, exiting...\n");
		return -1; 
	}
	if (check_and_submit_declarations(ast, global_scope) != DECL_CHECK_PASSED) {
		fprintf(stderr, "There were errors, exiting...\n");
		return -1;
	}
	if (do_type_inference(ast, global_scope, &type_table) != 0) {
		fprintf(stderr, "There were errors, exiting...\n");
		return -1;
	}

#if 0
	init_interpreter();
	generate_interpreter_code(ast);
	run_interpreter();
#endif

#if 1
	//DEBUG_print_type_table(&type_table);
	//DEBUG_print_node_type(stdout, ast, true);
	//printf("\n\nNumber of values in the infer queue = %d\n\n", array_get_length(infer_queue));
	DEBUG_print_ast(stdout, ast);
	DEBUG_check_returntype_ast(ast);
#endif

	// LLVM backend
	llvm_generate_ir(ast, &type_table, argv[1]);
	return 0;
}
#endif