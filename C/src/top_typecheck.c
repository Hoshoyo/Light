#include "top_typecheck.h"
#include "symbol_table.h"
#include "type.h"
#include "type_infer.h"
#include "utils/allocator.h"
#include "global_tables.h"
#include <light_array.h>
#include <assert.h>

#define TOKEN_STR(T) (T)->length, (T)->data

void typecheck_information_pass(Light_Ast* node, u32 flags, u32* error);

static bool
typecheck_push_to_infer_queue(Light_Ast* node) {
    if(node->flags & AST_FLAG_INFER_QUEUED)
        return false; // already in infer queue
    array_push(global_infer_queue, node);
    node->flags |= AST_FLAG_INFER_QUEUED;
    node->infer_queue_index = array_length(global_infer_queue) - 1;
    return true;
}

static bool
typecheck_remove_from_infer_queue(Light_Ast* node) {
    if(!(node->flags & AST_FLAG_INFER_QUEUED))
        return false; // not in infer queue

    global_infer_queue[array_length(global_infer_queue) - 1]->infer_queue_index = node->infer_queue_index;
    array_remove(global_infer_queue, node->infer_queue_index);
    node->flags &= ~(AST_FLAG_INFER_QUEUED);
    node->infer_queue_index = 0;
    return true;
}

static s32
typecheck_error_location(Light_Token* t) {
    if(!t) {
        return fprintf(stderr, "%s: ", t->filepath);
    } else {
        return fprintf(stderr, "%s:%d:%d: ", t->filepath, t->line + 1, t->column + 1);
    }
    return 0;
}

static s32
typecheck_error_undeclared_identifier(Light_Token* t) {
    s32 length = 0;
    length += typecheck_error_location(t);
    fprintf(stderr, "Type Error: undeclared identifier '%.*s'\n", TOKEN_STR(t));
}

static Light_Type_Check_Error
decl_check_redefinition(Light_Scope* scope, Light_Ast* node, Light_Token* token) {
    Light_Symbol s = {0};
    s.token = token;
    s.decl = node;

    Symbol_Table* symbol_table = (Symbol_Table*)scope->symb_table;

    s32 index = 0;
    if(symbol_table_entry_exist(symbol_table, s, &index, 0)) {
        Light_Symbol decl_symbol = symbol_table_get(symbol_table, index);
        typecheck_error_location(token);
        fprintf(stderr, "Type error: redeclaration of name '%.*s'\n", TOKEN_STR(token));
        fprintf(stderr, "  - previously declared at: ");
        typecheck_error_location(decl_symbol.token);
        fprintf(stderr, "\n");
        return TYPE_ERROR;
    } else {
        symbol_table_add(symbol_table, s, 0);
    }

    return TYPE_OK;
}

Light_Type_Check_Error 
top_typecheck(Light_Ast** top_level, Light_Scope* global_scope) {
    Light_Type_Check_Error error = TYPE_OK;

    global_scope->symb_table = light_alloc(sizeof(Symbol_Table));
    symbol_table_new((Symbol_Table*)global_scope->symb_table, (global_scope->decl_count + 4) * 8);

    // Check redefinition at top level
    for(u64 i = 0; i < array_length(top_level); ++i) {
        Light_Ast* node = top_level[i];
        switch(node->kind) {
            case AST_DECL_PROCEDURE:{
                if(node->decl_proc.body) {
                    error |= decl_check_redefinition(global_scope, node, node->decl_proc.name);
                }
            }break;
            case AST_DECL_CONSTANT:
                error |= decl_check_redefinition(global_scope, node, node->decl_constant.name);
                break;
            case AST_DECL_VARIABLE:
                error |= decl_check_redefinition(global_scope, node, node->decl_variable.name);
                break;
            case AST_DECL_TYPEDEF:
                error |= decl_check_redefinition(global_scope, node, node->decl_typedef.name);
                break;
            default: assert(0); break;
        }
    }

    // All symbols are loaded, perform normal typecheck
    for(u64 i = 0; i < array_length(top_level); ++i) {
        Light_Ast* node = top_level[i];
        typecheck_information_pass(node, 0, &error);
    }
    if(error & TYPE_ERROR)
        return error;

    u64 starting_length = array_length(global_infer_queue);
    while(starting_length > 0) {
        for(u64 i = 0; i < array_length(global_infer_queue); ++i) {
            Light_Ast* node = global_infer_queue[i];
            typecheck_information_pass(node, 0, &error);
        }

        if(error & TYPE_ERROR)
            break;

        if(array_length(global_infer_queue) == starting_length) {
            fprintf(stderr, "Type Error: circular dependencies\n");
            break;
        }
        starting_length = array_length(global_infer_queue);
    }

    return error;
}

static Light_Ast*
type_decl_from_alias(Light_Scope* scope, Light_Type* alias, u32* error) {
    Light_Symbol s = {0};
    s.token =  alias->alias.name;

    s32 index = 0;
    if(symbol_table_entry_exist((Symbol_Table*)scope->symb_table, s, &index, 0)) {
        s = symbol_table_get((Symbol_Table*)scope->symb_table, index);
    } else {
        *error |= TYPE_ERROR;
    }
    return s.decl;
}

// This function tries to internalize a type if possible
//
// Returns TYPE_ERROR in 'error' if undeclared type name is found.
// Returns the type internalized in case the function internalizes it.
//
// Strenghten type if not already (removes weak type flag)
static Light_Type*
typecheck_resolve_type(Light_Scope* scope, Light_Type* type, u32 flags, u32* error) {
    if(type->flags & TYPE_FLAG_INTERNALIZED) return type;

    switch(type->kind) {
        case TYPE_KIND_PRIMITIVE: {
            assert(type->flags & TYPE_FLAG_WEAK);
        } break;
        case TYPE_KIND_POINTER: {
            type->pointer_to = typecheck_resolve_type(scope, type->pointer_to, flags, error);
            if(type->pointer_to && type->pointer_to->flags & TYPE_FLAG_INTERNALIZED) {
                type = type_internalize(type);
            }
        } break;
        case TYPE_KIND_ARRAY: {
            // Check array type
            type->array_info.array_of = typecheck_resolve_type(scope, type->array_info.array_of, flags, error);
            // Check array dimension
            // TODO(psv): evaluate dimension
        } break;
        case TYPE_KIND_STRUCT: {
            bool all_fields_internalized = true;

            for(s32 i = 0; i < type->struct_info.fields_count; ++i) {
                Light_Ast* field = type->struct_info.fields[i];
                Light_Type* field_type = typecheck_resolve_type(scope, field->decl_variable.type, flags, error);
                field->decl_variable.type = field_type;
                if(field_type && !(field_type->flags & TYPE_FLAG_INTERNALIZED)) {
                    all_fields_internalized = false;
                }
            }

            if(all_fields_internalized) {
                type = type_internalize(type);
            }
        } break;
        case TYPE_KIND_UNION: {
            bool all_fields_internalized = true;

            for(s32 i = 0; i < type->union_info.fields_count; ++i) {
                Light_Ast* field = type->union_info.fields[i];
                Light_Type* field_type = typecheck_resolve_type(scope, field->decl_variable.type, flags, error);
                if(field_type && !(field_type->flags & TYPE_FLAG_INTERNALIZED)) {
                    all_fields_internalized = false;
                }
            }

            if(all_fields_internalized) {
                type = type_internalize(type);
            }
        } break;
        case TYPE_KIND_FUNCTION: {
            bool all_fields_internalized = true;
            for(s32 i = 0; i < type->function.arguments_count; ++i) {
                type->function.arguments_type[i] = typecheck_resolve_type(scope, type->function.arguments_type[i], flags, error);
                if(type->function.arguments_type[i] && !(type->function.arguments_type[i]->flags & TYPE_FLAG_INTERNALIZED)) {
                    all_fields_internalized = false;
                }
            }
            if(all_fields_internalized) {
                type = type_internalize(type);
            }
        } break;
        case TYPE_KIND_ENUM: {
            if(type->enumerator.type_hint) {
                type->enumerator.type_hint = typecheck_resolve_type(scope, type->enumerator.type_hint, flags, error);
                if(type->enumerator.type_hint && type->enumerator.type_hint->flags & TYPE_FLAG_INTERNALIZED) {
                    type = type_internalize(type);
                }
            }
        } break;
        case TYPE_KIND_ALIAS: {
            if(!type->alias.alias_to) {
                Light_Ast* decl = type_decl_from_alias(scope, type, error);
                if(*error & TYPE_ERROR) {
                    typecheck_error_undeclared_identifier(type->alias.name);
                    return type;
                }
                type->alias.alias_to = decl->decl_typedef.type_referenced;
            }
            if(type->alias.alias_to->flags & TYPE_FLAG_INTERNALIZED) {
                type = type_internalize(type);
            }
        } break;
        default: assert(0); break;
    }

    return type;
}

void
typecheck_information_pass(Light_Ast* node, u32 flags, u32* error) {
    Light_Scope* scope = node->scope_at;

    switch(node->kind) {
        case AST_DECL_CONSTANT:{
            // Infer the type of expression
            type_infer_expression(node->decl_constant.value, error);

            if(node->decl_constant.type_info && !(node->decl_constant.type_info->flags & TYPE_FLAG_INTERNALIZED)) {
                node->decl_constant.type_info = typecheck_resolve_type(scope, node->decl_constant.type_info, flags, error);
                if(TYPE_STRONG(node->decl_constant.type_info)) {
                    type_infer_propagate(node->decl_constant.type_info, node->decl_constant.value, error);
                }
            }

            // Infer to default type when type declaration is null
            Light_Type* type = type_infer_propagate(node->decl_constant.type_info, node->decl_constant.value, error);

            if(!node->decl_constant.type_info) {
                // Constant type must be inferred
                node->decl_constant.type_info = type;
            } else {
                // Type check
                if(node->decl_constant.type_info != type) {
                    *error |= type_infer_type_mismatch_error(node->decl_constant.name, node->decl_constant.type_info, type);
                    fprintf(stderr, " in '%.*s' constant declaration\n", TOKEN_STR(node->decl_constant.name));
                    return;
                }
            }

            if(node->decl_constant.type_info->flags & TYPE_FLAG_INTERNALIZED) {
                typecheck_remove_from_infer_queue(node);
            } else {
                typecheck_push_to_infer_queue(node);
            }

        } break;
        case AST_DECL_VARIABLE: {
            // Infer the type of expression
            if(node->decl_variable.assignment)
                type_infer_expression(node->decl_variable.assignment, error);
            
            if(node->decl_variable.type && !(node->decl_variable.type->flags & TYPE_FLAG_INTERNALIZED)) {
                node->decl_variable.type = typecheck_resolve_type(scope, node->decl_variable.type, flags, error);
                if(TYPE_STRONG(node->decl_variable.type) && node->decl_variable.assignment) {
                    type_infer_propagate(node->decl_variable.type, node->decl_variable.assignment, error);
                } 
            }

            if(node->decl_variable.assignment) {
                // Infer to default type when type declaration is null
                Light_Type* type = type_infer_propagate(node->decl_variable.type, node->decl_variable.assignment, error);
                
                if(!node->decl_variable.type) {
                    node->decl_variable.type = type;
                } else {
                    // Type check
                    if(node->decl_variable.type != type) {
                        *error |= type_infer_type_mismatch_error(node->decl_variable.name, node->decl_variable.type, type);
                        fprintf(stderr, " in '%.*s' variable declaration\n", TOKEN_STR(node->decl_variable.name));
                        return;
                    }
                }
            }
            if(node->decl_variable.type->flags & TYPE_FLAG_INTERNALIZED) {
                typecheck_remove_from_infer_queue(node);
            } else {
                typecheck_push_to_infer_queue(node);
            }
        } break;
        case AST_DECL_PROCEDURE: {

        } break;
        case AST_DECL_TYPEDEF: {
            node->decl_typedef.type_referenced = typecheck_resolve_type(scope, node->decl_typedef.type_referenced, flags, error);
            if(*error & TYPE_ERROR)
                return;
            if(node->decl_typedef.type_referenced->flags & TYPE_FLAG_INTERNALIZED) {
                typecheck_remove_from_infer_queue(node);
            } else {
                // Put it on the infer queue
                typecheck_push_to_infer_queue(node);
            }
        } break;
        default: break;
    }
}