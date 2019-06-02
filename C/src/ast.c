#include "ast.h"
#include "type.h"
#include "utils/allocator.h"
#include <stdio.h>
#include <assert.h>
#include <light_array.h>

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
    
    assert(parent);

    scope->id = scope_new_id();
    scope->decl_count = 0;
    scope->creator_node = creator_node;
    scope->parent = parent;
    scope->flags = flags;
    scope->level = parent->level + 1;

    return scope;
}

Light_Ast* 
ast_new_typedef(Light_Scope* scope, Light_Type* type, Light_Token* name) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_TYPEDEF;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_DECLARATION;
    result->id = ast_new_id();

    result->decl_typedef.name = name;
    result->decl_typedef.type_referenced = type;

    return result;
}

Light_Ast* 
ast_new_decl_variable(Light_Scope* scope, Light_Token* name, Light_Type* type, Light_Ast* expr, Light_Storage_Class storage, u32 flags) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_VARIABLE;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_DECLARATION;
    result->id = ast_new_id();

    result->decl_variable.name = name;
    result->decl_variable.flags = flags;
    result->decl_variable.storage_class = storage;
    result->decl_variable.type = type;
    result->decl_variable.assignment = expr;

    return result;
}

Light_Ast* 
ast_new_decl_constant(Light_Scope* scope, Light_Token* name, Light_Type* type, Light_Ast* expr, u32 flags) {
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_CONSTANT;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_DECLARATION;
    result->id = ast_new_id();

    result->decl_constant.name = name;
    result->decl_constant.flags = flags;
    result->decl_constant.type_info = type;
    result->decl_constant.value = expr;

    return result;
}

Light_Ast* 
ast_new_decl_procedure(
    Light_Scope* scope, Light_Token* name, Light_Ast* body, Light_Type* return_type, 
    Light_Scope* args_scope, Light_Ast_Decl_Variable** args, s32 args_count, u32 flags) 
{
    Light_Ast* result = light_alloc(sizeof(Light_Ast));

    result->kind = AST_DECL_PROCEDURE;
    result->scope_at = scope;
    result->type = type_primitive_get(TYPE_PRIMITIVE_VOID);
    result->flags = AST_FLAG_DECLARATION;
    result->id = ast_new_id();

    result->decl_proc.name = name;
    result->decl_proc.argument_count = args_count;
    result->decl_proc.arguments = args;
    result->decl_proc.body = body;
    result->decl_proc.arguments_scope = args_scope;
    result->decl_proc.flags = flags;
    result->decl_proc.return_type = return_type;
    result->decl_proc.proc_type = 0;

    return result;
}

// Print
#define TOKEN_STR(T) (T)->length, (T)->data

static FILE* ast_file_from_flags(u32 flags) {
    FILE* out = 0;
    if(flags & LIGHT_AST_PRINT_STDOUT)
        out = stdout;
    else if(flags & LIGHT_AST_PRINT_STDERR)
        out = stderr;
    else
        out = 0;
    return out;
}

s32
ast_print_expression(Light_Ast* expr) {
    printf("expr");
}

s32
ast_print_node(Light_Ast* node, u32 flags) {
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    switch(node->kind) {
        case AST_DECL_CONSTANT: {
            length += fprintf(out, "%.*s :", TOKEN_STR(node->decl_constant.name));
            if(node->decl_constant.type_info) {
                length += ast_print_type(node->decl_constant.type_info, flags);
            }
            length += fprintf(out, ":");
            length += ast_print_expression(node->decl_constant.value);
        }break;
        case AST_DECL_TYPEDEF:{
            length += fprintf(out, "%.*s -> ", TOKEN_STR(node->decl_typedef.name));
            length += ast_print_type(node->decl_typedef.type_referenced, flags);
        }break;
        case AST_DECL_VARIABLE:{
            length += fprintf(out, "%.*s :", TOKEN_STR(node->decl_variable.name));
            if(node->decl_variable.type) {
                length += ast_print_type(node->decl_variable.type, flags);
            }
            if(node->decl_variable.assignment) {
                length += fprintf(out, " = ");
                length += ast_print_expression(node->decl_variable.assignment);
            }
        }break;
        case AST_DECL_PROCEDURE:{
            length += fprintf(out, "%.*s :: (", TOKEN_STR(node->decl_proc.name));
            for(s32 i = 0; i < node->decl_proc.argument_count; ++i) {
                length += ast_print_node((Light_Ast*)node->decl_proc.arguments[i], flags);
            }
            length += fprintf(out, ") -> ");
            length += ast_print_type(node->decl_proc.return_type, flags);
            if(node->decl_proc.body) {
                length += ast_print_node(node->decl_proc.body, flags);
            } else {
                length += fprintf(out, ";");
            }
        }break;
        default: length += fprintf(out, "<invalid node>"); break;
    }

    return length;
}

s32 
ast_print_type(Light_Type* type, u32 flags) {
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;

    if(!type) return 0;
    switch(type->kind) {
        case TYPE_KIND_PRIMITIVE:{
            switch(type->primitive) {
                case TYPE_PRIMITIVE_VOID:   length += fprintf(out, "void"); break;
                case TYPE_PRIMITIVE_S8:     length += fprintf(out, "s8"); break;
                case TYPE_PRIMITIVE_S16:    length += fprintf(out, "s16"); break;
                case TYPE_PRIMITIVE_S32:    length += fprintf(out, "s32"); break;
                case TYPE_PRIMITIVE_S64:    length += fprintf(out, "s64"); break;
                case TYPE_PRIMITIVE_U8:     length += fprintf(out, "u8"); break;
                case TYPE_PRIMITIVE_U16:    length += fprintf(out, "u16"); break;
                case TYPE_PRIMITIVE_U32:    length += fprintf(out, "u32"); break;
                case TYPE_PRIMITIVE_U64:    length += fprintf(out, "u64"); break;
                case TYPE_PRIMITIVE_R32:    length += fprintf(out, "r32"); break;
                case TYPE_PRIMITIVE_R64:    length += fprintf(out, "r64"); break;
                case TYPE_PRIMITIVE_BOOL:   length += fprintf(out, "bool"); break;
                default: length += fprintf(out, "<invalid primitive type>"); break;
            }
        } break;
        case TYPE_KIND_POINTER:{
            length += fprintf(out, "^");
            length += ast_print_type(type->pointer_to, flags);
        } break;
        case TYPE_KIND_ALIAS:{
            length += fprintf(out, "%.*s", type->alias.name->length, type->alias.name->data);
        } break;
        case TYPE_KIND_ARRAY:{
            length += fprintf(out, "[%lu]", type->array_info.dimension);
            length += ast_print_type(type->array_info.array_of, flags);
        } break;
        case TYPE_KIND_FUNCTION:{
            length += fprintf(out, "(");
            for(s32 i = 0; i < type->function.arguments_count; ++i) {
                if(i != 0) length += fprintf(out, ", ");
                length += ast_print_type(type->function.arguments_type[i], flags);
            }
            length += fprintf(out, ") -> ");
            length += ast_print_type(type->function.return_type, flags);
        } break;
        case TYPE_KIND_STRUCT:{
            length += fprintf(out, "struct { ");
            for(s32 i = 0; i < type->struct_info.fields_count; ++i) {
                Light_Ast* field = type->struct_info.fields[i];
                length += ast_print_node(field, flags);
                length += fprintf(out, "; ");
            }
            length += fprintf(out, "}");
        } break;
        case TYPE_KIND_UNION: {
            length += fprintf(out, "union { ");
            for(s32 i = 0; i < type->union_info.fields_count; ++i) {
                Light_Ast* field = type->union_info.fields[i];
                length += ast_print_node(field, flags);
                length += fprintf(out, "; ");
            }
            length += fprintf(out, "}");
        } break;
        case TYPE_KIND_ENUM: {
            length += fprintf(out, "enum ");
            length += ast_print_type(type->enumerator.type_hint, flags);
            length += fprintf(out, "{ ");
            for(s32 i = 0; i < type->enumerator.field_count; ++i) {
                if(i != 0) length += fprintf(out, ", ");
                length += fprintf(out, "%.*s", type->enumerator.fields_names[i]->length, type->enumerator.fields_names[i]->data);
            }
            length += fprintf(out, " }");
        } break;
        case TYPE_KIND_NONE:
        default: length += fprintf(out, "<unknown type>"); break;
    }
}

s32
ast_print(Light_Ast** ast, u32 flags) {
    FILE* out = ast_file_from_flags(flags);
    s32 length = 0;
    for(u64 i = 0; i < array_length(ast); ++i) {
        length += ast_print_node(ast[i], flags);
    }
    return length;
}