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
        }
        scope = scope->parent;
    }
    return s.decl;
}

static Light_Type*
type_infer_propagate_literal_array(Light_Type* type, Light_Ast* expr, u32* error) {
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
                if(!propagated) {
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
            assert(0);
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
            break;
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
            break;
        case AST_EXPRESSION_DIRECTIVE:
            // TODO(psv):
            assert(0);
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
            //return type_alias_root(decl->decl_constant.type_info);
            return decl->decl_constant.type_info;
        } break;
        case AST_DECL_PROCEDURE:{
            expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
            return decl->decl_proc.proc_type;
        } break;
        case AST_DECL_VARIABLE:{
            //return type_alias_root(decl->decl_variable.type);
            expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
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
                Light_Type* field_type = struct_type->struct_info.fields[i]->decl_variable.type;
                expr_type = type_infer_propagate(field_type, field, error);
                if(!type_check_equality(expr_type, field_type)) {
                    type_error(error, expr->expr_literal_struct.token_struct,
                        "type mismatch in field #%d of struct literal.\n  '", i + 1);
                    ast_print_type(expr_type, LIGHT_AST_PRINT_STDERR, 0);
                    fprintf(stderr, "' vs '");
                    ast_print_type(field_type, LIGHT_AST_PRINT_STDERR, 0);
                    fprintf(stderr, "'\n");
                }
            }
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
        case LITERAL_CHAR:
        case LITERAL_FLOAT:
        case LITERAL_DEC_SINT:
        case LITERAL_BIN_INT:
        case LITERAL_HEX_INT:
        case LITERAL_DEC_UINT: {
            type = type_weak_primitive_from_literal(expr->expr_literal_primitive.type);
        } break;
        case LITERAL_POINTER:{
            type = type_new_pointer(type_primitive_get(TYPE_PRIMITIVE_VOID));
        } break;
        default: assert(0); break;
    }
    expr->type = type;
    eval_literal_primitive(expr);
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
            expr->type = expr->expr_unary.type_to_cast;
            // TODO(psv): check if type conversion exists
            type_infer_propagate(0, expr->expr_unary.operand, error);
            if((expr->expr_unary.flags & AST_FLAG_EXPRESSION_LVALUE)) {
                expr->flags |= AST_FLAG_EXPRESSION_LVALUE;
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
            expr->type = operand_type;
        } break;
        default: assert(0); break;
    }
    return expr->type;
}

static Light_Type*
types_compatible(Light_Type* left, Light_Type* right) {
    assert(TYPE_WEAK(left) && TYPE_WEAK(right));

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
    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:{
            if(type_primitive_int(inferred_right)) {
                expr->expr_binary.right->type = type_infer_propagate(type_primitive_get(TYPE_PRIMITIVE_S64), expr->expr_binary.right, error);
            } else {
                type_error(error, expr->expr_binary.token_op, 
                    "pointer type addition is only valid with an integer type\n");
            }
            expr->type = inferred_left;
        } break;
        case OP_BINARY_MINUS: {
            if(type_primitive_int(inferred_right)) {
                // ptr - int
                expr->expr_binary.right->type = type_infer_propagate(type_primitive_get(TYPE_PRIMITIVE_S64), expr->expr_binary.right, error);
                expr->type = inferred_left;
            } else if(inferred_right->kind == TYPE_KIND_POINTER) {
                // ptr - ptr
                if(!type_check_equality(inferred_left, inferred_right)) {
                    type_error_mismatch(error, expr->expr_binary.token_op, inferred_left, inferred_right);
                }
                expr->type = type_primitive_get(TYPE_PRIMITIVE_S64);
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

    if(expr->expr_proc_call.arg_count < caller_type->function.arguments_count) {
        type_error(error, expr->expr_proc_call.token, 
            "too few arguments for procedure call, wanted '%d', but got '%d'\n",
            caller_type->function.arguments_count, expr->expr_proc_call.arg_count);
        return 0;
    } else if(expr->expr_proc_call.arg_count > caller_type->function.arguments_count) {
        type_error(error, expr->expr_proc_call.token, 
            "too many arguments for procedure call, wanted '%d', but got '%d'\n",
            caller_type->function.arguments_count, expr->expr_proc_call.arg_count);
        return 0;
    }

    for(s32 i = 0; i < caller_type->function.arguments_count; ++i) {
        Light_Type* arg_type = caller_type->function.arguments_type[i];
        Light_Type* at = type_infer_expression(expr->expr_proc_call.args[i], error);
        at = type_infer_propagate(arg_type, expr->expr_proc_call.args[i], error);
        expr->expr_proc_call.args[i]->type = at;

        if(!type_check_equality(at, arg_type)) {
            type_error_mismatch(error, expr->expr_proc_call.token, at, arg_type);
            fprintf(stderr, " in argument #%d of procedure call\n", i + 1);
        }
    }
    
    return caller_type->function.return_type;
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
                if(type_check_equality(left, right)) {
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
                    type_error(error, expr->expr_binary.token_op, "Type Error: cannot access non array/pointer type\n");
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

    Light_Type* original_type = left;
    left = type_alias_root(left);

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
    if(expr->type && expr->type->flags && TYPE_FLAG_INTERNALIZED)
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
            // TODO(psv):
            assert(0);
            break;
        default: assert(0); break;
    }

    expr->type = type;
    return type;
}