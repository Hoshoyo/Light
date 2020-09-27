#include "utils/allocator.h"
#include "light_array.h"
#include <assert.h>
#include <stdio.h>
#include "type.h"
#include "ir.h"

#define ARRAY_LENGTH(A) (sizeof(A) / sizeof(*A))

int  ir_gen_expr(IR_Generator* gen, Light_Ast* expr, bool load);
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
    IR_Virtual_Reg vreg = { 0 };
    vreg.has_mem_address = false;
    vreg.preg_mapped = -1;
    vreg.uses = array_new(int);

    IR_Activation_Rec* ar = ir_get_current_ar(gen);
    array_push(ar->vregs, vreg);
    return ar->temp_int++;
}
static IR_Reg
ir_new_tempf(IR_Generator* gen) {
    IR_Virtual_FReg vfreg = { 0 };
    vfreg.uses = array_new(int);
    vfreg.has_mem_address = false;
    vfreg.pfreg_mapped = -1;

    IR_Activation_Rec* ar = ir_get_current_ar(gen);
    array_push(ar->vfregs, vfreg);
    return ar->temp_float++;
}

// where addr is offset from stack
static void
ir_set_vreg_addr(IR_Generator* gen, IR_Reg reg, int addr, bool fp)
{
    IR_Activation_Rec* ar = ir_get_current_ar(gen);
    if(fp)
    {
        ar->vfregs[reg].has_mem_address = true;
        ar->vfregs[reg].address = addr;
    }
    else
    {
        ar->vregs[reg].has_mem_address = true;
        ar->vregs[reg].address = addr;
    }
}

static int
ir_new_reg(IR_Generator* gen, Light_Type* type) {
    return (type_primitive_float(type)) ? ir_new_tempf(gen) : ir_new_temp(gen);
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
        iri_emit_load(gen, src, dst, (IR_Value){0}, type->size_bits / 8, type_primitive_float(type));
    else
    {
        // TODO(psv): bigger types
    }
}

void
ir_gen_decl(IR_Generator* gen, Light_Ast* decl)
{
    IR_Activation_Rec* ar = ir_get_current_ar(gen);

    if(decl->kind == AST_DECL_VARIABLE) {
        if(decl->decl_variable.type->size_bits <= type_pointer_size_bits())
        {
            IR_Reg temp = ir_new_reg(gen, decl->type);
            decl->decl_variable.ir_temporary = temp;
        }
        else
            decl->decl_variable.ir_temporary = IR_REG_NONE;
        decl->decl_variable.stack_index = ar->index--;
        decl->decl_variable.stack_offset = ar->offset;

        if(decl->decl_variable.ir_temporary != IR_REG_NONE)
        {
            if(type_primitive_float(decl->decl_variable.type))
            {
                ar->vfregs[decl->decl_variable.ir_temporary].has_mem_address = true;
                ar->vfregs[decl->decl_variable.ir_temporary].address = decl->decl_variable.stack_offset;
            }
            else
            {
                ar->vregs[decl->decl_variable.ir_temporary].has_mem_address = true;
                ar->vregs[decl->decl_variable.ir_temporary].address = decl->decl_variable.stack_offset;
            }
            ir_set_vreg_addr(gen, decl->decl_variable.ir_temporary, ar->offset, type_primitive_float(decl->decl_variable.type));
        }

        ar->offset -= (decl->decl_variable.type->size_bits / 8);

#if PRINT_VARIABLE_INFO || 1
        fprintf(stdout, "variable[SB+%d] %.*s => ", decl->decl_variable.stack_offset,
            decl->decl_constant.name->length, decl->decl_constant.name->data);
        if(type_primitive_float(decl->decl_variable.type))
            fprintf(stdout, "tf%d\n", decl->decl_variable.ir_temporary);
        else
            fprintf(stdout, "t%d\n", decl->decl_variable.ir_temporary);
#endif
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
        t = ir_new_tempf(gen);
        if(op_type->size_bits == 32)
        {
            // -> r32
            type = (type_primitive_uint(cast_type)) ? IR_CVT_R32_UI : IR_CVT_R32_I;
        }
        else
        {
            // -> r64
            type = (type_primitive_uint(cast_type)) ? IR_CVT_R64_UI : IR_CVT_R64_I;
        }
        
        iri_emit_cvt(gen, type, op_temp, t, op_type->size_bits / 8, cast_type->size_bits / 8);
    }
    else
    {
        t = ir_new_temp(gen);
        if(type_primitive_uint(cast_type) || type_primitive_bool(cast_type))
        {
            iri_emit_cvt(gen, IR_CVT_UI, op_temp, t, op_type->size_bits / 8, cast_type->size_bits / 8);
        }
        else if(type_primitive_sint(cast_type))
        {
            iri_emit_cvt(gen, IR_CVT_SI, op_temp, t, op_type->size_bits / 8, cast_type->size_bits / 8);
        }
    }

    return t;
}

IR_Reg
ir_gen_expr_unary_cast(IR_Generator* gen, Light_Ast* expr, int op_temp, bool load)
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
ir_gen_expr_unary(IR_Generator* gen, Light_Ast* expr, bool load)
{
    bool lval = expr->flags & AST_FLAG_EXPRESSION_LVALUE;

    // when the operand is a variable and the op is address of,
    // the expression address must be loaded, instead of value
    IR_Reg t1 = ir_gen_expr(gen, expr->expr_unary.operand, (expr->expr_unary.op != OP_UNARY_ADDRESSOF));
    IR_Reg t2 = t1;

    switch(expr->expr_unary.op)
    {
        case OP_UNARY_DEREFERENCE: {
            // only perform a dereference when it is an lvalue
            if(load)
            {
                t2 = ir_new_reg(gen, expr->type);
                iri_emit_load(gen, t1, t2, (IR_Value){0}, expr->type->size_bits / 8, type_primitive_float(expr->type));
            }
        } break;

        case OP_UNARY_ADDRESSOF:
            break;
        
        case OP_UNARY_PLUS:
            break;
        case OP_UNARY_MINUS: {
            t2 = ir_new_reg(gen, expr->type);
            iri_emit_neg(gen, t1, t2, expr->type->size_bits / 8, type_primitive_float(expr->type));
        } break;
        case OP_UNARY_BITWISE_NOT: {
            t2 = ir_new_reg(gen, expr->type);
            iri_emit_not(gen, t1, t2, expr->type->size_bits / 8);
        } break;
        case OP_UNARY_LOGIC_NOT: {
            t2 = ir_new_reg(gen, expr->type);
            iri_emit_logic_not(gen, t1, t2, expr->type->size_bits / 8);
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
ir_gen_expr_binary(IR_Generator* gen, Light_Ast* expr, bool load)
{
    IR_Reg t1 = ir_gen_expr(gen, expr->expr_binary.left, true);
    IR_Reg t2 = ir_gen_expr(gen, expr->expr_binary.right, true);
    IR_Reg t3 = ir_new_reg(gen, expr->type);

    bool fp = type_primitive_float(expr->type);

    int byte_size = expr->type->size_bits / 8;

    IR_Instruction_Type instr_type = IR_NONE;
    switch(expr->expr_binary.op) {
        case OP_BINARY_PLUS:            instr_type = (fp) ? IR_ADDF : IR_ADD; break;
        case OP_BINARY_MINUS:           instr_type = (fp) ? IR_SUBF : IR_SUB; break;
        case OP_BINARY_MULT:            instr_type = (fp) ? IR_MULF : IR_MUL; break;
        case OP_BINARY_DIV:             instr_type = (fp) ? IR_DIVF : IR_DIV; break;
        case OP_BINARY_MOD:             instr_type = IR_MOD; break;
        case OP_BINARY_AND:             instr_type = IR_AND; break;
        case OP_BINARY_OR:              instr_type = IR_OR; break;
        case OP_BINARY_XOR:             instr_type = IR_XOR; break;
        case OP_BINARY_SHL:             instr_type = IR_SHL; break;
        case OP_BINARY_SHR:             instr_type = IR_SHR; break;
        case OP_BINARY_LOGIC_AND:       instr_type = IR_LAND; break;
        case OP_BINARY_LOGIC_OR:        instr_type = IR_LOR; break;
        case OP_BINARY_VECTOR_ACCESS:   instr_type = IR_ADD; break;

        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE:
        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL:   return ir_gen_expr_cond(gen, expr, t1, t2, t3, fp);
        default: break;
    }
    iri_emit_arith(gen, instr_type, t1, t2, t3, (IR_Value){0}, byte_size);
    
    if(expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS && load)
    {
        IR_Reg t4 = ir_new_reg(gen, expr->type);
        iri_emit_load(gen, t3, t4, (IR_Value){0}, expr->type->size_bits / 8, type_primitive_float(expr->type));
        return t4;
    }

    return t3;
}

IR_Reg
ir_gen_expr_lit_prim(IR_Generator* gen, Light_Ast* expr)
{
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
    IR_Reg t = IR_REG_NONE;
    if(type_primitive_float(expr->type))
        t = ir_new_tempf(gen);
    else
        t = ir_new_temp(gen);
    iri_emit_mov(gen, IR_REG_NONE, t, value, iri_value_byte_size(value), type_primitive_float(expr->type));

    return t;
}

IR_Reg
ir_gen_expr_variable(IR_Generator* gen, Light_Ast* expr, bool load)
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
        array_push(gen->decl_patch, patch);
        iri_emit_mov(gen, IR_REG_NONE, t, (IR_Value){.type = IR_VALUE_U64, .v_u64 = 0}, type_pointer_size_bits() / 8, false);
    }
    else if(vdecl->kind == AST_DECL_VARIABLE)
    {
        Light_Ast_Decl_Variable* decl = &vdecl->decl_variable;
        // if it is not loaded in a temporary, then load it
        if(load && !(decl->flags & DECL_VARIABLE_FLAG_LOADED))
        {
            // LOAD SB+imm -> t
            t = decl->ir_temporary;
            iri_emit_load(gen, IR_REG_STACK_BASE, t, 
                (IR_Value){.v_s32 = decl->stack_offset, .type = IR_VALUE_S32},
                expr->type->size_bits / 8, type_primitive_float(expr->type));
            decl->flags |= DECL_VARIABLE_FLAG_LOADED;
        }
        else if(load)
        {
            t = decl->ir_temporary;
        }
        else
        {
            // LEA
            t = ir_new_temp(gen);
            iri_emit_lea(gen, IR_REG_STACK_BASE, t, 
                (IR_Value){.v_s32 = decl->stack_offset, .type = IR_VALUE_S32},
                (int)type_pointer_size_bits() / 8);
        }
    }
    else if (vdecl->kind == AST_DECL_CONSTANT)
    {
        t = ir_gen_expr(gen, vdecl->decl_constant.value, load);
    }

    return t;
}

IR_Reg
ir_gen_expr_proc_call(IR_Generator* gen, Light_Ast* expr, bool load)
{
    IR_Reg caller = ir_gen_expr(gen, expr->expr_proc_call.caller_expr, true);

    // push the arguments
    for(int i = 0; i < expr->expr_proc_call.arg_count; ++i)
    {
        Light_Ast* arg = expr->expr_proc_call.args[i];
        IR_Reg arg_r = ir_gen_expr(gen, arg, true);
        iri_emit_push(gen, arg_r, (IR_Value){0}, arg->type->size_bits / 8);
    }

    iri_emit_call(gen, caller, (IR_Value){0}, expr->type->size_bits / 8);

    return IR_REG_PROC_RET;
}

IR_Reg
ir_gen_expr_dot(IR_Generator* gen, Light_Ast* expr, bool load)
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

    IR_Reg t = ir_gen_expr(gen, expr->expr_dot.left, false);

    IR_Reg tres = IR_REG_NONE;
    if(ltype->kind == TYPE_KIND_STRUCT)
    {
        int32_t offset = 0;
        tres = ir_new_temp(gen);
        for(int i = 0; i < ltype->struct_info.fields_count; ++i)
        {
            if(ltype->struct_info.fields[i]->decl_variable.name->data == expr->expr_dot.identifier->data)
            {
                offset = (int32_t)(ltype->struct_info.offset_bits[i] / 8);
                break;
            }
        }
        iri_emit_arith(gen, IR_ADD, t, IR_REG_NONE, tres, (IR_Value){.type = IR_VALUE_S32, .v_s32 = offset}, type_pointer_size_bits() / 8);
    }
    else if(ltype->kind == TYPE_KIND_UNION)
    {
        // nothing need to be done, since offset in the union is always 0
        tres = t;
    }

    if(load)
    {
        IR_Reg r = ir_new_reg(gen, expr->type);
        assert(type_alias_root(expr->type)->kind == TYPE_KIND_PRIMITIVE);
        iri_emit_load(gen, tres, r, (IR_Value){0}, expr->type->size_bits / 8, type_primitive_float(expr->type));
        return r;
    }
    return tres;
}

IR_Reg
ir_gen_expr_lit_struct(IR_Generator* gen, Light_Ast* expr)
{
    int size_bytes = expr->type->size_bits / 8;
    // alocate bytes in the stack for it
    iri_emit_arith(gen, IR_SUB, IR_REG_STACK_PTR, IR_REG_NONE, IR_REG_STACK_PTR,
        (IR_Value){.type = IR_VALUE_S32, .v_s32 = size_bytes}, type_pointer_size_bits() / 8);
    
    Light_Type* struct_type = type_alias_root(expr->type);
    for(int i = 0, offset = 0;
        i < array_length(expr->expr_literal_struct.struct_exprs);
        ++i, offset = struct_type->struct_info.offset_bits[i] / 8)
    {
        Light_Ast* e = expr->expr_literal_struct.struct_exprs[i];
        IR_Reg expt = ir_gen_expr(gen, e, true);
        if(e->flags & AST_FLAG_EXPRESSION_LVALUE)
        {
            iri_emit_copy(gen, expt, IR_REG_STACK_PTR, (IR_Value){.type = IR_VALUE_S32, .v_s32 = offset}, e->type->size_bits / 8);
        }
        else
        {
            iri_emit_store(gen, expt, IR_REG_STACK_PTR, (IR_Value){.type = IR_VALUE_S32, .v_s32 = offset}, e->type->size_bits / 8,
                type_primitive_float(e->type));
        }
    }

    IR_Reg t = ir_new_temp(gen);
    iri_emit_lea(gen, IR_REG_STACK_PTR, t, (IR_Value){0}, type_pointer_size_bits() / 8);
    return t;
}

IR_Reg
ir_gen_expr_lit_array(IR_Generator* gen, Light_Ast* expr)
{
    int size_bytes = (expr->type->size_bits / 8);
    // alocate bytes in the stack for it
    iri_emit_arith(gen, IR_SUB, IR_REG_STACK_PTR, IR_REG_NONE, IR_REG_STACK_PTR,
        (IR_Value){.type = IR_VALUE_S32, .v_s32 = size_bytes}, type_pointer_size_bits() / 8);

    if(expr->expr_literal_array.raw_data)
    {
        // raw data
        // TODO(psv): optimize to store 4 or more bytes at a time
        IR_Reg t1 = ir_new_temp(gen);
        // skip " and leave " out
        for(int i = 1; i < expr->expr_literal_array.data_length_bytes - 1; ++i)
        {
            iri_emit_mov(gen, IR_REG_NONE, t1, (IR_Value){.type = IR_VALUE_U8, .v_u8 = expr->expr_literal_array.data[i]},
                8, false);
            iri_emit_store(gen, t1, IR_REG_STACK_PTR, 
                (IR_Value){.type = IR_VALUE_S32, .v_s32 = i}, 1, false);
        }
    }
    else
    {
        // expressions array
        for(int i = 0, offset = 0; i < array_length(expr->expr_literal_array.array_exprs); ++i)
        {
            Light_Ast* e = expr->expr_literal_array.array_exprs[i];
            IR_Reg expt = ir_gen_expr(gen, e, true);
            if(e->flags & AST_FLAG_EXPRESSION_LVALUE)
            {
                iri_emit_copy(gen, expt, IR_REG_STACK_PTR, (IR_Value){.type = IR_VALUE_S32, .v_s32 = offset}, e->type->size_bits / 8);
            }
            else
            {
                iri_emit_store(gen, expt, IR_REG_STACK_PTR, (IR_Value){.type = IR_VALUE_S32, .v_s32 = offset}, e->type->size_bits / 8,
                    type_primitive_float(e->type));
            }

            offset += (e->type->size_bits / 8);
        }
    }

    IR_Reg t = ir_new_temp(gen);
    iri_emit_lea(gen, IR_REG_STACK_PTR, t, (IR_Value){0}, type_pointer_size_bits() / 8);
    return t;
}

IR_Reg
ir_gen_expr(IR_Generator* gen, Light_Ast* expr, bool load)
{
    IR_Reg res = IR_REG_NONE;

    switch(expr->kind)
    {
        case AST_EXPRESSION_LITERAL_PRIMITIVE: res =  ir_gen_expr_lit_prim(gen, expr); break;
        case AST_EXPRESSION_VARIABLE:          res =  ir_gen_expr_variable(gen, expr, load); break;
        case AST_EXPRESSION_BINARY:            res =  ir_gen_expr_binary(gen, expr, load); break;
        case AST_EXPRESSION_UNARY:             res =  ir_gen_expr_unary(gen, expr, load); break;
        case AST_EXPRESSION_PROCEDURE_CALL:    res =  ir_gen_expr_proc_call(gen, expr, load); break;
        case AST_EXPRESSION_DOT:               res =  ir_gen_expr_dot(gen, expr, load); break;
        case AST_EXPRESSION_LITERAL_ARRAY:     res =  ir_gen_expr_lit_array(gen, expr); break;
        case AST_EXPRESSION_LITERAL_STRUCT:    res =  ir_gen_expr_lit_struct(gen, expr); break;
        default: break;
    }

    return res;
}

void
ir_gen_comm_if(IR_Generator* gen, Light_Ast* stmt)
{
    // if(condition)
    IR_Reg cond_temp = ir_gen_expr(gen, stmt->comm_if.condition, true);

    // jrz t, 0 -> else
    int if_true_index = iri_current_instr_index(gen);
    iri_emit_jrz(gen, cond_temp, (IR_Value){.type = IR_VALUE_TO_BE_FILLED}, 4);    // TODO(psv): identify proper size

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
    IR_Reg cond_temp = ir_gen_expr(gen, stmt->comm_while.condition, true);

    int while_true_index = iri_current_instr_index(gen);
    // jrz t, 0 -> end
    iri_emit_jrz(gen, cond_temp, (IR_Value){.type = IR_VALUE_TO_BE_FILLED}, 32);

    if(stmt->comm_while.body)
    {
        ir_gen_comm(gen, stmt->comm_while.body);
    }

    // generate while jump to beginning
    int jmp_start_index = iri_current_instr_index(gen);
    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = while_start_index - jmp_start_index}, 32);

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
    ir_gen_commands(gen, stmt->comm_for.prologue, array_length(stmt->comm_for.prologue));
    
    // jump over epilogue
    int jmp_over_ep_index = iri_current_instr_index(gen);
    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = 0}, 32);

    int for_start_index = iri_current_instr_index(gen);
    array_push(gen->loop_start_labels, for_start_index);

    // generate epilogue
    ir_gen_commands(gen, stmt->comm_for.epilogue, array_length(stmt->comm_for.epilogue));

    int cond_index = iri_current_instr_index(gen);

    IR_Instruction* jmp_over_ep_index_instr = iri_get_temp_instr_ptr(gen, jmp_over_ep_index);
    jmp_over_ep_index_instr->imm.v_s32 = cond_index - jmp_over_ep_index;

    // for(condition)
    IR_Reg cond_temp = ir_gen_expr(gen, stmt->comm_for.condition, true);

    int for_true_index = iri_current_instr_index(gen);
    // jrz t, 0 -> end
    iri_emit_jrz(gen, cond_temp, (IR_Value){.type = IR_VALUE_TO_BE_FILLED}, 32);

    if(stmt->comm_for.body)
    {
        ir_gen_comm(gen, stmt->comm_for.body);
    }

    // generate for jump to beginning
    int jmp_start_index = iri_current_instr_index(gen);
    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = for_start_index - jmp_start_index}, 32);

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

    IR_Reg t2 = ir_gen_expr(gen, expr, primitive_type);


    IR_Value stack_offset = (IR_Value){.type = IR_VALUE_S32, .v_s32 = decl->decl_variable.stack_offset};
    if(primitive_type || rvalue_type->kind == TYPE_KIND_FUNCTION || rvalue_type->kind == TYPE_KIND_POINTER)
    {
        iri_emit_store(gen, t2, IR_REG_STACK_BASE, stack_offset, byte_size, type_primitive_float(rvalue_type));
    }
    else
    {
        IR_Reg t1 = ir_new_reg(gen, rvalue_type);
        iri_emit_lea(gen, IR_REG_STACK_BASE, t1, stack_offset, type_pointer_size_bits() / 8);
        iri_emit_copy(gen, t2, t1, (IR_Value){.type = IR_VALUE_U32, .v_u32 = byte_size}, 
            type_pointer_size_bits() / 8);
    }
}

void
ir_gen_comm_assignment(IR_Generator* gen, Light_Ast_Comm_Assignment* comm)
{
    int byte_size = comm->rvalue->type->size_bits / 8;
    Light_Type* rvalue_type = type_alias_root(comm->rvalue->type);
    bool primitive_type = rvalue_type->kind == TYPE_KIND_PRIMITIVE;

    IR_Reg t2 = ir_gen_expr(gen, comm->rvalue, primitive_type);

    const bool always_store = true;

    if(primitive_type || rvalue_type->kind == TYPE_KIND_FUNCTION || rvalue_type->kind == TYPE_KIND_POINTER)
    {
        if(comm->lvalue->kind == AST_EXPRESSION_VARIABLE && !always_store)
        {
            Light_Ast* var_decl = comm->lvalue->expr_variable.decl;
            iri_emit_mov(gen, t2, var_decl->decl_variable.ir_temporary, (IR_Value){0},
                byte_size, type_primitive_float(comm->rvalue->type));
        }
        else
        {
            IR_Reg t1 = ir_gen_expr(gen, comm->lvalue, false);
            iri_emit_store(gen, t2, t1, (IR_Value){0}, byte_size, type_primitive_float(comm->rvalue->type));
        }
    }
    else
    {
        IR_Reg t1 = ir_gen_expr(gen, comm->lvalue, false);
        iri_emit_copy(gen, t2, t1, (IR_Value){.type = IR_VALUE_U32, .v_u32 = byte_size}, 
            type_pointer_size_bits() / 8);
    }
}

void
ir_gen_commands(IR_Generator* gen, Light_Ast** commands, int comm_count)
{
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

void
ir_gen_comm_block(IR_Generator* gen, Light_Ast* body)
{
    ir_gen_commands(gen, body->comm_block.commands, body->comm_block.command_count);
    #if 0
    // decls
    for(int i = 0; i < body->comm_block.command_count; ++i) {
        Light_Ast* comm = body->comm_block.commands[i];
        if(comm->flags & AST_FLAG_DECLARATION)
            ir_gen_decl(gen, comm);
    }

    // commands
    for(int i = 0; i < body->comm_block.command_count; ++i) {
        Light_Ast* comm = body->comm_block.commands[i];
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
    #endif
}

void
ir_gen_comm_return(IR_Generator* gen, Light_Ast* comm)
{
    if(comm->comm_return.expression)
    {
        IR_Reg t = ir_gen_expr(gen, comm->comm_return.expression, true);
        iri_emit_mov(gen, t, IR_REG_PROC_RET, (IR_Value){0}, 
            comm->comm_return.expression->type->size_bits / 8, 
            type_primitive_float(comm->comm_return.expression->type));
    }
    iri_emit_ret(gen);
}

void
ir_gen_comm_continue(IR_Generator* gen, Light_Ast* comm)
{
    int lvl = (int)comm->comm_continue.level_value - 1;
    int index = iri_current_instr_index(gen);

    int diff = gen->loop_start_labels[array_length(gen->loop_start_labels) - 1 - lvl] - index;
    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = diff}, 32);
}

void
ir_gen_comm_break(IR_Generator* gen, Light_Ast* comm)
{
    int lvl = (int)comm->comm_break.level_value;
    int index = iri_current_instr_index(gen);
    iri_emit_jr(gen, (IR_Value){.type = IR_VALUE_S32, .v_s32 = 0}, 32);
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

void
ir_gen_proc(IR_Generator* gen, Light_Ast* proc)
{
    Light_Ast* body = proc->decl_proc.body;

    ir_new_activation_record(gen);

    // setup arguments in the stack
    proc->decl_proc.ir_instr_index = iri_current_instr_index(gen);

    // TODO(psv): consider alignment
    int stack_offset = 0;
    for(int i = 0; i < proc->decl_proc.argument_count; ++i)
    {
        Light_Ast* arg = proc->decl_proc.arguments[i];
        ir_gen_decl(gen, arg);
        arg->decl_variable.ir_temporary = ir_new_reg(gen, arg->decl_variable.type);
        arg->decl_variable.stack_index = i + 2;
        arg->decl_variable.stack_offset = stack_offset + arg->decl_variable.type->size_bits / 8;
        stack_offset += (arg->decl_variable.type->size_bits / 8);
    }

    ir_gen_comm_block(gen, proc->decl_proc.body);
}

void
ir_patch_proc_calls(IR_Generator* gen)
{
    for(int i = 0; i < array_length(gen->decl_patch); ++i)
    {
        IR_Decl_To_Patch dpatch = gen->decl_patch[i];
        IR_Instruction* instr = iri_get_temp_instr_ptr(gen, dpatch.instr_number);
        instr->imm.v_u64 = dpatch.decl->decl_proc.ir_instr_index;
    }
}

void ir_allocate_register(IR_Generator* gen);

void
ir_new_activation_record(IR_Generator* gen)
{
    IR_Activation_Rec ar = {0};
    ar.index = -1;
    ar.offset = -(int)(type_pointer_size_bits() / 8);
    ar.temp_int = 1;   // temp 0 is reserved for proc call return value
    ar.temp_float = 1; // temp 0 is reserved for proc call return value 
    
    // r0
    ar.vregs = array_new(IR_Virtual_Reg);
    IR_Virtual_Reg nvreg = {0};
    array_push(ar.vregs, nvreg);

    // rf0
    ar.vfregs = array_new(IR_Virtual_FReg);
    IR_Virtual_FReg nfvreg = {0};
    array_push(ar.vfregs, nfvreg);

    for(int i = 0; i < ARRAY_LENGTH(ar.pregs); ++i)
        ar.pregs[i].vreg = IR_REG_NONE;
    ar.pregs[0].next = -100; // never pick the return value register

    ar.insertions = array_new(IR_Instr_Insert);

    array_push(gen->ars, ar);
}

void ir_generate(IR_Generator* gen, Light_Ast** ast) {
    gen->instructions = array_new(IR_Instruction);
    gen->decl_patch = array_new(IR_Decl_To_Patch);
    gen->jmp_patch = array_new(IR_Instr_Jmp_Patch);
    gen->loop_start_labels = array_new(int);
    gen->node_ranges = array_new(IR_Node_Range);
    gen->ars = array_new(IR_Activation_Rec);

    for(u64 i = 0; i < array_length(ast); ++i) {
        Light_Ast* n = ast[i];
        switch(n->kind) {
            case AST_DECL_PROCEDURE: {
                ir_gen_proc(gen, n);
            } break;
            case AST_DECL_VARIABLE: {
                // TODO(psv): global variables in the data segment
            } break;
            default: break;
        }
    }

    ir_patch_proc_calls(gen);

    iri_print_instructions(gen);

    ir_allocate_register(gen);
    fprintf(stdout, "\n");

    iri_print_instructions(gen);
}

// -----------------------------------
// register allocation
// -----------------------------------

static int
next_preg(IR_Activation_Rec* ar)
{
    for(int i = 1; i < ARRAY_LENGTH(ar->pregs); ++i)
    {
        if(ar->pregs[i].vreg == IR_REG_NONE)
            return i;
    }
    return -1;
}

static int
best_preg(IR_Activation_Rec* ar)
{
    int p = -1;
    int max = -1;
    for(int i = 0; i < ARRAY_LENGTH(ar->pregs); ++i)
    {
        if(ar->pregs[i].next > max)
            max = i;
    }
    return max;
}

static int
reg_alloc(IR_Activation_Rec* ar, IR_Reg reg, int byte_size, int index)
{
    int p = next_preg(ar);
    if(p == -1)
    {
        p = best_preg(ar);
        // emit store p -> vreg address
        IR_Instruction store = iri_new(IR_STORE, p, IR_REG_STACK_BASE, IR_REG_NONE,
            (IR_Value){.type = IR_VALUE_S32, .v_s32 = ar->vregs[reg].address}, byte_size);
        store.ot1 = ar->pregs[p].vreg;
        store.ot2 = reg;
        IR_Instr_Insert insert = {
            .index = index,
            .inst = store
        };
        array_push(ar->insertions, insert);
    }
    if(ar->pregs[p].vreg != IR_REG_NONE)
    {
        ar->vregs[ar->pregs[p].vreg].preg_mapped = IR_REG_NONE;
    }
    ar->pregs[p].next = -1;
    ar->pregs[p].vreg = reg;
    ar->vregs[reg].preg_mapped = p;
    return p;
}

static int
ensure(IR_Activation_Rec* ar, IR_Reg reg, int byte_size, int index)
{
    int p = -1;
    if(reg <= IR_REG_PROC_RET) return reg;
    if(ar->vregs[reg].preg_mapped != -1)
        p = ar->vregs[reg].preg_mapped;
    else
    {
        p = reg_alloc(ar, reg, byte_size, index);
        // emit load from reg.addr -> p
        IR_Instruction load = iri_new(IR_LOAD, IR_REG_STACK_BASE, IR_REG_NONE, p, 
            (IR_Value){.type = IR_VALUE_S32, .v_s32 = ar->vregs[reg].address}, byte_size);
        load.ot1 = reg;
        load.ot3 = ar->pregs[p].vreg;
        IR_Instr_Insert insert = {
            .index = index,
            .inst = load
        };
        array_push(ar->insertions, insert);
    }
    return p;
}

static void
free_preg(IR_Activation_Rec* ar, int pr)
{
    ar->pregs[pr].next = 10000;
    ar->pregs[pr].vreg = IR_REG_NONE;
}

static bool
needed_after(IR_Activation_Rec* ar, IR_Reg r, int index)
{
    if (r <= IR_REG_PROC_RET) return true;
    int last_use = ar->vregs[r].uses[array_length(ar->vregs[r].uses) - 1];
    return (last_use > index);
}

static void
ir_rewrite_with_pregs(IR_Instruction* instr, int p1, int p2, int p3)
{
    if (instr->t1 != IR_REG_NONE) instr->t1 = p1;
    if (instr->t2 != IR_REG_NONE) instr->t2 = p2;
    if (instr->t3 != IR_REG_NONE) instr->t3 = p3;
}

static void
update_next(IR_Activation_Rec* ar, IR_Reg treg, int preg, int index)
{
    if (preg <= IR_REG_PROC_RET) return;
    for(int i = 0; i < array_length(ar->vregs[treg].uses); ++i)
    {
        if(ar->vregs[treg].uses[i] > index)
        {
            ar->pregs[preg].next = ar->vregs[treg].uses[i];
            break;
        }
    }
}

void
ir_allocate_register(IR_Generator* gen)
{
    IR_Instruction* instructions = gen->instructions;

    for(int i = 0; i < array_length(instructions); ++i)
    {
        IR_Instruction* inst = &instructions[i];
        IR_Activation_Rec* ar = &gen->ars[inst->activation_record_index];
        inst->ot1 = inst->t1;
        inst->ot2 = inst->t2;
        inst->ot3 = inst->t3;
        int p1 = ensure(ar, inst->t1, inst->byte_size, i);
        int p2 = ensure(ar, inst->t2, inst->byte_size, i);

        // allocate physical register for operands
        if(!needed_after(ar, inst->t1, i))
            free_preg(ar, p1);
        if(!needed_after(ar, inst->t2, i))
            free_preg(ar, p2);

        // alocate physical register for result
        int p3 = (inst->t3 > IR_REG_PROC_RET) ? reg_alloc(ar, inst->t3, inst->byte_size /* consider dst and src byte size */, i) : inst->t3;
        
        // rewrite instruction to p1, p2 -> p3
        ir_rewrite_with_pregs(inst, p1, p2, p3);

        // update next instruction index where each virtual register is used
        if(needed_after(ar, inst->t1, i))
            update_next(ar, inst->t1, p1, i);
        if(needed_after(ar, inst->t2, i))
            update_next(ar, inst->t2, p2, i);
        update_next(ar, inst->t3, p3, i);
    }
}