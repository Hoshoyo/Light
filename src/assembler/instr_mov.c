#include "util.h"

#define MOVE_OI_8 0xB0
#define MOVE_OI 0xB8
#define MOVE_MI_8 0xC6
#define MOVE_MI 0xC7
#define MOVE_MOFFS 0xA3
#define MOVE_MOFFS_8 0xA2

u8*
emit_mov_reg(Instr_Emit_Result* out_info, u8* stream, X64_Mov_Instr opcode, X64_Addressing_Mode mode, 
    int bitsize, X64_Register dest, X64_Register src, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

	stream = emit_opcode(stream, opcode, bitsize, dest, src);
    *stream++ = make_modrm(mode, register_representation(src), register_representation(dest));
    
    if(opcode == MOV_MR8 || opcode == MOV_MR)
    {
        if(register_equivalent(RSP, dest))
        {
            *stream++ = make_sib(0, RSP, RSP);
        }
    }
    else if(opcode == MOV_RM8 || opcode == MOV_RM)
    {
        if(register_equivalent(RSP, src))
        {
            *stream++ = make_sib(0, RSP, RSP);
        }
    }

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

// *********************************
// Destination is always [rsp]
u8*
emit_mov_indirect_sib(Instr_Emit_Result* out_info, u8* stream, u8 opcode, X64_Register src, u8 scale, X64_Register index, X64_Register base_reg)
{
    u8* start = stream;
    // TODO(psv): handle [*] when base = RBP, should have a displacement
    if(base_reg == REG_NONE) base_reg = RSP;
    if(index == REG_NONE) index = RSP;

    base_reg = register_representation(base_reg);
    index = register_representation(index);

    // TODO(psv): avoid incorrect registers
    assert(scale >= 0 && scale <= 3 && "sib scale only has 2 bits");
    assert(index >= 0 && index <= 7 && "sib index only has 3 bits");

    stream = emit_opcode(stream, opcode, register_get_bitsize(src), RSP, src);
    *stream++ = make_modrm(INDIRECT, register_representation(src), register_representation(RSP));
    *stream++ = make_sib(scale, (u8)index, (u8)base_reg);
    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = -1;
        out_info->immediate_offset = -1;
    }
    return stream;
}
// Example: mov [rsp], eax
u8*
emit_mov_mr_indirect_sib(Instr_Emit_Result* out_info, u8* stream, X64_Register src, u8 scale, X64_Register index, X64_Register base_reg)
{
    return emit_mov_indirect_sib(out_info, stream, (register_get_bitsize(src) == 8) ? MOV_MR8 : MOV_MR, src, scale, index, base_reg);
}
// Example: mov eax, [rsp]
u8*
emit_mov_rm_indirect_sib(Instr_Emit_Result* out_info, u8* stream, X64_Register src, u8 scale, X64_Register index, X64_Register base_reg)
{
    return emit_mov_indirect_sib(out_info, stream, (register_get_bitsize(src) == 8) ? MOV_RM8 : MOV_RM, src, scale, index, base_reg);
}

// *********************************
// immediate
// Example: mov eax, 0x1234
u8*
emit_mov_oi(Instr_Emit_Result* out_info, u8* stream, X64_Register dest, Int_Value value)
{
    u8* start = stream;
    s8 imm_offset = 0;

    int bitsize = register_get_bitsize(dest);
    u8 opcode = MOVE_OI;
    if(bitsize == 8) opcode = MOVE_OI_8;

    opcode = (opcode | register_representation(dest));
    stream = emit_opcode(stream, opcode, bitsize, dest, REG_NONE);
    imm_offset = stream - start;
    stream = emit_int_value(stream, bitsize, value);
    assert(stream - start != imm_offset);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = -1;
        out_info->immediate_offset = imm_offset;
    }
    return stream;
}
// When register is 64 bits, max move imm32 sign extended
// Example
u8*
emit_mov_mi(Instr_Emit_Result* out_info, u8* stream, X64_Register dest, Int_Value value)
{
    u8* start = stream;
    s8 imm_offset = 0;

    int bitsize = register_get_bitsize(dest);
    u8 opcode = MOVE_MI;
    if(bitsize == 8) opcode = MOVE_MI_8;

    stream = emit_opcode(stream, opcode, bitsize, dest, REG_NONE);
    *stream++ = register_representation(dest);
    if(bitsize == 64) bitsize = 32;

    imm_offset = stream - start;
    stream = emit_int_value(stream, bitsize, value);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = imm_offset;
        out_info->diplacement_offset = -1;
    }

    return stream;
}

// *********************************
// MOFFS
u8*
emit_mov_moffs(Instr_Emit_Result* out_info, u8* stream, X64_Register src, uint64_t addr, bool invert)
{
    u8* start = stream;
    s8 imm_offset = 0;

    int bitsize = register_get_bitsize(src);
    if(bitsize == 16) *stream++ = 0x66;
    if(bitsize == 64) *stream++ = make_rex(0,0,0,1);

    u8 opcode = MOVE_MOFFS;
    if(bitsize == 8) opcode = MOVE_MOFFS_8;
    if(invert) opcode -= 2;
    *stream++ = opcode;

    imm_offset = stream - start;
    *(uint64_t*)stream = addr;
    stream += sizeof(addr);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = imm_offset;
        out_info->diplacement_offset = -1;
    }

    return stream;
}

u8*
emit_lea(Instr_Emit_Result* out_info, u8* stream, int bitsize, X64_Addressing_Mode mode, X64_Register dest, X64_Register src, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = -1;

    assert(register_get_bitsize(dest) == bitsize);
    stream = emit_opcode(stream, 0x8D, bitsize, src, dest);
    *stream++ = make_modrm(mode, register_representation(dest), register_representation(src));
    
    if (register_equivalent(RSP, src))
    {
        *stream++ = make_sib(0, RSP, RSP);
    }

    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1;
        out_info->diplacement_offset = disp_offset;
    }
    return stream;
}

// mov with sign extend (signed)
u8*
emit_movsx(Instr_Emit_Result* out_info, u8* stream, X64_Addressing_Mode mode, int src_bitsize, int dst_bitsize, 
    X64_Register dest, X64_Register src, u8 disp8, uint32_t disp32)
{
    bool using_extended_register = register_is_extended(dest) || register_is_extended(src);
    u8* start = stream;
    s8 disp_offset = -1;

    if(dst_bitsize == 16)
        *stream++ = 0x66;
    if(dst_bitsize == 64 || using_extended_register)
		*stream++ = make_rex(register_is_extended(dest), 0, register_is_extended(src), dst_bitsize == 64);

    if(src_bitsize == 8)
    {
        *stream++ = 0x0f;
        *stream++ = 0xbe;
    }
    else if(src_bitsize == 16)
    {
        *stream++ = 0x0f;
        *stream++ = 0xbf;
    }
    *stream++ = make_modrm(mode, register_representation(dest), register_representation(src));
    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1;
        out_info->diplacement_offset = disp_offset;
    }

    return stream;
}

// move with zero extend (unsigned)
u8*
emit_movzx(Instr_Emit_Result* out_info, u8* stream, X64_Addressing_Mode mode, int src_bitsize, int dst_bitsize, 
    X64_Register dest, X64_Register src, u8 disp8, uint32_t disp32)
{
    bool using_extended_register = register_is_extended(dest) || register_is_extended(src);
    u8* start = stream;
    s8 disp_offset = -1;

    if(dst_bitsize == 16)
        *stream++ = 0x66;
    if(dst_bitsize == 64 || using_extended_register)
		*stream++ = make_rex(register_is_extended(dest), 0, register_is_extended(src), dst_bitsize == 64);

    if(src_bitsize == 8)
    {
        *stream++ = 0x0f;
        *stream++ = 0xb6;
    }
    else if(src_bitsize == 16)
    {
        *stream++ = 0x0f;
        *stream++ = 0xb7;
    }
    *stream++ = make_modrm(mode, register_representation(dest), register_representation(src));
    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1;
        out_info->diplacement_offset = disp_offset;
    }

    return stream;
}

u8*
emit_rep_movs(Instr_Emit_Result* out_info, u8* stream, int bitsize)
{
    u8* start = stream;
    if(bitsize == 16)
        *stream++ = 0x66;
    if(bitsize == 64)
		*stream++ = make_rex(0, 0, 0, bitsize == 64);
    *stream++ = 0xf3;
    *stream++ = 0xa5;

    return stream;
}

u8*
emit_mov_test(u8* stream)
{
    //stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT_DWORD_DISPLACED, 32, ESP, ECX, 0, 0x12345);
    //stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT_DWORD_DISPLACED, 8, ESP, SIL, 0, 0x12345);
    //stream = emit_mov_reg(0, stream, MOV_MR8, INDIRECT_DWORD_DISPLACED, 32, ESP, ECX, 0, 0x12345);
    //stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT_DWORD_DISPLACED, 32, ECX, ESP, 0, 0x12345);

#if 0
    stream = emit_movsx(0, stream, DIRECT, 8, 16, CX, AL, 0, 0);
    stream = emit_movsx(0, stream, DIRECT, 8, 32, ECX, AL, 0, 0);
    stream = emit_movsx(0, stream, DIRECT, 16, 32, ECX, AX, 0, 0);

    stream = emit_movzx(0, stream, DIRECT, 8, 16, CX, AL, 0, 0);
    stream = emit_movzx(0, stream, DIRECT, 8, 32, ECX, AL, 0, 0);
    stream = emit_movzx(0, stream, DIRECT, 16, 32, ECX, AX, 0, 0);
#endif

#if 0
    // MR
    stream = emit_mov_reg(0, stream, MOV_MR8, DIRECT, 8, AL, BL, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, DIRECT, 16, AX, BX, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, DIRECT, 32, EAX, EBX, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, DIRECT, 64, RAX, RBX, 0, 0);

    stream = emit_mov_reg(0, stream, MOV_MR8, INDIRECT, 8, AL, BL, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT, 16, AX, BX, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT, 32, EAX, EBX, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT, 64, RAX, RBX, 0, 0);

    stream = emit_mov_reg(0, stream, MOV_MR8, INDIRECT_BYTE_DISPLACED, 8, AL, BL, 0x12, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT_BYTE_DISPLACED, 16, AX, BX, 0x12, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT_BYTE_DISPLACED, 32, EAX, EBX, 0x12, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT_BYTE_DISPLACED, 64, RAX, RBX, 0x12, 0);

    stream = emit_mov_reg(0, stream, MOV_MR8, INDIRECT_DWORD_DISPLACED, 8, AL, BL, 0, 0x12345678);
    stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT_DWORD_DISPLACED, 16, AX, BX, 0, 0x12345678);
    stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT_DWORD_DISPLACED, 32, EAX, EBX, 0, 0x12345678);
    stream = emit_mov_reg(0, stream, MOV_MR, INDIRECT_DWORD_DISPLACED, 64, RAX, RBX, 0, 0x12345678);

    stream = emit_mov_reg(0, stream, MOV_MR8, DIRECT, 8, AL, R8B, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, DIRECT, 16, AX, R8W, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, DIRECT, 32, EAX, R8D, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR, DIRECT, 64, RAX, R8, 0, 0);

    stream = emit_mov_reg(0, stream, MOV_MR8, DIRECT, 8, BL, AH, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR8, DIRECT, 8, BL, DIL, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR8, DIRECT, 8, DIL, BL, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR8, DIRECT, 8, BPL, SPL, 0, 0);

    stream = emit_mov_reg(0, stream, MOV_MR8, INDIRECT, 8, R8, SPL, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR8, INDIRECT, 16, R8, SP, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR8, INDIRECT, 32, R8, ESP, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_MR8, INDIRECT, 64, R8, RSP, 0, 0);

#endif
#if 0
    // RM
    stream = emit_mov_reg(0, stream, MOV_RM8, DIRECT, 8, AL, BL, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_RM, DIRECT, 16, AX, BX, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_RM, DIRECT, 32, EAX, EBX, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_RM, DIRECT, 64, RAX, RBX, 0, 0);

    stream = emit_mov_reg(0, stream, MOV_RM8, INDIRECT, 8, AL, BL, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT, 16, AX, BX, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT, 32, EAX, EBX, 0, 0);
    stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT, 64, RAX, RBX, 0, 0);

    stream = emit_mov_reg(0, stream, MOV_RM8, INDIRECT_BYTE_DISPLACED, 8, AL, BL, 0x12, 0);
    stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT_BYTE_DISPLACED, 16, AX, BX, 0x12, 0);
    stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT_BYTE_DISPLACED, 32, EAX, EBX, 0x12, 0);
    stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT_BYTE_DISPLACED, 64, RAX, RBX, 0x12, 0);

    stream = emit_mov_reg(0, stream, MOV_RM8, INDIRECT_DWORD_DISPLACED, 8, AL, BL, 0, 0x12345678);
    stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT_DWORD_DISPLACED, 16, AX, BX, 0, 0x12345678);
    stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT_DWORD_DISPLACED, 32, EAX, EBX, 0, 0x12345678);
    stream = emit_mov_reg(0, stream, MOV_RM, INDIRECT_DWORD_DISPLACED, 64, RAX, RBX, 0, 0x12345678);
#endif

#if 0
    stream = emit_lea(0, stream, 16, AX, R8);
    stream = emit_lea(0, stream, 16, DX, RBX);

    stream = emit_lea(0, stream, 32, EAX, R8);
    stream = emit_lea(0, stream, 32, EDX, RBX);

    stream = emit_lea(0, stream, 64, RAX, R8);
    stream = emit_lea(0, stream, 64, RDX, RBX);
#endif
#if 0
    stream = emit_mov_moffs(0, stream, AL, 0x123456789abcdef, false);
    stream = emit_mov_moffs(0, stream, AX, 0x123456789abcdef, false);
    stream = emit_mov_moffs(0, stream, EAX, 0x123456789abcdef, false);
    stream = emit_mov_moffs(0, stream, RAX, 0x123456789abcdef, false);

    stream = emit_mov_moffs(0, stream, AL, 0x123456789abcdef, true);
    stream = emit_mov_moffs(0, stream, AX, 0x123456789abcdef, true);
    stream = emit_mov_moffs(0, stream, EAX, 0x123456789abcdef, true);
    stream = emit_mov_moffs(0, stream, RAX, 0x123456789abcdef, true);
#endif
#if 0
    stream = emit_mov_mi(0, stream, BL, (Int_Value){.v8 = 0x12});
    stream = emit_mov_mi(0, stream, BX, (Int_Value){.v16 = 0x1234});
    stream = emit_mov_mi(0, stream, EBX, (Int_Value){.v32 = 0x12345678});
    stream = emit_mov_mi(0, stream, RBX, (Int_Value){.v32 = 0x12345678});

    stream = emit_mov_oi(0, stream, BL, (Int_Value){.v8 = 0x12});
    stream = emit_mov_oi(0, stream, BX, (Int_Value){.v16 = 0x1234});
    stream = emit_mov_oi(0, stream, EBX, (Int_Value){.v32 = 0x12345678});
    stream = emit_mov_oi(0, stream, RBX, (Int_Value){.v64 = 0x123456789abcdef});
#endif
#if 0
    stream = emit_mov_mr_indirect_sib(0, stream, EAX, 1, R8, RDX);
    stream = emit_mov_mr_indirect_sib(0, stream, EAX, 1, R8, REG_NONE);
    stream = emit_mov_mr_indirect_sib(0, stream, EAX, 1, REG_NONE, RDX);
    stream = emit_mov_mr_indirect_sib(0, stream, EAX, 1, RSP, ESP);
#endif

    return stream;
}