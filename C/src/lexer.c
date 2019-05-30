#include "lexer.h"
#include "global_tables.h"
#include <stdio.h>
#include <light_array.h>

#define MAKE_STR(S, V) (string){ sizeof(S) - 1, V, S }
#define MAKE_STR_LEN(S, L) (string){ L, 0, S }

static void
initialize_global_identifiers_table() {
    if(global_identifiers_table.entries_capacity == 0) {
        string_table_new(&global_identifiers_table, 1024 * 1024);
    }
}

static bool
is_letter(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static bool
is_number(char c) {
	return c >= '0' && c <= '9';
}

static bool
is_hex_digit(char c) {
	return (is_number(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

static Light_Token
token_number(u8* at, s32 line, s32 column) {
    Light_Token r = {0};
    r.line = line;
    r.column = column;
    r.data = at;

    bool floating = false;

    while(true) {
        if(*at == '.' && floating == false) { 
            floating = true;
            ++at;
            continue;
        } else if(is_number(*at)) {
            ++at;
        } else {
            break;
        }
    }

    if(floating) {
        // e suffix
        if(*at == 'e' || *at == 'E') {
            ++at;
            if(*at == '-' || *at == '+') {
                ++at;
            }
            while(is_number(*at)) ++at;
        }
    } else {
        bool uns = false;
        s32 long_count = 0;
        // u U suffix
        if(*at == 'u' || *at == 'U') {
            ++at;
            uns = true;
        }
        if(uns) {
            r.type = TOKEN_LITERAL_DEC_UINT;
        } else {
            r.type = TOKEN_LITERAL_DEC_INT;
        }
    }

    r.length = at - r.data;

    return r;
}

static Light_Token
token_next(Light_Lexer* lexer) {
	u8* at = lexer->stream + lexer->index;
	if (!at) return (Light_Token){ 0 };

	Light_Token r = { 0 };
	r.data = at;
	r.line = (s32)lexer->line;
	r.column = (s32)lexer->column;

	switch (*at) {
		case 0: return (Light_Token) { 0 };

        case '<':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_LESS_EQUAL;
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
                ++at;
            } else if(*at == '<') {
                ++at;
                if(*at == '=') {
                    r.type = TOKEN_SHL_EQUAL;
                    r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                } else {
                    r.type = TOKEN_BITSHIFT_LEFT;
                    r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
                }
            } else {
                r.type = '<';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
        case '>':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_GREATER_EQUAL;
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
                ++at;
            } else if(*at == '>') {
                ++at;
                if(*at == '=') {
                    r.type = TOKEN_SHR_EQUAL;
                    r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                } else {
                    r.type = TOKEN_BITSHIFT_RIGHT;
                    r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
                }
            } else {
                r.type = '>';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
        case '!':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_NOT_EQUAL;
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
                ++at;
            } else {
                r.type = '!';
                r.flags |= TOKEN_FLAG_UNARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
        case '|':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_OR_EQUAL;
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                ++at;
            } else if(*at == '|') {
                r.type = TOKEN_LOGIC_OR;
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
                ++at;
            } else {
                r.type = '|';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
        case '=':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_EQUAL_EQUAL;
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
                ++at;
            } else {
                r.type = '=';
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
            }
            r.length = at - r.data;
        }break;
        case '/':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_DIV_EQUAL;
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                ++at;
            } else {
                r.type = '/';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
		case '&': {
            ++at;
            if(*at == '=') {
                r.type = TOKEN_AND_EQUAL;
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                ++at;
            } else if(*at == '&') {
                r.type = TOKEN_LOGIC_AND;
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
                ++at;
            } else {
                r.type = '&';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
        case '+':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_PLUS_EQUAL;
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                ++at;
            } else {
                r.type = '+';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break; 
        case '-':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_MINUS_EQUAL;
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                ++at;
            } else if(*at == '>') {
                r.type = TOKEN_ARROW;
                ++at;
            } else {
                r.type = '-';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
		case '~':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_NOT_EQUAL;
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                ++at;
            } else {
                r.type = '~';
                r.flags |= TOKEN_FLAG_UNARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
		case '%':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_MOD_EQUAL;
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                ++at;
            } else {
                r.type = '%';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
        case '*': {
            ++at;
            if(*at == '=') {
                r.type = TOKEN_TIMES_EQUAL;
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                ++at;
            } else {
                r.type = '*';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;
        case '^':{
            ++at;
            if(*at == '=') {
                r.type = TOKEN_XOR_EQUAL;
                r.flags |= TOKEN_FLAG_ASSIGNMENT_OPERATOR;
                ++at;
            } else {
                r.type = '^';
                r.flags |= TOKEN_FLAG_BINARY_OPERATOR;
            }
            r.length = at - r.data;
        }break;

        case '\'': {
            r.type = TOKEN_LITERAL_CHAR;
            at++;
            if (*at == '\'') {
                at++;
                // TODO(psv): empty character constant error
            } else if(*at == '\\') {
                ++at;
                switch (*at) {
                    case 'a':
                    case 'b':
                    case 'f':
                    case 'n':
                    case 'r':
                    case 't':
                    case 'v':
                    case 'e':
                    case '\\':
                    case '\'':
                    case '"':
                    case '?':
                        at++;
                        break;
                    case 0:
                        break;
                    case 'x':
                        at++;
                        if (is_hex_digit(*at) || is_number(*at)) {
                            at++;
                            if (is_hex_digit(*at) || is_number(*at)) {
                                at++;
                            }
                        } else {
                            //printf("invalid escape sequence '\\x%c", *at);
                        }
                    default: {
                        //printf("invalid escape sequence '\\x%c", *at);
                    }break;
                }
                if(*at != '\'') {
                    //printf("expected end of character literal");
                }
                ++at;
            } else {
                ++at;
            }
            if(*at != '\'') {
                // TODO(psv): error invalid character literal
            } else {
                ++at;
            }
            r.length = at - r.data;
        } break;

        case '.': {
            if(!is_number(at[1])) {
                r.type = *at;
                r.length = 1;
                ++at;
                break;
            }
        }

		// Token string
		case '"': {
			r.type = TOKEN_LITERAL_STRING;
			at++;	// skip "

			for (; *at != '"'; ++at) {
				if (*at == 0) {
					break;
				} else if (*at == '\\') {
					at++;
					if (*at == '"') {
						at++;
					} else {
						switch (*at) {
						case 'a':
						case 'b':
						case 'f':
						case 'n':
						case 'r':
						case 't':
						case 'v':
						case 'e':
						case '\\':
						case '\'':
						case '"':
						case '?':
							break;
						case 0:
							break;
						case 'x':
							at++;
							if (is_hex_digit(*at) || is_number(*at)) {
								at++;
								if (is_hex_digit(*at) || is_number(*at)) {
									at++;
								}
							} else {
								//printf("invalid escape sequence '\\x%c", *at);
							}
						default: {
							//printf("invalid escape sequence '\\x%c", *at);
						}break;
						}
					}
				}
			}
			at++; // skip "
			r.length = at - r.data;
		} break;

		default: {
            if(*at == '.') {
                // float starting with .
                r = token_number(at, lexer->line, lexer->column);
                break;
            } else if (is_number(*at)) {
				if (*at == '0' && at[1] == 'x') {
					// hex
					at += 2;
					while (*at && is_hex_digit(*at)) {
						++at;
					}
					r.type = TOKEN_LITERAL_HEX_INT;
                } else if(*at == '0' && at[1] == 'b') {
                    // binary
                    at += 2;
                    while(*at && (*at == '1' || *at == '0')) {
                        ++at;
                    }
                    r.type = TOKEN_LITERAL_BIN_INT;
				} else {
					r = token_number(at, lexer->line, lexer->column);
                    break;
				}
			} else if (is_letter(*at) || *at == '_') {
                // identifier
				for (; is_letter(*at) || is_number(*at) || *at == '_'; ++at);

				r.type = TOKEN_IDENTIFIER;
                r.length = at - r.data;

                s32 kw_index = 0;
                if(string_table_entry_exist(&lexer->keyword_table, MAKE_STR_LEN(r.data, r.length), &kw_index, 0)) {
                    string kw = string_table_get(&lexer->keyword_table, kw_index);
                    r.type = kw.value;
                    r.flags |= TOKEN_FLAG_KEYWORD;
                } else {
                    // internalize the identifier
                    s32 identifier_index = 0;
                    if(string_table_entry_exist(&global_identifiers_table, MAKE_STR_LEN(r.data, r.length), &identifier_index, 0)) {
                        string v = string_table_get(&global_identifiers_table, identifier_index);
                        r.data = v.data;
                    } else {
                        string_table_add(&global_identifiers_table, MAKE_STR_LEN(r.data, r.length), 0);
                    }
                }

                break;
			} else {
				r.type = *at;
			    r.length = 1;
                ++at;
			}
			r.length = at - r.data;
		} break;
	}

	lexer->stream += r.length;
	lexer->column += r.length;
	return r;
}


static void
token_print(Light_Token t) {
    /* FOREGROUND */
    const char* RST  = "\x1B[0m";
    const char* KRED = "\x1B[31m";
    const char* KGRN = "\x1B[32m";
    const char* KYEL = "\x1B[33m";
    const char* KBLU = "\x1B[34m";
    const char* KMAG = "\x1B[35m";
    const char* KCYN = "\x1B[36m";
    const char* KWHT = "\x1B[37m";

    if(t.flags & TOKEN_FLAG_KEYWORD) {
        if(t.flags & TOKEN_FLAG_TYPE_KEYWORD) {
            printf("%s%d:%d: %.*s%s\n", KBLU, t.line, t.column, t.length, t.data, RST);
        } else {
            printf("%s%d:%d: %.*s%s\n", KGRN, t.line, t.column, t.length, t.data, RST);
        }
    } else {
        if(t.type == TOKEN_IDENTIFIER) {
            printf("%s%d:%d: %.*s%s\n", KBLU, t.line, t.column, t.length, t.data, RST);
        } else {
            printf("%d:%d: %.*s\n", t.line, t.column, t.length, t.data);
        }
    }
}

static void
lexer_eat_whitespace(Light_Lexer* lexer) {
    while(true) {
        u8 c = lexer->stream[lexer->index];
        if (c == ' ' ||
		    c == '\t' ||
		    c == '\v' ||
		    c == '\f' ||
		    c == '\r') 
        {
            lexer->index++;
            lexer->column++;
        } else if(c == '\n') {
            lexer->index++;
            lexer->line++;
            lexer->column = 0;
        } else {
            if(c == '/' && lexer->stream[lexer->index + 1] == '/') {
                // single line comment
                lexer->index += 2;
				while (lexer->stream[lexer->index] && lexer->stream[lexer->index] != '\n') {
					lexer->index++;
				}
                lexer->line++;
                lexer->column = 0;
            } else if(c == '/' && lexer->stream[lexer->index + 1] == '*') {
                // multi line comment
                lexer->index += 2;
                u8* at = lexer->stream + lexer->index;
                while(true) {
                    if(*at == '*' && at[1] == '/') break;
                    if(*at == '\n') {
                        lexer->line++;
                        lexer->column = 0;
                    }
                    ++at;
                    ++lexer->index;
                }
                lexer->index += 2;
            } else {
                break;
            }
        }
    }
}

static void
lexer_internalize_keywords(Light_Lexer* lexer) {
    if(!lexer->keyword_table_initialized) {
        string_table_new(&lexer->keyword_table, 2048);
        string_table_add(&lexer->keyword_table, MAKE_STR("bool", TOKEN_BOOL), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("void", TOKEN_VOID), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("r32", TOKEN_REAL32), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("r64", TOKEN_REAL64), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("s8", TOKEN_SINT8), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("s16", TOKEN_SINT16), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("s32", TOKEN_SINT32), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("s64", TOKEN_SINT64), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("u8", TOKEN_UINT8), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("u16", TOKEN_UINT16), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("u32", TOKEN_UINT32), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("u64", TOKEN_UINT64), 0);

        string_table_add(&lexer->keyword_table, MAKE_STR("true", TOKEN_LITERAL_BOOL_TRUE), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("false", TOKEN_LITERAL_BOOL_FALSE), 0);

        string_table_add(&lexer->keyword_table, MAKE_STR("if", TOKEN_KEYWORD_IF), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("else", TOKEN_KEYWORD_ELSE), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("for", TOKEN_KEYWORD_FOR), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("while", TOKEN_KEYWORD_WHILE), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("break", TOKEN_KEYWORD_BREAK), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("continue", TOKEN_KEYWORD_CONTINUE), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("return", TOKEN_KEYWORD_RETURN), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("struct", TOKEN_KEYWORD_STRUCT), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("enum", TOKEN_KEYWORD_ENUM), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("union", TOKEN_KEYWORD_UNION), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("array", TOKEN_KEYWORD_ARRAY), 0);
        string_table_add(&lexer->keyword_table, MAKE_STR("null", TOKEN_KEYWORD_NULL), 0);
    }
}

Light_Token* 
lexer_file(Light_Lexer* lexer, const char* filename, u32 flags) {

}

Light_Token* 
lexer_cstr(Light_Lexer* lexer, char* str, s32 length, u32 flags) {
    initialize_global_identifiers_table();
    lexer_internalize_keywords(lexer);

    lexer->stream = str;

	Light_Token* tokens = array_new(Light_Token);

    while(true) {
        lexer_eat_whitespace(lexer);
        Light_Token t = token_next(lexer);

        // push token
		array_push(tokens, t);

        if(t.type == TOKEN_END_OF_STREAM) break;

        token_print(t);
    }

	lexer->tokens = tokens;
	lexer->index = 0;

    return tokens;
}

Light_Token* 
lexer_next(Light_Lexer* lexer) {
    return &lexer->tokens[lexer->index++];
}

Light_Token* 
lexer_peek(Light_Lexer* lexer) {
    return &lexer->tokens[lexer->index];
}

Light_Token* 
lexer_peek_n(Light_Lexer* lexer, s32 n) {
    return &lexer->tokens[lexer->index + n];
}

void  
lexer_rewind(Light_Lexer* lexer, s32 count) {
    lexer->index -= count;
}

void  
lexer_free(Light_Lexer* lexer) {
    // Free keyword table
    string_table_free(&lexer->keyword_table);
    lexer->keyword_table_initialized = false;
}

const char*  token_to_str(Light_Token* token);
const char*  token_type_to_str(Light_Token_Type token_type);