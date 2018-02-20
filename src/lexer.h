#pragma once
#include "util.h"
#include "hash_table.h"

enum Token_Type
{
	TOKEN_END_OF_STREAM = 0,
	TOKEN_UNKNOWN = 1,

	// Symbols
	TOKEN_SYMBOL_NOT = '!',		// 33
	TOKEN_SYMBOL_POUND = '#',		// 35
	TOKEN_SYMBOL_DOLLAR = '$',		// 36
	TOKEN_SYMBOL_MOD = '%',		// 37
	TOKEN_SYMBOL_AND = '&',		// 38
	TOKEN_SYMBOL_OPEN_PAREN = '(',		// 40
	TOKEN_SYMBOL_CLOSE_PAREN = ')',		// 41
	TOKEN_SYMBOL_TIMES = '*',		// 42
	TOKEN_SYMBOL_PLUS = '+',		// 43
	TOKEN_SYMBOL_COMMA = ',',		// 44
	TOKEN_SYMBOL_MINUS = '-',		// 45
	TOKEN_SYMBOL_DOT = '.',		// 46
	TOKEN_SYMBOL_DIV = '/',		// 47
	TOKEN_SYMBOL_COLON = ':',		// 58
	TOKEN_SYMBOL_SEMICOLON = ';',		// 59
	TOKEN_SYMBOL_LESS = '<',		// 60
	TOKEN_SYMBOL_EQUAL = '=',		// 61
	TOKEN_SYMBOL_GREATER = '>',		// 62
	TOKEN_SYMBOL_INTERROGATION = '?',		// 63
	TOKEN_SYMBOL_AT = '@',		// 64
	TOKEN_SYMBOL_OPEN_BRACE = '{',		// 91
	TOKEN_SYMBOL_CLOSE_BRACE = '}',		// 93
	TOKEN_SYMBOL_CARAT = '^',		// 94
	TOKEN_SYMBOL_BACK_TICK = '`',		// 96
	TOKEN_SYMBOL_OPEN_BRACKET = '[',		// 123
	TOKEN_SYMBOL_PIPE = '|',		// 124
	TOKEN_SYMBOL_CLOSE_BRACKET = ']',		// 125
	TOKEN_SYMBOL_TILDE = '~',		// 126

	// Literal tokens
	TOKEN_INT_LITERAL = 127,
	TOKEN_FLOAT_LITERAL = 128,
	TOKEN_CHAR_LITERAL = 129,
	TOKEN_STRING_LITERAL = 130,
	TOKEN_BOOL_LITERAL = 131,
	TOKEN_IDENTIFIER = 132,

	// Operation tokens
	TOKEN_SYMBOL = 133,
	TOKEN_ARROW = 134,
	TOKEN_EQUAL_COMPARISON = 135,
	TOKEN_LESS_EQUAL = 136,
	TOKEN_GREATER_EQUAL = 137,
	TOKEN_NOT_EQUAL = 138,
	TOKEN_LOGIC_OR = 139,
	TOKEN_LOGIC_AND = 140,
	TOKEN_COLON_COLON = 141,
	TOKEN_BITSHIFT_LEFT = 142,
	TOKEN_BITSHIFT_RIGHT = 143,

	TOKEN_PLUS_EQUAL = 144,
	TOKEN_MINUS_EQUAL = 145,
	TOKEN_TIMES_EQUAL = 146,
	TOKEN_DIV_EQUAL = 147,
	TOKEN_MOD_EQUAL = 148,
	TOKEN_AND_EQUAL = 149,
	TOKEN_OR_EQUAL = 150,
	TOKEN_XOR_EQUAL = 151,
	TOKEN_SHL_EQUAL = 152,
	TOKEN_SHR_EQUAL = 153,

	TOKEN_PLUS_PLUS = 172,
	TOKEN_MINUS_MINUS = 173,
	TOKEN_CAST = 174,

	TOKEN_DOUBLE_DOT = 185,

	// Type tokens
	TOKEN_INT = 186,
	TOKEN_SINT64 = 187,
	TOKEN_SINT32 = 188,
	TOKEN_SINT16 = 189,
	TOKEN_SINT8 = 190,
	TOKEN_UINT64 = 191,
	TOKEN_UINT32 = 192,
	TOKEN_UINT16 = 193,
	TOKEN_UINT8 = 194,
	TOKEN_REAL32 = 195,
	TOKEN_REAL64 = 196,
	//TOKEN_CHAR = 197,
	//TOKEN_FLOAT = 198,
	//TOKEN_DOUBLE = 199,
	TOKEN_BOOL = 200,
	TOKEN_VOID = 201,

	// Statements tokens
	TOKEN_IF_STATEMENT = 202,
	TOKEN_ELSE_STATEMENT = 203,
	TOKEN_WHILE_STATEMENT = 204,
	TOKEN_FOR_STATEMENT = 205,
	TOKEN_DO_STATEMENT = 206,
	TOKEN_SWITCH_STATEMENT = 207,
	//TOKEN_CASE_STATEMENT = 208,
	//TOKEN_GOTO_STATEMENT = 209,
	TOKEN_BREAK_STATEMENT = 210,
	TOKEN_CONTINUE_STATEMENT = 211,
	TOKEN_RETURN_STATEMENT = 212,

	// Reserved words
	TOKEN_INTERNAL_WORD = 233,
	TOKEN_STRUCT_WORD = 234,
	TOKEN_UNION_WORD = 235,
	TOKEN_NEW_WORD = 236,
};

const u32 TOKEN_FLAG_RESERVED_WORD = FLAG(0);
const u32 TOKEN_FLAG_NUMERIC_LITERAL = FLAG(1);
const u32 TOKEN_FLAG_LITERAL = FLAG(2);
const u32 TOKEN_FLAG_BINARY_OPERATOR = FLAG(3);
const u32 TOKEN_FLAG_UNARY_OPERATOR = FLAG(4);
const u32 TOKEN_FLAG_UNARY_PREFIXED = FLAG(5);
const u32 TOKEN_FLAG_UNARY_POSTFIXED = FLAG(6);
const u32 TOKEN_FLAG_PRIMITIVE_TYPE = FLAG(7);
const u32 TOKEN_FLAG_ASSIGNMENT_OPERATOR = FLAG(8);

enum Lexer_Error {
	LEXER_SUCCESS = 0,
	LEXER_ERROR = 1,
};

struct Token
{
	Token_Type type;

	s32 line;
	s32 column;
	s32 offset_in_file;

	u32 flags = 0;

	string filename;
	string value;

	union literal_value{
		u64 uint64_v;
		s64 sint64_v;
		r32 float32_v;
		r64 float64_v;
	};
};

struct Lexer
{
	string filename;
	s64 file_size = 0;
	char* filedata = 0;

	s64 token_count = 0;
	s64 line_count = 1;

	static void init();

	Lexer_Error start(const char* filename);
	void rewind();
	Token* peek_token();
	Token* peek_token(int advance);
	Token_Type peek_token_type();
	Token_Type peek_token_type(int advance);
	Token* eat_token();
	Token* eat_token(int advance);
	Token* last_token();
	char* get_first_char_in_current_line();
	char* get_first_char_in_line(s64 line);

	char* get_token_string(Token_Type);

	s32 report_lexer_error(char* msg, ...);

	void lex_file();
	bool read_token(char** at);

	s64 current_token = 0;
	s64 current_line = 0;
	s64 current_col = 0;
	Token* token_array = 0;
};

struct Keyword {
	string word;
	Token_Type token_type;
	u32 flags;
};

void internalize_identifier(string* str);