#include "x86.h"
#include "../../assembler/assembler.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <light_array.h>

typedef struct {
    u8* issuer_addr;    // address of the instruction to calculate the relative offset
    u8* addr;           // address to be patched
    int instr_byte_size;
    int bytes;          // number of bytes to be replaced
    int issuer_index;       // index
    int rel_index_offset;   // relative offset from the instruction that issued to the target
} X86_Patch;

typedef struct {
    u8* base;
    u8* at;

    X86_Patch* relative_patches;
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
    // 8 16 32
    X64_Register n[][4] = {
        {BL,   BX,   REG_NONE, EBX},
        {CL,   CX,   REG_NONE, ECX},
        {DL,   DX,   REG_NONE, EDX},
        {SIL,  SI,   REG_NONE, ESI},
        {DIL,  DI,   REG_NONE, EDI},
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
x86_emit_mov(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);
    X64_Register rsrc = ir_to_x86_Reg(instr->t1, instr->byte_size);

    if(instr->imm.type != IR_VALUE_NONE)
    {
        // imm to reg
        em->at = emit_mov_oi(&info, em->at, rdst, (Int_Value){.v64 = instr->imm.v_u64});
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
    X64_Register rdst = ir_to_x86_Reg(instr->t2, instr->byte_size);
    X64_Register rsrc = ir_to_x86_Reg(instr->t1, instr->byte_size);

    X64_Addressing_Mode mode = INDIRECT;
    if(instr->imm.type == IR_VALUE_S8) mode = INDIRECT_BYTE_DISPLACED;
    else if(instr->imm.type == IR_VALUE_S32) mode = INDIRECT_DWORD_DISPLACED;

    em->at = emit_mov_reg(&info, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, mode, 
        instr->byte_size * 8, rsrc, rdst,
        (instr->imm.type == IR_VALUE_S8) ? instr->imm.v_s8 : 0,
        (instr->imm.type == IR_VALUE_S32) ? instr->imm.v_s32 : 0);
    return info;
}

Instr_Emit_Result
x86_emit_load(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);
    X64_Register rsrc = ir_to_x86_Reg(instr->t1, instr->byte_size);

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
        case IR_ADD: return ARITH_ADD;
        case IR_SUB: return ARITH_SUB;
        case IR_OR: return ARITH_OR;
        case IR_AND: return ARITH_AND;
        case IR_XOR: return ARITH_XOR;
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
        //case IR_IMUL: return IMUL;
        case IR_DIV:  return DIV;
        //case IR_IDIV: return IDIV;
        default: assert(0); break;
    }
    return -1;
}

Instr_Emit_Result
x86_emit_mul(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    X64_Register rop1 = ir_to_x86_Reg(instr->t1, instr->byte_size);
    X64_Register rop2 = ir_to_x86_Reg(instr->t2, instr->byte_size);
    X64_Register rdst = ir_to_x86_Reg(instr->t3, instr->byte_size);

    if(rop2 == rdst)
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
    }

    // mov to eax
    em->at = emit_mov_reg(0, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, 
        instr->byte_size * 8, x86_reg32_to_byte_size(EAX, instr->byte_size), rop1, 0, 0);

    em->at = emit_mul(&info, em->at, instr->byte_size * 8, x86_ir_mul_to_x86_arith(instr), rop2, DIRECT, 0, 0);

    // move result that is in edx to the destination register
    if(not_edx)
    {
        em->at = emit_mov_reg(0, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, 
            instr->byte_size * 8, rdst, x86_reg32_to_byte_size(EDX, instr->byte_size), 0, 0);
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

    // mov dst, op1
    // add dst, op2

    if(rop2 == rdst)
    {
        X64_Register temp = rop1;
        rop1 = rop2;
        rop2 = temp;
    }

    if(rop1 != rdst)
    {
        // move op1 to dst
        em->at = emit_mov_reg(0, em->at, (instr->byte_size == 1) ? MOV_MR8 : MOV_MR, DIRECT, 
            instr->byte_size * 8, rdst, rop1, 0, 0);
    }

    if(instr->imm.type != IR_VALUE_NONE)
    {
        // add rdst, imm -> reg
        em->at = emit_arith_mi(&info, em->at, x86_ir_arith_to_x86_arith(instr), DIRECT, rdst, (Int_Value){.v64 = instr->imm.v_u64}, 0, 0);
    }
    else
    {
        // add rdst, reg -> reg
        em->at = emit_arith_mr(&info, x86_ir_arith_to_x86_arith(instr), em->at, rdst, rop2, DIRECT, 0, 0);
    }
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
x86_emit_ret(X86_Emitter* em, IR_Instruction* instr)
{
    Instr_Emit_Result info = {0};
    
    em->at = emit_ret(&info, em->at, RET_NEAR);     // TODO(psv): ret far?

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
x86_emit_rjmp(X86_Emitter* em, IR_Instruction* instr, int index)
{
    Instr_Emit_Result info = {0};

    u8* jmp_addr = em->at;
    em->at = emit_jmp_rel_unconditional(&info, em->at, instr->byte_size * 8, (Int_Value){0});

    X86_Patch patch = {0};
    patch.issuer_addr = jmp_addr;
    patch.addr = jmp_addr + info.immediate_offset;
    patch.bytes = 1;
    patch.rel_index_offset = instr->imm.v_s32;
    patch.instr_byte_size = info.instr_byte_size;
    patch.issuer_index = index;
    array_push(em->relative_patches, patch);

    return info;
}

Instr_Emit_Result
x86_emit_instruction(X86_Emitter* em, IR_Instruction* instr, int index)
{
    instr->binary_offset = em->at;
    switch(instr->type)
    {
        case IR_LEA:  return  x86_emit_lea(em, instr);
        case IR_MOV:  return  x86_emit_mov(em, instr);
        case IR_LOAD: return  x86_emit_load(em, instr);
        case IR_STORE: return x86_emit_store(em, instr);
        case IR_ADD: case IR_SUB: case IR_OR: case IR_XOR: case IR_AND:
            return x86_emit_arith(em, instr);
        case IR_MUL: case IR_DIV:
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
        case IR_RET:
            return x86_emit_ret(em, instr);
        case IR_JRZ: case IR_JRNZ:
            return x86_emit_cond_rjmp(em, instr, index);
        case IR_JR:
            return x86_emit_rjmp(em, instr, index);
        
    }
    return (Instr_Emit_Result){0};
}

int
X86_generate(IR_Generator* gen)
{
    X86_Emitter em = {0};
    em.base = (u8*)calloc(1, 1024 * 1024);
    em.at = em.base;
    em.relative_patches = array_new(X86_Patch);

    IR_Activation_Rec* ar = 0;
    for(int i = 0, j = 0; i < array_length(gen->instructions); ++i)
    {
        IR_Instruction* instr = gen->instructions + i;
        IR_Activation_Rec* current_ar = &gen->ars[instr->activation_record_index];
        if(ar != current_ar)
        {
            ar = current_ar;
            j = 0;
        }
        if(array_length(ar->insertions) > 0 && ar->insertions[j].index == i)
        {
            x86_emit_instruction(&em, &ar->insertions[j].inst, -1);
            j++;
        }

        x86_emit_instruction(&em, instr, i);
    }

    for(int i = 0; i < array_length(em.relative_patches); ++i)
    {
        u8* issuer_addr = em.relative_patches[i].issuer_addr;
        u8* target_addr = gen->instructions[em.relative_patches[i].issuer_index + em.relative_patches[i].rel_index_offset].binary_offset;
        u8* patch_addr = em.relative_patches[i].addr;
        int diff = target_addr - issuer_addr - em.relative_patches[i].instr_byte_size;
        *(char*)patch_addr = (char)diff;
    }

    FILE* out = fopen("test.bin", "wb");
    if(!out)
    {
        fprintf(stderr, "could not open file test.bin for writing\n");
        return -1;
    }
    fwrite(em.base, em.at - em.base, 1, out);
    fclose(out);

    return 0;
}