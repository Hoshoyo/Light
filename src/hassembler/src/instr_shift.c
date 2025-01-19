u8*
emit_shift(Instr_Emit_Result* out_info, u8* stream, X64_Shift_Instruction instr, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    switch(amode.mode_type)
    {
        case ADDR_MODE_M1: {
            opcode.bytes[0] = (bitsize > 8) ? 0xD1 : 0xD0;
            amode.reg = instr;
        } break;
        case ADDR_MODE_MC: {
            opcode.bytes[0] = (bitsize > 8) ? 0xD3 : 0xD2;
            amode.reg = instr;
        } break;
        case ADDR_MODE_MI: {
            assert(amode.immediate_bitsize == 8);
            opcode.bytes[0] = (bitsize > 8) ? 0xC1 : 0xC0;
            amode.reg = instr;
        } break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_test(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;

    X64_Opcode opcode = {.byte_count = 1};
    switch(amode.mode_type)
    {
        case ADDR_MODE_I: {
            assert(amode.immediate_bitsize <= 32);
            assert(register_equivalent(amode.rm, RAX));
            opcode.bytes[0] = (bitsize > 8) ? 0xA9 : 0xA8;
        } break;
        case ADDR_MODE_MI: {
            assert(MIN(32, bitsize) == amode.immediate_bitsize);
            opcode.bytes[0] = (bitsize > 8) ? 0xF7 : 0xF6;
            amode.reg = 0;
        } break;
        case ADDR_MODE_MR: {
            opcode.bytes[0] = (bitsize > 8) ? 0x85 : 0x84;
        } break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}


u8*
emit_shld(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(((amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize) > 8);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    if(amode.mode_type == ADDR_MODE_MRI)
    {
        opcode.bytes[1] = 0xa4;
    }
    else if(amode.mode_type == ADDR_MODE_MRC)
    {
        opcode.bytes[1] = 0xa5;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_shrd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(((amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize) > 8);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    if(amode.mode_type == ADDR_MODE_MRI)
    {
        opcode.bytes[1] = 0xac;
    }
    else if(amode.mode_type == ADDR_MODE_MRC)
    {
        opcode.bytes[1] = 0xad;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}