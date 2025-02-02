#include "lexer.h"
#include "utils/os.h"
#include "global_tables.h"
#include "utils/allocator.h"
#include <stdio.h>
#include <string.h>
#include <light_array.h>

string light_special_idents_table[LIGHT_SPECIAL_IDENT_COUNT] = {0};

void
initialize_global_identifiers_table() {
    if(global_identifiers_table.entries_capacity == 0) {
        string_table_new(&global_identifiers_table, 1024 * 1024);
    }

    light_special_idents_table[LIGHT_SPECIAL_IDENT_MAIN]    = MAKE_STR_LEN("main", sizeof("main") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_FOREIGN] = MAKE_STR_LEN("foreign", sizeof("foreign") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_ASSERT]  = MAKE_STR_LEN("assert", sizeof("assert") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_STRING]  = MAKE_STR_LEN("string", sizeof("string") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_ARRAY]  = MAKE_STR_LEN("array", sizeof("array") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_IMPORT]  = MAKE_STR_LEN("import", sizeof("import") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_SIZEOF]  = MAKE_STR_LEN("size_of", sizeof("size_of") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_TYPEOF]  = MAKE_STR_LEN("type_of", sizeof("type_of") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_TYPEVALUE]  = MAKE_STR_LEN("type_value", sizeof("type_value") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_END]     = MAKE_STR_LEN("end", sizeof("end") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_RUN]     = MAKE_STR_LEN("run", sizeof("run") - 1);
    light_special_idents_table[LIGHT_SPECIAL_IDENT_EXTERN] = MAKE_STR_LEN("extern", sizeof("extern") - 1);

    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_MAIN], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_FOREIGN], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_ASSERT], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_STRING], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_ARRAY], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_IMPORT], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_SIZEOF], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_TYPEOF], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_TYPEVALUE], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_END], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_RUN], 0);
    string_table_add(&global_identifiers_table, light_special_idents_table[LIGHT_SPECIAL_IDENT_EXTERN], 0);
}

static int
print_char_hex(char c)
{
    int length = 0;
    char d1 = (c & 0xf0) >> 4;
    char d2 = (c & 0xf);
    length += printf("0x");
    if(d1 < 10) {
        length += printf("%c", d1 + '0');
    } else {
        length += printf("%c", d1 - 10 + 'a');
    }
    if(d2 < 10) {
        length += printf("%c", d2 + '0');
    } else {
        length += printf("%c", d2 - 10 + 'a');
    }
    return length;
}

static int
print_char(char c)
{
    switch(c)
    {
        case 0:  return printf("NUL");
        case 1:  return printf("SOH");
        case 2:  return printf("STX");
        case 3:  return printf("ETX");
        case 4:  return printf("EOT");
        case 5:  return printf("ENQ");
        case 6:  return printf("ACK");
        case 7:  return printf("\\a");
        case 8:  return printf("\\b");
        case 9:  return printf("\\t");
        case 10: return printf("\\n");
        case 11: return printf("\\v");
        case 12: return printf("\\f");
        case 13: return printf("\\r");
        case 14: return printf("SO");
        case 15: return printf("SI");
        case 16: return printf("DLE");
        case 17: return printf("DC1");
        case 18: return printf("DC2");
        case 19: return printf("DC3");
        case 20: return printf("DC4");
        case 21: return printf("NAK");
        case 22: return printf("SYN");
        case 23: return printf("ETB");
        case 24: return printf("CAN");
        case 25: return printf("EM");
        case 26: return printf("SUB");
        case 27: return printf("ESC");
        case 28: return printf("FS");
        case 29: return printf("GS");
        case 30: return printf("RS");
        case 31: return printf("US");
        case 127: return printf("DEL");
        default: {
            if((unsigned char)c > 127) {
                return print_char_hex(c);
            }
            return printf("%c", c);
        } break;
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

static u8
hex_digits_to_char(char d1, char d2)
{
    if(is_number(d1)) d1 -= '0';
    if(is_number(d2)) d2 -= '0';
    if(d1 >= 'a' && d1 <= 'f') d1 = d1 - 'a' + 10;
    if(d1 >= 'A' && d1 <= 'F') d1 = d1 - 'A' + 10;
    if(d2 >= 'a' && d2 <= 'f') d2 = d2 - 'a' + 10;
    if(d2 >= 'A' && d2 <= 'F') d2 = d2 - 'A' + 10;
    
    u8 r = ((d1 << 4) | d2);
    return r;
}

static Light_Token
token_number(u8* at, s32 line, s32 column) {
    Light_Token r = {0};
    r.line = line;
    r.column = column;
    r.data = at;
    r.original_data = at;

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
        r.type = TOKEN_LITERAL_FLOAT;
        r.flags |= TOKEN_FLAG_LITERAL;
    } else {
        bool uns = false;
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
        r.flags |= TOKEN_FLAG_LITERAL;
    }

    r.length = at - r.data;

    return r;
}

static int
token_str_to_raw(Light_Token* token)
{
    // TODO(psv): Optimize this, use an arena in the Lexer or something
    u8* mem = (u8*)calloc(1, token->length);
    for(int i = 1, r = 0; i < token->length - 1; ++i)
    {
        u8 c = token->data[i];
        switch(c)
        {
            case '\\': {
                i++;
                if(i < token->length - 1)
                {
                    u8 n = token->data[i];
                    switch(n)
                    {
                        case '0':   mem[r] = 0; break;
                        case '\\':  mem[r] = '\\'; break;
                        case 'n':   mem[r] = '\n'; break;
                        case 't':   mem[r] = '\t'; break;
                        case 'r':   mem[r] = '\r'; break;
                        case 'a':   mem[r] = '\a'; break;
                        case 'b':   mem[r] = '\b'; break;
                        case 'x': {
                            // hexadecimal
                            i++;
                            if(i + 1 < token->length - 1)
                            {
                                u8 d1 = token->data[i];
                                u8 d2 = token->data[i+1];
                                i++;
                                if(is_hex_digit(d1) && is_hex_digit(d2))
                                {
                                    mem[r] = hex_digits_to_char(d1, d2);
                                }
                                else
                                {
                                    fprintf(stderr, "invalid hex escape sequence, must be two hex digits\n");
                                    return -1;
                                }
                            }
                            else
                            {
                                fprintf(stderr, "invalid hex escape sequence at the end of string literal\n");
                                return -1;
                            }
                        } break;
                        default: {
                            fprintf(stderr, "invalid escape sequence '%c'\n", n);
                            return -1;
                        }
                    }
                    ++r;
                    token->raw_data_length++;
                }
                else
                {
                    fprintf(stderr, "invalid escape sequence at the end of string literal\n");
                    return -1;
                }
            } break;
            default: {
                mem[r] = token->data[i];
                token->raw_data_length++;
                ++r;
            } break;
        }
    }
    token->raw_data = mem;
    return 0;
}

static Light_Token
token_next(Light_Lexer* lexer) {
	u8* at = lexer->stream + lexer->index;
	if (!at) return (Light_Token){ 0 };

	Light_Token r = { 0 };
	r.data = at;
	r.line = (s32)lexer->line;
	r.column = (s32)lexer->column;
    r.original_data = at;

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
                    at++;
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
                    at++;
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
            r.flags |= TOKEN_FLAG_LITERAL;
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
        case '`': {
            r.type = TOKEN_LITERAL_STRING;
            r.flags |= TOKEN_FLAG_LITERAL;
            at++;	// skip "
            for (; *at && *at != '`'; ++at) {
                if(*at == '\n') {
                    lexer->column = 0;
                    lexer->line++;
                }
                    
                if(*at == '\\' && at[1] == '`') {
                    at += 2; // skip \ and `
                }
            }
            at++; // skip closing `
            r.length = at - r.data;
        } break;
		case '"': {
			r.type = TOKEN_LITERAL_STRING;
            r.flags |= TOKEN_FLAG_LITERAL;
			at++;	// skip "

			for (; *at && *at != '"'; ++at) {
                if(*at == '\n') {
                    lexer->column = 0;
                    lexer->line++;
                }
				if (*at == 0) {
					break;
				} else if (*at == '\\') {
					at++;
					if (*at == '"') {
						at++;
					}
                }
			}
			at++; // skip "
			r.length = at - r.data;
            if(token_str_to_raw(&r) == -1) exit(-1);
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
                    r.flags |= TOKEN_FLAG_LITERAL;
                } else if(*at == '0' && at[1] == 'b') {
                    // binary
                    at += 2;
                    while(*at && (*at == '1' || *at == '0')) {
                        ++at;
                    }
                    r.type = TOKEN_LITERAL_BIN_INT;
                    r.flags |= TOKEN_FLAG_LITERAL;
				} else {
					r = token_number(at, lexer->line, lexer->column);
                    break;
				}
                r.flags |= TOKEN_FLAG_INTEGER_LITERAL;
                r.flags |= TOKEN_FLAG_LITERAL;
			} else if (is_letter(*at) || *at == '_') {
                // identifier
				for (; is_letter(*at) || is_number(*at) || *at == '_'; ++at);

				r.type = TOKEN_IDENTIFIER;
                r.length = at - r.data;

                s32 kw_index = 0;
                if(string_table_entry_exist(&lexer->keyword_table, MAKE_STR_LEN((char*)r.data, r.length), &kw_index, 0)) {
                    string kw = string_table_get(&lexer->keyword_table, kw_index);
                    r.type = kw.value;
                    r.flags |= TOKEN_FLAG_KEYWORD;
                    if(kw.value == TOKEN_LITERAL_BOOL_TRUE || kw.value == TOKEN_LITERAL_BOOL_FALSE)
                        r.flags |= TOKEN_FLAG_LITERAL;
                } else {
                    // internalize the identifier
                    r.data = (u8*)lexer_internalize_identifier((const char*)r.data, r.length);
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

    r.filepath = lexer->filepath;
	lexer->stream += r.length;
	lexer->column += r.length;
	return r;
}

const char*
lexer_internalize_identifier(const char* data, int length) {
    s32 identifier_index = 0;
    if(string_table_entry_exist(&global_identifiers_table, MAKE_STR_LEN((char*)data, length), &identifier_index, 0)) {
        string v = string_table_get(&global_identifiers_table, identifier_index);
        return (const char*)v.data;
    }
    string_table_add(&global_identifiers_table, MAKE_STR_LEN((char*)data, length), 0);
    return data;
}

static void
token_print(Light_Token t) {
    /* FOREGROUND */
    const char* RST  = "";//"\x1B[0m";
    const char* KRED = "";//"\x1B[31m";
    const char* KGRN = "";//"\x1B[32m";
    const char* KYEL = "";//"\x1B[33m";
    const char* KBLU = "";//"\x1B[34m";
    const char* KMAG = "";//"\x1B[35m";
    const char* KCYN = "";//"\x1B[36m";
    const char* KWHT = "";//"\x1B[37m";

    if(t.flags & TOKEN_FLAG_KEYWORD) {
        if(t.flags & TOKEN_FLAG_TYPE_KEYWORD) {
            printf("%s%d:%d: %.*s%s\n", KBLU, t.line + 1, t.column, t.length, t.data, RST);
        } else {
            printf("%s%d:%d: %.*s%s\n", KGRN, t.line + 1, t.column, t.length, t.data, RST);
        }
    } else {
        if(t.type == TOKEN_IDENTIFIER) {
            printf("%s%d:%d: %.*s%s\n", KBLU, t.line + 1, t.column, t.length, t.data, RST);
        } else {
            printf("%d:%d: %.*s\n", t.line + 1, t.column, t.length, t.data);
        }
    }
}

static void
lexer_eat_whitespace(Light_Lexer* lexer) {
    int multiline_level = 0;
    bool running = true;
    while(running) 
    {
        u8 c = lexer->stream[lexer->index];
        switch(c) {
            case ' ': case '\t': case '\v': case '\f': case '\r':
                lexer->index++;
                lexer->column++;
            break;
            case '\n':
                lexer->index++;
                lexer->line++;
                lexer->column = 0;
            break;
            case '/':{
                u8 next_token = lexer->stream[lexer->index + 1];
                if(next_token == '/') {
                    // single line comment
                    lexer->index += 2;
                    while (lexer->stream[lexer->index] != '\n') {
                        if(!lexer->stream[lexer->index]) {
                            lexer->index--;
                            break;
                        }
                        lexer->index++;
                    }
                    lexer->line++;
                    lexer->index++;
                    lexer->column = 0;
                } else if(next_token == '*') {
                    // multi line comment
                    lexer->index += 2;
                    multiline_level++;
                    u8* at = lexer->stream + lexer->index;
                    while(true) {
                        if(*at == '*' && at[1] == '/') {
                            multiline_level--;
                            if(multiline_level == 0) break;
                        }
                        if(*at == '/' && at[1] == '*') {
                            multiline_level++;
                        }
                        if(*at == '\n') {
                            lexer->line++;
                            lexer->column = 0;
                        }
                        ++at;
                        ++lexer->index;
                    }
                    lexer->index += 2;
                } else {
                    running = false;
                    break;
                }
            } break;
            default: 
                running = false;
            break;
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
        string_table_add(&lexer->keyword_table, MAKE_STR("null", TOKEN_KEYWORD_NULL), 0);
    }
}

Light_Token* 
lexer_file(Light_Lexer* lexer, const char* filename, u32 flags) {
    FILE* file = fopen(filename, "rb");

    if(!file) {
        fprintf(stderr, "Could not load file %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t length_bytes = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* stream = light_alloc(length_bytes + 1);

    fread(stream, length_bytes, 1, file);
    fclose(file);

    // Copy filename to lexer
    size_t filename_size = strlen(filename);
    lexer->filepath = light_alloc(filename_size + 1);
    memcpy((void*)lexer->filepath, filename, filename_size);

    size_t filepath_absolute_size = 0;
    lexer->filepath_absolute = (char*)light_path_from_filename(filename, &filepath_absolute_size);

    lexer->filename = light_filename_from_path(lexer->filepath);

    return lexer_cstr(lexer, stream, (s32)length_bytes, flags);
}

Light_Token* 
lexer_cstr(Light_Lexer* lexer, char* str, s32 length, u32 flags) {
    lexer_internalize_keywords(lexer);

    lexer->stream = (u8*)str;
    lexer->stream_size_bytes = (size_t)length;

	Light_Token* tokens = array_new(Light_Token);

    while(true) {
        lexer_eat_whitespace(lexer);
        Light_Token t = token_next(lexer);

        // push token
		array_push(tokens, t);

        if(t.type == TOKEN_END_OF_STREAM) break;

        if(flags & LIGHT_LEXER_PRINT_TOKENS)
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

    if(lexer->filepath) {
        // do not free filename, since it is a substring of filepath
        light_free(lexer->filepath);
        light_free(lexer->stream);
    }
}

const char* 
token_type_to_str(Light_Token_Type token_type) {
    switch(token_type) {
        case TOKEN_END_OF_STREAM:        return "end of stream"; break;
        case TOKEN_SYMBOL_NOT:           return "!"; break;
        case TOKEN_SYMBOL_POUND:         return "#"; break;
        case TOKEN_SYMBOL_DOLLAR:        return "$"; break;
        case TOKEN_SYMBOL_MOD:           return "%%"; break;
        case TOKEN_SYMBOL_AND:           return "&"; break;
        case TOKEN_SYMBOL_OPEN_PAREN:    return "("; break;
        case TOKEN_SYMBOL_CLOSE_PAREN:   return ")"; break;
        case TOKEN_SYMBOL_TIMES:         return "*"; break;
        case TOKEN_SYMBOL_PLUS:          return "+"; break;
        case TOKEN_SYMBOL_COMMA:         return ","; break;
        case TOKEN_SYMBOL_MINUS:         return "-"; break;
        case TOKEN_SYMBOL_DOT:           return "."; break;
        case TOKEN_SYMBOL_DIV:           return "/"; break;
        case TOKEN_SYMBOL_COLON:         return ":"; break;
        case TOKEN_SYMBOL_SEMICOLON:     return ";"; break;
        case TOKEN_SYMBOL_LESS:          return "<"; break;
        case TOKEN_SYMBOL_EQUAL:         return "="; break;
        case TOKEN_SYMBOL_GREATER:       return ">"; break;
        case TOKEN_SYMBOL_INTERROGATION: return "?"; break;
        case TOKEN_SYMBOL_AT:            return "@"; break;
        case TOKEN_SYMBOL_OPEN_BRACE:    return "{"; break;
        case TOKEN_SYMBOL_CLOSE_BRACE:   return "}"; break;
        case TOKEN_SYMBOL_CARAT:         return "^"; break;
        case TOKEN_SYMBOL_BACK_TICK:     return "`"; break;
        case TOKEN_SYMBOL_OPEN_BRACKET:  return "["; break;
        case TOKEN_SYMBOL_CLOSE_BRACKET: return "]"; break;
        case TOKEN_SYMBOL_PIPE:          return "|"; break;
        case TOKEN_SYMBOL_TILDE:         return "~"; break;
        case TOKEN_LITERAL_DEC_INT:      return "integer"; break;
        case TOKEN_LITERAL_DEC_UINT:     return "unsigned integer"; break;
        case TOKEN_LITERAL_HEX_INT:      return "hexadecimal integer"; break;
        case TOKEN_LITERAL_BIN_INT:      return "binary integer"; break;
        case TOKEN_LITERAL_FLOAT:        return "float literal"; break;
        case TOKEN_LITERAL_CHAR:         return "character literal"; break;
        case TOKEN_LITERAL_STRING:       return "string literal"; break;
        case TOKEN_LITERAL_BOOL_TRUE:    return "true"; break;
        case TOKEN_LITERAL_BOOL_FALSE:   return "false"; break;
        case TOKEN_IDENTIFIER:           return "identifier"; break;
        case TOKEN_ARROW:                return "->"; break;
        case TOKEN_EQUAL_EQUAL:          return "=="; break;
        case TOKEN_LESS_EQUAL:           return "<="; break;
        case TOKEN_GREATER_EQUAL:        return ">="; break;
        case TOKEN_NOT_EQUAL:            return "!="; break;
        case TOKEN_LOGIC_OR:             return "||"; break;
        case TOKEN_LOGIC_AND:            return "&&"; break;
        case TOKEN_BITSHIFT_LEFT:        return "<<"; break;
        case TOKEN_BITSHIFT_RIGHT:       return ">>"; break;
        case TOKEN_PLUS_EQUAL:           return "+="; break;
        case TOKEN_MINUS_EQUAL:          return "-="; break;
        case TOKEN_TIMES_EQUAL:          return "*="; break;
        case TOKEN_DIV_EQUAL:            return "/="; break;
        case TOKEN_MOD_EQUAL:            return "%%="; break;
        case TOKEN_AND_EQUAL:            return "&="; break;
        case TOKEN_OR_EQUAL:             return "|="; break;
        case TOKEN_XOR_EQUAL:            return "^="; break;
        case TOKEN_SHL_EQUAL:            return "<<="; break;
        case TOKEN_SHR_EQUAL:            return ">>="; break;
        case TOKEN_SINT64:               return "s64"; break;
        case TOKEN_SINT32:               return "s32"; break;
        case TOKEN_SINT16:               return "s16"; break;
        case TOKEN_SINT8:                return "s8"; break;
        case TOKEN_UINT64:               return "u64"; break;
        case TOKEN_UINT32:               return "u32"; break;
        case TOKEN_UINT16:               return "u16"; break;
        case TOKEN_UINT8:                return "u8"; break;
        case TOKEN_REAL32:               return "r32"; break;
        case TOKEN_REAL64:               return "r64"; break;
        case TOKEN_BOOL:                 return "bool"; break;
        case TOKEN_VOID:                 return "void"; break;
        case TOKEN_KEYWORD_IF:           return "if"; break;
        case TOKEN_KEYWORD_ELSE:         return "else"; break;
        case TOKEN_KEYWORD_FOR:          return "for"; break;
        case TOKEN_KEYWORD_WHILE:        return "while"; break;
        case TOKEN_KEYWORD_BREAK:        return "break"; break;
        case TOKEN_KEYWORD_CONTINUE:     return "continue"; break;
        case TOKEN_KEYWORD_RETURN:       return "return"; break;
        case TOKEN_KEYWORD_ENUM:         return "enum"; break;
        case TOKEN_KEYWORD_STRUCT:       return "struct"; break;
        case TOKEN_KEYWORD_UNION:        return "union"; break;
        case TOKEN_KEYWORD_NULL:         return "null"; break;
        default:                         return "unknown"; break;
    }
}

Light_Token* 
token_new_identifier_from_string(const char* str, int length) {
    Light_Token* token = (Light_Token*)light_alloc(sizeof(Light_Token));
    token->column = 0;
    token->line = 0;
    token->type = TOKEN_IDENTIFIER;
    token->length = length;
    token->flags = 0;
    token->filepath = 0;
    token->original_data = (u8*)str;
    token->data = (u8*)lexer_internalize_identifier(str, length);;
    return token;
}