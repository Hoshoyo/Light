#include "error.h"
#include <stdio.h>
#include <stdarg.h>
#include "top_typecheck.h"

#define TOKEN_STR(T) (T)->length, (T)->data

static const char* ColorReset   = "\x1B[0m";
static const char* ColorRed     = "\x1B[31m";

s32
type_error_location(Light_Token* t) {
    if(!t) {
        return fprintf(stderr, "%s: ", t->filepath);
    } else {
        return fprintf(stderr, "%s:%d:%d: ", t->filepath, t->line + 1, t->column + 1);
    }
    return 0;
}

s32
type_error(u32* error, Light_Token* location, const char* fmt, ...) {
    if(error) *error |= TYPE_ERROR;
    
    s32 length = 0;
    va_list args;
    va_start(args, fmt);
    if(location) {
        length += type_error_location(location);
    }
    length += fprintf(stderr, "%sType Error%s: ", ColorRed, ColorReset);
    length += vfprintf(stderr, fmt, args);
    va_end(args);
    
    return length;
}

s32
type_error_undeclared_identifier(u32* error, Light_Token* t) {
    if(error) *error |= TYPE_ERROR;
    
    s32 length = 0;
    length += type_error_location(t);
    length += fprintf(stderr, "%sType Error%s: undeclared identifier '%.*s'\n", ColorRed, ColorReset, TOKEN_STR(t));
    
    return length;
}

s32
type_error_mismatch(u32* error, Light_Token* location, Light_Type* left, Light_Type* right) {
    if(error) *error |= TYPE_ERROR;
    
    s32 length = 0;
    length += type_error_location(location);
    length += fprintf(stderr, "%sType Error%s: type mismatch '", ColorRed, ColorReset);
    length += ast_print_type(left, LIGHT_AST_PRINT_STDERR, 0);
    length += fprintf(stderr, "' vs '");
    length += ast_print_type(right, LIGHT_AST_PRINT_STDERR, 0);
    length += fprintf(stderr, "'");

    return length;
}