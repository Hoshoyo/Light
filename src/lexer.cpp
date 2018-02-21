#include "lexer.h"
#include <ho_system.h>
#include <stdio.h>

extern Hash_Table keywords = {};
extern String_Hash_Table identifiers = {};

// All the keywords in the language
Keyword keywords_info[] = {
	{ MAKE_STRING("int"),  TOKEN_INT,    TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("bool"), TOKEN_BOOL,   TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("void"), TOKEN_VOID,   TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("r32"),  TOKEN_REAL32, TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("r64"),  TOKEN_REAL64, TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("s64"),  TOKEN_SINT64, TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("s32"),  TOKEN_SINT32, TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("s16"),  TOKEN_SINT16, TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("s8"),   TOKEN_SINT8,  TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("u64"),  TOKEN_UINT64, TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("u32"),  TOKEN_UINT32, TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("u16"),  TOKEN_UINT16, TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },
	{ MAKE_STRING("u8"),   TOKEN_UINT8,  TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_PRIMITIVE_TYPE },

	{ MAKE_STRING("if"),		TOKEN_IF_STATEMENT,			TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("else"),		TOKEN_ELSE_STATEMENT,		TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("for"),		TOKEN_FOR_STATEMENT,		TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("while"),		TOKEN_WHILE_STATEMENT,		TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("do"),		TOKEN_DO_STATEMENT,			TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("switch"),	TOKEN_SWITCH_STATEMENT,		TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("break"),		TOKEN_BREAK_STATEMENT,		TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("continue"),  TOKEN_CONTINUE_STATEMENT,	TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("return"),	TOKEN_RETURN_STATEMENT,		TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("struct"),	TOKEN_STRUCT_WORD,			TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("union"),		TOKEN_UNION_WORD,			TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("new"),		TOKEN_NEW_WORD,				TOKEN_FLAG_RESERVED_WORD },
	{ MAKE_STRING("cast"),		TOKEN_CAST,					TOKEN_FLAG_RESERVED_WORD },

	{ MAKE_STRING("true"),  TOKEN_BOOL_LITERAL,		TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_LITERAL },
	{ MAKE_STRING("false"),  TOKEN_BOOL_LITERAL,	TOKEN_FLAG_RESERVED_WORD | TOKEN_FLAG_LITERAL },
};

s32 Lexer::report_lexer_error(char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	s32 num_written = fprintf(stderr, "Lexer Error: ");
	num_written += vfprintf(stderr, msg, args);
	va_end(args);
	return num_written;
}

static u64 keyword_hash(void* data) {
	Keyword* kw = (Keyword*)data;
	return fnv_1_hash(kw->word.data, kw->word.length);
}
static bool keyword_equal(void* k1, void* k2) {
	Keyword* kw1 = (Keyword*)kw1;
	Keyword* kw2 = (Keyword*)kw2;
	if (kw1->word.length != kw2->word.length) return false;
	return (memcmp(kw1->word.data, kw2->word.data, kw1->word.length) == 0);
}

void Lexer::init() {
	if (keywords.entries == 0) {
		// Max 1024 keywords
		hash_table_init(&keywords, 1024, keyword_hash, keyword_equal);
		for (s32 i = 0; i < ARRAY_COUNT(keywords_info); ++i) {
			hash_table_add(&keywords, &keywords_info[i], sizeof(Keyword));
		}
	}
	if (identifiers.entries == 0) {
		// @TODO(psv): check for necessity for more
		// @Hardcoded

		// 32 Mega bytes of hash for identifiers
		hash_table_init(&identifiers, 1024 * 1024 * 32);
	}
}

Lexer_Error Lexer::start(const char* filename)
{
	this->filename = { -1, strlen(filename), (char*)filename };
	file_size = ho_getfilesize(filename);
	HANDLE filehandle = ho_openfile(filename, OPEN_EXISTING);
	if (filehandle == INVALID_HANDLE_VALUE) {
		report_lexer_error("Could not open file %s.\n", filename);
		return LEXER_ERROR;
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
	token_array = (Token*)malloc(sizeof(Token) * (MAX(file_size, 64) + 4));
	char* at = filedata;

	current_col = 0;
	bool lexing = true;
	do {
		line_count += remove_white_space(&at, &current_col);
		lexing = read_token(&at);
		current_token++;
	} while (lexing);
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
	if (!at || !*at) return 0;
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

static Token_Type resolve_id_vs_keyword(char* text, int length, u32* flags, s64* entry_index)
{
	Keyword k;
	k.word = { -1, (u64)length, text };
	s64 index = hash_table_entry_exist(&keywords, &k);
	*entry_index = index;
	if (index != -1) {
		Keyword* entry = ((Keyword*)hash_table_get_entry(&keywords, index));
		*flags |= entry->flags;
		return entry->token_type;
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

	s64 keyword_index = -1;	// needs to be -1
	int skip = 0;
	int length = 1;
	Token_Type type = TOKEN_UNKNOWN;
	u32 flags = 0;

	char ch_1 = 0;
	if (at) {
		ch_1 = at[0];
	}
	char ch_2 = 0;
	if(ch_1){
		ch_2 = at[1];
	}

	token.filename = filename;

	switch (ch_1) {

	case '\0': type = TOKEN_END_OF_STREAM;			break;
	case '{': type = TOKEN_SYMBOL_OPEN_BRACE;		flags |= TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_POSTFIXED; break;
	case '}': type = TOKEN_SYMBOL_CLOSE_BRACE;		break;
	case '[': type = TOKEN_SYMBOL_OPEN_BRACKET;		break;
	case ']': type = TOKEN_SYMBOL_CLOSE_BRACKET;	break;
	case '(': type = TOKEN_SYMBOL_OPEN_PAREN;		break;
	case ')': type = TOKEN_SYMBOL_CLOSE_PAREN;		break;
	case ',': type = TOKEN_SYMBOL_COMMA;			break;
	case ';': type = TOKEN_SYMBOL_SEMICOLON;		break;
	case '@': type = TOKEN_SYMBOL_AT;				break;
	case '$': type = TOKEN_SYMBOL_DOLLAR;			break;
	case '~': type = TOKEN_SYMBOL_TILDE;			flags |= TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED;  break;
	case '?': type = TOKEN_SYMBOL_INTERROGATION;	break;
	case '#': type = TOKEN_SYMBOL_POUND;			break;

	case '%': {
		if (ch_2 == '=') {
			type = TOKEN_MOD_EQUAL;
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
			length = 2;
		} else {
			type = TOKEN_SYMBOL_MOD;
		}
		flags |= TOKEN_FLAG_BINARY_OPERATOR;
	}break;
	case '*': {
		if (ch_2 == '=') {
			type = TOKEN_TIMES_EQUAL;
			flags |= TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_ASSIGNMENT_OPERATOR;
			length = 2;
		} else {
			type = TOKEN_SYMBOL_TIMES;
			flags |= TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED;
		}
	}break;
	case '^': {
		if (ch_2 == '=') {
			type = TOKEN_XOR_EQUAL;
			flags |= TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_ASSIGNMENT_OPERATOR;
			length = 2;
		} else {
			type = TOKEN_SYMBOL_CARAT;
			flags |= TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_UNARY_OPERATOR | TOKEN_FLAG_UNARY_PREFIXED;
		}
	}break;
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
			flags |= TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_ASSIGNMENT_OPERATOR;
		} else {
			flags |= TOKEN_FLAG_BINARY_OPERATOR;
			type = TOKEN_SYMBOL_DIV;
		}
	} break;

	case '+': {
		if (ch_2 == '=') {
			type = TOKEN_PLUS_EQUAL;
			flags |= TOKEN_FLAG_BINARY_OPERATOR | TOKEN_FLAG_ASSIGNMENT_OPERATOR;
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
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR | TOKEN_FLAG_BINARY_OPERATOR;
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
			if (at[2] && at[2] == '=') {
				type = TOKEN_SHL_EQUAL;
				length = 3;
				flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
			} else {
				type = TOKEN_BITSHIFT_LEFT;
				length = 2;
			}
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
			if (at[2] && at[2] == '=') {
				type = TOKEN_SHR_EQUAL;
				length = 3;
				flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
			} else {
				type = TOKEN_BITSHIFT_RIGHT;
				length = 2;
			}
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
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR | TOKEN_FLAG_BINARY_OPERATOR;
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
			flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR | TOKEN_FLAG_BINARY_OPERATOR;
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
			type = resolve_id_vs_keyword(at, length, &flags, &keyword_index);
			if (type != TOKEN_IDENTIFIER) {
				flags |= TOKEN_FLAG_RESERVED_WORD;
			}
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

	token.value = { -1, (size_t)length, at };
	token.type = type;
	token.line = line_count;
	token.column = current_col;
	token.flags = flags;
	token.offset_in_file = at - filedata;

	// This makes all the identifiers and keywords resolve to the same pointer
	if (keyword_index != -1) {
		if (flags & TOKEN_FLAG_RESERVED_WORD) {
			token.value = ((Keyword*)hash_table_get_entry(&keywords, keyword_index))->word;
		} else {
			token.value = hash_table_get_string_entry(&identifiers, keyword_index);
		}
	}

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
	case TOKEN_SYMBOL_NOT:				return  ("!"); break;
	case TOKEN_SYMBOL_POUND:			return  ("#"); break;
	case TOKEN_SYMBOL_DOLLAR:			return  ("$"); break;
	case TOKEN_SYMBOL_MOD:				return  ("%"); break;
	case TOKEN_SYMBOL_AND:				return  ("&"); break;
	case TOKEN_SYMBOL_OPEN_PAREN:		return  ("("); break;
	case TOKEN_SYMBOL_CLOSE_PAREN:		return  (")"); break;
	case TOKEN_SYMBOL_TIMES:			return  ("*"); break;
	case TOKEN_SYMBOL_PLUS:				return  ("+"); break;
	case TOKEN_SYMBOL_COMMA:			return  (","); break;
	case TOKEN_SYMBOL_MINUS:			return  ("-"); break;
	case TOKEN_SYMBOL_DOT:				return  ("."); break;
	case TOKEN_SYMBOL_DIV:				return  ("/"); break;
	case TOKEN_SYMBOL_COLON:			return  (":"); break;
	case TOKEN_SYMBOL_SEMICOLON:		return  (";"); break;
	case TOKEN_SYMBOL_LESS:				return  ("<"); break;
	case TOKEN_SYMBOL_EQUAL:			return  ("="); break;
	case TOKEN_SYMBOL_GREATER:			return  (">"); break;
	case TOKEN_SYMBOL_INTERROGATION:	return  ("?"); break;
	case TOKEN_SYMBOL_AT:				return  ("@"); break;
	case TOKEN_SYMBOL_OPEN_BRACKET:		return  ("["); break;
	case TOKEN_SYMBOL_CLOSE_BRACKET:	return  ("]"); break;
	case TOKEN_SYMBOL_CARAT:			return  ("^"); break;
	case TOKEN_SYMBOL_BACK_TICK:		return  ("`"); break;
	case TOKEN_SYMBOL_OPEN_BRACE:		return  ("{"); break;
	case TOKEN_SYMBOL_PIPE:				return  ("|"); break;
	case TOKEN_SYMBOL_CLOSE_BRACE:		return  ("}"); break;
	case TOKEN_SYMBOL_TILDE:			return  ("~"); break;

	case TOKEN_ARROW:				return ("->"); break;
	case TOKEN_EQUAL_COMPARISON:	return ("=="); break;
	case TOKEN_LESS_EQUAL:			return ("<="); break;
	case TOKEN_GREATER_EQUAL:		return (">="); break;
	case TOKEN_NOT_EQUAL:			return ("!="); break;
	case TOKEN_LOGIC_OR:			return ("||"); break;
	case TOKEN_LOGIC_AND:			return ("&&"); break;
	case TOKEN_COLON_COLON:			return ("::"); break;
	case TOKEN_BITSHIFT_LEFT:		return ("<<"); break;
	case TOKEN_BITSHIFT_RIGHT:		return (">>"); break;

	case TOKEN_PLUS_EQUAL:	return ("+="); break;
	case TOKEN_MINUS_EQUAL: return ("-="); break;
	case TOKEN_TIMES_EQUAL: return ("*="); break;
	case TOKEN_DIV_EQUAL:	return ("/="); break;
	case TOKEN_MOD_EQUAL:	return ("%="); break;
	case TOKEN_AND_EQUAL:	return ("&="); break;
	case TOKEN_OR_EQUAL:	return ("|="); break;
	case TOKEN_XOR_EQUAL:	return ("^="); break;
	case TOKEN_SHL_EQUAL:	return ("<<="); break;
	case TOKEN_SHR_EQUAL:	return (">>="); break;

	case TOKEN_PLUS_PLUS:	return  ("++"); break;
	case TOKEN_MINUS_MINUS: return  ("--"); break;
	case TOKEN_CAST:		return  ("cast"); break;

	case TOKEN_DOUBLE_DOT: return (".."); break;

	case TOKEN_INT:			return ("int"); break;
	case TOKEN_SINT64:		return ("s64"); break;
	case TOKEN_SINT32:		return ("s32"); break;
	case TOKEN_SINT16:		return ("s16"); break;
	case TOKEN_SINT8:		return ("s8"); break;
	case TOKEN_UINT64:		return ("u64"); break;
	case TOKEN_UINT32:		return ("u32"); break;
	case TOKEN_UINT16:		return ("u16"); break;
	case TOKEN_UINT8:		return ("u8"); break;
	case TOKEN_REAL32:		return ("r32"); break;
	case TOKEN_REAL64:		return ("r64"); break;
	case TOKEN_BOOL:		return ("bool"); break;
	case TOKEN_VOID:		return ("void"); break;

	case TOKEN_IF_STATEMENT:		return ("if"); break;
	case TOKEN_ELSE_STATEMENT:		return ("else"); break;
	case TOKEN_WHILE_STATEMENT:		return ("while"); break;
	case TOKEN_FOR_STATEMENT:		return ("for"); break;
	case TOKEN_DO_STATEMENT:		return ("do"); break;
	case TOKEN_SWITCH_STATEMENT:	return ("switch"); break;
	case TOKEN_BREAK_STATEMENT:		return ("break"); break;
	case TOKEN_CONTINUE_STATEMENT:	return ("continue"); break;
	case TOKEN_RETURN_STATEMENT:	return ("return"); break;

	case TOKEN_INTERNAL_WORD:	return ("internal"); break;
	case TOKEN_STRUCT_WORD:		return ("struct"); break;
	case TOKEN_UNION_WORD:		return ("union"); break;
	case TOKEN_NEW_WORD:		return ("new"); break;
	}
}

void internalize_identifier(string* str) {
	u64 str_hash = fnv_1_hash(str->data, str->length);
	s64 index = hash_table_entry_exist(&identifiers, *str);
	if (index == -1) {
		hash_table_add(&identifiers, *str);
	} else {
		str->data = hash_table_get_string_entry(&identifiers, index).data;
		assert(str->length == identifiers.entries[index].data.length);
	}
}