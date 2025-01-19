#define HO_ASSEMBLER_IMPLEMENT
#include "hoasm.h"
#include <stdio.h>
#include <stdlib.h>

#define TEST_JMP 1
#define TEST_LIDT 1
#define TEST_LGDT 1

u8*
emit_ret_test(u8* stream)
{
#if TEST_JMP
    stream = emit_ret(0, stream, RET_NEAR, 0);
    stream = emit_ret(0, stream, RET_NEAR, 0x15);
    stream = emit_ret(0, stream, RET_FAR, 0);
    stream = emit_ret(0, stream, RET_FAR, 0x15);
#endif
    return stream;
}

u8*
emit_push_test(u8* stream)
{
    // ZO
#if TEST_JMP
    stream = emit_push(0, stream, mk_zo_reg(FS));
    stream = emit_push(0, stream, mk_zo_reg(GS));
#endif

    // Immediate
#if TEST_JMP
    stream = emit_push(0, stream, mk_i(0x16, 8));
    stream = emit_push(0, stream, mk_i(0x1617, 16));
    stream = emit_push(0, stream, mk_i(0x16171819, 32));
#endif

    // O
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_push(0, stream, mk_o(i));
    for(X64_Register i = AX; i <= R15W; ++i)
        stream = emit_push(0, stream, mk_o(i));
#endif

    // M direct
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_push(0, stream, mk_m_direct(i));
    for(X64_Register i = AX; i <= R15W; ++i)
        stream = emit_push(0, stream, mk_m_direct(i));
#endif
    // M indirect
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_push(0, stream, mk_m_indirect(i, 0, ADDR_QWORDPTR));
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_push(0, stream, mk_m_indirect(i, 0, ADDR_WORDPTR));
#endif
    // M indirect byte displaced
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_push(0, stream, mk_m_indirect(i, 0x15, ADDR_QWORDPTR));
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_push(0, stream, mk_m_indirect(i, 0x15, ADDR_WORDPTR));
#endif
    // M indirect dword displaced
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_push(0, stream, mk_m_indirect(i, 0x15161718, ADDR_QWORDPTR));
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_push(0, stream, mk_m_indirect(i, 0x15161718, ADDR_WORDPTR));
#endif
    return stream;
}

u8*
emit_push_sib_test(u8* stream)
{
    X64_Register index = R13;
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_push(0, stream, mk_m_indirect_sib(i, index, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_push(0, stream, mk_m_indirect_sib(i, index, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_push(0, stream, mk_m_indirect_sib(i, index, SIB_X4, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_push(0, stream, mk_m_indirect_sib(i, index, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_pop_test(u8* stream)
{
    // ZO
#if TEST_JMP
    stream = emit_pop(0, stream, mk_zo_reg_bitsize(FS, 16));
    stream = emit_pop(0, stream, mk_zo_reg_bitsize(GS, 16));
    stream = emit_pop(0, stream, mk_zo_reg_bitsize(FS, 64));
    stream = emit_pop(0, stream, mk_zo_reg_bitsize(GS, 64));
#endif

    // O
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_pop(0, stream, mk_o(i));
    for(X64_Register i = AX; i <= R15W; ++i)
        stream = emit_pop(0, stream, mk_o(i));
#endif

    // M direct
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_pop(0, stream, mk_m_direct(i));
    for(X64_Register i = AX; i <= R15W; ++i)
        stream = emit_pop(0, stream, mk_m_direct(i));
#endif
    // M indirect
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_pop(0, stream, mk_m_indirect(i, 0, ADDR_QWORDPTR));
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_pop(0, stream, mk_m_indirect(i, 0, ADDR_WORDPTR));
#endif
    // M indirect byte displaced
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_pop(0, stream, mk_m_indirect(i, 0x15, ADDR_QWORDPTR));
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_pop(0, stream, mk_m_indirect(i, 0x15, ADDR_WORDPTR));
#endif
    // M indirect dword displaced
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_pop(0, stream, mk_m_indirect(i, 0x15161718, ADDR_QWORDPTR));
    for(X64_Register i = RAX; i <= R15; ++i)
        stream = emit_pop(0, stream, mk_m_indirect(i, 0x15161718, ADDR_WORDPTR));
#endif
    return stream;
}

u8*
emit_pop_sib_test(u8* stream)
{
    X64_Register index = R13;
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_pop(0, stream, mk_m_indirect_sib(i, index, SIB_X1, 0, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_pop(0, stream, mk_m_indirect_sib(i, index, SIB_X2, 0x15, ADDR_WORDPTR));
        }
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_pop(0, stream, mk_m_indirect_sib(i, index, SIB_X4, 0x15161718, ADDR_QWORDPTR));
        }
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        for(X64_Register j = RAX; j <= R15; ++j)
        {
            stream = emit_pop(0, stream, mk_m_indirect_sib(i, index, SIB_X8, 0x15161718, ADDR_WORDPTR));
        }
    }
#endif
    return stream;
}

u8*
emit_jmp_test(u8* stream)
{
#if TEST_JMP
    stream = emit_jmp(0, stream, mk_d(0x16));
    stream = emit_jmp(0, stream, mk_d(0x16171810));
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_jmp(0, stream, mk_m_direct(i));
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_jmp(0, stream, mk_m_indirect(i, 0, ADDR_QWORDPTR));
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_jmp(0, stream, mk_m_indirect(i, 0x15, ADDR_QWORDPTR));
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_jmp(0, stream, mk_m_indirect(i, 0x15161718, ADDR_QWORDPTR));
    }
#endif

#if TEST_JMP
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_fjmp(0, stream, mk_m_indirect(i, 0, ADDR_QWORDPTR));
    }
#endif
    return stream;
}

u8*
emit_call_test(u8* stream)
{
#if TEST_JMP
    stream = emit_call(0, stream, mk_d(0x16171810));
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_call(0, stream, mk_m_direct(i));
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_call(0, stream, mk_m_indirect(i, 0, ADDR_QWORDPTR));
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_call(0, stream, mk_m_indirect(i, 0x15, ADDR_QWORDPTR));
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15; ++i)
    {
        stream = emit_call(0, stream, mk_m_indirect(i, 0x15161718, ADDR_QWORDPTR));
    }
#endif
#if TEST_JMP
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_fcall(0, stream, mk_m_indirect(i, 0, ADDR_QWORDPTR));
    }
#endif
    return stream;
}

u8*
emit_lidt_test(u8* stream)
{
#if TEST_LIDT
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_lidt(0, stream, mk_m_indirect(i, 0, ADDR_QWORDPTR));
    }
#endif
#if TEST_LIDT
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_lidt(0, stream, mk_m_indirect(i, 0x15, ADDR_QWORDPTR));
    }
#endif
#if TEST_LIDT
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_lidt(0, stream, mk_m_indirect(i, 0x15161718, ADDR_QWORDPTR));
    }
#endif

#if TEST_LGDT
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_lgdt(0, stream, mk_m_indirect(i, 0, ADDR_QWORDPTR));
    }
#endif
#if TEST_LGDT
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_lgdt(0, stream, mk_m_indirect(i, 0x15, ADDR_QWORDPTR));
    }
#endif
#if TEST_LGDT
    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_lgdt(0, stream, mk_m_indirect(i, 0x15161718, ADDR_QWORDPTR));
    }
#endif
    return stream;
}

u8*
emit_loopcc_test(u8* stream)
{
    stream = emit_loopcc(0, stream, LOOP, 10);
    stream = emit_loopcc(0, stream, LOOPE, -128);
    stream = emit_loopcc(0, stream, LOOPNE, -128);
    stream = emit_loopcc(0, stream, LOOP, 127);
    stream = emit_loopcc(0, stream, LOOPE, 127);
    stream = emit_loopcc(0, stream, LOOPNE, 127);
    return stream;
}

u8*
emit_enter_test(u8* stream)
{
    stream = emit_enter(0, stream, 0x1234, 0, false);
    stream = emit_enter(0, stream, 0x1234, 1, false);
    stream = emit_enter(0, stream, 0x1234, 25, false);
    stream = emit_enter(0, stream, 0x1234, 255, false);

    stream = emit_enter(0, stream, 0x1234, 0, true);
    stream = emit_enter(0, stream, 0x1234, 1, true);
    stream = emit_enter(0, stream, 0x1234, 25, true);
    stream = emit_enter(0, stream, 0x1234, 255, true);
    return stream;
}

u8*
emit_in_test(u8* stream)
{
    stream = emit_in(0, stream, mk_i(0x15, 8));
    stream = emit_in(0, stream, mk_i(0, 8));
    stream = emit_in(0, stream, mk_i_reg(AL, 0x15, 8));
    stream = emit_in(0, stream, mk_i_reg(AX, 0x16, 8));
    stream = emit_in(0, stream, mk_i_reg(EAX, 0x17, 8));

    stream = emit_in(0, stream, mk_zo_bitsize(8));
    stream = emit_in(0, stream, mk_zo_bitsize(16));
    stream = emit_in(0, stream, mk_zo_bitsize(32));
    return stream;
}

u8*
emit_out_test(u8* stream)
{
    stream = emit_out(0, stream, mk_i(0x15, 8));
    stream = emit_out(0, stream, mk_i(0, 8));
    stream = emit_out(0, stream, mk_i_reg(AL, 0x15, 8));
    stream = emit_out(0, stream, mk_i_reg(AX, 0x16, 8));
    stream = emit_out(0, stream, mk_i_reg(EAX, 0x17, 8));

    stream = emit_out(0, stream, mk_zo_bitsize(8));
    stream = emit_out(0, stream, mk_zo_bitsize(16));
    stream = emit_out(0, stream, mk_zo_bitsize(32));
    return stream;
}

u8*
emit_ud_test(u8* stream)
{
    stream = emit_ud(0, stream, UD2, mk_zo());
    // Docs says that ud0 and ud1 should have a ModRM byte, 
    // but the disassembler shows another instruction after
    // the ud's, so we keep it with the zo.
    stream = emit_ud(0, stream, UD0, mk_zo());
    stream = emit_ud(0, stream, UD1, mk_zo());
    return stream;
}

int main()
{
    #define FILENAME "test_jmp.bin"
    FILE* out = fopen(FILENAME, "wb");
	u8* stream = (u8*)calloc(1, 1024*1024);
    u8* end = stream;
    {
        end = emit_ret_test(end);
        end = emit_push_test(end);
        end = emit_push_sib_test(end);
        end = emit_pop_test(end);
        end = emit_pop_sib_test(end);
        end = emit_jmp_test(end);
        end = emit_call_test(end);
        end = emit_lidt_test(end);
        end = emit_loopcc_test(end);
        end = emit_enter_test(end);
        end = emit_in_test(end);
        end = emit_out_test(end);
        end = emit_ud_test(end);        
    }

    fwrite(stream, 1, end - stream, out);
	fclose(out);

    system("objdump -D -Mintel,x86-64 -b binary -m i386 -w " FILENAME);
    //system("ndisasm -b 64 " FILENAME);
    return 0;
}