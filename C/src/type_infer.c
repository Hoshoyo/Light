#include "type_infer.h"
#include "symbol_table.h"
#include "top_typecheck.h"
#include "ast.h"
#include <stdio.h>
#include <assert.h>
#include <light_array.h>

#define TOKEN_STR(T) (T)->length, (T)->data

static s32
type_infer_error_location(Light_Token* t) {
    if(!t) {
        return fprintf(stderr, "%s: ", t->filepath);
    } else {
        return fprintf(stderr, "%s:%d:%d: ", t->filepath, t->line + 1, t->column + 1);
    }
    return 0;
}

static s32
type_infer_error_undeclared_identifier(Light_Token* t) {
    s32 length = 0;
    length += type_infer_error_location(t);
    fprintf(stderr, "Type Error: undeclared identifier '%.*s'\n", TOKEN_STR(t));
}

Light_Type_Check_Error
type_infer_type_mismatch_error(Light_Token* location, Light_Type* left, Light_Type* right) {
    type_infer_error_location(location);
    fprintf(stderr, "Type Error: type mismatch '");
    ast_print_type(left, LIGHT_AST_PRINT_STDERR);
    fprintf(stderr, "' vs '");
    ast_print_type(right, LIGHT_AST_PRINT_STDERR);
    fprintf(stderr, "' ");
    return TYPE_ERROR;
}

static Light_Ast*
decl_from_name(Light_Scope* scope, Light_Token* name) {
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

static
Light_Type* 
type_infer_propagate_literal(Light_Type* type, Light_Ast* expr, u32* error) {
    if(type) {
        assert(TYPE_STRONG(type));
    }
    assert(expr->type);

    switch(expr->expr_literal.type) {
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
        case LITERAL_STRUCT: {
            for(u64 i = 0; i < array_length(expr->expr_literal.struct_exprs); ++i) {
                if(type) {
                    type = type->struct_info.fields[i]->decl_variable.type;
                }
                type_infer_propagate(type, expr->expr_literal.struct_exprs[i], error);
            }
        } break;
        case LITERAL_ARRAY:{
            for(u64 i = 0; i < array_length(expr->expr_literal.array_exprs); ++i) {
                if(type) {
                    type = type->array_info.array_of;
                }
                type_infer_propagate(type, expr->expr_literal.array_exprs[i], error);
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
            
            if(left == right) {
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

            if(left == right) {
                expr->type = left;
            }
        } break;

        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE:
        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL: {
            assert(type == type_primitive_get(TYPE_PRIMITIVE_BOOL));
            Light_Type* left = type_infer_propagate(type, expr->expr_binary.left, error);
            Light_Type* right = type_infer_propagate(type, expr->expr_binary.right, error);
            assert(type_primitive_numeric(left) && type_primitive_numeric(right));
            
            expr->type = type;
        } break;

        case OP_BINARY_LOGIC_AND:
        case OP_BINARY_LOGIC_OR: {
            assert(expr->expr_binary.left->type == type_primitive_get(TYPE_PRIMITIVE_BOOL));
            assert(expr->expr_binary.right->type == type_primitive_get(TYPE_PRIMITIVE_BOOL));
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

    if(expr->type && TYPE_STRONG(expr->type)) {
        return expr->type;
    }

    switch(expr->kind) {
        case AST_EXPRESSION_LITERAL:
            return type_infer_propagate_literal(type, expr, error);
        case AST_EXPRESSION_BINARY:
            return type_infer_propagate_binary(type, expr, error);
        case AST_EXPRESSION_UNARY:
            return type_infer_propagate_unary(type, expr, error);
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
    Light_Ast* decl = decl_from_name(expr->scope_at, expr->expr_variable.name);
    if(!decl) {
        type_infer_error_undeclared_identifier(expr->expr_variable.name);
        *error |= TYPE_ERROR;
        return 0;
    }

    switch(decl->kind) {
        case AST_DECL_CONSTANT:{
            return decl->decl_constant.type_info;
        } break;
        case AST_DECL_PROCEDURE:{
            return decl->decl_proc.proc_type;
        } break;
        case AST_DECL_VARIABLE:{
            return decl->decl_variable.type;
        } break;
        case AST_DECL_TYPEDEF:{
            // TODO(psv): support alias of an alias
            if(decl->decl_typedef.type_referenced->kind == TYPE_KIND_ENUM && expr->flags & AST_FLAG_ALLOW_BASE_ENUM) {
                return decl->decl_typedef.type_referenced;
            }
            // Error, referencing a typename instead of a declaration
            type_infer_error_location(expr->expr_variable.name);
            fprintf(stderr, "Type Error: referencing the typename '%.*s' as an rvalue variable\n", TOKEN_STR(expr->expr_variable.name));
            *error |= TYPE_ERROR;
        } break;
        default: assert(0); break;
    }
    return 0;
}

static Light_Type*
type_infer_expr_literal(Light_Ast* expr, u32* error) {
    switch(expr->expr_literal.type) {
        case LITERAL_BOOL:
            return type_primitive_get(TYPE_PRIMITIVE_BOOL);
        case LITERAL_CHAR:
        case LITERAL_FLOAT:
        case LITERAL_DEC_SINT:
        case LITERAL_BIN_INT:
        case LITERAL_HEX_INT:
        case LITERAL_DEC_UINT: {
            return type_weak_primitive_from_literal(expr->expr_literal.type);
        } break;
        case LITERAL_POINTER:{
            return type_new_pointer(TYPE_PRIMITIVE_VOID);
        } break;
        case LITERAL_ARRAY: {
            // TODO(psv):
            assert(0);
        } break;
        case LITERAL_STRUCT: {
            // TODO(psv):
            assert(0);
        } break;
        default: assert(0); break;
    }
}

static Light_Type*
type_infer_expr_unary(Light_Ast* expr, u32* error) {
    assert(expr->kind == AST_EXPRESSION_UNARY);
    
    Light_Type* operand_type = type_infer_expression(expr->expr_unary.operand, error);
    if(*error & TYPE_ERROR) return 0;

    switch(expr->expr_unary.op) {
        case OP_UNARY_ADDRESSOF: {
            if(operand_type) {
                expr->type = type_new_pointer(operand_type);
            } else {
                expr->type = 0;
            }
        } break;
        case OP_UNARY_CAST:{
            expr->type = expr->expr_unary.type_to_cast;
            type_infer_propagate(0, expr->expr_unary.operand, error);
        } break;
        case OP_UNARY_DEREFERENCE:{
            if(operand_type->kind == TYPE_KIND_POINTER) {
                expr->type = operand_type->pointer_to;
            } else {
                type_infer_error_location(expr->expr_unary.token_op);
                fprintf(stderr, "Type Error: cannot derreference a non pointer type\n");
                *error |= TYPE_ERROR;
            }
        } break;
        case OP_UNARY_LOGIC_NOT:{
            expr->type = type_primitive_get(TYPE_PRIMITIVE_BOOL);
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
}

static Light_Type*
type_infer_expr_binary_pointer_arithmetic(Light_Ast* expr, Light_Type* inferred_left, Light_Type* inferred_right, u32* error) {
    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:{
            if(type_primitive_int(inferred_right)) {
                expr->type = type_infer_propagate(type_primitive_get(TYPE_PRIMITIVE_S64), expr->expr_binary.right, error);
            } else {
                type_infer_error_location(expr->expr_binary.token_op);
                fprintf(stderr, "Type Error: pointer type addition is only valid with an integer type\n");
                *error |= TYPE_ERROR;
            }
        } break;
        case OP_BINARY_MINUS: {
            if(type_primitive_int(inferred_right) || inferred_right->kind == TYPE_KIND_POINTER) {
                if(inferred_left != inferred_right) {
                    *error |= type_infer_type_mismatch_error(expr->expr_binary.token_op, inferred_left, inferred_right);
                } else {
                    expr->type = inferred_left;
                }
            } else {
                type_infer_error_location(expr->expr_binary.token_op);
                fprintf(stderr, "Type Error: pointer type difference is only defined for integer and pointer types\n");
                *error |= TYPE_ERROR;
            }
        } break;
        default: assert(0); break;
    }
}

static Light_Type* 
type_infer_expr_proc_call(Light_Ast* expr, u32* error) {
    assert(expr->kind == AST_EXPRESSION_PROCEDURE_CALL);
    Light_Type* caller_type = type_infer_expression(expr->expr_proc_call.caller_expr, error);
    if(*error & TYPE_ERROR) return 0;

    if(caller_type->kind != TYPE_KIND_FUNCTION) {
        type_infer_error_location(expr->expr_proc_call.token);
        fprintf(stderr, "Type Error: expected procedure type, but got '");
        ast_print_type(caller_type, LIGHT_AST_PRINT_STDERR);
        fprintf(stderr, "'\n");
        *error |= TYPE_ERROR;
        return expr->type;
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
                type_infer_error_location(expr->expr_binary.token_op);
                fprintf(stderr, "Type Error: binary operator '%.*s' requires numeric types\n", TOKEN_STR(expr->expr_binary.token_op));
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
                type_infer_error_location(expr->expr_binary.token_op);
                fprintf(stderr, "Type Error: binary operator '%.*s' requires integer types\n", TOKEN_STR(expr->expr_binary.token_op));
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
                if(res) {
                    left = res;
                    right = res;
                }
            }
            if(left != right) {
                *error |= type_infer_type_mismatch_error(expr->expr_binary.token_op, left, right);
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
                    if(res) {
                        left = res;
                        right = res;
                    }
                }
                if(left != right) {
                    *error |= type_infer_type_mismatch_error(expr->expr_binary.token_op, left, right);
                    fprintf(stderr, " in binary operation '%.*s'\n", TOKEN_STR(expr->expr_binary.token_op));
                } else {
                    type_infer_propagate(0, expr->expr_binary.left, error);
                    type_infer_propagate(0, expr->expr_binary.right, error);
                    return type_primitive_get(TYPE_PRIMITIVE_BOOL);
                }
            } else {
                type_infer_error_location(expr->expr_binary.token_op);
                fprintf(stderr, "Type Error: comparison operator '%.*s' requires numeric types\n", TOKEN_STR(expr->expr_binary.token_op));
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
                    if(res) {
                        left = res;
                        right = res;
                    }
                }
                if(left != right) {
                    *error |= type_infer_type_mismatch_error(expr->expr_binary.token_op, left, right);
                    fprintf(stderr, " in binary operation '%.*s'\n", TOKEN_STR(expr->expr_binary.token_op));
                } else {
                    type_infer_propagate(0, expr->expr_binary.left, error);
                    type_infer_propagate(0, expr->expr_binary.right, error);
                    return type_primitive_get(TYPE_PRIMITIVE_BOOL);
                }
            } else {
                type_infer_error_location(expr->expr_binary.token_op);
                fprintf(stderr, "Type Error: logic operator '%.*s' requires boolean types\n", TOKEN_STR(expr->expr_binary.token_op));
                *error |= TYPE_ERROR;
            }
        } break;

        case OP_BINARY_VECTOR_ACCESS:{
            if(type_primitive_int(right)) {
                type_infer_propagate(0, expr->expr_binary.right, error);
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
                    type_infer_error_location(expr->expr_binary.token_op);
                    fprintf(stderr, "Type Error: cannot access non array/pointer type\n");
                    *error |= TYPE_ERROR;
                }
            } else {
                type_infer_error_location(expr->expr_binary.token_op);
                fprintf(stderr, "Type Error: vector accessing operator requires integer indices, but got '");
                ast_print_type(right, LIGHT_AST_PRINT_STDERR);
                fprintf(stderr, "'\n");
                *error |= TYPE_ERROR;
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

static Light_Ast*
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

    switch(left->kind) {
        case TYPE_KIND_STRUCT:{
            Light_Ast* decl = find_struct_field_decl(left->struct_info.struct_scope, expr->expr_dot.identifier);
            if(!decl) {
                type_infer_error_location(expr->expr_dot.identifier);
                fprintf(stderr, "Type Error: Undeclared struct field '%.*s'\n", TOKEN_STR(expr->expr_dot.identifier));
                *error |= TYPE_ERROR;
                return 0;
            }
            type = decl->decl_variable.type; 
        } break;
        case TYPE_KIND_UNION:{
            Light_Ast* decl = find_union_field_decl(left->union_info.union_scope, expr->expr_dot.identifier);
            if(!decl) {
                type_infer_error_location(expr->expr_dot.identifier);
                fprintf(stderr, "Type Error: Undeclared union field '%.*s'\n", TOKEN_STR(expr->expr_dot.identifier));
                *error |= TYPE_ERROR;
                return 0;
            }
            type = decl->decl_variable.type;
        } break;
        case TYPE_KIND_ENUM:{
            Light_Ast* decl = find_enum_field_decl(left->enumerator.enum_scope, expr->expr_dot.identifier, error);
            if(*error & TYPE_ERROR) {
                type_infer_error_location(expr->expr_dot.identifier);
                fprintf(stderr, "Type Error: Undeclared enum field '%.*s'\n", TOKEN_STR(expr->expr_dot.identifier));
                *error |= TYPE_ERROR;
                return 0;
            }
            type = left;
        } break;
        case TYPE_KIND_POINTER: {
            // Already dereferenced once, if it is still a pointer, error out
            type_infer_error_location(expr->expr_dot.identifier);
            fprintf(stderr, "Type Error: Operator '.' cannot dereference lvalue twice\n");
            *error |= TYPE_ERROR;
            return 0;
        } break;
        case TYPE_KIND_ALIAS:{
            // TODO(psv): alias
            assert(0);
        } break;
        case TYPE_KIND_FUNCTION: {
            type_infer_error_location(expr->expr_dot.identifier);
            fprintf(stderr, "Type Error: Operator '.' is not defined for functional types\n");
            *error |= TYPE_ERROR;
            return 0;
        } break;
        case TYPE_KIND_PRIMITIVE:{
            type_infer_error_location(expr->expr_dot.identifier);
            fprintf(stderr, "Type Error: Operator '.' is not defined for primitive types\n");
            *error |= TYPE_ERROR;
            return 0;
        } break;
        case TYPE_KIND_ARRAY: {
            type_infer_error_location(expr->expr_dot.identifier);
            fprintf(stderr, "Type Error: Operator '.' is not defined for array types\n");
            *error |= TYPE_ERROR;
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

    Light_Type* type = 0;
    switch(expr->kind) {
        case AST_EXPRESSION_LITERAL:
            type = type_infer_expr_literal(expr, error);
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