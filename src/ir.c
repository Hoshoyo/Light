#include "utils/allocator.h"
#include "light_array.h"
#include <assert.h>
#include <stdio.h>
#include "type.h"
#include "ir.h"

int ir_gen_expr(IR_Generator* gen, Light_Ast* expr);

static int
ir_new_temp(IR_Generator* gen) {
    return gen->temp_int++;
}
static int
ir_new_tempf(IR_Generator* gen) {
    return gen->temp_float++;
}

// *****************************************************
// *****************************************************
// ***************** AST Generate **********************
// *****************************************************
// *****************************************************

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
    bool lval = expr->flags & AST_FLAG_EXPRESSION_LVALUE;

    int t1 = ir_gen_expr(gen, expr->expr_unary.operand);
    int t2 = ir_new_temp(gen);

    if ((expr->expr_unary.operand->flags & AST_FLAG_EXPRESSION_LVALUE) && expr->expr_unary.op != OP_UNARY_ADDRESSOF)
        iri_emit_load(gen, t1, t2, (IR_Value) { 0 }, expr->type->size_bits / 8);

    switch(expr->expr_unary.op)
    {
        case OP_UNARY_DEREFERENCE: {
            // only perform a dereference when it is an lvalue
        } break;

        case OP_UNARY_PLUS:
        case OP_UNARY_ADDRESSOF: // nothing need to be done
            break;
        
        case OP_UNARY_MINUS: {
            iri_emit_neg(gen, t1, t2, expr->type->size_bits / 8);
        } break;
        case OP_UNARY_BITWISE_NOT: {
            iri_emit_not(gen, t1, t2, expr->type->size_bits / 8);
        } break;
        case OP_UNARY_LOGIC_NOT: {
            iri_emit_logic_not(gen, t1, t2, expr->type->size_bits / 8);
        } break;

        case OP_UNARY_CAST: {
            // TODO(psv): implement
        } break;

        default: break;
    }
    return t2;
}

int
ir_gen_expr_binary(IR_Generator* gen, Light_Ast* expr)
{
    int t1 = ir_gen_expr(gen, expr->expr_binary.left);
    int t2 = ir_gen_expr(gen, expr->expr_binary.right);
    int t3 = ir_new_temp(gen);

    if(expr->expr_binary.left->flags & AST_FLAG_EXPRESSION_LVALUE)
        iri_emit_load(gen, t1, t1, (IR_Value){0}, (int)type_pointer_size_bits() / 8);
    if(expr->expr_binary.right->flags & AST_FLAG_EXPRESSION_LVALUE)
        iri_emit_load(gen, t2, t2, (IR_Value){0}, (int)type_pointer_size_bits() / 8);

    int byte_size = ((expr->expr_binary.left->flags & AST_FLAG_EXPRESSION_LVALUE)) ? 
        ((int)type_pointer_size_bits() / 8) : (expr->type->size_bits / 8);

    IR_Instruction_Type instr_type = IR_NONE;
    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:      instr_type = IR_ADD; break;
        case OP_BINARY_MINUS:     instr_type = IR_SUB; break;
        case OP_BINARY_MULT:      instr_type = IR_MUL; break;
        case OP_BINARY_DIV:       instr_type = IR_DIV; break;
        case OP_BINARY_MOD:       instr_type = IR_MOD; break;
        case OP_BINARY_AND:       instr_type = IR_AND; break;
        case OP_BINARY_OR:        instr_type = IR_OR; break;
        case OP_BINARY_XOR:       instr_type = IR_XOR; break;
        case OP_BINARY_SHL:       instr_type = IR_SHL; break;
        case OP_BINARY_SHR:       instr_type = IR_SHR; break;
        case OP_BINARY_LOGIC_AND: instr_type = IR_LAND; break;
        case OP_BINARY_LOGIC_OR:  instr_type = IR_LOR; break;
        case OP_BINARY_VECTOR_ACCESS: instr_type = IR_ADD; break;
        default: break; // TODO(psv): implement
    }
    iri_emit_arith(gen, instr_type, t1, t2, t3, (IR_Value){0}, byte_size);

    return t3;
}

int
ir_gen_expr_lit_prim(IR_Generator* gen, Light_Ast* expr)
{
    int t = ir_new_temp(gen);
    IR_Value value = {0};
    value.v_u64 = expr->expr_literal_primitive.value_u64; // this forced any type to be inside the union correctly
    switch(expr->type->primitive) {
        case TYPE_PRIMITIVE_S8:   value.type = IR_VALUE_S8;  break;
        case TYPE_PRIMITIVE_S16:  value.type = IR_VALUE_S16; break;
        case TYPE_PRIMITIVE_S32:  value.type = IR_VALUE_S32; break;
        case TYPE_PRIMITIVE_S64:  value.type = IR_VALUE_S64; break;
        case TYPE_PRIMITIVE_U8:   value.type = IR_VALUE_U8;  break;
        case TYPE_PRIMITIVE_U16:  value.type = IR_VALUE_U16; break;
        case TYPE_PRIMITIVE_U32:  value.type = IR_VALUE_U32; break;
        case TYPE_PRIMITIVE_U64:  value.type = IR_VALUE_U64; break;
        case TYPE_PRIMITIVE_R32:  value.type = IR_VALUE_R32; break;
        case TYPE_PRIMITIVE_R64:  value.type = IR_VALUE_R64; break;
        case TYPE_PRIMITIVE_BOOL: value.type = IR_VALUE_U8; break;
        default: break;
    }
    iri_emit_load(gen, -1, t, value, iri_value_byte_size(value));

    return t;
}

int
ir_gen_expr_variable(IR_Generator* gen, Light_Ast* expr)
{
    Light_Ast_Decl_Variable* decl = &expr->expr_variable.decl->decl_variable;

    // if it is not loaded in a temporary, then load it
    if(!(decl->flags & DECL_VARIABLE_FLAG_LOADED))
    {
        // LOAD SP+imm -> t
        iri_emit_load(gen, IR_REG_STACK_PTR, decl->ir_temporary, 
            (IR_Value){.v_s32 = decl->stack_offset, .type = IR_VALUE_S32},
            decl->type->size_bits / 8);
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
    // STORE t2 -> t1
    iri_emit_store(gen, t2, t1, (IR_Value){0}, comm->rvalue->type->size_bits / 8);
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
    gen.temp_int = 0;
    gen.temp_float = 0;
    gen.ar.index = 0;
    gen.ar.offset = 0;

    for(int i = 0; i < body->comm_block.command_count; ++i) {
        Light_Ast* comm = body->comm_block.commands[i];
        if(comm->flags & AST_FLAG_COMMAND)
            ir_gen_comm(&gen, comm);
        else if(comm->flags & AST_FLAG_DECLARATION)
            ir_gen_decl(&gen, comm);
    }
    iri_print_instructions(&gen);
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