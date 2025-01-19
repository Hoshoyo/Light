#define HO_ASSEMBLER_IMPLEMENT
#include "hoasm.h"
#include <stdio.h>
#include <stdlib.h>

u8*
emit_mul_test(u8* stream)
{
    for(X64_Register i = RAX; i <= DIL; ++i)
    {
        stream = emit_mul(0, stream, mk_m_direct(i));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_mul(0, stream, mk_m_indirect(i, 0, ADDR_QWORDPTR));
        stream = emit_mul(0, stream, mk_m_indirect(i, 0, ADDR_DWORDPTR));
        stream = emit_mul(0, stream, mk_m_indirect(i, 0, ADDR_WORDPTR));
        stream = emit_mul(0, stream, mk_m_indirect(i, 0, ADDR_BYTEPTR));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_mul(0, stream, mk_m_indirect(i, 0x15, ADDR_QWORDPTR));
        stream = emit_mul(0, stream, mk_m_indirect(i, 0x15, ADDR_DWORDPTR));
        stream = emit_mul(0, stream, mk_m_indirect(i, 0x15, ADDR_WORDPTR));
        stream = emit_mul(0, stream, mk_m_indirect(i, 0x15, ADDR_BYTEPTR));
    }

    for(X64_Register i = RAX; i <= R15D; ++i)
    {
        stream = emit_mul(0, stream, mk_m_indirect(i, 0x15161718, ADDR_QWORDPTR));
        stream = emit_mul(0, stream, mk_m_indirect(i, 0x15161718, ADDR_DWORDPTR));
        stream = emit_mul(0, stream, mk_m_indirect(i, 0x15161718, ADDR_WORDPTR));
        stream = emit_mul(0, stream, mk_m_indirect(i, 0x15161718, ADDR_BYTEPTR));
    }

    return stream;
}

int main()
{
    #define FILENAME "test_mul.bin"
    FILE* out = fopen(FILENAME, "wb");
	u8* stream = (u8*)calloc(1, 1024*1024);
    u8* end = emit_mul_test(stream);

    fwrite(stream, 1, end - stream, out);
	fclose(out);

    system("objdump -D -Mintel,x86-64 -b binary -m i386 -w " FILENAME);
    return 0;
}