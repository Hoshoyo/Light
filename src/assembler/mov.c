#include "util.h"

#define MOVE_MR_8 0x88
#define MOVE_MR 0x89
#define MOVE_RM_8 0x8A
#define MOVE_RM 0x8B
#define MOVE_OI_8 0xB0
#define MOVE_OI 0xB8
#define MOVE_MI_8 0xC6
#define MOVE_MI 0xC7
#define MOVE_MOFFS 0xA3
#define MOVE_MOFFS_8 0xA2

// *********************************
static u8*
emit_mov_direct(u8* stream, u8 opcode, int bitsize, X64_Register dest, X64_Register src)
{
    assert(register_get_bitsize(src) == register_get_bitsize(dest));
    
	stream = emit_opcode(stream, opcode, bitsize, dest, src);
	*stream++ = make_modrm(DIRECT, register_representation(src), register_representation(dest));
	return stream;
}
// Example: mov r8, rax
u8*
emit_mov_mr_direct(u8* stream, X64_Register dest, X64_Register src)
{
    int bitsize = register_get_bitsize(src);
    return emit_mov_direct(stream, (bitsize == 8) ? MOVE_MR_8 : MOVE_MR, bitsize, dest, src);
}
// Example: mov rax, r8
u8*
emit_mov_rm_direct(u8* stream, X64_Register dest, X64_Register src)
{
    int bitsize = register_get_bitsize(dest);
    return emit_mov_direct(stream, (bitsize == 8) ? MOVE_RM_8 : MOVE_RM, bitsize, dest, src);
}

// *********************************
static u8*
emit_mov_indirect(u8* stream, u8 opcode, int bitsize, X64_Register dest, X64_Register src)
{
    assert(register_get_bitsize(dest) == 64);
	if(register_representation(dest) == register_representation(RSP))
        assert(0 && "function cannot be used with RSP as indirect, need a SIB byte");

    stream = emit_opcode(stream, opcode, bitsize, dest, src);
	*stream++ = make_modrm(INDIRECT, register_representation(src), register_representation(dest));
	return stream;
}
// Example: mov [r8], r9d
u8*
emit_mov_mr_indirect(u8* stream, X64_Register dest, X64_Register src)
{
    int bitsize = register_get_bitsize(src);
    return emit_mov_indirect(stream, (bitsize == 8) ? MOVE_MR_8 : MOVE_MR, bitsize, dest, src);
}
// Example: mov r9d, [r8]
u8*
emit_mov_rm_indirect(u8* stream, X64_Register dest, X64_Register src)
{
    int bitsize = register_get_bitsize(dest);
    return emit_mov_indirect(stream, (bitsize == 8) ? MOVE_RM_8 : MOVE_RM, bitsize, dest, src);
}

// *********************************
static u8*
emit_mov_indirect_byte_displacement(u8* stream, u8 opcode, int bitsize, s8 displacement, X64_Register dest, X64_Register src)
{
    assert(register_get_bitsize(dest) == 64);

    stream = emit_opcode(stream, opcode, bitsize, dest, src);
    *stream++ = make_modrm(INDIRECT_BYTE_DISPLACED, register_representation(src), register_representation(dest));
    *stream++ = displacement;
    return stream;
}
// Example: mov [r8 + 0x7b], r9d
u8*
emit_mov_mr_indirect_byte_displacement(u8* stream, s8 displacement, X64_Register dest, X64_Register src)
{
    int bitsize = register_get_bitsize(src);
    return emit_mov_indirect_byte_displacement(stream, (bitsize == 8) ? MOVE_MR_8 : MOVE_MR, bitsize, displacement, dest, src);
}
// Example: mov [r8 + 0x7b], r9d
u8*
emit_mov_rm_indirect_byte_displacement(u8* stream, s8 displacement, X64_Register dest, X64_Register src)
{
    int bitsize = register_get_bitsize(dest);
    return emit_mov_indirect_byte_displacement(stream, (bitsize == 8) ? MOVE_RM_8 : MOVE_RM, bitsize, displacement, dest, src);
}

// *********************************
u8*
emit_mov_indirect_dword_displacement(u8* stream, u8 opcode, int bitsize, int displacement, X64_Register dest, X64_Register src)
{
    assert(register_get_bitsize(dest) == 64);

    stream = emit_opcode(stream, opcode, bitsize, dest, src);
    *stream++ = make_modrm(INDIRECT_DWORD_DISPLACED, register_representation(src), register_representation(dest));
    *(int*)stream = displacement;
    stream += sizeof(int);
    return stream;
}
// Example: mov [r8 + 0x123456], r9d
u8*
emit_mov_mr_indirect_dword_displacement(u8* stream, int displacement, X64_Register dest, X64_Register src)
{
    int bitsize = register_get_bitsize(src);
    return emit_mov_indirect_dword_displacement(stream, (bitsize == 8) ? MOVE_MR_8 : MOVE_MR, bitsize, displacement,
        dest, src);
}
// Example: mov r9d, [r8 + 0x123456]
u8*
emit_mov_rm_indirect_dword_displacement(u8* stream, int displacement, X64_Register dest, X64_Register src)
{
    int bitsize = register_get_bitsize(dest);
    return emit_mov_indirect_dword_displacement(stream, (bitsize == 8) ? MOVE_RM_8 : MOVE_RM, bitsize, displacement,
        dest, src);
}

// *********************************
// Destination is always [rsp]
u8*
emit_mov_indirect_sib(u8* stream, u8 opcode, X64_Register src, u8 scale, X64_Register index, X64_Register base_reg)
{
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
    return stream;
}
// Example: mov [rsp], eax
u8*
emit_mov_mr_indirect_sib(u8* stream, X64_Register src, u8 scale, X64_Register index, X64_Register base_reg)
{
    return emit_mov_indirect_sib(stream, (register_get_bitsize(src) == 8) ? MOVE_MR_8 : MOVE_MR, src, scale, index, base_reg);
}
// Example: mov eax, [rsp]
u8*
emit_mov_rm_indirect_sib(u8* stream, X64_Register src, u8 scale, X64_Register index, X64_Register base_reg)
{
    return emit_mov_indirect_sib(stream, (register_get_bitsize(src) == 8) ? MOVE_RM_8 : MOVE_RM, src, scale, index, base_reg);
}

// *********************************
// immediate
// Example: mov eax, 0x1234
u8*
emit_mov_oi(u8* stream, X64_Register dest, Int_Value value)
{
    int bitsize = register_get_bitsize(dest);
    u8 opcode = MOVE_OI;
    if(bitsize == 8) opcode = MOVE_OI_8;

    opcode = (opcode | register_representation(dest));
    stream = emit_opcode(stream, opcode, bitsize, dest, REG_NONE);
    return emit_int_value(stream, bitsize, value);
}
// When register is 64 bits, max move imm32 sign extended
// Example
u8*
emit_mov_mi(u8* stream, X64_Register dest, Int_Value value)
{
    int bitsize = register_get_bitsize(dest);
    u8 opcode = MOVE_MI;
    if(bitsize == 8) opcode = MOVE_MI_8;

    stream = emit_opcode(stream, opcode, bitsize, dest, REG_NONE);
    *stream++ = register_representation(dest);

    if(bitsize == 64) bitsize = 32;
    return emit_int_value(stream, bitsize, value);
}

// *********************************
// MOFFS
u8*
emit_mov_moffs(u8* stream, X64_Register src, uint64_t addr, bool invert)
{
    int bitsize = register_get_bitsize(src);
    if(bitsize == 16) *stream++ = 0x66;
    if(bitsize == 64) *stream++ = make_rex(0,0,0,1);

    u8 opcode = MOVE_MOFFS;
    if(bitsize == 8) opcode = MOVE_MOFFS_8;
    if(invert) opcode -= 2;
    *stream++ = opcode;

    *(uint64_t*)stream = addr;
    stream += sizeof(addr);

    return stream;
}

u8*
emit_mov_test(u8* stream)
{
#if 1
    stream = emit_mov_moffs(stream, AL, 0x123456789abcdef, false);
    stream = emit_mov_moffs(stream, AX, 0x123456789abcdef, false);
    stream = emit_mov_moffs(stream, EAX, 0x123456789abcdef, false);
    stream = emit_mov_moffs(stream, RAX, 0x123456789abcdef, false);

    stream = emit_mov_moffs(stream, AL, 0x123456789abcdef, true);
    stream = emit_mov_moffs(stream, AX, 0x123456789abcdef, true);
    stream = emit_mov_moffs(stream, EAX, 0x123456789abcdef, true);
    stream = emit_mov_moffs(stream, RAX, 0x123456789abcdef, true);
#endif
#if 1
    stream = emit_mov_mi(stream, BL, (Int_Value){.v8 = 0x12});
    stream = emit_mov_mi(stream, BX, (Int_Value){.v16 = 0x1234});
    stream = emit_mov_mi(stream, EBX, (Int_Value){.v32 = 0x12345678});
    stream = emit_mov_mi(stream, RBX, (Int_Value){.v32 = 0x12345678});

    stream = emit_mov_oi(stream, BL, (Int_Value){.v8 = 0x12});
    stream = emit_mov_oi(stream, BX, (Int_Value){.v16 = 0x1234});
    stream = emit_mov_oi(stream, EBX, (Int_Value){.v32 = 0x12345678});
    stream = emit_mov_oi(stream, RBX, (Int_Value){.v64 = 0x123456789abcdef});
#endif
#if 1
    stream = emit_mov_mr_direct(stream, AL, BL);
    stream = emit_mov_mr_direct(stream, AX, BX);
    stream = emit_mov_mr_direct(stream, EAX, EBX);
    stream = emit_mov_mr_direct(stream, RAX, RBX);

    stream = emit_mov_mr_direct(stream, AL, R8B);
    stream = emit_mov_mr_direct(stream, AX, R8W);
    stream = emit_mov_mr_direct(stream, EAX, R8D);
    stream = emit_mov_mr_direct(stream, RAX, R8);

    stream = emit_mov_mr_direct(stream, R8B, R9B);
    stream = emit_mov_mr_direct(stream, R8W, R9W);
    stream = emit_mov_mr_direct(stream, R8D, R9D);
    stream = emit_mov_mr_direct(stream, R8, R9);
    
    stream = emit_mov_mr_indirect(stream, RAX, BL);
    stream = emit_mov_mr_indirect(stream, RAX, BX);
    stream = emit_mov_mr_indirect(stream, RAX, EBX);
    stream = emit_mov_mr_indirect(stream, RAX, RBX);

    stream = emit_mov_mr_indirect(stream, RAX, R8B);
    stream = emit_mov_mr_indirect(stream, RAX, R8W);
    stream = emit_mov_mr_indirect(stream, RAX, R8D);
    stream = emit_mov_mr_indirect(stream, RAX, R8);

    stream = emit_mov_mr_indirect(stream, R8, R9B);
    stream = emit_mov_mr_indirect(stream, R8, R9W);
    stream = emit_mov_mr_indirect(stream, R8, R9D);
    stream = emit_mov_mr_indirect(stream, R8, R9);
#endif
#if 1
    stream = emit_mov_mr_direct(stream, BL, AH);
    stream = emit_mov_mr_direct(stream, BL, DIL);
    stream = emit_mov_mr_direct(stream, DIL, BL);
    stream = emit_mov_mr_direct(stream, BPL, SPL);
#endif
#if 1
    stream = emit_mov_mr_direct(stream, SIL, AL);
    stream = emit_mov_mr_direct(stream, DIL, AL);
    stream = emit_mov_mr_direct(stream, BPL, AL);
    stream = emit_mov_mr_direct(stream, SPL, AL);

    stream = emit_mov_mr_direct(stream, AL, SIL);
    stream = emit_mov_mr_direct(stream, AL, DIL);
    stream = emit_mov_mr_direct(stream, AL, BPL);
    stream = emit_mov_mr_direct(stream, AL, SPL);
    stream = emit_mov_mr_direct(stream, AX, SP);
    stream = emit_mov_mr_direct(stream, EAX, ESP);
    stream = emit_mov_mr_direct(stream, RAX, RSP);
#endif
#if 1
    stream = emit_mov_mr_indirect(stream, R8, SPL);
    stream = emit_mov_mr_indirect(stream, R8, SP);
    stream = emit_mov_mr_indirect(stream, R8, ESP);
    stream = emit_mov_mr_indirect(stream, R8, RSP);
#endif

    // Byte displaced
#if 1
    stream = emit_mov_mr_indirect_byte_displacement(stream, -5, RAX, BL);
    stream = emit_mov_mr_indirect_byte_displacement(stream, -5, RAX, BX);
    stream = emit_mov_mr_indirect_byte_displacement(stream, -5, RAX, EBX);
    stream = emit_mov_mr_indirect_byte_displacement(stream, -5, RAX, RBX);

    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, RAX, BL);
    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, RAX, BX);
    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, RAX, EBX);
    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, RAX, RBX);

    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, RAX, R8B);
    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, RAX, R8W);
    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, RAX, R8D);
    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, RAX, R8);

    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, R8, R9B);
    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, R8, R9W);
    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, R8, R9D);
    stream = emit_mov_mr_indirect_byte_displacement(stream, 123, R8, R9);
#endif

    // Dword displaced
#if 1
    stream = emit_mov_mr_indirect_dword_displacement(stream, -122225, RAX, BL);
    stream = emit_mov_mr_indirect_dword_displacement(stream, -122225, RAX, BX);
    stream = emit_mov_mr_indirect_dword_displacement(stream, -122225, RAX, EBX);
    stream = emit_mov_mr_indirect_dword_displacement(stream, -122225, RAX, RBX);

    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, RAX, BL);
    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, RAX, BX);
    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, RAX, EBX);
    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, RAX, RBX);

    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, RAX, R8B);
    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, RAX, R8W);
    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, RAX, R8D);
    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, RAX, R8);

    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, R8, R9B);
    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, R8, R9W);
    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, R8, R9D);
    stream = emit_mov_mr_indirect_dword_displacement(stream, 0x12345678, R8, R9);
#endif
#if 1
    stream = emit_mov_mr_indirect_sib(stream, EAX, 1, R8, RDX);
    stream = emit_mov_mr_indirect_sib(stream, EAX, 1, R8, REG_NONE);
    stream = emit_mov_mr_indirect_sib(stream, EAX, 1, REG_NONE, RDX);
    stream = emit_mov_mr_indirect_sib(stream, EAX, 1, RSP, ESP);
#endif

    return stream;
}