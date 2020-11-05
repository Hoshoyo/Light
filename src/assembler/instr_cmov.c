#include "util.h"

u8*
emit_cmov_reg(Instr_Emit_Result* out_info, u8* stream, X64_Addressing_Mode mode, 
    X64_CMOVcc_Instruction opcode, int bitsize, X64_Register dest, X64_Register src, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

    stream = emit_opcode_2bytes(stream, 0x0f | (opcode << 8), bitsize, dest, src);
    *stream++ = make_modrm(mode, register_representation(src), register_representation(dest));
    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);
    if((stream - start) == disp_offset) disp_offset = -1; // when stream is equal, no displacement exists

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1; // there is no immediate
        out_info->diplacement_offset = disp_offset;
    }

    return stream;
}

u8*
emit_cmov_test(u8* stream)
{
    stream = emit_cmov_reg(0, stream, DIRECT, CMOVE, 16, AX, DX, 0, 0);
    /*
    stream = emit_cmov_reg(0, stream, DIRECT, CMOVE, 32, EBX, ECX, 0, 0);
    stream = emit_cmov_reg(0, stream, DIRECT, CMOVNE, 32, EBX, ECX, 0, 0);
	stream = emit_cmov_reg(0, stream, DIRECT, CMOVA, 32, EBX, ECX, 0, 0);
	stream = emit_cmov_reg(0, stream, DIRECT, CMOVAE, 32, EBX, ECX, 0, 0);
	stream = emit_cmov_reg(0, stream, DIRECT, CMOVB, 32, EBX, ECX, 0, 0);
	stream = emit_cmov_reg(0, stream, DIRECT, CMOVBE, 32, EBX, ECX, 0, 0);
	stream = emit_cmov_reg(0, stream, DIRECT, CMOVG, 32, EBX, ECX, 0, 0);
	stream = emit_cmov_reg(0, stream, DIRECT, CMOVGE, 32, EBX, ECX, 0, 0);
	stream = emit_cmov_reg(0, stream, DIRECT, CMOVL, 32, EBX, ECX, 0, 0);
	stream = emit_cmov_reg(0, stream, DIRECT, CMOVLE, 32, EBX, ECX, 0, 0);

    stream = emit_cmov_reg(0, stream, INDIRECT, CMOVE, 32, EBX, ECX, 0, 0);
    stream = emit_cmov_reg(0, stream, INDIRECT_BYTE_DISPLACED, CMOVE, 32, EBX, ECX, 0x12, 0);
    stream = emit_cmov_reg(0, stream, INDIRECT_DWORD_DISPLACED, CMOVE, 32, EBX, ECX, 0, 0x12345678);
    */

    return stream;
}