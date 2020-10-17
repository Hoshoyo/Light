#include "x86.h"
#include "../../assembler/assembler.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <light_array.h>
#include "../light_elf.h"
#include "../light_pecoff.h"

/*
    NOTE(psv): Apparently idata without any imports crashes the executable
*/

typedef struct {
    u8* base;
    u8* at;

    X86_Import* imports;
    X86_Patch*  relative_patches;
    X86_Data*   data;
    int         data_offset;

    IR_Generator* ir_gen;
} X86_Emitter;

// Expect 32 bit register en
static X64_Register
x86_reg32_to_byte_size(X64_Register reg, int byte_size)
{
    switch(byte_size)
    {
        case 1: {
            switch(reg)
            {
                case EAX: return AL;
                case EBX: return BL;
                case ECX: return CL;
                case EDX: return DL;
                case ESP: return SPL;
                case EBP: return BPL;
                case ESI: return SIL;
                case EDI: return DIL;
                case R8D: return R8B;
                case R9D: return R9B;
                case R10D: return R10B;
                case R11D: return R11B;
                case R12D: return R12B;
                case R13D: return R13B;
                case R14D: return R14B;
                case R15D: return R15B;
                default: assert(0); break;
            }
        } break;
        case 2: {
            switch(reg)
            {
                case EAX: return AX;
                case EBX: return BX;
                case ECX: return CX;
                case EDX: return DX;
                case ESP: return SP;
                case EBP: return BP;
                case ESI: return SI;
                case EDI: return DI;
                case R8D: return R8W;
                case R9D: return R9W;
                case R10D: return R10W;
                case R11D: return R11W;
                case R12D: return R12W;
                case R13D: return R13W;
                case R14D: return R14W;
                case R15D: return R15W;
                default: assert(0); break;
            }
        } break;
        case 4: {
            return reg;
        } break;
        default: return REG_NONE;
    }
    return REG_NONE;
}

static X64_Register
ir_to_x86_Reg(IR_Reg r, int byte_size)
{
    if (r == IR_REG_NONE) return REG_NONE;
    // 8 16 32
    X64_Register n[][4] = {
        {REG_NONE, REG_NONE, REG_NONE, REG_NONE},
        {BL,   BX,   REG_NONE, EBX},
        {CL,   CX,   REG_NONE, ECX},
        {DL,   DX,   REG_NONE, EDX},
    };

    switch(r)
    {
        case IR_REG_PROC_RET:{
            switch(byte_size)
            {
                case 1:  return AL;
                case 2: return AX;
                case 4: return EAX;
                default: return REG_NONE;
            }
        } break;
        case IR_REG_INSTR_PTR:{
            // should not be used directly
            return REG_NONE;
        } break;
        case IR_REG_STACK_BASE: {
            switch(byte_size)
            {
                case 1:  return BPL;
                case 2: return BP;
                case 4: return EBP;
                default: return REG_NONE;
            }
        } break;
        case IR_REG_STACK_PTR: {
            switch(byte_size)
            {
                case 1:  return SPL;
                case 2: return SP;
                case 4: return ESP;
                default: return REG_NONE;
            }
        } break;
        default:
            return n[r][byte_size - 1];
        break;
    }
    return REG_NONE;
}

Instr_Emit_Result
x86_emit_lea(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);
    X64_Register rsrc = ir_to_x86_Reg(instr->t1, instr->byte_size);

    em->at = emit_lea(&info, em->at, instr->byte_size * 8, INDIRECT_DWORD_DISPLACED,
        rdst, rsrc,
        (instr->imm.type == IR_VALUE_S8) ? instr->imm.v_s8 : 0, 
        (instr->imm.type == IR_VALUE_S32) ? instr->imm.v_s32 : 0);

    return info;
}

Instr_Emit_Result
x86_emit_mov(X86_Emitter* em, IR_Instruction* instr, int index)
{
    Instr_Emit_Result info = {0};
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);
    X64_Register rsrc = ir_to_x86_Reg(instr->t1, instr->byte_size);

    if(instr->imm.type != IR_VALUE_NONE)
    {
        // imm to reg
        u8* patch_addr = em->at;
        em->at = emit_mov_oi(&info, em->at, rdst, (Int_Value){.v64 = instr->imm.v_u64});

        if(instr->flags & IIR_FLAG_INSTRUCTION_OFFSET)
        {
            X86_Patch patch = {0};
            patch.generate_relocation = true;
            patch.issuer_addr = patch_addr;
            patch.issuer_offset = patch_addr - em->base;
            patch.issuer_imm_offset = info.immediate_offset;
            patch.addr = patch_addr + info.immediate_offset;
            patch.bytes = 4;
            patch.extra_offset = info.instr_byte_size - 1;
            patch.instr_byte_size = instr->byte_size;
            patch.issuer_index = index;
            patch.rel_index_offset = instr->imm.v_s32;
            array_push(em->relative_patches, patch);
        }
        else if(instr->flags & IIR_FLAG_IMPORT_OFFSET)
        {
            // Get the import table and fill the patch offset with the
            // address of the current instruction
            IR_Import_Entry* imp = &em->ir_gen->import_table[instr->imm.v_u64];
            X86_Import import = {
                .decl = imp->decl, 
                .import_table_index = (int)instr->imm.v_u64, 
                .patch_addr = patch_addr + info.immediate_offset
            };
            array_push(em->imports, import);
        }
    }
    else
    {
        // reg to reg
        em->at = emit_mov_reg(&info, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, 
            instr->byte_size * 8, rdst, rsrc, 0, 0);
    }
    return info;
}

Instr_Emit_Result
x86_emit_store(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rdst = ir_to_x86_Reg(instr->t2, 4);
    X64_Register rsrc = ir_to_x86_Reg(instr->t1, instr->byte_size);

    X64_Addressing_Mode mode = INDIRECT;
    if(instr->imm.type == IR_VALUE_S8) mode = INDIRECT_BYTE_DISPLACED;
    else if(instr->imm.type == IR_VALUE_S32) mode = INDIRECT_DWORD_DISPLACED;

    em->at = emit_mov_reg(&info, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, mode, 
        instr->byte_size * 8, rdst, rsrc,
        (instr->imm.type == IR_VALUE_S8) ? instr->imm.v_s8 : 0,
        (instr->imm.type == IR_VALUE_S32) ? instr->imm.v_s32 : 0);
    return info;
}

Instr_Emit_Result
x86_emit_load(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rsrc = ir_to_x86_Reg(instr->t1, instr->src_byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->dst_byte_size);

    X64_Addressing_Mode mode = INDIRECT;
    if(instr->imm.type == IR_VALUE_S8) mode = INDIRECT_BYTE_DISPLACED;
    else if(instr->imm.type == IR_VALUE_S32) mode = INDIRECT_DWORD_DISPLACED;

    em->at = emit_mov_reg(&info, em->at, (instr->byte_size == 1) ? MOV_RM8 : MOV_RM, mode, 
        instr->byte_size * 8, rsrc, rdst,
        (instr->imm.type == IR_VALUE_S8) ? instr->imm.v_s8 : 0,
        (instr->imm.type == IR_VALUE_S32) ? instr->imm.v_s32 : 0);
    return info;
}

static X64_Arithmetic_Instr
x86_ir_arith_to_x86_arith(IR_Instruction* instr)
{
    switch(instr->type)
    {
        case IR_ADD:  return ARITH_ADD;
        case IR_SUB:  return ARITH_SUB;
        case IR_LOR:
        case IR_OR:   return ARITH_OR;
        case IR_LAND:
        case IR_AND:  return ARITH_AND;
        case IR_XOR:  return ARITH_XOR;
        default: assert(0); break;
    }
    return -1;
}

static X64_Multiplicative_Instr
x86_ir_mul_to_x86_arith(IR_Instruction* instr)
{
    switch(instr->type)
    {
        case IR_NOT:  return NOT;
        case IR_NEG:  return NEG;
        case IR_MUL:  return MUL;
        case IR_IMUL: return IMUL;
        case IR_MOD:
        case IR_DIV:  return DIV;
        case IR_IDIV: return IDIV;
        default: assert(0); break;
    }
    return -1;
}

Instr_Emit_Result
x86_emit_shift(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rop2 = ir_to_x86_Reg(instr->t2, instr->byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);
    
    bool op1_in_cl = (rop1 == CL || rop1 == CX || rop1 == ECX);
    bool op2_not_in_cl = (rop2 != CL && rop2 != CX && rop2 != ECX);

    if(op1_in_cl)
    {
        // mov ecx to eax
        em->at = emit_mov_reg(0, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, instr->byte_size * 8,
            x86_reg32_to_byte_size(EAX, instr->byte_size), rop1, 0, 0);
        rop1 = x86_reg32_to_byte_size(EAX, instr->byte_size);
    }

    if(op2_not_in_cl)
    {
        // push ecx
        // mov to ecx
        em->at = emit_push_reg(0, em->at, DIRECT, ECX, 0, 0);
        em->at = emit_mov_reg(0, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, instr->byte_size * 8, 
            x86_reg32_to_byte_size(ECX, instr->byte_size), rop2, 0, 0);
    }

    // shl op1, ecx
    em->at = emit_shift_reg(&info, em->at, instr->byte_size * 8, (instr->type == IR_SHL) ? SHL : SHR,
        DIRECT, rop1, 0, 0);

    if(op2_not_in_cl)
    {
        em->at = emit_pop_reg(0, em->at, ECX);
    }

    // mov to rdst
    em->at = emit_mov_reg(0, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, instr->byte_size * 8,
        rdst, rop1, 0, 0);

    return info;
}

Instr_Emit_Result
x86_emit_mul(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop2 = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rop1 = ir_to_x86_Reg(instr->t2, instr->byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);

    if(rop1 == rdst)
    {
        X64_Register temp = rop1;
        rop1 = rop2;
        rop2 = temp;
    }

    bool not_edx = rdst != DL && rdst != DX && rdst != EDX;
    // we need to save edx, since the result of the multiplication is always on edx
    if(not_edx)
    {
        em->at = emit_push_reg(0, em->at, DIRECT, EDX, 0, 0);
        // EDX needs to be zero to not cause integer overflow
        em->at = emit_arith_mr(0, ARITH_XOR, em->at, EDX, EDX, DIRECT, 0, 0);
    }

    // mov to eax
    em->at = emit_mov_reg(0, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, 
        instr->byte_size * 8, x86_reg32_to_byte_size(EAX, instr->byte_size), rop2, 0, 0);

    em->at = emit_mul(&info, em->at, instr->byte_size * 8, x86_ir_mul_to_x86_arith(instr), rop1, DIRECT, 0, 0);

    // move result that is in eax/edx to the destination register
    {
        X64_Register result_reg = (instr->type == IR_MOD) ? EDX : EAX;
        em->at = emit_mov_reg(0, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, 
            instr->byte_size * 8, rdst, x86_reg32_to_byte_size(result_reg, instr->byte_size), 0, 0);
    }

    // pop back edx if it was not the result in the first place
    if(not_edx)
    {
        em->at = emit_pop_reg(0, em->at, EDX);
    }

    return info;
}

Instr_Emit_Result
x86_emit_not_neg(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);

    // mov op1 to dest and then neg it
    if(rop1 != rdst)
    {
        em->at = emit_mov_reg(0, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, 
            instr->byte_size * 8, rdst, rop1, 0, 0);
    }
    em->at = emit_mul(&info, em->at, instr->byte_size * 8, x86_ir_mul_to_x86_arith(instr),
        rdst, DIRECT, 0, 0);

    return info;
}

Instr_Emit_Result
x86_emit_arith(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rop2 = ir_to_x86_Reg(instr->t2, instr->byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);

    if(instr->imm.type != IR_VALUE_NONE)
    {
        // add rdst, imm -> reg
        em->at = emit_arith_mi(&info, em->at, x86_ir_arith_to_x86_arith(instr), DIRECT, rop1, (Int_Value){.v64 = instr->imm.v_u64}, 0, 0);
    }
    else
    {
        // add rop1, rop2 -> rop1
        em->at = emit_arith_mr(&info, x86_ir_arith_to_x86_arith(instr), em->at, rop1, rop2, DIRECT, 0, 0);
    }

    if (rop1 != rdst)
    {
        // mov rop2 -> rdst
        em->at = emit_mov_reg(0, em->at, MOV_MR, DIRECT, instr->byte_size, rdst, rop1, 0, 0);
    }
    return info;
}

Instr_Emit_Result
x86_emit_cvt_si(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->src_byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->dst_byte_size);

    // src is < dest, convert with sign extension
    if(instr->src_byte_size < instr->dst_byte_size)
    {
        em->at = emit_movsx(&info, em->at, DIRECT, instr->src_byte_size * 8, instr->dst_byte_size * 8,
            rdst, rop1, 0, 0);
    }
    else if(rop1 != rdst)
    {
        em->at = emit_mov_reg(0, em->at, MOV_MR, DIRECT, instr->byte_size * 8, rdst, rop1, 0, 0);
    }

    return info;
}

Instr_Emit_Result
x86_emit_cvt_ui(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->src_byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->dst_byte_size);

    // src is < dest, convert with zero extension
    if(instr->src_byte_size < instr->dst_byte_size)
    {
        em->at = emit_movzx(&info, em->at, DIRECT, instr->src_byte_size * 8, instr->dst_byte_size * 8,
            rdst, rop1, 0, 0);
    }
    else if (rop1 != rdst)
    {
        em->at = emit_mov_reg(0, em->at, MOV_MR, DIRECT, instr->byte_size * 8, rdst, rop1, 0, 0);
    }

    return info;
}

Instr_Emit_Result
x86_emit_cvt_r32_r64(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_XMM_Register rop1 = instr->t1;
    X64_XMM_Register rdst = instr->t3;

    em->at = emit_cvt(&info, em->at, SSE_CVT_F64_F32, DIRECT, rop1, rdst, 0, 0, true);

    return info;
}

Instr_Emit_Result
x86_emit_cvt_r64_r32(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_XMM_Register rop1 = instr->t1;
    X64_XMM_Register rdst = instr->t3;

    em->at = emit_cvt(&info, em->at, SSE_CVT_F64_F32, DIRECT, rop1, rdst, 0, 0, false);

    return info;
}

Instr_Emit_Result
x86_emit_cvt_float_si(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_XMM_Register rop1 = instr->t1;
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->src_byte_size);

    em->at = emit_cvt(&info, em->at, SSE_CVT_F32_INT32, DIRECT, rop1, rdst, 0, 0, instr->src_byte_size == 4);

    return info;
}

Instr_Emit_Result
x86_emit_cvt_si_float(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->src_byte_size);
    X64_XMM_Register rdst = instr->t3;

    // first convert to 32 bit integer
    if(instr->src_byte_size < 32)
    {
        em->at = emit_movsx(&info, em->at, DIRECT, instr->src_byte_size * 8, instr->dst_byte_size * 8,
            rdst, rop1, 0, 0);
    }

    em->at = emit_cvt(&info, em->at, SSE_CVT_INT32_F32, DIRECT, rop1, rdst, 0, 0, instr->dst_byte_size == 4);

    return info;
}

Instr_Emit_Result
x86_emit_lnot(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);

    if(rop1 != rdst)
    {
        em->at = emit_mov_reg(&info, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, 
            instr->byte_size * 8, rdst, rop1, 0, 0);
    }

    // xor op1, 1
    em->at = emit_arith_mi(0, em->at, ARITH_XOR, DIRECT, rdst, (Int_Value){.v64 = 1}, 0, 0);

    return info;
}

static X64_CMOVcc_Instruction
x86_ir_cmov_to_x86_cmov(IR_Instruction* instr)
{
    switch(instr->type)
    {
        case IR_CMOVEQ:    return CMOVE;
        case IR_CMOVNE:    return CMOVNE;
        case IR_CMOVGS:    return CMOVG;
        case IR_CMOVGES:   return CMOVGE;
        case IR_CMOVLS:    return CMOVL;
        case IR_CMOVLES:   return CMOVLE;
        case IR_CMOVGU:    return CMOVA;
        case IR_CMOVGEU:   return CMOVAE;
        case IR_CMOVLU:    return CMOVB;
        case IR_CMOVLEU:   return CMOVBE;
        default: assert(0); break;
    }
    return 0;
}

static X64_XMM_Arithmetic_Instr
x86_ir_arithf_to_x86_arithf(IR_Instruction* instr)
{
    switch(instr->type)
    {
        case IR_ADDF: return XMM_ADDS;
        case IR_SUBF: return XMM_SUBS;
        case IR_MULF: return XMM_MULS;
        case IR_DIVF: return XMM_DIVS;
        default: break;
    }
    return 0;
}

Instr_Emit_Result
x86_emit_cmp(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rop2 = ir_to_x86_Reg(instr->t2, instr->byte_size);

    em->at = emit_arith_mr(&info, ARITH_CMP, em->at, rop1, rop2, DIRECT, 0, 0);

    return info;
}

Instr_Emit_Result
x86_emit_cmov(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);

    // mov eax, imm
    em->at = emit_mov_oi(&info, em->at, x86_reg32_to_byte_size(EAX, instr->byte_size), (Int_Value){.v64 = instr->imm.v_u64});

    // cmov dst, eax
    em->at = emit_cmov_reg(0, em->at, DIRECT, x86_ir_cmov_to_x86_cmov(instr), 
        instr->byte_size * 8, x86_reg32_to_byte_size(EAX, instr->byte_size), rdst, 0, 0);

    return info;
}

Instr_Emit_Result
x86_emit_hlt(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};

#if defined (_WIN32) || defined(_WIN64)
    em->at = emit_ret(&info, em->at, RET_NEAR);
#elif defined (__linux__)
    // mov ebx, eax
    em->at = emit_mov_reg(&info, em->at, MOV_MR, DIRECT, 32, EBX, EAX, 0, 0);
    // mov eax, 1
    em->at = emit_mov_oi(0, em->at, EAX, (Int_Value){.v32 = 1});
    // int 0x80
    em->at = emit_int(0, em->at, 0x80);
#endif

    return info;
}

Instr_Emit_Result
x86_emit_ret(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    
    if(instr->imm.type == IR_VALUE_S32)
    {
        em->at = emit_ret_stack_pop(&info, em->at, RET_NEAR_STACK_POP, (uint16_t)instr->imm.v_s32);
    }
    else
    {
        em->at = emit_ret(&info, em->at, RET_NEAR);     // TODO(psv): ret far?
    }

    return info;
}

Instr_Emit_Result
x86_emit_pop(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    
    if(instr->flags & IIR_FLAG_IS_FP_OPERAND1)
    {
        X64_XMM_Register rop = instr->t1 - IR_REG_PROC_RETF;
        em->at = emit_movs_mem_to_reg(&info, em->at, INDIRECT, rop, ESP, (instr->byte_size == 4), 0, 0);
        em->at = emit_pop_reg(&info, em->at, EAX);
    }
    else
    {
        X64_Register rop = ir_to_x86_Reg(instr->t1, instr->byte_size);
        em->at = emit_pop_reg(&info, em->at, rop);
    }

    return info;
}

Instr_Emit_Result
x86_emit_push(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    if(instr->flags & IIR_FLAG_IS_FP_OPERAND1)
    {
        X64_XMM_Register rop = instr->t1 - IR_REG_PROC_RETF;
        // allocate space for the xmm register
        em->at = emit_push_reg(&info, em->at, DIRECT, EAX, 0, 0);
        em->at = emit_movs_reg_to_mem(&info, em->at, INDIRECT, rop, ESP, (instr->byte_size == 4), 0, 0);
    }
    else
    {
        X64_Register rop = ir_to_x86_Reg(instr->t1, instr->byte_size);
        em->at = emit_push_reg(&info, em->at, DIRECT, rop, 0, 0);
    }

    return info;
}

Instr_Emit_Result
x86_emit_cond_rjmp(X86_Emitter* em, IR_Instruction* instr, int index)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->byte_size);

    em->at = emit_arith_mi(&info, em->at, ARITH_CMP, DIRECT, rop1, (Int_Value){.v64 = 0}, 0, 0);

    u8* jmp_addr = em->at;
    em->at = emit_jmp_cond_rel32(&info, em->at, (instr->type == IR_JRZ) ? JZ : JNZ, 0);

    X86_Patch patch = {0};
    patch.generate_relocation = false;
    patch.issuer_addr = jmp_addr;
    patch.addr = jmp_addr + info.immediate_offset;
    patch.bytes = sizeof(int);
    patch.rel_index_offset = instr->imm.v_s32;
    patch.instr_byte_size = info.instr_byte_size;
    patch.issuer_index = index;
    array_push(em->relative_patches, patch);

    return info;
}

Instr_Emit_Result
x86_emit_call(X86_Emitter* em, IR_Instruction* instr, int index)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->byte_size);

    em->at = emit_call_reg(&info, em->at, DIRECT, rop1, 0, 0);

    return info;
}

Instr_Emit_Result
x86_emit_rjmp(X86_Emitter* em, IR_Instruction* instr, int index)
{
    Instr_Emit_Result info = {0};

    u8* jmp_addr = em->at;
    em->at = emit_jmp_rel_unconditional(&info, em->at, instr->byte_size * 8, (Int_Value){0});

    X86_Patch patch = {0};
    patch.generate_relocation = false;
    patch.issuer_addr = jmp_addr;
    patch.addr = jmp_addr + info.immediate_offset;
    patch.bytes = sizeof(int);
    patch.rel_index_offset = instr->imm.v_s32;
    patch.instr_byte_size = info.instr_byte_size;
    patch.issuer_index = index;
    array_push(em->relative_patches, patch);

    return info;
}

Instr_Emit_Result
x86_emit_movf(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_XMM_Register rdst = instr->t3 - IR_REG_PROC_RETF;

    X86_Data data = {.length_bytes = instr->byte_size};
    if(instr->byte_size == 4)
        memcpy(data.reg_size_data, &instr->imm.v_r32, 4);
    else if(instr->byte_size == 8)
        memcpy(data.reg_size_data, &instr->imm.v_r64, 8);

    int patch_offset = (em->at - em->base);
    em->at = emit_movs_ds_to_reg(&info, em->at, rdst, instr->byte_size == 4, 0);
    data.patch_offset = patch_offset + info.diplacement_offset;
    em->data_offset += instr->byte_size;

    array_push(em->data, data);

    return info;
}

Instr_Emit_Result
x86_emit_loadf(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = { 0 };
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->src_byte_size);
    X64_XMM_Register rdst = instr->t3 - IR_REG_PROC_RETF;

    if(instr->imm.type == IR_VALUE_S32)
    {
        em->at = emit_movs_mem_to_reg(&info, em->at, INDIRECT_DWORD_DISPLACED, rdst, rop1, instr->byte_size == 4, 0,
            instr->imm.v_s32);
    }
    else
    {
        em->at = emit_movs_mem_to_reg(&info, em->at, INDIRECT, rdst, rop1, instr->byte_size == 4, 0, 0);
    }

    return info;
}

Instr_Emit_Result
x86_emit_storef(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = { 0 };
    X64_XMM_Register rop = instr->t1 - IR_REG_PROC_RETF;
    X64_Register rdest = ir_to_x86_Reg(instr->t2, instr->byte_size);

    if(instr->imm.type == IR_VALUE_S32)
    {
        em->at = emit_movs_reg_to_mem(&info, em->at, INDIRECT_DWORD_DISPLACED, rop, rdest, instr->byte_size == 4, 0,
            instr->imm.v_s32);
    }
    else
    {
        em->at = emit_movs_reg_to_mem(&info, em->at, INDIRECT_BYTE_DISPLACED, rop, rdest, instr->byte_size == 4, 0, 0);
    }

    return info;
}

Instr_Emit_Result
x86_emit_arithf(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = { 0 };
    X64_XMM_Register rop1 = instr->t1;
    X64_XMM_Register rop2 = instr->t2;
    X64_XMM_Register rdst = instr->t3;

    if(rop1 == rdst)
    {
        X64_XMM_Register temp = rop2;
        rop2 = rop1;
        rop1 = rop2;
    }

    // mov rop1 -> dst
    em->at = emit_movs_direct(0, em->at, rdst, rop1, instr->byte_size == 4);

    // adds rdst, rop2
    em->at = emit_arith_sse(&info, em->at, x86_ir_arithf_to_x86_arithf(instr), rdst, rop2, instr->byte_size == 4);

    return info;
}

Instr_Emit_Result
x86_emit_cmpf(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = { 0 };
    X64_XMM_Register rop1 = instr->t1;
    X64_XMM_Register rop2 = instr->t2;

    em->at = emit_comiss_sse(&info, em->at, rop1, rop2);

    return info;
}

// move rnbytes zeros to rdst + imm
Instr_Emit_Result
x86_emit_clear(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = { 0 };
    X64_Register rdst = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rnbyte = ir_to_x86_Reg(instr->t2, instr->byte_size);
    IR_Value imm = instr->imm;

    if(rnbyte != ECX)
    {
        // push ecx
        em->at = emit_push_reg(0, em->at, DIRECT, ECX, 0, 0);
        // mov ecx, imm
        em->at = emit_mov_reg(0, em->at, MOV_MR, DIRECT, instr->byte_size * 8, ECX, rnbyte, 0, 0);
    }

    // clear eax
    em->at = emit_arith_mr(0, ARITH_XOR, em->at, EAX, EAX, DIRECT, 0, 0);

    em->at = emit_lea(0, em->at, instr->byte_size * 8, INDIRECT_DWORD_DISPLACED, EDI, rdst, 0, instr->imm.v_s32);

    em->at = emit_rep_stos(&info, em->at, 8);

    if(rnbyte != ECX)
    {
        em->at = emit_pop_reg(0, em->at, ECX);
    }

    return info;
}

Instr_Emit_Result
x86_emit_copy(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = { 0 };
    X64_Register rsrc = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rdest = ir_to_x86_Reg(instr->t3, instr->byte_size);

    // mov esi, src
    em->at = emit_mov_reg(&info, em->at, MOV_MR, DIRECT, instr->byte_size * 8,
        ESI, rsrc, 0, 0);

    // mov edi, dst
    em->at = emit_mov_reg(0, em->at, MOV_MR, DIRECT, instr->byte_size * 8,
        EDI, rdest, 0, 0);

    // push ecx
    em->at = emit_push_reg(0, em->at, DIRECT, ECX, 0, 0);
    
    // mov ecx, imm
    em->at = emit_mov_oi(0, em->at, ECX, (Int_Value){.v32 = instr->imm.v_s32});
    
    em->at = emit_rep_movs(0, em->at, 8);

    em->at = emit_pop_reg(0, em->at, ECX);

    return info;
}

Instr_Emit_Result
x86_emit_instruction(X86_Emitter* em, IR_Instruction* instr, int index)
{
    instr->binary_offset = em->at;
    switch(instr->type)
    {
        case IR_LEA:  return  x86_emit_lea(em, instr);
        case IR_MOV:  return  x86_emit_mov(em, instr, index);
        case IR_LOAD: return  x86_emit_load(em, instr);
        case IR_STORE: return x86_emit_store(em, instr);
        case IR_ADD: case IR_SUB: case IR_OR: case IR_XOR: case IR_AND:
        case IR_LAND: case IR_LOR:
            return x86_emit_arith(em, instr);
        case IR_SHL: case IR_SHR:
            return x86_emit_shift(em, instr);
        case IR_MUL: case IR_DIV: case IR_MOD: case IR_IDIV: case IR_IMUL:
            return x86_emit_mul(em, instr);
        case IR_NOT: case IR_NEG:
            return x86_emit_not_neg(em, instr);
        case IR_LNOT:
            return x86_emit_lnot(em, instr);
        case IR_CMP:
            return x86_emit_cmp(em, instr);
        case IR_CMOVEQ: case IR_CMOVNE: case IR_CMOVGS: case IR_CMOVGES:
        case IR_CMOVLS: case IR_CMOVLES: case IR_CMOVGU: case IR_CMOVGEU:
        case IR_CMOVLU: case IR_CMOVLEU:
            return x86_emit_cmov(em, instr);
        case IR_CALL:
            return x86_emit_call(em, instr, index);
        case IR_RET:
            return x86_emit_ret(em, instr);
        case IR_HLT:
            return x86_emit_hlt(em, instr);
        case IR_PUSH:
            return x86_emit_push(em, instr);
        case IR_POP:
            return x86_emit_pop(em, instr);
        case IR_JRZ: case IR_JRNZ:
            return x86_emit_cond_rjmp(em, instr, index);
        case IR_JR:
            return x86_emit_rjmp(em, instr, index);

        // Conversions
        case IR_CVT_SI:
            return x86_emit_cvt_si(em, instr);
        case IR_CVT_UI:
            return x86_emit_cvt_ui(em, instr);
        case IR_CVT_SI_R32:
        case IR_CVT_SI_R64:
            return x86_emit_cvt_si_float(em, instr);
        case IR_CVT_R64_SI:
        case IR_CVT_R32_SI:
            return x86_emit_cvt_float_si(em, instr);
        case IR_CVT_R32_R64:
            return x86_emit_cvt_r32_r64(em, instr);
        case IR_CVT_R64_R32:
            return x86_emit_cvt_r64_r32(em, instr);

        case IR_CVT_R64_UI:
        case IR_CVT_UI_R64:
        case IR_CVT_R32_UI:
        case IR_CVT_UI_R32: 
            assert(0); // TODO(psv): hard stuff
            break;

        // Floating point instructions
        case IR_MOVF:
            return x86_emit_movf(em, instr);
        case IR_LOADF:
            return x86_emit_loadf(em, instr);
        case IR_STOREF:
            return x86_emit_storef(em, instr);
        case IR_ADDF: case IR_SUBF: case IR_MULF: case IR_DIVF:
            return x86_emit_arithf(em, instr);
        case IR_CMPF:
            return x86_emit_cmpf(em, instr);
    
        case IR_COPY:
            return x86_emit_copy(em, instr);
        case IR_CLEAR:
            return x86_emit_clear(em, instr);
        default: break;
    }
    return (Instr_Emit_Result){0};
}
#include "../../utils/os.h"
int
X86_generate(IR_Generator* gen)
{
    X86_Emitter em = {0};
    em.base = (u8*)calloc(1, 1024 * 1024 * 16);
    em.at = em.base;
    em.relative_patches = array_new(X86_Patch);
    em.data = array_new(X86_Data);
    em.imports = array_new(X86_Import);
    em.ir_gen = gen;
    int entry_point_offset = 0;

    IR_Activation_Rec* ar = 0;
    for(int i = 0; i < array_length(gen->instructions); ++i)
    {
        IR_Instruction* instr = gen->instructions + i;
        IR_Activation_Rec* current_ar = &gen->ars[instr->activation_record_index];
        if(ar != current_ar)
        {
            ar = current_ar;
        }

        if(gen->index_of_entry_point == i)
        {
            entry_point_offset = em.at - em.base;
        }
        x86_emit_instruction(&em, instr, i);
    }

    // Do instruction relative address patching
    for(int i = 0; i < array_length(em.relative_patches); ++i)
    {
        u8* issuer_addr = em.relative_patches[i].issuer_addr;
        bool gr = em.relative_patches[i].generate_relocation;
        // TODO(psv): Cleanup, both here should not require issuer index
        u8* target_addr = gen->instructions[((gr) ? 0 : em.relative_patches[i].issuer_index) + em.relative_patches[i].rel_index_offset].binary_offset;
        u8* patch_addr = em.relative_patches[i].addr;
        int diff = target_addr - issuer_addr - em.relative_patches[i].instr_byte_size;
        int bytes = em.relative_patches[i].bytes;
        int extra = em.relative_patches[i].extra_offset;
        if(bytes == 1)
            *(char*)patch_addr = (char)(diff);
        else if(bytes == 4)
            *(int*)patch_addr = diff + extra;
    }

#if defined(_WIN32) || defined(_WIN64)
    light_pecoff_emit(em.base, em.at - em.base, entry_point_offset, em.relative_patches, em.data, em.imports);
#else
    light_elf_emit(em.base, em.at - em.base, em.relative_patches);
#endif
    return 0;
}