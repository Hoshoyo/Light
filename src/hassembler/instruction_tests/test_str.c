#define HO_ASSEMBLER_IMPLEMENT
#include "hoasm.h"
#include <stdio.h>
#include <stdlib.h>

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

u8*
emit_cmps_test(u8* stream)
{
    stream = emit_cmps(0, stream, ADDR_BYTEPTR);
    stream = emit_cmps(0, stream, ADDR_WORDPTR);
    stream = emit_cmps(0, stream, ADDR_DWORDPTR);
    stream = emit_cmps(0, stream, ADDR_QWORDPTR);

    return stream;
}

u8*
emit_scas_test(u8* stream)
{
    stream = emit_scas(0, stream, ADDR_BYTEPTR);
    stream = emit_scas(0, stream, ADDR_WORDPTR);
    stream = emit_scas(0, stream, ADDR_DWORDPTR);
    stream = emit_scas(0, stream, ADDR_QWORDPTR);

    return stream;
}

u8*
emit_stos_test(u8* stream)
{
    stream = emit_stos(0, stream, ADDR_BYTEPTR);
    stream = emit_stos(0, stream, ADDR_WORDPTR);
    stream = emit_stos(0, stream, ADDR_DWORDPTR);
    stream = emit_stos(0, stream, ADDR_QWORDPTR);

    return stream;
}

u8*
emit_lods_test(u8* stream)
{
    stream = emit_lods(0, stream, ADDR_BYTEPTR);
    stream = emit_lods(0, stream, ADDR_WORDPTR);
    stream = emit_lods(0, stream, ADDR_DWORDPTR);
    stream = emit_lods(0, stream, ADDR_QWORDPTR);

    return stream;
}

u8*
emit_movs_test(u8* stream)
{
    stream = emit_movs(0, stream, ADDR_BYTEPTR);
    stream = emit_movs(0, stream, ADDR_WORDPTR);
    stream = emit_movs(0, stream, ADDR_DWORDPTR);
    stream = emit_movs(0, stream, ADDR_QWORDPTR);

    return stream;
}

u8*
emit_ins_test(u8* stream)
{
    stream = emit_ins(0, stream, ADDR_BYTEPTR);
    stream = emit_ins(0, stream, ADDR_WORDPTR);
    stream = emit_ins(0, stream, ADDR_DWORDPTR);

    return stream;
}

u8*
emit_outs_test(u8* stream)
{
    stream = emit_outs(0, stream, ADDR_BYTEPTR);
    stream = emit_outs(0, stream, ADDR_WORDPTR);
    stream = emit_outs(0, stream, ADDR_DWORDPTR);

    return stream;
}

u8*
emit_cmpxchg_test(u8* stream)
{
    // MR Direct
#if TEST_MR_DIRECT_64
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_32
    for(X64_Register i = EAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_16
    for(X64_Register i = AX; i <= R15W; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_direct(i, j));
        }
    }
#endif
#if TEST_MR_DIRECT_8
    for(X64_Register i = AL; i <= DIL; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_direct(i, j));
        }
    }
#endif

    // Indirect
#if TEST_MR_INDIRECT_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect byte displaced
#if TEST_MR_INDIRECT_BYTE_DISPLACED_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_BYTE_DISPLACED_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // Indirect dword displaced
#if TEST_MR_INDIRECT_DWORD_DISPLACED_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_INDIRECT_DWORD_DISPLACED_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect(i, j, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif

    return stream;
}

u8*
emit_cmpxchg_sib_test(u8* stream)
{
    X64_Register index_reg = R13;
    assert(index_reg != RSP);
    
    // SIB INDIRECT X1
#if TEST_MR_SIB_X1_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_X1_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X1, 0, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_BYTE_DISPLACED X2
#if TEST_MR_SIB_BYTE_DISPLACED_X2_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_BYTE_DISPLACED_X2_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X2, 0x15, ADDR_BYTEPTR));
        }
    }
#endif

    // SIB INDIRECT_DWORD_DISPLACED X8
#if TEST_MR_SIB_DWORD_DISPLACED_X8_64
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_32
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = EAX; j <= R15D; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_DWORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_16
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AX; j <= R15W; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_MR_SIB_DWORD_DISPLACED_X8_8
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        for(X64_Register j = AL; j <= DIL; ++j)
        {
            stream = emit_cmpxchg(0, stream, mk_mr_indirect_sib(i, j, index_reg, SIB_X8, 0x15161718, ADDR_BYTEPTR));
        }
    }
#endif
    return stream;
}

int main()
{
    #define FILENAME "test_str.bin"
    FILE* out = fopen(FILENAME, "wb");
	u8* stream = (u8*)calloc(1, 1024*1024);
    u8* end = stream;
    {
        end = emit_cmps_test(end);
        end = emit_cmpxchg_test(end);
        end = emit_cmpxchg_sib_test(end);
        end = emit_ins_test(end);
        end = emit_outs_test(end);
        end = emit_scas_test(end);
        end = emit_stos_test(end);
        end = emit_lods_test(end);
        end = emit_movs_test(end);
    }

    fwrite(stream, 1, end - stream, out);
	fclose(out);

    system("objdump -D -Mintel,x86-64 -b binary -m i386 -w " FILENAME);
    //system("ndisasm -b 64 " FILENAME);
    return 0;
}