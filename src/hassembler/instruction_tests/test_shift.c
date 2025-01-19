#define HO_ASSEMBLER_IMPLEMENT
#include "hoasm.h"
#include <stdio.h>
#include <stdlib.h>

X64_Shift_Instruction sinstr = SHR;

#define TEST_M1_DIRECT_8  1
#define TEST_M1_DIRECT_16 1
#define TEST_M1_DIRECT_32 1
#define TEST_M1_DIRECT_64 1

#define TEST_MRI_DIRECT 1
#define TEST_MRI_INDIRECT 1
#define TEST_MRC_DIRECT 1
#define TEST_MRC_INDIRECT 1

#define TEST_M1_INDIRECT 1
#define TEST_M1_INDIRECT_BYTE_DISPLACED 1
#define TEST_M1_INDIRECT_DWORD_DISPLACED 1

#define TEST_M1_SIB_X1_64 1
#define TEST_M1_SIB_X2_32 1
#define TEST_M1_SIB_X1_16 1
#define TEST_M1_SIB_X1_8 1
#define TEST_M1_SIB_X4_BYTE_DISPLACED_32 1
#define TEST_M1_SIB_X8_DWORD_DISPLACED_16 1

#define TEST_MC_DIRECT_8 1
#define TEST_MC_DIRECT_16 1
#define TEST_MC_DIRECT_32 1
#define TEST_MC_DIRECT_64 1

#define TEST_MC_INDIRECT 1
#define TEST_MC_INDIRECT_BYTE_DISPLACED 1
#define TEST_MC_INDIRECT_DWORD_DISPLACED 1

#define TEST_MC_SIB_X1_64 1
#define TEST_MC_SIB_X2_64 1
#define TEST_MC_SIB_X1_16 1
#define TEST_MC_SIB_X1_8 1
#define TEST_MC_SIB_X4_BYTE_DISPLACED_32 1
#define TEST_MC_SIB_X8_DWORD_DISPLACED_16 1

#define TEST_MI_DIRECT_8 1
#define TEST_MI_DIRECT_16 1
#define TEST_MI_DIRECT_32 1
#define TEST_MI_DIRECT_64 1
#define TEST_MI_INDIRECT 1
#define TEST_MI_INDIRECT_BYTE_DISPLACED 1
#define TEST_MI_INDIRECT_DWORD_DISPLACED 1

#define TEST_MI_SIB_X1_64 1
#define TEST_MI_SIB_X1_16 1
#define TEST_MI_SIB_X1_8 1
#define TEST_MI_SIB_X2_BYTE_DISPLACED_64 1
#define TEST_MI_SIB_X4_BYTE_DISPLACED_32 1
#define TEST_MI_SIB_X8_DWORD_DISPLACED_16 1

// MI
#define TEST_MI_DIRECT 1
#define TEST_MI_INDIRECT 1
#define TEST_MI_INDIRECT_BYTE_DISPLACED 1
#define TEST_MI_INDIRECT_DWORD_DISPLACED 1
#define TEST_MI_SIB_X1_IMM8 1
#define TEST_MI_SIB_X1_IMM16 1
#define TEST_MI_SIB_X1_IMM32 1
#define TEST_MI_SIB_X2_IMM8 1
#define TEST_MI_SIB_X2_IMM16 1
#define TEST_MI_SIB_X2_IMM32 1
#define TEST_MI_SIB_X4_BYTE_DISPLACED_IMM8 1
#define TEST_MI_SIB_X4_BYTE_DISPLACED_IMM16 1
#define TEST_MI_SIB_X4_BYTE_DISPLACED_IMM32 1
#define TEST_MI_SIB_X8_DWORD_DISPLACED_IMM8 1
#define TEST_MI_SIB_X8_DWORD_DISPLACED_IMM16 1
#define TEST_MI_SIB_X8_DWORD_DISPLACED_IMM32 1

// MR
#define TEST_MR_DIRECT_64 1
#define TEST_MR_DIRECT_32 1
#define TEST_MR_DIRECT_16 1
#define TEST_MR_DIRECT_8  1
#define TEST_MR_INDIRECT_64 1
#define TEST_MR_INDIRECT_32 1
#define TEST_MR_INDIRECT_16 1
#define TEST_MR_INDIRECT_8  1
#define TEST_MR_INDIRECT_BYTE_DISPLACED_64 1
#define TEST_MR_INDIRECT_BYTE_DISPLACED_32 1
#define TEST_MR_INDIRECT_BYTE_DISPLACED_16 1
#define TEST_MR_INDIRECT_BYTE_DISPLACED_8  1
#define TEST_MR_INDIRECT_DWORD_DISPLACED_64 1
#define TEST_MR_INDIRECT_DWORD_DISPLACED_32 1
#define TEST_MR_INDIRECT_DWORD_DISPLACED_16 1
#define TEST_MR_INDIRECT_DWORD_DISPLACED_8  1
#define TEST_MR_SIB_X1_64 1
#define TEST_MR_SIB_X1_32 1
#define TEST_MR_SIB_X1_16 1
#define TEST_MR_SIB_X1_8  1
#define TEST_MR_SIB_BYTE_DISPLACED_X2_64 1
#define TEST_MR_SIB_BYTE_DISPLACED_X2_32 1
#define TEST_MR_SIB_BYTE_DISPLACED_X2_16 1
#define TEST_MR_SIB_BYTE_DISPLACED_X2_8  1
#define TEST_MR_SIB_DWORD_DISPLACED_X8_64 1
#define TEST_MR_SIB_DWORD_DISPLACED_X8_32 1
#define TEST_MR_SIB_DWORD_DISPLACED_X8_16 1
#define TEST_MR_SIB_DWORD_DISPLACED_X8_8  1

#define TEST_I_DIRECT 1

u8*
emit_shift_m1_test(u8* stream)
{
    // DIRECT
#if TEST_M1_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_m1_direct(i));
    }
#endif
#if TEST_M1_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_m1_direct(i));
    }
#endif
#if TEST_M1_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_m1_direct(i));
    }
#endif
#if TEST_M1_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_m1_direct(i));
    }
#endif

    // INDIRECT
#if TEST_M1_INDIRECT
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0, ADDR_BYTEPTR));
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0, ADDR_WORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0, ADDR_DWORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0, ADDR_QWORDPTR));
    }
#endif

    // INDIRECT BYTE DISPLACED
#if TEST_M1_INDIRECT_BYTE_DISPLACED
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0x15, ADDR_BYTEPTR));
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0x15, ADDR_WORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0x15, ADDR_DWORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0x15, ADDR_QWORDPTR));
    }
#endif

    // INDIRECT DWORD DISPLACED
#if TEST_M1_INDIRECT_DWORD_DISPLACED
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0x15161718, ADDR_BYTEPTR));
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0x15161718, ADDR_WORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0x15161718, ADDR_DWORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_m1_indirect(i, 0x15161718, ADDR_QWORDPTR));
    }
#endif
    
    return stream;
}

u8*
emit_shift_m1_test_sib(u8* stream)
{
#if TEST_M1_SIB_X1_64
    X64_Register index_reg = RCX;
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_m1_indirect_sib(i, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
    index_reg = RBP;
#if TEST_M1_SIB_X2_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_m1_indirect_sib(i, index_reg, SIB_X2, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_M1_SIB_X4_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_m1_indirect_sib(i, index_reg, SIB_X4, 0x16, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_M1_SIB_X8_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_m1_indirect_sib(i, index_reg, SIB_X8, 0x16171819, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_M1_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_m1_indirect_sib(i, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_M1_SIB_X1_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_m1_indirect_sib(i, index_reg, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_shift_mc_test(u8* stream)
{
    // DIRECT
#if TEST_MC_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mc_direct(i));
    }
#endif
#if TEST_MC_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mc_direct(i));
    }
#endif
#if TEST_MC_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mc_direct(i));
    }
#endif
#if TEST_MC_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mc_direct(i));
    }
#endif

    // INDIRECT
#if TEST_MC_INDIRECT
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0, ADDR_BYTEPTR));
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0, ADDR_WORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0, ADDR_DWORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0, ADDR_QWORDPTR));
    }
#endif

    // INDIRECT BYTE DISPLACED
#if TEST_MC_INDIRECT_BYTE_DISPLACED
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0x15, ADDR_BYTEPTR));
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0x15, ADDR_WORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0x15, ADDR_DWORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0x15, ADDR_QWORDPTR));
    }
#endif

    // INDIRECT DWORD DISPLACED
#if TEST_MC_INDIRECT_DWORD_DISPLACED
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0x15161718, ADDR_BYTEPTR));
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0x15161718, ADDR_WORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0x15161718, ADDR_DWORDPTR));
        stream = emit_shift(0, stream, sinstr, mk_mc_indirect(i, 0x15161718, ADDR_QWORDPTR));
    }
#endif
    
    return stream;
}

u8*
emit_shift_mc_test_sib(u8* stream)
{
    X64_Register index_reg = RCX;
#if TEST_MC_SIB_X1_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mc_indirect_sib(i, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
    index_reg = RBP;
#if TEST_MC_SIB_X2_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mc_indirect_sib(i, index_reg, SIB_X2, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MC_SIB_X4_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mc_indirect_sib(i, index_reg, SIB_X4, 0x16, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MC_SIB_X8_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mc_indirect_sib(i, index_reg, SIB_X8, 0x151617, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MC_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mc_indirect_sib(i, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MC_SIB_X1_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mc_indirect_sib(i, index_reg, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_shift_mi_test(u8* stream)
{
    // DIRECT
#if TEST_MI_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mi_direct(i, 0x15, 8));
    }
#endif
#if TEST_MI_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mi_direct(i, 0x15, 8));
    }
#endif
#if TEST_MI_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mi_direct(i, 0x15, 8));
    }
#endif
#if TEST_MI_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mi_direct(i, 0x15, 8));
    }
#endif

    // INDIRECT
#if TEST_MI_INDIRECT
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0, ADDR_BYTEPTR, 0x15, 8));
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0, ADDR_WORDPTR, 0x15, 8));
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0, ADDR_DWORDPTR, 0x15, 8));
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0, ADDR_QWORDPTR, 0x15, 8));
    }
#endif

    // INDIRECT BYTE DISPLACED
#if TEST_MI_INDIRECT_BYTE_DISPLACED
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0x17, ADDR_BYTEPTR, 0x15, 8));
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0x17, ADDR_WORDPTR, 0x15, 8));
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0x17, ADDR_DWORDPTR, 0x15, 8));
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0x17, ADDR_QWORDPTR, 0x15, 8));
    }
#endif

    // INDIRECT DWORD DISPLACED
#if TEST_MI_INDIRECT_DWORD_DISPLACED
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0x17181920, ADDR_BYTEPTR, 0x15, 8));
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0x17181920, ADDR_WORDPTR, 0x15, 8));
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0x17181920, ADDR_DWORDPTR, 0x15, 8));
        stream = emit_shift(0, stream, sinstr, mk_mi_indirect(i, 0x17181920, ADDR_QWORDPTR, 0x15, 8));
    }
#endif

    return stream;
}

u8*
emit_shift_mi_test_sib(u8* stream)
{
    X64_Register index_reg = RCX;
#if TEST_MI_SIB_X1_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mi_indirect_sib(i, index_reg, SIB_X1, 0, ADDR_QWORDPTR, 0x15, 8));
        }
    }
#endif
    index_reg = RBP;
#if TEST_MI_SIB_X2_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mi_indirect_sib(i, index_reg, SIB_X2, 0x17, ADDR_QWORDPTR, 0x15, 8));
        }
    }
#endif
#if TEST_MI_SIB_X4_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mi_indirect_sib(i, index_reg, SIB_X4, 0x17, ADDR_DWORDPTR, 0x15, 8));
        }
    }
#endif
#if TEST_MI_SIB_X8_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mi_indirect_sib(i, index_reg, SIB_X8, 0x17181920, ADDR_WORDPTR, 0x15, 8));
        }
    }
#endif
#if TEST_MI_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mi_indirect_sib(i, index_reg, SIB_X1, 0, ADDR_WORDPTR, 0x15, 8));
        }
    }
#endif
#if TEST_MI_SIB_X1_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_shift(0, stream, sinstr, mk_mi_indirect_sib(i, index_reg, SIB_X1, 0, ADDR_BYTEPTR, 0x15, 8));
        }
    }
#endif
    return stream;
}

u8*
emit_test_mi_test(u8* stream)
{
    // Direct
#if TEST_MI_DIRECT
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        stream = emit_test(0, stream, mk_mi_direct(i, 0x12, 8));
    }

    for(X64_Register i = AX; i <= R15W; ++i)
    {
        stream = emit_test(0, stream, mk_mi_direct(i, 0x1234, 0));
    }
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_direct(i, 0x12345678, 0));
    }
#endif

    // Indirect
#if TEST_MI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_indirect(i, 0, ADDR_BYTEPTR, 0x12, 8));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_indirect(i, 0, ADDR_WORDPTR, 0x1234, 16));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_indirect(i, 0, ADDR_QWORDPTR, 0x12345678, 32));
        stream = emit_test(0, stream, mk_mi_indirect(i, 0, ADDR_DWORDPTR, 0x12345678, 32));
    }
#endif

    // Indirect byte displaced
#if TEST_MI_INDIRECT_BYTE_DISPLACED
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_indirect(i, 0x15, ADDR_BYTEPTR, 0x12, 8));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_indirect(i, 0x15, ADDR_WORDPTR, 0x1234, 16));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_indirect(i, 0x15, ADDR_QWORDPTR, 0x12345678, 32));
        stream = emit_test(0, stream, mk_mi_indirect(i, 0x15, ADDR_DWORDPTR, 0x12345678, 32));
    }
#endif

    // Indirect dword displaced
#if TEST_MI_INDIRECT_DWORD_DISPLACED
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_indirect(i, 0x15161718, ADDR_BYTEPTR, 0x12, 8));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_indirect(i, 0x15161718, ADDR_WORDPTR, 0x1234, 16));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_test(0, stream, mk_mi_indirect(i, 0x15161718, ADDR_QWORDPTR, 0x12345678, 32));
        stream = emit_test(0, stream, mk_mi_indirect(i, 0x15161718, ADDR_DWORDPTR, 0x12345678, 32));
    }
#endif

    return stream;
}

u8*
emit_test_mi_sib_test(u8* stream)
{
    // SIB X1
#if TEST_MI_SIB_X1_IMM8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_BYTEPTR, 0x12, 8));
        }
    }
#endif
#if TEST_MI_SIB_X1_IMM16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_WORDPTR, 0x1234, 16));
        }
    }
#endif
#if TEST_MI_SIB_X1_IMM32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_QWORDPTR, 0x12345678, 32));
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_DWORDPTR, 0x12345678, 32));
        }
    }
#endif

    // SIB X2
#if TEST_MI_SIB_X2_IMM8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_BYTEPTR, 0x12, 8));
        }
    }
#endif
#if TEST_MI_SIB_X2_IMM16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_WORDPTR, 0x1234, 16));
        }
    }
#endif
#if TEST_MI_SIB_X2_IMM32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_QWORDPTR, 0x12345678, 32));
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_DWORDPTR, 0x12345678, 32));
        }
    }
#endif
    
    // SIB X4 Byte Displaced
#if TEST_MI_SIB_X4_BYTE_DISPLACED_IMM8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_BYTEPTR, 0x12, 8));
        }
    }
#endif
#if TEST_MI_SIB_X4_BYTE_DISPLACED_IMM16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_WORDPTR, 0x1234, 16));
        }
    }
#endif
#if TEST_MI_SIB_X4_BYTE_DISPLACED_IMM32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_QWORDPTR, 0x12345678, 32));
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_DWORDPTR, 0x12345678, 32));
        }
    }
#endif

    // SIB X8 DWORD Displaced
#if TEST_MI_SIB_X8_DWORD_DISPLACED_IMM8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_BYTEPTR, 0x12, 8));
        }
    }
#endif
#if TEST_MI_SIB_X8_DWORD_DISPLACED_IMM16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_WORDPTR, 0x1234, 16));
        }
    }
#endif
#if TEST_MI_SIB_X8_DWORD_DISPLACED_IMM32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_QWORDPTR, 0x12345678, 32));
            stream = emit_test(0, stream, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_DWORDPTR, 0x12345678, 32));
        }
    }
#endif
    return stream;
}

u8*
emit_test_mr_test(u8* stream)
{
    // MR Direct
#if TEST_MR_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_test(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_test(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_test(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_test(0, stream, mk_mr_direct(i, j));
        }
    }
#endif

    // Indirect
#if TEST_MR_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_MR_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_MR_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif

    return stream;
}

u8*
emit_test_mr_sib_test(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);
    
    // SIB INDIRECT X1
#if TEST_MR_SIB_X1_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_MR_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_MR_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_test(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_test_i_test(u8* stream)
{
#if TEST_I_DIRECT
    stream = emit_test(0, stream, mk_i_reg(AL, 0x15, 8));
    stream = emit_test(0, stream, mk_i_reg(AX, 0x1516, 16));
    stream = emit_test(0, stream, mk_i_reg(EAX, 0x15161718, 32));
    stream = emit_test(0, stream, mk_i_reg(RAX, 0x15161718, 32));
#endif
    return stream;
}

u8*
emit_test_mri_shld(u8* stream)
{
    // Direct
#if TEST_MRI_DIRECT
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mri_direct(i, j, 0x15));
#endif
#if TEST_MRI_DIRECT
    for(X64_Register i = EAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mri_direct(i, j, 0x15));
#endif
#if TEST_MRI_DIRECT
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mri_direct(i, j, 0x15));
#endif
#if TEST_MRI_DIRECT
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mri_direct(i, j, 0x15));
#endif

    // Indirect
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect(i, j, 0, ADDR_QWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect(i, j, 0, ADDR_DWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect(i, j, 0, ADDR_WORDPTR, 0x15));
#endif

    // Indirect BD
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect(i, j, 0x17, ADDR_QWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect(i, j, 0x17, ADDR_DWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect(i, j, 0x17, ADDR_WORDPTR, 0x15));
#endif

    // Indirect DD
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect(i, j, 0x17181920, ADDR_QWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect(i, j, 0x17181920, ADDR_DWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect(i, j, 0x17181920, ADDR_WORDPTR, 0x15));
#endif
    return stream;
}

u8*
emit_test_mri_sib_shld(u8* stream)
{
    X64_Register index = R13;
    // Indirect SIB
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X1, 0, ADDR_QWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X2, 0, ADDR_DWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X4, 0, ADDR_WORDPTR, 0x15));
#endif

    // Indirect SIB BD
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X1, 0x18, ADDR_QWORDPTR, 0x15));
#endif

    // Indirect SIB DD
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X8, 0x12345678, ADDR_DWORDPTR, 0x15));
#endif
    return stream;
}

u8*
emit_test_mrc_shld(u8* stream)
{
    // Direct
#if TEST_MRC_DIRECT
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mrc_direct(i, j));
#endif
#if TEST_MRC_DIRECT
    for(X64_Register i = EAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mrc_direct(i, j));
#endif
#if TEST_MRC_DIRECT
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mrc_direct(i, j));
#endif
#if TEST_MRC_DIRECT
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mrc_direct(i, j));
#endif

    // Indirect
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect(i, j, 0, ADDR_QWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect(i, j, 0, ADDR_DWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect(i, j, 0, ADDR_WORDPTR));
#endif

    // Indirect BD
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect(i, j, 0x17, ADDR_QWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect(i, j, 0x17, ADDR_DWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect(i, j, 0x17, ADDR_WORDPTR));
#endif

    // Indirect DD
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect(i, j, 0x17181920, ADDR_QWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect(i, j, 0x17181920, ADDR_DWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect(i, j, 0x17181920, ADDR_WORDPTR));
#endif
    return stream;
}

u8*
emit_test_mrc_sib_shld(u8* stream)
{
    X64_Register index = R13;
    // Indirect SIB
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X1, 0, ADDR_QWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X2, 0, ADDR_DWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X4, 0, ADDR_WORDPTR));
#endif

    // Indirect SIB BD
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X1, 0x18, ADDR_QWORDPTR));
#endif

    // Indirect SIB DD
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shld(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X8, 0x12345678, ADDR_DWORDPTR));
#endif

    return stream;
}

u8*
emit_test_mri_shrd(u8* stream)
{
    // Direct
#if TEST_MRI_DIRECT
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mri_direct(i, j, 0x15));
#endif
#if TEST_MRI_DIRECT
    for(X64_Register i = EAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mri_direct(i, j, 0x15));
#endif
#if TEST_MRI_DIRECT
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mri_direct(i, j, 0x15));
#endif
#if TEST_MRI_DIRECT
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mri_direct(i, j, 0x15));
#endif

    // Indirect
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect(i, j, 0, ADDR_QWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect(i, j, 0, ADDR_DWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect(i, j, 0, ADDR_WORDPTR, 0x15));
#endif

    // Indirect BD
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect(i, j, 0x17, ADDR_QWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect(i, j, 0x17, ADDR_DWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect(i, j, 0x17, ADDR_WORDPTR, 0x15));
#endif

    // Indirect DD
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect(i, j, 0x17181920, ADDR_QWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect(i, j, 0x17181920, ADDR_DWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect(i, j, 0x17181920, ADDR_WORDPTR, 0x15));
#endif
    return stream;
}

u8*
emit_test_mri_sib_shrd(u8* stream)
{
    X64_Register index = R13;
    // Indirect SIB
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X1, 0, ADDR_QWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X2, 0, ADDR_DWORDPTR, 0x15));
#endif
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X4, 0, ADDR_WORDPTR, 0x15));
#endif

    // Indirect SIB BD
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X1, 0x18, ADDR_QWORDPTR, 0x15));
#endif

    // Indirect SIB DD
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X8, 0x12345678, ADDR_DWORDPTR, 0x15));
#endif

    // Indirect SIB DD
#if TEST_MRI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mri_indirect_sib(i, j, index, SIB_X4, 0x12345678, ADDR_WORDPTR, 0x15));
#endif
    return stream;
}

u8*
emit_test_mrc_shrd(u8* stream)
{
    // Direct
#if TEST_MRC_DIRECT
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mrc_direct(i, j));
#endif
#if TEST_MRC_DIRECT
    for(X64_Register i = EAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mrc_direct(i, j));
#endif
#if TEST_MRC_DIRECT
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mrc_direct(i, j));
#endif
#if TEST_MRC_DIRECT
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mrc_direct(i, j));
#endif

    // Indirect
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect(i, j, 0, ADDR_QWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect(i, j, 0, ADDR_DWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect(i, j, 0, ADDR_WORDPTR));
#endif

    // Indirect BD
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect(i, j, 0x17, ADDR_QWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect(i, j, 0x17, ADDR_DWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect(i, j, 0x17, ADDR_WORDPTR));
#endif

    // Indirect DD
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect(i, j, 0x17181920, ADDR_QWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect(i, j, 0x17181920, ADDR_DWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect(i, j, 0x17181920, ADDR_WORDPTR));
#endif
    return stream;
}

u8*
emit_test_mrc_sib_shrd(u8* stream)
{
    X64_Register index = R13;
    // Indirect SIB
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X1, 0, ADDR_QWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X2, 0, ADDR_DWORDPTR));
#endif
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X4, 0, ADDR_WORDPTR));
#endif

    // Indirect SIB BD
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X1, 0x18, ADDR_QWORDPTR));
#endif

    // Indirect SIB DD
#if TEST_MRC_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_shrd(0, stream, mk_mrc_indirect_sib(i, j, index, SIB_X8, 0x12345678, ADDR_DWORDPTR));
#endif

    return stream;
}

int main()
{
    #define FILENAME "test_shift.bin"
    FILE* out = fopen(FILENAME, "wb");
	u8* stream = (u8*)calloc(1, 1024*1024);
    u8* end = stream;

    {
        end = emit_shift_m1_test(end);
        end = emit_shift_m1_test_sib(end);
        end = emit_shift_mc_test(end);
        end = emit_shift_mc_test_sib(end);
        end = emit_shift_mi_test(end);
        end = emit_shift_mi_test_sib(end);
    }
    {
        end = emit_test_mi_test(end);
        end = emit_test_mi_sib_test(end);
        end = emit_test_mr_test(end);
        end = emit_test_mr_sib_test(end);
        end = emit_test_i_test(end);
    }
    {
        end = emit_test_mri_shld(end);
        end = emit_test_mri_sib_shld(end);
        end = emit_test_mrc_shld(end);
        end = emit_test_mrc_sib_shld(end);
        end = emit_test_mri_shrd(end);
        end = emit_test_mri_sib_shrd(end);
        end = emit_test_mrc_shrd(end);
        end = emit_test_mrc_sib_shrd(end);
    }

    fwrite(stream, 1, end - stream, out);
	fclose(out);

    system("objdump -D -Mintel,x86-64 -b binary -m i386 -w " FILENAME);
    
    return 0;
}