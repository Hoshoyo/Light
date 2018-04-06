#include "error.h"

void report_error_location(Token* tok) {
	fprintf(stderr, "%.*s (%d:%d) ", tok->filename.length, tok->filename.data, tok->line, tok->column);
}
void report_error_location(Ast* node) {
	switch (node->node_type) {
	case AST_DECL_CONSTANT:				report_error_location(node->decl_constant.name); break;
	case AST_DECL_VARIABLE:				report_error_location(node->decl_variable.name); break;
	case AST_DECL_PROCEDURE:			report_error_location(node->decl_procedure.name); break;
	case AST_DECL_ENUM:					report_error_location(node->decl_enum.name); break;
	case AST_DECL_STRUCT:				report_error_location(node->decl_struct.name); break;
	case AST_DECL_UNION:				report_error_location(node->decl_union.name); break;

	case AST_EXPRESSION_BINARY:			report_error_location(node->expr_binary.token_op); break;
	case AST_EXPRESSION_LITERAL:		report_error_location(node->expr_literal.token); break;
	case AST_EXPRESSION_PROCEDURE_CALL:	report_error_location(node->expr_proc_call.name); break;
	case AST_EXPRESSION_UNARY:			report_error_location(node->expr_unary.token_op); break;
	case AST_EXPRESSION_VARIABLE:		report_error_location(node->expr_variable.name); break;

	case AST_COMMAND_BLOCK:					break;// TODO(psv): put token for block start in the ast
	case AST_COMMAND_BREAK:					report_error_location(node->comm_break.token_break); break;
	case AST_COMMAND_CONTINUE:				report_error_location(node->comm_continue.token_continue); break;
	case AST_COMMAND_FOR:					break;//report_error_location(node->comm_); break;
	case AST_COMMAND_IF:					break;//report_error_location(node->comm_); break;
	case AST_COMMAND_RETURN:				report_error_location(node->comm_return.token_return); break;
	case AST_COMMAND_VARIABLE_ASSIGNMENT:	break;//report_error_location(node->comm_); break;
	}
}

/*
	Type Errors
*/

Type_Error report_type_error(Type_Error e, char* fmt, ...){
    va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Type Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}
Type_Error report_type_error(Type_Error e, Token* location, char* fmt, ...){
    report_error_location(location);
    va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Type Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}
Type_Error report_type_error(Type_Error e, Ast* location, char* fmt, ...){
    report_error_location(location);
    va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Type Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}

/*
	Declaration Errors
*/

Decl_Error report_decl_error(Decl_Error e, char* fmt, ...){
    va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Declaration Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}
Decl_Error report_decl_error(Decl_Error e, Token* location, char* fmt, ...){
    report_error_location(location);
    va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Declaration Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}
Decl_Error report_decl_error(Decl_Error e, Ast* location, char* fmt, ...){
    report_error_location(location);
    va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Declaration Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}

Decl_Error report_semantic_error(Decl_Error e, char* fmt, ...){
    va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Semantic Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}
Decl_Error report_semantic_error(Decl_Error e, Token* location, char* fmt, ...){
    report_error_location(location);
    va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Semantic Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}
Decl_Error report_semantic_error(Decl_Error e, Ast* location, char* fmt, ...){
    report_error_location(location);
    va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Semantic Error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return e;
}
/*
	Dependencies Errors
*/

static void print_dependency(FILE* out, Ast* decl) {

}

static void print_name_of_decl(FILE* out, Ast* decl) {
	switch (decl->node_type) {
		case AST_DECL_CONSTANT: {
			fprintf(out, "constant %.*s", TOKEN_STR(decl->decl_constant.name));
		}break;
		case AST_DECL_VARIABLE: {
			fprintf(out, "variable %.*s", TOKEN_STR(decl->decl_variable.name));
		}break;
		case AST_DECL_PROCEDURE: {
			fprintf(out, "procedure %.*s", TOKEN_STR(decl->decl_procedure.name));
		}break;
		case AST_DECL_ENUM: {
			fprintf(out, "enum %.*s", TOKEN_STR(decl->decl_enum.name));
		}break;
		case AST_DECL_UNION: {
			fprintf(out, "union %.*s", TOKEN_STR(decl->decl_union.name));
		}break;
		case AST_DECL_STRUCT: {
			fprintf(out, "struct %.*s", TOKEN_STR(decl->decl_struct.name));
		}break;
	}
}

void report_dependencies_error(Ast** infer_queue) {
	fprintf(stderr, "Semantic Error: dependencies unresolved:\n");
	size_t n = array_get_length(infer_queue);
	for (size_t i = 0; i < n; ++i) {
		print_name_of_decl(stderr, infer_queue[i]);
		fprintf(stderr, " is depending on ");
		print_dependency(stderr, infer_queue[i]);
		fprintf(stderr, "\n");
	}
}