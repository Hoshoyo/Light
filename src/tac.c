#include "tac.h"
#include "utils/allocator.h"
#include "light_array.h"
#include <assert.h>
#include <stdio.h>
#include "type.h"

const u32 TAC_FLAG_CADDR = (1 << 0);
const u32 TAC_FLAG_LVALUE = (1 << 1);
const u32 TAC_FLAG_RVALUE = (1 << 2);
#if 0
s32 tac_generate_expression_unary(Light_Ast* expr) {
    assert(expr->kind == AST_EXPRESSION_UNARY);
    Tac_Node node = {0};

    switch(expr->expr_unary.op) {
        case OP_UNARY_PLUS: {
            result = tac_generate_expression(bb, expr->expr_unary.operand, true);
        } break;
        case OP_UNARY_LOGIC_NOT:
        case OP_UNARY_BITWISE_NOT:
        case OP_UNARY_MINUS: {
            node.kind = TAC_EXPRESSION_UNARY;
            node.unop.op = expr->expr_unary.op;
            node.flags |= TAC_FLAG_RVALUE;
            Tac_Node left = tac_generate_expression(expr->expr_unary.operand, true);
            array_push(bb->nodes, node);
        } break;
        case OP_UNARY_DEREFERENCE: {
            if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_ADDRESSOF) {
                // do nothing
            } else {
                node.kind = TAC_EXPRESSION_DEREF;
                node.flags |= TAC_FLAG_LVALUE;
                result = tac_generate_expression(bb, expr->expr_unary.operand, true);
                array_push(bb->nodes, node);
            }
        } break;
        case OP_UNARY_ADDRESSOF: {
            if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_DEREFERENCE) {
                // do nothing
            } else {
                node.kind = TAC_EXPRESSION_ADDRESS_OF;
                node.flags |= TAC_FLAG_CADDR;
                result = tac_generate_expression(bb, expr->expr_unary.operand, false);
                array_push(bb->nodes, node);
            }
        } break;
        case OP_UNARY_CAST: {
            // TODO(psv)
        } break;
        default: assert(0); break;
    }

    return result;
}
#endif

Tac_Atom tac_new_temporary() {
    Tac_Atom result = {0};
    result.type = TAC_ATOM_TEMPORARY;
    result.temporary_number = 1;
    return result;
}

Tac_Atom tac_result_atom(Tac_Node* node) {
    Tac_Atom result = {0};
    switch(node->kind) {
        case TAC_EXPRESSION_BINARY_FLOAT: result = node->binop_float.left; break;
        case TAC_EXPRESSION_BINARY_INT: result = node->binop_int.left; break;
        case TAC_EXPRESSION_UNARY:  result = node->unop.left; break;
    }
    return result;
}

Tac_Atom 
tac_generate_expression_binop_float(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result) {
    assert(expr->kind == AST_EXPRESSION_BINARY);
    assert(type_primitive_float(expr->type));

    Tac_Node* node = light_alloc(sizeof(Tac_Node));
    node->binop_int.op = expr->expr_binary.op;
    node->binop_int.size_bytes = expr->type->size_bits << 3;
    node->binop_int.left = tac_new_temporary();

    tac_generate_expression(tac, expr->expr_binary.left, &node->binop_float.e1, true);
    tac_generate_expression(tac, expr->expr_binary.right, &node->binop_float.e2, true);

    return node->binop_float.left;
}

Tac_Atom 
tac_generate_expression_binop_integer(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result) {
    assert(expr->kind == AST_EXPRESSION_BINARY);
    assert(type_primitive_int(expr->type) || type_primitive_bool(expr->type));

    Tac_Node* node = light_alloc(sizeof(Tac_Node));
    node->binop_int.op = expr->expr_binary.op;
    node->binop_int.size_bytes = expr->type->size_bits << 3;
    node->binop_int.left = tac_new_temporary();

    tac_generate_expression(tac, expr->expr_binary.left, &node->binop_int.e1, true);
    tac_generate_expression(tac, expr->expr_binary.right, &node->binop_int.e2, true);

    return node->binop_int.left;
}

Tac_Atom tac_generate_expression_literal_primitive(Light_Ast* expr) {
    assert(expr->kind == AST_EXPRESSION_LITERAL_PRIMITIVE);
    Tac_Atom atom = {0};
    
    switch(expr->type->kind) {
        case TYPE_KIND_PRIMITIVE: {
            switch(expr->type->primitive) {
                case TYPE_PRIMITIVE_U8:
                case TYPE_PRIMITIVE_U16:
                case TYPE_PRIMITIVE_U32:
                case TYPE_PRIMITIVE_U64:
                case TYPE_PRIMITIVE_S8:
                case TYPE_PRIMITIVE_S16:
                case TYPE_PRIMITIVE_S32:
                case TYPE_PRIMITIVE_S64: {
                    atom.type = TAC_ATOM_CONSTANT_INTEGER;
                    atom.const_int.val_u64 = expr->expr_literal_primitive.value_u64;
                    atom.const_int.byte_size = expr->type->size_bits << 3;
                } break;
                case TYPE_PRIMITIVE_R32: {
                    atom.type = TAC_ATOM_CONSTANT_FLOAT;
                    atom.const_float.val_r32 = expr->expr_literal_primitive.value_r32;
                    atom.const_float.byte_size = 4;
                } break;
                case TYPE_PRIMITIVE_R64: {
                    atom.type = TAC_ATOM_CONSTANT_FLOAT;
                    atom.const_float.val_r64 = expr->expr_literal_primitive.value_r64;
                    atom.const_float.byte_size = 8;
                } break;
                case TYPE_PRIMITIVE_BOOL: {
                    atom.type = TAC_ATOM_CONSTANT_INTEGER;
                    atom.const_int.val_u64 = 1;
                    atom.const_int.byte_size = 1;
                } break;
                default: assert(0); break;
            }
        } break;
        case TYPE_KIND_POINTER: {
            atom.type = TAC_ATOM_CONSTANT_INTEGER;
            atom.const_int.val_u64 = 0;
            atom.const_int.byte_size = 8;
        } break;
        default: break;
    }

    return atom;
}

Tac_Atom tac_generate_expression_binary(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result) {
    if(type_primitive_int(expr->type) || type_primitive_bool(expr->type)) {
        return tac_generate_expression_binop_integer(tac, expr, out_result);
    } else if(type_primitive_float(expr->type)) {
        return tac_generate_expression_binop_float(tac, expr, out_result);
    } else {
        // pointer arithmetic
        // TODO(psv):
        assert(0);
    }
}

Tac_Node* tac_generate_expression(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result, bool eval) {

    switch(expr->kind) {
        case AST_EXPRESSION_DOT: {
            // TODO(psv)
        } break;
        case AST_EXPRESSION_LITERAL_ARRAY: {
            // TODO(psv)
        } break;
        case AST_EXPRESSION_LITERAL_STRUCT: {
            // TODO(psv)
        } break;
        case AST_EXPRESSION_PROCEDURE_CALL: {
            // TODO(psv)
        } break;
        case AST_EXPRESSION_UNARY: {
            //node = tac_generate_expression_unary(expr);
        } break;
        case AST_EXPRESSION_BINARY: {
            //node = tac_generate_expression_binary(expr);
            *out_result = tac_generate_expression_binary(tac, expr);
        } break;
        case AST_EXPRESSION_VARIABLE: {
            assert(0);
        } break;
        case AST_EXPRESSION_LITERAL_PRIMITIVE: {
            *out_result = tac_generate_expression_literal_primitive(expr);
        } break;
        default: break;
    }

    return 0;
}

void
tac_generate_assignment(Tac_State* state, Light_Ast* comm) {

}

Tac_Node* tac_generate_block(Light_Ast* block) {
    Tac_State* state = light_alloc(sizeof(Tac_State));
    state->nodes = array_new(Tac_Node*);

    for(s32 i = 0; i < block->comm_block.command_count; ++i) {
        Light_Ast* comm = block->comm_block.commands[i];
        switch(comm->kind) {
            case AST_COMMAND_ASSIGNMENT:  {
                tac_generate_assignment(state, comm);
            } break;
            case AST_COMMAND_BLOCK:
            case AST_COMMAND_BREAK:
            case AST_COMMAND_CONTINUE:
            case AST_COMMAND_FOR:
            case AST_COMMAND_IF:
            case AST_COMMAND_RETURN:
            case AST_COMMAND_WHILE:
            default: break;
        }
    }
}

Tac_Node* tac_generate_procedure(Light_Ast* proc) {
    // Generate the activation record

    if(proc->decl_proc.body) {
        tac_generate_block(proc->decl_proc.body);
    }
}

Tac_Node* tac_generate(Light_Ast** ast) {
    for(u64 i = 0; i < array_length(ast); ++i) {
        Light_Ast* n = ast[i];
        switch(n->kind) {
            case AST_DECL_PROCEDURE: {
                tac_generate_procedure(n);
            } break;
            case AST_DECL_VARIABLE: {

            } break;
            default: break;
        }
    }
}