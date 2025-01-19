#define HO_ASSEMBLER_IMPLEMENT
#include "hoasm.h"
#include <stdio.h>
#include <stdlib.h>

// MR
#define TEST_MR_DIRECT_64 1
#define TEST_MR_DIRECT_32 1
#define TEST_MR_DIRECT_16 1
#define TEST_MR_INDIRECT_64 1
#define TEST_MR_INDIRECT_32 1
#define TEST_MR_INDIRECT_16 1
#define TEST_MR_INDIRECT_BYTE_DISPLACED_64 1
#define TEST_MR_INDIRECT_BYTE_DISPLACED_32 1
#define TEST_MR_INDIRECT_BYTE_DISPLACED_16 1
#define TEST_MR_INDIRECT_DWORD_DISPLACED_64 1
#define TEST_MR_INDIRECT_DWORD_DISPLACED_32 1
#define TEST_MR_INDIRECT_DWORD_DISPLACED_16 1
#define TEST_MR_SIB_X1_64 1
#define TEST_MR_SIB_X1_32 1
#define TEST_MR_SIB_X1_16 1
#define TEST_MR_SIB_BYTE_DISPLACED_X2_64 1
#define TEST_MR_SIB_BYTE_DISPLACED_X2_32 1
#define TEST_MR_SIB_BYTE_DISPLACED_X2_16 1
#define TEST_MR_SIB_DWORD_DISPLACED_X8_64 1
#define TEST_MR_SIB_DWORD_DISPLACED_X8_32 1
#define TEST_MR_SIB_DWORD_DISPLACED_X8_16 1

// RM
#define TEST_RM_DIRECT_64 1
#define TEST_RM_DIRECT_32 1
#define TEST_RM_DIRECT_16 1
#define TEST_RM_DIRECT_8  1
#define TEST_RM_INDIRECT_64 1
#define TEST_RM_INDIRECT_32 1
#define TEST_RM_INDIRECT_16 1
#define TEST_RM_INDIRECT_BYTE_DISPLACED_64 1
#define TEST_RM_INDIRECT_BYTE_DISPLACED_32 1
#define TEST_RM_INDIRECT_BYTE_DISPLACED_16 1
#define TEST_RM_INDIRECT_DWORD_DISPLACED_64 1
#define TEST_RM_INDIRECT_DWORD_DISPLACED_32 1
#define TEST_RM_INDIRECT_DWORD_DISPLACED_16 1
#define TEST_RM_SIB_X1_64 1
#define TEST_RM_SIB_X1_32 1
#define TEST_RM_SIB_X1_16 1
#define TEST_RM_SIB_BYTE_DISPLACED_X2_64 1
#define TEST_RM_SIB_BYTE_DISPLACED_X2_32 1
#define TEST_RM_SIB_BYTE_DISPLACED_X2_16 1
#define TEST_RM_SIB_DWORD_DISPLACED_X8_64 1
#define TEST_RM_SIB_DWORD_DISPLACED_X8_32 1
#define TEST_RM_SIB_DWORD_DISPLACED_X8_16 1

// MI
#define TEST_MI_DIRECT 1
#define TEST_MI_INDIRECT 1
#define TEST_MI_INDIRECT_BYTE_DISPLACED 1
#define TEST_MI_INDIRECT_DWORD_DISPLACED 1
#define TEST_MI_SIB_X1_IMM8 1
#define TEST_MI_SIB_X2_IMM8 1
#define TEST_MI_SIB_X4_BYTE_DISPLACED_IMM8 1
#define TEST_MI_SIB_X8_DWORD_DISPLACED_IMM8 1

#define TEST_BSWAP 1

u8*
emit_bsf_rm_test(u8* stream)
{
    // RM Direct
#if TEST_RM_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if TEST_RM_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_direct(i, j));
        }
    }
#endif
#if TEST_RM_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_direct(i, j));
        }
    }
#endif

    // Indirect
#if TEST_RM_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect(j, i, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_32
    for(X64_Register j = EAX; j <= R15D; ++j)
    {
        for(X64_Register i = RAX; i <= R15D; ++i)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect(j, i, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_16
    for(X64_Register j = AX; j <= R15W; ++j)
    {
        for(X64_Register i = RAX; i <= R15; ++i)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect(j, i, 0, ADDR_WORDPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_RM_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect(j, i, 0x15, ADDR_WORDPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_RM_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect(j, i, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_bsf_rm_sib_test(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);
    
    // SIB INDIRECT X1
#if TEST_RM_SIB_X1_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_X1_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_X1_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_RM_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_RM_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_RM_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bsf(0, stream, mk_rm_indirect_sib(j, i, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_bswap_test(u8* stream)
{
#if TEST_BSWAP
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_bswap(0, stream, mk_o(i));
    }
#endif
    return stream;
}

u8*
emit_bt_mr_test(u8* stream)
{
    X64_BitTest_Instr instr = BITTEST_COMPLEMENT;

        // MR Direct
#if TEST_MR_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_direct(i, j));
        }
    }
#endif

    // Indirect
#if TEST_MR_INDIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_indirect(i, j, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_indirect(i, j, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_MR_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_indirect(i, j, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_indirect(i, j, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_MR_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_indirect(i, j, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_bt(0, stream, instr, mk_mr_indirect(i, j, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_bt_mi_test(u8* stream)
{
    X64_BitTest_Instr instr = BITTEST_COMPLEMENT;
    // Direct
#if TEST_MI_DIRECT
    for(X64_Register i = RAX; i <= R15W; ++i)
    {
        stream = emit_bt(0, stream, instr, mk_mi_direct(i, 0x12, 8));
    }
#endif

    // Indirect
#if TEST_MI_INDIRECT
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_bt(0, stream, instr, mk_mi_indirect(i, 0, ADDR_QWORDPTR, 0x12, 8));
        stream = emit_bt(0, stream, instr, mk_mi_indirect(i, 0, ADDR_DWORDPTR, 0x12, 8));
        stream = emit_bt(0, stream, instr, mk_mi_indirect(i, 0, ADDR_WORDPTR, 0x12, 8));
    }
#endif

    // Indirect byte displaced
#if TEST_MI_INDIRECT_BYTE_DISPLACED
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_bt(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_QWORDPTR, 0x12, 8));
        stream = emit_bt(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_DWORDPTR, 0x12, 8));
        stream = emit_bt(0, stream, instr, mk_mi_indirect(i, 0x15, ADDR_WORDPTR, 0x12, 8));
    }
#endif

    // Indirect dword displaced
#if TEST_MI_INDIRECT_DWORD_DISPLACED
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_bt(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_QWORDPTR, 0x12, 8));
        stream = emit_bt(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_DWORDPTR, 0x12, 8));
        stream = emit_bt(0, stream, instr, mk_mi_indirect(i, 0x15161718, ADDR_WORDPTR, 0x12, 8));
    }
#endif
    
    return stream;
}

u8*
emit_bt_mi_sib_test(u8* stream)
{
    X64_BitTest_Instr instr = BITTEST_SET;
    // SIB X1
#if TEST_MI_SIB_X1_IMM8
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = REG_NONE; j <= R15; ++j)
        {
            if(j == RSP) continue;
            
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_QWORDPTR, 0x12, 8));
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_DWORDPTR, 0x12, 8));
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X1, 0, ADDR_WORDPTR, 0x12, 8));
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
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_QWORDPTR, 0x12, 8));
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_DWORDPTR, 0x12, 8));
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X2, 0, ADDR_WORDPTR, 0x12, 8));
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
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_QWORDPTR, 0x12, 8));
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_DWORDPTR, 0x12, 8));
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X4, 0x15, ADDR_WORDPTR, 0x12, 8));
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
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_QWORDPTR, 0x12, 8));
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_DWORDPTR, 0x12, 8));
            stream = emit_bt(0, stream, instr, mk_mi_indirect_sib(i, j, SIB_X8, 0x15161718, ADDR_WORDPTR, 0x12, 8));
        }
    }
#endif
    return stream;
}

int main()
{
    #define FILENAME "test_bit.bin"
    FILE* out = fopen(FILENAME, "wb");
	u8* stream = (u8*)calloc(1, 1024*1024);
    u8* end = stream;
    {
        end = emit_bsf_rm_test(end);
        end = emit_bsf_rm_sib_test(end);
        end = emit_bswap_test(end);
        end = emit_bt_mr_test(end);
        end = emit_bt_mi_test(end);
        end = emit_bt_mi_sib_test(end);
    }

    fwrite(stream, 1, end - stream, out);
	fclose(out);

    system("objdump -D -Mintel,x86-64 -b binary -m i386 -w " FILENAME);
    //system("ndisasm -b 64 " FILENAME);
    return 0;
}