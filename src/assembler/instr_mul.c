#include "util.h"
// /digit only the r/m is used on ModRM byte, reg field contains extension for the instruction
// /r ModRM byte contains r/m and reg

// unsigned multiply AX := AL * r/m8
u8*
emit_mul(Instr_Emit_Result* out_info, u8* stream, int bitsize, X64_Multiplicative_Instr instr, X64_Register reg, X64_Addressing_Mode mode, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

    assert(bitsize == 8 || bitsize == 16 || bitsize == 32 || bitsize == 64);
    u8 opcode = (bitsize == 8) ? 0xf6 : 0xf7;
    stream = emit_opcode(stream, opcode, bitsize, RAX, register_representation(reg));
    *stream++ = make_modrm(mode, instr, register_representation(reg));

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

u8*
emit_test_mul(u8* stream)
{
    X64_Multiplicative_Instr instr = MUL;
#if 1
    stream = emit_mul(0, stream, 8, instr, BL, DIRECT, 0, 0);
    stream = emit_mul(0, stream, 8, instr, RDX, INDIRECT, 0, 0);
    stream = emit_mul(0, stream, 8, instr, RDX, INDIRECT_BYTE_DISPLACED, 0x12, 0);
    stream = emit_mul(0, stream, 8, instr, RDX, INDIRECT_DWORD_DISPLACED, 0, 0x12345678);

    stream = emit_mul(0, stream, 16, instr, BL, DIRECT, 0, 0);
    stream = emit_mul(0, stream, 16, instr, RDX, INDIRECT, 0, 0);
    stream = emit_mul(0, stream, 16, instr, RDX, INDIRECT_BYTE_DISPLACED, 0x12, 0);
    stream = emit_mul(0, stream, 16, instr, RDX, INDIRECT_DWORD_DISPLACED, 0, 0x12345678);

    stream = emit_mul(0, stream, 32, instr, BL, DIRECT, 0, 0);
    stream = emit_mul(0, stream, 32, instr, RDX, INDIRECT, 0, 0);
    stream = emit_mul(0, stream, 32, instr, RDX, INDIRECT_BYTE_DISPLACED, 0x12, 0);
    stream = emit_mul(0, stream, 32, instr, RDX, INDIRECT_DWORD_DISPLACED, 0, 0x12345678);

    stream = emit_mul(0, stream, 64, instr, BL, DIRECT, 0, 0);
    stream = emit_mul(0, stream, 64, instr, RDX, INDIRECT, 0, 0);
    stream = emit_mul(0, stream, 64, instr, RDX, INDIRECT_BYTE_DISPLACED, 0x12, 0);
    stream = emit_mul(0, stream, 64, instr, RDX, INDIRECT_DWORD_DISPLACED, 0, 0x12345678);
#endif

    return stream;
}