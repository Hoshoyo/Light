#pragma once
#include "../lexer.h"

typedef struct {
    int   length;
    int   capacity;
    char* data;
} catstring;

int       catsprint(catstring* buffer, char* str, ...);
int       catsprint_token(catstring* buffer, Light_Token* t);
int       catstring_append(catstring* to, catstring* s);
int       catstring_to_file(const char* filename, catstring s);
catstring catstring_new(const char* str, int length);
catstring catstring_new_from_file(const char* filename);
catstring catstring_copy(catstring* s);
void      catstring_print(catstring* s);
void      catstring_free(catstring* s);