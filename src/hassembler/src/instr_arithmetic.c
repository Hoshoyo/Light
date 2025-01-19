#define AND_IMM8 0x80
#define AND_IMM 0x81
#define ADD_IMM8 0x80
#define ADD_IMM 0x81
#define ADDS_IMM8 0x83
#define ADD_MR 0x01
#define ADD_RM 0x03
#define ADD_AL 0x04
#define ADD_A 0x05
#define AND_AL 0x24
#define AND_A 0x25
#define OR_AL 0xc
#define OR_A 0xd
#define ADC_AL 0x14
#define ADC_A 0x15
#define SBB_AL 0x1c
#define SBB_A 0x1d
#define SUB_AL 0x2c
#define SUB_A 0x2d
#define XOR_AL 0x34
#define XOR_A 0x35
#define CMP_AL 0x3c
#define CMP_A 0x3d

#define ADD_MR8 0x00
#define CMP_MR8 0x38
#define AND_MR8 0x20
#define ADC_MR8 0x10
#define SBB_MR8 0x18
#define SUB_MR8 0x28
#define XOR_MR8 0x30
#define OR_MR8 0x08

#define ADD_RM8 0x02
#define CMP_RM8 0x3A
#define AND_RM8 0x22
#define ADC_RM8 0x12
#define SBB_RM8 0x1A
#define SUB_RM8 0x2A
#define XOR_RM8 0x32
#define OR_RM8 0x0A

static u8
mi_opcode_a(X64_Arithmetic_Instr instr, int bitsize)
{
    u8 opcode = 0;
    switch(instr)
    {
        case ARITH_AND: opcode = AND_AL; break;
        case ARITH_OR:  opcode = OR_AL; break;
        case ARITH_XOR: opcode = XOR_AL; break;
        case ARITH_ADD: opcode = ADD_AL; break;
        case ARITH_ADC: opcode = ADC_AL; break;
        case ARITH_SUB: opcode = SUB_AL; break;
        case ARITH_SBB: opcode = SBB_AL; break;
        case ARITH_CMP: opcode = CMP_AL; break;
        default: assert(0 && "invalid instruction"); break;
    }
    if(bitsize > 8) opcode += 1;
    return opcode;
}

static u8
mr_opcode(X64_Arithmetic_Instr instr, int bitsize)
{
    u8 opcode = 0;
    switch(instr)
    {
        case ARITH_AND: opcode = AND_MR8; break;
        case ARITH_OR:  opcode = OR_MR8; break;
        case ARITH_XOR: opcode = XOR_MR8; break;
        case ARITH_ADD: opcode = ADD_MR8; break;
        case ARITH_ADC: opcode = ADC_MR8; break;
        case ARITH_SUB: opcode = SUB_MR8; break;
        case ARITH_SBB: opcode = SBB_MR8; break;
        case ARITH_CMP: opcode = CMP_MR8; break;
        default: assert(0 && "invalid instruction"); break;
    }
    if(bitsize > 8) opcode += 1;
    return opcode;
}

static u8
rm_opcode(X64_Arithmetic_Instr instr, int bitsize)
{
    u8 opcode = 0;
    switch(instr)
    {
        case ARITH_AND: opcode = AND_RM8; break;
        case ARITH_OR:  opcode = OR_RM8; break;
        case ARITH_XOR: opcode = XOR_RM8; break;
        case ARITH_ADD: opcode = ADD_RM8; break;
        case ARITH_ADC: opcode = ADC_RM8; break;
        case ARITH_SUB: opcode = SUB_RM8; break;
        case ARITH_SBB: opcode = SBB_RM8; break;
        case ARITH_CMP: opcode = CMP_RM8; break;
        default: assert(0 && "invalid instruction"); break;
    }
    if(bitsize > 8) opcode += 1;
    return opcode;
}

u8*
emit_arithmetic(Instr_Emit_Result* out_info, u8* stream, X64_Arithmetic_Instr instr, X64_AddrMode amode)
{
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 1};
    if(amode.immediate_bitsize > 0)
    {
        u8 op = 0x80; // imm8
        if(bitsize > 8) op += 1;    // 0x81
        if(amode.immediate_bitsize == 8 && bitsize > 8) op += 2;   // 0x83
        opcode.bytes[0] = op;

        if(amode.addr_mode == DIRECT && amode.immediate_bitsize == 16 && register_get_bitsize(amode.rm) > 16)
            amode.immediate_bitsize = 32;
        else if(amode.addr_mode != DIRECT && amode.immediate_bitsize == 16 && amode.ptr_bitsize > 16)
            amode.immediate_bitsize = 32;
        amode.reg = instr;
    }
    else if(amode.mode_type == ADDR_MODE_RM)
    {
        opcode.bytes[0] = rm_opcode(instr, bitsize);
    }
    else if(amode.mode_type == ADDR_MODE_MR)
    {
        opcode.bytes[0] = mr_opcode(instr, bitsize);
    }
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_lea(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 1};
    opcode.bytes[0] = 0x8d;
    assert(register_get_bitsize(amode.reg) > 8);
    assert(amode.addr_mode != DIRECT);
    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_xadd(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    assert(amode.mode_type == ADDR_MODE_MR);
    s32 bitsize = (amode.addr_mode == DIRECT) ? register_get_bitsize(amode.rm) : amode.ptr_bitsize;
    X64_Opcode opcode = {.byte_count = 2};
    opcode.bytes[0] = 0x0f;
    opcode.bytes[1] = (bitsize == 8) ? 0xc0 : 0xc1;

    return emit_instruction(out_info, stream, amode, opcode);
}

u8*
emit_xchg(Instr_Emit_Result* out_info, u8* stream, X64_AddrMode amode)
{
    X64_Opcode opcode = {.byte_count = 1};
    if(amode.mode_type == ADDR_MODE_O)
    {    
        assert(register_get_bitsize(amode.reg) > 8);
        u8* start = stream;
        if(amode.addr_mode == DIRECT) amode.rm = amode.reg;
        // size override
        stream = emit_size_override(stream, amode.ptr_bitsize, amode.rm, amode.addr_mode);

        // rex
        if(register_is_extended(amode.reg) || register_get_bitsize(amode.reg) == 64)
            *stream++ = make_rex(register_is_extended(amode.reg), 0, 0, register_get_bitsize(amode.reg) == 64);

        // opcode
        *stream++ = 0x90 | (0x7 & amode.reg);
        
        fill_outinfo(out_info, (s8)(stream - start), -1, -1);
        return stream;
    }
    else if(amode.mode_type == ADDR_MODE_RM || amode.mode_type == ADDR_MODE_MR)
    {
        opcode.bytes[0] = (register_get_bitsize(amode.reg) == 8) ? 0x86 : 0x87;
    }
    return emit_instruction(out_info, stream, amode, opcode);
}