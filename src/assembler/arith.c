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

static u8*
emit_arith_mi(u8* stream, u8 opcode, int instr_digit, X64_Addressing_Mode mode, X64_Register dest, Int_Value value, u8 disp8, uint32_t disp32)
{
    int bitsize = register_get_bitsize(dest);
    stream = emit_opcode(stream, opcode, register_get_bitsize(dest), dest, dest);
    *stream++ = make_modrm(mode, instr_digit, register_representation(dest));

    if(mode == INDIRECT_BYTE_DISPLACED)
        *stream++ = disp8;
    else if(mode == INDIRECT_DWORD_DISPLACED)
    {
        *(uint32_t*)stream = disp32;
        stream += sizeof(uint32_t);
    }

    stream = emit_int_value(stream, MIN(32, register_get_bitsize(dest)), value);
    return stream;
}

// ADD MI
u8*
emit_arith_mi_direct(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, Int_Value value)
{
    return emit_arith_mi(stream, (register_get_bitsize(dest) == 8) ? ADD_IMM8 : ADD_IMM, instr, DIRECT, dest, value, 0, 0);
}

u8*
emit_arith_mi_indirect(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, Int_Value value)
{
    assert(register_get_bitsize(dest) == 64);
    return emit_arith_mi(stream, (register_get_bitsize(dest) == 8) ? ADD_IMM8 : ADD_IMM, instr, INDIRECT, dest, value, 0, 0);
}

u8*
emit_arith_mi_indirect_byte_displaced(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, Int_Value value, u8 displacement)
{
    assert(register_get_bitsize(dest) == 64);
    return emit_arith_mi(stream, (register_get_bitsize(dest) == 8) ? ADD_IMM8 : ADD_IMM, instr, INDIRECT_BYTE_DISPLACED, dest, value, displacement, 0);
}

u8*
emit_arith_mi_indirect_dword_displaced(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, Int_Value value, uint32_t displacement)
{
    assert(register_get_bitsize(dest) == 64);
    return emit_arith_mi(stream, (register_get_bitsize(dest) == 8) ? ADD_IMM8 : ADD_IMM, instr, INDIRECT_DWORD_DISPLACED, dest, value, 0, displacement);
}

// *******************************

static u8*
emit_arith_mi_imm(u8* stream, u8 opcode, int instr_digit, X64_Register dest, s8 value, X64_Addressing_Mode mode, u8 displ8, uint32_t displ32)
{
    int bitsize = register_get_bitsize(dest);
    assert(bitsize > 8 && "add mi signed does not support 8 bits reg destination");
    stream = emit_opcode(stream, ADDS_IMM8, register_get_bitsize(dest), dest, dest);
    *stream++ = make_modrm(mode, instr_digit, register_representation(dest));
    if(mode == INDIRECT_BYTE_DISPLACED)
        *stream++ = displ8;
    else if(mode == INDIRECT_DWORD_DISPLACED)
    {
        *(uint32_t*)stream = displ32;
        stream += sizeof(uint32_t);
    }

    *stream++ = value;
    return stream;
}

// ADD IMM8 SEXT
// Add immediate 8bit signed extended value to register
// Example: add rax, 0x12
u8*
emit_arith_mi_signed_ext(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, s8 value)
{
    return emit_arith_mi_imm(stream, ADDS_IMM8, instr, dest, value, DIRECT, 0, 0);
}

// Add immediate 8bit signed extended value to register indirect
// Example: add [rax], 0x12
u8*
emit_arith_mi_signed_ext_indirect(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, s8 value)
{
    return emit_arith_mi_imm(stream, ADDS_IMM8, instr, dest, value, INDIRECT, 0, 0);
}

// Example: add [rax+0x34], 0x12
u8*
emit_arith_mi_signed_ext_indirect_byte_displaced(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, s8 value, u8 displacement)
{
    return emit_arith_mi_imm(stream, ADDS_IMM8, instr, dest, value, INDIRECT_BYTE_DISPLACED, displacement, 0);
}

// Example: add [rax+0x12345678], 0x12
u8*
emit_arith_mi_signed_ext_indirect_dword_displaced(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, s8 value, uint32_t displacement)
{
    return emit_arith_mi_imm(stream, ADDS_IMM8, instr, dest, value, INDIRECT_DWORD_DISPLACED, 0, displacement);
}

// ****************************************
// ************** MR & RM *****************
// ****************************************

// Add register to register
static u8*
emit_arith_reg(u8* stream, u8 opcode, X64_Register dest, X64_Register src, X64_Addressing_Mode mode, u8 disp8, uint32_t disp32)
{
    int bitsize = register_get_bitsize(src);
    stream = emit_opcode(stream, opcode, bitsize, dest, src);
    *stream++ = make_modrm(mode, register_representation(src), register_representation(dest));

    if(mode != DIRECT && register_equivalent(dest, RSP))
        *stream++ = register_representation(dest);

    if(mode == INDIRECT_BYTE_DISPLACED)
        *stream++ = disp8;
    else if(mode == INDIRECT_DWORD_DISPLACED)
    {
        *(uint32_t*)stream = disp32;
        stream += sizeof(uint32_t);
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

static u8*
emit_arith_mr(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src, X64_Addressing_Mode mode, u8 disp8, uint32_t disp32)
{
    u8 opcode = mr_opcode(instr, register_get_bitsize(src));
    return emit_arith_reg(stream, opcode, dest, src, mode, disp8, disp32);
}

static u8*
emit_arith_rm(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src, X64_Addressing_Mode mode, u8 disp8, uint32_t disp32)
{
    u8 opcode = rm_opcode(instr, register_get_bitsize(dest));
    return emit_arith_reg(stream, opcode, dest, src, mode, disp8, disp32);
}

u8*
emit_arith_mr_direct(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src)
{
    assert(register_get_bitsize(dest) == register_get_bitsize(src));
    return emit_arith_mr(instr, stream, dest, src, DIRECT, 0, 0);
}

u8*
emit_arith_mr_indirect(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src)
{
    assert(register_get_bitsize(dest) == 64);
    return emit_arith_mr(instr, stream, dest, src, INDIRECT, 0, 0);
}

u8*
emit_arith_mr_indirect_byte_displaced(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src, u8 displacement)
{
    assert(register_get_bitsize(dest) == 64);
    return emit_arith_mr(instr, stream, dest, src, INDIRECT_BYTE_DISPLACED, displacement, 0);
}

u8*
emit_arith_mr_indirect_dword_displaced(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src, uint32_t displacement)
{
    assert(register_get_bitsize(dest) == 64);
    return emit_arith_mr(instr, stream, dest, src, INDIRECT_DWORD_DISPLACED, 0, displacement);
}

u8*
emit_arith_rm_direct(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src)
{
    assert(register_get_bitsize(src) == register_get_bitsize(dest));
    return emit_arith_rm(instr, stream, src, dest, DIRECT, 0, 0);
}

u8*
emit_arith_rm_indirect(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src)
{
    assert(register_get_bitsize(src) == 64);
    return emit_arith_rm(instr, stream, src, dest, INDIRECT, 0, 0);
}

u8*
emit_arith_rm_indirect_byte_displaced(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src, u8 value)
{
    assert(register_get_bitsize(src) == 64);
    return emit_arith_rm(instr, stream, src, dest, INDIRECT_BYTE_DISPLACED, value, 0);
}

u8*
emit_arith_rm_indirect_dword_displaced(X64_Arithmetic_Instr instr, u8* stream, X64_Register dest, X64_Register src, uint32_t value)
{
    assert(register_get_bitsize(src) == 64);
    return emit_arith_rm(instr, stream, src, dest, INDIRECT_DWORD_DISPLACED, 0, value);
}

u8*
emit_add_test(u8* stream)
{
    X64_Arithmetic_Instr instr = ARITH_ADD;
#if 0
    stream = emit_arith_rm_direct(instr, stream, RAX, RBX);
    stream = emit_arith_rm_direct(instr, stream, EAX, EBX);
    stream = emit_arith_rm_direct(instr, stream, AX, BX);
    stream = emit_arith_rm_direct(instr, stream, AL, BL);

    stream = emit_arith_rm_direct(instr, stream, R8, RBX);
    stream = emit_arith_rm_direct(instr, stream, R8D, EBX);
    stream = emit_arith_rm_direct(instr, stream, R8W, BX);
    stream = emit_arith_rm_direct(instr, stream, R8B, BL);

    stream = emit_arith_rm_direct(instr, stream, RSP, RBX);
    stream = emit_arith_rm_direct(instr, stream, ESP, EBX);
    stream = emit_arith_rm_direct(instr, stream, SP, BX);
    stream = emit_arith_rm_direct(instr, stream, SPL, BL);

    stream = emit_arith_rm_indirect(instr, stream, RBX, RAX);
    stream = emit_arith_rm_indirect(instr, stream, EBX, RAX);
    stream = emit_arith_rm_indirect(instr, stream, BX, RAX);
    stream = emit_arith_rm_indirect(instr, stream, BL, RAX);

    stream = emit_arith_rm_indirect(instr, stream, RBX, R8);
    stream = emit_arith_rm_indirect(instr, stream, EBX, R8);
    stream = emit_arith_rm_indirect(instr, stream, BX, R8);
    stream = emit_arith_rm_indirect(instr, stream, BL, R8);

    stream = emit_arith_rm_indirect(instr, stream, RBX, RSP);
    stream = emit_arith_rm_indirect(instr, stream, EBX, RSP);
    stream = emit_arith_rm_indirect(instr, stream, BX, RSP);
    stream = emit_arith_rm_indirect(instr, stream, BL, RSP);

    stream = emit_arith_rm_indirect_byte_displaced(instr, stream, RAX, RBX, 0x12);
    stream = emit_arith_rm_indirect_byte_displaced(instr, stream, EAX, RBX, 0x12);
    stream = emit_arith_rm_indirect_byte_displaced(instr, stream, AX, RBX, 0x12);
    stream = emit_arith_rm_indirect_byte_displaced(instr, stream, AL, RBX, 0x12);

    stream = emit_arith_rm_indirect_dword_displaced(instr, stream, R8, RBX, 0x12345678);
    stream = emit_arith_rm_indirect_dword_displaced(instr, stream, R8D, RBX, 0x12345678);
    stream = emit_arith_rm_indirect_dword_displaced(instr, stream, R8W, RBX, 0x12345678);
    stream = emit_arith_rm_indirect_dword_displaced(instr, stream, R8B, RBX, 0x12345678);
#endif
#if 0
    stream = emit_arith_mr_direct(instr, stream, RAX, RBX);
    stream = emit_arith_mr_direct(instr, stream, EAX, EBX);
    stream = emit_arith_mr_direct(instr, stream, AX, BX);
    stream = emit_arith_mr_direct(instr, stream, AL, BL);

    stream = emit_arith_mr_direct(instr, stream, R8, RBX);
    stream = emit_arith_mr_direct(instr, stream, R8D, EBX);
    stream = emit_arith_mr_direct(instr, stream, R8W, BX);
    stream = emit_arith_mr_direct(instr, stream, R8B, BL);

    stream = emit_arith_mr_direct(instr, stream, RSP, RBX);
    stream = emit_arith_mr_direct(instr, stream, ESP, EBX);
    stream = emit_arith_mr_direct(instr, stream, SP, BX);
    stream = emit_arith_mr_direct(instr, stream, SPL, BL);
    
    stream = emit_arith_mr_indirect(instr, stream, RAX, RBX);
    stream = emit_arith_mr_indirect(instr, stream, RAX, EBX);
    stream = emit_arith_mr_indirect(instr, stream, RAX, BX);
    stream = emit_arith_mr_indirect(instr, stream, RAX, BL);

    stream = emit_arith_mr_indirect(instr, stream, R8, RBX);
    stream = emit_arith_mr_indirect(instr, stream, R8, EBX);
    stream = emit_arith_mr_indirect(instr, stream, R8, BX);
    stream = emit_arith_mr_indirect(instr, stream, R8, BL);

    stream = emit_arith_mr_indirect(instr, stream, RSP, RBX);
    stream = emit_arith_mr_indirect(instr, stream, RSP, EBX);
    stream = emit_arith_mr_indirect(instr, stream, RSP, BX);
    stream = emit_arith_mr_indirect(instr, stream, RSP, BL);

    stream = emit_arith_mr_indirect(instr, stream, RSP, R8);
    stream = emit_arith_mr_indirect(instr, stream, RSP, R8D);
    stream = emit_arith_mr_indirect(instr, stream, RSP, R8W);
    stream = emit_arith_mr_indirect(instr, stream, RSP, R8B);
#endif
#if 0
    stream = emit_arith_mi_direct(instr, stream, RAX, (Int_Value){.v64 = 0x12345678});
    stream = emit_arith_mi_direct(instr, stream, RBX, (Int_Value){.v64 = 0x12345678});
    stream = emit_arith_mi_direct(instr, stream, EAX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_direct(instr, stream, EBX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_direct(instr, stream, BX, (Int_Value){.v16 = 0x123});
    stream = emit_arith_mi_direct(instr, stream, AX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_direct(instr, stream, AL, (Int_Value){.v64 = 0x12});

    stream = emit_arith_mi_indirect(instr, stream, RAX, (Int_Value){.v64 = 0x12345678});
    stream = emit_arith_mi_indirect(instr, stream, R8, (Int_Value){.v64 = 0x12345678});
    stream = emit_arith_mi_indirect(instr, stream, RAX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_indirect(instr, stream, R8, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_indirect(instr, stream, RBX, (Int_Value){.v16 = 0x123});
    stream = emit_arith_mi_indirect(instr, stream, RAX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_indirect(instr, stream, RAX, (Int_Value){.v64 = 0x12});
    stream = emit_arith_mi_indirect(instr, stream, R9, (Int_Value){.v64 = 0x12});

    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x12345678}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, R8, (Int_Value){.v64 = 0x12345678}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x123}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, R8, (Int_Value){.v64 = 0x123}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RBX, (Int_Value){.v16 = 0x123}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x123}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x12}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, R9, (Int_Value){.v64 = 0x12}, 0x13);

    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x12345678}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, R8, (Int_Value){.v64 = 0x12345678}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x123}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, R8, (Int_Value){.v64 = 0x123}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RBX, (Int_Value){.v16 = 0x123}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x123}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x12}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, R9, (Int_Value){.v64 = 0x12}, 0x12345678);
#endif
#if 1
    stream = emit_arith_mi_signed_ext(instr, stream, RBX, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, EBX, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, BX, 0x12);

    stream = emit_arith_mi_signed_ext(instr, stream, RSP, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, RBP, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, SI, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, DI, 0x12);

    stream = emit_arith_mi_signed_ext_indirect(instr, stream, RBX, 0x12);
    stream = emit_arith_mi_signed_ext_indirect(instr, stream, EBX, 0x12);
    stream = emit_arith_mi_signed_ext_indirect(instr, stream, BX, 0x12);

    stream = emit_arith_mi_signed_ext_indirect_byte_displaced(instr, stream, RBX, 0x12, 0x45);
    stream = emit_arith_mi_signed_ext_indirect_byte_displaced(instr, stream, EBX, 0x12, 0x45);
    stream = emit_arith_mi_signed_ext_indirect_byte_displaced(instr, stream, BX, 0x12, 0x45);

    stream = emit_arith_mi_signed_ext_indirect_dword_displaced(instr, stream, RBX, 0x12, 0x12345678);
    stream = emit_arith_mi_signed_ext_indirect_dword_displaced(instr, stream, EBX, 0x12, 0x12345678);
    stream = emit_arith_mi_signed_ext_indirect_dword_displaced(instr, stream, BX, 0x12, 0x12345678);
#endif
    return stream;
}

u8*
emit_and_test(u8* stream)
{
    X64_Arithmetic_Instr instr = ARITH_CMP;
#if 1
    stream = emit_arith_mi_direct(instr, stream, RAX, (Int_Value){.v64 = 0x12345678});
    stream = emit_arith_mi_direct(instr, stream, RBX, (Int_Value){.v64 = 0x12345678});
    stream = emit_arith_mi_direct(instr, stream, EAX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_direct(instr, stream, EBX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_direct(instr, stream, BX, (Int_Value){.v16 = 0x123});
    stream = emit_arith_mi_direct(instr, stream, AX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_direct(instr, stream, AL, (Int_Value){.v64 = 0x12});

    stream = emit_arith_mi_indirect(instr, stream, RAX, (Int_Value){.v64 = 0x12345678});
    stream = emit_arith_mi_indirect(instr, stream, R8, (Int_Value){.v64 = 0x12345678});
    stream = emit_arith_mi_indirect(instr, stream, RAX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_indirect(instr, stream, R8, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_indirect(instr, stream, RBX, (Int_Value){.v16 = 0x123});
    stream = emit_arith_mi_indirect(instr, stream, RAX, (Int_Value){.v64 = 0x123});
    stream = emit_arith_mi_indirect(instr, stream, RAX, (Int_Value){.v64 = 0x12});
    stream = emit_arith_mi_indirect(instr, stream, R9, (Int_Value){.v64 = 0x12});

    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x12345678}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, R8, (Int_Value){.v64 = 0x12345678}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x123}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, R8, (Int_Value){.v64 = 0x123}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RBX, (Int_Value){.v16 = 0x123}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x123}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x12}, 0x13);
    stream = emit_arith_mi_indirect_byte_displaced(instr, stream, R9, (Int_Value){.v64 = 0x12}, 0x13);

    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x12345678}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, R8, (Int_Value){.v64 = 0x12345678}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x123}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, R8, (Int_Value){.v64 = 0x123}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RBX, (Int_Value){.v16 = 0x123}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x123}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, RAX, (Int_Value){.v64 = 0x12}, 0x12345678);
    stream = emit_arith_mi_indirect_dword_displaced(instr, stream, R9, (Int_Value){.v64 = 0x12}, 0x12345678);
#endif
#if 1
    stream = emit_arith_mi_signed_ext(instr, stream, RBX, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, EBX, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, BX, 0x12);

    stream = emit_arith_mi_signed_ext(instr, stream, RSP, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, RBP, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, SI, 0x12);
    stream = emit_arith_mi_signed_ext(instr, stream, DI, 0x12);

    stream = emit_arith_mi_signed_ext_indirect(instr, stream, RBX, 0x12);
    stream = emit_arith_mi_signed_ext_indirect(instr, stream, EBX, 0x12);
    stream = emit_arith_mi_signed_ext_indirect(instr, stream, BX, 0x12);

    stream = emit_arith_mi_signed_ext_indirect_byte_displaced(instr, stream, RBX, 0x12, 0x45);
    stream = emit_arith_mi_signed_ext_indirect_byte_displaced(instr, stream, EBX, 0x12, 0x45);
    stream = emit_arith_mi_signed_ext_indirect_byte_displaced(instr, stream, BX, 0x12, 0x45);

    stream = emit_arith_mi_signed_ext_indirect_dword_displaced(instr, stream, RBX, 0x12, 0x12345678);
    stream = emit_arith_mi_signed_ext_indirect_dword_displaced(instr, stream, EBX, 0x12, 0x12345678);
    stream = emit_arith_mi_signed_ext_indirect_dword_displaced(instr, stream, BX, 0x12, 0x12345678);
#endif
    return stream;
}