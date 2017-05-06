#define HO_SYSTEM_IMPLEMENT
#include <ho_system.h>
#include "lexer.h"
#include "parser.h"
#include "type.h"

int main(int argc, char** argv) {
	if (argc <= 1) {
		fprintf(stderr, "No files were given.\nusage: prism [option ...] filename");
		return -1;
	}

	Lexer lexer;
	if (lexer.start(argv[1]) != LEXER_SUCCESS) {
		fprintf(stderr, "There were errors, exiting.\n");
		return -1;
	}

	setup_types();

	Parser parser(&lexer);
	Ast** ast = parser.parse_top_level();

	DEBUG_print_ast(stdout, ast);

	return 0;
}