#include "parser.h"
#include "util.h"
#include "type.h"
#include "type_table.h"
#include <stdarg.h>
#include <ho_system.h>

static Parse_Queue parsing_queue;
static String_Hash_Table lib_table;
string* g_lib_table = 0;

void Parser::init() {
	if(!parsing_queue.queue_imports) {
		parsing_queue.queue_imports = array_create(Token*, 4);
	}
	if(!parsing_queue.files_toplevels) {
		parsing_queue.files_toplevels = array_create(Ast**, 4);
	}
	if(!parsing_queue.queue_main) {
		parsing_queue.queue_main = array_create(char*, 4);
	}
}

void lib_table_push(Token* t) {
	assert(t->type == TOKEN_LITERAL_STRING);

	if(lib_table.entries_capacity == 0) {
		hash_table_init(&lib_table, 4096 * 8);
		g_lib_table = array_create(string, 1024);
	}
	string s = string_make((char*)t->value.data, t->value.length);
	s64 index = hash_table_entry_exist(&lib_table, s);
	if(index == -1) {
		hash_table_add(&lib_table, s);
		array_push(g_lib_table, &s);
	}
}

void queue_file_for_parsing(char* filename) {
	array_push(parsing_queue.queue_main, &filename);
}

void queue_file_for_parsing(Token* token) {
	array_push(parsing_queue.queue_imports, &token);
}

char internal_data[] = R"(
string :: struct {
	length   : s64;
	capacity : s64;
	data     : ^u8;
}
)";

Ast** parse_files_in_queue(Scope* global_scope) {
	TIME_FUNC();
	// TODO(psv): check for double include
	// TODO(psv): include recursively

	{
		// internal
		Lexer lexer;
		if(lexer.start_internal(internal_data) != LEXER_OK) {
			return 0;
		}

		Parser parser(&lexer, global_scope);
		Ast** ast_top_level = parser.parse_top_level();
		array_push(parsing_queue.files_toplevels, &ast_top_level);
	}

	// Main files
	size_t mlen = array_get_length(parsing_queue.queue_main);
	for(size_t i = 0; i < mlen; ++i) {
		char* file = parsing_queue.queue_main[i];

		Lexer lexer;
		if (lexer.start(file) != LEXER_OK)
			return 0;

		Parser parser(&lexer, global_scope);
		Ast** ast_top_level = parser.parse_top_level();
		array_push(parsing_queue.files_toplevels, &ast_top_level);
	}

	while(array_get_length(parsing_queue.queue_imports) > 0) {
		Token* file = parsing_queue.queue_imports[0];
		// remove the first of the array
		array_remove(parsing_queue.queue_imports, 0);

		Lexer lexer;
		// TODO(psv): Make lexer accept my style of string for filename so
		// we dont need to allocate a name for this
		char* c_filename = make_c_string((char*)file->value.data, file->value.length);
		if (lexer.start(c_filename, file) != LEXER_OK)
			return 0;

		Parser parser(&lexer, global_scope);
		Ast** ast_top_level = parser.parse_top_level();
		if(ast_top_level){
			array_push(parsing_queue.files_toplevels, &ast_top_level);
		}
	}

	size_t num_files = array_get_length(parsing_queue.files_toplevels);
	size_t num_top_level_decls = 0;
	for(size_t i = 0; i < num_files; ++i) {
		num_top_level_decls += array_get_length(parsing_queue.files_toplevels[i]);
	}

	// Fuse all top levels
	Ast** top_level = array_create(Ast*, num_top_level_decls);
	for(size_t i = 0; i < num_files; ++i) {
		size_t ndecls = array_get_length(parsing_queue.files_toplevels[i]);
		for(size_t j = 0; j < ndecls; ++j){
			array_push(top_level, &parsing_queue.files_toplevels[i][j]);
		}
	}

	return top_level;
}

void Parser::report_error_location(Token* tok) {
	fprintf(stderr, "%.*s:%d:%d ", tok->filename.length, tok->filename.data, tok->line, tok->column);
}

void Parser::report_syntax_error(Token* error_token, char* msg, ...) {
	va_list args;
	va_start(args, msg);
	report_error_location(error_token);
	fprintf(stderr, "Syntax Error: ");
	vfprintf(stderr, msg, args);
	va_end(args);
	system_exit(-1);
}

void Parser::report_fatal_error(Token* error_token, char* msg, ...) {
	va_list args;
	va_start(args, msg);
	report_error_location(error_token);
	vfprintf(stderr, msg, args);
	va_end(args);
	system_exit(-1);
}

Token* Parser::require_and_eat(Token_Type tt) {
	Token* t = lexer->eat_token();
	if(t->type == TOKEN_END_OF_STREAM && tt != TOKEN_END_OF_STREAM)
		report_syntax_error(t, "unexpected end of stream, expected '%s'\n", lexer->get_token_string(tt));
	if (t->type != tt) 
		report_syntax_error(t, "expected '%s', but got '%.*s'\n", lexer->get_token_string(tt), TOKEN_STR(t));
	return t;
}
Token* Parser::require_and_eat(char t) {
	return require_and_eat((Token_Type)t);
}

Ast** Parser::parse_top_level() {
	TIME_FUNC();
	// Empty file check
	if (lexer->token_count == 1 && lexer->peek_token_type() == TOKEN_END_OF_STREAM)
		return 0;
	Parser_Error perr = PARSER_OK;

	top_level = array_create(Ast*, 64);

	while (perr == PARSER_OK && lexer->peek_token_type() != TOKEN_END_OF_STREAM) {
		if(lexer->peek_token_type() == '#') {
			parse_directive(global_scope);
			continue;
		}
		Ast* decl = parse_declaration(global_scope);
		switch (decl->node_type) {
			case AST_DECL_VARIABLE:
			case AST_DECL_CONSTANT:
				require_and_eat(';');
				break;
			default:
				break;
		}
		if (!decl) break;
		array_push(top_level, &decl);
	}

	if (perr == PARSER_ERROR_FATAL) {
		fprintf(stderr, "There were errors, exiting...\n");
		return 0;
	}
	return top_level;
}

// pushes a u8* literal to the data segment
Ast* Parser::data_global_string_push(Token* s) {

	char* data = (char*)s->value.data;
	s64   length = s->value.length;

	s->value = compiler_tags[COMPILER_TAG_STRING];

	Ast* node = ast_create_data(GLOBAL_STRING, global_scope, s, (u8*)data, length, type_pointer_get(TYPE_PRIMITIVE_U8));
	array_push(top_level, &node);
	return node;
}

Ast* Parser::parse_directive_expression(Scope* scope) {
	require_and_eat('#');

	Token* directive = lexer->eat_token();

	if (directive->type != TOKEN_IDENTIFIER) {
		report_syntax_error(directive, "expected compiler expression directive but got '%.*s'\n", TOKEN_STR(directive));
	}

	if (directive->value.data == compiler_tags[COMPILER_TAG_SIZEOF].data) {
		// #sizeof directive
		Type_Instance* type = parse_type();
		return ast_create_expr_sizeof(type, scope, directive);
	} else if (directive->value.data == compiler_tags[COMPILER_TAG_TYPEOF].data) {
		// #typeof directive
		Ast* expr = parse_expression(scope);
		return ast_create_expr_typeof(expr, scope, directive);
	} else if(directive->value.data == compiler_tags[COMPILER_TAG_RUN].data) {
		// #run directive
		Ast* expr = parse_expression(scope);
		return ast_create_expr_run(scope, directive, expr);
	} else {
		report_syntax_error(directive, "unrecognized compiler directive '%.*s'\n", TOKEN_STR(directive));
	}
}

void Parser::parse_directive(Scope* scope) {
	require_and_eat('#');

	Token* directive = lexer->eat_token();
	if(directive->type != TOKEN_IDENTIFIER) {
		report_syntax_error(directive, "expected compiler directive but got '%.*s'\n", TOKEN_STR(directive));
	}

	// #import directive
	if(directive->value.data == compiler_tags[COMPILER_TAG_IMPORT].data){
		Token* import_str = lexer->eat_token();
		if(import_str->type == TOKEN_LITERAL_STRING) {
			//
			// keep relative path to import other files from 
			// relative path and not absolute
			//
			char* ptr = 0;
			size_t size = 0;

			char* import_c_str = make_c_string((char*)import_str->value.data, import_str->value.length);
			if(ho_file_exists(import_c_str)) {
				ptr = import_c_str;
				size = import_str->value.length;
			} else {
				string solo_path = string_new_append(&lexer->path, (char*)import_str->value.data, import_str->value.length);
				char* b = make_c_string(solo_path);
				ptr = ho_realpath(b, &size);
				free(b);
				string_free(&solo_path);
			}

			if(ptr) {
				s64 file_index = file_table_push(ptr);
				if(file_index != -1) {
					import_str->value.data = (u8*)ptr;
					import_str->value.length = size;
					queue_file_for_parsing(import_str);
				} else {
					// do nothing, since this file was already included
				}
			} else {
				report_fatal_error(import_str, "import directive, file '%.*s' could not be found\n", TOKEN_STR(import_str));
			}
		} else {
			report_syntax_error(directive, "expected filepath after import directive but got '%.*s'\n", TOKEN_STR(directive));	
		}
	} else if(directive->value.data == compiler_tags[COMPILER_TAG_FOREIGN].data) {
		require_and_eat('(');
		Token* libname = lexer->eat_token();
		if(libname->type == TOKEN_LITERAL_STRING) {
			if(!current_foreign) {
				current_foreign = array_create(Token*, 4);
			}
			array_push(current_foreign, &libname);
			require_and_eat(')');
		} else {
			report_syntax_error(libname, "expected library name after #foreign directive but got '%.*s'\n", TOKEN_STR(libname));
		}
	} else if(directive->value.data == compiler_tags[COMPILER_TAG_END].data){
		if(current_foreign && array_get_length(current_foreign) > 0) {
			array_pop(current_foreign);
		} else {
			report_syntax_error(directive, "#end directive is not ending any directive block\n");
		}
	} else {
		report_syntax_error(directive, "unrecognized compiler directive '%.*s'\n", TOKEN_STR(directive));
	}
}

// -------------------------------------------
// ------------- Declarations ----------------
// -------------------------------------------

Ast* Parser::parse_declaration(Scope* scope) {
	Token* name = lexer->eat_token();

	if (name->type != TOKEN_IDENTIFIER)
		report_syntax_error(name, "invalid identifier %.*s on declaration.\n", TOKEN_STR(name));

	Token* decl = lexer->eat_token();

	if (decl->type == ':')
	{
		Token* next = lexer->peek_token();
		if (next->type == ':') {
			// struct, procedure and constant declaration
			lexer->eat_token(); // :
			next = lexer->peek_token();
			if (next->type == '(') {
				return parse_decl_proc(name, scope);
			} else if (next->type == TOKEN_KEYWORD_STRUCT) {
				return parse_decl_struct(name, scope);
			} else if(next->type == TOKEN_KEYWORD_UNION) {
				return parse_decl_union(name, scope);
			} else if (next->type == TOKEN_KEYWORD_ENUM) {
				return parse_decl_enum(name, scope, 0);
			} else {
				return parse_decl_constant(name, scope, 0);
			}
		} else if (next->value.data == compiler_tags[COMPILER_TAG_TYPEDEF].data) {
			// this is a type alias
			return parse_decl_typedef(name, scope);
		} else {
			// type for a variable, enum or constant declaration
			Type_Instance* declaration_type = 0;
			if (lexer->peek_token_type() != '=')
				declaration_type = parse_type();
			next = lexer->peek_token();
			if (next->type == ':') {
				// typed constant
				lexer->eat_token(); // ':'
				if (lexer->peek_token_type() == TOKEN_KEYWORD_ENUM)
					return parse_decl_enum(name, scope, declaration_type);
				else
					return parse_decl_constant(name, scope, declaration_type);
			} else {
				// variable declaration
				return parse_decl_variable(name, scope, declaration_type);
			}
		}
	} else {
		report_syntax_error(decl, "invalid declaration of '%.*s', declaration requires ':'\n", TOKEN_STR(name));
	}
	return 0;
}

Ast* Parser::parse_decl_typedef(Token* name, Scope* scope) {
	Token* tdef = lexer->eat_token();
	assert(tdef->value.data == compiler_tags[COMPILER_TAG_TYPEDEF].data);
	require_and_eat(':');

	Type_Instance* type = parse_type();
	require_and_eat(';');
	return ast_create_decl_typedef(name, scope, type);
}

Ast* Parser::parse_decl_proc(Token* name, Scope* scope) {
	require_and_eat('(');

	Ast**          arguments = 0;
	Scope*         arguments_scope = 0;
	Type_Instance* return_type = 0;
	s32 nargs = 0;

	if (lexer->peek_token_type() != ')') {
		arguments_scope = scope_create(0, scope, SCOPE_PROCEDURE_ARGUMENTS);
		arguments = array_create(Ast*, 4);

		for (;;) {
			if (nargs != 0)
				require_and_eat(',');
			Token* name = lexer->eat_token();
			if (name->type != TOKEN_IDENTIFIER) {
				report_syntax_error(name, "expected argument declaration but got '%.*s'\n", TOKEN_STR(name));
			}
			Ast* arg = parse_decl_variable(name, arguments_scope);
			array_push(arguments, &arg);
			++nargs;
			if (lexer->peek_token_type() != ',')
				break;
		}
	}
	require_and_eat(')');

	if (lexer->peek_token_type() == '{') {
		return_type = type_primitive_get(TYPE_PRIMITIVE_VOID);
	} else {
		require_and_eat(TOKEN_ARROW);
		return_type = parse_type();
	}

	Type_Instance* proc_type = type_new_temporary();
	proc_type->kind = KIND_FUNCTION;
	proc_type->function_desc.arguments_names = array_create(string, nargs);
	proc_type->function_desc.arguments_type  = array_create(Type_Instance*, nargs);
	proc_type->function_desc.num_arguments = nargs;
	proc_type->function_desc.return_type = return_type;
	for (s32 i = 0; i < nargs; ++i) {
		array_push(proc_type->function_desc.arguments_names, &arguments[i]->decl_variable.name->value);
		array_push(proc_type->function_desc.arguments_type, &arguments[i]->decl_variable.variable_type);
	}

	scope->decl_count += 1;
	Ast* body = 0;
	u32  flags = 0;
	Token* extern_library_name = 0;

	Token* next = lexer->peek_token();
	// TODO(psv): forward declared procs (foreign)
	if (next->type == '#') {
		lexer->eat_token();
		Token* tag = lexer->eat_token();
		if (compiler_tags[COMPILER_TAG_FOREIGN].data == tag->value.data) {
			require_and_eat('(');
			Token* libname = lexer->eat_token();
			// TODO(psv): still not used
			if (libname->type == TOKEN_LITERAL_STRING) {
				extern_library_name = libname;
				lib_table_push(libname);
			} else {
				report_syntax_error(tag, "foreign compiler tag requires string literal as library path\n");
			}
			require_and_eat(')');
		} else {
			report_syntax_error(tag, "expected compiler directive but got '%.*s'\n", TOKEN_STR(tag));
		}
		require_and_eat(';');
		flags |= DECL_PROC_FLAG_FOREIGN;
	} else if(next->type == ';') {
		// make sure this is inside a foreign block, or TODO:(psv): treat as a forward declaration
		lexer->eat_token();
		if(current_foreign && array_get_length(current_foreign) > 0) {
			extern_library_name = current_foreign[array_get_length(current_foreign) - 1];
			lib_table_push(extern_library_name);
			flags |= DECL_PROC_FLAG_FOREIGN;
		} else {
			// TODO(psv): temporary, this should be possible
			report_syntax_error(next, "cannot forward declare outside a #foreign block\n");
		}
	} else {
		if (arguments_scope)
			body = parse_comm_block(arguments_scope, 0);
		else
			body = parse_comm_block(scope, 0);
		if(body->comm_block.block_scope){
			body->comm_block.block_scope->flags |= SCOPE_PROCEDURE_BODY;
		}
	}

	Ast* node = ast_create_decl_proc(name, scope, arguments_scope, proc_type, arguments, body, return_type, flags, nargs);
	node->decl_procedure.extern_library_name = extern_library_name;
	if (body) {
		body->comm_block.creator = node;
	}

	if(arguments_scope)
		arguments_scope->creator_node = node;
	return node;
}

Ast* Parser::parse_decl_variable(Token* name, Scope* scope, Type_Instance* type) {
	Ast* assignment = 0;

	Token* next = lexer->peek_token();
	if (next->type == '=') {
		next = lexer->eat_token();
		assignment = parse_expression(scope);
	}
	scope->decl_count += 1;
	return ast_create_decl_variable(name, scope, assignment, type, 0);
}

Ast* Parser::parse_decl_variable(Token* name, Scope* scope) {
	require_and_eat(':');

	Type_Instance* var_type = 0;

	Token* next = lexer->peek_token();
	if (next->type != '=')
		var_type = parse_type();

	scope->decl_count += 1;
	return parse_decl_variable(name, scope, var_type);
}

Ast* Parser::parse_decl_union(Token* name, Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_UNION);

	s32    fields_count = 0;
	Ast**  fields = array_create(Ast*, 8);
	Scope* scope_union = scope_create(0, scope, SCOPE_UNION);

	require_and_eat('{');

	for (;;) {
		Token* field_name = lexer->eat_token();
		if (field_name->type != TOKEN_IDENTIFIER) {
			report_syntax_error(field_name, "expected union field declaration, but got '%.*s'\n", TOKEN_STR(field_name));
		}
		Ast* field = parse_decl_variable(field_name, scope_union);
		require_and_eat(';');
		array_push(fields, &field);
		++fields_count;
		if (lexer->peek_token_type() == '}')
			break;
	}

	require_and_eat('}');

	Type_Instance* union_type = type_new_temporary();
	union_type->kind = KIND_UNION;
	union_type->union_desc.fields_names = array_create(string, fields_count);
	union_type->union_desc.fields_types = array_create(Type_Instance*, fields_count);
	union_type->union_desc.fields_count = fields_count;
	union_type->union_desc.alignment    = 0;
	union_type->union_desc.name = name;
	array_allocate(union_type->union_desc.fields_names, fields_count);
	array_allocate(union_type->union_desc.fields_types, fields_count);
	for (s32 i = 0; i < fields_count; ++i) {
		union_type->union_desc.fields_names[i] = fields[i]->decl_variable.name->value;
		union_type->union_desc.fields_types[i] = fields[i]->decl_variable.variable_type;
	}

	scope->decl_count += 1;

	Ast* node = ast_create_decl_union(name, scope, scope_union, union_type, fields, 0, fields_count);
	scope_union->creator_node = node;
	return node;
}

Ast* Parser::parse_decl_struct(Token* name, Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_STRUCT);

	s32    fields_count = 0;
	Ast**  fields = array_create(Ast*, 8);
	Scope* scope_struct = scope_create(0, scope, SCOPE_STRUCTURE);

	require_and_eat('{');

	for (;;) {
		Token* field_name = lexer->eat_token();
		if (field_name->type != TOKEN_IDENTIFIER) {
			report_syntax_error(field_name, "expected struct field declaration, but got '%.*s'\n", TOKEN_STR(field_name));
		}
		Ast* field = parse_decl_variable(field_name, scope_struct);
		require_and_eat(';');
		array_push(fields, &field);
		++fields_count;
		if (lexer->peek_token_type() == '}')
			break;
	}

	require_and_eat('}');

	Type_Instance* struct_type = type_new_temporary();
	struct_type->kind = KIND_STRUCT;
	struct_type->struct_desc.fields_names = array_create(string, fields_count);
	struct_type->struct_desc.fields_types = array_create(Type_Instance*, fields_count);
	struct_type->struct_desc.offset_bits  = array_create(s64, fields_count);
	struct_type->struct_desc.fields_count = fields_count;
	struct_type->struct_desc.alignment    = 0;
	struct_type->struct_desc.name = name;
	array_allocate(struct_type->struct_desc.fields_names, fields_count);
	array_allocate(struct_type->struct_desc.fields_types, fields_count);
	array_allocate(struct_type->struct_desc.offset_bits, fields_count);
	for (s32 i = 0; i < fields_count; ++i) {
		struct_type->struct_desc.fields_names[i] = fields[i]->decl_variable.name->value;
		struct_type->struct_desc.fields_types[i] = fields[i]->decl_variable.variable_type;
	}

	scope->decl_count += 1;

	Ast* node = ast_create_decl_struct(name, scope, scope_struct, struct_type, fields, 0, fields_count);
	scope_struct->creator_node = node;
	return node;
}

Ast* Parser::parse_decl_enum(Token* name, Scope* scope, Type_Instance* hint_type) {

	require_and_eat(TOKEN_KEYWORD_ENUM);
	require_and_eat('{');

	if (!hint_type) {
		// TODO(psv): decide default value
		hint_type = type_primitive_get(TYPE_PRIMITIVE_U32);
	}

	s32    fields_count = 0;
	Ast**  fields = array_create(Ast*, 4);
	Scope* enum_scope = scope_create(0, scope, SCOPE_ENUM);

	for (;;) {
		if (fields_count != 0)
			require_and_eat(',');
		Token* const_name = lexer->eat_token();
		if (const_name->type != TOKEN_IDENTIFIER)
			report_syntax_error(const_name, "expected enum field constant declaration, but got '%.*s'\n", TOKEN_STR(const_name));
		require_and_eat(':');
		Ast* field = parse_decl_constant(const_name, enum_scope, hint_type);
		array_push(fields, &field);
		++fields_count;
		if (lexer->peek_token_type() != ',')
			break;
	}

	require_and_eat('}');

	scope->decl_count += 1;

	Ast* node = ast_create_decl_enum(name, scope, enum_scope, fields, hint_type, 0, fields_count);
	enum_scope->creator_node = node;
	return node;
}

Ast* Parser::parse_decl_constant(Token* name, Scope* scope, Type_Instance* type) {
	// either literal or another constant
	Ast* value = parse_expression(scope);

	scope->decl_count += 1;

	return ast_create_decl_constant(name, scope, value, type, 0);
}

// -------------------------------------------
// ------------- Expressions -----------------
// -------------------------------------------
/*
Ast* Parser::parse_expr_proc_call(Scope* scope) {
	Token* name = lexer->eat_token();
	Ast**  arguments = 0;
	s32    args_count = 0;
	
	require_and_eat('(');
	if (lexer->peek_token_type() != ')') {
		arguments = array_create(Ast*, 4);
		for (;;) {
			if (args_count != 0)
				require_and_eat(',');
			Ast* argument = parse_expression(scope);
			array_push(arguments, &argument);
			++args_count;
			if (lexer->peek_token_type() != ',')
				break;
		}
	}
	require_and_eat(')');

	return ast_create_expr_proc_call(scope, name, arguments, args_count);
}*/

Ast* Parser::parse_expression_precedence10(Scope* scope) {
	Token* t = lexer->peek_token();
	if (t->flags & TOKEN_FLAG_LITERAL) {
		// literal
		return parse_expr_literal(scope);
	} /*else if(t->type == TOKEN_KEYWORD_STRUCT) {

	} */else if(t->type == TOKEN_KEYWORD_ARRAY) {
		// array literal
		return parse_expr_literal_array(scope);
	} else if (t->type == TOKEN_IDENTIFIER) {
		// variable
		lexer->eat_token();
		if (lexer->peek_token_type() == ':') {
			lexer->eat_token();
			return parse_expr_literal_struct(t, scope);
		}
		else {
			return ast_create_expr_variable(t, scope, 0);
		}
	} else if(t->type == '#') {
		return parse_directive_expression(scope);
	} else if(t->type == '(') {
		// ( expr )
		lexer->eat_token();
		Ast* expr = parse_expression(scope);
		require_and_eat(')');
		return expr;
	} else {
		report_syntax_error(t, "expected expression but got '%.*s'\n", TOKEN_STR(t));
	}
	return 0;
}

Ast* Parser::parse_expression_precedence9(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_precedence10(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '['){
			lexer->eat_token();
			Ast* r = parse_expression(scope);
			expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
			require_and_eat(']');
		} else if(op->type == '(') {
			// procedure call
			Ast**  arguments = 0;
			s32    args_count = 0;
			
			require_and_eat('(');
			if (lexer->peek_token_type() != ')') {
				arguments = array_create(Ast*, 4);
				for (;;) {
					if (args_count != 0)
						require_and_eat(',');
					Ast* argument = parse_expression(scope);
					array_push(arguments, &argument);
					++args_count;
					if (lexer->peek_token_type() != ',')
						break;
				}
			}
			require_and_eat(')');

			return ast_create_expr_proc_call(scope, expr, arguments, args_count);
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_left_dot(Scope* scope){
	Token* op = 0;
	Ast* expr = parse_expression_precedence10(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '['){
			lexer->eat_token();
			Ast* r = parse_expression(scope);
			expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
			require_and_eat(']');
		} else if(op->type == '(') {
			// procedure call
			Ast**  arguments = 0;
			s32    args_count = 0;
			
			require_and_eat('(');
			if (lexer->peek_token_type() != ')') {
				arguments = array_create(Ast*, 4);
				for (;;) {
					if (args_count != 0)
						require_and_eat(',');
					Ast* argument = parse_expression(scope);
					array_push(arguments, &argument);
					++args_count;
					if (lexer->peek_token_type() != ',')
						break;
				}
			}
			require_and_eat(')');

			return ast_create_expr_proc_call(scope, expr, arguments, args_count);
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence8(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_left_dot(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '.'){
			lexer->eat_token();
			Ast* r = parse_expression_precedence10(scope);
			expr = ast_create_expr_binary(scope, expr, r, OP_BINARY_DOT, op);
			while(true) {
				op = lexer->peek_token();
				if(op->type == '['){
					lexer->eat_token();
					Ast* r = parse_expression(scope);
					expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
					require_and_eat(']');
				} else {
					break;
				}
			}
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence7(Scope* scope) {
	return parse_expression_precedence8(scope);
}

Ast* Parser::parse_expression_precedence6(Scope* scope) {
	Token_Type next = lexer->peek_token_type();
	if (next == '~' || next == '!' || next == '&' || next == '*' || next == '-' || next == '+') {
		Token* op = lexer->eat_token();
		Ast* operand = parse_expression_precedence6(scope);
		return ast_create_expr_unary(scope, operand, token_to_unary_op(op), op, 0, UNARY_EXPR_FLAG_PREFIXED);
	} else if (next == '[') {
		// parse cast
		//Token* cast = lexer->eat_token();
		Token* cast = require_and_eat('[');
		Type_Instance* ttc = parse_type();
		require_and_eat(']');
		Ast* operand = parse_expression_precedence6(scope);
		return ast_create_expr_unary(scope, operand, OP_UNARY_CAST, cast, ttc, UNARY_EXPR_FLAG_PREFIXED);
	}
	return parse_expression_precedence7(scope);
}

Ast* Parser::parse_expression_precedence5(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_precedence6(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '*' || op->type == '/' || op->type == '%'){
			lexer->eat_token();
			Ast* r = parse_expression_precedence6(scope);
			expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence4(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_precedence5(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '+' || op->type == '-'){
			lexer->eat_token();
			Ast* r = parse_expression_precedence5(scope);
			expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence3(Scope* scope) {
	Token* op = 0;
	Ast* expr = parse_expression_precedence4(scope);
	while(true) {
		op = lexer->peek_token();
		if(op->type == '^' || op->type == '|' || op->type == '&' || op->type == TOKEN_BITSHIFT_LEFT || op->type == TOKEN_BITSHIFT_RIGHT){
			lexer->eat_token();
			Ast* r = parse_expression_precedence4(scope);
			expr = ast_create_expr_binary(scope, expr, r, token_to_binary_op(op), op);
		} else {
			break;
		}
	}
	return expr;
}

Ast* Parser::parse_expression_precedence2(Scope* scope) {
	Ast* expr = parse_expression_precedence3(scope);
	Token_Type next = lexer->peek_token_type();
	if (next == TOKEN_EQUAL_COMPARISON || next == TOKEN_GREATER_EQUAL || next == TOKEN_NOT_EQUAL ||
		next == TOKEN_LESS_EQUAL || next == '<' || next == '>') 
	{
		Token* op = lexer->eat_token();
		Ast* right = parse_expression_precedence2(scope);
		return ast_create_expr_binary(scope, expr, right, token_to_binary_op(op), op);
	}
	return expr;
}

Ast* Parser::parse_expression_precedence1(Scope* scope) {
	Ast* expr = parse_expression_precedence2(scope);
	Token_Type next = lexer->peek_token_type();
	if (next == TOKEN_LOGIC_OR || next == TOKEN_LOGIC_AND) {
		Token* op = lexer->eat_token();
		Ast* right = parse_expression_precedence1(scope);
		return ast_create_expr_binary(scope, expr, right, token_to_binary_op(op), op);
	}
	return expr;
}

Ast* Parser::parse_expression(Scope* scope) {
	return parse_expression_precedence1(scope);
}

Ast* Parser::parse_expr_literal_struct(Token* name, Scope* scope){
	Ast* node = ast_create_expr_literal(scope, LITERAL_STRUCT, name, 0, 0);
	require_and_eat('{');
	
	Ast** exprs = array_create(Ast*, 8);
	if(lexer->peek_token_type() != '}'){
		while(true){
			Ast* expr = parse_expression(scope);
			array_push(exprs, &expr);

			if(lexer->peek_token_type() != ','){
				break;
			} else {
				lexer->eat_token();
			}
		}
		node->expr_literal.struct_exprs = exprs;
	}
	require_and_eat('}');
	return node;
}

Ast* Parser::parse_expr_literal_array(Scope* scope) {
	Token* arraytok = require_and_eat(TOKEN_KEYWORD_ARRAY);
	require_and_eat('{');

	Ast* node = ast_create_expr_literal(scope, LITERAL_ARRAY, arraytok, 0, 0);

	if(lexer->peek_token_type() != '}') {
		Ast** exprs = array_create(Ast*, 16);
		while(true){
			Ast* expr = parse_expression(scope);
			array_push(exprs, &expr);

			if(lexer->peek_token_type() != ','){
				break;
			} else {
				lexer->eat_token();
			}
		}
		node->expr_literal.array_exprs = exprs;
	}

	require_and_eat('}');
	return node;
}

Ast* Parser::parse_expr_literal(Scope* scope) {
	Token* first = lexer->eat_token();
	Ast* node = ast_create_expr_literal(scope, LITERAL_UNKNOWN, first, 0, 0);
	bool negate = false;
	bool prefixed = false;
	switch (first->type) {
		case '-':
			negate = true;
		case '+':
			first = lexer->eat_token();
			prefixed = true;
		break;
		default: break;
	}
	if (prefixed && first->type == TOKEN_LITERAL_INT) {
		node->expr_literal.type = LITERAL_SINT;
		node->expr_literal.value_s64 = str_to_s64((char*)first->value.data, first->value.length);
		if (negate)
			node->expr_literal.value_s64 = -node->expr_literal.value_s64;
	} else if (prefixed) {
		report_syntax_error(first, "expected integer literal after %c operator but got '%.*s'\n", (char)first->type, TOKEN_STR(first));
	} else {
		switch (first->type) {
			case TOKEN_LITERAL_INT: {
				node->expr_literal.type = LITERAL_SINT;
				node->expr_literal.value_s64 = str_to_s64((char*)first->value.data, first->value.length);
			} break;
			case TOKEN_LITERAL_HEX_INT: {
				node->expr_literal.type = LITERAL_HEX_INT;
				node->expr_literal.value_u64 = literal_integer_to_u64(first);
			} break;
			case TOKEN_LITERAL_BIN_INT: {
				node->expr_literal.type = LITERAL_BIN_INT;
				node->expr_literal.value_u64 = literal_integer_to_u64(first);
			} break;
			case TOKEN_LITERAL_BOOL_FALSE: {
				node->expr_literal.type = LITERAL_BOOL;
				node->expr_literal.value_bool = false;
			} break;
			case TOKEN_LITERAL_BOOL_TRUE: {
				node->expr_literal.type = LITERAL_BOOL;
				node->expr_literal.value_bool = true;
			} break;
			case TOKEN_LITERAL_CHAR:
				// TODO(psv): utf8 encoding
				node->expr_literal.type = LITERAL_UINT;
				node->expr_literal.value_u64 = literal_char_to_u64(first);
				node->type_return = type_primitive_get(TYPE_PRIMITIVE_U32);
				break;
			case TOKEN_LITERAL_FLOAT:
				node->expr_literal.type = LITERAL_FLOAT;
				node->expr_literal.value_r64 = literal_float_to_r64(first);
				break;
			case TOKEN_LITERAL_STRING: {
				node->expr_literal.type = LITERAL_STRUCT;
				node->expr_literal.flags |= LITERAL_FLAG_STRING;

				// example "Hello"
				// string { 5, -1, &global_array }

				Ast* g_data = data_global_string_push(first);
				node->expr_literal.struct_exprs = array_create(Ast*, 3);
				Ast** exprs = node->expr_literal.struct_exprs;

				// Length
				Ast* length_expr = ast_create_expr_literal(scope, LITERAL_HEX_INT, first, 0, type_primitive_get(TYPE_PRIMITIVE_S64));
				length_expr->expr_literal.value_s64 = first->real_string_length;

				Ast* capacity_expr = ast_create_expr_literal(scope, LITERAL_HEX_INT, first, 0, type_primitive_get(TYPE_PRIMITIVE_S64));
				capacity_expr->expr_literal.value_s64 = -1;	// start immutable

				array_push(exprs, &length_expr);
				array_push(exprs, &capacity_expr);
				array_push(exprs, &g_data);
				node->expr_literal.struct_exprs = exprs;
			} break;
			case TOKEN_KEYWORD_NULL: {
				node->expr_literal.type = LITERAL_POINTER;
				node->expr_literal.value_s64 = 0;
			}break;
			default: {
				report_syntax_error(first, "expected literal expression but got '%.*s'\n", TOKEN_STR(first));
			}break;
		}
	}

	return node;
}

// -------------------------------------------
// --------------- Commands ------------------
// -------------------------------------------

Ast** Parser::parse_command_comma_list(Scope* scope) {
	Ast** commands = array_create(Ast*, 2);
	while(true) {
		Ast* comm = parse_command(scope, false);
		array_push(commands, &comm);
		if (lexer->peek_token_type() != ',') {
			break;
		} else {
			lexer->eat_token();
		}
	}
	return commands;
}

Ast* Parser::parse_command(Scope* scope, bool eat_semicolon) {
	Token* next = lexer->peek_token();
	Ast* command = 0;
	switch (next->type) {
		case '{':
			command = parse_comm_block(scope, 0);
			break;
		case TOKEN_KEYWORD_IF:
			command = parse_comm_if(scope); 
			break;
		case TOKEN_KEYWORD_WHILE:
			command = parse_comm_while(scope);
			break;
		case TOKEN_KEYWORD_FOR:
			command = parse_comm_for(scope);
			break;
		case TOKEN_KEYWORD_BREAK:
			command = parse_comm_break(scope);
			if(eat_semicolon) require_and_eat(';');
			break;
		case TOKEN_KEYWORD_CONTINUE:
			command = parse_comm_continue(scope);
			if (eat_semicolon) require_and_eat(';');
			break;
		case TOKEN_KEYWORD_RETURN:
			command = parse_comm_return(scope);
			if (eat_semicolon) require_and_eat(';');
			break;
		case TOKEN_IDENTIFIER:{
			Token_Type t = lexer->peek_token_type(1);
			if (t == ':') {
				command = parse_declaration(scope);
			} else if (t == '(') {
				// syntatic sugar void proc call
				Ast* pcall = parse_expression(scope);
				command = ast_create_comm_variable_assignment(scope, 0, pcall);
			} else {
				command = parse_comm_variable_assignment(scope);
			}
			if (eat_semicolon) require_and_eat(';');
		}break;
		default: {
			command = parse_comm_variable_assignment(scope);
			if (eat_semicolon) require_and_eat(';');
		}break;
	}
	return command;
}

Ast* Parser::parse_comm_variable_assignment(Scope* scope) {
	s64 start_token = lexer->current_token;
	Ast* lvalue = parse_expression(scope);

	Token* op = lexer->eat_token();
	if (!(op->flags & TOKEN_FLAG_ASSIGNMENT_OPERATOR))
		report_syntax_error(op, "expected assignment operator on variable assignment, but got '%.*s'\n", TOKEN_STR(op));

	Ast* rvalue = parse_expression(scope);
	Operator_Binary binop = OP_BINARY_UNKNOWN;

	switch (op->type) {
		case '=':
			break;
		case TOKEN_PLUS_EQUAL:	binop = OP_BINARY_PLUS; break;
		case TOKEN_MINUS_EQUAL:	binop = OP_BINARY_MINUS; break;
		case TOKEN_TIMES_EQUAL:	binop = OP_BINARY_MULT; break;
		case TOKEN_DIV_EQUAL:	binop = OP_BINARY_DIV; break;
		case TOKEN_MOD_EQUAL:	binop = OP_BINARY_MOD; break;
		case TOKEN_SHL_EQUAL:	binop = OP_BINARY_SHL; break;
		case TOKEN_SHR_EQUAL:	binop = OP_BINARY_SHR; break;
		case TOKEN_XOR_EQUAL:	binop = OP_BINARY_XOR; break;
		case TOKEN_AND_EQUAL:	binop = OP_BINARY_AND; break;
		case TOKEN_OR_EQUAL:	binop = OP_BINARY_OR; break;
		default: {
			report_internal_compiler_error(__FILE__, __LINE__, "Token '%.*s' flag assignment operator expected to be set but apparently it is not.\n", TOKEN_STR(op));
		}break;
	}
	s64 end_token = lexer->current_token;

	// it needs to be a copy in order to be an RVALUE node not flagged.
	if(binop != OP_BINARY_UNKNOWN) {
		lexer->current_token = start_token;
		Ast* rvalue_rep = parse_expression(scope);
		lexer->current_token = end_token;
		rvalue = ast_create_expr_binary(scope, rvalue_rep, rvalue, binop, op);
	}

	return ast_create_comm_variable_assignment(scope, lvalue, rvalue);
}

Ast* Parser::parse_comm_return(Scope* scope) {
	Token* t = require_and_eat(TOKEN_KEYWORD_RETURN);
	Ast* expr = 0;
	if (lexer->peek_token_type() != ';')
		expr = parse_expression(scope);
	return ast_create_comm_return(scope, expr, t);
}

Ast* Parser::parse_comm_continue(Scope* scope) {
	Token* t = require_and_eat(TOKEN_KEYWORD_CONTINUE);
	return ast_create_comm_continue(scope, t);
}

Ast* Parser::parse_comm_break(Scope* scope) {
	Token* t = require_and_eat(TOKEN_KEYWORD_BREAK);
	Ast* lit = 0;
	Token_Type next_type = lexer->peek_token_type();
	if(next_type != ';' && next_type != ',')
		lit = parse_expr_literal(scope);
	return ast_create_comm_break(scope, lit, t);
}

Ast* Parser::parse_comm_while(Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_WHILE);

	Ast* condition = parse_expression(scope);

	Ast* body = parse_command(scope);
	if (body->node_type == AST_COMMAND_BLOCK) {
		if (body->comm_block.block_scope)
			body->comm_block.block_scope->flags |= SCOPE_LOOP;
		return ast_create_comm_for(scope, condition, body);
	}
	else {
		Ast** commands = array_create(Ast*, 1);
		array_push(commands, &body);
		Ast* inner = ast_create_comm_block(scope, scope_create(0, scope, SCOPE_LOOP), commands, 0, 1);

		Ast* for_cmd = ast_create_comm_for(scope, condition, inner);
		inner->comm_block.creator = for_cmd;

		inner->comm_block.block_scope->creator_node = for_cmd;
		inner->comm_block.block_scope->decl_count = 1;
		return ast_create_comm_for(scope, condition, inner);
	}
}

Ast* Parser::parse_comm_for(Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_FOR);

	Ast** commands = 0;
	Scope* for_scope = scope_create(0, scope, SCOPE_BLOCK);

	if (lexer->peek_token_type() != ';') {
		Ast** initialization = parse_command_comma_list(for_scope);
		commands = initialization;
	} else {
		commands = array_create(Ast*, 4);
	}
	require_and_eat(';');
	
	Ast* condition = parse_expression(for_scope);

	require_and_eat(';');

	Ast** deferred = 0;
	if (lexer->peek_token_type() != ';') {
		// fill scope of the commands with the loop scope later
		deferred = parse_command_comma_list(for_scope);
	}

	Ast* comm_for = 0;
	Ast* body = parse_command(for_scope);
	if(body->node_type == AST_COMMAND_BLOCK) {
		if (body->comm_block.block_scope) {
			body->comm_block.block_scope->flags |= SCOPE_LOOP;
		}
		if (!body->comm_block.commands && deferred) {
			body->comm_block.commands = array_create(Ast*, array_get_length(deferred));
		}
		comm_for = ast_create_comm_for(for_scope, condition, body);
	} else {
		Ast** commands = array_create(Ast*, 1);
		array_push(commands, &body);
		Ast* inner = ast_create_comm_block(for_scope, scope_create(0, for_scope, SCOPE_LOOP), commands, 0, 1);
		body = inner;
		
		Ast* for_cmd = ast_create_comm_for(for_scope, condition, inner);
		inner->comm_block.creator = for_cmd;

		inner->comm_block.block_scope->creator_node = for_cmd;
		inner->comm_block.block_scope->decl_count = 1;
		comm_for = ast_create_comm_for(for_scope, condition, inner);
	}

	// fill deferred
	if (deferred) {
		size_t def_count = array_get_length(deferred);
		body->comm_block.command_count += def_count;
		if (body->comm_block.commands) {
			body->comm_block.commands = (Ast**)array_append(body->comm_block.commands, deferred);
			array_release(deferred);
		} else {
			comm_for->comm_block.commands = deferred;
		}
	}

	array_push(commands, &comm_for);

	for_scope->creator_node = comm_for;
	return ast_create_comm_block(scope, for_scope, commands, comm_for, array_get_length(commands));
}

Ast* Parser::parse_comm_if(Scope* scope) {
	require_and_eat(TOKEN_KEYWORD_IF);
	Ast* condition = parse_expression(scope);
	Ast* command_true  = 0;
	Ast* command_false = 0;

	if (lexer->peek_token_type('{')) {
		command_true = parse_command(scope);
	} else {
		Ast** commands = array_create(Ast*, 1);
		Scope* block_scope = scope_create(0, scope, SCOPE_BLOCK);
		command_true = parse_command(block_scope);
		array_push(commands, &command_true);

		command_true = ast_create_comm_block(scope, block_scope, commands, 0, 1);
	}

	if (lexer->peek_token_type() == TOKEN_KEYWORD_ELSE) {
		lexer->eat_token();

		if (lexer->peek_token_type('{')) {
			command_false = parse_command(scope);
		} else {
			Ast** commands = array_create(Ast*, 1);
			Scope* block_scope = scope_create(0, scope, SCOPE_BLOCK);
			command_false = parse_command(block_scope);
			array_push(commands, &command_false);

			command_false = ast_create_comm_block(scope, block_scope, commands, 0, 1);
		}
	}
	Ast* cmd_if = ast_create_comm_if(scope, condition, command_true, command_false);
	command_true->scope->creator_node = cmd_if;
	if (command_false) {
		command_false->scope->creator_node = cmd_if;
	}

	return cmd_if;
}

Ast* Parser::parse_comm_block(Scope* scope, Ast* creator) {
	require_and_eat('{');
	
	if (lexer->peek_token_type() == '}') {
		lexer->eat_token();
		return ast_create_comm_block(scope, 0, 0, creator, 0);
	}

	Ast** commands = array_create(Ast*, 4);
	Scope* block_scope = scope_create(0, scope, SCOPE_BLOCK);

	s32 command_count = 0;
	for (;;) {
		Ast* command = parse_command(block_scope);
		array_push(commands, &command);
		++command_count;
		if (lexer->peek_token_type() == '}')
			break;
	}

	require_and_eat('}');

	Ast* node = ast_create_comm_block(scope, block_scope, commands, creator, command_count);
	node->comm_block.block_scope->creator_node = node;
	return node;
}


// -------------------------------------------
// ---------------- Types --------------------
// -------------------------------------------
// Parse Type, doest not internalize types yet

Type_Instance* Parser::parse_type() {
	Token* tok = lexer->eat_token();
	switch (tok->type) {
		case TOKEN_SINT64:
		case TOKEN_SINT32:
		case TOKEN_SINT16:
		case TOKEN_SINT8:
		case TOKEN_UINT64:
		case TOKEN_UINT32:
		case TOKEN_UINT16:
		case TOKEN_UINT8:
		case TOKEN_REAL32:
		case TOKEN_REAL64:
		case TOKEN_BOOL:
		case TOKEN_VOID:
			return parse_type_primitive();

		case '^':
			return parse_type_pointer();

		case TOKEN_IDENTIFIER:
			//return parse_type_struct(tok);
			return parse_type_alias(tok);

		case '[':
			return parse_type_array();

		case '(':
			return parse_type_function();

		default:
			report_syntax_error(tok, "expected type declaration, but got '%.*s'\n", TOKEN_STR(tok));
		break;
	}
	return 0;
}

Type_Instance* Parser::parse_type_primitive() {
	switch (lexer->last_token()->type) {
		case TOKEN_SINT64:	return type_primitive_get(TYPE_PRIMITIVE_S64);
		case TOKEN_SINT32:  return type_primitive_get(TYPE_PRIMITIVE_S32);
		case TOKEN_SINT16:  return type_primitive_get(TYPE_PRIMITIVE_S16);
		case TOKEN_SINT8:   return type_primitive_get(TYPE_PRIMITIVE_S8);
		case TOKEN_UINT64:  return type_primitive_get(TYPE_PRIMITIVE_U64);
		case TOKEN_UINT32:  return type_primitive_get(TYPE_PRIMITIVE_U32);
		case TOKEN_UINT16:  return type_primitive_get(TYPE_PRIMITIVE_U16);
		case TOKEN_UINT8:   return type_primitive_get(TYPE_PRIMITIVE_U8);
		case TOKEN_REAL32:  return type_primitive_get(TYPE_PRIMITIVE_R32);
		case TOKEN_REAL64:  return type_primitive_get(TYPE_PRIMITIVE_R64);
		case TOKEN_BOOL:    return type_primitive_get(TYPE_PRIMITIVE_BOOL);
		case TOKEN_VOID:    return type_primitive_get(TYPE_PRIMITIVE_VOID);
		default: report_internal_compiler_error(__FILE__, __LINE__, "tried to parse unknown primitive type\n"); break;
	}
}
Type_Instance* Parser::parse_type_pointer() {
	Type_Instance* t = type_new_temporary();
	t->kind  = KIND_POINTER;
	t->flags = TYPE_FLAG_SIZE_RESOLVED;
	t->type_size_bits = type_pointer_size();
	t->pointer_to = parse_type();
	return t;
}
Type_Instance* Parser::parse_type_alias(Token* name) {
	Type_Instance* t = type_new_temporary();
	t->kind = KIND_ALIAS;
	t->flags = 0;
	t->type_size_bits = 0;
	t->struct_desc.fields_names = 0;
	t->struct_desc.fields_types = 0;
	t->struct_desc.name = name;
	return t;
}

Type_Instance* Parser::parse_type_struct(Token* name) {
	Type_Instance* t = type_new_temporary();
	t->kind = KIND_STRUCT;
	t->flags = 0;
	t->type_size_bits = 0;
	t->struct_desc.fields_names = 0;
	t->struct_desc.fields_types = 0;
	t->struct_desc.name = name;
	return t;
}
Type_Instance* Parser::parse_type_array() {
	Token* dimension = lexer->eat_token();
	Type_Instance* t = type_new_temporary();
	t->kind = KIND_ARRAY;
	t->flags = 0;
	t->type_size_bits = 0;
	t->array_desc.dimension = 0;

	if (dimension->flags & TOKEN_FLAG_NUMERIC_LITERAL) {
		// numeric literal
		t->array_desc.dimension = literal_integer_to_u64(dimension);
		t->array_desc.dimension_evaluated = true;
	} else if (dimension->type == TOKEN_IDENTIFIER) {
		// TODO: constant or enum constant
		t->array_desc.constant_dimension_name = require_and_eat(TOKEN_IDENTIFIER);
		t->array_desc.dimension_evaluated = false;
	}
	require_and_eat(']');
	t->array_desc.array_of = parse_type();
	return t;
}
Type_Instance* Parser::parse_type_function() {
	Type_Instance* t = type_new_temporary();
	t->kind = KIND_FUNCTION;
	t->flags = 0;
	t->type_size_bits = 0;
	if (lexer->peek_token_type() == ')') {
		t->function_desc.arguments_names = 0;
		t->function_desc.arguments_type = 0;
		t->function_desc.num_arguments = 0;
		require_and_eat(')');
	} else {
		t->function_desc.arguments_type = array_create(Type_Instance*, 4);
		t->function_desc.num_arguments = 0;
		t->function_desc.arguments_names = 0;
		for(s32 i = 0; ; ++i) {
			if (i != 0)
				require_and_eat(',');
			Type_Instance* argtype = parse_type();
			array_push(t->function_desc.arguments_type, &argtype);
			t->function_desc.num_arguments += 1;
			if (lexer->peek_token_type() != ',')
				break;
		}
		require_and_eat(')');
	}
	require_and_eat(TOKEN_ARROW);
	t->function_desc.return_type = parse_type();
	return t;
}

// Expressions Precedence

Precedence Parser::unary_op_precedence_level(Operator_Unary unop, bool prefixed)
{
	if (prefixed) {
		if (unop == OP_UNARY_PLUS)			return PRECEDENCE_7;
		if (unop == OP_UNARY_DEREFERENCE)	return PRECEDENCE_7;	// dereference
		if (unop == OP_UNARY_ADDRESSOF)		return PRECEDENCE_7;	// address of
		if (unop == OP_UNARY_BITWISE_NOT)	return PRECEDENCE_7;	// not
		if (unop == OP_UNARY_MINUS)			return PRECEDENCE_7;	// minus
		if (unop == OP_UNARY_LOGIC_NOT)		return PRECEDENCE_7;	// not logic
		if (unop == OP_UNARY_CAST)			return PRECEDENCE_7;	// cast
	} else {
		return PRECEDENCE_7;
	}
	assert(0);
	return PRECEDENCE_0;
}

Precedence Parser::binary_op_precedence_level(Operator_Binary bo)
{
	Precedence p = PRECEDENCE_0;
	switch (bo) {
	case OP_BINARY_PLUS:			p = PRECEDENCE_4; break;
	case OP_BINARY_MINUS:			p = PRECEDENCE_4; break;
	case OP_BINARY_MULT:			p = PRECEDENCE_5; break;
	case OP_BINARY_DIV:				p = PRECEDENCE_5; break;
	case OP_BINARY_AND:				p = PRECEDENCE_3; break;
	case OP_BINARY_OR:				p = PRECEDENCE_3; break;
	case OP_BINARY_XOR:				p = PRECEDENCE_3; break;
	case OP_BINARY_MOD:				p = PRECEDENCE_5; break;
	case OP_BINARY_LOGIC_AND:		p = PRECEDENCE_1; break;
	case OP_BINARY_LOGIC_OR:		p = PRECEDENCE_1; break;
	case OP_BINARY_SHL:				p = PRECEDENCE_3; break;
	case OP_BINARY_SHR:				p = PRECEDENCE_3; break;
	case OP_BINARY_LT:				p = PRECEDENCE_2; break;
	case OP_BINARY_GT:				p = PRECEDENCE_2; break;
	case OP_BINARY_LE:				p = PRECEDENCE_2; break;
	case OP_BINARY_GE:				p = PRECEDENCE_2; break;
	case OP_BINARY_EQUAL:			p = PRECEDENCE_2; break;
	case OP_BINARY_NOT_EQUAL:		p = PRECEDENCE_2; break;
	case OP_BINARY_DOT:				p = PRECEDENCE_8; break;
	default: assert(0); break;
	}
	return p;
}