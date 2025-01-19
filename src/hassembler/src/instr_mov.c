/*
• 2EH — CS segment override (use with any branch instruction is reserved).
• 36H — SS segment override prefix (use with any branch instruction is reserved).
• 3EH — DS segment override prefix (use with any branch instruction is reserved).
• 26H — ES segment override prefix (use with any branch instruction is reserved).
• 64H — FS segment override prefix (use with any branch instruction is reserved).
• 65H — GS segment override prefix (use with any branch instruction is reserved).
*/

// ------------
u8*
emit_mov(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    s8 extra_byte = 0;

    switch(amode.mode_type)
    {
        case ADDR_MODE_OI: {
            // TODO(psv): assert invalid cases here
            opcode.bytes[0] = ((bitsize > 8) ? 0xB8 : 0xB0) | register_representation(amode.rm);
        } break;
        case ADDR_MODE_MI: {
            assert(value_bitsize(amode.immediate) <= bitsize && amode.immediate_bitsize < 64);

            u8 op = 0xC6;               // imm8
            if(bitsize > 8) op += 1;    // 0xC7
            opcode.bytes[0] = op;

            if(amode.addr_mode == DIRECT && amode.immediate_bitsize == 16 && register_get_bitsize(amode.rm) > 16)
                amode.immediate_bitsize = 32;
            else if(amode.addr_mode != DIRECT && amode.immediate_bitsize == 16 && amode.ptr_bitsize > 16)
                amode.immediate_bitsize = 32;
            amode.reg = 0; // /0
        } break;
        case ADDR_MODE_MR: {
            opcode.bytes[0] = (bitsize > 8) ? 0x89 : 0x88;
            if(register_is_segment(amode.reg))
                opcode.bytes[0] = 0x8C;
            if (amode.addr_mode != DIRECT && amode.ptr_bitsize == 16)
                amode.ptr_bitsize = 32;
        } break;
        case ADDR_MODE_RM: {
            opcode.bytes[0] = (bitsize > 8) ? 0x8B : 0x8A;
            if(register_is_segment(amode.reg))
                opcode.bytes[0] = 0x8E;
            if (amode.addr_mode != DIRECT && amode.ptr_bitsize == 16)
                amode.ptr_bitsize = 32;
        } break;
        case ADDR_MODE_FD: {
            opcode.bytes[0] = (bitsize > 8) ? 0xA1 : 0xA0;
            if(amode.rm != REGISTER_NONE)
            {
                switch(amode.moffs_base)
                {
                    case CS: *stream++ = 0x2e; break;
                    case SS: *stream++ = 0x36; break;
                    case DS: *stream++ = 0x3e; break;
                    case ES: *stream++ = 0x26; break;
                    case FS: *stream++ = 0x64; break;
                    case GS: *stream++ = 0x65; break;
                    default: break;
                }
                extra_byte = 1;
            }
        } break;
        case ADDR_MODE_TD: {
            opcode.bytes[0] = (bitsize > 8) ? 0xA3 : 0xA2;
            if(amode.rm != REGISTER_NONE)
            {
                switch(amode.moffs_base)
                {
                    case CS: *stream++ = 0x2e; break;
                    case SS: *stream++ = 0x36; break;
                    case DS: *stream++ = 0x3e; break;
                    case ES: *stream++ = 0x26; break;
                    case FS: *stream++ = 0x64; break;
                    case GS: *stream++ = 0x65; break;
                    default: break;
                }
                extra_byte = 1;
            }
        } break;
    }

    u8* result = emit_instruction(out_info, stream, amode, opcode);
    if(out_info && extra_byte > 0)
    {
        out_info->instr_byte_size += extra_byte;
        out_info->immediate_offset += ((out_info->immediate_offset != -1) ? extra_byte : 0);
        out_info->diplacement_offset += ((out_info->diplacement_offset != -1) ? extra_byte : 0);
    }
    return result;
}

u8*
emit_cmovcc(Instr_Emit_Result* out_info, u8* stream, X64_CMOVcc_Instruction instr, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = (u8)instr;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_setcc(Instr_Emit_Result* out_info, u8* stream, X64_SETcc_Instruction instr, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT && amode.ptr_bitsize == ADDR_BYTEPTR);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = (u8)instr;
    amode.reg = RAX;
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_lar(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_RM);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = 0x02;

    assert((amode.addr_mode == DIRECT && register_get_bitsize(amode.reg) == register_get_bitsize(amode.rm)) ||
        (amode.addr_mode != DIRECT && register_get_bitsize(amode.rm) >= 32 && amode.ptr_bitsize == 16));
    amode.ptr_bitsize = register_get_bitsize(amode.reg);

    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_lsl(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_RM);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = 0x03;

    assert((amode.addr_mode == DIRECT && register_get_bitsize(amode.reg) == register_get_bitsize(amode.rm)) ||
        (amode.addr_mode != DIRECT && register_get_bitsize(amode.rm) >= 32 && amode.ptr_bitsize == 16));
    amode.ptr_bitsize = register_get_bitsize(amode.reg);

    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_mov_debug_reg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_MR || amode.mode_type == ADDR_MODE_RM);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    if(amode.mode_type == ADDR_MODE_MR)
        opcode.bytes[1] = 0x21;
    else if(amode.mode_type == ADDR_MODE_RM)
        opcode.bytes[1] = 0x23;

    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_mov_control_reg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_MR || amode.mode_type == ADDR_MODE_RM);

    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    if(amode.mode_type == ADDR_MODE_MR)
        opcode.bytes[1] = 0x20;
    else if(amode.mode_type == ADDR_MODE_RM)
        opcode.bytes[1] = 0x22;

    return emit_instruction(out_info, stream, amode, opcode);
}