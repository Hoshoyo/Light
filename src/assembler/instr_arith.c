#include "util.h"

#define AND_IMM8 0x80
#define AND_IMM 0x81
#define ADD_IMM8 0x80
#define ADD_IMM 0x81
#define ADDS_IMM8 0x83
#define ADD_MR 0x01
#define ADD_RM 0x03

#define ADD_MR8 0x00
#define CMP_MR8 0x38
#define AND_MR8 0x20
#define ADC_MR8 0x10
#define SBB_MR8 0x18
#define SUB_MR8 0x28
#define XOR_MR8 0x30
#define OR_MR8 0x08

#define ADD_RM8 0x02
#define CMP_RM8 0x3A
#define AND_RM8 0x22
#define ADC_RM8 0x12
#define SBB_RM8 0x1A
#define SUB_RM8 0x2A
#define XOR_RM8 0x32
#define OR_RM8 0x0A

u8*
emit_arith_mi(Instr_Emit_Result* out_info, u8* stream, int instr_digit, X64_Addressing_Mode mode, X64_Register dest, Int_Value value, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 imm_offset = 0;
    s8 disp_offset = 0;

    u8 opcode = (register_get_bitsize(dest) == 8) ? ADD_IMM8 : ADD_IMM;
    int bitsize = register_get_bitsize(dest);
    stream = emit_opcode(stream, opcode, register_get_bitsize(dest), dest, dest);
    *stream++ = make_modrm(mode, instr_digit, register_representation(dest));

    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);
    if((stream - start) == disp_offset) disp_offset = -1;

    imm_offset = stream - start;
    stream = emit_int_value(stream, MIN(32, register_get_bitsize(dest)), value);
    if((stream - start) == imm_offset) imm_offset = -1;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = disp_offset;
        out_info->immediate_offset = imm_offset;
    }

    return stream;
}

// *******************************

// ADD IMM8 SEXT
// Add immediate 8bit signed extended value to register
// Example: add rax, 0x12
u8*
emit_arith_mi_imm8_sext(Instr_Emit_Result* out_info, u8* stream, int instr_digit, X64_Register dest, s8 value, X64_Addressing_Mode mode, u8 displ8, uint32_t displ32)
{
    u8* start = stream;
    s8 imm_offset = 0;
    s8 disp_offset = 0;

    int bitsize = register_get_bitsize(dest);
    assert(bitsize > 8 && "add mi signed does not support 8 bits reg destination");
    stream = emit_opcode(stream, ADDS_IMM8, register_get_bitsize(dest), dest, dest);
    *stream++ = make_modrm(mode, instr_digit, register_representation(dest));

    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, displ8, displ32);
    if((stream - start) == disp_offset) disp_offset = -1;

    imm_offset = stream - start;
    *stream++ = value;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = imm_offset;
        out_info->diplacement_offset = disp_offset;
    }
    return stream;
}

// ****************************************
// ************** MR & RM *****************
// ****************************************

// Add register to register
static u8*
emit_arith_reg(Instr_Emit_Result* out_info, u8* stream, u8 opcode, X64_Register dest, X64_Register src, X64_Addressing_Mode mode, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

    int bitsize = register_get_bitsize(src);
    stream = emit_opcode(stream, opcode, bitsize, dest, src);
    *stream++ = make_modrm(mode, register_representation(src), register_representation(dest));

    if(mode != DIRECT && register_equivalent(dest, RSP))
        *stream++ = register_representation(dest);

    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);
    if((stream - start) == disp_offset) disp_offset = -1;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1;
        out_info->diplacement_offset = disp_offset;
    }

    return stream;
}

static u8
mr_opcode(X64_Arithmetic_Instr instr, int bitsize)
{
    u8 opcode = 0;
    switch(instr)
    {
        case ARITH_AND: opcode = AND_MR8; break;
        case ARITH_OR:  opcode = OR_MR8; break;
        case ARITH_XOR: opcode = XOR_MR8; break;
        case ARITH_ADD: opcode = ADD_MR8; break;
        case ARITH_ADC: opcode = ADC_MR8; break;
        case ARITH_SUB: opcode = SUB_MR8; break;
        case ARITH_SBB: opcode = SBB_MR8; break;
        case ARITH_CMP: opcode = CMP_MR8; break;
        default: assert(0 && "invalid instruction"); break;
    }
    if(bitsize > 8) opcode += 1;
    return opcode;
}

static u8
rm_opcode(X64_Arithmetic_Instr instr, int bitsize)
{
    u8 opcode = 0;
    switch(instr)
    {
        case ARITH_AND: opcode = AND_RM8; break;
        case ARITH_OR:  opcode = OR_RM8; break;
        case ARITH_XOR: opcode = XOR_RM8; break;
        case ARITH_ADD: opcode = ADD_RM8; break;
        case ARITH_ADC: opcode = ADC_RM8; break;
        case ARITH_SUB: opcode = SUB_RM8; break;
        case ARITH_SBB: opcode = SBB_RM8; break;
        case ARITH_CMP: opcode = CMP_RM8; break;
        default: assert(0 && "invalid instruction"); break;
    }
    if(bitsize > 8) opcode += 1;
    return opcode;
}

u8*
emit_arith_mr(Instr_Emit_Result* out_info, X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src, X64_Addressing_Mode mode, u8 disp8, uint32_t disp32)
{
    u8 opcode = mr_opcode(instr, register_get_bitsize(src));
    return emit_arith_reg(out_info, stream, opcode, dest, src, mode, disp8, disp32);
}

u8*
emit_arith_rm(Instr_Emit_Result* out_info, X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src, X64_Addressing_Mode mode, u8 disp8, uint32_t disp32)
{
    u8 opcode = rm_opcode(instr, register_get_bitsize(dest));
    return emit_arith_reg(out_info, stream, opcode, dest, src, mode, disp8, disp32);
}

u8*
emit_and_test(u8* stream)
{
    X64_Arithmetic_Instr instr = ARITH_CMP;
#if 1
    stream = emit_arith_mi(0, stream, instr, DIRECT, RAX, (Int_Value){.v64 = 0x12345678}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, DIRECT, RBX, (Int_Value){.v64 = 0x12345678}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, DIRECT, EAX, (Int_Value){.v64 = 0x123}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, DIRECT, EBX, (Int_Value){.v64 = 0x123}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, DIRECT, BX, (Int_Value){.v16 = 0x123}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, DIRECT, AX, (Int_Value){.v64 = 0x123}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, DIRECT, AL, (Int_Value){.v64 = 0x12}, 0, 0);

    stream = emit_arith_mi(0, stream, instr, INDIRECT, RAX, (Int_Value){.v64 = 0x12345678}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT, R8, (Int_Value){.v64 = 0x12345678}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT, RAX, (Int_Value){.v64 = 0x123}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT, R8, (Int_Value){.v64 = 0x123}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT, RBX, (Int_Value){.v16 = 0x123}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT, RAX, (Int_Value){.v64 = 0x123}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT, RAX, (Int_Value){.v64 = 0x12}, 0, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT, R9, (Int_Value){.v64 = 0x12}, 0, 0);

    stream = emit_arith_mi(0, stream, instr, INDIRECT_BYTE_DISPLACED, RAX, (Int_Value){.v64 = 0x12345678}, 0x13, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_BYTE_DISPLACED, R8, (Int_Value){.v64 = 0x12345678}, 0x13, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_BYTE_DISPLACED, RAX, (Int_Value){.v64 = 0x123}, 0x13, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_BYTE_DISPLACED, R8, (Int_Value){.v64 = 0x123}, 0x13, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_BYTE_DISPLACED, RBX, (Int_Value){.v16 = 0x123}, 0x13, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_BYTE_DISPLACED, RAX, (Int_Value){.v64 = 0x123}, 0x13, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_BYTE_DISPLACED, RAX, (Int_Value){.v64 = 0x12}, 0x13, 0);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_BYTE_DISPLACED, R9, (Int_Value){.v64 = 0x12}, 0x13, 0);

    stream = emit_arith_mi(0, stream, instr, INDIRECT_DWORD_DISPLACED, RAX, (Int_Value){.v64 = 0x12345678}, 0, 0x12345678);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_DWORD_DISPLACED, R8, (Int_Value){.v64 = 0x12345678}, 0, 0x12345678);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_DWORD_DISPLACED, RAX, (Int_Value){.v64 = 0x123}, 0, 0x12345678);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_DWORD_DISPLACED, R8, (Int_Value){.v64 = 0x123}, 0, 0x12345678);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_DWORD_DISPLACED, RBX, (Int_Value){.v16 = 0x123}, 0, 0x12345678);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_DWORD_DISPLACED, RAX, (Int_Value){.v64 = 0x123}, 0, 0x12345678);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_DWORD_DISPLACED, RAX, (Int_Value){.v64 = 0x12}, 0, 0x12345678);
    stream = emit_arith_mi(0, stream, instr, INDIRECT_DWORD_DISPLACED, R9, (Int_Value){.v64 = 0x12}, 0, 0x12345678);
#endif
#if 1
    stream = emit_arith_mi_imm8_sext(0, stream, instr, RBX, 0x12, DIRECT, 0, 0);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, EBX, 0x12, DIRECT, 0, 0);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, BX, 0x12, DIRECT, 0, 0);

    stream = emit_arith_mi_imm8_sext(0, stream, instr, RSP, 0x12, DIRECT, 0, 0);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, RBP, 0x12, DIRECT, 0, 0);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, SI, 0x12, DIRECT, 0, 0);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, DI, 0x12, DIRECT, 0, 0);

    stream = emit_arith_mi_imm8_sext(0, stream, instr, RBX, 0x12, INDIRECT, 0, 0);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, EBX, 0x12, INDIRECT, 0, 0);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, BX, 0x12, INDIRECT, 0, 0);

    stream = emit_arith_mi_imm8_sext(0, stream, instr, RBX, 0x12, INDIRECT_BYTE_DISPLACED, 0x45, 0);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, EBX, 0x12, INDIRECT_BYTE_DISPLACED, 0x45, 0);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, BX, 0x12, INDIRECT_BYTE_DISPLACED, 0x45, 0);

    stream = emit_arith_mi_imm8_sext(0, stream, instr, RBX, 0x12, INDIRECT_DWORD_DISPLACED, 0, 0x12345678);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, EBX, 0x12, INDIRECT_DWORD_DISPLACED, 0, 0x12345678);
    stream = emit_arith_mi_imm8_sext(0, stream, instr, BX, 0x12, INDIRECT_DWORD_DISPLACED, 0, 0x12345678);
#endif
    return stream;
}