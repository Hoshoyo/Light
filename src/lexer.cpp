#include "lexer.h"
#include <ho_system.h>
#include <stdio.h>

s32 Lexer::start(const char* filename)
{
	make_immutable_string(this->filename, filename);
	file_size = ho_getfilesize(filename);
	HANDLE filehandle = ho_openfile(filename, OPEN_EXISTING);
	if (filehandle == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Error: could not open file %s.\nexiting...\n", filename);
		return LEXER_FAILURE;
	}
	this->file_size = file_size;

	void* file_memory = malloc(file_size + 1);
	*((char*)file_memory + file_size) = 0;
	filedata = (char*)ho_readentirefile(filehandle, file_size, file_memory);
	ho_closefile(filehandle);

	lex_file();
	current_token = 0;
	return LEXER_SUCCESS;
}

static int remove_comments(char** start, s64* col_ptr);
static int remove_white_space(char** start, s64* col_ptr);

void Lexer::lex_file()
{
	token_array = (Token*)malloc(sizeof(Token) * file_size);

	char* at = filedata;

	current_col = 0;
	bool lexing = true;
	do {
		line_count += remove_white_space(&at, &current_col);
		lexing = read_token(&at);
		current_token++;
	} while (lexing);

	int x = 0;
}

// return lines that advanced
static int remove_comments(char** start, s64* col_ptr)
{
	s64 column_ptr = *col_ptr;
	char* at = *start;
	int lines = 0;
	if (at[0] && at[0] == '/' && at[1] == '/') {
		at += 2;
		while (*at && !(*at == '\n' || *at == '\r')) {
			at++;
		}
		at++;
		column_ptr = 0;
	}
	else if (at[0] && at[0] == '/' && at[1] == '*') {
		at += 2;
		column_ptr += 2;
		int keep_going = 1;
		while (*at && keep_going) {
			if (*at && at[0] == '/' && at[1] == '*')
				keep_going++;
			if (at[0] == '*' && at[1] == '/')
				keep_going--;
			if (*at == '\n') {
				lines++;
				column_ptr = -1;
			}
			at++;
			column_ptr++;
		}
		at++;
		column_ptr++;
	}
	*start = at;
	*col_ptr = column_ptr;
	return lines;
}

static int remove_white_space(char** start, s64* col_ptr)
{
	s64 column_ptr = *col_ptr;
	char* at = *start;
	int line_count = 0;
	int advance = 0;

	line_count += remove_comments(&at, &column_ptr);
	while (is_white_space(*at)) {
		if (*at == '\n') {
			line_count++;
			column_ptr = -1;
		}
		at++;
		column_ptr++;
		line_count += remove_comments(&at, &column_ptr);
	}
	*start = at;
	*col_ptr = column_ptr;
	return line_count;
}


#define CHECK_STR_EQUAL(S) str_equal(text, length, S, sizeof(S) - 1)
// TODO: make this into a hash table
static Token_Type resolve_id_vs_keyword(char* text, int length, u32* flags)
{
	// primitive types
	Token_Type prim = TOKEN_UNKNOWN;
	if (CHECK_STR_EQUAL("int"))	prim = TOKEN_INT;
	else if (CHECK_STR_EQUAL("float")) prim = TOKEN_FLOAT;
	else if (CHECK_STR_EQUAL("bool")) prim = TOKEN_BOOL;
	else if (CHECK_STR_EQUAL("char")) prim = TOKEN_CHAR;
	else if (CHECK_STR_EQUAL("void")) prim = TOKEN_VOID;
	else if (CHECK_STR_EQUAL("double")) prim = TOKEN_DOUBLE;
	else if (CHECK_STR_EQUAL("r32")) prim = TOKEN_REAL32;
	else if (CHECK_STR_EQUAL("r64")) prim = TOKEN_REAL64;
	else if (CHECK_STR_EQUAL("s64")) prim = TOKEN_SINT64;
	else if (CHECK_STR_EQUAL("s32")) prim = TOKEN_SINT32;
	else if (CHECK_STR_EQUAL("s16")) prim = TOKEN_SINT16;
	else if (CHECK_STR_EQUAL("s8")) prim = TOKEN_SINT8;
	else if (CHECK_STR_EQUAL("u64")) prim = TOKEN_UINT64;
	else if (CHECK_STR_EQUAL("u32")) prim = TOKEN_UINT32;
	else if (CHECK_STR_EQUAL("u16")) prim = TOKEN_UINT16;
	else if (CHECK_STR_EQUAL("u8")) prim = TOKEN_UINT8;

	if (prim != TOKEN_UNKNOWN) {
		*flags |= TOKEN_FLAG_PRIMITIVE_TYPE;
		return prim;
	}

	// statements/commands
	if (CHECK_STR_EQUAL("if")) return TOKEN_IF_STATEMENT;
	if (CHECK_STR_EQUAL("else")) return TOKEN_ELSE_STATEMENT;
	if (CHECK_STR_EQUAL("for")) return TOKEN_FOR_STATEMENT;
	if (CHECK_STR_EQUAL("while")) return TOKEN_WHILE_STATEMENT;
	if (CHECK_STR_EQUAL("do")) return TOKEN_DO_STATEMENT;
	if (CHECK_STR_EQUAL("switch")) return TOKEN_SWITCH_STATEMENT;
	if (CHECK_STR_EQUAL("case")) return TOKEN_CASE_STATEMENT;
	if (CHECK_STR_EQUAL("goto")) return TOKEN_GOTO_STATEMENT;
	if (CHECK_STR_EQUAL("break")) return TOKEN_BREAK_STATEMENT;
	if (CHECK_STR_EQUAL("continue")) return TOKEN_CONTINUE_STATEMENT;
	if (CHECK_STR_EQUAL("return")) return TOKEN_RETURN_STATEMENT;

	if (CHECK_STR_EQUAL("internal")) return TOKEN_INTERNAL_WORD;
	if (CHECK_STR_EQUAL("struct")) return TOKEN_STRUCT_WORD;
	if (CHECK_STR_EQUAL("union")) return TOKEN_UNION_WORD;
	if (CHECK_STR_EQUAL("new")) return TOKEN_NEW_WORD;
	if (CHECK_STR_EQUAL("cast")) return TOKEN_CAST;

	if (CHECK_STR_EQUAL("true") || CHECK_STR_EQUAL("false")) {
		*flags |= TOKEN_FLAG_LITERAL;
		return TOKEN_BOOL_LITERAL;
	}

	return TOKEN_IDENTIFIER;
}

char* Lexer::get_first_char_in_current_line()
{
	s32 current = current_token - 1;
	if (current < 0) current = 0;
	Token* res = 0;
	char* at = token_array[current].value.data;
	while (at > filedata) {
		if (*at == '\n') {
			at++;
			break;
		}
		at--;
	}
	return at;
}

char* Lexer::get_first_char_in_line(s64 line)
{
	assert(line <= line_count);
	s64 count = 0;
	for (int i = 0; i < token_count; ++i) {
		if (token_array[i].line == line) {
			break;
		}
		++count;
	}
	return token_array[count].value.data;
}

bool Lexer::read_token(char** begin)
{
	Token& token = this->token_array[current_token];

	char* at = *begin;

	int skip = 0;
	int length = 1;
	Token_Type type = TOKEN_UNKNOWN;
	u32 flags = 0;

	char ch_1 = at[0];
	char ch_2 = at[1];

	token.filename = filename;

	switch (ch_1) {

	case '\0': {
		type = TOKEN_END_OF_STREAM;
	} break;

	case '{': type = TOKEN_SYMBOL_OPEN_BRACE;		flags |= TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_POSTFIXED; break;
	case '}': type = TOKEN_SYMBOL_CLOSE_BRACE;		break;
	case '[': type = TOKEN_SYMBOL_OPEN_BRACKET;		break;
	case ']': type = TOKEN_SYMBOL_CLOSE_BRACKET;	break;
	case '(': type = TOKEN_SYMBOL_OPEN_PAREN;		break;
	case ')': type = TOKEN_SYMBOL_CLOSE_PAREN;		break;
	case ',': type = TOKEN_SYMBOL_COMMA;			break;
	case ';': type = TOKEN_SYMBOL_SEMICOLON;		break;
	case '*': type = TOKEN_SYMBOL_TIMES;			flags |= TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED; break;
	case '%': type = TOKEN_SYMBOL_MOD;				flags |= TOKEN_FLAG_BINARY_OPERATOR; break;
	case '@': type = TOKEN_SYMBOL_AT;				break;
	case '$': type = TOKEN_SYMBOL_DOLLAR;			break;
	case '^': type = TOKEN_SYMBOL_CARAT;			flags |= TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED; break;
	case '~': type = TOKEN_SYMBOL_TILDE;			flags |= TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED;  break;
	case '?': type = TOKEN_SYMBOL_INTERROGATION;	break;

	case '.': {
		if (ch_2 == '.') {
			type = TOKEN_DOUBLE_DOT;
			length = 2;
		}
		else {
			type = TOKEN_SYMBOL_DOT;
			flags |= TOKEN_FLAG_BINARY_OPERATOR;
		}
	}break;

	case '/': {
		if (ch_2 == '=') {
			type = TOKEN_DIV_EQUAL;
			length = 2;
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
		}
		else {
			flags |= TOKEN_FLAG_BINARY_OPERATOR;
			type = TOKEN_SYMBOL_DIV;
		}
	} break;

	case '+': {
		if (ch_2 == '=') {
			type = TOKEN_PLUS_EQUAL;
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
			length = 2;
		}
		else if (ch_2 == '+') {
			type = TOKEN_PLUS_PLUS;
			flags |= TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED | TOKEN_FLAG_UNARY_POSTFIXED;
			length = 2;
		}
		else {
			flags |= TOKEN_FLAG_BINARY_OPERATOR;
			type = TOKEN_SYMBOL_PLUS;
		}
	} break;

	case '-': {
		if (ch_2 == '>') {
			type = TOKEN_ARROW;
			length = 2;
		}
		else if (ch_2 == '=') {
			type = TOKEN_MINUS_EQUAL;
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
			length = 2;
		}
		else if (ch_2 == '-') {
			type = TOKEN_MINUS_MINUS;
			flags |= TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED | TOKEN_FLAG_UNARY_POSTFIXED;
			length = 2;
		}
		else {
			type = TOKEN_SYMBOL_MINUS;
			flags |= TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED;
		}
	}break;

	case '=': {
		if (ch_2 == '=') {
			type = TOKEN_EQUAL_COMPARISON;
			length = 2;
			flags |= TOKEN_FLAG_BINARY_OPERATOR;
		}
		else {
			type = TOKEN_SYMBOL_EQUAL;
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
		}
	}break;
	case '<': {
		if (ch_2 == '=') {
			type = TOKEN_LESS_EQUAL;
			length = 2;
		}
		else if (ch_2 == '<') {
			type = TOKEN_BITSHIFT_LEFT;
			length = 2;
		}
		else type = TOKEN_SYMBOL_LESS;
		flags |= TOKEN_FLAG_BINARY_OPERATOR;
	}break;
	case '>': {
		if (ch_2 == '=') {
			type = TOKEN_GREATER_EQUAL;
			length = 2;
		}
		else if (ch_2 == '>') {
			type = TOKEN_BITSHIFT_RIGHT;
			length = 2;
		}
		else type = TOKEN_SYMBOL_GREATER;
		flags |= TOKEN_FLAG_BINARY_OPERATOR;
	}break;

	case '!': {
		if (ch_2 == '=') {
			type = TOKEN_NOT_EQUAL;
			length = 2;
			flags |= TOKEN_FLAG_BINARY_OPERATOR;
		}
		else {
			type = TOKEN_SYMBOL_NOT;
			flags |= TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED;
		}
	}break;

	case '|': {
		if (ch_2 == '|') {
			type = TOKEN_LOGIC_OR;
			length = 2;
			flags |= TOKEN_FLAG_BINARY_OPERATOR;
		}
		else if (ch_2 == '=') {
			type = TOKEN_OR_EQUAL;
			length = 2;
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
		}
		else {
			type = TOKEN_SYMBOL_PIPE;
			flags |= TOKEN_FLAG_BINARY_OPERATOR;
		}
	}break;

	case '&': {
		if (ch_2 == '&') {
			type = TOKEN_LOGIC_AND;
			length = 2;
			flags |= TOKEN_FLAG_BINARY_OPERATOR;
		}
		else if (ch_2 == '=') {
			type = TOKEN_AND_EQUAL;
			length = 2;
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
		}
		else {
			type = TOKEN_SYMBOL_AND;
			flags |= TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED;
		}
	}break;
	case ':': {
		if (ch_2 == ':') {
			type = TOKEN_COLON_COLON;
			length = 2;
		}
		else type = TOKEN_SYMBOL_COLON;
	}break;

	case '"': {
		flags |= TOKEN_FLAG_LITERAL;
		type = TOKEN_STRING_LITERAL;
		int i = 0;
		at++;
		for (; at[i] != '"'; ++i) {
			if (at[i] == 0) {
				type = TOKEN_UNKNOWN;
				break;
			}
			length++;
		}
		length--;
		skip++;
	}break;

	case '\'': {
		flags |= TOKEN_FLAG_LITERAL;
		type = TOKEN_CHAR_LITERAL;
		int i = 0;
		at++;
		length = 0;
		for (; at[i] != '\''; ++i) {
			if (at[i] == 0) {
				type = TOKEN_UNKNOWN;
				break;
			}
			length++;
		}
		if (i == 0) type = TOKEN_UNKNOWN;
		skip++;
	}break;

	default: {
		if (is_letter(ch_1) || ch_1 == '_') {
			length = 0;
			for (int i = 0; is_letter(at[i]) || is_number(at[i]) || at[i] == '_'; ++i)
				length++;
			type = resolve_id_vs_keyword(at, length, &flags);
			if (type != TOKEN_IDENTIFIER) flags |= TOKEN_FLAG_RESERVED_WORD;
		}
		else if (is_number(ch_1)) {
			bool floating_point = false;
			int i = 0;
			length = 0;
			do {
				++i;
				length++;
				if (at[i] == '.' && !floating_point) {
					floating_point = true;
					++i;
					length++;
				}
			} while (at[i] && (is_number(at[i])));

			flags |= TOKEN_FLAG_NUMERIC_LITERAL | TOKEN_FLAG_LITERAL;
			type = (floating_point) ? TOKEN_FLOAT_LITERAL : TOKEN_INT_LITERAL;
		}
		else {
			type = TOKEN_UNKNOWN;
		}
	}break;
	}

	make_immutable_string(token.value, at, length);
	token.type = type;
	token.line = line_count;
	token.column = current_col;
	token.flags = flags;

	at += length + skip;
	current_col += skip + length;
	token_count++;

	*begin = at;

	if (type == TOKEN_END_OF_STREAM) return false;
	return true;
}

void Lexer::rewind()
{
	if (current_token == 0) return;
	current_token--;
}
Token* Lexer::last_token()
{
	if (current_token > 0)
		return &token_array[current_token - 1];
	return &token_array[0];
}
Token* Lexer::peek_token()
{
	return &token_array[current_token];
}
Token* Lexer::peek_token(int advance)
{
	if (advance + current_token >= token_count)
		return &token_array[token_count - 1];
	if (advance + current_token < 0)
		return 0;
	return &token_array[current_token + advance];
}
Token_Type Lexer::peek_token_type()
{
	return token_array[current_token].type;
}
Token_Type Lexer::peek_token_type(int advance)
{
	if (advance + current_token >= token_count)
		return token_array[token_count - 1].type;
	return token_array[current_token + advance].type;
}

Token* Lexer::eat_token(int advance) {
	if (current_token + advance >= token_count)
		return &token_array[token_count];
	current_token += advance;
	return &token_array[current_token - 1];
}

Token* Lexer::eat_token()
{
	if (current_token >= token_count)
		return &token_array[token_count];
	current_token++;
	return &token_array[current_token - 1];
}

static char* make_null_term_string(char* s)
{
	s32 len = strlen(s);
	char* ptr = (char*)malloc(len + 1);
	memcpy(ptr, s, len);
	ptr[len] = 0;
	return ptr;
}

char* Lexer::get_token_string(Token_Type t)
{
	switch (t) {
	case TOKEN_SYMBOL_NOT:				return  make_null_term_string("!"); break;
	case TOKEN_SYMBOL_POUND:			return  make_null_term_string("#"); break;
	case TOKEN_SYMBOL_DOLLAR:			return  make_null_term_string("$"); break;
	case TOKEN_SYMBOL_MOD:				return  make_null_term_string("%"); break;
	case TOKEN_SYMBOL_AND:				return  make_null_term_string("&"); break;
	case TOKEN_SYMBOL_OPEN_PAREN:		return  make_null_term_string("("); break;
	case TOKEN_SYMBOL_CLOSE_PAREN:		return  make_null_term_string(")"); break;
	case TOKEN_SYMBOL_TIMES:			return  make_null_term_string("*"); break;
	case TOKEN_SYMBOL_PLUS:				return  make_null_term_string("+"); break;
	case TOKEN_SYMBOL_COMMA:			return  make_null_term_string(","); break;
	case TOKEN_SYMBOL_MINUS:			return  make_null_term_string("-"); break;
	case TOKEN_SYMBOL_DOT:				return  make_null_term_string("."); break;
	case TOKEN_SYMBOL_DIV:				return  make_null_term_string("/"); break;
	case TOKEN_SYMBOL_COLON:			return  make_null_term_string(":"); break;
	case TOKEN_SYMBOL_SEMICOLON:		return  make_null_term_string(";"); break;
	case TOKEN_SYMBOL_LESS:				return  make_null_term_string("<"); break;
	case TOKEN_SYMBOL_EQUAL:			return  make_null_term_string("="); break;
	case TOKEN_SYMBOL_GREATER:			return  make_null_term_string(">"); break;
	case TOKEN_SYMBOL_INTERROGATION:	return  make_null_term_string("?"); break;
	case TOKEN_SYMBOL_AT:				return  make_null_term_string("@"); break;
	case TOKEN_SYMBOL_OPEN_BRACKET:		return  make_null_term_string("["); break;
	case TOKEN_SYMBOL_CLOSE_BRACKET:	return  make_null_term_string("]"); break;
	case TOKEN_SYMBOL_CARAT:			return  make_null_term_string("^"); break;
	case TOKEN_SYMBOL_BACK_TICK:		return  make_null_term_string("`"); break;
	case TOKEN_SYMBOL_OPEN_BRACE:		return  make_null_term_string("{"); break;
	case TOKEN_SYMBOL_PIPE:				return  make_null_term_string("|"); break;
	case TOKEN_SYMBOL_CLOSE_BRACE:		return  make_null_term_string("}"); break;
	case TOKEN_SYMBOL_TILDE:			return  make_null_term_string("~"); break;

	case TOKEN_ARROW:				return  make_null_term_string("->"); break;
	case TOKEN_EQUAL_COMPARISON:	return  make_null_term_string("=="); break;
	case TOKEN_LESS_EQUAL:			return  make_null_term_string("<="); break;
	case TOKEN_GREATER_EQUAL:		return  make_null_term_string(">="); break;
	case TOKEN_NOT_EQUAL:			return  make_null_term_string("!="); break;
	case TOKEN_LOGIC_OR:			return  make_null_term_string("||"); break;
	case TOKEN_LOGIC_AND:			return  make_null_term_string("&&"); break;
	case TOKEN_COLON_COLON:			return  make_null_term_string("::"); break;
	case TOKEN_BITSHIFT_LEFT:		return  make_null_term_string("<<"); break;
	case TOKEN_BITSHIFT_RIGHT:		return  make_null_term_string(">>"); break;

	case TOKEN_PLUS_EQUAL:	return  make_null_term_string("+="); break;
	case TOKEN_MINUS_EQUAL: return  make_null_term_string("-="); break;
	case TOKEN_TIMES_EQUAL: return  make_null_term_string("*="); break;
	case TOKEN_DIV_EQUAL:	return  make_null_term_string("/="); break;
	case TOKEN_MOD_EQUAL:	return  make_null_term_string("%="); break;
	case TOKEN_AND_EQUAL:	return  make_null_term_string("&="); break;
	case TOKEN_OR_EQUAL:	return  make_null_term_string("|="); break;
	case TOKEN_XOR_EQUAL:	return  make_null_term_string("^="); break;
	case TOKEN_SHL_EQUAL:	return  make_null_term_string("<<="); break;
	case TOKEN_SHR_EQUAL:	return  make_null_term_string(">>="); break;

	case TOKEN_PLUS_PLUS:	return  make_null_term_string("++"); break;
	case TOKEN_MINUS_MINUS: return  make_null_term_string("--"); break;
	case TOKEN_CAST:		return  make_null_term_string("cast"); break;

	case TOKEN_DOUBLE_DOT: return  make_null_term_string(".."); break;

	case TOKEN_INT:			return  make_null_term_string("int"); break;
	case TOKEN_SINT64:		return  make_null_term_string("s64"); break;
	case TOKEN_SINT32:		return  make_null_term_string("s32"); break;
	case TOKEN_SINT16:		return  make_null_term_string("s16"); break;
	case TOKEN_SINT8:		return  make_null_term_string("s8"); break;
	case TOKEN_UINT64:		return  make_null_term_string("u64"); break;
	case TOKEN_UINT32:		return  make_null_term_string("u32"); break;
	case TOKEN_UINT16:		return  make_null_term_string("u16"); break;
	case TOKEN_UINT8:		return  make_null_term_string("u8"); break;
	case TOKEN_REAL32:		return  make_null_term_string("r32"); break;
	case TOKEN_REAL64:		return  make_null_term_string("r64"); break;
	case TOKEN_CHAR:		return  make_null_term_string("char"); break;		// @remove?
	case TOKEN_FLOAT:		return  make_null_term_string("float"); break;
	case TOKEN_DOUBLE:		return  make_null_term_string("double"); break;		// @remove?
	case TOKEN_BOOL:		return  make_null_term_string("bool"); break;
	case TOKEN_VOID:		return  make_null_term_string("void"); break;

	case TOKEN_IF_STATEMENT:		return  make_null_term_string("if"); break;
	case TOKEN_ELSE_STATEMENT:		return  make_null_term_string("else"); break;
	case TOKEN_WHILE_STATEMENT:		return  make_null_term_string("while"); break;
	case TOKEN_FOR_STATEMENT:		return  make_null_term_string("for"); break;
	case TOKEN_DO_STATEMENT:		return  make_null_term_string("do"); break;
	case TOKEN_SWITCH_STATEMENT:	return  make_null_term_string("switch"); break;
	case TOKEN_CASE_STATEMENT:		return  make_null_term_string("case"); break;
	case TOKEN_GOTO_STATEMENT:		return  make_null_term_string("goto"); break;
	case TOKEN_BREAK_STATEMENT:		return  make_null_term_string("break"); break;
	case TOKEN_CONTINUE_STATEMENT:	return  make_null_term_string("continue"); break;
	case TOKEN_RETURN_STATEMENT:	return  make_null_term_string("return"); break;

	case TOKEN_INTERNAL_WORD:	return  make_null_term_string("internal"); break;
	case TOKEN_STRUCT_WORD:		return  make_null_term_string("struct"); break;
	case TOKEN_UNION_WORD:		return  make_null_term_string("union"); break;
	case TOKEN_NEW_WORD:		return  make_null_term_string("new"); break;
	}
}
