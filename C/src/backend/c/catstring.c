#include "catstring.h"
#include "light_array.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// --------------------------------
// ---------- Catstring -----------
// --------------------------------

#define MAX(A, B) ((A > B) ? (A) : (B))

static void
buffer_grow(catstring* s) {
    if(s->capacity < 32) {
        s->data = realloc(s->data, s->capacity + 32);
        s->capacity = s->capacity + 32;
    } else {
        s->data = realloc(s->data, s->capacity * 2);
        s->capacity = s->capacity * 2;
    }
}

static void
buffer_grow_by(catstring* s, int amount) {
    int a = (s->capacity + amount) * 2;
    s->data = realloc(s->data, a);
    s->capacity = a;
}

int
catsprint_token(catstring* buffer, Light_Token* t) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, MAX(32, t->length));
        buffer->capacity = MAX(32, t->length);
    }

    int n = 0;

    if((buffer->length + t->length) >= buffer->capacity) {
        buffer_grow_by(buffer, t->length);
    }
    memcpy(buffer->data + buffer->length, t->data, t->length);
    buffer->length += t->length;

    return t->length;
}

static int
catsprint_string(catstring* buffer, const char* str) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, 32);
        buffer->capacity = 32;
    }

    int n = 0;
    for(const char* at = str; ; ++at, ++n) {
        // push to stream
        if(buffer->length >= buffer->capacity) {
            buffer_grow(buffer);
        }

        buffer->data[buffer->length] = *at;
        if(!(*at)) break;
        buffer->length++;
    }

    return n;
}

static int
catsprint_decimal_unsigned(catstring* buffer, long long int value) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, 32);
        buffer->capacity = 32;
    }

    char mem[64] = {0};
    int n = sprintf(mem, "%llu", value);
    return catsprint_string(buffer, mem);
}

static int
catsprint_decimal_signed(catstring* buffer, long long int value) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, 32);
        buffer->capacity = 32;
    }

    char mem[64] = {0};
    int n = sprintf(mem, "%lld", value);
    return catsprint_string(buffer, mem);
}

static int
catsprint_hexadecimal(catstring* buffer, long long int value) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, 32);
        buffer->capacity = 32;
    }

    char mem[64] = {0};
    int n = sprintf(mem, "0x%llx", value);
    return catsprint_string(buffer, mem);
}

// %%
// %s string, %
int 
catsprint(catstring* buffer, char* str, ...) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, 32);
        buffer->capacity = 32;
    }

    va_list args;
    va_start(args, str);

    int n = 0;
    for(char* at = str; ; ++at, ++n) {
        // push to stream
        if(buffer->length >= buffer->capacity) {
            buffer_grow(buffer);
        }

        if(*at == '%') {
            at++; // skip
            switch(*at) {
                case '%': break; // do nothing, just push character
                case 's': {
                    at++;
                    // push string
                    n += catsprint_string(buffer, va_arg(args, const char*));
                } break;
                case 'u':{
                    at++;
                    n += catsprint_decimal_unsigned(buffer, va_arg(args, unsigned long long int));
                } break;
                case 'd':{
                    at++;
                    n += catsprint_decimal_signed(buffer, va_arg(args, int));
                } break;
                case 'l':{
                    at++;
                    n += catsprint_decimal_signed(buffer, va_arg(args, long long int));
                } break;
                case 'x':{
                    at++;
                    n += catsprint_hexadecimal(buffer, va_arg(args, unsigned long long int));
                } break;
            }
        }

        buffer->data[buffer->length] = *at;
        if(!(*at)) break;
        buffer->length++;
    }

    va_end(args);

    return n;
}

catstring 
catstring_copy(catstring* s) {
    catstring result = {0};

    result.data = calloc(1, s->capacity);
    result.capacity = s->capacity;
    result.length = s->length;
    memcpy(result.data, s->data, s->length);

    return result;
}

int
catstring_to_file(const char* filename, catstring s) {
    FILE* out = fopen(filename, "wb");
    if(!out) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        return -1;
    }

    if(fwrite(s.data, s.length, 1, out) != 1) {
        fclose(out);
        fprintf(stderr, "Could not write to file %s\n", filename);
        return -1;
    }

    fclose(out);
    return 0;
}