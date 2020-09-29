#include "util.h"

static u8
jump_short_to_near(u8 opcode) {
	assert(opcode != JCXZ);
	return opcode = 0x80 | (opcode & 0x0f);
}

static u8
jump_near_to_short(u8 opcode) {
	assert(opcode != JCXZ);
	return opcode = 0x70 | (opcode & 0x0f);
}

u8*
emit_jmp_cond_rel32(Instr_Emit_Result* out_info, u8* stream, X64_Jump_Conditional_Short opcode, int offset)
{
    u8* start = stream;
    s8 imm_offset = 0;

    *stream++ = 0x0f;
    *stream++ = jump_short_to_near(opcode);
    imm_offset = stream - start;
    *(int*)stream = offset;
    stream += sizeof(int);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = imm_offset;
        out_info->diplacement_offset = -1;
    }
    return stream;
}

u8*
emit_jmp_cond_short(Instr_Emit_Result* out_info, u8* stream, X64_Jump_Conditional_Short opcode, u8 offset)
{
    u8* start = stream;
    s8 imm_offset = 0;

    *stream++ = jump_near_to_short(opcode);
    imm_offset = stream - start;
    *stream++ = offset;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = imm_offset;
        out_info->diplacement_offset = -1;
    }
    return stream;
}

u8*
emit_jmp_rel_unconditional(Instr_Emit_Result* out_info, u8* stream, int bitsize, Int_Value offset)
{
    u8* start = stream;
    s8 imm_offset = 0;

    assert(ASSEMBLER_BITSIZE == 32);
    u8 opcode = (bitsize == 8) ? 0xEB : 0xE9;
    *stream++ = opcode;
    imm_offset = stream - start;
    stream = emit_int_value(stream, bitsize, offset);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = imm_offset;
        out_info->diplacement_offset = -1;
    }
    return stream;
}

u8*
emit_jmp_reg_unconditional(Instr_Emit_Result* out_info, u8* stream, X64_Addressing_Mode mode, X64_Register reg, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

    // 4 = jmp, 0 = inc, 1 = dec, 2 = call, 6 = push
    *stream++ = 0xff;
    *stream++ = make_modrm(mode, 4, register_representation(reg));

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
emit_call_reg(Instr_Emit_Result* out_info, u8* stream, X64_Addressing_Mode mode, X64_Register reg, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

    // 4 = jmp, 0 = inc, 1 = dec, 2 = call, 6 = push
    *stream++ = 0xff;
    *stream++ = make_modrm(mode, 2, register_representation(reg));

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
emit_call_rel32(Instr_Emit_Result* out_info, u8* stream, int imm)
{
    u8* start = stream;
    *stream++ = 0xe8;
    s8 imm_offset = stream - start;
    *(int*)stream = imm;
    stream += sizeof(int);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = -1;
        out_info->immediate_offset = imm_offset;
    }
    return stream;
}

u8*
emit_push_reg(Instr_Emit_Result* out_info, u8* stream, X64_Addressing_Mode mode, X64_Register reg, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

    // 4 = jmp, 0 = inc, 1 = dec, 2 = call, 6 = push
    *stream++ = 0xff;
    *stream++ = make_modrm(mode, 6, register_representation(reg));

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
emit_pop_reg(Instr_Emit_Result* out_info, u8* stream, X64_Register reg)
{
    u8* start = stream;
    *stream++ = (0x58 | (register_representation(reg) & 0xf));

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = -1;
        out_info->immediate_offset = -1;
    }
    return stream;
}

u8*
emit_ret(Instr_Emit_Result* out_info, u8* stream, X64_Ret_Instruction opcode)
{
    u8* start = stream;
    *stream++ = opcode;
    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = -1;
        out_info->immediate_offset = -1;
    }
    return stream;
}

u8*
emit_jmp_cond_test(u8* stream)
{
    stream = emit_pop_reg(0, stream, EBP);

#if 0
    stream = emit_jmp_cond_rel32(0, stream, JZ, 0x12345678);
    stream = emit_jmp_reg_unconditional(0, stream, DIRECT, RBX, 0, 0);
    stream = emit_jmp_reg_unconditional(0, stream, INDIRECT, RBX, 0, 0);
    stream = emit_jmp_reg_unconditional(0, stream, INDIRECT_BYTE_DISPLACED, RBX, 0x12, 0);
    stream = emit_jmp_reg_unconditional(0, stream, INDIRECT_DWORD_DISPLACED, RBX, 0, 0x12345678);

    stream = emit_call_reg(0, stream, DIRECT, RBX, 0, 0);
    stream = emit_call_reg(0, stream, INDIRECT, RBX, 0, 0);
    stream = emit_call_reg(0, stream, INDIRECT_BYTE_DISPLACED, RBX, 0x12, 0);
    stream = emit_call_reg(0, stream, INDIRECT_DWORD_DISPLACED, RBX, 0, 0x12345678);
#endif
#if 0
    stream = emit_jmp_rel_unconditional(0, stream, 8, (Int_Value){.v8 = 0x12});
    //stream = emit_jmp_rel_unconditional(0, stream, 16, (Int_Value){.v16 = 0x1234});  // 32 bit only
    stream = emit_jmp_rel_unconditional(0, stream, 32, (Int_Value){.v32 = 0x12345678});
#endif
#if 0
    stream = emit_jmp_cond_short(0, stream, JE, 0x12);
    stream = emit_jmp_cond_short(0, stream, JNE, 0x12);
    stream = emit_jmp_cond_short(0, stream, JLE, 0x12);
    stream = emit_jmp_cond_short(0, stream, JG, 0x12);
    stream = emit_jmp_cond_short(0, stream, JO, 0x12);
#endif

    //stream = emit_jump_short_to_near(0, stream, JE, 0x12);
    return stream;
}