#include "tac.h"
#include "utils/allocator.h"
#include "light_array.h"
#include <assert.h>
#include <stdio.h>
#include "type.h"

Tac_Node* tac_generate_expression(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result, bool eval);
void tac_print(Tac_State* tac);

const u32 TAC_FLAG_CADDR = (1 << 0);
const u32 TAC_FLAG_LVALUE = (1 << 1);
const u32 TAC_FLAG_RVALUE = (1 << 2);
const u32 TAC_FLAG_INDIRECT_WRITE = (1 << 3);
const u32 TAC_FLAG_INDIRECT_READ = (1 << 4);

Tac_Atom tac_new_temporary() {
    static start = 1;
    Tac_Atom result = {0};
    result.type = TAC_ATOM_TEMPORARY;
    result.temporary_number = start++;
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

Tac_Atom* tac_result_atom_ptr(Tac_Node* node) {
    Tac_Atom* result = 0;
    switch(node->kind) {
        case TAC_EXPRESSION_BINARY_FLOAT: result = &node->binop_float.left; break;
        case TAC_EXPRESSION_BINARY_INT: result = &node->binop_int.left; break;
        case TAC_EXPRESSION_UNARY:  result = &node->unop.left; break;
        case TAC_EXPRESSION_ASSIGN_INT: result = &node->assign_int.left; break;
        case TAC_EXPRESSION_ASSIGN_FLOAT: result = &node->assign_float.left; break;
    }
    return result;
}

Tac_Node*
tac_generate_expression_binop_float(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result) {
    assert(expr->kind == AST_EXPRESSION_BINARY);
    assert(type_primitive_float(expr->type));

    Tac_Node* node = light_alloc(sizeof(Tac_Node));
    node->kind = TAC_EXPRESSION_BINARY_FLOAT;
    node->binop_float.op = expr->expr_binary.op;
    node->binop_float.size_bytes = expr->type->size_bits >> 3;
    node->binop_float.left = tac_new_temporary();
    node->binop_float.left.flags |= TAC_FLAG_RVALUE;
    *out_result = node->binop_float.left;

    tac_generate_expression(tac, expr->expr_binary.left, &node->binop_float.e1, true);
    tac_generate_expression(tac, expr->expr_binary.right, &node->binop_float.e2, true);

    return node;
}

Tac_Node*
tac_generate_expression_binop_ptr(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result) {
    assert(expr->kind == AST_EXPRESSION_BINARY);
    assert(expr->type->kind == TYPE_KIND_POINTER || expr->type->kind == TYPE_KIND_ARRAY);
    
    Tac_Node* node = light_alloc(sizeof(Tac_Node));
    node->kind = TAC_EXPRESSION_BINARY_INT;
    node->binop_int.op = expr->expr_binary.op;
    node->binop_int.size_bytes = expr->type->size_bits >> 3;
    node->binop_int.left = tac_new_temporary();
    *out_result = node->binop_int.left;

    tac_generate_expression(tac, expr->expr_binary.left, &node->binop_int.e1, true);
    tac_generate_expression(tac, expr->expr_binary.right, &node->binop_int.e2, true);

    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS: {
            // right type must be an integer
            node->binop_int.left.flags |= TAC_FLAG_LVALUE;
        } break;
        case OP_BINARY_MINUS:
        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL: {
            // comparisons and difference
            node->binop_int.left.flags |= TAC_FLAG_RVALUE;
        } break;
        default: assert(0); break;
    }
    array_push(tac->nodes, node);
    return node;
}

Tac_Node*
tac_generate_expression_binop_integer(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result) {
    assert(expr->kind == AST_EXPRESSION_BINARY);
    assert(type_primitive_int(expr->type) || type_primitive_bool(expr->type));

    Tac_Node* node = light_alloc(sizeof(Tac_Node));
    node->kind = TAC_EXPRESSION_BINARY_INT;
    node->binop_int.op = expr->expr_binary.op;
    node->binop_int.size_bytes = expr->type->size_bits >> 3;
    node->binop_int.left = tac_new_temporary();
    node->binop_int.left.flags |= TAC_FLAG_RVALUE;
    *out_result = node->binop_int.left;

    tac_generate_expression(tac, expr->expr_binary.left, &node->binop_int.e1, true);
    tac_generate_expression(tac, expr->expr_binary.right, &node->binop_int.e2, true);

    array_push(tac->nodes, node);

    return node;
}

Tac_Node*
tac_generate_expression_unop_simple(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result) {
    assert(expr->kind == AST_EXPRESSION_UNARY);
    assert(expr->expr_unary.op == OP_UNARY_BITWISE_NOT ||
        expr->expr_unary.op == OP_UNARY_LOGIC_NOT ||
        expr->expr_unary.op == OP_UNARY_MINUS ||
        expr->expr_unary.op == OP_UNARY_PLUS);

    Tac_Node* node = light_alloc(sizeof(Tac_Node));
    node->kind = TAC_EXPRESSION_UNARY;
    node->unop.op = expr->expr_unary.op;
    node->unop.size_bytes = expr->type->size_bits >> 3;
    node->unop.left = tac_new_temporary();
    node->unop.left.flags |= TAC_FLAG_RVALUE;
    *out_result = node->unop.left;
    
    tac_generate_expression(tac, expr->expr_unary.operand, &node->unop.e, true);

    array_push(tac->nodes, node);

    return node;
}

Tac_Node*
tac_generate_expression_deref(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result, bool eval) {
    assert(expr->kind == AST_EXPRESSION_UNARY);
    assert(expr->expr_unary.op == OP_UNARY_DEREFERENCE);
    if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_ADDRESSOF) {
        return tac_generate_expression(tac, expr->expr_unary.operand->expr_unary.operand, out_result, true);
    }
    Tac_Atom operand_result = {0};
    Tac_Node* node = tac_generate_expression(tac, expr->expr_unary.operand, &operand_result, true);
    
    if(eval) {
        // dereference for real
        Tac_Node* deref_node = light_alloc(sizeof(Tac_Node));
        deref_node->kind = TAC_EXPRESSION_ASSIGN_INT;
        deref_node->flags = (operand_result.flags & TAC_FLAG_LVALUE) ? TAC_FLAG_LVALUE : TAC_FLAG_RVALUE;
        deref_node->assign_int.left = tac_new_temporary();
        operand_result.flags |= TAC_FLAG_INDIRECT_READ;
        deref_node->assign_int.e = operand_result;
        deref_node->assign_int.size_bytes = expr->type->size_bits >> 3;
        *out_result = deref_node->assign_int.left;

        array_push(tac->nodes, deref_node);
        node = deref_node;
    } else if (node) {
        Tac_Atom* a = tac_result_atom_ptr(node);
        *out_result = *a;
    }
    return node;
}

Tac_Node*
tac_generate_expression_address_of(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result, bool eval) {
    assert(expr->kind == AST_EXPRESSION_UNARY);
    assert(expr->expr_unary.op == OP_UNARY_ADDRESSOF);
    if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_DEREFERENCE) {
        return tac_generate_expression(tac, expr->expr_unary.operand->expr_unary.operand, out_result, false);
    }

    Tac_Node* node = tac_generate_expression(tac, expr->expr_unary.operand, out_result, false);
    assert(out_result->flags & TAC_FLAG_CADDR);
    if (node) {
        Tac_Atom* a = tac_result_atom_ptr(node);
        *a = *out_result;
    }
    return node;
}

Tac_Node*
tac_generate_expression_unop(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result, bool eval) {
    switch(expr->expr_unary.op) {
        case OP_UNARY_PLUS:
        case OP_UNARY_MINUS:
        case OP_UNARY_BITWISE_NOT:
        case OP_UNARY_LOGIC_NOT:
            return tac_generate_expression_unop_simple(tac, expr, out_result);
        case OP_UNARY_DEREFERENCE:
            return tac_generate_expression_deref(tac, expr, out_result, eval);
        case OP_UNARY_ADDRESSOF:
            return tac_generate_expression_address_of(tac, expr, out_result, eval);
        case OP_UNARY_CAST:
        default: assert(0); break;
    }
    return 0;
}

Tac_Atom tac_generate_expression_literal_primitive(Light_Ast* expr) {
    assert(expr->kind == AST_EXPRESSION_LITERAL_PRIMITIVE);
    Tac_Atom atom = {0};
    atom.flags = TAC_FLAG_RVALUE;
    
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
                    atom.const_int.byte_size = expr->type->size_bits >> 3;
                    atom.byte_size = atom.const_int.byte_size;
                } break;
                case TYPE_PRIMITIVE_R32: {
                    atom.type = TAC_ATOM_CONSTANT_FLOAT;
                    atom.const_float.val_r32 = expr->expr_literal_primitive.value_r32;
                    atom.const_float.byte_size = 4;
                    atom.byte_size = atom.const_float.byte_size;
                } break;
                case TYPE_PRIMITIVE_R64: {
                    atom.type = TAC_ATOM_CONSTANT_FLOAT;
                    atom.const_float.val_r64 = expr->expr_literal_primitive.value_r64;
                    atom.const_float.byte_size = 8;
                    atom.byte_size = atom.const_float.byte_size;
                } break;
                case TYPE_PRIMITIVE_BOOL: {
                    atom.type = TAC_ATOM_CONSTANT_INTEGER;
                    atom.const_int.val_u64 = 1;
                    atom.const_int.byte_size = 1;
                    atom.byte_size = atom.const_int.byte_size;
                } break;
                default: assert(0); break;
            }
        } break;
        case TYPE_KIND_POINTER: {
            atom.type = TAC_ATOM_CONSTANT_INTEGER;
            atom.const_int.val_u64 = 0;
            atom.const_int.byte_size = 8;
            atom.byte_size = atom.const_int.byte_size;
            atom.flags = TAC_FLAG_CADDR;
        } break;
        default: break;
    }

    return atom;
}

Tac_Node* 
tac_generate_expression_binary(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result) {
    if(type_primitive_int(expr->type) || type_primitive_bool(expr->type)) {
        return tac_generate_expression_binop_integer(tac, expr, out_result);
    } else if(type_primitive_float(expr->type)) {
        return tac_generate_expression_binop_float(tac, expr, out_result);
    } else {
        // pointer arithmetic
        return tac_generate_expression_binop_ptr(tac, expr, out_result);
    }
    return 0;
}

Tac_Node*
tac_generate_expression_vector_access(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result) {
    assert(expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS);

    Tac_Atom left_result = {0}, right_result = {0};
    Tac_Node* nl = tac_generate_expression(tac, expr->expr_binary.left, &left_result, true);
    Tac_Node* nr = tac_generate_expression(tac, expr->expr_binary.right, &right_result, true);

}

void 
tac_generate_expression_variable(Tac_State* tac, Light_Ast* expr, Tac_Atom* out_result, bool eval) {
    assert(expr->kind == AST_EXPRESSION_VARIABLE);
    assert(out_result);

    out_result->type = TAC_ATOM_VARIABLE;
    out_result->variable_hash = fnv_1_hash(expr->expr_variable.name->data, expr->expr_variable.name->length);
    out_result->variable = expr->expr_variable.name;
    out_result->byte_size = expr->type->size_bits >> 3;
    out_result->flags = (eval) ? TAC_FLAG_LVALUE : TAC_FLAG_CADDR;

    // TODO(psv): stack allocation
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
            return tac_generate_expression_unop(tac, expr, out_result, eval);
        } break;
        case AST_EXPRESSION_BINARY: {
            if(expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS) {
                return tac_generate_expression_vector_access(tac, expr, out_result);
            } else {
                return tac_generate_expression_binary(tac, expr, out_result);
            }
        } break;
        case AST_EXPRESSION_VARIABLE: {
            tac_generate_expression_variable(tac, expr, out_result, eval);
        } break;
        case AST_EXPRESSION_LITERAL_PRIMITIVE: {
            *out_result = tac_generate_expression_literal_primitive(expr);
        } break;
        default: break;
    }

    return 0;
}

void
tac_generate_assignment(Tac_State* tac, Light_Ast* comm) {
    assert(comm->kind == AST_COMMAND_ASSIGNMENT);

    Tac_Atom lvalue_result = {0};
    Tac_Node* lvalue_node = tac_generate_expression(tac, comm->comm_assignment.lvalue, &lvalue_result, false);
    lvalue_result.flags |= TAC_FLAG_INDIRECT_WRITE;

    Tac_Atom rvalue_result = { 0 };
    Tac_Node* rvalue_node = tac_generate_expression(tac, comm->comm_assignment.rvalue, &rvalue_result, true);

    if (rvalue_node) {
        Tac_Atom* atom = tac_result_atom_ptr(rvalue_node);
        *atom = lvalue_result;
    }

    if ((!lvalue_node && !rvalue_node) || (lvalue_node && !rvalue_node)) {
        // this is a simple assign
        Tac_Node* node = light_alloc(sizeof(Tac_Node));
        Light_Type* ltype = comm->comm_assignment.lvalue->type;
        if (type_primitive_int(ltype) || type_primitive_bool(ltype)) {
            node->kind = TAC_EXPRESSION_ASSIGN_INT;
            node->assign_int.left = lvalue_result;
            node->assign_int.e = rvalue_result;
        } else if (type_primitive_float(ltype)) {
            node->kind = TAC_EXPRESSION_ASSIGN_FLOAT;
            node->assign_float.left = lvalue_result;
            node->assign_float.e = rvalue_result;
        } else {
            node->kind = TAC_EXPRESSION_ASSIGN_INT;
            node->assign_int.left = lvalue_result;
            node->assign_int.e = rvalue_result;
        }
        array_push(tac->nodes, node);
    }
}

void
tac_generate_block(Tac_State* tac, Light_Ast* block) {
    for(s32 i = 0; i < block->comm_block.command_count; ++i) {
        Light_Ast* comm = block->comm_block.commands[i];
        switch(comm->kind) {
            case AST_COMMAND_ASSIGNMENT:  {
                tac_generate_assignment(tac, comm);
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

void tac_generate_procedure(Tac_State* tac, Light_Ast* proc) {
    // Generate the activation record

    if(proc->decl_proc.body) {
        tac_generate_block(tac, proc->decl_proc.body);
    }
}

Tac_State tac_generate(Light_Ast** ast) {
    Tac_State tac = { 0 };
    tac.nodes = array_new(Tac_Node*);

    for(u64 i = 0; i < array_length(ast); ++i) {
        Light_Ast* n = ast[i];
        switch(n->kind) {
            case AST_DECL_PROCEDURE: {
                tac_generate_procedure(&tac, n);
            } break;
            case AST_DECL_VARIABLE: {

            } break;
            default: break;
        }
    }

    tac_print(&tac);
    return tac;
}

/*
    Printing
*/
s32 tac_print_atom(Tac_Atom* atom) {
    s32 length = 0;
    if(atom->flags & TAC_FLAG_LVALUE) {
        printf("[lvalue]");
        if (atom->flags & TAC_FLAG_INDIRECT_WRITE) printf("[]");
        if (atom->flags & TAC_FLAG_INDIRECT_READ) printf("()");
    } else if (atom->flags & TAC_FLAG_CADDR) {
        printf("[caddr]");
        if (atom->flags & TAC_FLAG_INDIRECT_WRITE) printf("[]");
        if (atom->flags & TAC_FLAG_INDIRECT_READ) printf("()");
    } else if (atom->flags & TAC_FLAG_RVALUE) {
        if (atom->flags & TAC_FLAG_INDIRECT_READ) printf("()");
    }
    switch (atom->type) {
        case TAC_ATOM_CONSTANT_FLOAT: {
            if(atom->const_float.byte_size == 4)
                length += printf("%f", atom->const_float.val_r32);
            else
                length += printf("%f", atom->const_float.val_r64);
        } break;
        case TAC_ATOM_CONSTANT_INTEGER: {
            if(atom->is_signed) {
                switch(atom->byte_size) {
                    case 1:
                        length += printf("%d", atom->const_int.val_s8); break;
                    case 2:
                        length += printf("%d", atom->const_int.val_s16); break;
                    case 4:
                        length += printf("%d", atom->const_int.val_s32); break;
                    case 8:
                        length += printf("%lld", atom->const_int.val_s64); break;
                    default: assert(0); break;
                }
            } else {
                switch(atom->byte_size) {
                    case 1:
                        length += printf("%u", atom->const_int.val_u8); break;
                    case 2:
                        length += printf("%u", atom->const_int.val_u16); break;
                    case 4:
                        length += printf("%u", atom->const_int.val_u32); break;
                    case 8:
                        length += printf("%llu", atom->const_int.val_u64); break;
                    default: assert(0); break;
                }
            }
        } break;
        case TAC_ATOM_TEMPORARY: {
            length += printf("t%lld", atom->temporary_number);
        } break;
        case TAC_ATOM_VARIABLE: {
            //length += printf("v_%llx", atom->variable_hash);
            length += printf("%.*s", atom->variable->length, atom->variable->data);
        } break;
        default: break;
    }
    return length;
}

void tac_print(Tac_State* tac) {
    for (s32 i = 0; i < array_length(tac->nodes); ++i) {
        Tac_Node* n = tac->nodes[i];
        switch (tac->nodes[i]->kind) {
            case TAC_EXPRESSION_BINARY_INT: {
                tac_print_atom(&n->binop_int.left);
                printf(" = ");
                tac_print_atom(&n->binop_int.e1);
                ast_print_binary_op(n->binop_int.op, LIGHT_AST_PRINT_STDOUT);
                tac_print_atom(&n->binop_int.e2);
            } break;
            case TAC_EXPRESSION_BINARY_FLOAT: {
                tac_print_atom(&n->binop_float.left);
                printf(" = ");
                tac_print_atom(&n->binop_float.e1);
                ast_print_binary_op(n->binop_float.op, LIGHT_AST_PRINT_STDOUT);
                tac_print_atom(&n->binop_float.e2);
            } break;
            case TAC_EXPRESSION_ASSIGN_INT: {
                tac_print_atom(&n->assign_int.left);
                printf(" = ");
                tac_print_atom(&n->assign_int.e);
            } break;
            case TAC_EXPRESSION_ASSIGN_FLOAT: {
                tac_print_atom(&n->assign_float.left);
                printf(" = ");
                tac_print_atom(&n->assign_float.e);
            } break;
            case TAC_EXPRESSION_UNARY: {
                tac_print_atom(&n->unop.left);
                printf(" = ");
                switch(n->unop.op) {
                    case OP_UNARY_PLUS:
                        printf("+"); break;
                    case OP_UNARY_MINUS:
                        printf("-"); break;
                    case OP_UNARY_BITWISE_NOT:
                    case OP_UNARY_LOGIC_NOT:
                        printf("~"); break;
                }
                tac_print_atom(&n->unop.e);
            } break;
            default: break;
        }
        printf("\n");
    }
}