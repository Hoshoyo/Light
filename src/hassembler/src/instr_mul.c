#define INC_DIGIT 0
#define DEC_DIGIT 1
#define NOT_DIGIT 2
#define NEG_DIGIT 3
#define MUL_DIGIT 4
#define DIV_DIGIT 6
#define IDIV_DIGIT 7
#define IMUL_DIGIT 5
#define NOP_DIGIT 0

u8*
emit_mul(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = MUL_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_div(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = DIV_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_idiv(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = IDIV_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_neg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = NEG_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_not(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    amode.reg = NOT_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_dec(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xfe : 0xff;
    amode.reg = DEC_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_inc(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xfe : 0xff;
    amode.reg = INC_DIGIT;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_nop(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    if(amode.mode_type == ADDR_MODE_ZO)
    {
        return emit_single_byte_instruction(out_info, stream, 0x90);
    }
    else
    {
        s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
        X64_Opcode opcode = {.byte_count = 2};
        opcode.bytes[0] = 0x0f;
        opcode.bytes[1] = 0x1f;
        amode.reg = NOP_DIGIT;
        return emit_instruction(out_info, stream, amode, opcode);
    }
}

u8*
emit_imul(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = (bitsize == 8) ? 0xf6 : 0xf7;
    
    if(amode.immediate_bitsize > 0)
    {
        assert((register_get_bitsize(amode.reg) > 8 && amode.immediate_bitsize == 8) || 
               (register_get_bitsize(amode.reg) == amode.immediate_bitsize) ||
               (register_get_bitsize(amode.reg) == 64 && amode.immediate_bitsize == 32));
        opcode.bytes[0] = (amode.immediate_bitsize == 8) ? 0x6B : 0x69;
    }
    else if(amode.reg != REGISTER_NONE)
    {
        assert(register_get_bitsize(amode.reg) > 8);

        opcode.bytes[0] = 0x0f;
        opcode.bytes[1] = 0xaf;
        opcode.byte_count = 2;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_movsxd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = 0x63;
    bool extended = register_is_extended(amode.rm);

    assert(register_get_bitsize(amode.reg) > 8);
    //assert((amode.addr_mode != DIRECT) || register_get_bitsize(amode.reg) > 32 && bitsize == 32);

    if(register_get_bitsize(amode.reg) == 64)
    {
        // We require a 32 ptrsize just to know that we are using a up-cast 32bit->64bit sign extended
        assert(amode.ptr_bitsize == 32 || amode.addr_mode == DIRECT);
        // override the 32, because the instruction encodes 32 by default since its an up-cast.
        if (amode.addr_mode == DIRECT)
            amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        else
            amode.ptr_bitsize = 64;
    }
    
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_movsx(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    bool extended = register_is_extended(amode.rm);

    if(bitsize == 16)
    {
        opcode.bytes[1] = 0xbf;
        if(register_get_bitsize(amode.reg) == 64)
        {
            if(amode.addr_mode != DIRECT)
            {
                amode.flags |= ADDRMODE_FLAG_REXW;
                amode.ptr_bitsize = register_get_bitsize(amode.rm); // just to ignore the 16 bit override prefix
            }
            else
                amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        }
        else if (register_get_bitsize(amode.reg) == 32)
            amode.ptr_bitsize = 32;
    }
    else
    {
        opcode.bytes[1] = 0xbe;
        if(register_get_bitsize(amode.reg) == 64)
        {
            if(amode.addr_mode != DIRECT)
                amode.flags |= ADDRMODE_FLAG_REXW;
            else
                amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        }
        else if(register_get_bitsize(amode.reg) == 16)
            amode.ptr_bitsize = 16;
    }

    assert(register_get_bitsize(amode.reg) > 8);

    return emit_instruction(out_info, stream, amode, opcode);
}

// TODO(psv): test this
u8*
emit_movzx(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    bool extended = register_is_extended(amode.rm);

    if(bitsize == 16)
    {
        opcode.bytes[1] = 0xb6;
        if(register_get_bitsize(amode.reg) == 64)
        {
            if(amode.addr_mode != DIRECT)
            {
                amode.flags |= ADDRMODE_FLAG_REXW;
                amode.ptr_bitsize = register_get_bitsize(amode.rm); // just to ignore the 16 bit override prefix
            }
            else
                amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        }
        else if (register_get_bitsize(amode.reg) == 32)
            amode.ptr_bitsize = 32;
    }
    else
    {
        opcode.bytes[1] = 0xb7;
        if(register_get_bitsize(amode.reg) == 64)
        {
            if(amode.addr_mode != DIRECT)
                amode.flags |= ADDRMODE_FLAG_REXW;
            else
                amode.rm = register_representation(amode.rm) + ((extended) ? R8 : 0);
        }
        else if(register_get_bitsize(amode.reg) == 16)
            amode.ptr_bitsize = 16;
    }

    assert(register_get_bitsize(amode.reg) > 8);

    return emit_instruction(out_info, stream, amode, opcode);
}