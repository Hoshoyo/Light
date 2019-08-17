#include "toplevel.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "light_array.h"
#include "catstring.h"

static void emit_typed_declaration(catstring* buffer, Light_Type* type, Light_Token* name);

static void
emit_type_end(catstring* buffer, Light_Type* type) {
    switch(type->kind) {
        case TYPE_KIND_ARRAY:{
            catsprint(buffer, "[%u]", type->array_info.dimension);
            emit_type_end(buffer, type->array_info.array_of);
        } break;
        case TYPE_KIND_FUNCTION:{
            catsprint(buffer, ")(");
            for(int i = 0; i < type->function.arguments_count; ++i) {
                if(i > 0) catsprint(buffer, ", ");

                if(type->function.arguments_names) {
                    Light_Token arg_name = {0};
                    arg_name.type = TOKEN_IDENTIFIER;
                    arg_name.data = (u8*)(type->function.arguments_names[i]);
                    arg_name.length = type->function.arguments_names_length[i];
                    emit_typed_declaration(buffer, type->function.arguments_type[i], &arg_name);
                } else {
                    emit_typed_declaration(buffer, type->function.arguments_type[i], 0);
                }
            }
            catsprint(buffer, ")");
            emit_type_end(buffer, type->function.return_type);
        } break;
        case TYPE_KIND_POINTER:{
            emit_type_end(buffer, type->pointer_to);
        } break;

        case TYPE_KIND_ALIAS:
        case TYPE_KIND_ENUM:
        case TYPE_KIND_PRIMITIVE:
        case TYPE_KIND_STRUCT:
        case TYPE_KIND_UNION:
            break;

        case TYPE_KIND_NONE:
        default: assert(0); break;
    }
}

static void
emit_type_start(catstring* buffer, Light_Type* type) {
    switch(type->kind) {
        case TYPE_KIND_ALIAS:{
            catsprint_token(buffer, type->alias.name);
        } break;
        case TYPE_KIND_ARRAY:{
            emit_type_start(buffer, type->array_info.array_of);
        } break;
        case TYPE_KIND_FUNCTION:{
            emit_type_start(buffer, type->function.return_type);
            catsprint(buffer, "(*");
        } break;
        case TYPE_KIND_POINTER:{
            emit_type_start(buffer, type->pointer_to);
            catsprint(buffer, "*");
        } break;
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
        } break;
        case TYPE_KIND_STRUCT:{
            catsprint(buffer, "struct {");
            for(int i = 0; i < type->struct_info.fields_count; ++i) {
                Light_Ast* field = type->struct_info.fields[i];
                emit_typed_declaration(buffer, field->decl_variable.type, field->decl_variable.name);
                catsprint(buffer, ";\n");
            }
            catsprint(buffer, "}");
        } break;
        case TYPE_KIND_UNION: {
            catsprint(buffer, "union {");
            for(int i = 0; i < type->union_info.fields_count; ++i) {
                Light_Ast* field = type->union_info.fields[i];
                emit_typed_declaration(buffer, field->decl_variable.type, field->decl_variable.name);
                catsprint(buffer, ";\n");
            }
            catsprint(buffer, "}");
        } break;
        case TYPE_KIND_ENUM: break;
        default: assert(0); break;
    }
}

static void
emit_typed_declaration(catstring* buffer, Light_Type* type, Light_Token* name) {
    switch(type->kind) {
        case TYPE_KIND_ALIAS: {
            catsprint_token(buffer, type->alias.name);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
        } break;

        case TYPE_KIND_FUNCTION:
        case TYPE_KIND_POINTER:
        case TYPE_KIND_ARRAY:{
            emit_type_start(buffer, type);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
            emit_type_end(buffer, type);
        } break;

        case TYPE_KIND_PRIMITIVE:
        case TYPE_KIND_UNION:
        case TYPE_KIND_STRUCT:
        {
            emit_type_start(buffer, type);
            catsprint(buffer, " ");
            if(name)
                catsprint_token(buffer, name);
        } break;

        case TYPE_KIND_ENUM:
            break;
        case TYPE_KIND_NONE:
        default: assert(0); break;
    }
}

static void
emit_declaration(catstring* buffer, Light_Ast* node) {
    switch(node->kind) {
        case AST_DECL_PROCEDURE: {
        } break;
        case AST_DECL_VARIABLE: {
            emit_typed_declaration(buffer, node->decl_variable.type, node->decl_variable.name);
            catsprint(buffer, ";\n");
        } break;

        case AST_DECL_CONSTANT: // should not do anything, constant must be directly writted as an expression
        case AST_DECL_TYPEDEF:  // should not be handled here
        default: break;
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

    // Emit, in order, all type aliases
    //emit_forward_type_decl(&code, global_type_array);

    for(int i = 0; i < array_length(ast); ++i) {
        emit_declaration(&code, ast[i]);
        catsprint(&code, "\n");
    }

    catstring_to_file("test/generated_out.c", code);
}