#define HO_ASSEMBLER_IMPLEMENT
#include "hoasm.h"
#include <stdio.h>
#include <stdlib.h>

#define MOV_TEST 1
#define SET_TEST 1
#define LAR_TEST 1
#define LSL_TEST 1
#define MOV_DBGREG 1

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

u8*
emit_mov_mi_test(u8* stream)
{
    // DIRECT
#if MOV_TEST
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        stream = emit_mov(0, stream, mk_mi_direct(i, 0x15, 8));
    }
#endif
#if MOV_TEST
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        stream = emit_mov(0, stream, mk_mi_direct(i, 0x15, 16));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_mi_direct(i, 0x15, 32));
    }
#endif
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_mi_direct(i, 0x15, 32));
    }
#endif

    // INDIRECT
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0, ADDR_BYTEPTR, 0x15, 8));
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0, ADDR_WORDPTR, 0x15, 16));
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0, ADDR_DWORDPTR, 0x15, 32));
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0, ADDR_QWORDPTR, 0x15, 32));
    }
#endif

    // INDIRECT BYTE DISPLACED
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0x17, ADDR_BYTEPTR, 0x15, 8));
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0x17, ADDR_WORDPTR, 0x15, 16));
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0x17, ADDR_DWORDPTR, 0x15, 32));
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0x17, ADDR_QWORDPTR, 0x15, 32));
    }
#endif

    // INDIRECT DWORD DISPLACED
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0x15161718, ADDR_BYTEPTR, 0x15, 8));
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0x15161718, ADDR_WORDPTR, 0x15, 16));
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0x15161718, ADDR_DWORDPTR, 0x15, 32));
        stream = emit_mov(0, stream, mk_mi_indirect(i, 0x15161718, ADDR_QWORDPTR, 0x15, 32));
    }
#endif
    
    return stream;
}

u8*
emit_mov_oi_test(u8* stream)
{
    // DIRECT
#if MOV_TEST
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        stream = emit_mov(0, stream, mk_oi(i, 0x15, 8));
    }
#endif
#if MOV_TEST
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        stream = emit_mov(0, stream, mk_oi(i, 0x15, 16));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_oi(i, 0x15, 32));
    }
#endif
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_oi(i, 0x15, 64));
    }
#endif
    return stream;
}

u8*
emit_mov_mr_test_sreg(u8* stream)
{
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_mr_direct(i, ES));
        stream = emit_mov(0, stream, mk_mr_direct(i, CS));
        stream = emit_mov(0, stream, mk_mr_direct(i, SS));
        stream = emit_mov(0, stream, mk_mr_direct(i, DS));
        stream = emit_mov(0, stream, mk_mr_direct(i, FS));
        stream = emit_mov(0, stream, mk_mr_direct(i, GS));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_mr_direct(i, ES));
        stream = emit_mov(0, stream, mk_mr_direct(i, CS));
        stream = emit_mov(0, stream, mk_mr_direct(i, SS));
        stream = emit_mov(0, stream, mk_mr_direct(i, DS));
        stream = emit_mov(0, stream, mk_mr_direct(i, FS));
        stream = emit_mov(0, stream, mk_mr_direct(i, GS));
    }
#endif
#if MOV_TEST
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        stream = emit_mov(0, stream, mk_mr_direct(i, ES));
        stream = emit_mov(0, stream, mk_mr_direct(i, CS));
        stream = emit_mov(0, stream, mk_mr_direct(i, SS));
        stream = emit_mov(0, stream, mk_mr_direct(i, DS));
        stream = emit_mov(0, stream, mk_mr_direct(i, FS));
        stream = emit_mov(0, stream, mk_mr_direct(i, GS));
    }
#endif

    // Indirect
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_mr_indirect(i, ES, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, CS, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, SS, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, DS, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, FS, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, GS, 0, ADDR_WORDPTR));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_mr_indirect(i, ES, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, CS, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, SS, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, DS, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, FS, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, GS, 0, ADDR_WORDPTR));
    }
#endif

    // Indirect byte displaced
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_mr_indirect(i, ES, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, CS, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, SS, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, DS, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, FS, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, GS, 0x15, ADDR_WORDPTR));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_mr_indirect(i, ES, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, CS, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, SS, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, DS, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, FS, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, GS, 0x15, ADDR_WORDPTR));
    }
#endif

    // Indirect dword displaced
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_mr_indirect(i, ES, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, CS, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, SS, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, DS, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, FS, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, GS, 0x15161718, ADDR_WORDPTR));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_mr_indirect(i, ES, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, CS, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, SS, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, DS, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, FS, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_mr_indirect(i, GS, 0x15161718, ADDR_WORDPTR));
    }
#endif

    return stream;
}

u8*
emit_mov_mr_sib_test_sreg(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);

    // SIB INDIRECT X1
#if TEST_MR_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = ES; j <= GS; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_MR_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = ES; j <= GS; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_MR_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = ES; j <= GS; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_mov_rm_test_sreg(u8* stream)
{
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_rm_direct(ES, i));
        stream = emit_mov(0, stream, mk_rm_direct(CS, i));
        stream = emit_mov(0, stream, mk_rm_direct(SS, i));
        stream = emit_mov(0, stream, mk_rm_direct(DS, i));
        stream = emit_mov(0, stream, mk_rm_direct(FS, i));
        stream = emit_mov(0, stream, mk_rm_direct(GS, i));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_rm_direct(ES, i));
        stream = emit_mov(0, stream, mk_rm_direct(CS, i));
        stream = emit_mov(0, stream, mk_rm_direct(SS, i));
        stream = emit_mov(0, stream, mk_rm_direct(DS, i));
        stream = emit_mov(0, stream, mk_rm_direct(FS, i));
        stream = emit_mov(0, stream, mk_rm_direct(GS, i));
    }
#endif
#if MOV_TEST
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        stream = emit_mov(0, stream, mk_rm_direct(ES, i));
        stream = emit_mov(0, stream, mk_rm_direct(CS, i));
        stream = emit_mov(0, stream, mk_rm_direct(SS, i));
        stream = emit_mov(0, stream, mk_rm_direct(DS, i));
        stream = emit_mov(0, stream, mk_rm_direct(FS, i));
        stream = emit_mov(0, stream, mk_rm_direct(GS, i));
    }
#endif

    // Indirect
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_rm_indirect(ES, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(CS, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(SS, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(DS, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(FS, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(GS, i, 0, ADDR_WORDPTR));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_rm_indirect(ES, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(CS, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(SS, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(DS, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(FS, i, 0, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(GS, i, 0, ADDR_WORDPTR));
    }
#endif

    // Indirect byte displaced
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_rm_indirect(ES, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(CS, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(SS, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(DS, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(FS, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(GS, i, 0x15, ADDR_WORDPTR));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_rm_indirect(ES, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(CS, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(SS, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(DS, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(FS, i, 0x15, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(GS, i, 0x15, ADDR_WORDPTR));
    }
#endif

    // Indirect dword displaced
#if MOV_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_mov(0, stream, mk_rm_indirect(ES, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(CS, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(SS, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(DS, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(FS, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(GS, i, 0x15161718, ADDR_WORDPTR));
    }
#endif
#if MOV_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        stream = emit_mov(0, stream, mk_rm_indirect(ES, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(CS, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(SS, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(DS, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(FS, i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mov(0, stream, mk_rm_indirect(GS, i, 0x15161718, ADDR_WORDPTR));
    }
#endif

    return stream;
}

u8*
emit_mov_rm_sib_test_sreg(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);

    // SIB INDIRECT X1
#if TEST_RM_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = ES; j <= GS; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_RM_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = ES; j <= GS; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_RM_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = ES; j <= GS; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8* 
emit_mov_moffs_test(u8* stream)
{
    // MOFFS FD
#if MOV_TEST
    stream = emit_mov(0, stream, mk_fd(REG_NONE, 0x12345678, 64));
    stream = emit_mov(0, stream, mk_fd(REG_NONE, 0x12345678, 32));
    stream = emit_mov(0, stream, mk_fd(REG_NONE, 0x12345678, 16));
    stream = emit_mov(0, stream, mk_fd(REG_NONE, 0x12345678, 8));
#endif
#if MOV_TEST
    for(X64_Register i = ES; i <= GS; ++i)
    {
        stream = emit_mov(0, stream, mk_fd(i, 0x12345678, 64));
        stream = emit_mov(0, stream, mk_fd(i, 0x12345678, 32));
        stream = emit_mov(0, stream, mk_fd(i, 0x12345678, 16));
        stream = emit_mov(0, stream, mk_fd(i, 0x12345678, 8));
    }
#endif

    // MOFFS TD
#if MOV_TEST
    stream = emit_mov(0, stream, mk_td(REG_NONE, 0x12345678, 64));
    stream = emit_mov(0, stream, mk_td(REG_NONE, 0x12345678, 32));
    stream = emit_mov(0, stream, mk_td(REG_NONE, 0x12345678, 16));
    stream = emit_mov(0, stream, mk_td(REG_NONE, 0x12345678, 8));
#endif
#if MOV_TEST
    for(X64_Register i = ES; i <= GS; ++i)
    {
        stream = emit_mov(0, stream, mk_td(i, 0x12345678, 64));
        stream = emit_mov(0, stream, mk_td(i, 0x12345678, 32));
        stream = emit_mov(0, stream, mk_td(i, 0x12345678, 16));
        stream = emit_mov(0, stream, mk_td(i, 0x12345678, 8));
    }
#endif
    return stream;
}

u8*
emit_mov_mr_test(u8* stream)
{
    // MR Direct
#if TEST_MR_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_direct(i, j));
        }
    }
#endif

    // Indirect
#if TEST_MR_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_MR_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_MR_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_mov_mr_sib_test(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);
    
    // SIB INDIRECT X1
#if TEST_MR_SIB_X1_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_MR_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_MR_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_mov_rm_test(u8* stream)
{
    // RM Direct
#if TEST_RM_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if TEST_RM_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if TEST_RM_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if TEST_RM_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_direct(i, j));
        }
    }
#endif

    // Indirect
#if TEST_RM_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_RM_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_RM_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_mov_rm_sib_test(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);
    
    // SIB INDIRECT X1
#if TEST_RM_SIB_X1_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_X1_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_X1_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_RM_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_RM_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_mov(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_movsxd_rm_test(u8* stream)
{
    // This instruction encoding as R16_R32, R16_M32, R32_R32 and R32_M32 is discourage by intel, 
    // it says in the manual:
    // "The use of MOVSXD without REX.W in 64-bit mode is discouraged. 
    // Regular MOV should be used instead of using MOVSXD without REX.W."

#if MOV_TEST
    // RM Direct
    // R64_R32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if MOV_TEST
    // RM Direct
    // R32_R32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if MOV_TEST
    // RM Direct
    // R16_R32 it says in the manual R16_R16, but all the disassemblers show R16_R32 instead, idk why.
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_direct(i, j));
        }
    }
#endif

    // Indirect
#if MOV_TEST
    // R64_M32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_indirect(i, j, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if MOV_TEST
    // R32_M32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_indirect(i, j, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if MOV_TEST
    // R16_M32
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            // TODO(psv): assert that this must be ADDR_WORDPTR, since otherwise
            // the instruction encodes it as a 32 bit copy
            stream = emit_movsxd(0, stream, mk_rm_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif

    // Indirect byte displaced
#if MOV_TEST
    // R64_M32 BD
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if MOV_TEST
    // R32_M32 BD
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if MOV_TEST
    // R16_M32 BD
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif

    // Indirect dword displaced
#if MOV_TEST
    // R64_M32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if MOV_TEST
    // R32_M32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if MOV_TEST
    // R16_M32
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsxd(0, stream, mk_rm_indirect(i, j, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_movsx_rm_test(u8* stream)
{
    // Direct
#if MOV_TEST
    // RM Direct R64_R8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if MOV_TEST
    // RM Direct R32_R8
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if MOV_TEST
    // RM Direct R16_R8
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if MOV_TEST
    // RM Direct R64_R16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if MOV_TEST
    // RM Direct R32_R16
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_direct(i, j));
        }
    }
#endif

    // Indirect
#if MOV_TEST
    // RM Indirect R64_R8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0, ADDR_BYTEPTR));
        }
    }
#endif
#if MOV_TEST
    // RM Indirect R32_R8
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0, ADDR_BYTEPTR));
        }
    }
#endif
#if MOV_TEST
    // RM Indirect R16_R8
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0, ADDR_BYTEPTR));
        }
    }
#endif
#if MOV_TEST
    // RM Indirect R64_R16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif
#if MOV_TEST
    // RM Indirect R32_R16
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif

    // Indirect byte displaced
#if MOV_TEST
    // RM Indirect BD R64_R8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_BYTEPTR));
        }
    }
#endif
#if MOV_TEST
    // RM Indirect R32_R8
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_BYTEPTR));
        }
    }
#endif
#if MOV_TEST
    // RM Indirect R16_R8
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_BYTEPTR));
        }
    }
#endif
#if MOV_TEST
    // RM Indirect R64_R16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if MOV_TEST
    // RM Indirect R32_R16
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_movsx(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif

    return stream;
}

u8*
emit_cmovcc_test(u8* stream)
{
    X64_CMOVcc_Instruction i = CMOVE;
    // Direct
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_direct(j, k));
            }
        }
    }
#endif

    // Indirect
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect(j, k, 0, ADDR_QWORDPTR));
            }
        }
    }
#endif
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect(j, k, 0, ADDR_DWORDPTR));
            }
        }
    }
#endif
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect(j, k, 0, ADDR_WORDPTR));
            }
        }
    }
#endif
    // TODO(psv): forbid 8 bits

    // Indirect byte displaced
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect(j, k, 0x15, ADDR_QWORDPTR));
            }
        }
    }
#endif
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect(j, k, 0x15, ADDR_DWORDPTR));
            }
        }
    }
#endif
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect(j, k, 0x15, ADDR_WORDPTR));
            }
        }
    }
#endif

    // Indirect dword displaced
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect(j, k, 0x15161718, ADDR_QWORDPTR));
            }
        }
    }
#endif
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect(j, k, 0x15161718, ADDR_DWORDPTR));
            }
        }
    }
#endif
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect(j, k, 0x15161718, ADDR_WORDPTR));
            }
        }
    }
#endif
    return stream;
}

u8*
emit_cmovcc_sib_test(u8* stream)
{
    X64_Register index = R13;
    X64_CMOVcc_Instruction i = CMOVE;

#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect_sib(j, k, index, SIB_X1, 0, ADDR_QWORDPTR));
            }
        }
    }
#endif
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect_sib(j, k, index, SIB_X2, 0, ADDR_DWORDPTR));
            }
        }
    }
#endif
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect_sib(j, k, index, SIB_X4, 0x15, ADDR_WORDPTR));
            }
        }
    }
#endif
#if MOV_TEST
    for(i = CMOVE; i <= CMOVG; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            for(X64_Register k = RAX; k <= R15D; ++k)
            {
                stream = emit_cmovcc(0, stream, i, mk_rm_indirect_sib(j, k, index, SIB_X8, 0x15161718, ADDR_QWORDPTR));
            }
        }
    }
#endif
    return stream;
}

u8*
emit_setcc_test(u8* stream)
{
    // Indirect
#if SET_TEST
    for(X64_SETcc_Instruction i = SETE; i <= SETG; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_setcc(0, stream, i, mk_m_indirect(j, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect byte displaced
#if SET_TEST
    for(X64_SETcc_Instruction i = SETE; i <= SETG; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_setcc(0, stream, i, mk_m_indirect(j, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect dword displaced
#if SET_TEST
    for(X64_SETcc_Instruction i = SETE; i <= SETG; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_setcc(0, stream, i, mk_m_indirect(j, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_setcc_sib_test(u8* stream)
{
    X64_Register index = R13;

#if SET_TEST
    for(X64_SETcc_Instruction i = SETE; i <= SETG; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_setcc(0, stream, i, mk_m_indirect_sib(j, index, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif
#if SET_TEST
    for(X64_SETcc_Instruction i = SETE; i <= SETG; ++i)
    {
        for(X64_Register j = RAX; j <= R15D; ++j)
        {
            stream = emit_setcc(0, stream, i, mk_m_indirect_sib(j, index, SIB_X8, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_lar_test(u8* stream)
{
    X64_Register index = R13;
#if LAR_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_lar(0, stream, mk_rm_direct(i, j));
#endif
#if LAR_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_lar(0, stream, mk_rm_direct(i, j));
#endif
#if LAR_TEST
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_lar(0, stream, mk_rm_direct(i, j));
#endif
#if LAR_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lar(0, stream, mk_rm_indirect(i, j, 0, ADDR_WORDPTR));
#endif
#if LAR_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lar(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_WORDPTR));
#endif
#if LAR_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lar(0, stream, mk_rm_indirect(i, j, 0x15161718, ADDR_WORDPTR));
#endif
#if LAR_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lar(0, stream, mk_rm_indirect_sib(i, j, index, SIB_X1, 0, ADDR_WORDPTR));
#endif
#if LAR_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lar(0, stream, mk_rm_indirect_sib(i, j, index, SIB_X2, 0x15, ADDR_WORDPTR));
#endif
#if LAR_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lar(0, stream, mk_rm_indirect_sib(i, j, index, SIB_X8, 0x15161718, ADDR_WORDPTR));
#endif
    return stream;
}

u8*
emit_lsl_test(u8* stream)
{
    X64_Register index = R13;
#if LSL_TEST
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = RAX; j <= R15; ++j)
            stream = emit_lsl(0, stream, mk_rm_direct(i, j));
#endif
#if LSL_TEST
    for(X64_Register i = EAX; i <= R15D; ++i)
        for(X64_Register j = EAX; j <= R15D; ++j)
            stream = emit_lsl(0, stream, mk_rm_direct(i, j));
#endif
#if LSL_TEST
    for(X64_Register i = AX; i <= R15W; ++i)
        for(X64_Register j = AX; j <= R15W; ++j)
            stream = emit_lsl(0, stream, mk_rm_direct(i, j));
#endif
#if LSL_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lsl(0, stream, mk_rm_indirect(i, j, 0, ADDR_WORDPTR));
#endif
#if LSL_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lsl(0, stream, mk_rm_indirect(i, j, 0x15, ADDR_WORDPTR));
#endif
#if LSL_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lsl(0, stream, mk_rm_indirect(i, j, 0x15161718, ADDR_WORDPTR));
#endif
#if LSL_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lsl(0, stream, mk_rm_indirect_sib(i, j, index, SIB_X1, 0, ADDR_WORDPTR));
#endif
#if LSL_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lsl(0, stream, mk_rm_indirect_sib(i, j, index, SIB_X2, 0x15, ADDR_WORDPTR));
#endif
#if LSL_TEST
    for(X64_Register i = RAX; i <= R15W; ++i)
        for(X64_Register j = RAX; j <= R15D; ++j)
            stream = emit_lsl(0, stream, mk_rm_indirect_sib(i, j, index, SIB_X8, 0x15161718, ADDR_WORDPTR));
#endif
    return stream;
}

u8*
emit_movdbg_test(u8* stream)
{
    // TODO(psv): indirect doesnt work, but generated direct ?? check this
#if MOV_DBGREG
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = DR0; j <= DR7; ++j)
            stream = emit_mov_debug_reg(0, stream, mk_mr_direct(i, j));
#endif
#if MOV_DBGREG
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = DR0; j <= DR7; ++j)
            stream = emit_mov_debug_reg(0, stream, mk_rm_direct(j, i));
#endif
    return stream;
}

u8*
emit_movcontrol_test(u8* stream)
{
    // TODO(psv): indirect doesnt work, but generated direct ?? check this
#if MOV_DBGREG
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = CR0; j <= CR15; ++j)
            stream = emit_mov_control_reg(0, stream, mk_mr_direct(i, j));
#endif
#if MOV_DBGREG
    for(X64_Register i = RAX; i <= R15; ++i)
        for(X64_Register j = CR0; j <= CR15; ++j)
            stream = emit_mov_control_reg(0, stream, mk_rm_direct(j, i));
#endif
    return stream;
}

int main()
{
    #define FILENAME "test_mov.bin"
    FILE* out = fopen(FILENAME, "wb");
	u8* stream = (u8*)calloc(1, 1024*1024);
    u8* end = stream;

    {
        end = emit_mov_mi_test(end);
        end = emit_mov_oi_test(end);
        end = emit_mov_mr_test(end);
        end = emit_mov_mr_sib_test(end);
        end = emit_mov_rm_test(end);
        end = emit_mov_rm_sib_test(end);
        end = emit_mov_mr_test_sreg(end);
        end = emit_mov_mr_sib_test_sreg(end);
        end = emit_mov_rm_test_sreg(end);
        end = emit_mov_rm_sib_test_sreg(end);
        end = emit_mov_moffs_test(end);
    }
    {
        end = emit_movsx_rm_test(end);
        end = emit_movsxd_rm_test(end);
    }
    {
        end = emit_cmovcc_test(end);
        end = emit_cmovcc_sib_test(end);
    }
    {
        end = emit_setcc_test(end);
        end = emit_setcc_sib_test(end);
    }
    {
        end = emit_lar_test(end);
        end = emit_lsl_test(end);
    }
    {
        end = emit_movdbg_test(end);
        end = emit_movcontrol_test(end);
    }

    fwrite(stream, 1, end - stream, out);
	fclose(out);

    //system("objdump -D -Mintel,x86-64 -b binary -m i386 -w " FILENAME " > arith_result.res");
    system("objdump -D -Mintel,x86-64 -b binary -m i386 -w " FILENAME);
    return 0;
}