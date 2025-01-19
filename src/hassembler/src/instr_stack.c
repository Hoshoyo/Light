u8*
emit_int3(Instr_Emit_Result* out_info, u8* stream)
{
    return emit_single_byte_instruction(out_info, stream, 0xcc);
}

u8*
emit_int1(Instr_Emit_Result* out_info, u8* stream)
{
    return emit_single_byte_instruction(out_info, stream, 0xf1);
}

u8*
emit_int(Instr_Emit_Result* out_info, u8* stream, u8 rel)
{
    *stream++ = 0xcd;
    *stream++ = rel;
    fill_outinfo(out_info, 2, -1, 1);
    return stream;
}

u8*
emit_leave(Instr_Emit_Result* out_info, u8* stream)
{
    return emit_single_byte_instruction(out_info, stream, 0xc9);
}

u8*
emit_leave16(Instr_Emit_Result* out_info, u8* stream)
{
    *stream++ = 0x66;
    *stream++ = 0xc9;
    fill_outinfo(out_info, 2, -1, -1);
    return stream;
}

u8*
emit_ret(Instr_Emit_Result* out_info, u8* stream, X64_Ret_Instruction ret, u16 imm)
{
    if(imm == 0) 
    {
        switch(ret)
        {
            case RET_NEAR: {
                *stream++ = 0xc3;
            } break;
            case RET_FAR: {
                *stream++ = 0xcb;
            } break;            
        }
        fill_outinfo(out_info, 1, -1, -1);
    }
    else
    {
        switch(ret)
        {
            case RET_NEAR: {
                *stream++ = 0xc2;
                *(uint16_t*)stream = imm;
                stream += sizeof(uint16_t);
            } break;
            case RET_FAR: {
                *stream++ = 0xca;
                *(uint16_t*)stream = imm;
                stream += sizeof(uint16_t);
            } break;
        }
        fill_outinfo(out_info, 3, -1, 1);
    }
    return stream;
}

#define PUSH_DIGIT 6
#define POP_DIGIT 0

u8*
emit_push(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    u8* start = stream;
    switch(amode.mode_type)
    {
        case ADDR_MODE_I: {
            s8 imm_offset = 1;
            switch(amode.immediate_bitsize)
            {
                case 8: {
                    *stream++ = 0x6A;
                } break;
                case 16: {
                    *stream++ = 0x66;
                    imm_offset = 2;
                } 
                case 32: {
                    *stream++ =0x68;
                } break;
            }
            stream = emit_value_raw(stream, amode.immediate, amode.immediate_bitsize);
            fill_outinfo(out_info, stream - start, -1, imm_offset);
        } break;
        case ADDR_MODE_ZO: {
            assert(amode.reg == FS || amode.reg == GS);

            *stream++ = 0x0F;
            if(amode.reg == FS)
                *stream++ = 0xA0;
            else if(amode.reg == GS)
                *stream++ = 0xA8;
            fill_outinfo(out_info, stream - start, -1, -1);
        } break;
        case ADDR_MODE_O: {
            assert(register_get_bitsize(amode.reg) == 16 || register_get_bitsize(amode.reg) == 64);
            
            if(register_get_bitsize(amode.reg) == 16)
                *stream++ = 0x66;
            if(register_is_extended(amode.reg))
                *stream++ = make_rex(1, 0, 0, 0);
            *stream++ = 0x50 | (0x7 & register_representation(amode.reg));            
            fill_outinfo(out_info, stream - start, -1, -1);
        } break;
        case ADDR_MODE_M: {
            s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
            assert(bitsize == 16 || bitsize == 64);
            X64_Opcode opcode = {.byte_count = 1};
            opcode.bytes[0] = 0xff;
            amode.reg = PUSH_DIGIT;
            if(bitsize == 64)
                amode.flags |= ADDRMODE_FLAG_NO_REXW;

            stream = emit_instruction(out_info, stream, amode, opcode);
        } break;
    }
    return stream;
}

u8* 
emit_pop(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    u8* start = stream;
    switch(amode.mode_type)
    {
        case ADDR_MODE_ZO: {
            assert(amode.reg == FS || amode.reg == GS);

            if(amode.ptr_bitsize == 16)
                *stream++ = 0x66;

            *stream++ = 0x0F;
            if(amode.reg == FS)
                *stream++ = 0xA1;
            else if(amode.reg == GS)
                *stream++ = 0xA9;
            fill_outinfo(out_info, stream - start, -1, -1);
        } break;
        case ADDR_MODE_O: {
            assert(register_get_bitsize(amode.reg) == 16 || register_get_bitsize(amode.reg) == 64);
            
            if(register_get_bitsize(amode.reg) == 16)
                *stream++ = 0x66;
            if(register_is_extended(amode.reg))
                *stream++ = make_rex(1, 0, 0, 0);
            *stream++ = 0x58 | (0x7 & register_representation(amode.reg));            
            fill_outinfo(out_info, stream - start, -1, -1);
        } break;
        case ADDR_MODE_M: {
            s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
            assert(bitsize == 16 || bitsize == 64);
            X64_Opcode opcode = {.byte_count = 1};
            opcode.bytes[0] = 0x8f;
            amode.reg = POP_DIGIT;
            if(bitsize == 64)
                amode.flags |= ADDRMODE_FLAG_NO_REXW;

            stream = emit_instruction(out_info, stream, amode, opcode);
        } break;
    }
    return stream;
}

u8* 
emit_enter(Instr_Emit_Result* out_info, u8* stream, u16 storage_size, u8 lex_nest_level, bool b16)
{
    u8* start = stream;
    if(b16) *stream++ = 0x66;
    *stream++ = 0xc8;
    s8 imm_offset = (s8)(stream - start);
    *((u16*)stream) = storage_size;
    stream += sizeof(u16);
    *stream++ = lex_nest_level;
    fill_outinfo(out_info, (s8)(stream - start), -1, imm_offset);
    return stream;
}

u8*
emit_in(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 1};
    if(amode.mode_type == ADDR_MODE_I)
    {
        assert(amode.immediate_bitsize == 8);
        opcode.bytes[0] = (register_get_bitsize(amode.rm) == 8) ? 0xe4 : 0xe5;
        assert(amode.rm == REGISTER_NONE || register_equivalent(RAX, amode.rm));
    }
    else if(amode.mode_type == ADDR_MODE_ZO)
    {
        opcode.bytes[0] = (amode.ptr_bitsize == 8) ? 0xec : 0xed;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_out(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 1};
    if(amode.mode_type == ADDR_MODE_I)
    {
        assert(amode.immediate_bitsize == 8);
        opcode.bytes[0] = (register_get_bitsize(amode.rm) == 8) ? 0xe6 : 0xe7;
        assert(amode.rm == REGISTER_NONE || register_equivalent(RAX, amode.rm));
    }
    else if(amode.mode_type == ADDR_MODE_ZO)
    {
        opcode.bytes[0] = (amode.ptr_bitsize == 8) ? 0xee : 0xef;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}