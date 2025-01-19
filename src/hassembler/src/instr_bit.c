u8*
emit_bsf(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    assert(bitsize > 8);

    X64_Opcode opcode = {0};
    switch(amode.mode_type)
    {
        case ADDR_MODE_RM: {
            opcode.byte_count = 2;
            opcode.bytes[0] = 0x0f;
            opcode.bytes[1] = 0xbc;
        } break;
        default: assert(0); break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_bsr(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    assert(bitsize > 8);

    X64_Opcode opcode = {0};
    switch(amode.mode_type)
    {
        case ADDR_MODE_RM: {
            opcode.byte_count = 2;
            opcode.bytes[0] = 0x0f;
            opcode.bytes[1] = 0xbd;
        } break;
        default: assert(0); break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_bswap(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_O);
    
    u8* start = stream;
    if(register_is_extended(amode.reg) || register_get_bitsize(amode.reg) == 64)
        *stream++ = make_rex(register_is_extended(amode.reg), 0, 0, register_get_bitsize(amode.reg) == 64);

    // opcode
    *stream++ = 0x0f;
    *stream++ = 0xc8 | (0x7 & amode.reg);
    
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

static u8 bittest_opcode_mr(X64_BitTest_Instr instr)
{
    switch(instr)
    {
        case BITTEST:               return 0xa3;
        case BITTEST_COMPLEMENT:    return 0xbb;
        case BITTEST_RESET:         return 0xb3;
        case BITTEST_SET:           return 0xab;
        default: assert(0); return 0;
    }
}

u8*
emit_bt(Instr_Emit_Result* out_info, u8* stream, X64_BitTest_Instr instr, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    assert(bitsize > 8);

    X64_Opcode opcode = {0};
    switch(amode.mode_type)
    {
        case ADDR_MODE_MR: {
            opcode.byte_count = 2;
            opcode.bytes[0] = 0x0f;
            opcode.bytes[1] = bittest_opcode_mr(instr);
        } break;
        case ADDR_MODE_MI: {
            opcode.byte_count = 2;
            opcode.bytes[0] = 0x0f;
            opcode.bytes[1] = 0xba;
            amode.reg = instr;
            assert(amode.immediate_bitsize == 8);
        } break;
        default: assert(0); break;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}