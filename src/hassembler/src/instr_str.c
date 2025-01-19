u8*
emit_strinstr(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize, u8 op)
{
    u8* start = stream;
    switch(ptr_bitsize)
    {
        case 8:  *stream++ = op; break;
        case 16: {
            *stream++ = 0x66;
            *stream++ = op;
        } break;
        case 32: *stream++ = op; break;
        case 64: {
            *stream++ = make_rex(0, 0, 0, 1);
            *stream++ = op;
        } break;
        default: assert(0); break;
    }
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

#define CMPS 0xa7
#define CMPS8 0xa6
u8*
emit_cmps(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? CMPS8 : CMPS);
}

#define SCAS 0xaf
#define SCAS8 0xae
u8*
emit_scas(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? SCAS8 : SCAS);
}

#define STOS 0xab
#define STOS8 0xaa
u8*
emit_stos(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? STOS8 : STOS);
}

#define LODS 0xad
#define LODS8 0xac
u8*
emit_lods(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? LODS8 : LODS);
}

#define MOVS 0xa5
#define MOVS8 0xa4
u8*
emit_movs(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    return emit_strinstr(out_info, stream, ptr_bitsize, (ptr_bitsize == 8) ? MOVS8 : MOVS);
}

u8*
emit_cmpxchg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    assert(amode.mode_type == ADDR_MODE_MR);
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = (bitsize == 8) ? 0xb0 : 0xb1;
    return emit_instruction(out_info, stream, amode, opcode);
}

#define INS 0x6d
#define INS8 0x6c
u8*
emit_ins(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    u8* start = stream;
    switch(ptr_bitsize)
    {
        case 8:  *stream++ = INS8; break;
        case 16: {
            *stream++ = 0x66;
            *stream++ = INS;
        } break;
        case 32: *stream++ = INS; break;
        default: assert(0); break;
    }
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}

#define OUTS 0x6f
#define OUTS8 0x6e
u8*
emit_outs(Instr_Emit_Result* out_info, u8* stream, X64_AddrSize ptr_bitsize)
{
    u8* start = stream;
    switch(ptr_bitsize)
    {
        case 8:  *stream++ = OUTS8; break;
        case 16: {
            *stream++ = 0x66;
            *stream++ = OUTS;
        } break;
        case 32: *stream++ = OUTS; break;
        default: assert(0); break;
    }
    fill_outinfo(out_info, (s8)(stream - start), -1, -1);
    return stream;
}