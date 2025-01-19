#include <stdio.h>
#include <stdlib.h>
#define HO_ASSEMBLER_IMPLEMENT
#include "../hoasm.h"
#include <windows.h>

char*
sumto10(char* at)
{
    // returns the sum from 1 to 10
    /*
        xor eax, eax
        mov ecx, 10
    start:
        cmp ecx, 0
        je end
        add eax, ecx
        dec ecx
        jmp start
    end:
        ret
    */
    Instr_Emit_Result to_end;
    Instr_Emit_Result jmp_up;
    at = emit_arithmetic(0, at, ARITH_XOR, mk_mr_direct(EAX, EAX));
    at = emit_mov(0, at, mk_oi(ECX, 10, 32));
    char* start = at;
    at = emit_arithmetic(0, at, ARITH_CMP, mk_oi(ECX, 0, 8));
    u8* to_end_at = at;
    at = emit_jcc(&to_end, at, JE, 0, 8);
    at = emit_arithmetic(0, at, ARITH_ADD, mk_mr_direct(EAX, ECX));
    at = emit_dec(0, at, mk_m_direct(ECX));
    at = emit_jmp(&jmp_up, at, mk_d(start - at));
    *(to_end_at + to_end.immediate_offset) += (at - to_end_at);
    at = emit_ret(0, at, RET_NEAR, 0);
    return at;
}

typedef int __stdcall sumtype();

int main()
{
    FILE* out = fopen("example.txt", "wb");
	char* stream = VirtualAlloc(0, 4096, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    char* at = stream;

    sumtype* addr = (sumtype*)at;
    at = sumto10(at);

    printf("Result: %d\n", addr());
    
    fwrite(stream, 1, at - stream, out);
	fclose(out);

    system("objdump -D -Mintel,x86-64 -b binary -m i386 -w example.txt");
    return 0;
}