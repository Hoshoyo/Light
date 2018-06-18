#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool int
#define true 1
#define false 0

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_IDENTIFIER = 255,
    TOKEN_NUMBER,
    TOKEN_TYPEDEF,
    TOKEN_STRUCT,
} Token_Type;

typedef struct {
    Token_Type type;
    char*      data;
    int        length;
    // ... other info line number, etc
} Token;

typedef struct {
    Token* tokens;
    int    token_count;
} Tokenized;

char* read_entire_file (char* filename, int* out_file_size) {
    FILE* fd = fopen(filename, "r");

    fseek(fd, 0, SEEK_END);
    int size = ftell(fd);
    if(out_file_size) *out_file_size = size;
    fseek(fd, 0, SEEK_SET);

    char* memory = (char*)malloc(size + 1);
    memory[size] = 0;

    fread(memory, size, 1, fd);

    fclose(fd);

    return memory;
}

bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}
bool is_alpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

Tokenized tokenize(char* data, int length) {
    Tokenized result = {0};
    Token* tokens = (Token*)malloc(length * sizeof(Token));
    int token_count = 0;
    char* at = data;

    while(*at) {
        switch(*at) {
            case '{':
            case ';':
            case '*':{
                tokens[token_count].data   = at;
                tokens[token_count].length = 1;
                tokens[token_count].type   = *at;
                at++;
            } break;

            default: {
                if(is_digit(*at)) {
                    char* start = at;
                    while(is_digit(*at++));
                    int length = at - start;
                    tokens[token_count].data = start;
                    tokens[token_count].length = length;
                    tokens[token_count].type = TOKEN_NUMBER;
                } else if(is_alpha(*at)) {
                    char* start = at;
                    while(is_alpha(*at) || is_digit(*at) || *at == '_') {
                        at++;
                    }
                    int length = at - start;
                    tokens[token_count].data = start;
                    tokens[token_count].length = length;
                    if(strncmp(start, "typedef", sizeof("typedef") - 1) == 0) {
                        tokens[token_count].type = TOKEN_TYPEDEF;
                    } else if(strncmp(start, "struct", sizeof("struct") - 1) == 0) {
                        tokens[token_count].type = TOKEN_STRUCT;
                    } else {
                        tokens[token_count].type = TOKEN_IDENTIFIER;
                    }
                } else {
                    // ignore
                    at++;
                    token_count--;
                }
            }break;
        }
        token_count++;
    }
    tokens[token_count].type = TOKEN_EOF;
    result.token_count = token_count;
    result.tokens = tokens;
    return result;
}

typedef enum {
    KIND_POINTER,
    KIND_PRIMITIVE,
} Kind;

typedef enum {
    TYPE_PRIMITIVE_INT,
    TYPE_PRIMITIVE_CHAR,
    TYPE_PRIMITIVE_FLOAT,
} Type_Primitive;

typedef struct {
    Kind kind;
    union {
        Type_Primitive primitive;
        struct Type_Info* ptr_to;
    };
} Type_Info;

typedef struct {
    char* name;
    int name_length;
} Field;

typedef struct {
    // struct
    char* struct_name;
    int   name_length;

    Field* fields;
    int    field_count;
} Ast;

Ast* parse() {

}

int main(int argc, char** argv) {
    int file_size = 0;
    char* data = read_entire_file("introspect.h", &file_size);

    Tokenized tokens = tokenize(data, file_size);

    for(int i = 0; i < tokens.token_count; ++i) {
        printf("TOKEN:%.*s, %d\n", tokens.tokens[i].length, tokens.tokens[i].data, tokens.tokens[i].type);
    }

    free(tokens.tokens);
    return 0;
}