#include "catstring.h"
#include "light_array.h"
#include "os.h"
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
catsprint_string_length(catstring* buffer, const char* str, int length) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, MAX(32, length));
        buffer->capacity = MAX(32, length);
    }

    int n = 0;

    if((buffer->length + length) >= buffer->capacity) {
        buffer_grow_by(buffer, length);
    }
    memcpy(buffer->data + buffer->length, str, length);
    buffer->length += length;

    return length;
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

static int
catsprint_double(catstring* buffer, double value) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, 32);
        buffer->capacity = 32;
    }

    char mem[64] = {0};
    int n = sprintf(mem, "%f", value);
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

    int len = 0;
    for(char* at = str; ;) {
        // push to stream
        if(buffer->length >= buffer->capacity) {
            buffer_grow(buffer);
        }

        int n = 0;

        if(*at == '%') {
            at++; // skip
            switch(*at) {
                case '%': {
                    // just push character
                    buffer->data[buffer->length] = *at;
                    if(!(*at)) break;
                    buffer->length++;
                    len++;
                    at++;
                } break; 
                case 's': {
                    at++;
                    if(*at == '+') {
                        at++;
                        int length = va_arg(args, int);
                        // push string
                        n = catsprint_string_length(buffer, va_arg(args, const char*), length);
                    } else {
                        // push string
                        n = catsprint_string(buffer, va_arg(args, const char*));
                    }
                } break;
                case 'u':{
                    at++;
                    n = catsprint_decimal_unsigned(buffer, va_arg(args, unsigned long long int));
                } break;
                case 'd':{
                    at++;
                    n = catsprint_decimal_signed(buffer, va_arg(args, int));
                } break;
                case 'l':{
                    at++;
                    n = catsprint_decimal_signed(buffer, va_arg(args, long long int));
                } break;
                case 'x':{
                    at++;
                    n = catsprint_hexadecimal(buffer, va_arg(args, unsigned long long int));
                } break;
                case 'f':{
                    at++;
                    n = catsprint_double(buffer, va_arg(args, double));
                } break;
            }
            len += n;
        } else {
            buffer->data[buffer->length] = *at;
            if(!(*at)) break;
            buffer->length++;
            len++;
            at++;
        }
    }

    va_end(args);

    return len;
}

int
catstring_append(catstring* to, catstring* s) {
    if(to->capacity == 0) {
        to->data = calloc(1, MAX(32, s->length));
        to->capacity = MAX(32, s->length);
    }

    int n = 0;

    if((to->length + s->length) >= to->capacity) {
        buffer_grow_by(to, s->length);
    }
    memcpy(to->data + to->length, s->data, s->length);
    to->length += s->length;

    return s->length;
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

catstring
catstring_new_from_file(const char* filename) {
    size_t size = 0;
    char* file_data = light_read_entire_file(filename, &size);

    catstring result = {0};
    result.data = file_data;
    result.capacity = (int)size;
    result.length = (int)size;

    return result;
}

catstring 
catstring_new(const char* str, int length) {
    catstring result = {0};

    result.data = calloc(1, length);
    result.capacity = length;
    memcpy(result.data, str, length);

    return result;
}

void
catstring_print(catstring* s) {
    if(!s->data) return;
    printf("%.*s", s->length, s->data);
}

void
catstring_free(catstring* s) {
    free(s->data);
    s->capacity = 0;
    s->length = 0;
}