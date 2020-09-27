#include "util.h"

u8*
emit_movs_reg_to_mem(Instr_Emit_Result* out_info, u8* stream, X64_Addressing_Mode mode, X64_XMM_Register src, X64_Register dst, bool single_precision, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

    assert(mode == INDIRECT || mode == INDIRECT_BYTE_DISPLACED || mode == INDIRECT_DWORD_DISPLACED);
    *stream++ = (single_precision) ? 0xf3 : 0xf2;
    *stream++ = 0x0f;
    *stream++ = 0x11;
    *stream++ = make_modrm(mode, src, register_representation(dst));

    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);
    if((stream - start) == disp_offset) disp_offset = -1;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = disp_offset;
        out_info->immediate_offset = -1;
    }
    return stream;
}

u8*
emit_movs_mem_to_reg(Instr_Emit_Result* out_info, u8* stream, X64_Addressing_Mode mode, X64_XMM_Register dest, X64_Register src, bool single_precision, u8 disp8, uint32_t disp32)
{
    u8* start = stream;
    s8 disp_offset = 0;

    assert(mode == INDIRECT || mode == INDIRECT_BYTE_DISPLACED || mode == INDIRECT_DWORD_DISPLACED);
    *stream++ = (single_precision) ? 0xf3 : 0xf2;
    *stream++ = 0x0f;
    *stream++ = 0x10;

    if(register_equivalent(src, RBP))
    {
        // emit sib byte
        *stream++ = make_sib(1, register_representation(dest), RBP);
        *stream++ = 0;
    }
    else if(register_equivalent(src, RSP))
    {
        *stream++ = make_modrm(mode, dest, register_representation(src));
        *stream++ = make_sib(0, RSP, RSP);
    }
    else
    {
        *stream++ = make_modrm(mode, dest, register_representation(src));
    }

    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);
    if((stream - start) == disp_offset) disp_offset = -1;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = disp_offset;
        out_info->immediate_offset = -1;
    }
    return stream;
}

u8*
emit_movs_ds_to_reg(Instr_Emit_Result* out_info, u8* stream, X64_XMM_Register dest, bool single_precision, uint32_t ds_offset)
{
    u8* start = stream;
    s8 disp_offset = 0;

    *stream++ = (single_precision) ? 0xf3 : 0xf2;
    *stream++ = 0x0f;
    *stream++ = 0x10;
    *stream++ = 0x05;
    disp_offset = stream - start;
    *(uint32_t*)stream = ds_offset;
    stream += sizeof(uint32_t);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->diplacement_offset = disp_offset;
        out_info->immediate_offset = -1;
    }

    return stream;
}

u8*
emit_movs_direct(Instr_Emit_Result* out_info, u8* stream, X64_XMM_Register dest, X64_XMM_Register src, bool single_precision)
{
    u8* start = stream;

    *stream++ = (single_precision) ? 0xf3 : 0xf2;
    *stream++ = 0x0f;
    *stream++ = 0x10;
    *stream++ = make_modrm(DIRECT, dest, src);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1;
        out_info->diplacement_offset = -1;
    }
    return stream;
}

u8*
emit_arith_sse(Instr_Emit_Result* out_info, u8* stream, X64_XMM_Arithmetic_Instr instr, X64_XMM_Register dest, X64_XMM_Register src, bool single_precision)
{
    u8* start = stream;

    *stream++ = (single_precision) ? 0xf3 : 0xf2;
    *stream++ = 0x0f;
    *stream++ = instr;
    *stream++ = make_modrm(DIRECT, dest, src);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1;
        out_info->diplacement_offset = -1;
    }
    return stream;
}

u8*
emit_cmp_sse(Instr_Emit_Result* out_info, u8* stream, X64_SSE_Compare_Flag flag, X64_XMM_Register r1, X64_XMM_Register r2)
{
    u8* start = stream;

    *stream++ = 0xf3;
    *stream++ = 0x0f;
    *stream++ = 0xc2;
    *stream++ = make_modrm(DIRECT, r1, r2);
    *stream++ = flag;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1;
        out_info->diplacement_offset = -1;
    }

    return stream;
}

u8*
emit_comiss_sse(Instr_Emit_Result* out_info, u8* stream, X64_XMM_Register r1, X64_XMM_Register r2)
{
    u8* start = stream;

    *stream++ = 0x0f;
    *stream++ = 0x2f;
    *stream++ = make_modrm(DIRECT, r1, r2);

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1;
        out_info->diplacement_offset = -1;
    }

    return stream;
}

u8*
emit_cvt(Instr_Emit_Result* out_info, u8* stream, X64_SSE_Convert_Instr instr, X64_Addressing_Mode mode, u8 src, u8 dest, u8 disp8, uint32_t disp32, bool single_prec)
{
    u8* start = stream;
    s8 disp_offset = 0;

    *stream++ = (single_prec) ? 0xf3 : 0xf2;
    *stream++ = 0x0f;
    *stream++ = instr;
    *stream++ = make_modrm(mode, register_representation(dest), register_representation(src));

    disp_offset = stream - start;
    stream = emit_displacement(mode, stream, disp8, disp32);
    if((stream - start) == disp_offset) disp_offset = -1;

    if(out_info)
    {
        out_info->instr_byte_size = stream - start;
        out_info->immediate_offset = -1;
        out_info->diplacement_offset = disp_offset;
    }
    return stream;
}

u8*
emit_float_test(u8* stream)
{
    bool single_prec = false;
#if 0
    stream = emit_cmp_sse(0, stream, SSE_CMP_EQ, XMM1, XMM2);
#endif
#if 0
    stream = emit_cvt(0, stream, SSE_CVT_F32_INT32, DIRECT, XMM3, EBX, 0, 0, single_prec);
    stream = emit_cvt(0, stream, SSE_CVT_F32_INT32, INDIRECT, XMM3, EBX, 0, 0, single_prec);

    stream = emit_cvt(0, stream, SSE_CVT_INT32_F32, DIRECT, XMM3, EBX, 0, 0, single_prec);
    stream = emit_cvt(0, stream, SSE_CVT_INT32_F32, INDIRECT, XMM3, EBX, 0, 0, single_prec);

    stream = emit_cvt(0, stream, SSE_CVT_F64_F32, DIRECT, XMM3, XMM4, 0, 0, single_prec);
    stream = emit_cvt(0, stream, SSE_CVT_F64_F32, INDIRECT, XMM3, EBX, 0, 0, single_prec);
#endif
#if 0
    stream = emit_movs_mem_to_reg(0, stream, INDIRECT, XMM7, RAX, single_prec, 0, 0);
    stream = emit_movs_reg_to_mem(0, stream, INDIRECT, XMM7, RAX, single_prec, 0, 0);
    stream = emit_movs_direct(0, stream, XMM7, XMM3, single_prec);
    stream = emit_movs_mem_to_reg(0, stream, INDIRECT, XMM0, RBX, single_prec, 0, 0);
    stream = emit_movs_mem_to_reg(0, stream, INDIRECT, XMM0, RCX, single_prec, 0, 0);
    stream = emit_movs_mem_to_reg(0, stream, INDIRECT, XMM0, RDX, single_prec, 0, 0);
    stream = emit_movs_mem_to_reg(0, stream, INDIRECT_BYTE_DISPLACED, XMM0, RBX, single_prec, 0x12, 0);
    stream = emit_movs_mem_to_reg(0, stream, INDIRECT_DWORD_DISPLACED, XMM0, RCX, single_prec, 0, 0x12345678);
    //stream = emit_movss(0, stream, RSP);
    stream = emit_movs_mem_to_reg(0, stream, INDIRECT, XMM1, RDI, single_prec, 0, 0);
    stream = emit_movs_mem_to_reg(0, stream, INDIRECT, XMM1, RSI, single_prec, 0, 0);
#endif
#if 0
    stream = emit_arith_sse(0, stream, XMM_ADDS, XMM1, XMM2, single_prec);
    stream = emit_arith_sse(0, stream, XMM_SUBS, XMM1, XMM2, single_prec);
    stream = emit_arith_sse(0, stream, XMM_MULS, XMM1, XMM2, single_prec);
    stream = emit_arith_sse(0, stream, XMM_DIVS, XMM1, XMM2, single_prec);
#endif
    stream = emit_movs_mem_to_reg(0, stream, INDIRECT, XMM7, EBX, single_prec, 0, 0);
    return stream;
}