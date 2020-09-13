#include "util.h"

#define ADD_8 0x80
#define ADD 0x81
#define ADDS 0x83

static u8*
arith_simple_start(u8* stream, u8 opcode, int bitsize)
{
    
}

/*
00007FF6043B6540 81 C3 45 23 01 00    add         ebx,12345h  
add rbx, 12345h
00007FF6043B6546 48 81 C3 45 23 01 00 add         rbx,12345h 
*/

u8*
emit_add_mi_unsigned(u8* stream, X64_Register dest, Int_Value value)
{
    int bitsize = register_get_bitsize(dest);
    u8 opcode = (bitsize == 8) ? ADD_8 : ADD;
    stream = emit_opcode(stream, opcode, register_get_bitsize(dest), dest, dest);
    *stream++ = make_modrm(DIRECT, 0, register_representation(dest));

    stream = emit_int_value(stream, MIN(32, register_get_bitsize(dest)), value);
    return stream;
}

u8*
emit_add_mi_signed_ext(u8* stream, X64_Register dest, s8 value)
{
    int bitsize = register_get_bitsize(dest);
    assert(bitsize > 8 && "add mi signed does not support 8 bits reg destination");
    stream = emit_opcode(stream, ADDS, register_get_bitsize(dest), dest, dest);
    *stream++ = make_modrm(INDIRECT, 0, register_representation(dest));

    *stream++ = value;
    return stream;
}

u8*
emit_arith_test(u8* stream)
{
#if 0
    stream = emit_add_mi(stream, RAX, (Int_Value){.v64 = 0x12345678});
    stream = emit_add_mi(stream, RBX, (Int_Value){.v64 = 0x12345678});
    stream = emit_add_mi(stream, EAX, (Int_Value){.v64 = 0x123});
    stream = emit_add_mi(stream, EBX, (Int_Value){.v64 = 0x123});
    stream = emit_add_mi(stream, BX, (Int_Value){.v16 = 0x123});
    stream = emit_add_mi(stream, AX, (Int_Value){.v64 = 0x123});
    stream = emit_add_mi(stream, AL, (Int_Value){.v64 = 0x12});
#endif
#if 1
    stream = emit_add_mi_signed_ext(stream, RBX, 0x12);
    stream = emit_add_mi_signed_ext(stream, EBX, 0x12);
    stream = emit_add_mi_signed_ext(stream, BX, 0x12);
#endif
    return stream;
}