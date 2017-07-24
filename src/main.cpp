#define HO_SYSTEM_IMPLEMENT
#include <ho_system.h>
#include "lexer.h"
#include "parser.h"
#include "type.h"
#include "symbol_table.h"
#include "semantic.h"

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

	Parser parser(&lexer);
	Scope* global_scope = 0;
	Ast** ast = parser.parse_top_level(&global_scope);

	if (check_declarations(ast, global_scope) == DECL_CHECK_PASSED) {
		printf("\n*** Code preview ***\n\n");
		DEBUG_print_ast(stdout, ast);
	}

	return 0;
}