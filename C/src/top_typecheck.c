#include "top_typecheck.h"
#include "symbol_table.h"
#include "type.h"
#include "type_infer.h"
#include "utils/allocator.h"
#include "global_tables.h"
#include "eval.h"
#include "error.h"
#include <light_array.h>
#include <assert.h>

#define TOKEN_STR(T) (T)->length, (T)->data

void typecheck_information_pass_decl(Light_Ast* node, u32 flags, u32* error);
void typecheck_information_pass_command(Light_Ast* node, u32 flags, u32* error);

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

static Light_Ast* 
typecheck_decl_proc_from_scope(Light_Scope* scope) {
    while(scope) {
        if(scope->flags & SCOPE_PROCEDURE_BODY) {
            assert(scope->creator_node->kind == AST_DECL_PROCEDURE);
            return scope->creator_node;
        }
        scope = scope->parent;
    }
    return 0;
}

// Return value 0 means not inside a loop
static s32
typecheck_loop_scope_level(Light_Scope* scope) {
    s32 level = 0;
    while(scope) {
        if(scope->flags & SCOPE_LOOP) {
            level++;
        } else {
            // we gotta get at least to the global scope
        }
        scope = scope->parent;
    }
    return level;
}

Light_Type_Error
decl_check_redefinition(Light_Scope* scope, Light_Ast* node, Light_Token* token) {
    Light_Symbol s = {0};
    s.token = token;
    s.decl = node;

    Symbol_Table* symbol_table = (Symbol_Table*)scope->symb_table;

    s32 index = 0;
    if(symbol_table_entry_exist(symbol_table, s, &index, 0)) {
        Light_Symbol decl_symbol = symbol_table_get(symbol_table, index);
        if(decl_symbol.decl == node) return TYPE_OK;
        type_error_location(token);
        fprintf(stderr, "Type error: redeclaration of name '%.*s'\n", TOKEN_STR(token));
        fprintf(stderr, "  - previously declared at: ");
        type_error_location(decl_symbol.token);
        fprintf(stderr, "\n");
        return TYPE_ERROR;
    } else {
        symbol_table_add(symbol_table, s, 0);
    }

    return TYPE_OK;
}

Light_Type_Error 
top_typecheck(Light_Ast** top_level, Light_Scope* global_scope) {
    Light_Type_Error error = TYPE_OK;

    if(!top_level) return error;

    global_scope->symb_table = light_alloc(sizeof(Symbol_Table));
    symbol_table_new((Symbol_Table*)global_scope->symb_table, (global_scope->decl_count + 4) * 8);

    // Check redefinition at top level
    for(u64 i = 0; i < array_length(top_level); ++i) {
        Light_Ast* node = top_level[i];
        switch(node->kind) {
            case AST_DECL_PROCEDURE:{
                error |= decl_check_redefinition(global_scope, node, node->decl_proc.name);
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
        typecheck_information_pass_decl(node, 0, (u32*)&error);
    }
    if(error & TYPE_ERROR)
        return error;

    // Try to resolve everything in the infer table while
    // the table is shrinking
    u64 starting_length = array_length(global_infer_queue);
    while(starting_length > 0) {
        for(u64 i = 0; i < array_length(global_infer_queue); ++i) {
            Light_Ast* node = global_infer_queue[i];
            typecheck_information_pass_decl(node, 0, (u32*)&error);
            if(error & TYPE_ERROR) break;
        }

        if(error & TYPE_ERROR)
            break;

        if(array_length(global_infer_queue) == starting_length) {
            type_error(&error, 0, "circular dependencies\n");
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

    while(scope){

        if(!scope->symb_table) {
            scope = scope->parent;
            continue;
        }
        
        s32 index = 0;
        if(symbol_table_entry_exist((Symbol_Table*)scope->symb_table, s, &index, 0)) {
            s = symbol_table_get((Symbol_Table*)scope->symb_table, index);
            break;
        }
        scope = scope->parent;
    }
    if(!s.decl)
        *error |= TYPE_ERROR;
    return s.decl;
}

static Light_Type*
typecheck_resolve_type_symbol_tables(Light_Type* type, u32 flags, u32* error) {
    switch(type->kind) {
        case TYPE_KIND_STRUCT:{
            Light_Scope* scope = type->struct_info.struct_scope;
            if(!scope->symb_table) {
                scope->symb_table = light_alloc(sizeof(Symbol_Table));
                symbol_table_new(scope->symb_table, (scope->decl_count + 4) * 8);
            }
            for(s32 i = 0; i < type->struct_info.fields_count; ++i) {
                *error |= decl_check_redefinition(scope, type->struct_info.fields[i], type->struct_info.fields[i]->decl_variable.name);
            }
        } break;
        case TYPE_KIND_UNION:{
            Light_Scope* scope = type->union_info.union_scope;
            if(!scope->symb_table) {
                scope->symb_table = light_alloc(sizeof(Symbol_Table));
                symbol_table_new(scope->symb_table, (scope->decl_count + 4) * 8);
            }
            for(s32 i = 0; i < type->union_info.fields_count; ++i) {
                *error |= decl_check_redefinition(scope, type->union_info.fields[i], type->union_info.fields[i]->decl_variable.name);
            }
        } break;
        case TYPE_KIND_ENUM:{
            Light_Scope* scope = type->enumerator.enum_scope;
            if(!scope->symb_table) {
                scope->symb_table = light_alloc(sizeof(Symbol_Table));
                symbol_table_new(scope->symb_table, (scope->decl_count + 4) * 8);
            }
            for(s32 i = 0; i < type->enumerator.field_count; ++i) {
                Light_Ast* field = type->enumerator.fields[i];
                *error |= decl_check_redefinition(scope, field, field->decl_constant.name);
            }
        } break;
        default: assert(0); break;
    }
    return type;
}

// This function tries to internalize a type if possible
//
// Returns TYPE_ERROR in 'error' if undeclared type name is found.
// Returns the type internalized in case the function internalizes it.
//
// Strenghten type if not already (removes weak type flag)
Light_Type*
typecheck_resolve_type(Light_Scope* scope, Light_Type* type, u32 flags, u32* error) {
    if(type->flags & TYPE_FLAG_INTERNALIZED) return type;

    switch(type->kind) {
        case TYPE_KIND_PRIMITIVE: {
            assert(type->flags & TYPE_FLAG_WEAK);
        } break;
        case TYPE_KIND_POINTER: {
            if(type->pointer_to->kind == TYPE_KIND_ALIAS) {

                // @Important same scope as the declaration, otherwise it
                // won't work, since the scope from the expression is not
                // unique.
                Light_Ast* type_decl_from_name = type_infer_decl_from_name(scope, type->pointer_to->alias.name);
                if(!type_decl_from_name) {
                    type_error_undeclared_identifier(error, type->pointer_to->alias.name);
                    return 0;
                }
                type->pointer_to->alias.scope = type_decl_from_name->scope_at;


                // flagged as unresolved, meaning the alias is not yet internalized, but
                // it is considered so in order to do recursive referencing
                type->pointer_to->flags |= TYPE_FLAG_UNRESOLVED;
                type->pointer_to = type_internalize(type->pointer_to);

                Light_Type* tt = type_internalize(type);
                Light_Ast* decl = type_decl_from_alias(scope, tt->pointer_to, error);
                if(!decl || (*error & TYPE_ERROR)) {
                    type_error_undeclared_identifier(error, tt->pointer_to->alias.name);
                    return 0;
                }
                type = tt;
            } else {
                type->pointer_to = typecheck_resolve_type(scope, type->pointer_to, flags, error);
                if(type->pointer_to && type->pointer_to->flags & TYPE_FLAG_INTERNALIZED) {
                    type = type_internalize(type);
                }
            }
        } break;
        case TYPE_KIND_ARRAY: {
            // Check array type
            type->array_info.array_of = typecheck_resolve_type(scope, type->array_info.array_of, flags, error);
            if(*error & TYPE_ERROR) return type;
            if(TYPE_WEAK(type->array_info.array_of)) return type;

            // Infer type of dimension
            Light_Type* dim_type = type_infer_expression(type->array_info.const_expr, error);
            if(*error & TYPE_ERROR) return type;
            if(!dim_type) return type;

            // Make sure the type is not weak
            if(TYPE_WEAK(dim_type)) type_infer_propagate(0, type->array_info.const_expr, error);
            if(*error & TYPE_ERROR) return type;

            // Array dimension must be an integer
            if(!type_primitive_int(dim_type)) {
                type_error(error, type->array_info.token_array, "Type Error: Array dimension must be an integer type constant expression, given '");
                ast_print_type(dim_type, LIGHT_AST_PRINT_STDERR, 0);
                fprintf(stderr, "'\n");
                return type;
            }

            // Array dimension must be constant
            if(!eval_expr_is_constant(type->array_info.const_expr, EVAL_ERROR_REPORT, error)) {
                fprintf(stderr, "  could not resolve array type, dimension is not a constant integer\n");
                return type;
            }

            // Evaluate dimension
            s64 dimension_value = eval_expr_constant_int(type->array_info.const_expr, error);
            if(*error & TYPE_ERROR) return type;
            if(dimension_value < 0) {
                type_error(error, type->array_info.token_array, 
                    "array dimension cannot be negative, given '%lld'\n", dimension_value);
                *error |= TYPE_ERROR;
                return type;
            }

            type->size_bits = type->array_info.array_of->size_bits * dimension_value;
            type->flags |= TYPE_FLAG_SIZE_RESOLVED;
            type->array_info.dimension_evaluated = true;
            type->array_info.dimension = dimension_value;
            type = type_internalize(type);
        } break;
        case TYPE_KIND_STRUCT: {
            bool all_fields_internalized = true;

            for(s32 i = 0; i < type->struct_info.fields_count; ++i) {
                Light_Ast* field = type->struct_info.fields[i];

                // When there is an assignment
                if(field->decl_variable.assignment) {
                    // Propagate the internalized type to the assignment expression
                    field->decl_variable.assignment->type = 
                        type_infer_expression(field->decl_variable.assignment, error);
                    if(!field->decl_variable.type) {
                        // Infer variable type from expression
                        field->decl_variable.assignment->type = 
                            type_infer_propagate(0, field->decl_variable.assignment, error);
                        if(*error & TYPE_ERROR || !field->decl_variable.assignment->type) {
                            all_fields_internalized = false;
                            // do not attemp to continue inferring the type of this field
                            continue;
                        }
                        field->decl_variable.type = field->decl_variable.assignment->type;
                    }
                }

                Light_Type* field_type = typecheck_resolve_type(scope, field->decl_variable.type, flags, error);
                if(!field_type || (*error & TYPE_ERROR)) return type;

                field->decl_variable.type = field_type;
                if(field_type && !(field_type->flags & TYPE_FLAG_INTERNALIZED)) {
                    all_fields_internalized = false;
                } else {
                    if(field->decl_variable.assignment) {
                        field->decl_variable.assignment->type = 
                            type_infer_propagate(field_type, field->decl_variable.assignment, error);
                        if(!type_check_equality(field_type, field->decl_variable.assignment->type)) {
                            // Type does not match the assignment
                            type_error(error, field->decl_variable.name, 
                                "type mismatch in struct field declaration. '");
                            ast_print_type(field->decl_variable.assignment->type, LIGHT_AST_PRINT_STDERR, 0);
                            fprintf(stderr, "' vs '");
                            ast_print_type(field_type, LIGHT_AST_PRINT_STDERR, 0);
                            fprintf(stderr, "'\n");
                            all_fields_internalized = false;
                        }
                    }
                }
            }

            if(!all_fields_internalized)
                return type;

            // Calculate sizes and offsets for every field
            type->struct_info.offset_bits = array_new(s32);
            s32 size_bits = 0;
            s32 offset_bits = 0;
            s32 struct_alignment_bytes = type->struct_info.alignment_bytes;
            for(s32 i = 0; i < type->struct_info.fields_count; ++i) {
                s32 field_type_size_bits = type->struct_info.fields[i]->decl_variable.type->size_bits;
                // align to whatever size bits of the current field is
                offset_bits += (offset_bits % field_type_size_bits);
                
                array_push(type->struct_info.offset_bits, offset_bits);
                offset_bits += field_type_size_bits;
            }

            // @Important TODO(psv): default alignment is 4, but we gotta
            // support smaller ones
            if(struct_alignment_bytes == 0) struct_alignment_bytes = 4; // default alignment is 32 bit/4 byte
            size_bits = offset_bits + (offset_bits % (struct_alignment_bytes * 8));
            type->size_bits = size_bits;
            type->struct_info.size_bits = size_bits;
            type->struct_info.alignment_bytes = struct_alignment_bytes;
            type->flags |= TYPE_FLAG_SIZE_RESOLVED;

            if(all_fields_internalized) {
                // It is important that we check redefinitions before we internalize
                // since we don't want to check it again and get a redefinition error.
                type = typecheck_resolve_type_symbol_tables(type, flags, error);
                type = type_internalize(type);
            }
        } break;
        case TYPE_KIND_UNION: {
            bool all_fields_internalized = true;

            for(s32 i = 0; i < type->union_info.fields_count; ++i) {
                Light_Ast* field = type->union_info.fields[i];
                Light_Type* field_type = typecheck_resolve_type(scope, field->decl_variable.type, flags, error);
                field->decl_variable.type = field_type;
                if(field_type && !(field_type->flags & TYPE_FLAG_INTERNALIZED)) {
                    all_fields_internalized = false;
                }
            }

            if(!all_fields_internalized)
                return type;

            // Calculate sizes for every field
            s32 max_size = 0;
            s32 size_bits = 0;
            s32 offset_bits = 0;
            s32 union_alignment_bytes = type->union_info.alignment_bytes;
            for(s32 i = 0; i < type->union_info.fields_count; ++i) {
                s32 field_type_size_bits = type->union_info.fields[i]->decl_variable.type->size_bits;
                if(field_type_size_bits > max_size) max_size = field_type_size_bits;
            }

            // TODO(psv): consider alignment here
            size_bits = max_size;
            type->size_bits = size_bits;
            type->union_info.size_bits = size_bits;
            type->union_info.alignment_bytes = 4; // TODO(psv): alignment
            type->flags |= TYPE_FLAG_SIZE_RESOLVED;

            if(all_fields_internalized) {
                type = typecheck_resolve_type_symbol_tables(type, flags, error);
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
            type->function.return_type = typecheck_resolve_type(scope, type->function.return_type, flags, error);
            if(all_fields_internalized && type->function.return_type->flags & TYPE_FLAG_INTERNALIZED) {
                type = type_internalize(type);
            }
        } break;
        case TYPE_KIND_ENUM: {
            // Create evaluated values array if there isn't one already
            if(!type->enumerator.evaluated_values) {
                type->enumerator.evaluated_values = array_new_len(s64, type->enumerator.field_count);
            }

            if(type->enumerator.type_hint) {
                type->enumerator.type_hint = typecheck_resolve_type(scope, type->enumerator.type_hint, flags, error);
                if(type->enumerator.type_hint && type->enumerator.type_hint->flags & TYPE_FLAG_INTERNALIZED) {
                    type = type_internalize(type);
                    type = typecheck_resolve_type_symbol_tables(type, flags, error);
                }
            } else {
                bool all_fields_internalized = true;
                Light_Type* strongest_enum_type = 0;
                // First infer all expressions and whether or not they are integer types
                // Type checking against any strong type found
                for(s32 i = 0; i < type->enumerator.field_count; ++i) {
                    Light_Ast* field_value = type->enumerator.fields[i]->decl_constant.value;
                    Light_Token* field_name = type->enumerator.fields[i]->decl_constant.name;
                    if(!field_value) {
                        // We are going to infer the value later based on the others
                        continue;
                    }
                    if(field_value) {
                        Light_Type* field_type = type_infer_expression(field_value, error);
                        
                        if(!field_type) {
                            // When no error is returned there is still unresolved type names
                            all_fields_internalized = false;
                            continue;
                        }

                        if(!type_primitive_int(field_type)) {
                            all_fields_internalized = false;
                            type_error(error, field_name,
                                "enumeration requires integer fields, but field '%.*s' evaluated to '", 
                                    TOKEN_STR(field_name));
                            ast_print_type(field_type, LIGHT_AST_PRINT_STDERR, 0);
                            fprintf(stderr, "'\n");
                            continue;
                        }

                        if(field_type->flags & TYPE_FLAG_INTERNALIZED) {
                            if(!strongest_enum_type) {
                                // When an internalized
                                strongest_enum_type = field_type;
                            } else {
                                // Convert to the biggest type
                                strongest_enum_type = type_get_bigger(field_type, strongest_enum_type);
                            }
                        }
                    }
                }

                if(!all_fields_internalized) {
                    return type;
                }

                // Check if the expressions are constant and evaluate them aswell as
                // propagating their types, which could be any integer type
                s64 previous_value = -1;
                for(s32 i = 0; i < type->enumerator.field_count; ++i) {
                    Light_Ast* expr = type->enumerator.fields[i]->decl_constant.value;
                    Light_Token* field_name = type->enumerator.fields[i]->decl_constant.name;
                    if(!expr) {
                        // There is no expression, so the semantic is previous + 1
                        // even when it collides with another enum field, same as C.
                        previous_value += 1;
                        array_push(type->enumerator.evaluated_values, previous_value);
                        continue;
                    }

                    expr->type = type_infer_propagate(0, expr, error);

                    if(!eval_expr_is_constant(expr, 0, error)){
                        all_fields_internalized = false;
                        type_error(error, field_name,
                            "enumeration field '%.*s' must be constant\n", TOKEN_STR(field_name));
                    } else {
                        s64 v = eval_expr_constant_int(expr, error);
                        array_push(type->enumerator.evaluated_values, v);
                        previous_value = v;
                    }
                }

                if(all_fields_internalized) {
                    type->size_bits = 32;
                    type->flags |= TYPE_FLAG_SIZE_RESOLVED;
                    type = type_internalize(type);
                    type = typecheck_resolve_type_symbol_tables(type, flags, error);
                }
            }
        } break;
        case TYPE_KIND_ALIAS: {
            Light_Ast* decl = type_decl_from_alias(scope, type, error);
            if(!type->alias.alias_to) {
                if(*error & TYPE_ERROR) {
                    type_error_undeclared_identifier(error, type->alias.name);
                    return type;
                }
                if(decl->kind != AST_DECL_TYPEDEF) {
                    type_error(error, type->alias.name, "invalid type name '%.*s'\n", TOKEN_STR(type->alias.name));
                    return type;
                }
                if(!(decl->decl_typedef.type_referenced->flags & TYPE_FLAG_INTERNALIZED)) {
                    return type;
                }
                type = decl->decl_typedef.type_referenced;
            }
            assert(decl->kind == AST_DECL_TYPEDEF);

            if(type->alias.alias_to->flags & TYPE_FLAG_INTERNALIZED) {
                type = type_internalize(type);
            }
        } break;
        case TYPE_KIND_DIRECTIVE: {
            Light_Ast* directive_expr = type->directive->expr_directive.expr;
            Light_Type* inferred = type_infer_expression(directive_expr, error);
            if(!inferred || *error & TYPE_ERROR) return 0;
            return typecheck_resolve_type(directive_expr->scope_at, inferred, 0, error);
        } break;
        default: assert(0); break;
    }

    return type;
}

void
typecheck_information_pass_decl(Light_Ast* node, u32 flags, u32* decl_error) {
    Light_Scope* scope = node->scope_at;

    u32 error = 0;
    switch(node->kind) {
        case AST_DECL_CONSTANT:{
            if(scope->level > 0 && !(node->flags & AST_FLAG_INFER_QUEUED)) {
                error |= decl_check_redefinition(scope, node, node->decl_constant.name);
                if(error & TYPE_ERROR) { *decl_error |= error; return; }
            }

            // Infer the type of expression
            Light_Type* inferred = type_infer_expression(node->decl_constant.value, &error);
            if(error & TYPE_ERROR) { *decl_error |= error; return; }
             if(!inferred) {
                typecheck_push_to_infer_queue(node);
                return;
            }

            if(node->decl_constant.type_info && !(node->decl_constant.type_info->flags & TYPE_FLAG_INTERNALIZED)) {
                node->decl_constant.type_info = typecheck_resolve_type(scope, node->decl_constant.type_info, flags, &error);
                if(error & TYPE_ERROR) { *decl_error |= error; return; }
                if(TYPE_STRONG(node->decl_constant.type_info)) {
                    type_infer_propagate(node->decl_constant.type_info, node->decl_constant.value, &error);
                }
            }

            // Infer to default type when type declaration is null
            Light_Type* type = type_infer_propagate(node->decl_constant.type_info, node->decl_constant.value, &error);
            if(error & TYPE_ERROR) { *decl_error |= error; return; }

            if(!node->decl_constant.type_info) {
                // Constant type must be inferred
                node->decl_constant.type_info = type;
            } else {
                // Type check
                if(node->decl_constant.type_info != type) {
                    type_error_mismatch(&error, node->decl_constant.name, node->decl_constant.type_info, type);
                    fprintf(stderr, " in '%.*s' constant declaration\n", TOKEN_STR(node->decl_constant.name));
                    *decl_error |= error;
                    return;
                }
            }

            if(node->decl_constant.type_info->flags & TYPE_FLAG_INTERNALIZED) {
                if(!eval_expr_is_constant(node->decl_constant.value, EVAL_ERROR_REPORT, &error)) {
                    *decl_error |= error;
                    return;
                } else {
                    typecheck_remove_from_infer_queue(node);
                }
            } else {
                typecheck_push_to_infer_queue(node);
            }
        } break;
        case AST_DECL_VARIABLE: {
            if(scope->level > 0 && !(node->flags & AST_FLAG_INFER_QUEUED)) {
                error |= decl_check_redefinition(scope, node, node->decl_variable.name);
                if(error & TYPE_ERROR) { *decl_error |= error; return; }
            }

            // Infer the type of expression
            if(node->decl_variable.assignment){
                Light_Type* inferred = type_infer_expression(node->decl_variable.assignment, &error);
                if(error & TYPE_ERROR) { *decl_error |= error; return; }
                if(!inferred) {
                    typecheck_push_to_infer_queue(node);
                    return;
                }
            }
            
            if(node->decl_variable.type && !(node->decl_variable.type->flags & TYPE_FLAG_INTERNALIZED)) {
                node->decl_variable.type = typecheck_resolve_type(scope, node->decl_variable.type, flags, &error);
                if(error & TYPE_ERROR) { *decl_error |= error; return; }
                if(TYPE_STRONG(node->decl_variable.type) && node->decl_variable.assignment) {
                    type_infer_propagate(node->decl_variable.type, node->decl_variable.assignment, &error);
                    if(error & TYPE_ERROR) { *decl_error |= error; return; }
                } 
            }

            if(node->decl_variable.assignment) {
                // Infer to default type when type declaration is null
                Light_Type* type = type_infer_propagate(node->decl_variable.type, node->decl_variable.assignment, &error);
                if(error & TYPE_ERROR) { *decl_error |= error; return; }
                
                if(!node->decl_variable.type) {
                    node->decl_variable.type = type;
                } else {
                    // Type check
                    if(!type_check_equality(node->decl_variable.type, type)) {
                        type_error_mismatch(&error, node->decl_variable.name, node->decl_variable.type, type);
                        fprintf(stderr, " in '%.*s' variable declaration\n", TOKEN_STR(node->decl_variable.name));
                        *decl_error |= error;
                        return;
                    }
                }
            }
            if(node->decl_variable.type && node->decl_variable.type->flags & TYPE_FLAG_INTERNALIZED) {
                typecheck_remove_from_infer_queue(node);
                node->decl_variable.flags |= DECL_VARIABLE_FLAG_RESOLVED;

                // Cannot declare a variable of type void
                if(node->decl_variable.type == type_primitive_get(TYPE_PRIMITIVE_VOID)) {
                    type_error(decl_error, node->decl_variable.name, 
                        "cannot declare the variable '%.*s' with type 'void'\n", TOKEN_STR(node->decl_variable.name));
                    return;
                }
            } else {
                typecheck_push_to_infer_queue(node);
            }
        } break;
        case AST_DECL_PROCEDURE: {
            if(scope->level > 0 && !(node->flags & AST_FLAG_INFER_QUEUED)) {
                error |= decl_check_redefinition(scope, node, node->decl_proc.name);
                if(error & TYPE_ERROR) { *decl_error |= error; return; }
            }

            if(node->decl_proc.argument_count > 0) {
                Light_Scope* arg_scope = node->decl_proc.arguments_scope;
                if(!arg_scope->symb_table) {
                    arg_scope->symb_table = light_alloc(sizeof(Symbol_Table));
                    symbol_table_new(arg_scope->symb_table, (arg_scope->decl_count + 4) * 8);
                }
                for(s32 i = 0; i < node->decl_proc.argument_count; ++i) {
                    Light_Ast* var = node->decl_proc.arguments[i];
                    if(var->flags & DECL_VARIABLE_FLAG_RESOLVED) {
                        continue;
                    }
                    typecheck_information_pass_decl(var, flags, &error);
                    if(error & TYPE_ERROR) { *decl_error |= error; return; }
                }
            }

            node->decl_proc.proc_type = typecheck_resolve_type(scope, node->decl_proc.proc_type, flags, &error);
            if(error & TYPE_ERROR) { *decl_error |= error; return; }
            if(node->decl_proc.body) {
                typecheck_information_pass_command(node->decl_proc.body, flags, &error);
                if(error & TYPE_ERROR) { *decl_error |= error; return; }
            }
            if(!(node->decl_proc.proc_type->flags & TYPE_FLAG_INTERNALIZED)) {
                typecheck_push_to_infer_queue(node);
            } else {
                node->decl_proc.return_type = node->decl_proc.proc_type->function.return_type;
                typecheck_remove_from_infer_queue(node);

                if(node->decl_proc.name->data == (u8*)light_special_idents_table[LIGHT_SPECIAL_IDENT_MAIN].data) {
                    node->decl_proc.flags |= DECL_PROC_FLAG_MAIN;
                }
            }
        } break;
        case AST_DECL_TYPEDEF: {
            if(scope->level > 0 && !(node->flags & AST_FLAG_INFER_QUEUED)) {
                error |= decl_check_redefinition(scope, node, node->decl_typedef.name);
                if(error & TYPE_ERROR) { *decl_error |= error; return; }
            }

            Light_Type* referenced = node->decl_typedef.type_referenced;
            assert(referenced->kind == TYPE_KIND_ALIAS);

            Light_Type* type = typecheck_resolve_type(scope, referenced->alias.alias_to, flags, &error);
            if(error & TYPE_ERROR) { *decl_error |= error; return; }
            if(type->flags & TYPE_FLAG_INTERNALIZED) {
                // @PatchType
                Light_Type* alias_type = type_new_alias(node->scope_at, node->decl_typedef.name, type);
                alias_type->size_bits = type->size_bits;
                node->decl_typedef.type_referenced = type_internalize(alias_type);

                typecheck_remove_from_infer_queue(node);
            } else {
                // Put it on the infer queue
                typecheck_push_to_infer_queue(node);
            }
        } break;
        default: typecheck_information_pass_command(node, flags, &error); break;
    }
    *decl_error |= error;
}

void
typecheck_information_pass_command(Light_Ast* node, u32 flags, u32* error) {
    Light_Scope* scope = node->scope_at;

    switch(node->kind) {
        case AST_COMMAND_BLOCK: {
            Light_Scope* block_scope = node->comm_block.block_scope;
            if(!block_scope) return;
            if(!block_scope->symb_table && block_scope->decl_count > 0) {
                block_scope->symb_table = light_alloc(sizeof(Symbol_Table));
                symbol_table_new(block_scope->symb_table, (block_scope->decl_count + 4) * 8);
            }

            for(s32 i = 0; i < node->comm_block.command_count; ++i) {
                typecheck_information_pass_decl(node->comm_block.commands[i], flags, error);
            }
        } break;
        case AST_COMMAND_ASSIGNMENT:{
            Light_Type* inferred_left = 0;
            Light_Type* inferred_right = 0;
            // Infer the type of expression
            if(node->comm_assignment.lvalue){
                inferred_left = type_infer_expression(node->comm_assignment.lvalue, error);
                if(*error & TYPE_ERROR) return;
                if(!inferred_left) {
                    typecheck_push_to_infer_queue(node);
                    return;
                }

                if(!(node->comm_assignment.lvalue->flags & AST_FLAG_EXPRESSION_LVALUE)) {
                    type_error(error, node->comm_assignment.op_token, 
                        "assignment operation requires an lvalue expression as the assigned value\n");
                    return;
                }
            }

            inferred_right = type_infer_expression(node->comm_assignment.rvalue, error);

            if((!inferred_left && node->comm_assignment.lvalue) || !inferred_right) {
                typecheck_push_to_infer_queue(node);
                return;
            }
            if(TYPE_WEAK(inferred_right)) {
                inferred_right = type_infer_propagate(inferred_left, node->comm_assignment.rvalue, error);
            }

            // Type check
            if(inferred_left && !type_check_equality(inferred_left, inferred_right)) {
                type_error_mismatch(error, node->comm_assignment.op_token, inferred_left, inferred_right);
                fprintf(stderr, " in assignment\n");
                return;
            }

            typecheck_remove_from_infer_queue(node);
        } break;
        case AST_COMMAND_BREAK:{
            s32 level_deep = typecheck_loop_scope_level(node->scope_at);
            if(level_deep == 0) {
                // Not inside a loop
                type_error(error, node->comm_break.token_break, "break not inside a loop\n");
                return;
            }

            if(node->comm_break.level) {
                Light_Type* type = type_infer_expression(node->comm_break.level, error);
                if(*error & TYPE_ERROR) return;
                if(!type) typecheck_push_to_infer_queue(node);
                if(TYPE_WEAK(type)) type_infer_propagate(0, node->comm_break.level, error);
                if(*error & TYPE_ERROR) return;

                if(!type_primitive_int(type)) {
                    type_error(error, node->comm_break.token_break, "break expression must be of integer type, but got '\n");
                    ast_print_type(type, LIGHT_AST_PRINT_STDERR, 0);
                    fprintf(stderr, "'\n");
                    return;
                }

                if(eval_expr_is_constant(node->comm_break.level, EVAL_ERROR_REPORT, error)){
                    assert(*error == 0);
                    s64 value = eval_expr_constant_int(node->comm_break.level, error);
                    if(*error & TYPE_ERROR) return;
                    node->comm_break.level_evaluated = true;
                    node->comm_break.level_value = value;
                    if(value > level_deep) {
                        type_error(error, node->comm_break.token_break, 
                            "break command is not inside an iterative loop nested '%lld' deep\n", value);
                        return;
                    }

                    typecheck_remove_from_infer_queue(node);
                } else {
                    if(*error & TYPE_ERROR) return;
                    typecheck_push_to_infer_queue(node);
                }
            } else {
                node->comm_break.level_evaluated = true;
                node->comm_break.level_value = 1;
            }
        } break;
        case AST_COMMAND_CONTINUE: {
            s32 level_deep = typecheck_loop_scope_level(node->scope_at);
            if(level_deep == 0) {
                // Not inside a loop
                type_error(error, node->comm_break.token_break, "continue not inside a loop\n");
                return;
            }

            if(node->comm_continue.level) {
                Light_Type* type = type_infer_expression(node->comm_continue.level, error);
                if(*error & TYPE_ERROR) return;
                if(!type) typecheck_push_to_infer_queue(node);
                if(TYPE_WEAK(type)) type_infer_propagate(0, node->comm_continue.level, error);
                if(*error & TYPE_ERROR) return;

                if(!type_primitive_int(type)) {
                    type_error(error, node->comm_continue.token_continue, "continue expression must be of integer type, but got '\n");
                    ast_print_type(type, LIGHT_AST_PRINT_STDERR, 0);
                    fprintf(stderr, "'\n");
                    return;
                }

                if(eval_expr_is_constant(node->comm_continue.level, EVAL_ERROR_REPORT, error)){
                    assert(*error == 0);
                    s64 value = eval_expr_constant_int(node->comm_continue.level, error);
                    if(*error & TYPE_ERROR) return;
                    node->comm_continue.level_evaluated = true;
                    node->comm_continue.level_value = value;
                    if(value > level_deep) {
                        type_error(error, node->comm_continue.token_continue, 
                            "continue command is not inside an iterative loop nested '%lld' deep\n", value);
                        return;
                    }

                    typecheck_remove_from_infer_queue(node);
                } else {
                    if(*error & TYPE_ERROR) return;
                    typecheck_push_to_infer_queue(node);
                }
            } else {
                node->comm_continue.level_evaluated = true;
                node->comm_continue.level_value = 1;
            }
        } break;
        case AST_COMMAND_RETURN: {
            Light_Ast* decl_proc = typecheck_decl_proc_from_scope(node->scope_at);
            if(!decl_proc) {
                type_error(error, node->comm_return.token_return, "return must be within a procedure but is not\n");
                return;
            }

            decl_proc->decl_proc.proc_type = typecheck_resolve_type(scope, decl_proc->decl_proc.proc_type, flags, error);
            if(*error & TYPE_ERROR) return;

            if(TYPE_WEAK(decl_proc->decl_proc.proc_type)) {
                typecheck_push_to_infer_queue(node);
                return;
            }

            Light_Type* expr_type = 0;
            if(node->comm_return.expression) {
                Light_Type* type = type_infer_expression(node->comm_return.expression, error);
                if(*error & TYPE_ERROR) return;
                if(!type) {
                    typecheck_push_to_infer_queue(node);
                    return;
                }
                node->comm_return.expression->type = type_infer_propagate(decl_proc->decl_proc.proc_type->function.return_type, 
                    node->comm_return.expression, error);
                if(*error & TYPE_ERROR) return;
                if(!node->comm_return.expression->type || !(node->comm_return.expression->type->flags & TYPE_FLAG_INTERNALIZED)) {
                    typecheck_push_to_infer_queue(node);
                    return;
                }
                expr_type = node->comm_return.expression->type;
            } else {
                expr_type = type_primitive_get(TYPE_PRIMITIVE_VOID);
            }

            // Type check
            if(!type_check_equality(expr_type, decl_proc->decl_proc.proc_type->function.return_type)) {
                type_error(error, node->comm_return.token_return, 
                    "procedure '%.*s' requires return type '", TOKEN_STR(decl_proc->decl_proc.name));
                ast_print_type(decl_proc->decl_proc.proc_type->function.return_type, LIGHT_AST_PRINT_STDERR, 0);
                fprintf(stderr, "', but got '");
                ast_print_type(expr_type, LIGHT_AST_PRINT_STDERR, 0);
                fprintf(stderr, "'\n");
                return;
            }

            typecheck_remove_from_infer_queue(node);
        } break;
        case AST_COMMAND_FOR: {
            Light_Scope* for_scope = node->comm_for.for_scope;
            if(for_scope) {
                if(!for_scope->symb_table && for_scope->decl_count > 0) {
                    for_scope->symb_table = light_alloc(sizeof(Symbol_Table));
                    symbol_table_new(for_scope->symb_table, (for_scope->decl_count + 4) * 8);
                }
            }

            if(node->comm_for.prologue){
                for(u64 i = 0; i < array_length(node->comm_for.prologue); ++i) {
                    typecheck_information_pass_decl(node->comm_for.prologue[i], flags, error);
                }
            }
            if(node->comm_for.epilogue) {
                for(u64 i = 0; i < array_length(node->comm_for.epilogue); ++i) {
                    typecheck_information_pass_decl(node->comm_for.epilogue[i], flags, error);
                }
            }
            typecheck_information_pass_decl(node->comm_for.body, flags, error);
            if(*error & TYPE_ERROR) return;

            // Conditional check, must be boolean
            // When condition doesnt exist, assume true
            if(node->comm_for.condition) {
                Light_Type* type = type_infer_expression(node->comm_for.condition, error);
                if(*error & TYPE_ERROR) return;
                if(!type) {
                    typecheck_push_to_infer_queue(node);
                    return;
                }

                if(!type_check_equality(type, type_primitive_get(TYPE_PRIMITIVE_BOOL))) {
                    type_error(error, node->comm_for.for_token, 
                        "for command requires boolean type condition, but got '");
                    ast_print_type(type, LIGHT_AST_PRINT_STDERR, 0);
                    fprintf(stderr, "'\n");
                    return;
                }
            }

            typecheck_remove_from_infer_queue(node);
        } break;
        case AST_COMMAND_IF: {
            // Check if true path
            typecheck_information_pass_decl(node->comm_if.body_true, flags, error);
            // Check if false path
            if(node->comm_if.body_false) {
                typecheck_information_pass_decl(node->comm_if.body_false, flags, error);
            }
            if(*error & TYPE_ERROR) return;

            // Conditional check, must be boolean
            Light_Type* type = type_infer_expression(node->comm_return.expression, error);
            if(*error & TYPE_ERROR) return;
            if(!type) {
                typecheck_push_to_infer_queue(node);
                return;
            }

            if(!type_check_equality(type, type_primitive_get(TYPE_PRIMITIVE_BOOL))) {
                type_error(error, node->comm_if.if_token, 
                    "if command requires boolean type condition, but got '");
                ast_print_type(type, LIGHT_AST_PRINT_STDERR, 0);
                fprintf(stderr, "'\n");
                return;
            }

            typecheck_remove_from_infer_queue(node);
        } break;
        case AST_COMMAND_WHILE:{
            // Check body
            typecheck_information_pass_decl(node->comm_while.body, flags, error);
            if(*error & TYPE_ERROR) return;

            // Conditional check, must be boolean
            Light_Type* type = type_infer_expression(node->comm_return.expression, error);
            if(*error & TYPE_ERROR) return;
            if(!type) {
                typecheck_push_to_infer_queue(node);
                return;
            }

            if(!type_check_equality(type, type_primitive_get(TYPE_PRIMITIVE_BOOL))) {
                type_error(error, node->comm_while.while_token, 
                    "while command requires boolean type condition, but got '");
                ast_print_type(type, LIGHT_AST_PRINT_STDERR, 0);
                fprintf(stderr, "'\n");
                return;
            }

            typecheck_remove_from_infer_queue(node);
        } break;
        default: assert(0); break;
    }
}