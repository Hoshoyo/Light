#pragma once
#include <common.h>
#include "utils/string_table.h"

typedef enum {
	TOKEN_END_OF_STREAM = 0,

	// Symbols
	TOKEN_SYMBOL_NOT             = '!', // 33
	TOKEN_SYMBOL_POUND           = '#', // 35
	TOKEN_SYMBOL_DOLLAR          = '$', // 36
	TOKEN_SYMBOL_MOD             = '%', // 37
	TOKEN_SYMBOL_AND             = '&', // 38
	TOKEN_SYMBOL_OPEN_PAREN      = '(', // 40
	TOKEN_SYMBOL_CLOSE_PAREN     = ')', // 41
	TOKEN_SYMBOL_TIMES           = '*', // 42
	TOKEN_SYMBOL_PLUS            = '+', // 43
	TOKEN_SYMBOL_COMMA           = ',', // 44
	TOKEN_SYMBOL_MINUS           = '-', // 45
	TOKEN_SYMBOL_DOT             = '.', // 46
	TOKEN_SYMBOL_DIV             = '/', // 47
	TOKEN_SYMBOL_COLON           = ':', // 58
	TOKEN_SYMBOL_SEMICOLON       = ';', // 59
	TOKEN_SYMBOL_LESS            = '<', // 60
	TOKEN_SYMBOL_EQUAL           = '=', // 61
	TOKEN_SYMBOL_GREATER         = '>', // 62
	TOKEN_SYMBOL_INTERROGATION   = '?', // 63
	TOKEN_SYMBOL_AT              = '@', // 64
	TOKEN_SYMBOL_OPEN_BRACE      = '{', // 91
	TOKEN_SYMBOL_CLOSE_BRACE     = '}', // 93 ' // stop being confused @vscode
	TOKEN_SYMBOL_CARAT           = '^', // 94
	TOKEN_SYMBOL_BACK_TICK       = '`', // 96
	TOKEN_SYMBOL_OPEN_BRACKET    = '[', // 123
	TOKEN_SYMBOL_PIPE            = '|', // 124
	TOKEN_SYMBOL_CLOSE_BRACKET   = ']', // 125
	TOKEN_SYMBOL_TILDE           = '~', // 126

	// Literal tokens
	TOKEN_LITERAL_DEC_INT            = 127,
    TOKEN_LITERAL_DEC_UINT,
	TOKEN_LITERAL_HEX_INT,
	TOKEN_LITERAL_BIN_INT,
	TOKEN_LITERAL_FLOAT,
	TOKEN_LITERAL_CHAR,
	TOKEN_LITERAL_STRING,
	TOKEN_LITERAL_BOOL_TRUE,
	TOKEN_LITERAL_BOOL_FALSE,

	TOKEN_IDENTIFIER,

	// Operation tokens
	TOKEN_ARROW,
	TOKEN_EQUAL_EQUAL,
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

	//TOKEN_KEYWORD_CAST,

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
	TOKEN_KEYWORD_WHILE,
	TOKEN_KEYWORD_BREAK,
	TOKEN_KEYWORD_CONTINUE,
	TOKEN_KEYWORD_RETURN,
	TOKEN_KEYWORD_ENUM,
	TOKEN_KEYWORD_STRUCT,
	TOKEN_KEYWORD_UNION,
	TOKEN_KEYWORD_NULL,
} Light_Token_Type;

typedef enum {
    TOKEN_FLAG_KEYWORD             = (1 << 0),
    TOKEN_FLAG_TYPE_KEYWORD        = (1 << 1),
    TOKEN_FLAG_ASSIGNMENT_OPERATOR = (1 << 2),
    TOKEN_FLAG_BINARY_OPERATOR     = (1 << 3),
    TOKEN_FLAG_UNARY_OPERATOR      = (1 << 4),
	TOKEN_FLAG_INTEGER_LITERAL     = (1 << 5),
	TOKEN_FLAG_LITERAL             = (1 << 6),
} Light_Token_Flag;

typedef struct {
    Light_Token_Type type;
    s32              line;
    s32              column;
    u8*              data;
    s32              length;
	s32              raw_data_length;
    u32              flags;
	char*            filepath;
	u8*              raw_data;
	u8*              original_data;
} Light_Token;

typedef enum {
	LIGHT_LEXER_PRINT_TOKENS = (1 << 0),
} Light_Lexer_Flags;

typedef struct {
    const char*  filename;
	char*        filepath;
	char*        filepath_absolute;

    s32          line;
    s32          column;
    s32          index;
    Light_Token* tokens;
    u8*          stream;
    u64          stream_size_bytes;

    String_Table  keyword_table;
    bool          keyword_table_initialized;
} Light_Lexer;

typedef enum {
	LIGHT_SPECIAL_IDENT_MAIN,
	LIGHT_SPECIAL_IDENT_FOREIGN,
	LIGHT_SPECIAL_IDENT_ASSERT,
	LIGHT_SPECIAL_IDENT_STRING,
	LIGHT_SPECIAL_IDENT_ARRAY,
	LIGHT_SPECIAL_IDENT_IMPORT,
	LIGHT_SPECIAL_IDENT_SIZEOF,
	LIGHT_SPECIAL_IDENT_TYPEOF,
	LIGHT_SPECIAL_IDENT_TYPEVALUE,
	LIGHT_SPECIAL_IDENT_END,
	LIGHT_SPECIAL_IDENT_RUN,
	LIGHT_SPECIAL_IDENT_EXTERN,

	LIGHT_SPECIAL_IDENT_COUNT,
} Light_Special_Identifiers;

Light_Token* lexer_file(Light_Lexer* lexer, const char* filename, u32 flags);
Light_Token* lexer_cstr(Light_Lexer* lexer, char* str, s32 length, u32 flags);
Light_Token* lexer_next(Light_Lexer* lexer);
Light_Token* lexer_peek(Light_Lexer* lexer);
Light_Token* lexer_peek_n(Light_Lexer* lexer, s32 n);
void         lexer_rewind(Light_Lexer* lexer, s32 count);
void         lexer_free(Light_Lexer* lexer);
const char*  lexer_internalize_identifier(const char* data, int length);
void         initialize_global_identifiers_table();

const char*  token_type_to_str(Light_Token_Type token_type);

Light_Token* token_new_identifier_from_string(const char* str, int length);

extern string light_special_idents_table[LIGHT_SPECIAL_IDENT_COUNT];
