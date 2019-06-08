#include "top_typecheck.h"
#include "symbol_table.h"
#include "type.h"
#include "type_infer.h"
#include "utils/allocator.h"
#include "global_tables.h"
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

    if(!top_level) return error;

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
        typecheck_information_pass_decl(node, 0, &error);
    }
    if(error & TYPE_ERROR)
        return error;

    // Try to resolve everything in the infer table while
    // the table is shrinking
    u64 starting_length = array_length(global_infer_queue);
    while(starting_length > 0) {
        for(u64 i = 0; i < array_length(global_infer_queue); ++i) {
            Light_Ast* node = global_infer_queue[i];
            typecheck_information_pass_decl(node, 0, &error);
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
                *error |= decl_check_redefinition(scope, type->enumerator.fields_values[i], type->enumerator.fields_names[i]);
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
                type = typecheck_resolve_type_symbol_tables(type, flags, error);
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

            if(all_fields_internalized) {
                type = type_internalize(type);
                type = typecheck_resolve_type_symbol_tables(type, flags, error);
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
            if(type->enumerator.type_hint) {
                type->enumerator.type_hint = typecheck_resolve_type(scope, type->enumerator.type_hint, flags, error);
                if(type->enumerator.type_hint && type->enumerator.type_hint->flags & TYPE_FLAG_INTERNALIZED) {
                    type = type_internalize(type);
                    type = typecheck_resolve_type_symbol_tables(type, flags, error);
                }
            } else {
                type->size_bits = 32;
                type->flags |= TYPE_FLAG_SIZE_RESOLVED;
                type = type_internalize(type);
                type = typecheck_resolve_type_symbol_tables(type, flags, error);
            }
        } break;
        case TYPE_KIND_ALIAS: {
            if(!type->alias.alias_to) {
                Light_Ast* decl = type_decl_from_alias(scope, type, error);
                if(*error & TYPE_ERROR) {
                    typecheck_error_undeclared_identifier(type->alias.name);
                    return type;
                }
                if(decl->kind != AST_DECL_TYPEDEF) {
                    *error |= TYPE_ERROR;
                    typecheck_error_location(type->alias.name);
                    fprintf(stderr, "Type Error: invalid type name '%.*s'\n", TOKEN_STR(type->alias.name));
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
typecheck_information_pass_decl(Light_Ast* node, u32 flags, u32* error) {
    Light_Scope* scope = node->scope_at;

    switch(node->kind) {
        case AST_DECL_CONSTANT:{
            if(scope->level > 0 && !(node->flags & AST_FLAG_INFER_QUEUED)) {
                *error |= decl_check_redefinition(scope, node, node->decl_constant.name);
                if(*error & TYPE_ERROR) return;
            }

            // Infer the type of expression
            Light_Type* inferred = type_infer_expression(node->decl_constant.value, error);
            if(*error & TYPE_ERROR) return;
             if(!inferred) {
                typecheck_push_to_infer_queue(node);
                return;
            }

            if(node->decl_constant.type_info && !(node->decl_constant.type_info->flags & TYPE_FLAG_INTERNALIZED)) {
                node->decl_constant.type_info = typecheck_resolve_type(scope, node->decl_constant.type_info, flags, error);
                if(*error & TYPE_ERROR) return;
                if(TYPE_STRONG(node->decl_constant.type_info)) {
                    type_infer_propagate(node->decl_constant.type_info, node->decl_constant.value, error);
                }
            }

            // Infer to default type when type declaration is null
            Light_Type* type = type_infer_propagate(node->decl_constant.type_info, node->decl_constant.value, error);
            if(*error & TYPE_ERROR) return;

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
            if(scope->level > 0 && !(node->flags & AST_FLAG_INFER_QUEUED)) {
                *error |= decl_check_redefinition(scope, node, node->decl_variable.name);
                if(*error & TYPE_ERROR) return;
            }

            // Infer the type of expression
            if(node->decl_variable.assignment){
                Light_Type* inferred = type_infer_expression(node->decl_variable.assignment, error);
                if(*error & TYPE_ERROR) return;
                if(!inferred) {
                    typecheck_push_to_infer_queue(node);
                    return;
                }
            }
            
            if(node->decl_variable.type && !(node->decl_variable.type->flags & TYPE_FLAG_INTERNALIZED)) {
                node->decl_variable.type = typecheck_resolve_type(scope, node->decl_variable.type, flags, error);
                if(*error & TYPE_ERROR) return;
                if(TYPE_STRONG(node->decl_variable.type) && node->decl_variable.assignment) {
                    type_infer_propagate(node->decl_variable.type, node->decl_variable.assignment, error);
                } 
            }

            if(node->decl_variable.assignment) {
                // Infer to default type when type declaration is null
                Light_Type* type = type_infer_propagate(node->decl_variable.type, node->decl_variable.assignment, error);
                if(*error & TYPE_ERROR) return;
                
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
            if(scope->level > 0 && !(node->flags & AST_FLAG_INFER_QUEUED)) {
                *error |= decl_check_redefinition(scope, node, node->decl_proc.name);
                if(*error & TYPE_ERROR) return;
            }

            node->decl_proc.proc_type = typecheck_resolve_type(scope, node->decl_proc.proc_type, flags, error);
            if(*error & TYPE_ERROR) return;
            if(node->decl_proc.body) {
                typecheck_information_pass_command(node->decl_proc.body, flags, error);
                if(*error & TYPE_ERROR) return;
            }
            if(!(node->decl_proc.proc_type->flags & TYPE_FLAG_INTERNALIZED)) {
                typecheck_push_to_infer_queue(node);
            } else {
                typecheck_remove_from_infer_queue(node);
            }
        } break;
        case AST_DECL_TYPEDEF: {
            if(scope->level > 0 && !(node->flags & AST_FLAG_INFER_QUEUED)) {
                *error |= decl_check_redefinition(scope, node, node->decl_typedef.name);
                if(*error & TYPE_ERROR) return;
            }

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
        default: typecheck_information_pass_command(node, flags, error); break;
    }
}

void
typecheck_information_pass_command(Light_Ast* node, u32 flags, u32* error) {
    Light_Scope* scope = node->scope_at;

    switch(node->kind) {
        case AST_COMMAND_BLOCK: {
            Light_Scope* block_scope = node->comm_block.block_scope;
            if(!block_scope) return;
            if(block_scope->decl_count > 0) {
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
            }

            inferred_right = type_infer_expression(node->comm_assignment.rvalue, error);

            if(!inferred_left || !inferred_right) {
                typecheck_push_to_infer_queue(node);
                return;
            }
            if(TYPE_WEAK(inferred_right)) {
                inferred_right = type_infer_propagate(inferred_left, node->comm_assignment.rvalue, error);
            }

            // Type check
            if(inferred_left != inferred_right) {
                *error |= type_infer_type_mismatch_error(node->comm_assignment.op_token, inferred_left, inferred_right);
                fprintf(stderr, "in assignment command\n");
                return;
            }

            typecheck_remove_from_infer_queue(node);
        } break;
        case AST_COMMAND_BREAK:{
            // TODO(psv):
        } break;
        case AST_COMMAND_CONTINUE: {
            // TODO(psv):
        } break;
        case AST_COMMAND_RETURN: {
            Light_Ast* decl_proc = typecheck_decl_proc_from_scope(node->scope_at);
            if(!decl_proc) {
                typecheck_error_location(node->comm_return.token_return);
                fprintf(stderr, "Type error: return must be within a procedure but is not\n");
                *error |= TYPE_ERROR;
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
                if(TYPE_WEAK(node->comm_return.expression)) {
                    typecheck_push_to_infer_queue(node);
                    return;
                }
                expr_type = node->comm_return.expression->type;
            } else {
                expr_type = type_primitive_get(TYPE_PRIMITIVE_VOID);
            }

            // Type check
            if(expr_type != decl_proc->decl_proc.proc_type->function.return_type) {
                typecheck_error_location(node->comm_return.token_return);
                fprintf(stderr, "Type Error: procedure '%.*s' requires return type '", TOKEN_STR(decl_proc->decl_proc.name));
                ast_print_type(decl_proc->decl_proc.proc_type->function.return_type, LIGHT_AST_PRINT_STDERR);
                fprintf(stderr, "', but got '");
                ast_print_type(expr_type, LIGHT_AST_PRINT_STDERR);
                fprintf(stderr, "'\n");
                *error |= TYPE_ERROR;
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

                if(type != type_primitive_get(TYPE_PRIMITIVE_BOOL)) {
                    typecheck_error_location(node->comm_for.for_token);
                    fprintf(stderr, "Type Error: for command requires boolean type condition, but got '");
                    ast_print_type(type, LIGHT_AST_PRINT_STDERR);
                    fprintf(stderr, "'\n");
                    *error |= TYPE_ERROR;
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

            if(type != type_primitive_get(TYPE_PRIMITIVE_BOOL)) {
                typecheck_error_location(node->comm_if.if_token);
                fprintf(stderr, "Type Error: if command requires boolean type condition, but got '");
                ast_print_type(type, LIGHT_AST_PRINT_STDERR);
                fprintf(stderr, "'\n");
                *error |= TYPE_ERROR;
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

            if(type != type_primitive_get(TYPE_PRIMITIVE_BOOL)) {
                typecheck_error_location(node->comm_if.if_token);
                fprintf(stderr, "Type Error: while command requires boolean type condition, but got '");
                ast_print_type(type, LIGHT_AST_PRINT_STDERR);
                fprintf(stderr, "'\n");
                *error |= TYPE_ERROR;
                return;
            }

            typecheck_remove_from_infer_queue(node);
        } break;
        default: assert(0); break;
    }

}