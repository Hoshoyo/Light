#include "utils/allocator.h"
#include "light_array.h"
#include <assert.h>
#include <stdio.h>
#include "type.h"
#include "ir.h"

#define ARRAY_LENGTH(A) (sizeof(A) / sizeof(*A))
#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#define MIN(A, B) (((A) < (B)) ? (A) : (B))

int  ir_gen_expr(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset);
void ir_gen_comm(IR_Generator* gen, Light_Ast* comm);
void ir_gen_commands(IR_Generator* gen, Light_Ast** commands, int comm_count);
void ir_new_activation_record(IR_Generator* gen);

static IR_Activation_Rec*
ir_get_current_ar(IR_Generator* gen)
{
    return &gen->ars[array_length(gen->ars) - 1];
}

static IR_Reg
ir_new_temp(IR_Generator* gen) {
    IR_Activation_Rec* ar = ir_get_current_ar(gen);

    for(int i = 1; i < 4; ++i)
    {
        if(ar->reg_int[i] == 0)
        {
            ar->reg_int[i] = 1;
            return i;
        }
    }
    fprintf(stderr, "tried to allocate more than 3 integer registers\n");
    assert(0);
    return IR_REG_NONE;
}
static IR_Reg
ir_new_tempf(IR_Generator* gen) {
    IR_Activation_Rec* ar = ir_get_current_ar(gen);
    for(int i = 1; i < 8; ++i)
    {
        if(ar->reg_float[i] == 0)
        {
            ar->reg_float[i] = 1;
            return i + IR_REG_PROC_RETF;
        }
    }
    fprintf(stderr, "tried to allocate more than 8 float registers\n");
    assert(0);
    return IR_REG_NONE;
}

static void
ir_free_reg(IR_Generator* gen, IR_Reg r)
{
    IR_Activation_Rec* ar = ir_get_current_ar(gen);
    if(r > IR_REG_PROC_RETF)
    {
        r = r - IR_REG_PROC_RETF;
        ar->reg_float[r] = 0;
    }
    else if(r < IR_REG_PROC_RETF)
    {
        ar->reg_int[r] = 0;
    }
    else
        assert(0);
}

static IR_Reg
ir_new_reg(IR_Generator* gen, Light_Type* type) {
    return (type_primitive_float(type)) ? ir_new_tempf(gen) : ir_new_temp(gen);
}

// X86
#define IR_TO_X86 1
void
ir_gen_x86_epilogue(IR_Generator* gen)
{
    // mov esp, ebp
    iri_emit_mov(gen, IR_REG_STACK_BASE, IR_REG_STACK_PTR, (IR_Value){0}, type_pointer_size_bytes(), false);
    // pop ebp
    iri_emit_pop(gen, IR_REG_STACK_BASE, type_pointer_size_bytes(), false);
}

// *****************************************************
// *****************************************************
// ***************** AST Generate **********************
// *****************************************************
// *****************************************************

void
ir_gen_load(IR_Generator* gen, Light_Ast* expr, IR_Reg src, IR_Reg dst)
{
    Light_Type* type = type_alias_root(expr->type);
    if(type->kind == TYPE_KIND_PRIMITIVE)
        iri_emit_load(gen, src, dst, (IR_Value){0}, type_pointer_size_bytes(), type->size_bits / 8, type_primitive_float(type));
    else
    {
        // TODO(psv): bigger types
    }
}

// requires r0 to be zeroed
void
ir_gen_variable_initialization(IR_Generator* gen, Light_Ast* decl)
{
    IR_Activation_Rec* ar = ir_get_current_ar(gen);
    Light_Type* type = type_alias_root(decl->decl_variable.type);

    int soff = decl->decl_variable.stack_offset;

    if(type->size_bits == 8 || type->size_bits == 16 || type->size_bits == 32)
    {
        // emit normal store r0 -> sb + imm
        iri_emit_store(gen, IR_REG_PROC_RET, IR_REG_STACK_BASE, 
            iri_value_new_signed(4, decl->decl_variable.stack_offset), type->size_bits / 8, false);
    }
    else
    {
        // emit copy sb relative
        IR_Reg t = ir_new_temp(gen);
        iri_emit_mov(gen, IR_REG_NONE, t, iri_value_new_signed(4, type->size_bits / 8), type_pointer_size_bytes(), false);
        iri_emit_clear(gen, IR_REG_STACK_BASE, t, (IR_Value){.type = IR_VALUE_S32, .v_s32 = soff}, type_pointer_size_bytes());
        ir_free_reg(gen, t);
    }
}

void
ir_gen_decl(IR_Generator* gen, Light_Ast* decl)
{
    IR_Activation_Rec* ar = ir_get_current_ar(gen);

    if(decl->kind == AST_DECL_VARIABLE)
    {
        decl->decl_variable.stack_offset = ar->offset - decl->decl_variable.type->size_bits / 8;

        // Initialize with the default value
        // TODO(psv): for now just do 0
        ir_gen_variable_initialization(gen, decl);

        ar->offset -= (decl->decl_variable.type->size_bits / 8);

        //fprintf(stdout, "variable[SB+%d -(0x%x)] %.*s\n", decl->decl_variable.stack_offset, -decl->decl_variable.stack_offset,
        //    decl->decl_constant.name->length, decl->decl_constant.name->data);
    }
}

static IR_Reg
ir_gen_cvt_to_r32(IR_Generator* gen, Light_Ast* expr, IR_Reg op_temp)
{
    Light_Type* op_type = expr->expr_unary.operand->type;
    // u8, u16, u32, u64, s8, s16, s32, s64, r64
    IR_Reg t = ir_new_tempf(gen);
    if(type_primitive_sint(op_type))
    {
        // cast from sint to (r32)
        iri_emit_cvt(gen, IR_CVT_SI_R32, op_temp, t, op_type->size_bits / 8, 32);
    }
    else if(type_primitive_uint(op_type))
    {
        // cast from uint to (r32)
        iri_emit_cvt(gen, IR_CVT_UI_R32, op_temp, t, op_type->size_bits / 8, 32);
    }
    else if(type_primitive_float(op_type))
    {
        if(op_type->size_bits == 64)
        {
            iri_emit_cvt(gen, IR_CVT_R64_R32, op_temp, t, 64, 32);
        }
        // the other conversion is useless, since it is from r32 to r32
    }
    ir_free_reg(gen, op_temp);
    return t;
}

static IR_Reg
ir_gen_cvt_to_r64(IR_Generator* gen, Light_Ast* expr, IR_Reg op_temp)
{
    Light_Type* op_type = expr->expr_unary.operand->type;
    // u8, u16, u32, u64, s8, s16, s32, s64, r64
    IR_Reg t = ir_new_tempf(gen);
    if(type_primitive_sint(op_type))
    {
        // cast from sint to (r64)
        iri_emit_cvt(gen, IR_CVT_SI_R64, op_temp, t, op_type->size_bits / 8, 64);
    }
    else if(type_primitive_uint(op_type))
    {
        // cast from uint to (r64)
        iri_emit_cvt(gen, IR_CVT_UI_R64, op_temp, t, op_type->size_bits / 8, 64);
    }
    else if(type_primitive_float(op_type))
    {
        if(op_type->size_bits == 32)
        {
            iri_emit_cvt(gen, IR_CVT_R32_R64, op_temp, t, 32, 64);
        }
        // the other conversion is useless, since it is from r64 to r64
    }
    ir_free_reg(gen, op_temp);
    return t;
}

IR_Reg
ir_gen_cvt_to_bool(IR_Generator* gen, Light_Ast* expr, int op_temp)
{
    Light_Type* op_type = expr->expr_unary.operand->type;
    Light_Type* cast_type = expr->expr_unary.type_to_cast;
    if(type_alias_root(op_type) == type_alias_root(cast_type))
        return op_temp;

    IR_Reg t = ir_new_temp(gen);
    // zero register out since when false the value must be 0
    iri_emit_arith(gen, IR_XOR, t, t, t, (IR_Value){0}, cast_type->size_bits / 8);

    iri_emit_cmp(gen, op_temp, IR_REG_NONE, (IR_Value){.type = IR_VALUE_U8, .v_u8 = 0}, op_type->size_bits / 8, false);

    // mov 1 if the value is different from 0
    iri_emit_cmov(gen, IR_CMOVNE, IR_REG_NONE, t, (IR_Value){.type = IR_VALUE_U8, .v_u8 = 1}, cast_type->size_bits / 8);

    ir_free_reg(gen, op_temp);

    return t;
}

IR_Reg
ir_gen_cvt_to_int(IR_Generator* gen, Light_Ast* expr, int op_temp)
{
    IR_Reg t = IR_REG_NONE;

    Light_Type* op_type = expr->expr_unary.operand->type;
    Light_Type* cast_type = expr->expr_unary.type_to_cast;
    
    if(type_alias_root(op_type) == type_alias_root(cast_type))
        return op_temp;

    if(type_primitive_float(op_type))
    {
        IR_Instruction_Type type = IR_NONE;
        t = ir_new_temp(gen);
        if(op_type->size_bits == 32)
        {
            // -> r32
            type = (type_primitive_uint(cast_type)) ? IR_CVT_R32_UI : IR_CVT_R32_SI;
        }
        else
        {
            // -> r64
            type = (type_primitive_uint(cast_type)) ? IR_CVT_R64_UI : IR_CVT_R64_SI;
        }
        
        iri_emit_cvt(gen, type, op_temp, t, op_type->size_bits / 8, cast_type->size_bits / 8);
    }
    else
    {
        if(type_primitive_uint(cast_type) || type_primitive_bool(cast_type))
        {
            t = ir_new_temp(gen);
            iri_emit_cvt(gen, IR_CVT_UI, op_temp, t, op_type->size_bits / 8, cast_type->size_bits / 8);
        }
        else if(type_primitive_sint(cast_type))
        {
            t = ir_new_temp(gen);
            iri_emit_cvt(gen, IR_CVT_SI, op_temp, t, op_type->size_bits / 8, cast_type->size_bits / 8);
        }
        else if(cast_type->kind == TYPE_KIND_POINTER)
        {
            return op_temp;
        }
    }
    ir_free_reg(gen, op_temp);

    return t;
}

IR_Reg
ir_gen_expr_unary_cast(IR_Generator* gen, Light_Ast* expr, IR_Reg op_temp, bool load)
{
    Light_Ast* operand = expr->expr_unary.operand;
    Light_Type* tcast = type_alias_root(expr->expr_unary.type_to_cast);
    
    if(tcast->kind == TYPE_KIND_POINTER)
    {
        // int  -> ^T => convert if sizes dont match
        // ^S   -> ^T => do nothing
        // []V  -> ^T => do nothing
        if(type_primitive_int(operand->type))
        {
            return ir_gen_cvt_to_int(gen, expr, op_temp);
        }
    }
    else if(tcast->kind == TYPE_KIND_PRIMITIVE)
    {
        // TODO(psv): implement
        switch(tcast->primitive)
        {
            case TYPE_PRIMITIVE_R32:  return ir_gen_cvt_to_r32(gen, expr, op_temp);
            case TYPE_PRIMITIVE_R64:  return ir_gen_cvt_to_r64(gen, expr, op_temp);
            case TYPE_PRIMITIVE_S8:
            case TYPE_PRIMITIVE_S16:
            case TYPE_PRIMITIVE_S32:
            case TYPE_PRIMITIVE_S64:  return ir_gen_cvt_to_int(gen, expr, op_temp);
            case TYPE_PRIMITIVE_U8:
            case TYPE_PRIMITIVE_U16:
            case TYPE_PRIMITIVE_U32:
            case TYPE_PRIMITIVE_U64:  return ir_gen_cvt_to_int(gen, expr, op_temp);
            case TYPE_PRIMITIVE_BOOL: return ir_gen_cvt_to_bool(gen, expr, op_temp);
            default: break;
        }
    }
    return op_temp;
}

IR_Reg
ir_gen_expr_unary(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    bool lval = expr->flags & AST_FLAG_EXPRESSION_LVALUE;

    if (expr->expr_unary.op == OP_UNARY_ADDRESSOF)
        inside_literal = false;
    // when the operand is a variable and the op is address of,
    // the expression address must be loaded, instead of value
    IR_Reg t1 = ir_gen_expr(gen, expr->expr_unary.operand, (expr->expr_unary.op != OP_UNARY_ADDRESSOF), inside_literal, outer_offset);
    IR_Reg t2 = t1;

    switch(expr->expr_unary.op)
    {
        case OP_UNARY_DEREFERENCE: {
            // only perform a dereference when it is an lvalue
            if(load)
            {
                ir_free_reg(gen, t1);
                t2 = ir_new_reg(gen, expr->type);
                iri_emit_load(gen, t1, t2, (IR_Value){0}, type_pointer_size_bytes(), expr->type->size_bits / 8, type_primitive_float(expr->type));
            }
        } break;

        case OP_UNARY_ADDRESSOF:
            break;
        
        case OP_UNARY_PLUS:
            break;
        case OP_UNARY_MINUS: {
            t2 = ir_new_reg(gen, expr->type);
            iri_emit_neg(gen, t1, t2, expr->type->size_bits / 8, type_primitive_float(expr->type));
            ir_free_reg(gen, t1);
        } break;
        case OP_UNARY_BITWISE_NOT: {
            t2 = ir_new_reg(gen, expr->type);
            iri_emit_not(gen, t1, t2, expr->type->size_bits / 8);
            ir_free_reg(gen, t1);
        } break;
        case OP_UNARY_LOGIC_NOT: {
            t2 = ir_new_reg(gen, expr->type);
            iri_emit_logic_not(gen, t1, t2, expr->type->size_bits / 8);
            ir_free_reg(gen, t1);
        } break;

        case OP_UNARY_CAST: {
            t2 = ir_gen_expr_unary_cast(gen, expr, t1, load);
        } break;

        default: break;
    }
    return t2;
}

IR_Reg
ir_gen_expr_cond(IR_Generator* gen, Light_Ast* expr, IR_Reg t1, IR_Reg t2, IR_Reg t3, bool fp)
{
    int byte_size = expr->expr_binary.left->type->size_bits / 8;
    bool is_signed = type_primitive_sint(expr->expr_binary.left->type) || fp;
    
    // Zero out t3 to hold the result when comparison is false
    iri_emit_arith(gen, IR_XOR, t3, t3, t3, (IR_Value){0}, expr->type->size_bits / 8);

    // CMP t1, t2
    // CMPF tf1, tf2
    iri_emit_cmp(gen, t1, t2, (IR_Value){0}, byte_size, fp);
    ir_free_reg(gen, t1);
    ir_free_reg(gen, t2);

    IR_Value one = {.v_u8 = 1, .type = IR_VALUE_U8};

    switch(expr->expr_binary.op)
    {
        case OP_BINARY_LT:
            iri_emit_cmov(gen, (is_signed) ? IR_CMOVLS : IR_CMOVLU, IR_REG_NONE, t3, one, byte_size);
            break;
        case OP_BINARY_GT:
            iri_emit_cmov(gen, (is_signed) ? IR_CMOVGS : IR_CMOVGU, IR_REG_NONE, t3, one, byte_size);
            break;
        case OP_BINARY_LE:
            iri_emit_cmov(gen, (is_signed) ? IR_CMOVLES : IR_CMOVLEU, IR_REG_NONE, t3, one, byte_size);
            break;
        case OP_BINARY_GE:
            iri_emit_cmov(gen, (is_signed) ? IR_CMOVGES : IR_CMOVGEU, IR_REG_NONE, t3, one, byte_size);
            break;
        case OP_BINARY_EQUAL:
            iri_emit_cmov(gen, IR_CMOVEQ, IR_REG_NONE, t3, one, byte_size);
            break;
        case OP_BINARY_NOT_EQUAL:
            iri_emit_cmov(gen, IR_CMOVNE, IR_REG_NONE, t3, one, byte_size);
            break;
        default: break;
    }
    return t3;
}

IR_Reg
ir_gen_expr_vector_access(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    int type_size_bytes = expr->type->size_bits / 8;

    IR_Reg t1 = ir_gen_expr(gen, expr->expr_binary.left, (expr->expr_binary.left->type->kind == TYPE_KIND_POINTER), inside_literal, outer_offset);
    ir_free_reg(gen, t1);
    iri_emit_push(gen, t1, (IR_Value) { 0 }, type_pointer_size_bytes(), false);

    IR_Reg t2 = ir_gen_expr(gen, expr->expr_binary.right, true, inside_literal, outer_offset);

    t1 = ir_new_reg(gen, expr->expr_binary.left->type);

    iri_emit_pop(gen, t1, type_pointer_size_bytes(), false);
    IR_Reg t3 = ir_new_temp(gen);

    ir_free_reg(gen, t2);
    ir_free_reg(gen, t1);

    // multiply by the type size
    if(type_size_bytes > 1)
    {
        iri_emit_mov(gen, IR_REG_NONE, IR_REG_PROC_RET, (IR_Value){.type = IR_VALUE_S32, .v_s32 = type_size_bytes}, type_pointer_size_bytes(), false);
        // mul t2, imm -> t3
        iri_emit_arith(gen, IR_MUL, t2, IR_REG_PROC_RET, t3, (IR_Value){0}, type_pointer_size_bytes());
        iri_emit_arith(gen, IR_ADD, t3, t1, t3, (IR_Value){0}, type_pointer_size_bytes());
    }
    else
    {
        iri_emit_arith(gen, IR_ADD, t1, t2, t3, (IR_Value) { 0 }, type_pointer_size_bytes());
    }
    
    if(load)
    {
        if (type_primitive_float(expr->type))
        {
            ir_free_reg(gen, t3);
            IR_Reg t4 = ir_new_reg(gen, expr->type);
            iri_emit_load(gen, t3, t4, (IR_Value){0}, type_pointer_size_bytes(), expr->type->size_bits / 8, true);
            return t4;
        }
        else
        {
            if (expr->type->size_bits > type_pointer_size_bits())
            {

            }
            else
            {
                iri_emit_load(gen, t3, t3, (IR_Value){0}, type_pointer_size_bytes(), expr->type->size_bits / 8, type_primitive_float(expr->type));
            }
            return t3;
        }
    }
    return t3;
}

IR_Reg
ir_gen_expr_ptr_arith_sum(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    int type_size_bytes = expr->type->pointer_to->size_bits / 8;

    IR_Reg t1 = ir_gen_expr(gen, expr->expr_binary.left, (expr->expr_binary.left->type->kind == TYPE_KIND_POINTER), inside_literal, outer_offset);
    ir_free_reg(gen, t1);
    iri_emit_push(gen, t1, (IR_Value) { 0 }, type_pointer_size_bytes(), false);

    IR_Reg t2 = ir_gen_expr(gen, expr->expr_binary.right, true, inside_literal, outer_offset);

    t1 = ir_new_reg(gen, expr->expr_binary.left->type);

    iri_emit_pop(gen, t1, type_pointer_size_bytes(), false);
    IR_Reg t3 = ir_new_temp(gen);

    ir_free_reg(gen, t2);
    ir_free_reg(gen, t1);

    // multiply by the type size
    if(type_size_bytes > 1)
    {
        iri_emit_mov(gen, IR_REG_NONE, IR_REG_PROC_RET, (IR_Value){.type = IR_VALUE_S32, .v_s32 = type_size_bytes}, type_pointer_size_bytes(), false);
        // mul t2, imm -> t3
        iri_emit_arith(gen, IR_MUL, t2, IR_REG_PROC_RET, t3, (IR_Value){0}, type_pointer_size_bytes());
        iri_emit_arith(gen, IR_ADD, t3, t1, t3, (IR_Value){0}, type_pointer_size_bytes());
    }
    else
    {
        iri_emit_arith(gen, IR_ADD, t1, t2, t3, (IR_Value) { 0 }, type_pointer_size_bytes());
    }
    
    if(load)
    {
        if (type_primitive_float(expr->type))
        {
            ir_free_reg(gen, t3);
            IR_Reg t4 = ir_new_reg(gen, expr->type);
            iri_emit_load(gen, t3, t4, (IR_Value){0}, type_pointer_size_bytes(), expr->type->size_bits / 8, true);
            return t4;
        }
        else
        {
            iri_emit_load(gen, t3, t3, (IR_Value){0}, type_pointer_size_bytes(), expr->type->size_bits / 8, type_primitive_float(expr->type));
            return t3;
        }
    }
    return t3;
}

IR_Reg
ir_gen_expr_binary_float(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    // TODO(psv): register allocation here is not viable
    IR_Reg t1 = ir_gen_expr(gen, expr->expr_binary.left, true, inside_literal, outer_offset);
    IR_Reg t2 = ir_gen_expr(gen, expr->expr_binary.right, true, inside_literal, outer_offset);
    IR_Reg t3 = ir_new_reg(gen, expr->type);
    ir_free_reg(gen, t2);
    ir_free_reg(gen, t1);

    int byte_size = expr->type->size_bits / 8;

    IR_Instruction_Type instr_type = IR_NONE;
    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:            instr_type = IR_ADDF; break;
        case OP_BINARY_MINUS:           instr_type = IR_SUBF; break;
        case OP_BINARY_MULT:            instr_type = IR_MULF; break;
        case OP_BINARY_DIV:             instr_type = IR_DIVF; break;

        case OP_BINARY_LT:
        case OP_BINARY_LE:
        case OP_BINARY_GT:
        case OP_BINARY_GE:
        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL:   return ir_gen_expr_cond(gen, expr, t1, t2, t3, true);
        default: assert(0); break;
    }

    iri_emit_arith(gen, instr_type, t1, t2, t3, (IR_Value){0}, byte_size);

    return t3;
}

IR_Reg
ir_gen_expr_pointer_arithmetic(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    assert(expr->kind == AST_EXPRESSION_BINARY);
    if(expr->expr_binary.op == OP_BINARY_PLUS)
        return ir_gen_expr_ptr_arith_sum(gen, expr, false, inside_literal, outer_offset);
    else
        return IR_REG_NONE;
}

IR_Reg
ir_gen_expr_binary(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    // first check for pointer arithmetic
    if(expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS)
        return ir_gen_expr_vector_access(gen, expr, load, inside_literal, outer_offset);

    // pointer arithmetic
    if (expr->expr_binary.left->type->kind == TYPE_KIND_POINTER) {
        IR_Reg res = ir_gen_expr_pointer_arithmetic(gen, expr, load, inside_literal, outer_offset);
        if(res != IR_REG_NONE) return res;
    }
    
    if(type_primitive_float(expr->expr_binary.left->type))
        return ir_gen_expr_binary_float(gen, expr, load, inside_literal, outer_offset);

    IR_Reg t1 = ir_gen_expr(gen, expr->expr_binary.left, true, inside_literal, outer_offset);
    ir_free_reg(gen, t1);
    iri_emit_push(gen, t1, (IR_Value){0}, expr->expr_binary.left->type->size_bits / 8, false);

    IR_Reg t2 = ir_gen_expr(gen, expr->expr_binary.right, true, inside_literal, outer_offset);

    t1 = ir_new_reg(gen, expr->expr_binary.left->type);
    iri_emit_pop(gen, t1, expr->expr_binary.left->type->size_bits / 8, false);

    IR_Reg t3 = ir_new_reg(gen, expr->type);
    ir_free_reg(gen, t2);
    ir_free_reg(gen, t1);

    bool signed_ = type_primitive_sint(expr->expr_binary.left->type);

    int byte_size = expr->type->size_bits / 8;

    IR_Instruction_Type instr_type = IR_NONE;
    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:            instr_type = IR_ADD; break;
        case OP_BINARY_MINUS:           instr_type = IR_SUB; break;
        case OP_BINARY_MULT:            instr_type = ((signed_) ? IR_IMUL : IR_MUL); break;
        case OP_BINARY_DIV:             instr_type = ((signed_) ? IR_IDIV : IR_DIV); break;
        case OP_BINARY_MOD:             instr_type = IR_MOD; break;
        case OP_BINARY_AND:             instr_type = IR_AND; break;
        case OP_BINARY_OR:              instr_type = IR_OR; break;
        case OP_BINARY_XOR:             instr_type = IR_XOR; break;
        case OP_BINARY_SHL:             instr_type = IR_SHL; break;
        case OP_BINARY_SHR:             instr_type = IR_SHR; break;
        case OP_BINARY_LOGIC_AND:       instr_type = IR_LAND; break;
        case OP_BINARY_LOGIC_OR:        instr_type = IR_LOR; break;

        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE:
        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL:   return ir_gen_expr_cond(gen, expr, t1, t2, t3, false);
        default: break;
    }
    iri_emit_arith(gen, instr_type, t1, t2, t3, (IR_Value){0}, byte_size);

    return t3;
}

IR_Reg
ir_gen_expr_comp_generated(IR_Generator* gen, Light_Ast* expr)
{
    IR_Reg t = ir_new_reg(gen, expr->type);
    switch(expr->expr_compiler_generated.kind)
    {
        case COMPILER_GENERATED_TYPE_VALUE_POINTER:
        case COMPILER_GENERATED_USER_TYPE_INFO_POINTER:
        case COMPILER_GENERATED_POINTER_TO_TYPE_INFO:
            iri_emit_arith(gen, IR_XOR, t, t, t, (IR_Value){0}, type_pointer_size_bytes());
        break;
        default: assert(0);
    }
    return t;
}

IR_Reg
ir_gen_expr_lit_prim(IR_Generator* gen, Light_Ast* expr)
{
    IR_Value value = {0};
    value.v_u64 = expr->expr_literal_primitive.value_u64; // this forced any type to be inside the union correctly
    if (expr->expr_literal_primitive.type == LITERAL_POINTER)
    {
        value.type = IR_VALUE_S32; // TODO(psv): 64 bit support
    }
    else
    {
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
    }
    IR_Reg t = ir_new_reg(gen, expr->type);
    if(expr->type->primitive == TYPE_PRIMITIVE_BOOL)
        iri_emit_arith(gen, IR_XOR, t, t, t, (IR_Value){0}, type_pointer_size_bytes());
    iri_emit_mov(gen, IR_REG_NONE, t, value, iri_value_byte_size(value), type_primitive_float(expr->type));

    return t;
}

IR_Reg
ir_gen_expr_variable(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    IR_Reg t = IR_REG_NONE;
    Light_Ast* vdecl = expr->expr_variable.decl;

    if(vdecl->kind == AST_DECL_PROCEDURE)
    {
        // get the procedure address
        t = ir_new_temp(gen);
        IR_Decl_To_Patch patch = {0};
        patch.decl = vdecl;
        patch.instr_number = iri_current_instr_index(gen);
        if(vdecl->decl_proc.flags & DECL_PROC_FLAG_EXTERN)
        {
            patch.external = true;
            // mov 0x12345678 -> t
            // load t -> t
            iri_emit_mov(gen, IR_REG_NONE, t, (IR_Value){.type = IR_VALUE_U64, .v_u64 = 0}, type_pointer_size_bytes(), false);
            iri_emit_load(gen, t, t, (IR_Value){0}, type_pointer_size_bytes(), type_pointer_size_bytes(), false);
        }
        else
        {
            iri_emit_mov(gen, IR_REG_NONE, t, (IR_Value){.type = IR_VALUE_U64, .v_u64 = 0}, type_pointer_size_bytes(), false);
        }
        array_push(gen->procdecl_patch, patch);
    }
    else if(vdecl->kind == AST_DECL_VARIABLE)
    {
        Light_Ast_Decl_Variable* decl = &vdecl->decl_variable;
        // if it is not loaded in a temporary, then load it
        t = ir_new_reg(gen, expr->type);
        if(load && expr->type->size_bits <= type_pointer_size_bits() && expr->type->kind != TYPE_KIND_ARRAY)
        {
            if(decl->storage_class == STORAGE_CLASS_STACK)
            {
                // LOAD SB+imm -> t
                iri_emit_load(gen, IR_REG_STACK_BASE, t, 
                    (IR_Value){.v_s32 = decl->stack_offset, .type = IR_VALUE_S32},
                    type_pointer_size_bytes(), expr->type->size_bits / 8, type_primitive_float(expr->type));
            }
            else if(decl->storage_class == STORAGE_CLASS_DATA_SEGMENT)
            {
                IR_Decl_To_Patch patch = {0};
                patch.decl = vdecl;
                patch.instr_number = iri_current_instr_index(gen);
                
                iri_emit_mov(gen, IR_REG_NONE, t, (IR_Value){.type = IR_VALUE_U64, .v_u64 = 0}, type_pointer_size_bytes(), false);
                iri_emit_load(gen, t, t, (IR_Value){0}, type_pointer_size_bytes(), type_pointer_size_bytes(), false);

                array_push(gen->vardecl_patch, patch);
            }
            else
                assert(0); // not supported yet
        }
        else
        {
            if (decl->storage_class == STORAGE_CLASS_STACK)
            {
                // LEA
                iri_emit_lea(gen, IR_REG_STACK_BASE, t, 
                    (IR_Value){.v_s32 = decl->stack_offset, .type = IR_VALUE_S32},
                    (int)type_pointer_size_bytes());
            }
            else if(decl->storage_class == STORAGE_CLASS_DATA_SEGMENT)
            {
                // move data segment address to t
                IR_Decl_To_Patch patch = {0};
                patch.decl = vdecl;
                patch.instr_number = iri_current_instr_index(gen);

                iri_emit_mov(gen, IR_REG_NONE, t, (IR_Value){.type = IR_VALUE_U64, .v_u64 = 0}, type_pointer_size_bytes(), false);

                array_push(gen->vardecl_patch, patch);
            }
            else
                assert(0); // not supported yet
        }
    }
    else if (vdecl->kind == AST_DECL_CONSTANT)
    {
        t = ir_gen_expr(gen, vdecl->decl_constant.value, load, false, 0);
    }

    return t;
}

IR_Reg
ir_gen_expr_proc_call(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    IR_Reg caller = ir_gen_expr(gen, expr->expr_proc_call.caller_expr, true, inside_literal, outer_offset);

    int arg_stack_size = 0;
    if (expr->expr_proc_call.arg_count > 0)
    {
        // push caller into the stack
        iri_emit_push(gen, caller, (IR_Value){0}, type_pointer_size_bytes(), false);
        ir_free_reg(gen, caller);

        // push the arguments
        for(int i = expr->expr_proc_call.arg_count - 1; i >= 0; --i)
        {
            Light_Ast* arg = expr->expr_proc_call.args[i];
            IR_Reg arg_r = ir_gen_expr(gen, arg, true, inside_literal, outer_offset);
            int push_bytes = MAX(MIN(arg->type->size_bits / 8, type_pointer_size_bytes()), type_pointer_size_bytes());
            iri_emit_push(gen, arg_r, (IR_Value){0}, push_bytes, type_primitive_float(arg->type));
            arg_stack_size += push_bytes;
            ir_free_reg(gen, arg_r);
        }

        // pop caller back to use it
        iri_emit_load(gen, IR_REG_STACK_PTR, caller, (IR_Value){.type = IR_VALUE_S32, .v_s32 = arg_stack_size},
            type_pointer_size_bytes(), type_pointer_size_bytes(), false);
    }

    iri_emit_call(gen, caller, (IR_Value){0}, type_pointer_size_bytes());
    ir_free_reg(gen, caller);

    if(arg_stack_size > 0)
    {
        // pop an extra 4 bytes that is the caller address that was in the stack before
        iri_emit_arith(gen, IR_ADD, IR_REG_STACK_PTR, IR_REG_NONE, IR_REG_STACK_PTR, 
                (IR_Value){.type = IR_VALUE_S32, .v_s32 = 4}, type_pointer_size_bytes());
    }

    return IR_REG_PROC_RET;
}

IR_Reg
ir_gen_expr_dot(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    Light_Type* ltype = type_alias_root(expr->expr_dot.left->type);

    if(ltype->kind == TYPE_KIND_ENUM)
    {
        for (int i = 0; i < array_length(ltype->enumerator.fields); ++i)
        {
            if(ltype->enumerator.fields[i]->decl_variable.name->data == expr->expr_dot.identifier->data)
            {
                IR_Reg t1 = ir_new_temp(gen);
                iri_emit_mov(gen, IR_REG_NONE, t1, 
                    (IR_Value){.type = IR_VALUE_S32, .v_s32 = (s32)ltype->enumerator.evaluated_values[i]}, 32, false);
                return t1;
            }
        }
    }

    IR_Reg t = ir_gen_expr(gen, expr->expr_dot.left, false, inside_literal, outer_offset);

    IR_Reg tres = IR_REG_NONE;
    // TODO: unions
    if(ltype->kind == TYPE_KIND_STRUCT || ltype->kind == TYPE_KIND_POINTER)
    {
        int32_t offset = 0;
        tres = ir_new_temp(gen);

        if (ltype->kind == TYPE_KIND_POINTER)
        {
            ltype = type_alias_root(ltype->pointer_to);
            iri_emit_load(gen, t, t, (IR_Value) { 0 }, type_pointer_size_bytes(), type_pointer_size_bytes(), false);
        }

        for(int i = 0; i < ltype->struct_info.fields_count; ++i)
        {
            if(ltype->struct_info.fields[i]->decl_variable.name->data == expr->expr_dot.identifier->data)
            {
                offset = (int32_t)(ltype->struct_info.offset_bits[i] / 8);
                break;
            }
        }
        iri_emit_arith(gen, IR_ADD, t, IR_REG_NONE, tres, (IR_Value){.type = IR_VALUE_S32, .v_s32 = offset}, type_pointer_size_bytes());
        ir_free_reg(gen, t);
    }
    else if(ltype->kind == TYPE_KIND_UNION)
    {
        // nothing need to be done, since offset in the union is always 0
        tres = t;
    }

    if(load)
    {
        IR_Reg r = ir_new_reg(gen, expr->type);
        //assert(type_alias_root(expr->type)->kind == TYPE_KIND_PRIMITIVE);
        iri_emit_load(gen, tres, r, (IR_Value){0}, 
            type_pointer_size_bytes(), expr->type->size_bits / 8, type_primitive_float(expr->type));
        ir_free_reg(gen, tres);
        return r;
    }
    return tres;
}

IR_Reg
ir_gen_expr_lit_struct(IR_Generator* gen, Light_Ast* expr, bool inside_literal, int outer_offset)
{
    int size_bytes = expr->type->size_bits / 8;
    int stack_offset = outer_offset;

    // alocate bytes in the stack for it only if its not inner
    if (!inside_literal)
    {
        // alocate space for it in the activation record
        IR_Activation_Rec* ar = ir_get_current_ar(gen);
        expr->expr_literal_struct.stack_offset = -(ar->literals_stack_size + ar->stack_size_bytes) - size_bytes;
        ar->literals_stack_size += size_bytes;
        stack_offset = expr->expr_literal_struct.stack_offset;
    }
    
    Light_Type* struct_type = type_alias_root(expr->type);
    for(int i = 0, offset = stack_offset;
        i < array_length(expr->expr_literal_struct.struct_exprs);
        ++i, offset = stack_offset + struct_type->struct_info.offset_bits[i] / 8)
    {
        Light_Ast* e = expr->expr_literal_struct.struct_exprs[i];
        IR_Reg expt = ir_gen_expr(gen, e, true, true, offset);
#if 1
        if (e->kind == AST_EXPRESSION_LITERAL_STRUCT || e->kind == AST_EXPRESSION_LITERAL_ARRAY)
        {
            // did everything already
        }
        else if(e->flags & AST_FLAG_EXPRESSION_LVALUE)
        {
            //iri_emit_copy(gen, expt, IR_REG_STACK_BASE, (IR_Value){.type = IR_VALUE_S32, .v_s32 = offset}, e->type->size_bits / 8);
        }
        else
        {
            iri_emit_store(gen, expt, IR_REG_STACK_BASE, (IR_Value){.type = IR_VALUE_S32, .v_s32 = offset}, e->type->size_bits / 8,
                type_primitive_float(e->type));
        }
#endif
        ir_free_reg(gen, expt);
    }

    IR_Reg t = ir_new_temp(gen);
    iri_emit_lea(gen, IR_REG_STACK_BASE, t, (IR_Value){.type = IR_VALUE_S32, .v_s32 = stack_offset}, type_pointer_size_bytes());
    return t;
}

IR_Reg
ir_gen_expr_lit_array(IR_Generator* gen, Light_Ast* expr, bool inside_literal, int outer_offset)
{
    // TODO(psv): when raw data is string, remove 2 bytes extra here
    int size_bytes = (expr->type->size_bits / 8);
    int stack_offset = outer_offset;

    if (!inside_literal || expr->expr_literal_array.raw_data)
    {
        // alocate space for it in the activation record
        IR_Activation_Rec* ar = ir_get_current_ar(gen);
        expr->expr_literal_array.stack_offset = -(ar->literals_stack_size + ar->stack_size_bytes) - size_bytes;
        ar->literals_stack_size += size_bytes;
        stack_offset = expr->expr_literal_array.stack_offset;
    }

    if(expr->expr_literal_array.raw_data)
    {
        // raw data
        // TODO(psv): optimize to store 4 or more bytes at a time
        IR_Reg t1 = ir_new_temp(gen);
        for(int i = 0; i < expr->expr_literal_array.data_length_bytes; ++i)
        {
            iri_emit_mov(gen, IR_REG_NONE, t1, (IR_Value){.type = IR_VALUE_U8, .v_u8 = expr->expr_literal_array.data[i]},
                1, false);
            iri_emit_store(gen, t1, IR_REG_STACK_BASE, (IR_Value){.type = IR_VALUE_S32, .v_s32 = i + stack_offset}, 1, false);
        }
        ir_free_reg(gen, t1);
    }
    else
    {
        // expressions array
        for(int i = 0, offset = stack_offset; i < array_length(expr->expr_literal_array.array_exprs); ++i)
        {
            Light_Ast* e = expr->expr_literal_array.array_exprs[i];
            IR_Reg expt = ir_gen_expr(gen, e, true, true, offset);
            IR_Reg copy_to = ir_new_temp(gen);
            if(e->flags & AST_FLAG_EXPRESSION_LVALUE)
            {
                iri_emit_mov(gen, IR_REG_STACK_BASE, copy_to, (IR_Value) { 0 }, type_pointer_size_bytes(), false);
                iri_emit_arith(gen, IR_ADD, copy_to, IR_REG_NONE, copy_to, (IR_Value) { .type = IR_VALUE_S32, .v_s32 = offset }, type_pointer_size_bytes());
                iri_emit_copy(gen, expt, copy_to, (IR_Value){.type = IR_VALUE_S32, .v_s32 = e->type->size_bits / 8}, type_pointer_size_bytes());
            }
            else
            {
                iri_emit_store(gen, expt, IR_REG_STACK_BASE, (IR_Value){.type = IR_VALUE_S32, .v_s32 = offset}, e->type->size_bits / 8,
                    type_primitive_float(e->type));
            }
            ir_free_reg(gen, expt);
            ir_free_reg(gen, copy_to);
            offset += (e->type->size_bits / 8);
        }
    }

    IR_Reg t = ir_new_temp(gen);
    iri_emit_lea(gen, IR_REG_STACK_BASE, t, (IR_Value){.type = IR_VALUE_S32, .v_s32 = stack_offset}, type_pointer_size_bytes());
    return t;
}

IR_Reg
ir_gen_expr(IR_Generator* gen, Light_Ast* expr, bool load, bool inside_literal, int outer_offset)
{
    IR_Reg res = IR_REG_NONE;

    switch(expr->kind)
    {
        case AST_EXPRESSION_COMPILER_GENERATED: res = ir_gen_expr_comp_generated(gen, expr); break;
        case AST_EXPRESSION_LITERAL_PRIMITIVE:  res = ir_gen_expr_lit_prim(gen, expr); break;
        case AST_EXPRESSION_VARIABLE:           res = ir_gen_expr_variable(gen, expr, load, inside_literal, outer_offset); break;
        case AST_EXPRESSION_BINARY:             res = ir_gen_expr_binary(gen, expr, load, inside_literal, outer_offset); break;
        case AST_EXPRESSION_UNARY:              res = ir_gen_expr_unary(gen, expr, load, inside_literal, outer_offset); break;
        case AST_EXPRESSION_PROCEDURE_CALL:     res = ir_gen_expr_proc_call(gen, expr, load, inside_literal, outer_offset); break;
        case AST_EXPRESSION_DOT:                res = ir_gen_expr_dot(gen, expr, load, inside_literal, outer_offset); break;
        case AST_EXPRESSION_LITERAL_ARRAY:      res = ir_gen_expr_lit_array(gen, expr, inside_literal, outer_offset); break;
        case AST_EXPRESSION_LITERAL_STRUCT:     res = ir_gen_expr_lit_struct(gen, expr, inside_literal, outer_offset); break;
        default: break;
    }
    gen->instructions[array_length(gen->instructions) - 1].origin_node = expr;

    return res;
}

void
ir_gen_comm_if(IR_Generator* gen, Light_Ast* stmt)
{
    // if(condition)
    IR_Reg cond_temp = ir_gen_expr(gen, stmt->comm_if.condition, true, false, 0);

    // jrz t, 0 -> else
    int if_true_index = iri_current_instr_index(gen);
    iri_emit_jrz(gen, cond_temp, (IR_Value){.type = IR_VALUE_TO_BE_FILLED}, 4);    // TODO(psv): identify proper size
    ir_free_reg(gen, cond_temp);

    // generate true branch
    ir_gen_comm(gen, stmt->comm_if.body_true);

    int else_jump_index = iri_current_instr_index(gen);
    if(stmt->comm_if.body_false)
    {
        // jump over false clause first
        iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_TO_BE_FILLED}, 4);
    }

    int else_index = iri_current_instr_index(gen);
    // fill the first jump instruction with the relative address (in instruction number)
    IR_Instruction* if_true_instr = iri_get_temp_instr_ptr(gen, if_true_index);
    if_true_instr->imm.type = IR_VALUE_S32;
    if_true_instr->imm.v_s32 = else_index - if_true_index;

    // generate false branch
    if(stmt->comm_if.body_false)
    {
        ir_gen_comm(gen, stmt->comm_if.body_false);
        int end_index = iri_current_instr_index(gen);
        // fill the second jump instruction with the relative address (in instruction number)
        IR_Instruction* jmp_end_instr = iri_get_temp_instr_ptr(gen, else_jump_index);
        jmp_end_instr->imm.type = IR_VALUE_S32;
        jmp_end_instr->imm.v_s32 = end_index - else_jump_index;
    }
}

static void
ir_patch_jumps(IR_Generator* gen, int current_index)
{
    for(int i = 0; i < array_length(gen->jmp_patch); ++i)
    {
        if(gen->jmp_patch[i].level == 0)
        {
            IR_Instruction* instr = iri_get_temp_instr_ptr(gen, gen->jmp_patch[i].index);
            instr->imm.v_s32 = current_index - gen->jmp_patch[i].index;
            array_remove(gen->jmp_patch, i);
            i--;
        }
        else
            gen->jmp_patch[i].level--;
    }
}

static void
ir_clear_start_labels_level(IR_Generator* gen)
{
    for(int i = 0; i < array_length(gen->loop_start_labels); ++i)
    {
        if(gen->loop_start_labels[i] > 0)
            gen->loop_start_labels[i]--;
        else
        {
            array_remove(gen->loop_start_labels, i);
            i--;
        }
    }
}

void
ir_gen_comm_while(IR_Generator* gen, Light_Ast* stmt)
{
    int while_start_index = iri_current_instr_index(gen);
    array_push(gen->loop_start_labels, while_start_index);
    // while(condition)
    IR_Reg cond_temp = ir_gen_expr(gen, stmt->comm_while.condition, true, false, 0);

    int while_true_index = iri_current_instr_index(gen);
    // jrz t, 0 -> end
    iri_emit_jrz(gen, cond_temp, (IR_Value){.type = IR_VALUE_TO_BE_FILLED}, type_pointer_size_bytes());
    ir_free_reg(gen, cond_temp);

    if(stmt->comm_while.body)
    {
        ir_gen_comm(gen, stmt->comm_while.body);
    }
    // generate while jump to beginning
    int jmp_start_index = iri_current_instr_index(gen);

    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = while_start_index - jmp_start_index}, type_pointer_size_bytes());

    int end_index = iri_current_instr_index(gen);
    // fill condition jump with the end address
    IR_Instruction* while_cond_instr = iri_get_temp_instr_ptr(gen, while_true_index);
    while_cond_instr->imm.type = IR_VALUE_S32;
    while_cond_instr->imm.v_s32 = end_index - while_true_index;

    ir_clear_start_labels_level(gen);
    ir_patch_jumps(gen, end_index);
}

void
ir_gen_comm_for(IR_Generator* gen, Light_Ast* stmt)
{
    // generate prologue
    if(stmt->comm_for.prologue)
        ir_gen_commands(gen, stmt->comm_for.prologue, array_length(stmt->comm_for.prologue));
    
    // jump over epilogue
    int jmp_over_ep_index = iri_current_instr_index(gen);
    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = 0}, type_pointer_size_bytes());

    int for_start_index = iri_current_instr_index(gen);
    array_push(gen->loop_start_labels, for_start_index);

    // generate epilogue
    if(stmt->comm_for.epilogue)
        ir_gen_commands(gen, stmt->comm_for.epilogue, array_length(stmt->comm_for.epilogue));

    int cond_index = iri_current_instr_index(gen);

    IR_Instruction* jmp_over_ep_index_instr = iri_get_temp_instr_ptr(gen, jmp_over_ep_index);
    jmp_over_ep_index_instr->imm.v_s32 = cond_index - jmp_over_ep_index;

    // for(condition)
    IR_Reg cond_temp = ir_gen_expr(gen, stmt->comm_for.condition, true, false, 0);

    int for_true_index = iri_current_instr_index(gen);
    // jrz t, 0 -> end
    iri_emit_jrz(gen, cond_temp, (IR_Value){.type = IR_VALUE_TO_BE_FILLED}, type_pointer_size_bytes());
    ir_free_reg(gen, cond_temp);

    if(stmt->comm_for.body)
    {
        ir_gen_comm(gen, stmt->comm_for.body);
    }

    // generate for jump to beginning
    int jmp_start_index = iri_current_instr_index(gen);
    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = for_start_index - jmp_start_index}, type_pointer_size_bytes());

    int end_index = iri_current_instr_index(gen);
    // fill condition jump with the end address
    IR_Instruction* while_cond_instr = iri_get_temp_instr_ptr(gen, for_true_index);
    while_cond_instr->imm.type = IR_VALUE_S32;
    while_cond_instr->imm.v_s32 = end_index - for_true_index;

    ir_clear_start_labels_level(gen);
    ir_patch_jumps(gen, end_index);
}

void
ir_gen_decl_assignment(IR_Generator* gen, Light_Ast* decl)
{
    assert(decl->kind == AST_DECL_VARIABLE);
    Light_Ast* expr = decl->decl_variable.assignment;
    int byte_size = expr->type->size_bits / 8;
    Light_Type* rvalue_type = type_alias_root(expr->type);
    bool primitive_type = rvalue_type->kind == TYPE_KIND_PRIMITIVE || rvalue_type->kind == TYPE_KIND_ENUM;

    IR_Reg t2 = ir_gen_expr(gen, expr, primitive_type || (expr->flags & AST_FLAG_EXPRESSION_LVALUE), false, 0);

    IR_Value stack_offset = (IR_Value){.type = IR_VALUE_S32, .v_s32 = decl->decl_variable.stack_offset};
    if(primitive_type || rvalue_type->kind == TYPE_KIND_FUNCTION || rvalue_type->kind == TYPE_KIND_POINTER)
    {
        iri_emit_store(gen, t2, IR_REG_STACK_BASE, stack_offset, byte_size, type_primitive_float(rvalue_type));
    }
    else
    {
        IR_Reg t1 = ir_new_reg(gen, rvalue_type);
        iri_emit_lea(gen, IR_REG_STACK_BASE, t1, stack_offset, type_pointer_size_bytes());
        iri_emit_copy(gen, t2, t1, (IR_Value){.type = IR_VALUE_U32, .v_u32 = byte_size}, 
            type_pointer_size_bytes());
        ir_free_reg(gen, t1);
    }
    ir_free_reg(gen, t2);
}

void
ir_gen_comm_assignment_big(IR_Generator* gen, Light_Ast_Comm_Assignment* comm)
{
    int byte_size = comm->rvalue->type->size_bits / 8;

    IR_Reg rval = ir_gen_expr(gen, comm->rvalue, false, false, 0);
    iri_emit_push(gen, rval, (IR_Value) { 0 }, type_pointer_size_bytes(), false);
    ir_free_reg(gen, rval);

    IR_Reg lval = ir_gen_expr(gen, comm->lvalue, false, false, 0);
    if (lval == rval)
        rval = ir_new_temp(gen);

    iri_emit_pop(gen, rval, type_pointer_size_bytes(), false);

    // addresses are in rval and lval, copy from rval to lval byte_size bytes
    iri_emit_copy(gen, rval, lval, iri_value_s32(byte_size), type_pointer_size_bytes());

    ir_free_reg(gen, rval);
    ir_free_reg(gen, lval);
}

void
ir_gen_comm_assignment(IR_Generator* gen, Light_Ast_Comm_Assignment* comm)
{
    int byte_size = comm->rvalue->type->size_bits / 8;
    if (byte_size > type_pointer_size_bytes())
    {
        ir_gen_comm_assignment_big(gen, comm);
        return;
    }

    Light_Type* rvalue_type = type_alias_root(comm->rvalue->type);
    bool primitive_type = rvalue_type->kind == TYPE_KIND_PRIMITIVE;
    bool pointer_type = rvalue_type->kind == TYPE_KIND_POINTER;

    IR_Reg t2 = ir_gen_expr(gen, comm->rvalue, primitive_type||pointer_type, false, 0);

    // This is in case a function call without return assignment
    if (!comm->lvalue)
        return;

    // push result to the stack
    iri_emit_push(gen, t2, (IR_Value){0}, rvalue_type->size_bits / 8, type_primitive_float(comm->rvalue->type));
    ir_free_reg(gen, t2);

    IR_Reg t1 = ir_gen_expr(gen, comm->lvalue, false, false, 0);
    if (t2 == t1)
    {
        t2 = ir_new_temp(gen);
    }

    iri_emit_pop(gen, t2, rvalue_type->size_bits / 8, type_primitive_float(comm->rvalue->type));

    ir_free_reg(gen, t1);
    ir_free_reg(gen, t2);
    
    if(primitive_type || rvalue_type->kind == TYPE_KIND_FUNCTION || rvalue_type->kind == TYPE_KIND_POINTER)
    {
        iri_emit_store(gen, t2, t1, (IR_Value){0}, byte_size, type_primitive_float(comm->rvalue->type));
    }
    else
    {
        iri_emit_copy(gen, t2, t1, (IR_Value){.type = IR_VALUE_U32, .v_u32 = byte_size}, 
            type_pointer_size_bytes());
    }
}

void
ir_gen_commands(IR_Generator* gen, Light_Ast** commands, int comm_count)
{
    if (!commands) return;

    // clear r0 to use in the variable initialization
    iri_emit_arith(gen, IR_XOR, IR_REG_PROC_RET, IR_REG_PROC_RET, IR_REG_PROC_RET, (IR_Value){0}, type_pointer_size_bytes());

    // decls
    for(int i = 0; i < comm_count; ++i) {
        Light_Ast* comm = commands[i];
        if(comm->flags & AST_FLAG_DECLARATION)
            ir_gen_decl(gen, comm);
    }

    // commands
    for(int i = 0; i < comm_count; ++i) {
        Light_Ast* comm = commands[i];
        if(comm->flags & AST_FLAG_COMMAND)
            ir_gen_comm(gen, comm);
        if(comm->flags & AST_FLAG_DECLARATION)
        {
            if(comm->kind == AST_DECL_VARIABLE && comm->decl_variable.assignment)
            {
                ir_gen_decl_assignment(gen, comm);
            }
        }
    }
}

static int calc_block_stack_size(Light_Ast* block);
static int calc_commands_stack_size(Light_Ast** commands);

static int
calc_command_stack_size(Light_Ast* comm)
{
    int stack_size_bytes = 0;
    switch(comm->kind)
    {
        case AST_DECL_VARIABLE: {
            int var_size_bytes = comm->decl_variable.type->size_bits / 8;
            stack_size_bytes += var_size_bytes;
        } break;
        case AST_COMMAND_BLOCK: {
            stack_size_bytes += calc_block_stack_size(comm);
        } break;
        case AST_COMMAND_IF: {
            if(comm->comm_if.body_true && comm->comm_if.body_true->kind == AST_COMMAND_BLOCK)
                stack_size_bytes += calc_block_stack_size(comm->comm_if.body_true);
            if(comm->comm_if.body_false && comm->comm_if.body_false->kind == AST_COMMAND_BLOCK)
                stack_size_bytes += calc_block_stack_size(comm->comm_if.body_false);
        } break;
        case AST_COMMAND_WHILE: {
            if(comm->comm_while.body && comm->comm_while.body->kind == AST_COMMAND_BLOCK)
                stack_size_bytes += calc_block_stack_size(comm->comm_while.body);
        } break;
        case AST_COMMAND_FOR: {
            if(comm->comm_for.prologue)
            {
                stack_size_bytes += calc_commands_stack_size(comm->comm_for.prologue);
            }
            if(comm->comm_for.body && comm->comm_for.body->kind == AST_COMMAND_BLOCK)
            {
                stack_size_bytes += calc_block_stack_size(comm->comm_for.body);
            }
        } break;
        default: break;
    }
    return stack_size_bytes;
}

static int
calc_commands_stack_size(Light_Ast** commands)
{
    if (!commands) return 0;
    int stack_size_bytes = 0;
    for(int i = 0; i < array_length(commands); ++i)
    {
        Light_Ast* comm = commands[i];
        stack_size_bytes += calc_command_stack_size(comm);
    }
    return stack_size_bytes;
}

static int
calc_block_stack_size(Light_Ast* block)
{
    if (!block) return 0;
    return calc_commands_stack_size(block->comm_block.commands);
}

void
ir_gen_comm_block(IR_Generator* gen, Light_Ast* body)
{
    ir_gen_commands(gen, body->comm_block.commands, body->comm_block.command_count);
}

void
ir_gen_comm_return(IR_Generator* gen, Light_Ast* comm)
{
    if(comm->comm_return.expression)
    {
        IR_Reg t = ir_gen_expr(gen, comm->comm_return.expression, true, false, 0);
        ir_free_reg(gen, t);
        bool fp = type_primitive_float(comm->comm_return.expression->type);
        iri_emit_mov(gen, t, (fp) ? IR_REG_PROC_RETF : IR_REG_PROC_RET, (IR_Value){0}, 
            comm->comm_return.expression->type->size_bits / 8, fp);
    }

#if IR_TO_X86
    ir_gen_x86_epilogue(gen);
#endif

    if(scope_inside_main(comm->scope_at))
    {
        iri_emit_hlt(gen);
    }
    else
    {
        // TODO(psv): check calling convention
        // clear the stack (stdcall only)
        IR_Activation_Rec* ar = ir_get_current_ar(gen);
        iri_emit_ret(gen, ar->stack_args_size_bytes);
    }
}

void
ir_gen_comm_continue(IR_Generator* gen, Light_Ast* comm)
{
    int lvl = (int)comm->comm_continue.level_value - 1;
    int index = iri_current_instr_index(gen);

    int diff = gen->loop_start_labels[array_length(gen->loop_start_labels) - 1 - lvl] - index;
    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = diff}, type_pointer_size_bytes());
}

void
ir_gen_comm_break(IR_Generator* gen, Light_Ast* comm)
{
    int lvl = (int)comm->comm_break.level_value;
    int index = iri_current_instr_index(gen);
    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = 0}, type_pointer_size_bytes());
    IR_Instr_Jmp_Patch ijp = {0};
    ijp.index = index;
    ijp.level = lvl - 1;
    array_push(gen->jmp_patch, ijp);
}

void
ir_gen_comm(IR_Generator* gen, Light_Ast* comm)
{
    int start = iri_current_instr_index(gen);
    switch(comm->kind) {
        case AST_COMMAND_ASSIGNMENT: 
            ir_gen_comm_assignment(gen, &comm->comm_assignment);
            break;
        case AST_COMMAND_IF:
            ir_gen_comm_if(gen, comm);
            break;
        case AST_COMMAND_BLOCK:
            ir_gen_comm_block(gen, comm);
            break;
        case AST_COMMAND_WHILE:
            ir_gen_comm_while(gen, comm);
            break;
        case AST_COMMAND_FOR:
            ir_gen_comm_for(gen, comm);
            break;
        case AST_COMMAND_RETURN:
            ir_gen_comm_return(gen, comm);
            break;
        case AST_COMMAND_BREAK:
            ir_gen_comm_break(gen, comm);
            break;
        case AST_COMMAND_CONTINUE:
            ir_gen_comm_continue(gen, comm);
            break;
        default: break;
    }
    int end = iri_current_instr_index(gen);
    IR_Node_Range range = {comm, start, end};
    array_push(gen->node_ranges, range);
}

int
ir_calculate_proc_stack_size(Light_Ast* proc)
{
    Light_Scope* pscope = proc->decl_proc.body->comm_block.block_scope;
    if (!pscope) return 0; // procedure has no declarations

    int stack_size_bytes = calc_block_stack_size(proc->decl_proc.body);

    // allocate stack space for the arguments in the stack
    for(int i = 0; i < proc->decl_proc.argument_count; ++i)
    {
        Light_Type* arg_type = proc->decl_proc.arguments[i]->decl_variable.type;
        if(arg_type->size_bits > type_pointer_size_bits())
        {
            stack_size_bytes += (arg_type->size_bits / 8);
        }
    }
    return stack_size_bytes;
}

void
ir_gen_proc(IR_Generator* gen, Light_Ast* proc)
{
    Light_Ast* body = proc->decl_proc.body;

    ir_new_activation_record(gen);
    IR_Activation_Rec* ar = ir_get_current_ar(gen);

    // setup arguments in the stack
    int instr_index = iri_current_instr_index(gen);
    if(proc->decl_proc.flags & DECL_PROC_FLAG_MAIN)
    {
        gen->index_of_entry_point = instr_index;
    }

    proc->decl_proc.ir_instr_index = instr_index;
    int stack_size_bytes = ir_calculate_proc_stack_size(proc);

    // prologue
    // push ebp
    // mov esp -> ebp
    // sub esp, stack_size -> esp
    iri_emit_push(gen, IR_REG_STACK_BASE, (IR_Value){0}, type_pointer_size_bytes(), false);
    iri_emit_mov(gen, IR_REG_STACK_PTR, IR_REG_STACK_BASE, (IR_Value){0}, type_pointer_size_bytes(), false);
    int sub_esp_index = iri_current_instr_index(gen);
    iri_emit_arith(gen, IR_SUB, IR_REG_STACK_PTR, IR_REG_NONE, IR_REG_STACK_PTR, 
        (IR_Value){.type = IR_VALUE_S32, .v_s32 = stack_size_bytes}, type_pointer_size_bytes());

    // TODO(psv): consider alignment
    int stack_offset = 8;   // after return value and ebp
    int stack_args_size_bytes = 0;
    for(int i = 0; i < proc->decl_proc.argument_count; ++i)
    {
        Light_Ast* arg = proc->decl_proc.arguments[i];
        int arg_byte_size = arg->decl_variable.type->size_bits / 8;
        arg->decl_variable.ir_temporary = IR_REG_NONE;
        arg->decl_variable.stack_index = i + 2;
        arg->decl_variable.stack_offset = stack_offset;
        arg->decl_variable.stack_argument_offset = stack_offset;
        stack_offset += arg_byte_size;
        stack_args_size_bytes += MIN(arg_byte_size, type_pointer_size_bytes());

        if(arg_byte_size > type_pointer_size_bytes())
        {
            // Change the stack offset to be local, meaning we copy this
            // to the local stack
            arg->decl_variable.stack_offset = ar->offset - arg_byte_size;

            IR_Reg t1 = ir_new_temp(gen);
            IR_Reg t2 = ir_new_temp(gen);

            // Copy the value to local stack
            // load ebp + arg_offset -> t1
            iri_emit_load(gen, IR_REG_STACK_BASE, t1, iri_value_s32(arg->decl_variable.stack_argument_offset), 
                type_pointer_size_bytes(), type_pointer_size_bytes(), false);
            // lea ebp + local_offset -> t2
            iri_emit_lea(gen, IR_REG_STACK_BASE, t2, iri_value_s32(arg->decl_variable.stack_offset), type_pointer_size_bytes());
            // copy t1, size -> t2
            iri_emit_copy(gen, t1, t2, iri_value_s32(arg_byte_size), type_pointer_size_bytes());

            ir_free_reg(gen, t1);
            ir_free_reg(gen, t2);

            ar->offset -= arg_byte_size;
        }
    }
    ar->stack_args_size_bytes = stack_args_size_bytes;

    gen->ars[array_length(gen->ars) - 1].stack_size_bytes = stack_size_bytes;

    ir_gen_comm_block(gen, proc->decl_proc.body);

    // allocate more bytes to the eventual literals in this block
    ar = ir_get_current_ar(gen);
    if (ar->literals_stack_size > 0)
    {
        gen->instructions[sub_esp_index].imm.v_s32 += ar->literals_stack_size;
    }
}

void
ir_gen_proc_external(IR_Generator* gen, Light_Ast* proc)
{
    IR_Import_Entry entry = {0};
    entry.decl = proc;
    proc->decl_proc.ir_instr_index = array_length(gen->import_table);
    array_push(gen->import_table, entry);
}

void
ir_gen_dataseg_variable(IR_Generator* gen, Light_Ast* decl)
{
    IR_Data_Segment_Entry entry = {0};
    entry.decl = decl;
    entry.data_length_bytes = decl->decl_variable.type->size_bits / 8;
    decl->decl_variable.ir_instr_index = array_length(gen->dataseg);
    array_push(gen->dataseg, entry);
}

void
ir_patch_proc_calls(IR_Generator* gen)
{
    for(int i = 0; i < array_length(gen->procdecl_patch); ++i)
    {
        IR_Decl_To_Patch dpatch = gen->procdecl_patch[i];
        IR_Instruction* instr = iri_get_temp_instr_ptr(gen, dpatch.instr_number);
        if(dpatch.external)
        {
            instr->flags |= (IIR_FLAG_HAS_IMM | IIR_FLAG_IMPORT_OFFSET);
        }
        else
        {
            instr->flags |= (IIR_FLAG_HAS_IMM | IIR_FLAG_INSTRUCTION_OFFSET);
        }
        instr->imm.v_u64 = dpatch.decl->decl_proc.ir_instr_index;
    }
}

void
ir_patch_dataseg_variables(IR_Generator* gen)
{
    for(int i = 0; i < array_length(gen->vardecl_patch); ++i)
    {
        IR_Decl_To_Patch dpatch = gen->vardecl_patch[i];
        IR_Instruction* instr = iri_get_temp_instr_ptr(gen, dpatch.instr_number);
        instr->imm.v_u64 = dpatch.decl->decl_variable.ir_instr_index;
        instr->flags |= IIR_FLAG_DATASEG_OFFSET;
    }
}

void
ir_new_activation_record(IR_Generator* gen)
{
    IR_Activation_Rec ar = {0};
    ar.index = -1;
    ar.offset = 0; // -(int)(type_pointer_size_bytes());
    ar.temp_int = 1;            // temp 0 is reserved for proc call return value
    ar.temp_float = 0x0f000001; // temp 0x0f000000 is reserved for proc call return value 

    array_push(gen->ars, ar);
}

void ir_generate(IR_Generator* gen, Light_Ast** ast) {
    gen->instructions = array_new(IR_Instruction);
    gen->procdecl_patch = array_new(IR_Decl_To_Patch);
    gen->vardecl_patch = array_new(IR_Decl_To_Patch);
    gen->jmp_patch = array_new(IR_Instr_Jmp_Patch);
    gen->loop_start_labels = array_new(int);
    gen->node_ranges = array_new(IR_Node_Range);
    gen->ars = array_new(IR_Activation_Rec);
    gen->import_table = array_new(IR_Import_Entry);
    gen->dataseg = array_new(IR_Data_Segment_Entry);

    for(u64 i = 0; i < array_length(ast); ++i) {
        Light_Ast* n = ast[i];
        switch(n->kind) {
            case AST_DECL_PROCEDURE: {
                if(n->decl_proc.flags & DECL_PROC_FLAG_EXTERN)
                    ir_gen_proc_external(gen, n);
                else
                    ir_gen_proc(gen, n);
            } break;
            case AST_DECL_VARIABLE: {
                ir_gen_dataseg_variable(gen, n);
            } break;
            default: break;
        }
    }

    ir_patch_proc_calls(gen);
    ir_patch_dataseg_variables(gen);

    //FILE* ir_out = fopen("irout.txt", "w");
    //iri_print_instructions(stdout, gen);
    //fclose(ir_out);
}