#include "ast.h"
#include "utils/allocator.h"
#include <stdio.h>

static int32_t
ast_new_id() {
    static int32_t id = 0;
    return id++;
}

static int32_t
scope_new_id() {
    static int32_t id = 1;
    return id++;
}

Light_Scope* 
light_scope_new(Light_Ast* creator_node, Light_Scope* parent, uint32_t flags) {
    Light_Scope* scope = light_alloc(sizeof(Light_Scope));
    
    scope->id = scope_new_id();
    scope->decl_count = 0;
    scope->creator_node = creator_node;
    scope->parent = parent;
    scope->flags = flags;

    return scope;
}

Light_Ast* 
ast_new_typedef(Light_Scope* scope, struct Light_Type_t* type, Light_Token* name) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_TYPEDEF;
    result->scope_at = scope;
    result->type = 0;
    result->flags = 0;
    result->id = ast_new_id();

    result->decl_typedef.name = name;
    result->decl_typedef.type_referenced = type;

    return result;
}


// Print

s32 
ast_print_type(Light_Type* type) {
    switch(type->kind) {
        case TYPE_KIND_PRIMITIVE:{
            switch(type->primitive) {
                case TYPE_PRIMITIVE_VOID:   printf("void"); break;
                case TYPE_PRIMITIVE_S8:     printf("s8"); break;
                case TYPE_PRIMITIVE_S16:    printf("s16"); break;
                case TYPE_PRIMITIVE_S32:    printf("s32"); break;
                case TYPE_PRIMITIVE_S64:    printf("s64"); break;
                case TYPE_PRIMITIVE_U8:     printf("u8"); break;
                case TYPE_PRIMITIVE_U16:    printf("u16"); break;
                case TYPE_PRIMITIVE_U32:    printf("u32"); break;
                case TYPE_PRIMITIVE_U64:    printf("u64"); break;
                case TYPE_PRIMITIVE_R32:    printf("r32"); break;
                case TYPE_PRIMITIVE_R64:    printf("r64"); break;
                case TYPE_PRIMITIVE_BOOL:   printf("bool"); break;
                default: printf("<invalid primitive type>"); break;
            }
        } break;
        case TYPE_KIND_POINTER:{
            printf("^");
            ast_print_type(type->pointer_to);
        } break;
        case TYPE_KIND_ALIAS:{
            printf("%.*s", type->alias.name->length, type->alias.name->data);
        } break;
        case TYPE_KIND_ARRAY:{
            printf("[%lu]", type->array_info.dimension);
            ast_print_type(type->array_info.array_of);
        } break;
        case TYPE_KIND_FUNCTION:{
            printf("(");
            for(s32 i = 0; i < type->function.arguments_count; ++i) {
                ast_print_type(type->function.arguments_type[i]);
            }
            printf(") -> ");
            ast_print_type(type->function.return_type);
        } break;
        case TYPE_KIND_STRUCT:{
            printf("struct {");
            for(s32 i = 0; i < type->struct_info.fields_count; ++i) {
                ast_print_type(type->struct_info.fields_types[i]);
                printf("; ");
            }
            printf("}");
        } break;
        case TYPE_KIND_UNION: {
            printf("union {");
            for(s32 i = 0; i < type->union_info.fields_count; ++i) {
                ast_print_type(type->union_info.fields_types[i]);
                printf("; ");
            }
            printf("}");
        } break;
        case TYPE_KIND_NONE:
        default: printf("<unknown type>"); break;
    }
}