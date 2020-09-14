#include "util.h"
// /digit only the r/m is used on ModRM byte, reg field contains extension for the instruction
// /r ModRM byte contains r/m and reg

// unsigned multiply AX := AL * r/m8
u8*
emit_mul(u8* stream, int bitsize, X64_Multiplicative_Instr instr, X64_Register reg, X64_Addressing_Mode mode, u8 disp8, uint32_t disp32)
{
    assert(bitsize == 8 || bitsize == 16 || bitsize == 32 || bitsize == 64);
    u8 opcode = (bitsize == 8) ? 0xf6 : 0xf7;
    stream = emit_opcode(stream, opcode, bitsize, RAX, register_representation(reg));

    *stream++ = make_modrm(mode, instr, register_representation(reg));
    stream = emit_displacement(mode, stream, disp8, disp32);

    return stream;
}

// Example: mul bl -> result in ax = al * bl
u8*
emit_mul_direct(u8* stream, int bitsize, X64_Multiplicative_Instr instr, X64_Register reg)
{
    return emit_mul(stream, bitsize, instr, reg, DIRECT, 0, 0);
}

// Example: mul byte[rdx] -> result in ax = al * byte[rdx]
u8*
emit_mul_indirect(u8* stream, int bitsize, X64_Multiplicative_Instr instr, X64_Register reg)
{
    assert(register_get_bitsize(reg) == ASSEMBLER_BITSIZE);
    return emit_mul(stream, bitsize, instr, reg, INDIRECT, 0, 0);
}

// Example: mul byte[rdx+0x12] -> result in ax = al * byte[rdx+0x12]
u8*
emit_mul_indirect_byte_displacement(u8* stream, int bitsize, X64_Multiplicative_Instr instr, X64_Register reg, u8 displacement)
{
    assert(register_get_bitsize(reg) == ASSEMBLER_BITSIZE);
    return emit_mul(stream, bitsize, instr, reg, INDIRECT_BYTE_DISPLACED, displacement, 0);
}

// Example: mul byte[rdx+0x12345678] -> result in ax = al * byte[rdx+0x12345678]
u8*
emit_mul_indirect_dword_displacement(u8* stream, int bitsize, X64_Multiplicative_Instr instr, X64_Register reg, uint32_t displacement)
{
    assert(register_get_bitsize(reg) == ASSEMBLER_BITSIZE);
    return emit_mul(stream, bitsize, instr, reg, INDIRECT_DWORD_DISPLACED, 0, displacement);
}

// *****************************
// DIV




u8*
emit_test_mul(u8* stream)
{
    X64_Multiplicative_Instr instr = MUL;
#if 1
    stream = emit_mul_direct(stream, 8, instr, BL);
    stream = emit_mul_indirect(stream, 8, instr, RDX);
    stream = emit_mul_indirect_byte_displacement(stream, 8, instr, RDX, 0x12);
    stream = emit_mul_indirect_dword_displacement(stream, 8, instr, RDX, 0x12345678);

    stream = emit_mul_direct(stream, 16, instr,  BL);
    stream = emit_mul_indirect(stream, 16, instr, RDX);
    stream = emit_mul_indirect_byte_displacement(stream, 16, instr,  RDX, 0x12);
    stream = emit_mul_indirect_dword_displacement(stream, 16, instr,  RDX, 0x12345678);

    stream = emit_mul_direct(stream, 32, instr, BL);
    stream = emit_mul_indirect(stream, 32, instr, RDX);
    stream = emit_mul_indirect_byte_displacement(stream, 32, instr, RDX, 0x12);
    stream = emit_mul_indirect_dword_displacement(stream, 32, instr, RDX, 0x12345678);

    stream = emit_mul_direct(stream, 64, instr, BL);
    stream = emit_mul_indirect(stream, 64, instr, RDX);
    stream = emit_mul_indirect_byte_displacement(stream, 64, instr, RDX, 0x12);
    stream = emit_mul_indirect_dword_displacement(stream, 64, instr, RDX, 0x12345678);
#endif

    return stream;
}