#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "light_array.h"

#define bool int
#define true 1
#define false 0

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_IDENTIFIER = 255,
    TOKEN_NUMBER,
    TOKEN_TYPEDEF,
    TOKEN_STRUCT,
    TOKEN_INT,
    TOKEN_CHAR,
    TOKEN_FLOAT,
} Token_Type;

typedef struct {
    Token_Type type;
    char*      data;
    int        length;
    int        line;
} Token;

typedef struct {
    Token* tokens;
    int    token_count;
    int    token_current;
} Tokenized;

Token* token_next(Tokenized* tokens) {
    return &tokens->tokens[tokens->token_current++];
}
Token* token_peek(Tokenized* tokens) {
    return &tokens->tokens[tokens->token_current];
}

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
    int line = 1;

    while(*at) {
        switch(*at) {
            case '{':
            case '}':
            case ';':
            case '*':{
                tokens[token_count].data   = at;
                tokens[token_count].length = 1;
                tokens[token_count].type   = *at;
                tokens[token_count].line   = line;
                at++;
            } break;
            case '/':
                if(at[1] == '/'){
                    while(*at++ != '\n' && *at != 0){
                    }
                    token_count--;
                    break;
                }

            default: {
                if(is_digit(*at)) {
                    char* start = at;
                    while(is_digit(*at++));
                    int length = at - start;
                    tokens[token_count].data = start;
                    tokens[token_count].length = length;
                    tokens[token_count].type = TOKEN_NUMBER;
                    tokens[token_count].line   = line;
                } else if(is_alpha(*at)) {
                    char* start = at;
                    while(is_alpha(*at) || is_digit(*at) || *at == '_') {
                        at++;
                    }
                    int length = at - start;
                    tokens[token_count].data = start;
                    tokens[token_count].length = length;
                    tokens[token_count].line   = line;
                    if(strncmp(start, "typedef", sizeof("typedef") - 1) == 0) {
                        tokens[token_count].type = TOKEN_TYPEDEF;
                    } else if(strncmp(start, "struct", sizeof("struct") - 1) == 0) {
                        tokens[token_count].type = TOKEN_STRUCT;
                    } else if(strncmp(start, "int", sizeof("int") - 1) == 0) {
                        tokens[token_count].type = TOKEN_INT;
                    } else if(strncmp(start, "char", sizeof("char") - 1) == 0) {
                        tokens[token_count].type = TOKEN_CHAR;
                    } else if(strncmp(start, "float", sizeof("float") - 1) == 0) {
                        tokens[token_count].type = TOKEN_FLOAT;
                    } else {
                        tokens[token_count].type = TOKEN_IDENTIFIER;
                    }
                } else {
                    if(*at == '\n'){
                        line++;
                    }
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
    result.token_current = 0;
    return result;
}

typedef enum {
    KIND_POINTER,
    KIND_PRIMITIVE,
    KIND_STRUCT,
} Kind;

typedef enum {
    TYPE_PRIMITIVE_INT,
    TYPE_PRIMITIVE_CHAR,
    TYPE_PRIMITIVE_FLOAT,
} Type_Primitive;

typedef struct Type_Info_t{
    Kind kind;
    union {
        Type_Primitive primitive;
        struct Type_Info_t* ptr_to;
        struct {
            char* name;
            int   name_length;
        } struct_desc;
    };
} Type_Info;

typedef struct {
    char*      name;
    int        name_length;
    Type_Info* type_info;
} Field;

typedef struct {
    // struct
    char*  struct_name;
    int    name_length;

    Field* fields;
    int    field_count;
} Ast;

void require_next(Tokenized* tokens, Token_Type type) {
    Token* t = token_next(tokens);
    if(t->type != type) {
        fprintf(stderr, "%d: required token type %d but got %d\n", t->line, type, t->type);
        exit(-1);
    }
}

Type_Info* parse_type(Tokenized* tokens, Type_Info* subtype) {
    Token* n = token_next(tokens);
    Type_Info* type_info = (Type_Info*)calloc(1, sizeof(Type_Info));
    if(n->type == TOKEN_STRUCT) {
        n = token_next(tokens);
    }
    switch(n->type) {
        case TOKEN_INT:
            type_info->kind = KIND_PRIMITIVE;
            type_info->primitive = TYPE_PRIMITIVE_INT;
            break;
        case TOKEN_CHAR:
            type_info->kind = KIND_PRIMITIVE;
            type_info->primitive = TYPE_PRIMITIVE_CHAR;
            break;
        case TOKEN_FLOAT:
            type_info->kind = KIND_PRIMITIVE;
            type_info->primitive = TYPE_PRIMITIVE_FLOAT;
            break;
        case '*':
            if(!subtype){
                fprintf(stderr, "%d: invalid type definition\n", n->line);
                exit(-1);
            }
            type_info->kind = KIND_POINTER;
            type_info->ptr_to = subtype;
            break;
        case TOKEN_IDENTIFIER:
            type_info->kind = KIND_STRUCT;
            type_info->struct_desc.name = n->data;
            type_info->struct_desc.name_length = n->length;
            break;
        default:{
            fprintf(stderr, "%d: invalid token '%.*s' in type definition\n", n->line, n->length, n->data);
            exit(-1);
        }break;
    }
    if(token_peek(tokens)->type == '*'){
        return parse_type(tokens, type_info);
    }
    return type_info;
}

Field parse_field(Tokenized* tokens){
    Field result;
    result.type_info = parse_type(tokens, 0);
    Token* name = token_next(tokens);
    if(name->type == TOKEN_IDENTIFIER) {
        result.name = name->data;
        result.name_length = name->length;
        require_next(tokens, ';');
    } else {
        fprintf(stderr, "%d: expected identifier after type declaration of struct field\n", name->line);
        exit(-1);
    }
    return result;
}

Ast** parse(Tokenized* tokens) {
    Ast** decls = array_new(Ast*);
    while(true) {
        Token* t = token_next(tokens);
        if(t->type == TOKEN_EOF) break;
        if(t->type == TOKEN_TYPEDEF){
            Ast* node = (Ast*)calloc(1, sizeof(Ast));
            require_next(tokens, TOKEN_STRUCT);
            if(token_peek(tokens)->type == TOKEN_IDENTIFIER) {
                token_next(tokens);
            }
            require_next(tokens, '{');

            Field* fields = array_new(Field);
            while(token_peek(tokens)->type != '}'){
                array_push(fields, parse_field(tokens));
            }

            require_next(tokens, '}');
            Token* name = token_next(tokens);
            if(name->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "%d: struct declaration must be named\n", name->line);
                exit(-1);
            }

            require_next(tokens, ';');
            
            node->field_count = array_length(fields);
            node->fields = fields;
            node->name_length = name->length;
            node->struct_name = name->data;
            array_push(decls, node);
        } else {
            fprintf(stderr, "%d: invalid token '%.*s'\n", t->line, t->length, t->data);
            exit(-1);
        }
    }
    return decls;
}

void generate_indent_level(FILE* out, char* var_name) {
    fprintf(out, "{ for(int i = 0; i < %s; ++i) printf(\"\\t\"); }\n", var_name);
}

#define STRUCT_STR(T) T->name_length, T->struct_name
void generate_introspection_code(Ast** decls){
    FILE* out = fopen("introspection_generated.h", "w");
    //out = stdout;
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include \"introspect.h\"\n");
    for(int i = 0; i < array_length(decls); ++i) {
        Ast* node = decls[i];
        // If the struct being read starts with entity generate the code
        if(true || strncmp(node->struct_name, "Entity", sizeof("Entity") - 1) == 0) {
            fprintf(out, "void print_%.*s(%.*s *e, int indent_level) {\n", STRUCT_STR(node), STRUCT_STR(node));
            generate_indent_level(out, "indent_level");
            fprintf(out, "printf(\"%.*s\\n\");\n", STRUCT_STR(node));
            for(int f = 0; f < array_length(node->fields); ++f) {
                Field* field = &node->fields[f];
                switch(field->type_info->kind){
                    case KIND_PRIMITIVE:{
                        char* specifier = 0;
                        switch(field->type_info->primitive) {
                            case TYPE_PRIMITIVE_INT:   specifier = "d"; break;
                            case TYPE_PRIMITIVE_CHAR:  specifier = "c"; break;
                            case TYPE_PRIMITIVE_FLOAT: specifier = "f"; break;
                        }
                        generate_indent_level(out, "indent_level");
                        fprintf(out, "printf(\"%.*s -> %%%s\\n\", ", field->name_length, field->name, specifier);
                        fprintf(out, "e->%.*s);\n", field->name_length, field->name);
                    }break;

                    case KIND_POINTER:{
                        generate_indent_level(out, "indent_level");
                        if(field->type_info->ptr_to->kind == KIND_STRUCT) {
                            fprintf(out, "if(e->%.*s) {\n", field->name_length, field->name);
                            fprintf(out, "print_%.*s(", field->type_info->ptr_to->struct_desc.name_length, field->type_info->ptr_to->struct_desc.name);
                            fprintf(out, "e->%.*s, indent_level + 1);\n", field->name_length, field->name);
                            fprintf(out, "}\n");
                        } else {
                            if(field->type_info->ptr_to->kind == KIND_PRIMITIVE && field->type_info->ptr_to->primitive == TYPE_PRIMITIVE_CHAR){
                                fprintf(out, "printf(\"%.*s -> %%s\\n\", ", field->name_length, field->name);
                                fprintf(out, "e->%.*s);\n", field->name_length, field->name);
                            } else {
                                fprintf(out, "printf(\"%.*s -> %%p\\n\", ", field->name_length, field->name);
                                fprintf(out, "e->%.*s);\n", field->name_length, field->name);
                            }
                        }
                    }break;
                    case KIND_STRUCT:{
                        generate_indent_level(out, "indent_level");
                        fprintf(out, "printf(\"%.*s -> \");\n", field->name_length, field->name);
                        fprintf(out, "print_%.*s(&e->%.*s, indent_level + 1);\n", 
                            field->type_info->struct_desc.name_length, field->type_info->struct_desc.name, 
                            field->name_length, field->name);
                    }break;
                    break;
                }
            }
            fprintf(out, "\tprintf(\"\\n\");\n");
            fprintf(out, "}\n");
        }
    }
    fclose(out);
}

void generate_introspection_graph_code(Ast** decls) {
    FILE* out = fopen("introspection_generated.h", "w");
    //out = stdout;
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include \"introspect.h\"\n");
    for(int i = 0; i < array_length(decls); ++i) {
        Ast* node = decls[i];
        // If the struct being read starts with entity generate the code
        if(true || strncmp(node->struct_name, "Entity", sizeof("Entity") - 1) == 0) {
            fprintf(out, "void print_%.*s(%.*s *e, int indent_level) {\n", STRUCT_STR(node), STRUCT_STR(node));
            for(int f = 0; f < array_length(node->fields); ++f) {
                Field* field = &node->fields[f];
                switch(field->type_info->kind){
                    case KIND_PRIMITIVE:{
                        char* specifier = 0;
                        switch(field->type_info->primitive) {
                            case TYPE_PRIMITIVE_INT:   specifier = "d"; break;
                            case TYPE_PRIMITIVE_CHAR:  specifier = "c"; break;
                            case TYPE_PRIMITIVE_FLOAT: specifier = "f"; break;
                        }
                        fprintf(out, "printf(\"%.*s -> %.*s\\n\");\n", STRUCT_STR(node), field->name_length, field->name);
                        fprintf(out, "printf(\"%.*s -> %%%s\\n\", ", field->name_length, field->name, specifier);
                        fprintf(out, "e->%.*s);\n", field->name_length, field->name);
                    }break;

                    case KIND_POINTER:{
                        if(field->type_info->ptr_to->kind == KIND_STRUCT) {
                        } else {
                            if(field->type_info->ptr_to->kind == KIND_PRIMITIVE && field->type_info->ptr_to->primitive == TYPE_PRIMITIVE_CHAR){
                                fprintf(out, "printf(\"%.*s -> %.*s\\n\");\n", STRUCT_STR(node), field->name_length, field->name);
                                fprintf(out, "printf(\"%.*s -> %%s\\n\", ", field->name_length, field->name);
                                fprintf(out, "e->%.*s);\n", field->name_length, field->name);
                            } else {

                            }
                        }
                    }break;

                    case KIND_STRUCT:{

                    }break;
                }
            }
            fprintf(out, "\tprintf(\"\\n\");\n");
            fprintf(out, "}\n");
        }
    }
    fclose(out);
}

int main(int argc, char** argv) {
    int file_size = 0;
    char* data = read_entire_file("introspect.h", &file_size);

    Tokenized tokens = tokenize(data, file_size);

    Ast** decls = parse(&tokens);
    generate_introspection_code(decls);
    //generate_introspection_graph_code(decls);

    free(tokens.tokens);
    return 0;
}