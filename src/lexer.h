#pragma once
#include "util.h"
#include "hash_table.h"

#define TOKEN_STR(T) (T)->value.length, (T)->value.data

enum Token_Type
{
	TOKEN_END_OF_STREAM = 0,
	TOKEN_UNKNOWN = 1,

	// Symbols
	TOKEN_SYMBOL_NOT = '!',				// 33
	TOKEN_SYMBOL_POUND = '#',			// 35
	TOKEN_SYMBOL_DOLLAR = '$',			// 36
	TOKEN_SYMBOL_MOD = '%',				// 37
	TOKEN_SYMBOL_AND = '&',				// 38
	TOKEN_SYMBOL_OPEN_PAREN = '(',		// 40
	TOKEN_SYMBOL_CLOSE_PAREN = ')',		// 41
	TOKEN_SYMBOL_TIMES = '*',			// 42
	TOKEN_SYMBOL_PLUS = '+',			// 43
	TOKEN_SYMBOL_COMMA = ',',			// 44
	TOKEN_SYMBOL_MINUS = '-',			// 45
	TOKEN_SYMBOL_DOT = '.',				// 46
	TOKEN_SYMBOL_DIV = '/',				// 47
	TOKEN_SYMBOL_COLON = ':',			// 58
	TOKEN_SYMBOL_SEMICOLON = ';',		// 59
	TOKEN_SYMBOL_LESS = '<',			// 60
	TOKEN_SYMBOL_EQUAL = '=',			// 61
	TOKEN_SYMBOL_GREATER = '>',			// 62
	TOKEN_SYMBOL_INTERROGATION = '?',	// 63
	TOKEN_SYMBOL_AT = '@',				// 64
	TOKEN_SYMBOL_OPEN_BRACE = '{',		// 91
	TOKEN_SYMBOL_CLOSE_BRACE = '}',		// 93
	TOKEN_SYMBOL_CARAT = '^',			// 94
	TOKEN_SYMBOL_BACK_TICK = '`',		// 96
	TOKEN_SYMBOL_OPEN_BRACKET = '[',	// 123
	TOKEN_SYMBOL_PIPE = '|',			// 124
	TOKEN_SYMBOL_CLOSE_BRACKET = ']',	// 125
	TOKEN_SYMBOL_TILDE = '~',			// 126

	// Literal tokens
	TOKEN_LITERAL_INT = 127,
	TOKEN_LITERAL_HEX_INT,
	TOKEN_LITERAL_BIN_INT,
	TOKEN_LITERAL_FLOAT,
	TOKEN_LITERAL_CHAR,
	TOKEN_LITERAL_STRING,
	TOKEN_LITERAL_BOOL_TRUE,
	TOKEN_LITERAL_BOOL_FALSE,

	TOKEN_IDENTIFIER,

	// Operation tokens
	TOKEN_SYMBOL,
	TOKEN_ARROW,
	TOKEN_EQUAL_COMPARISON,
	TOKEN_LESS_EQUAL,
	TOKEN_GREATER_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_LOGIC_OR,
	TOKEN_LOGIC_AND,
	TOKEN_BITSHIFT_LEFT,
	TOKEN_BITSHIFT_RIGHT,

	TOKEN_PLUS_EQUAL,
	TOKEN_MINUS_EQUAL,
	TOKEN_TIMES_EQUAL,
	TOKEN_DIV_EQUAL,
	TOKEN_MOD_EQUAL,
	TOKEN_AND_EQUAL,
	TOKEN_OR_EQUAL,
	TOKEN_XOR_EQUAL,
	TOKEN_SHL_EQUAL,
	TOKEN_SHR_EQUAL,

	TOKEN_KEYWORD_CAST,

	// Type tokens
	TOKEN_SINT64,
	TOKEN_SINT32,
	TOKEN_SINT16,
	TOKEN_SINT8,
	TOKEN_UINT64,
	TOKEN_UINT32,
	TOKEN_UINT16,
	TOKEN_UINT8,
	TOKEN_REAL32,
	TOKEN_REAL64,
	TOKEN_BOOL,
	TOKEN_VOID,

	// Reserved words
	TOKEN_KEYWORD_IF,
	TOKEN_KEYWORD_ELSE,
	TOKEN_KEYWORD_FOR,
	TOKEN_KEYWORD_BREAK,
	TOKEN_KEYWORD_CONTINUE,
	TOKEN_KEYWORD_RETURN,
	TOKEN_KEYWORD_ENUM,
	TOKEN_KEYWORD_STRUCT,
	TOKEN_KEYWORD_UNION,
	TOKEN_KEYWORD_ARRAY,
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
const u32 TOKEN_FLAG_INTEGER_LITERAL = FLAG(9);

enum Lexer_Error {
	LEXER_OK = 0,
	LEXER_ERROR_FATAL,
	LEXER_ERROR_WARNING,
	LEXER_ERROR_INTERNAL_COMPILER_ERROR,
};

struct Token {
	Token_Type type;

	s32 line;
	s32 column;
	s32 offset_in_file;
	u32 flags = 0;

	string filename;
	string value;
};

struct Lexer
{
	string filename;
	s64    file_size = 0;
	char*  filedata  = 0;

	s64    token_count = 0;
	s64    line_count  = 1;

	s64    current_token = 0;
	s64    current_line  = 0;
	s64    current_col   = 0;
	Token* token_array   = 0;

	Lexer_Error start(const char* filename);

	void rewind();
	Token* peek_token();
	Token* peek_token(int advance);
	Token_Type peek_token_type();
	Token_Type peek_token_type(int advance);
	Token* eat_token();
	Token* eat_token(int advance);
	Token* last_token();

	char* get_token_string(Token_Type);

	s32 report_lexer_error(char* msg, ...);

	void lex_file();
	bool read_token(char** at);

	u64 literal_integer_to_u64(Token* t);

	static void init();
};

struct Keyword {
	string word;
	Token_Type token_type;
	u32 flags;
};

void internalize_identifier(string* str);

enum Operator_Unary;
enum Operator_Binary;

Operator_Unary  token_to_unary_op(Token* t);
Operator_Binary token_to_binary_op(Token* t);