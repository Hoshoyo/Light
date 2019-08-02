#include "backend_c.h"
#include "../include/light_array.h"
#include "type_table.h"
#include <assert.h>
#include <stdio.h>

// --------------------------------
// ---------- Catstring -----------
// --------------------------------

#define MAX(A, B) ((A > B) ? (A) : (B))

typedef struct {
    int   length;
    int   capacity;
    char* data;
} catstring;

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

static int 
catsprint_u64(catstring* buffer, u64 num) {
    if((buffer->length + 32) >= buffer->capacity) {
        buffer_grow_by(buffer, 32);
    }
    int c = sprintf(buffer->data, "%lx", num);
    buffer->length += c;

    return c;
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

int 
catsprint(catstring* buffer, char* str) {
    if(buffer->capacity == 0) {
        buffer->data = calloc(1, 32);
        buffer->capacity = 32;
    }

    int n = 0;
    for(char* at = str;; ++at, ++n) {
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

// Generate

static Light_Type** 
type_table_array_get(Type_Table type_table) {
    Light_Type** result = array_new(Light_Type*);
    for(int i = 0; i < type_table.entries_capacity; ++i) {
        if(type_table.entries[i].flags & HASH_TABLE_OCCUPIED) {
            array_push(result, type_table.entries[i].data);
        }
    }
    return result;
}

void emit_type(catstring* buffer, Light_Type* type, char* name);

static void emit_type_internal(catstring* buffer, Light_Type* type) {
    if(type->kind == TYPE_KIND_ALIAS) {
        catsprint_token(buffer, type->alias.name);
    } else {
        char b[64] = {0};
        sprintf(b, "type_%p", type);
        catsprint(buffer, b);
    }
}

static void
emit_declaration(catstring* s, Light_Ast* node) {
    switch(node->kind) {
        case AST_DECL_PROCEDURE: {

        } break;
        case AST_DECL_VARIABLE: {
            emit_type_internal(s, node->decl_variable.type);
            catsprint(s, " ");
            catsprint_token(s, node->decl_variable.name);
        } break;

        case AST_DECL_CONSTANT:
        case AST_DECL_TYPEDEF:
        default: break;
    }
}

void
emit_type(catstring* buffer, Light_Type* type, char* name) {
    switch(type->kind){
        case TYPE_KIND_PRIMITIVE:{
            switch(type->primitive){
                case TYPE_PRIMITIVE_BOOL:
                    catsprint(buffer, "bool"); break;
                case TYPE_PRIMITIVE_R32:
                    catsprint(buffer, "r32"); break;
                case TYPE_PRIMITIVE_R64:
                    catsprint(buffer, "r64"); break;
                case TYPE_PRIMITIVE_S8:
                    catsprint(buffer, "s8"); break;
                case TYPE_PRIMITIVE_S16:
                    catsprint(buffer, "s16"); break;
                case TYPE_PRIMITIVE_S32:
                    catsprint(buffer, "s32"); break;
                case TYPE_PRIMITIVE_S64:
                    catsprint(buffer, "s64"); break;
                case TYPE_PRIMITIVE_U8:
                    catsprint(buffer, "u8"); break;
                case TYPE_PRIMITIVE_U16:
                    catsprint(buffer, "u16"); break;
                case TYPE_PRIMITIVE_U32:
                    catsprint(buffer, "u32"); break;
                case TYPE_PRIMITIVE_U64:
                    catsprint(buffer, "u64"); break;
                case TYPE_PRIMITIVE_VOID:
                    catsprint(buffer, "void"); break;
                default: assert(0); break;
            }
        }break;
        case TYPE_KIND_POINTER: {
            if(type->pointer_to->kind == TYPE_KIND_ALIAS) {
                Light_Type* ptrto_root = type_alias_root(type->pointer_to);
                switch(ptrto_root->kind) {
                    case TYPE_KIND_ENUM: catsprint(buffer, "enum "); break;
                    case TYPE_KIND_STRUCT: catsprint(buffer, "struct "); break;
                    case TYPE_KIND_UNION: catsprint(buffer, "union "); break;
                    default: break;
                }
            }

            emit_type(buffer, type->pointer_to, 0);
            catsprint(buffer, "*");
        }break;
        case TYPE_KIND_ARRAY:{
        }break;
        case TYPE_KIND_ENUM: {
        } break;
        case TYPE_KIND_FUNCTION:{
            switch(type->function.return_type->kind) {
                case TYPE_KIND_FUNCTION:
                case TYPE_KIND_ARRAY:
                    catsprint(buffer, "void*");
                    break;
                default:
                    emit_type(buffer, type->function.return_type, 0);
                    break;
            }
            if(name) {
                catsprint(buffer, " ");
                catsprint(buffer, name);
            }
        }break;
		case TYPE_KIND_UNION: {
            catsprint(buffer, "union {");
            for(int i = 0; i < type->union_info.fields_count; ++i) {
                emit_declaration(buffer, type->union_info.fields[i]);
                catsprint(buffer, ";");
            }
            catsprint(buffer, "}");
		}break;
        case TYPE_KIND_STRUCT: {
			catsprint(buffer, "struct {");
            for(int i = 0; i < type->struct_info.fields_count; ++i) {
                emit_declaration(buffer, type->struct_info.fields[i]);
                catsprint(buffer, ";");
            }
            catsprint(buffer, "}");
        }break;
        case TYPE_KIND_ALIAS: {
            catsprint_token(buffer, type->alias.name);
        } break;
        default: break;
    }
}

void
emit_forward_type_decl(catstring* buffer, Light_Type** types) {
    for(int i = 0; i < array_length(types); ++i){
        Light_Type* type = types[i];
        Light_Type* type_root = type_alias_root(type);

        switch(type->kind) {
            case TYPE_KIND_ALIAS: {
                catsprint(buffer, "typedef ");
                emit_type(buffer, type_root, 0);
                catsprint(buffer, " ");
                catsprint_token(buffer, type->alias.name);
                catsprint(buffer, ";\n");
            } break;

            case TYPE_KIND_PRIMITIVE: 
            case TYPE_KIND_ENUM: 
            case TYPE_KIND_ARRAY:
            break;
            default: {
                char b[64] = {0};
                sprintf(b, "type_%p", type);
                catsprint(buffer, "typedef ");
                emit_type(buffer, type, 0);
                catsprint(buffer, " ");
                catsprint(buffer, b);
                catsprint(buffer, ";\n");
            } break;
        }
    }
}

void 
backend_c_generate_top_level(Light_Ast** ast, Type_Table type_table) {
    catstring code = {0};

    catsprint(&code, "typedef char s8;\n");
    catsprint(&code, "typedef short s16;\n");
    catsprint(&code, "typedef int s32;\n");
    catsprint(&code, "typedef long long s64;\n");
    catsprint(&code, "typedef unsigned char u8;\n");
    catsprint(&code, "typedef unsigned short u16;\n");
    catsprint(&code, "typedef unsigned int u32;\n");
    catsprint(&code, "typedef unsigned long long u64;\n");
    catsprint(&code, "typedef u32 bool;\n");
    catsprint(&code, "typedef float r32;\n");
    catsprint(&code, "typedef double r64;\n");
	catsprint(&code, "#define true 1\n");
	catsprint(&code, "#define false 0\n");
    catsprint(&code, "\n\n");

    catsprint(&code, "void __memory_copy(void* dest, void* src, u64 size) {\n");
	catsprint(&code, "\tfor(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];\n");
	catsprint(&code, "}\n");

    Light_Type** type_table_array = type_table_array_get(type_table);

    emit_forward_type_decl(&code, type_table_array);

    for(int i = 0; i < array_length(ast); ++i) {
        //c_gen_declaration(&code, ast[i]);
    }

    catstring_to_file("test/generated_out.c", code);
}