#include "util.h"

u8*
emit_shift_imm8(Instr_Emit_Result* out_info, u8* stream, int bitsize, X64_Shift_Instruction instr ,X64_Addressing_Mode mode, X64_Register reg, u8 imm, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 imm_offset = 0;
    s8 disp_offset = 0;

    u8 opcode = (bitsize == 8) ? 0xC0 : 0xC1;
    stream = emit_opcode(stream, opcode, register_get_bitsize(reg), register_representation(reg), 0);
    *stream++ = make_modrm(mode, instr, register_representation(reg));
    
    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);
    if((stream - start) == disp_offset) disp_offset = -1;
    
    imm_offset = stream - start;
    *stream++ = imm;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = disp_offset;
        out_info->immediate_offset = imm_offset;
    }

    return stream;
}

// cl is implicitly used as the parameter (shift count)
u8*
emit_shift_reg(Instr_Emit_Result* out_info, u8* stream, int bitsize, X64_Shift_Instruction instr, X64_Addressing_Mode mode, X64_Register reg, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

    u8 opcode = (bitsize == 8) ? 0xD2 : 0xD3;
    stream = emit_opcode(stream, opcode, register_get_bitsize(reg), RAX, 0);
    *stream++ = make_modrm(mode, instr, register_representation(reg));

    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);
    if((stream - start) == disp_offset) disp_offset = -1;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = disp_offset;
        out_info->immediate_offset = -1;
    }

    return stream;
}

u8*
emit_shl_test(u8* stream)
{
    X64_Shift_Instruction instr = 7;
    stream = emit_shift_reg(0, stream, 8,  instr, DIRECT, BL, 0, 0);
    stream = emit_shift_reg(0, stream, 16, instr, DIRECT, BX, 0, 0);
    stream = emit_shift_reg(0, stream, 32, instr, DIRECT, EBX, 0, 0);
    stream = emit_shift_reg(0, stream, 64, instr, DIRECT, RBX, 0, 0);

    stream = emit_shift_reg(0, stream, 8,  instr, INDIRECT, BL, 0, 0);
    stream = emit_shift_reg(0, stream, 16, instr, INDIRECT, BX, 0, 0);
    stream = emit_shift_reg(0, stream, 32, instr, INDIRECT, EBX, 0, 0);
    stream = emit_shift_reg(0, stream, 64, instr, INDIRECT, RBX, 0, 0);

    stream = emit_shift_reg(0, stream, 8,  instr, INDIRECT_BYTE_DISPLACED, BL, 0x12, 0);
    stream = emit_shift_reg(0, stream, 16, instr, INDIRECT_BYTE_DISPLACED, BX, 0x12, 0);
    stream = emit_shift_reg(0, stream, 32, instr, INDIRECT_BYTE_DISPLACED, EBX, 0x12, 0);
    stream = emit_shift_reg(0, stream, 64, instr, INDIRECT_BYTE_DISPLACED, RBX, 0x12, 0);

    stream = emit_shift_reg(0, stream, 8,  instr, INDIRECT_DWORD_DISPLACED, BL, 0, 0x12345678);
    stream = emit_shift_reg(0, stream, 16, instr, INDIRECT_DWORD_DISPLACED, BX, 0, 0x12345678);
    stream = emit_shift_reg(0, stream, 32, instr, INDIRECT_DWORD_DISPLACED, EBX, 0, 0x12345678);
    stream = emit_shift_reg(0, stream, 64, instr, INDIRECT_DWORD_DISPLACED, RBX, 0, 0x12345678);
#if 1
    stream = emit_shift_imm8(0, stream, 8,  instr, DIRECT, BL, 3, 0, 0);
    stream = emit_shift_imm8(0, stream, 16, instr, DIRECT, BX, 3, 0, 0);
    stream = emit_shift_imm8(0, stream, 32, instr, DIRECT, EBX, 3, 0, 0);
    stream = emit_shift_imm8(0, stream, 64, instr, DIRECT, RBX, 3, 0, 0);

    stream = emit_shift_imm8(0, stream, 8,  instr, INDIRECT, BL, 3, 0, 0);
    stream = emit_shift_imm8(0, stream, 16, instr, INDIRECT, BX, 3, 0, 0);
    stream = emit_shift_imm8(0, stream, 32, instr, INDIRECT, EBX, 3, 0, 0);
    stream = emit_shift_imm8(0, stream, 64, instr, INDIRECT, RBX, 3, 0, 0);

    stream = emit_shift_imm8(0, stream, 8,  instr, INDIRECT_BYTE_DISPLACED, BL, 3, 0x12, 0);
    stream = emit_shift_imm8(0, stream, 16, instr, INDIRECT_BYTE_DISPLACED, BX, 3, 0x12, 0);
    stream = emit_shift_imm8(0, stream, 32, instr, INDIRECT_BYTE_DISPLACED, EBX, 3, 0x12, 0);
    stream = emit_shift_imm8(0, stream, 64, instr, INDIRECT_BYTE_DISPLACED, RBX, 3, 0x12, 0);

    stream = emit_shift_imm8(0, stream, 8,  instr, INDIRECT_DWORD_DISPLACED, BL, 3, 0, 0x12345678);
    stream = emit_shift_imm8(0, stream, 16, instr, INDIRECT_DWORD_DISPLACED, BX, 3, 0, 0x12345678);
    stream = emit_shift_imm8(0, stream, 32, instr, INDIRECT_DWORD_DISPLACED, EBX, 3, 0, 0x12345678);
    stream = emit_shift_imm8(0, stream, 64, instr, INDIRECT_DWORD_DISPLACED, RBX, 3, 0, 0x12345678);
#endif
    return stream;
}