u8*
emit_p3(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, u8 prefix, u8 op)
{
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = op;
    return emit_instruction_prefixed(out_info, stream, amode, opcode, prefix);
}

u8*
emit_p2(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, u8 op)
{
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = op;
    return emit_instruction(out_info, stream, amode, opcode);
}

// ----------------------------------------------------------------------
// ------------------------------ SSE -----------------------------------
u8* 
emit_addps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x58);
}

u8* 
emit_addss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x58);
}

u8* 
emit_subps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x5c);
}

u8* 
emit_subss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x5c);
}

u8* 
emit_andps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x54);
}

u8* 
emit_orps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x56);
}

u8* 
emit_xorps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x57);
}

u8* 
emit_andnotps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x55);
}

u8* 
emit_divps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x5e);
}

u8* 
emit_divss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x5e);
}

u8* 
emit_mulps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x59);
}

u8* 
emit_mulss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x59);
}

u8* 
emit_cmpps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    amode.immediate_bitsize = 8;
    amode.immediate = cmp_predicate;
    return emit_p2(out_info, stream, amode, 0xC2);
}

u8* 
emit_cmpss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    amode.immediate_bitsize = 8;
    amode.immediate = cmp_predicate;
    return emit_p3(out_info, stream, amode, 0xf3, 0xC2);
}

u8* 
emit_comiss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2f);
}

u8* 
emit_ucomiss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2e);
}

u8* 
emit_cvtpi2ps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2a);
}

u8* 
emit_cvtps2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2d);
}

u8* 
emit_cvttps2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x2c);
}

u8* 
emit_cvtsi2ss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf3, 0x2a);
}

u8* 
emit_cvtss2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf3, 0x2d);
}

u8* 
emit_cvttss2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf3, 0x2c);
}

// Load
u8* 
emit_movaps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    switch(amode.mode_type)
    {
        case ADDR_MODE_A:
            return emit_p2(out_info, stream, amode, 0x28);
        case ADDR_MODE_B:
            return emit_p2(out_info, stream, amode, 0x29);
        default: break;
    }
    return stream;
}

u8* 
emit_movups(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x10);
}

u8* 
emit_movhps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x16);
}

u8* 
emit_movlps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x12);
}

u8* 
emit_movss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x10);
}

// ----------------------------------------------------------------------
// ------------------------------ SSE2 ----------------------------------

u8* 
emit_addpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x58);
}

u8* 
emit_addsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x58);
}

u8* 
emit_subpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x5c);
}

u8* 
emit_subsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x5c);
}

u8* 
emit_andpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x54);
}

u8* 
emit_orpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x56);
}

u8* 
emit_xorpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x57);
}

u8* 
emit_andnotpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x55);
}

u8* 
emit_divpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x5e);
}

u8* 
emit_divsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x5e);
}

u8* 
emit_mulpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x59);
}

u8* 
emit_mulsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x59);
}

u8* 
emit_cmppd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    amode.immediate_bitsize = 8;
    amode.immediate = cmp_predicate;
    return emit_p3(out_info, stream, amode, 0x66, 0xC2);
}

u8* 
emit_cmpsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode, X64_SSE_Compare_Flag cmp_predicate)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    amode.immediate_bitsize = 8;
    amode.immediate = cmp_predicate;
    return emit_p3(out_info, stream, amode, 0xf2, 0xC2);
}

u8* 
emit_comisd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2f);
}

u8* 
emit_ucomisd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2e);
}

u8* 
emit_cvtpi2pd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2a);
}

u8* 
emit_cvtpd2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2d);
}

u8* 
emit_cvtss2sd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x5a);
}

u8* 
emit_cvtsd2ss(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0x5a);
}

u8* 
emit_cvtdq2pd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0xe6);
}

u8* 
emit_cvtdq2ps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x5b);
}

u8* 
emit_cvtpd2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf2, 0xe6);
}

u8* 
emit_cvtpd2ps(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x5a);
}

u8* 
emit_cvtps2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x5b);
}

u8* 
emit_cvtps2pd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p2(out_info, stream, amode, 0x5a);
}

u8* 
emit_cvtsd2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf2, 0x2d);
}

u8* 
emit_cvtsi2sd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf2, 0x2a);
}

u8* 
emit_cvttpd2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0xe6);
}

u8* 
emit_cvttpd2pi(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0x66, 0x2c);
}

u8* 
emit_cvttps2dq(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    amode.flags |= ADDRMODE_FLAG_NO_REXW;
    return emit_p3(out_info, stream, amode, 0xf3, 0x5b);
}

u8* 
emit_cvttsd2si(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf2, 0x2c);
}

// Load
u8* 
emit_movapd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    switch(amode.mode_type)
    {
        case ADDR_MODE_A:
            return emit_p3(out_info, stream, amode, 0x66, 0x28);
        case ADDR_MODE_B:
            return emit_p3(out_info, stream, amode, 0x66, 0x29);
        default: break;
    }
    return stream;
}

u8* 
emit_movupd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0x66, 0x10);
}

u8* 
emit_movhpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    return emit_p3(out_info, stream, amode, 0x66, 0x16);
}

u8* 
emit_movlpd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.addr_mode != DIRECT);
    return emit_p3(out_info, stream, amode, 0x66, 0x12);
}

u8* 
emit_movsd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    return emit_p3(out_info, stream, amode, 0xf2, 0x10);
}