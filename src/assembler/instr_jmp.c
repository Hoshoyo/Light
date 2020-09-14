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
emit_jmp_cond_short(u8* stream, X64_Jump_Conditional_Short opcode, u8 offset)
{
    *stream++ = jump_near_to_short(opcode);
    *stream++ = offset;
    return stream;
}

u8*
emit_jmp_rel_unconditional(u8* stream, int bitsize, Int_Value offset)
{
    assert(ASSEMBLER_BITSIZE == 32);
    u8 opcode = (bitsize == 8) ? 0xEB : 0xE9;
    *stream++ = opcode;
    stream = emit_int_value(stream, bitsize, offset);
    return stream;
}

u8*
emit_jmp_reg_unconditional(u8* stream, X64_Addressing_Mode mode, X64_Register reg, u8 disp8, uint32_t disp32)
{
    // 4 = jmp, 0 = inc, 1 = dec, 2 = call, 6 = push
    *stream++ = 0xff;
    *stream++ = make_modrm(mode, 4, register_representation(reg));
    stream = emit_displacement(mode, stream, disp8, disp32);
    return stream;
}

u8*
emit_call_reg(u8* stream, X64_Addressing_Mode mode, X64_Register reg, u8 disp8, uint32_t disp32)
{
    // 4 = jmp, 0 = inc, 1 = dec, 2 = call, 6 = push
    *stream++ = 0xff;
    *stream++ = make_modrm(mode, 2, register_representation(reg));
    stream = emit_displacement(mode, stream, disp8, disp32);
    return stream;
}

u8*
emit_jmp_cond_test(u8* stream)
{
    stream = emit_jmp_reg_unconditional(stream, DIRECT, RBX, 0, 0);
    stream = emit_jmp_reg_unconditional(stream, INDIRECT, RBX, 0, 0);
    stream = emit_jmp_reg_unconditional(stream, INDIRECT_BYTE_DISPLACED, RBX, 0x12, 0);
    stream = emit_jmp_reg_unconditional(stream, INDIRECT_DWORD_DISPLACED, RBX, 0, 0x12345678);

    stream = emit_call_reg(stream, DIRECT, RBX, 0, 0);
    stream = emit_call_reg(stream, INDIRECT, RBX, 0, 0);
    stream = emit_call_reg(stream, INDIRECT_BYTE_DISPLACED, RBX, 0x12, 0);
    stream = emit_call_reg(stream, INDIRECT_DWORD_DISPLACED, RBX, 0, 0x12345678);
#if 0
    stream = emit_jmp_rel_unconditional(stream, 8, (Int_Value){.v8 = 0x12});
    //stream = emit_jmp_rel_unconditional(stream, 16, (Int_Value){.v16 = 0x1234});  // 32 bit only
    stream = emit_jmp_rel_unconditional(stream, 32, (Int_Value){.v32 = 0x12345678});
#endif
#if 0
    stream = emit_jmp_cond_short(stream, JE, 0x12);
    stream = emit_jmp_cond_short(stream, JNE, 0x12);
    stream = emit_jmp_cond_short(stream, JLE, 0x12);
    stream = emit_jmp_cond_short(stream, JG, 0x12);
    stream = emit_jmp_cond_short(stream, JO, 0x12);
#endif

    //stream = emit_jump_short_to_near(stream, JE, 0x12);
    return stream;
}