#define HO_ASSEMBLER_IMPLEMENT
#include "hoasm.h"
#include <stdio.h>
#include <stdlib.h>

// O
#define TEST_O 1

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

// RM
#define TEST_RM_DIRECT_64 1
#define TEST_RM_DIRECT_32 1
#define TEST_RM_DIRECT_16 1
#define TEST_RM_DIRECT_8  1
#define TEST_RM_INDIRECT_64 1
#define TEST_RM_INDIRECT_32 1
#define TEST_RM_INDIRECT_16 1
#define TEST_RM_INDIRECT_8  1
#define TEST_RM_INDIRECT_BYTE_DISPLACED_64 1
#define TEST_RM_INDIRECT_BYTE_DISPLACED_32 1
#define TEST_RM_INDIRECT_BYTE_DISPLACED_16 1
#define TEST_RM_INDIRECT_BYTE_DISPLACED_8  1
#define TEST_RM_INDIRECT_DWORD_DISPLACED_64 1
#define TEST_RM_INDIRECT_DWORD_DISPLACED_32 1
#define TEST_RM_INDIRECT_DWORD_DISPLACED_16 1
#define TEST_RM_INDIRECT_DWORD_DISPLACED_8  1
#define TEST_RM_SIB_X1_64 1
#define TEST_RM_SIB_X1_32 1
#define TEST_RM_SIB_X1_16 1
#define TEST_RM_SIB_X1_8  1
#define TEST_RM_SIB_BYTE_DISPLACED_X2_64 1
#define TEST_RM_SIB_BYTE_DISPLACED_X2_32 1
#define TEST_RM_SIB_BYTE_DISPLACED_X2_16 1
#define TEST_RM_SIB_BYTE_DISPLACED_X2_8  1
#define TEST_RM_SIB_DWORD_DISPLACED_X8_64 1
#define TEST_RM_SIB_DWORD_DISPLACED_X8_32 1
#define TEST_RM_SIB_DWORD_DISPLACED_X8_16 1
#define TEST_RM_SIB_DWORD_DISPLACED_X8_8  1

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

#define TEST_RELATIVE 1

#define TEST_LEA_INDIRECT_64 1
#define TEST_LEA_INDIRECT_32 1
#define TEST_LEA_INDIRECT_16 1
#define TEST_LEA_INDIRECT_BYTE_DISPLACED_64 1
#define TEST_LEA_INDIRECT_BYTE_DISPLACED_32 1
#define TEST_LEA_INDIRECT_BYTE_DISPLACED_16 1
#define TEST_LEA_INDIRECT_DWORD_DISPLACED_64 1
#define TEST_LEA_INDIRECT_DWORD_DISPLACED_32 1
#define TEST_LEA_INDIRECT_DWORD_DISPLACED_16 1
#define TEST_LEA_SIB_X1_64 1
#define TEST_LEA_SIB_X2_BYTE_DISPLACED_32 1
#define TEST_LEA_SIB_X4_DWORD_DISPLACED_16 1

X64_Arithmetic_Instr instr = ARITH_ADD;

u8*
emit_mi_test(u8* stream)
{
    // Direct
#if TEST_MI_DIRECT
    for(X64_Register i = RAX; i <= DIL; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_direct(i, 0x12, 8));
    }
    for(X64_Register i = RAX; i <= R15W; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_direct(i, 0x1234, 0));
    }
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_direct(i, 0x12345678, 0));
    }
#endif

    // Indirect
#if TEST_MI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0, ADDR_QWORDPTR, 0x12, 8));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0, ADDR_DWORDPTR, 0x12, 8));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0, ADDR_WORDPTR, 0x12, 8));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0, ADDR_BYTEPTR, 0x12, 8));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0, ADDR_QWORDPTR, 0x1234, 16));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0, ADDR_DWORDPTR, 0x1234, 16));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0, ADDR_WORDPTR, 0x1234, 16));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0, ADDR_QWORDPTR, 0x12345678, 32));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0, ADDR_DWORDPTR, 0x12345678, 32));
    }
#endif

    // Indirect byte displaced
#if TEST_MI_INDIRECT_BYTE_DISPLACED
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_QWORDPTR, 0x12, 8));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_DWORDPTR, 0x12, 8));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_WORDPTR, 0x12, 8));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_BYTEPTR, 0x12, 8));
    }
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_QWORDPTR, 0x1234, 16));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_DWORDPTR, 0x1234, 16));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_WORDPTR, 0x1234, 16));
    }
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_QWORDPTR, 0x12345678, 32));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_DWORDPTR, 0x12345678, 32));
    }
#endif

    // Indirect dword displaced
#if TEST_MI_INDIRECT_DWORD_DISPLACED
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_QWORDPTR, 0x12, 8));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_DWORDPTR, 0x12, 8));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_WORDPTR, 0x12, 8));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_BYTEPTR, 0x12, 8));
    }
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_QWORDPTR, 0x1234, 16));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_DWORDPTR, 0x1234, 16));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_WORDPTR, 0x1234, 16));
    }
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_QWORDPTR, 0x12345678, 32));
        stream = emit_arithmetic(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_DWORDPTR, 0x12345678, 32));
    }
#endif
    
    return stream;
}

u8*
emit_mi_sib_test(u8* stream)
{
    // SIB X1
#if TEST_MI_SIB_X1_IMM8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_QWORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_DWORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_WORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_BYTEPTR, 0x12, 8));
        }
    }
#endif
#if TEST_MI_SIB_X1_IMM16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_QWORDPTR, 0x1234, 16));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_DWORDPTR, 0x1234, 16));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_WORDPTR, 0x1234, 16));
        }
    }
#endif
#if TEST_MI_SIB_X1_IMM32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_QWORDPTR, 0x12345678, 32));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_DWORDPTR, 0x12345678, 32));
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
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_QWORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_DWORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_WORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_BYTEPTR, 0x12, 8));
        }
    }
#endif
#if TEST_MI_SIB_X2_IMM16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_QWORDPTR, 0x1234, 16));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_DWORDPTR, 0x1234, 16));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_WORDPTR, 0x1234, 16));
        }
    }
#endif
#if TEST_MI_SIB_X2_IMM32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_QWORDPTR, 0x12345678, 32));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_DWORDPTR, 0x12345678, 32));
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
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_QWORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_DWORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_WORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_BYTEPTR, 0x12, 8));
        }
    }
#endif
#if TEST_MI_SIB_X4_BYTE_DISPLACED_IMM16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_QWORDPTR, 0x1234, 16));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_DWORDPTR, 0x1234, 16));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_WORDPTR, 0x1234, 16));
        }
    }
#endif
#if TEST_MI_SIB_X4_BYTE_DISPLACED_IMM32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_QWORDPTR, 0x12345678, 32));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_DWORDPTR, 0x12345678, 32));
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
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_QWORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_DWORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_WORDPTR, 0x12, 8));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_BYTEPTR, 0x12, 8));
        }
    }
#endif
#if TEST_MI_SIB_X8_DWORD_DISPLACED_IMM16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_QWORDPTR, 0x1234, 16));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_DWORDPTR, 0x1234, 16));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_WORDPTR, 0x1234, 16));
        }
    }
#endif
#if TEST_MI_SIB_X8_DWORD_DISPLACED_IMM32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_QWORDPTR, 0x12345678, 32));
            stream = emit_arithmetic(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_DWORDPTR, 0x12345678, 32));
        }
    }
#endif
    return stream;
}

u8*
emit_rm_test(u8* stream)
{
    // RM Direct
#if TEST_RM_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_direct(i, j));
        }
    }
#endif
#if TEST_RM_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_direct(i, j));
        }
    }
#endif
#if TEST_RM_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_direct(i, j));
        }
    }
#endif
#if TEST_RM_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_direct(i, j));
        }
    }
#endif

    // Indirect
#if TEST_RM_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_32
    for(X64_Register j = EAX; j <= R15D; ++j)
    {
        for(X64_Register i = RAX; i <= R15D; ++i)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_16
    for(X64_Register j = AX; j <= R15W; ++j)
    {
        for(X64_Register i = RAX; i <= R15; ++i)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_8
    for(X64_Register j = AL; j <= DIL; ++j)
    {
        for(X64_Register i = RAX; i <= R15D; ++i)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0, ADDR_BYTEPTR));
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_RM_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_RM_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect(j, i, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_rm_sib_test(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);
    
    // SIB INDIRECT X1
#if TEST_RM_SIB_X1_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_X1_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_X1_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_RM_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_RM_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_mr_test(u8* stream)
{
    // MR Direct
#if TEST_MR_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_direct(i, j));
        }
    }
#endif

    // Indirect
#if TEST_MR_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_MR_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_MR_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect(i, j, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif

    return stream;
}

u8*
emit_mr_sib_test(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);
    
    // SIB INDIRECT X1
#if TEST_MR_SIB_X1_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_MR_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_MR_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_arithmetic(0, stream, instr, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_rel_test(u8* stream)
{
#if TEST_RELATIVE
    stream = emit_arithmetic(0, stream, ARITH_ADD, mk_mi_indirect(REG_NONE, 0x15, ADDR_QWORDPTR, 0x66667777, 32));
    stream = emit_arithmetic(0, stream, ARITH_ADD, mk_mi_indirect(REG_NONE, 0x15, ADDR_DWORDPTR, 0x66667777, 32));
    stream = emit_arithmetic(0, stream, ARITH_ADD, mk_mi_indirect(REG_NONE, 0x15, ADDR_WORDPTR, 0x6666, 16));
    stream = emit_arithmetic(0, stream, ARITH_ADD, mk_mi_indirect(REG_NONE, 0x15, ADDR_BYTEPTR, 0x66, 8));
#endif

    return stream;
}

u8*
emit_lea_test(u8* stream)
{
    // Indirect
#if TEST_LEA_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect(i, j, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_LEA_INDIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect(i, j, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_LEA_INDIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_LEA_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_LEA_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_LEA_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_LEA_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect(i, j, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_LEA_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_LEA_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect(i, j, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_lea_sib_test(u8* stream)
{
    X64_Register index = R13;
    // Indirect sib
#if TEST_LEA_SIB_X1_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect_sib(i, j, index, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_LEA_SIB_X2_BYTE_DISPLACED_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect_sib(i, j, index, SIB_X2, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_LEA_SIB_X4_DWORD_DISPLACED_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_lea(0, stream, mk_rm_indirect_sib(i, j, index, SIB_X4, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_xadd_mr_test(u8* stream)
{
    // MR Direct
#if TEST_MR_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_direct(i, j));
        }
    }
#endif

    // Indirect
#if TEST_MR_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_MR_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_MR_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif

    return stream;
}

u8*
emit_xadd_mr_sib_test(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);
    
    // SIB INDIRECT X1
#if TEST_MR_SIB_X1_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_MR_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_MR_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_xadd(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_xchg_o_test(u8* stream)
{
#if TEST_O
    for(X64_Register i = RAX; i <= R15W; ++i)    
        stream = emit_xchg(0, stream, mk_o(i));
#endif
    return stream;
}

u8*
emit_xchg_mr_test(u8* stream)
{
    // Direct
#if TEST_MR_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_mr_direct(i, j));
#endif
#if TEST_MR_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_mr_direct(i, j));
#endif
#if TEST_MR_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_mr_direct(i, j));
#endif
#if TEST_MR_DIRECT_8
    for(X64_Register i = AL; i <= R15B; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_mr_direct(i, j));
#endif

    // Indirect
#if TEST_MR_INDIRECT_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0, ADDR_QWORDPTR));
#endif
#if TEST_MR_INDIRECT_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0, ADDR_DWORDPTR));
#endif
#if TEST_MR_INDIRECT_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0, ADDR_WORDPTR));
#endif
#if TEST_MR_INDIRECT_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0, ADDR_BYTEPTR));
#endif

    // Indirect BD
#if TEST_MR_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_QWORDPTR));
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_DWORDPTR));
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_WORDPTR));
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_BYTEPTR));
#endif

    // Indirect DD
#if TEST_MR_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_QWORDPTR));
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_WORDPTR));
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_BYTEPTR));
#endif
    return stream;
}

u8*
emit_xchg_mr_sib_test(u8* stream)
{
    X64_Register index = R13;
    // Indirect
#if TEST_MR_SIB_X1_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X1, 0, ADDR_QWORDPTR));
#endif
#if TEST_MR_SIB_X1_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X1, 0, ADDR_DWORDPTR));
#endif
#if TEST_MR_SIB_X1_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X1, 0, ADDR_WORDPTR));
#endif
#if TEST_MR_SIB_X1_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X1, 0, ADDR_BYTEPTR));
#endif

    // Indirect BD
#if TEST_MR_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X2, 0x15, ADDR_QWORDPTR));
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X2, 0x15, ADDR_DWORDPTR));
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X2, 0x15, ADDR_WORDPTR));
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X2, 0x15, ADDR_BYTEPTR));
#endif

    // Indirect DD
#if TEST_MR_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X8, 0x15161718, ADDR_QWORDPTR));
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X8, 0x15161718, ADDR_DWORDPTR));
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X8, 0x15161718, ADDR_WORDPTR));
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_mr_indirect_sib(i, j, index, SIB_X8, 0x15161718, ADDR_BYTEPTR));
#endif
    return stream;
}

u8*
emit_xchg_rm_test(u8* stream)
{
    // Direct
#if TEST_RM_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_rm_direct(j, i));
#endif
#if TEST_RM_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_rm_direct(j, i));
#endif
#if TEST_RM_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_rm_direct(j, i));
#endif
#if TEST_RM_DIRECT_8
    for(X64_Register i = AL; i <= R15B; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_rm_direct(j, i));
#endif

    // Indirect
#if TEST_RM_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)    
        for(X64_Register j = RAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(i, j, 0, ADDR_QWORDPTR));
#endif
#if TEST_RM_INDIRECT_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0, ADDR_DWORDPTR));
#endif
#if TEST_RM_INDIRECT_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0, ADDR_WORDPTR));
#endif
#if TEST_RM_INDIRECT_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0, ADDR_BYTEPTR));
#endif

    // Indirect BD
#if TEST_RM_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_QWORDPTR));
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_DWORDPTR));
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_WORDPTR));
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_BYTEPTR));
#endif

    // Indirect DD
#if TEST_RM_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_QWORDPTR));
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_DWORDPTR));
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_WORDPTR));
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_BYTEPTR));
#endif
    return stream;
}

u8*
emit_xchg_rm_sib_test(u8* stream)
{
    X64_Register index = R13;
    // Indirect
#if TEST_RM_SIB_X1_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X1, 0, ADDR_QWORDPTR));
#endif
#if TEST_RM_SIB_X1_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X1, 0, ADDR_DWORDPTR));
#endif
#if TEST_RM_SIB_X1_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X1, 0, ADDR_WORDPTR));
#endif
#if TEST_RM_SIB_X1_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X1, 0, ADDR_BYTEPTR));
#endif

    // Indirect BD
#if TEST_RM_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X2, 0x15, ADDR_QWORDPTR));
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X2, 0x15, ADDR_DWORDPTR));
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X2, 0x15, ADDR_WORDPTR));
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X2, 0x15, ADDR_BYTEPTR));
#endif

    // Indirect DD
#if TEST_RM_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = RAX; j <= R15; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X8, 0x15161718, ADDR_QWORDPTR));
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = EAX; j <= R15D; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X8, 0x15161718, ADDR_DWORDPTR));
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AX; j <= R15W; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X8, 0x15161718, ADDR_WORDPTR));
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_8
    for(X64_Register i = RAX; i <= R15D; ++i)    
        for(X64_Register j = AL; j <= R15B; ++j)   
            stream = emit_xchg(0, stream, mk_rm_indirect_sib(j, i, index, SIB_X8, 0x15161718, ADDR_BYTEPTR));
#endif
    return stream;
}

int main()
{
    #define FILENAME "test_arith.bin"
    FILE* out = fopen(FILENAME, "wb");
	u8* stream = (u8*)calloc(1, 1024*1024);
    u8* end = stream;
    {
        end = emit_mi_test(end);
        end = emit_mi_sib_test(end);
        end = emit_rm_test(end);
        end = emit_rm_sib_test(end);
        end = emit_mr_test(end);
        end = emit_mr_sib_test(end);
        end = emit_rel_test(end);
    }
    {
        end = emit_lea_test(end);
        end = emit_lea_sib_test(end);
    }
    {
        end = emit_xadd_mr_test(end);
        end = emit_xadd_mr_sib_test(end);
        end = emit_xchg_o_test(end);
        end = emit_xchg_mr_test(end);
        end = emit_xchg_mr_sib_test(end);
        end = emit_xchg_rm_test(end);
        end = emit_xchg_rm_sib_test(end);
    }

    fwrite(stream, 1, end - stream, out);
	fclose(out);

    system("objdump -D -Mintel,x86-64 -b binary -m i386 -w " FILENAME);
    return 0;
}