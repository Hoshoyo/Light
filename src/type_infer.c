#include "type_infer.h"
#include "symbol_table.h"
#include "top_typecheck.h"
#include "eval.h"
#include "ast.h"
#include "error.h"
#include "utils/allocator.h"
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <light_array.h>

#define MAX(A, B) (((A) > (B)) ? A : B)
#define TOKEN_STR(T) (T)->length, (T)->data

static bool
type_cast_is_valid(Light_Type* from, Light_Type* to) {
    // Valid conversions:
    // number  -> number
    // ^T      -> ^U
    // ^T      -> integer
    // integer -> ^T
    // []T     -> ^U
    // func    -> ^T
    // enum    -> integer
    // integer -> enum
    assert(from->kind != TYPE_KIND_ALIAS && to->kind != TYPE_KIND_ALIAS);

    if(from == to) return true;

    if(type_primitive_numeric(from) && type_primitive_numeric(to)) {
        return true;
    }

    if(from->kind == TYPE_KIND_POINTER) {
        return type_primitive_int(to) || to->kind == TYPE_KIND_POINTER 
            || to->kind == TYPE_KIND_FUNCTION || to->kind == TYPE_KIND_ARRAY;
    }

    if(to->kind == TYPE_KIND_POINTER && type_primitive_int(from)) {
        return true;
    }

    if(from->kind == TYPE_KIND_ARRAY || from->kind == TYPE_KIND_FUNCTION) {
        return to->kind == TYPE_KIND_POINTER;
    }

    if(from->kind == TYPE_KIND_ENUM && type_primitive_int(to)) {
        return true;
    }

    if(type_primitive_int(from) && to->kind == TYPE_KIND_ENUM) {
        return true;
    }

    if(type_primitive_bool(to) && type_primitive_int(from) ||
        type_primitive_int(to) && type_primitive_bool(from))
    {
        return true;
    }

    return false;
}

Light_Ast*
type_infer_decl_from_name(Light_Scope* scope, Light_Token* name) {
    Light_Symbol s = {0};
    s.token = name;

    while(scope) {

        if(!scope->symb_table){
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
    return s.decl;
}

static Light_Type*
type_infer_propagate_literal_array(Light_Type* type, Light_Ast* expr, u32* error) {
    if(type && type->kind != TYPE_KIND_ARRAY) {
        type_error_mismatch(error, expr->expr_literal_array.token_array, 
            type, expr->type);
            fprintf(stderr, " in literal array\n");
        return 0;
    }

    if(type) {
        assert(TYPE_STRONG(type));
    }

    if(expr->expr_literal_array.raw_data) {
        if(type && type_alias_root(type)->kind == TYPE_KIND_POINTER) {
            expr->type = type;
        }
    } else {
        assert(expr->type);
        Light_Type* array_of_type = 0;
        if(type) {
            bool all_internalized = true;
            array_of_type = type->array_info.array_of;
            for(u64 i = 0; i < array_length(expr->expr_literal_array.array_exprs); ++i) {
                Light_Type* propagated = type_infer_propagate(array_of_type, expr->expr_literal_array.array_exprs[i], error);
                if(!propagated) {
                    all_internalized = false;
                    continue;
                }
                expr->expr_literal_array.array_exprs[i]->type = propagated;
                if(array_of_type && !type_check_equality(propagated, array_of_type)) {
                    type_error_mismatch(error, expr->expr_literal_array.token_array, 
                        propagated, array_of_type);
                    fprintf(stderr, " in literal array\n");
                    break;
                }
            }

            if(all_internalized) {
                Light_Type* t = type_new_array(0, array_of_type, expr->expr_literal_array.token_array);
                t->array_info.dimension_evaluated = true;
                t->array_info.dimension = array_length(expr->expr_literal_array.array_exprs);
                t->size_bits = t->array_info.dimension * array_of_type->size_bits;
                t->flags = TYPE_FLAG_SIZE_RESOLVED;
                t = type_internalize(t);
                if(!type_check_equality(t, type)) {
                    type_error_mismatch(error, expr->expr_literal_array.token_array, 
                        t, type);
                    fprintf(stderr, " in literal array\n");
                    expr->type = 0;
                } else {
                    expr->type = type;
                }
            } else {
                expr->type = 0;
            }
        } else {
            bool all_internalized = true;
            for(u64 i = 0; i < array_length(expr->expr_literal_array.array_exprs); ++i) {
                Light_Type* propagated = type_infer_propagate(array_of_type, expr->expr_literal_array.array_exprs[i], error);
                if(!propagated || !(propagated->flags & TYPE_FLAG_INTERNALIZED)) {
                    all_internalized = false;
                    continue;
                }
                expr->expr_literal_array.array_exprs[i]->type = propagated;

                if(!array_of_type) {
                    array_of_type = propagated;
                    continue;
                }
                
                if(!type_check_equality(propagated, array_of_type)) {
                    type_error_mismatch(error, expr->expr_literal_array.token_array, 
                        propagated, array_of_type);
                    fprintf(stderr, " in literal array\n");
                    break;
                }
            }
            if(all_internalized) {
                Light_Type* type = type_new_array(0, array_of_type, expr->expr_literal_array.token_array);
                type->array_info.dimension_evaluated = true;
                type->array_info.dimension = array_length(expr->expr_literal_array.array_exprs);
                type->size_bits = type->array_info.dimension * array_of_type->size_bits;
                type->flags = TYPE_FLAG_SIZE_RESOLVED;
                expr->type = type_internalize(type);
            } else {
                expr->type = 0;
            }
        }
    }

    return expr->type;
}

static Light_Type*
type_infer_propagate_literal_struct(Light_Type* type, Light_Ast* expr, u32* error) {
    if(type) {
        assert(TYPE_STRONG(type));
    }
    assert(expr->type);
    for(u64 i = 0; i < array_length(expr->expr_literal_struct.struct_exprs); ++i) {
        if(type) {
            type = type->struct_info.fields[i]->decl_variable.type;
        }
        if(expr->expr_literal_struct.named) {
            // If named, we have declarations instead of expressions,
            Light_Ast* assignment = expr->expr_literal_struct.struct_decls[i]->decl_variable.assignment;
            assert(assignment);
            type_infer_propagate(type, assignment, error);
        } else {
            // Otherwise we have normal expressions
            type_infer_propagate(type, expr->expr_literal_struct.struct_exprs[i], error);
        }
    }
    return expr->type;
}

static Light_Type* 
type_infer_propagate_literal_primitive(Light_Type* type, Light_Ast* expr, u32* error) {
    if(type) {
        assert(TYPE_STRONG(type));
    }
    assert(expr->type);

    switch(expr->expr_literal_primitive.type) {
        case LITERAL_BIN_INT:
        case LITERAL_DEC_SINT:
        case LITERAL_DEC_UINT:
        case LITERAL_HEX_INT:
        case LITERAL_CHAR:{
            if(!type) {
                // Default propagate integer is s32
                type = type_primitive_get(TYPE_PRIMITIVE_S32);
            }
            if(type_primitive_int(type) && type_primitive_int(expr->type)) {
                expr->type = type;
            }
        } break;

        case LITERAL_FLOAT:{
            if(!type) {
                // Default propagate integer is r32
                type = type_primitive_get(TYPE_PRIMITIVE_R32);
            }
            if(type_primitive_float(type) && type_primitive_float(expr->type)) {
                expr->type = type;
            }
        } break;

        case LITERAL_BOOL: {
            // Expression of type bool should not get here, since
            // is should already be a strong type by default
            assert(0);
        } break;
        case LITERAL_POINTER:{
            if(!type) {
                type = type_new_pointer(type_primitive_get(TYPE_PRIMITIVE_VOID));
            }
            if(type->kind == TYPE_KIND_POINTER){
                expr->type = type;
            }
        } break;
        default: assert(0); break;
    }

    eval_literal_primitive(expr);
    return expr->type;
}

static Light_Type* 
type_infer_propagate_binary(Light_Type* type, Light_Ast* expr, u32* error) {
    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:
        case OP_BINARY_MINUS:
        case OP_BINARY_MULT:
        case OP_BINARY_DIV:{
            Light_Type* left = type_infer_propagate(type, expr->expr_binary.left, error);
            Light_Type* right = type_infer_propagate(type, expr->expr_binary.right, error);
            
            if(type_check_equality(left, right)) {
                expr->type = left;
            }
        } break;

        case OP_BINARY_MOD:
        case OP_BINARY_AND:
        case OP_BINARY_OR:
        case OP_BINARY_XOR:
        case OP_BINARY_SHL:
        case OP_BINARY_SHR:{
            Light_Type* left = type_infer_propagate(type, expr->expr_binary.left, error);
            Light_Type* right = type_infer_propagate(type, expr->expr_binary.right, error);
            assert(type_primitive_int(left) && type_primitive_int(right));

            if(type_check_equality(left, right)) {
                expr->type = left;
            }
        } break;

        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE:
        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL: {
            assert(type_check_equality(type, type_primitive_get(TYPE_PRIMITIVE_BOOL)));
            Light_Type* left = type_infer_propagate(type, expr->expr_binary.left, error);
            Light_Type* right = type_infer_propagate(type, expr->expr_binary.right, error);
            assert(type_primitive_numeric(left) && type_primitive_numeric(right));
            
            expr->type = type;
        } break;

        case OP_BINARY_LOGIC_AND:
        case OP_BINARY_LOGIC_OR: {
            assert(type_check_equality(expr->expr_binary.left->type, type_primitive_get(TYPE_PRIMITIVE_BOOL)));
            assert(type_check_equality(expr->expr_binary.right->type, type_primitive_get(TYPE_PRIMITIVE_BOOL)));
            expr->type = type_primitive_get(TYPE_PRIMITIVE_BOOL);
        } break;

        case OP_BINARY_VECTOR_ACCESS:
            type_infer_propagate(type, expr->expr_binary.left, error);
            type_infer_propagate(0, expr->expr_binary.right, error);
            Light_Type* left_type = expr->expr_binary.left->type;
            if(left_type->kind == TYPE_KIND_ARRAY) {
                expr->type = left_type->array_info.array_of;
            } else {
                expr->type = left_type->pointer_to;
            }
            break;

        default: assert(0); break;
    }
    return expr->type;
}

static Light_Type* 
type_infer_propagate_dot(Light_Type* type, Light_Ast* expr, u32* error) {
    assert(expr->kind == AST_EXPRESSION_DOT);
    expr->type = type_infer_propagate(type, expr->expr_dot.left, error);
    return expr->type;
}

static Light_Type* 
type_infer_propagate_unary(Light_Type* type, Light_Ast* expr, u32* error) {
    switch(expr->expr_unary.op) {
        case OP_UNARY_ADDRESSOF:
        case OP_UNARY_CAST:
        case OP_UNARY_DEREFERENCE:
            // cast, addressof and dereference should already be strong
            assert(TYPE_STRONG(expr->type));
            return expr->type;
        case OP_UNARY_LOGIC_NOT:{
            expr->type = type_primitive_get(TYPE_PRIMITIVE_BOOL);
            return type_infer_propagate(expr->type, expr->expr_unary.operand, error);
        } break;
        case OP_UNARY_BITWISE_NOT:
        case OP_UNARY_MINUS:
        case OP_UNARY_PLUS:{
            expr->type = type_infer_propagate(type, expr->expr_unary.operand, error);
            return expr->type;
        } break;
        default: assert(0); break;
    }
    return 0;
}

Light_Type* 
type_infer_propagate(Light_Type* type, Light_Ast* expr, u32* error) {
    assert(expr->flags & AST_FLAG_EXPRESSION);

    if(expr->type && TYPE_STRONG(expr->type) && expr->type->flags & TYPE_FLAG_INTERNALIZED) {
        return expr->type;
    }

    switch(expr->kind) {
        case AST_EXPRESSION_LITERAL_ARRAY:
            return type_infer_propagate_literal_array(type, expr, error);
        case AST_EXPRESSION_LITERAL_STRUCT:
            return type_infer_propagate_literal_struct(type, expr, error);
        case AST_EXPRESSION_LITERAL_PRIMITIVE:
            return type_infer_propagate_literal_primitive(type, expr, error);
        case AST_EXPRESSION_BINARY:
            return type_infer_propagate_binary(type, expr, error);
        case AST_EXPRESSION_UNARY:
            return type_infer_propagate_unary(type, expr, error);
        case AST_EXPRESSION_DOT:
            return type_infer_propagate_dot(type, expr, error);
        case AST_EXPRESSION_PROCEDURE_CALL:
            // passthrough
        case AST_EXPRESSION_VARIABLE:
            assert(TYPE_STRONG(expr->type));
            //return expr->type;
            break;
        case AST_EXPRESSION_DIRECTIVE: {
            if(expr->expr_directive.type == EXPR_DIRECTIVE_RUN)
                type_infer_propagate(type, expr->expr_directive.expr, error);
            else   
                assert(0);
        } break;
        case AST_EXPRESSION_COMPILER_GENERATED:
            break;
        default: assert(0); break;
    }
    return 0;
}

static Light_Type*
type_infer_expr_variable(Light_Ast* expr, u32* error) {
    assert(expr->kind == AST_EXPRESSION_VARIABLE);
    Light_Ast* decl = type_infer_decl_from_name(expr->scope_at, expr->expr_variable.name);
    if(!decl) {
        type_error_undeclared_identifier(error, expr->expr_variable.name);
        return 0;
    }
    expr->expr_variable.decl = decl;

    switch(decl->kind) {
        case AST_DECL_CONSTANT:{
            return decl->decl_constant.type_info;
        } break;
        case AST_DECL_PROCEDURE:{
            expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
            if(decl->decl_proc.proc_type && decl->decl_proc.proc_type->flags & TYPE_FLAG_INTERNALIZED)
                return decl->decl_proc.proc_type;
        } break;
        case AST_DECL_VARIABLE:{
            expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
            if(decl->decl_variable.type && decl->decl_variable.type->flags & TYPE_FLAG_INTERNALIZED)
                return decl->decl_variable.type;
        } break;
        case AST_DECL_TYPEDEF:{
            Light_Type* type = decl->decl_typedef.type_referenced;
            type = type_alias_root(type);
            if(type && type->kind == TYPE_KIND_ENUM && expr->flags & AST_FLAG_ALLOW_BASE_ENUM) {
                return decl->decl_typedef.type_referenced;
            }
            if(!type) return 0;
            // Error, referencing a typename instead of a declaration
            type_error(error, expr->expr_variable.name, "referencing the typename '%.*s' as an rvalue\n", TOKEN_STR(expr->expr_variable.name));
        } break;
        default: assert(0); break;
    }
    return 0;
}

static Light_Type*
type_infer_expr_literal_struct(Light_Ast* expr, u32* error) {
    assert(expr->kind == AST_EXPRESSION_LITERAL_STRUCT);
    expr->flags |= AST_FLAG_EXPRESSION_LVALUE;

    if(expr->expr_literal_struct.name) {
        // If the struct has a name, typecheck against the type
        Light_Ast* decl = type_infer_decl_from_name(expr->scope_at, expr->expr_literal_struct.name);
        if(!decl) {
            type_error_undeclared_identifier(error, expr->expr_literal_struct.name);
            return 0;
        }
        if(decl->kind != AST_DECL_TYPEDEF) {
            type_error(error, expr->expr_literal_struct.name, "'%.*s' is not a struct typename\n", TOKEN_STR(expr->expr_literal_struct.name));
            return 0;
        }
        // Get the struct type instead of the alias
        Light_Type* struct_type = type_alias_root(decl->decl_typedef.type_referenced);
        if(struct_type->kind != TYPE_KIND_STRUCT) {
            type_error(error, expr->expr_literal_struct.name, "'%.*s' is not a struct typename\n", TOKEN_STR(expr->expr_literal_struct.name));
            return 0;
        }
        // Require to be internalized to proceed with the type inference.
        if(!(struct_type->flags & TYPE_FLAG_INTERNALIZED)) {
            return expr->type;
        }

        // Verify if the struct literal is named, meaning is declared with field names
        // i.e. struct string {length: 0, capacity: 0, data: "foo" }
        bool named = expr->expr_literal_struct.named;
        s32 decl_field_count = struct_type->struct_info.fields_count;
        if (named) {
            s32 lit_field_count = (s32)array_length(expr->expr_literal_struct.struct_decls);
            if(decl_field_count != lit_field_count) {
                type_error(error, expr->expr_literal_struct.token_struct, 
                    "incompatible field count for struct literal, declaration requires %d, but got %d\n",
                    decl_field_count, lit_field_count);
                return 0;
            }
        } else {
            s32 lit_field_count = (s32)array_length(expr->expr_literal_struct.struct_exprs);
            if(decl_field_count != lit_field_count) {
                type_error(error, expr->expr_literal_struct.token_struct,
                    "incompatible field count for struct literal, declaration requires %d, but got %d\n",
                    decl_field_count, lit_field_count);
                return 0;
            }
        }

        bool all_fields_checked = true;
        // Type is already inferred, only need to check
        for(s32 i = 0; i < struct_type->struct_info.fields_count; ++i) {
            if(named) {
                Light_Ast* field = expr->expr_literal_struct.struct_decls[i];
                if(struct_type->struct_info.fields[i]->decl_variable.name->data != field->decl_variable.name->data) 
                {
                    // TODO(psv): orderless fields
                    // Fields names are incompatible
                }
            } else {
                Light_Ast* field = expr->expr_literal_struct.struct_exprs[i];
                Light_Type* expr_type = type_infer_expression(field, error);
                // TODO(psv):
                if(!expr_type) {
                    all_fields_checked = false;
                    continue;
                }
                Light_Type* field_type = struct_type->struct_info.fields[i]->decl_variable.type;
                expr_type = type_infer_propagate(field_type, field, error);
                if(!type_check_equality(expr_type, field_type)) {
                    type_error(error, expr->expr_literal_struct.token_struct,
                        "type mismatch in field #%d of struct literal. '", i + 1);
                    ast_print_type(expr_type, LIGHT_AST_PRINT_STDERR, 0);
                    fprintf(stderr, "' vs '");
                    ast_print_type(field_type, LIGHT_AST_PRINT_STDERR, 0);
                    fprintf(stderr, "'\n");
                }
            }
        }
        if(!all_fields_checked) {
            expr->type = 0;
            return 0;
        }
        expr->type = decl->decl_typedef.type_referenced;
    } else {
        // Create a new structure type
        if(expr->expr_literal_struct.struct_exprs) {
            // named fields struct literal
            Light_Scope* struct_scope = expr->expr_literal_struct.struct_scope;
 
            Light_Type* struct_type = type_new_struct(expr->expr_literal_struct.struct_decls, 
                array_length(expr->expr_literal_struct.struct_decls), struct_scope);
            struct_type = typecheck_resolve_type(struct_scope, struct_type, 0, error);
            if(struct_type && struct_type->flags & TYPE_FLAG_INTERNALIZED) {
                expr->type = struct_type;
            }
        } else {
            expr->type = global_type_empty_struct;
        }
    }
    return expr->type;
}

static Light_Type*
type_infer_expr_literal_array(Light_Ast* lexpr, u32* error) {
    assert(lexpr->kind == AST_EXPRESSION_LITERAL_ARRAY);
    if(lexpr->id == 724) {
        int xx = 0;
    }

    lexpr->flags |= AST_FLAG_EXPRESSION_LVALUE;

    if(lexpr->expr_literal_array.raw_data) {
        // Create the type for the array
        Light_Type* type = type_new_array(0, type_primitive_get(TYPE_PRIMITIVE_U8), lexpr->expr_literal_array.token_array);
        type->array_info.dimension_evaluated = true;
        type->array_info.dimension = lexpr->expr_literal_array.data_length_bytes;
        type->size_bits = lexpr->expr_literal_array.data_length_bytes * 8;
        type->flags = TYPE_FLAG_SIZE_RESOLVED;
        return type_internalize(type);
    }

    bool unresolved = false;
    Light_Type* strong_type = 0;
    s32 first_strong_type_index = array_length(lexpr->expr_literal_array.array_exprs);

    for(s32 i = 0; i < array_length(lexpr->expr_literal_array.array_exprs); ++i) {
        Light_Ast* expr = lexpr->expr_literal_array.array_exprs[i];
        
        expr->type = type_infer_expression(expr, error);
        if(!expr->type) {
            unresolved = true;
            continue;
        }

        if(strong_type) {
            if(expr->type->flags & TYPE_FLAG_INTERNALIZED) {
                if(!type_check_equality(expr->type, strong_type)) {
                    // TODO(psv): use lexing range for all expressions when is implemented
                    type_error_mismatch(error, lexpr->expr_literal_array.token_array, expr->type, strong_type);
                    fprintf(stderr, " in array literal\n");
                }
            } else {
                Light_Type* t = type_infer_propagate(strong_type, expr, error);
                if(!t) unresolved = true;
                expr->type = t;
            }
        } else {
            if(expr->type->flags & TYPE_FLAG_INTERNALIZED) {
                strong_type = expr->type;
                first_strong_type_index = i;
            }
        }
    }

    if(unresolved) return 0;

    if(strong_type) {
        for(s32 i = 0; i < first_strong_type_index; ++i) {
            // Expression is already inferred
            Light_Ast* expr = lexpr->expr_literal_array.array_exprs[i];
            if(expr->type->flags & TYPE_FLAG_INTERNALIZED) {
                if(!type_check_equality(expr->type, strong_type)) {
                    type_error_mismatch(error, lexpr->expr_literal_array.token_array, expr->type, strong_type);
                    fprintf(stderr, "\n");
                }
            } else {
                Light_Type* t = type_infer_propagate(strong_type, expr, error);
                if(!t) unresolved = true;
                expr->type = t;
            }
        }
    }
    if(*error & TYPE_ERROR) return 0;

    if(!strong_type) {
        if(array_length(lexpr->expr_literal_array.array_exprs) > 0) {
            lexpr->type = type_new_array(0, lexpr->expr_literal_array.array_exprs[0]->type, lexpr->expr_literal_array.token_array);
            lexpr->type->array_info.dimension_evaluated = true;
            lexpr->type->array_info.dimension = array_length(lexpr->expr_literal_array.array_exprs);
            return lexpr->type;
        } else {
            type_error(error, lexpr->expr_literal_array.token_array, 
                "literal array must have at least 1 element\n");
            return 0;
        }
    }

    Light_Type* type = type_new_array(0, strong_type, lexpr->expr_literal_array.token_array);
    type->array_info.dimension_evaluated = true;
    type->array_info.dimension = array_length(lexpr->expr_literal_array.array_exprs);
    type->size_bits = type->array_info.dimension * strong_type->size_bits;
    type->flags = TYPE_FLAG_SIZE_RESOLVED;
    lexpr->type = type_internalize(type);

    return lexpr->type;
}

static Light_Type*
type_infer_expr_literal_primitive(Light_Ast* expr, u32* error) {
    Light_Type* type = 0;
    switch(expr->expr_literal_primitive.type) {
        case LITERAL_BOOL:
            type = type_primitive_get(TYPE_PRIMITIVE_BOOL);
        break;
        case LITERAL_CHAR:{
            type = type_weak_primitive_from_literal(expr->expr_literal_primitive.type);
            // TODO(psv): eval character literal
        } break;
        case LITERAL_FLOAT:
        case LITERAL_DEC_SINT:
        case LITERAL_BIN_INT:
        case LITERAL_HEX_INT:
        case LITERAL_DEC_UINT: {
            type = type_weak_primitive_from_literal(expr->expr_literal_primitive.type);
        } break;
        case LITERAL_POINTER:{
            type = type_new_pointer(type_primitive_get(TYPE_PRIMITIVE_VOID));
            expr->type = type;
            return type;
        } break;
        default: assert(0); break;
    }
    expr->type = type;
    if(type->flags & TYPE_FLAG_INTERNALIZED) {
        eval_literal_primitive(expr);
    }
    return type;
}

static Light_Type*
type_infer_expr_unary(Light_Ast* expr, u32* error) {
    assert(expr->kind == AST_EXPRESSION_UNARY);
    
    Light_Type* operand_type = type_infer_expression(expr->expr_unary.operand, error);
    if(*error & TYPE_ERROR) return 0;
    if(!operand_type) return 0;

    switch(expr->expr_unary.op) {
        case OP_UNARY_ADDRESSOF: {
            if(operand_type) {
                if(TYPE_WEAK(operand_type)) {
                    operand_type = type_infer_propagate(0, expr->expr_unary.operand, error);
                }
                if(expr->expr_unary.operand->flags & AST_FLAG_EXPRESSION_LVALUE) {
                    expr->type = type_new_pointer(operand_type);
                } else {
                    type_error(error, expr->expr_unary.token_op, 
                        "operand of unary 'address of' must be an addressable lvalue\n");
                    expr->type = 0;
                }
            } else {
                expr->type = 0;
            }
        } break;
        case OP_UNARY_CAST:{
            if(!(expr->expr_unary.type_to_cast->flags & TYPE_FLAG_INTERNALIZED)) {
                Light_Type* resolved = typecheck_resolve_type(expr->scope_at, expr->expr_unary.type_to_cast, 0, error);
                if(!resolved || *error & TYPE_ERROR) {
                    expr->type = 0;
                } else {
                    expr->type = resolved;
                    expr->expr_unary.type_to_cast = resolved;
                }
            } else {
                expr->type = expr->expr_unary.type_to_cast;
            }

            type_infer_propagate(0, expr->expr_unary.operand, error);
            if((expr->expr_unary.flags & AST_FLAG_EXPRESSION_LVALUE)) {
                expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
            }

            Light_Type* lroot = type_alias_root(expr->expr_unary.operand->type);
            Light_Type* rroot = type_alias_root(expr->expr_unary.type_to_cast);
            if(!lroot || !rroot) {
                expr->type = 0;
            } else if(!type_cast_is_valid(lroot, rroot)) {
                type_error(error, expr->expr_unary.token_op, "invalid type conversion from '");
                ast_print_type(expr->expr_unary.operand->type, LIGHT_AST_PRINT_STDERR, 0);
                fprintf(stderr, "' to '");
                ast_print_type(expr->expr_unary.type_to_cast, LIGHT_AST_PRINT_STDERR, 0);
                fprintf(stderr, "'\n");
            }
        } break;
        case OP_UNARY_DEREFERENCE:{
            if(operand_type->kind == TYPE_KIND_POINTER) {
                expr->type = operand_type->pointer_to;
            } else {
                type_error(error, expr->expr_unary.token_op, "cannot derreference a non pointer type\n");
            }
            expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
        } break;
        case OP_UNARY_LOGIC_NOT:{
            expr->type = type_primitive_get(TYPE_PRIMITIVE_BOOL);
            if(!type_check_equality(expr->type, operand_type)) {
                type_error(error, expr->expr_unary.token_op, 
                    "logic operator '!' requires boolean type\n");
            }
        } break;

        case OP_UNARY_BITWISE_NOT:
        case OP_UNARY_MINUS:
        case OP_UNARY_PLUS:{
            if(!(operand_type->flags & TYPE_FLAG_INTERNALIZED)) {
                expr->type = type_infer_propagate(0, expr->expr_unary.operand, error);
            } else {
                expr->type = operand_type;
            }
        } break;
        default: assert(0); break;
    }
    return expr->type;
}

static Light_Type*
types_compatible(Light_Type* left, Light_Type* right) {
    //assert(TYPE_WEAK(left) && TYPE_WEAK(right));

    switch(left->kind) {
        case TYPE_KIND_PRIMITIVE:{
            Light_Type_Primitive pl = left->primitive;
            Light_Type_Primitive pr = right->primitive;
            if(type_primitive_uint(left))
                pl -= 4;    // transform it into signed
            if(type_primitive_uint(right))
                pr -= 4;    // transform it into signed
            if(type_primitive_int(left) && type_primitive_int(right)){
                return (pl > pr) ? left : right;
            }
            if(type_primitive_float(left) && type_primitive_float(right)) {
                return (left->primitive < right->primitive) ? left : right;
            }
            if(type_hash(left) == type_hash(right)) {
                return left;
            }
            // otherwise not compatible
            return 0;
        } break;
        case TYPE_KIND_ALIAS:
        case TYPE_KIND_ARRAY:
        case TYPE_KIND_ENUM:
        case TYPE_KIND_POINTER:
        case TYPE_KIND_STRUCT:
        case TYPE_KIND_UNION:
            assert(0);
            break;
        default: assert(0); break;
    }
    return 0;
}

static Light_Type*
type_infer_expr_binary_pointer_arithmetic(Light_Ast* expr, Light_Type* inferred_left, Light_Type* inferred_right, u32* error) {
    const Light_Type_Primitive ptr_arith_int_type = TYPE_PRIMITIVE_S32; // this must be s64 for 64 bit compiler @TODO
    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:{
            if(type_primitive_int(inferred_right)) {
                expr->expr_binary.right->type = type_infer_propagate(type_primitive_get(ptr_arith_int_type), expr->expr_binary.right, error);
            } else {
                type_error(error, expr->expr_binary.token_op, 
                    "pointer type addition is only valid with an integer type\n");
            }
            expr->type = inferred_left;
        } break;
        case OP_BINARY_MINUS: {
            if(type_primitive_int(inferred_right)) {
                // ptr - int
                expr->expr_binary.right->type = type_infer_propagate(type_primitive_get(ptr_arith_int_type), expr->expr_binary.right, error);
                expr->type = inferred_left;
            } else if(inferred_right->kind == TYPE_KIND_POINTER) {
                // ptr - ptr
                if(!type_check_equality(inferred_left, inferred_right)) {
                    type_error_mismatch(error, expr->expr_binary.token_op, inferred_left, inferred_right);
                }
                expr->type = type_primitive_get(ptr_arith_int_type);
            } else {
                type_error(error, expr->expr_binary.token_op, 
                    "pointer type difference is only defined for integer and pointer types\n");
            }
        } break;
        default: assert(0); break;
    }
    return expr->type;;
}

static Light_Type* 
type_infer_expr_proc_call(Light_Ast* expr, u32* error) {
    assert(expr->kind == AST_EXPRESSION_PROCEDURE_CALL);

    Light_Type* caller_type = type_infer_expression(expr->expr_proc_call.caller_expr, error);
    if(*error & TYPE_ERROR) return 0;

    if(!caller_type || !(caller_type->flags & TYPE_FLAG_INTERNALIZED)) return 0;

    if(caller_type->kind != TYPE_KIND_FUNCTION) {
        type_error(error, expr->expr_proc_call.token, "expected procedure type, but got '");
        ast_print_type(caller_type, LIGHT_AST_PRINT_STDERR, 0);
        fprintf(stderr, "'\n");
        return expr->type;
    }
    
    bool variadic = (caller_type->function.flags & TYPE_FUNCTION_VARIADIC) != 0;
    bool stdcall = (caller_type->function.flags & TYPE_FUNCTION_STDCALL) != 0;

    if(expr->expr_proc_call.arg_count < caller_type->function.arguments_count) {
        if(!variadic || (variadic && expr->expr_proc_call.arg_count < (caller_type->function.arguments_count - 1)))  {
            type_error(error, expr->expr_proc_call.token, 
                "too few arguments for procedure call, wanted '%d', but got '%d'\n",
                caller_type->function.arguments_count, expr->expr_proc_call.arg_count);
            return 0;
        }
    } else if(expr->expr_proc_call.arg_count > caller_type->function.arguments_count && !variadic) {
        type_error(error, expr->expr_proc_call.token, 
            "too many arguments for procedure call, wanted '%d', but got '%d'\n",
            caller_type->function.arguments_count, expr->expr_proc_call.arg_count);
        return 0;
    }

    bool all_arguments_internalized = true;
    for(s32 i = 0; i < caller_type->function.arguments_count - ((variadic) ? 1 : 0); ++i) {

        Light_Type* arg_type = caller_type->function.arguments_type[i];
        Light_Type* at = type_infer_expression(expr->expr_proc_call.args[i], error);
    
        if(!at || (*error & TYPE_ERROR)) {
            all_arguments_internalized = false;
            continue;
        }

        at = type_infer_propagate(arg_type, expr->expr_proc_call.args[i], error);
        if(!at || !(at->flags & TYPE_FLAG_INTERNALIZED)) {
            all_arguments_internalized = false;
            if(!types_compatible(at, arg_type)) {
                type_error_mismatch(error, expr->expr_proc_call.token, at, arg_type);
                fprintf(stderr, " in argument #%d of procedure call\n", i + 1);
            }
            continue;
        }
        expr->expr_proc_call.args[i]->type = at;

        if(!type_check_equality(at, arg_type)) {
            type_check_equality(at, arg_type);
            type_error_mismatch(error, expr->expr_proc_call.token, at, arg_type);
            fprintf(stderr, " in argument #%d of procedure call\n", i + 1);
        }
    }

    if(variadic) {
        // Transform the trailing arguments into an array literal.
        int count_trailing_exprs = expr->expr_proc_call.arg_count - (caller_type->function.arguments_count - 1);
        assert(count_trailing_exprs >= 0);

        if(count_trailing_exprs > 0) {
            // When variadic, propagate each argument after the not variadic
            // arguments.
            for(s32 i = 0; i < expr->expr_proc_call.arg_count; ++i) {
                if(i < caller_type->function.arguments_count - 1) continue;
                Light_Ast* arg = expr->expr_proc_call.args[i];
                Light_Type* t = type_infer_expression(arg, error);
                if(!t) {
                    all_arguments_internalized = false;
                    continue;
                }
                arg->type = t;
                arg->type = type_infer_propagate(0, arg, error);

                if(!(arg->type->flags & TYPE_FLAG_INTERNALIZED)) {
                    all_arguments_internalized = false;
                    continue;
                }

                //array_push(trailing_exprs, arg);
            }
        }

        if(!all_arguments_internalized) {
            return 0;
        }
    }

    if(variadic && !stdcall) {

        // Transform the trailing arguments into an array literal.
        int count_trailing_exprs = expr->expr_proc_call.arg_count - (caller_type->function.arguments_count - 1);
        assert(count_trailing_exprs >= 0);

        if(count_trailing_exprs > 0) {

            // User type value token
            Light_Token* user_type_value = token_new_identifier_from_string("User_Type_Value", sizeof("User_Type_Value") -1);

            Light_Ast** trailing_exprs = array_new(Light_Ast*);
            for(s32 i = 0; i < expr->expr_proc_call.arg_count; ++i) {
                if(i < caller_type->function.arguments_count - 1) continue;
                Light_Ast* arg = expr->expr_proc_call.args[i];

                // To fill the array of trailing expressions
                // we first need to transform it into an User_Type_Value
                // struct literal

                // 1, 2
                // to
                // User_Type_Value:{&[1], <ptr user type info>}, User_Type_Value:{&[2], <ptr user type info>}
                
                Light_Ast** arr_exprs = array_new(Light_Ast*);
                array_push(arr_exprs, arg);

                Light_Ast* arr = ast_new_expr_literal_array(arg->scope_at, 0, arr_exprs, (Lexical_Range){0});
                arr->flags |= AST_FLAG_COMPILER_GENERATED;
                Light_Ast* addr = ast_new_expr_unary(arg->scope_at, arr, 0, OP_UNARY_ADDRESSOF, (Lexical_Range){0});
                addr->flags |= AST_FLAG_COMPILER_GENERATED;

                Light_Ast** struct_exprs = array_new(Light_Ast*);
                array_push(struct_exprs, addr);
                Light_Ast* ptr_user_info = ast_new_expr_compiler_generated(arg->scope_at, COMPILER_GENERATED_POINTER_TO_TYPE_INFO);
                ptr_user_info->expr_compiler_generated.type_value = arg->type;
                array_push(struct_exprs, ptr_user_info);

                Light_Ast* arg_struct_literal = ast_new_expr_literal_struct(arg->scope_at, user_type_value, user_type_value, struct_exprs, false, 0, (Lexical_Range){0});
                array_push(trailing_exprs, arg_struct_literal);
                arg_struct_literal->flags |= AST_FLAG_COMPILER_GENERATED;

                arg_struct_literal->type = type_infer_expression(arg_struct_literal, error);
                if(!arg_struct_literal->type || !(arg_struct_literal->type->flags & TYPE_FLAG_INTERNALIZED)) {
                    all_arguments_internalized = false;
                    break;
                }
            }

            if(!all_arguments_internalized) {
                return 0;
            }

            // type_info : ^User_Type_Info;
            // i.e.: array{ capacity, length, ^User_Type_Info, [a, b, c, d]} 
            
            Light_Ast** struct_exprs = array_new(Light_Ast*);

            // TODO(psv): no location for this
            Light_Ast* capacity_expr = 0;
            Light_Ast* length_expr = 0;
            if (type_pointer_size_bits() == 64) {
                capacity_expr = ast_new_expr_literal_primitive_u64(expr->scope_at, (u64)count_trailing_exprs, (Lexical_Range){0});
                length_expr = ast_new_expr_literal_primitive_u64(expr->scope_at, (u64)count_trailing_exprs, (Lexical_Range){0});
            } else {
                capacity_expr = ast_new_expr_literal_primitive_u32(expr->scope_at, (u64)count_trailing_exprs, (Lexical_Range) { 0 });
                length_expr = ast_new_expr_literal_primitive_u32(expr->scope_at, (u64)count_trailing_exprs, (Lexical_Range) { 0 });
            }
            Light_Ast* type_info_expr = ast_new_expr_compiler_generated(expr->scope_at, COMPILER_GENERATED_USER_TYPE_INFO_POINTER);
            Light_Ast* trailing_array_literal = ast_new_expr_literal_array(expr->scope_at, 0, trailing_exprs, (Lexical_Range){0});
            Light_Ast* array_addr = ast_new_expr_unary(expr->scope_at, trailing_array_literal, 0, OP_UNARY_ADDRESSOF, (Lexical_Range){0});
            Light_Ast* array_cast = ast_new_expr_unary(expr->scope_at, array_addr, 0, OP_UNARY_CAST, (Lexical_Range){0});
            array_cast->expr_unary.type_to_cast = type_new_pointer(type_primitive_get(TYPE_PRIMITIVE_VOID));

            capacity_expr->flags |= AST_FLAG_COMPILER_GENERATED;
            length_expr->flags |= AST_FLAG_COMPILER_GENERATED;
            trailing_array_literal->flags |= AST_FLAG_COMPILER_GENERATED;
            array_cast->flags |= AST_FLAG_COMPILER_GENERATED;

            array_push(struct_exprs, capacity_expr);
            array_push(struct_exprs, length_expr);
            array_push(struct_exprs, type_info_expr);
            array_push(struct_exprs, array_cast);

            Light_Token* arr_token = token_new_identifier_from_string(
                light_special_idents_table[LIGHT_SPECIAL_IDENT_ARRAY].data, 
                light_special_idents_table[LIGHT_SPECIAL_IDENT_ARRAY].length);

            Light_Ast* struct_literal = ast_new_expr_literal_struct(expr->scope_at, 
                arr_token, 0, struct_exprs, false, 0, (Lexical_Range){0});
            struct_literal->flags |= AST_FLAG_COMPILER_GENERATED;

            Light_Ast* addr_of_struct_literal = ast_new_expr_unary(expr->scope_at, struct_literal, 
                0, OP_UNARY_ADDRESSOF, (Lexical_Range){0});
            addr_of_struct_literal->flags |= AST_FLAG_COMPILER_GENERATED;

            expr->expr_proc_call.arg_count -= count_trailing_exprs;
            expr->expr_proc_call.arg_count++;
            array_length(expr->expr_proc_call.args) -= count_trailing_exprs;
            array_push(expr->expr_proc_call.args, addr_of_struct_literal);

            addr_of_struct_literal->type = type_infer_expression(addr_of_struct_literal, error);
            if(!addr_of_struct_literal->type || *error & TYPE_ERROR) return 0;
            if(!(addr_of_struct_literal->type->flags & TYPE_FLAG_INTERNALIZED))
                all_arguments_internalized = false;
        } else {
            // Insert a null as second argument to call
            // to avoid zero arguments passed to a variadic function
            if(all_arguments_internalized) {
                Light_Ast* nullexpr = ast_new_expr_literal_primitive(expr->scope_at, 0, (Lexical_Range){0});
                nullexpr->flags |= AST_FLAG_COMPILER_GENERATED;
                nullexpr->expr_literal_primitive.type = LITERAL_POINTER;
                nullexpr->type = type_infer_expression(nullexpr, error);
                if(!expr->expr_proc_call.args) {
                    expr->expr_proc_call.args = array_new(Light_Ast*);
                }
                array_push(expr->expr_proc_call.args, nullexpr);
                expr->expr_proc_call.arg_count++;
            }
        }
    }
    
    // Only return the correct type if everything is internalized,
    // otherwise things won't be put in the infer queue.
    // @Optimization: Maybe think about putting individual expressions to the infer queue
    if(all_arguments_internalized) {
        return caller_type->function.return_type;
    }
    return 0;
}

static Light_Type* 
type_infer_expr_binary(Light_Ast* expr, u32* error) {
    Light_Type* left = type_infer_expression(expr->expr_binary.left, error);
    Light_Type* right = type_infer_expression(expr->expr_binary.right, error);

    if(!left || !right) {
        // Could not infer type, put this node in type infer queue
        return 0;
    }
    if(left->kind == TYPE_KIND_ALIAS && !type_alias_root(left)) {
        // Could not infer type, put this node in type infer queue
        return 0;
    }
    if(right->kind == TYPE_KIND_ALIAS && !type_alias_root(right)) {
        // Could not infer type, put this node in type infer queue
        return 0;
    }

    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:
        case OP_BINARY_MINUS:{
            if(left->kind == TYPE_KIND_POINTER){
                return type_infer_expr_binary_pointer_arithmetic(expr, left, right, error);
            }
        }
        case OP_BINARY_MULT:
        case OP_BINARY_DIV:
            if(!(type_primitive_numeric(left) && type_primitive_numeric(right))) {
                type_error(error, expr->expr_binary.token_op, 
                    "binary operator '%.*s' requires numeric types\n", TOKEN_STR(expr->expr_binary.token_op));
                return 0;
            }
            break;
        case OP_BINARY_MOD:
        case OP_BINARY_AND:
        case OP_BINARY_OR:
        case OP_BINARY_XOR:
        case OP_BINARY_SHL:
        case OP_BINARY_SHR: {
            if(!(type_primitive_int(left) && type_primitive_int(right))) {
                type_error(error, expr->expr_binary.token_op, 
                    "binary operator '%.*s' requires integer types\n", TOKEN_STR(expr->expr_binary.token_op));
                return 0;
            }
        }break;

        default: break;
    }

    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:
        case OP_BINARY_MINUS:
        case OP_BINARY_MULT:
        case OP_BINARY_DIV:
        case OP_BINARY_MOD:
        case OP_BINARY_AND:
        case OP_BINARY_OR:
        case OP_BINARY_XOR:
        case OP_BINARY_SHL:
        case OP_BINARY_SHR:{
            if(TYPE_STRONG(left) && TYPE_STRONG(right)) {
                // strong strong
            } else if(TYPE_STRONG(left) && TYPE_WEAK(right)) {
                // strong weak
                right = type_infer_propagate(left, expr->expr_binary.right, error);
            } else if(TYPE_WEAK(left) && TYPE_STRONG(right)) {
                // weak strong
                left = type_infer_propagate(right, expr->expr_binary.left, error);
            } else {
                // weak weak
                Light_Type* res = types_compatible(left, right);
                if(!res) {
                    type_error_mismatch(error, expr->expr_binary.token_op, left, right);
                    fprintf(stderr, " in binary operation '%.*s'\n", TOKEN_STR(expr->expr_binary.token_op));
                }
                return res;
            }
            if(!type_check_equality(left, right)) {
                type_error_mismatch(error, expr->expr_binary.token_op, left, right);
                fprintf(stderr, " in binary operation '%.*s'\n", TOKEN_STR(expr->expr_binary.token_op));
            } else {
                return left;
            }
        } break;

        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL:
        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE: {
            if((type_primitive_numeric(left) && type_primitive_numeric(right)) || 
                expr->expr_binary.op == OP_BINARY_EQUAL || expr->expr_binary.op == OP_BINARY_NOT_EQUAL) {
                if(TYPE_STRONG(left) && TYPE_STRONG(right)) {
                    // strong strong
                } else if(TYPE_STRONG(left) && TYPE_WEAK(right)) {
                    // strong weak
                    right = type_infer_propagate(left, expr->expr_binary.right, error);
                } else if(TYPE_WEAK(left) && TYPE_STRONG(right)) {
                    // weak strong
                    left = type_infer_propagate(right, expr->expr_binary.left, error);
                } else {
                    // weak weak
                    Light_Type* res = types_compatible(left, right);
                    if(!res) {
                        type_error_mismatch(error, expr->expr_binary.token_op, left, right);
                        fprintf(stderr, " in binary operation '%.*s'\n", TOKEN_STR(expr->expr_binary.token_op));
                    }
                    left = type_infer_propagate(0, expr->expr_binary.left, error);
                    right = type_infer_propagate(0, expr->expr_binary.right, error);
                }
                if(!type_check_equality(left, right)) {
                    type_error_mismatch(error, expr->expr_binary.token_op, left, right);
                    fprintf(stderr, " in binary operation '%.*s'\n", TOKEN_STR(expr->expr_binary.token_op));
                }
                return type_primitive_get(TYPE_PRIMITIVE_BOOL);
            } else {
                type_error(error, expr->expr_binary.token_op, 
                    "comparison operator '%.*s' requires numeric types\n", TOKEN_STR(expr->expr_binary.token_op));
                *error |= TYPE_ERROR;
            }
        } break;

        case OP_BINARY_LOGIC_AND:
        case OP_BINARY_LOGIC_OR:{
            if((type_primitive_bool(left) && type_primitive_bool(right))) {
                if(TYPE_STRONG(left) && TYPE_STRONG(right)) {
                    // strong strong
                } else if(TYPE_STRONG(left) && TYPE_WEAK(right)) {
                    // strong weak
                    right = type_infer_propagate(left, expr->expr_binary.right, error);
                } else if(TYPE_WEAK(left) && TYPE_STRONG(right)) {
                    // weak strong
                    left = type_infer_propagate(right, expr->expr_binary.left, error);
                } else {
                    // weak weak
                    Light_Type* res = types_compatible(left, right);
                    if(!res) {
                        type_error_mismatch(error, expr->expr_binary.token_op, left, right);
                    }
                    return res;
                }
                if(!type_check_equality(left, right)) {
                    type_error_mismatch(error, expr->expr_binary.token_op, left, right);
                    fprintf(stderr, " in binary operation '%.*s'\n", TOKEN_STR(expr->expr_binary.token_op));
                } else {
                    type_infer_propagate(0, expr->expr_binary.left, error);
                    type_infer_propagate(0, expr->expr_binary.right, error);
                    return type_primitive_get(TYPE_PRIMITIVE_BOOL);
                }
            } else {
                type_error(error, expr->expr_binary.token_op, 
                    "logic operator '%.*s' requires boolean types\n", TOKEN_STR(expr->expr_binary.token_op));
            }
        } break;

        case OP_BINARY_VECTOR_ACCESS:{
            if(type_primitive_int(right)) {
                type_infer_propagate(0, expr->expr_binary.right, error);
                expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
                
                if(left->kind == TYPE_KIND_ALIAS) {
                    left = left->alias.alias_to;
                    if(!left) return 0;
                }
                if(left->kind == TYPE_KIND_ARRAY){
                    // TODO(psv): static array bounds check maybe
                    return left->array_info.array_of;
                } else if(left->kind == TYPE_KIND_POINTER) {
                    return left->pointer_to;
                } else {
                    type_error(error, expr->expr_binary.token_op, "cannot access non array/pointer type\n");
                }
            } else {
                type_error(error, expr->expr_binary.token_op, 
                    "vector accessing operator requires integer indices, but got '");
                ast_print_type(right, LIGHT_AST_PRINT_STDERR, 0);
                fprintf(stderr, "'\n");
            }
        } break;

        default: assert(0); break;
    }

    return 0;
}

static Light_Ast*
find_struct_field_decl(Light_Scope* scope, Light_Token* ident) {
    Light_Symbol symb = {0};
    symb.token = ident;
    Symbol_Table* symbol_table = (Symbol_Table*)scope->symb_table;
    if(symbol_table) {
        int index = 0;
        if(symbol_table_entry_exist(symbol_table, symb, &index, 0)) {
            return symbol_table_get(symbol_table, index).decl;
        }
    }
    return 0;
}
static Light_Ast*
find_union_field_decl(Light_Scope* scope, Light_Token* ident) {
    Light_Symbol symb = {0};
    symb.token = ident;
    Symbol_Table* symbol_table = (Symbol_Table*)scope->symb_table;
    if(symbol_table) {
        int index = 0;
        if(symbol_table_entry_exist(symbol_table, symb, &index, 0)) {
            return symbol_table_get(symbol_table, index).decl;
        }
    }
    return 0;
}

Light_Ast*
find_enum_field_decl(Light_Scope* scope, Light_Token* ident, u32* error) {
    Light_Symbol symb = {0};
    symb.token = ident;
    Symbol_Table* symbol_table = (Symbol_Table*)scope->symb_table;

    if(symbol_table) {
        int index = 0;
        if(symbol_table_entry_exist(symbol_table, symb, &index, 0)) {
            return symbol_table_get(symbol_table, index).decl;
        }
    }
    // undeclared
    *error |= TYPE_ERROR;
    return 0;
}

Light_Type*
type_infer_expr_directive(Light_Ast* expr, u32* error) {
    assert(expr->kind == AST_EXPRESSION_DIRECTIVE);

    switch(expr->expr_directive.type) {
        case EXPR_DIRECTIVE_SIZEOF:{
            Light_Type* type = typecheck_resolve_type(expr->scope_at, expr->expr_directive.type_expr, 0, error);
            if(type && type->flags & TYPE_FLAG_INTERNALIZED) {
                // transform this node into a literal with the size of the type
                expr->expr_directive.type_expr = type;
                eval_directive_sizeof(expr);
                return expr->type;
            }
        } break;
        case EXPR_DIRECTIVE_TYPEOF:{
            type_error(error, expr->expr_directive.directive_token, "'#type_of' directive cannot be used as an expression, consider using '#type_value' instead.\n");
        } break;
        case EXPR_DIRECTIVE_TYPEVALUE:{
            assert(0);
        } break;

        case EXPR_DIRECTIVE_RUN: {
            Light_Type* type = type_infer_expression(expr->expr_directive.expr, error);
            if (type)
            {
                type = type_infer_propagate(0, expr->expr_directive.expr, error);
                if(type && type->flags & TYPE_FLAG_INTERNALIZED) {
                    // transform this node into a literal with the size of the type                
                    if(eval_directive_run(expr))
                        return expr->type;
                    else
                        return 0;
                }
            }
        } break;
        case EXPR_DIRECTIVE_COMPILE:
        // TODO(psv): other directives
        default: assert(0); break;
    }

    return 0;
}

Light_Type* 
type_infer_expr_dot(Light_Ast* expr, u32* error) {
    assert(expr->kind == AST_EXPRESSION_DOT);

    Light_Type* type = 0;

    if(expr->expr_dot.left->kind == AST_EXPRESSION_VARIABLE) {
        expr->expr_dot.left->flags |= AST_FLAG_ALLOW_BASE_ENUM;
    }
    Light_Type* left = type_infer_expression(expr->expr_dot.left, error);

    if(*error & TYPE_ERROR) return 0;
    if(!left || !(left->flags & TYPE_FLAG_INTERNALIZED)) return 0;

    if(left->kind == TYPE_KIND_POINTER) {
        left = left->pointer_to;
    }

    if(!left || !(left->flags & TYPE_FLAG_INTERNALIZED)) return 0;

    Light_Type* original_type = left;
    left = type_alias_root(left);
    if(!left || !(left->flags & TYPE_FLAG_INTERNALIZED)) return 0;

    switch(left->kind) {
        case TYPE_KIND_STRUCT:{
            Light_Ast* decl = find_struct_field_decl(left->struct_info.struct_scope, expr->expr_dot.identifier);
            if(!decl) {
                type_error(error, expr->expr_dot.identifier, 
                    "undeclared struct field '%.*s'\n", TOKEN_STR(expr->expr_dot.identifier));
                return 0;
            }
            type = decl->decl_variable.type;
            expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
        } break;
        case TYPE_KIND_UNION:{
            Light_Ast* decl = find_union_field_decl(left->union_info.union_scope, expr->expr_dot.identifier);
            if(!decl) {
                type_error(error, expr->expr_dot.identifier, 
                    "undeclared union field '%.*s'\n", TOKEN_STR(expr->expr_dot.identifier));
                return 0;
            }
            type = decl->decl_variable.type;
            expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
        } break;
        case TYPE_KIND_ENUM:{
            Light_Ast* decl = find_enum_field_decl(left->enumerator.enum_scope, expr->expr_dot.identifier, error);
            if(!decl) {
                type_error(error, expr->expr_dot.identifier, 
                    "undeclared enum field '%.*s'\n", TOKEN_STR(expr->expr_dot.identifier));
                return 0;
            }
            type = original_type;
        } break;
        case TYPE_KIND_POINTER: {
            // Already dereferenced once, if it is still a pointer, error out
            type_error(error, expr->expr_dot.identifier, 
                "operator '.' cannot dereference lvalue twice\n");
            expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
            return 0;
        } break;
        case TYPE_KIND_ALIAS:{
            // We should never get here, since the type should already be 'de-aliased' when
            // inferring from the left side.
            assert(0);
        } break;
        case TYPE_KIND_FUNCTION: {
            type_error(error, expr->expr_dot.identifier, 
                "operator '.' is not defined for functional types\n");
            return 0;
        } break;
        case TYPE_KIND_PRIMITIVE:{
            type_error(error, expr->expr_dot.identifier, 
                "operator '.' is not defined for primitive types\n");
            return 0;
        } break;
        case TYPE_KIND_ARRAY: {
            type_error(error, expr->expr_dot.identifier, 
                "operator '.' is not defined for array types\n");
            return 0;
        } break;
        // Maybe implement namespaces in the future
        default: assert(0); break;
    }

    return type;
}

Light_Type* 
type_infer_expression(Light_Ast* expr, u32* error) {
    assert(expr->flags & AST_FLAG_EXPRESSION);
    if(expr->type && expr->type->flags & TYPE_FLAG_INTERNALIZED)
        return expr->type;

    Light_Type* type = 0;
    switch(expr->kind) {
        case AST_EXPRESSION_LITERAL_ARRAY:
            type = type_infer_expr_literal_array(expr, error);
            break;
        case AST_EXPRESSION_LITERAL_STRUCT:
            type = type_infer_expr_literal_struct(expr, error);
            break;
        case AST_EXPRESSION_LITERAL_PRIMITIVE:
            type = type_infer_expr_literal_primitive(expr, error);
            break;
        case AST_EXPRESSION_BINARY:
            type = type_infer_expr_binary(expr, error);
            break;
        case AST_EXPRESSION_PROCEDURE_CALL:
            type = type_infer_expr_proc_call(expr, error);
            break;
        case AST_EXPRESSION_UNARY:
            type = type_infer_expr_unary(expr, error);
            break;
        case AST_EXPRESSION_VARIABLE:
            type = type_infer_expr_variable(expr, error);
            break;
        case AST_EXPRESSION_DOT:
            type = type_infer_expr_dot(expr, error);
            break;

        case AST_EXPRESSION_DIRECTIVE:
            type = type_infer_expr_directive(expr, error);
            break;
        case AST_EXPRESSION_COMPILER_GENERATED:
            // TODO(psv): should this be empty?
            break;
        default: assert(0); break;
    }

    expr->type = type;
    return type;
}