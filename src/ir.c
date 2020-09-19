#include "utils/allocator.h"
#include "light_array.h"
#include <assert.h>
#include <stdio.h>
#include "type.h"
#include "ir.h"

int ir_gen_expr(IR_Generator* gen, Light_Ast* expr);

static int
ir_new_temp(IR_Generator* gen) {
    return gen->temp++;
}

void
ir_gen_decl(IR_Generator* gen, Light_Ast* decl)
{
    if(decl->kind == AST_DECL_VARIABLE) {
        decl->decl_variable.ir_temporary = ir_new_temp(gen);
        decl->decl_variable.stack_index = gen->ar.index++;
        decl->decl_variable.stack_offset = gen->ar.offset;
        gen->ar.offset += (decl->decl_variable.type->size_bits / 8);
    }
}

int
ir_gen_expr_unary(IR_Generator* gen, Light_Ast* expr)
{
    return 0;
}

int
ir_gen_expr_binary(IR_Generator* gen, Light_Ast* expr)
{
    int t1 = ir_gen_expr(gen, expr->expr_binary.left);
    int t2 = ir_gen_expr(gen, expr->expr_binary.right);
    int t3 = ir_new_temp(gen);

    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:    fprintf(stdout, "ADD"); break;
        case OP_BINARY_MINUS:   fprintf(stdout, "SUB"); break;
        case OP_BINARY_MULT:    fprintf(stdout, "MUL"); break;
        case OP_BINARY_DIV:     fprintf(stdout, "DIV"); break;
        case OP_BINARY_MOD:     fprintf(stdout, "MOD"); break;
        case OP_BINARY_AND:     fprintf(stdout, "AND"); break;
        case OP_BINARY_OR:      fprintf(stdout, "OR"); break;
        case OP_BINARY_XOR:     fprintf(stdout, "XOR"); break;
        case OP_BINARY_SHL:     fprintf(stdout, "SHL"); break;
        case OP_BINARY_SHR:     fprintf(stdout, "SHR"); break;
        default: break; // TODO(psv): implement
    }
    fprintf(stdout, " t%d, t%d -> t%d\n", t1, t2, t3);

    return t3;
}

int
ir_gen_expr_lit_prim(IR_Generator* gen, Light_Ast* expr)
{
    int t = ir_new_temp(gen);
    switch(expr->type->primitive) {
        case TYPE_PRIMITIVE_S8:
            fprintf(stdout, "LOAD byte %d -> t%d\n", expr->expr_literal_primitive.value_s8, t);
            break;
        case TYPE_PRIMITIVE_S16:
            fprintf(stdout, "LOAD word %d -> t%d\n", expr->expr_literal_primitive.value_s16, t);
            break;
        case TYPE_PRIMITIVE_S32:
            fprintf(stdout, "LOAD dword %d -> t%d\n", expr->expr_literal_primitive.value_s32, t);
            break;
        case TYPE_PRIMITIVE_S64:
            fprintf(stdout, "LOAD qword %ld -> t%d\n", expr->expr_literal_primitive.value_s64, t);
            break;
        case TYPE_PRIMITIVE_U8:
            fprintf(stdout, "LOAD byte 0x%x -> t%d\n", expr->expr_literal_primitive.value_u8, t);
            break;
        case TYPE_PRIMITIVE_U16:
            fprintf(stdout, "LOAD byte 0x%x -> t%d\n", expr->expr_literal_primitive.value_u16, t);
            break;
        case TYPE_PRIMITIVE_U32:
            fprintf(stdout, "LOAD byte 0x%x -> t%d\n", expr->expr_literal_primitive.value_u32, t);
            break;
        case TYPE_PRIMITIVE_U64:
            fprintf(stdout, "LOAD byte 0x%lx -> t%d\n", expr->expr_literal_primitive.value_u64, t);
            break;
        case TYPE_PRIMITIVE_R32:
            fprintf(stdout, "LOAD %ff -> t%d\n", expr->expr_literal_primitive.value_r32, t);
            break;
        case TYPE_PRIMITIVE_R64:
            fprintf(stdout, "LOAD byte %ff -> t%d\n", expr->expr_literal_primitive.value_r64, t);
            break;
        case TYPE_PRIMITIVE_BOOL:
            fprintf(stdout, "LOAD byte %d -> t%d\n", expr->expr_literal_primitive.value_bool, t);
            break;
        default: break;
    }
    return t;
}

int
ir_gen_expr_variable(IR_Generator* gen, Light_Ast* expr)
{
    Light_Ast_Decl_Variable* decl = &expr->expr_variable.decl->decl_variable;

    // if it is not loaded in a temporary, then load it
    if(!(decl->flags & DECL_VARIABLE_FLAG_LOADED))
    {
        fprintf(stdout, "LOAD SP+%d -> t%d\n", decl->stack_index, decl->ir_temporary);
        decl->flags |= DECL_VARIABLE_FLAG_LOADED;
    }
    return decl->ir_temporary;
}

int
ir_gen_expr(IR_Generator* gen, Light_Ast* expr)
{
    switch(expr->kind)
    {
        case AST_EXPRESSION_LITERAL_PRIMITIVE: return ir_gen_expr_lit_prim(gen, expr);
        case AST_EXPRESSION_VARIABLE:          return ir_gen_expr_variable(gen, expr);
        case AST_EXPRESSION_BINARY:            return ir_gen_expr_binary(gen, expr);
        case AST_EXPRESSION_UNARY:             return ir_gen_expr_unary(gen, expr);
        default: break;
    }
    return -1;
}

void
ir_gen_comm_assignment(IR_Generator* gen, Light_Ast_Comm_Assignment* comm)
{
    int t1 = ir_gen_expr(gen, comm->lvalue);
    int t2 = ir_gen_expr(gen, comm->rvalue);
    // emit store t2 -> t1
    fprintf(stdout, "STORE t%d -> t%d\n", t2, t1);
}

void
ir_gen_comm(IR_Generator* gen, Light_Ast* comm)
{
    switch(comm->kind) {
        case AST_COMMAND_ASSIGNMENT: 
            ast_print_node(comm, LIGHT_AST_PRINT_STDOUT, 0);
            fprintf(stdout, "\n");
            ir_gen_comm_assignment(gen, &comm->comm_assignment); break;
        default: break;
    }
}

void
ir_gen_proc(Light_Ast* proc)
{
    Light_Ast* body = proc->decl_proc.body;

    IR_Generator gen = {0};
    gen.instructions = array_new(IR_Instruction);
    gen.temp = 0;
    gen.ar.index = 0;
    gen.ar.offset = 0;

    for(int i = 0; i < body->comm_block.command_count; ++i) {
        Light_Ast* comm = body->comm_block.commands[i];
        if(comm->flags & AST_FLAG_COMMAND)
            ir_gen_comm(&gen, comm);
        else if(comm->flags & AST_FLAG_DECLARATION)
            ir_gen_decl(&gen, comm);
    }
}

void ir_generate(Light_Ast** ast) {
    for(u64 i = 0; i < array_length(ast); ++i) {
        Light_Ast* n = ast[i];
        switch(n->kind) {
            case AST_DECL_PROCEDURE: {
                ir_gen_proc(n);
            } break;
            case AST_DECL_VARIABLE: {

            } break;
            default: break;
        }
    }
}