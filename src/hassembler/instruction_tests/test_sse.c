#define HO_ASSEMBLER_IMPLEMENT
#include "hoasm.h"
#include <stdio.h>
#include <stdlib.h>

u8*
emit_test_sse(u8* stream)
{
    stream = emit_cvttps2pi(0, stream, mk_rm_direct(XMM0, RAX));
    stream = emit_cvttps2pi(0, stream, mk_rm_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvttps2pi(0, stream, mk_rm_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvttss2si(0, stream, mk_a_direct(XMM0, RAX));
    stream = emit_cvttss2si(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvttss2si(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvttss2si(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvttsd2si(0, stream, mk_a_direct(XMM0, RAX));
    stream = emit_cvttsd2si(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvttsd2si(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvttsd2si(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvttps2dq(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_cvttps2dq(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvttps2dq(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvttpd2pi(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_cvttpd2pi(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvttpd2pi(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvttpd2dq(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_cvttpd2dq(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvttpd2dq(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtsi2sd(0, stream, mk_a_direct(XMM0, RAX));
    stream = emit_cvtsi2sd(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtsi2sd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtsi2sd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtsd2si(0, stream, mk_a_direct(XMM0, RAX));
    stream = emit_cvtsd2si(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtsd2si(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtsd2si(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtps2pd(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtps2pd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtps2pd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtps2dq(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtps2dq(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtps2dq(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtpd2ps(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtpd2ps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtpd2ps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtpd2dq(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtpd2dq(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtpd2dq(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtdq2ps(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtdq2ps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtdq2ps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtdq2pd(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtdq2pd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtdq2pd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtss2sd(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtss2sd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtss2sd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtsd2ss(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtsd2ss(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtsd2ss(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtsi2ss(0, stream, mk_a_direct(XMM0, RAX));
    stream = emit_cvtsi2ss(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtsi2ss(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtsi2ss(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtss2si(0, stream, mk_a_direct(XMM0, RAX));
    stream = emit_cvtss2si(0, stream, mk_a_direct(XMM0, EAX));
    stream = emit_cvtss2si(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtss2si(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_xorps(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_xorps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_xorps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));
    stream = emit_xorpd(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_xorpd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_xorpd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_orps(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_orps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_orps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));
    stream = emit_orpd(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_orpd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_orpd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movlps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movlps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movlpd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movlpd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movhps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movhps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movhpd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movhpd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movups(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_movups(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movups(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movupd(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_movupd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movupd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movss(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_movss(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movss(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movsd(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_movsd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movsd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movaps(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_movaps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movaps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));
    stream = emit_movaps(0, stream, mk_b_direct(XMM0, XMM1));
    stream = emit_movaps(0, stream, mk_b_indirect(XMM0, RAX, 0, 64));
    stream = emit_movaps(0, stream, mk_b_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_movapd(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_movapd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_movapd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_divps(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_divps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_divps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_divpd(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_divpd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_divpd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtps2pi(0, stream, mk_rm_direct(XMM0, XMM1));
    stream = emit_cvtps2pi(0, stream, mk_rm_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtps2pi(0, stream, mk_rm_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtpd2pi(0, stream, mk_rm_direct(XMM0, XMM1));
    stream = emit_cvtpd2pi(0, stream, mk_rm_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtpd2pi(0, stream, mk_rm_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtpi2ps(0, stream, mk_rm_direct(XMM0, XMM1));
    stream = emit_cvtpi2ps(0, stream, mk_rm_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtpi2ps(0, stream, mk_rm_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_cvtpi2pd(0, stream, mk_rm_direct(XMM0, XMM1));
    stream = emit_cvtpi2pd(0, stream, mk_rm_indirect(XMM0, RAX, 0, 64));
    stream = emit_cvtpi2pd(0, stream, mk_rm_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_comiss(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_comiss(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_comiss(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));
    stream = emit_comisd(0, stream, mk_a_direct(XMM0, XMM1));

    for (X64_SSE_Compare_Flag i = SSE_CMP_EQ; i <= SSE_CMP_ORD; ++i)
        stream = emit_cmpsd(0, stream, mk_a_direct(XMM0, XMM1), i);
    for (X64_SSE_Compare_Flag i = SSE_CMP_EQ; i <= SSE_CMP_ORD; ++i)
        stream = emit_cmpss(0, stream, mk_a_direct(XMM0, XMM1), i);
    for (X64_SSE_Compare_Flag i = SSE_CMP_EQ; i <= SSE_CMP_ORD; ++i)
        stream = emit_cmpps(0, stream, mk_a_direct(XMM0, XMM1), i);
    for (X64_SSE_Compare_Flag i = SSE_CMP_EQ; i <= SSE_CMP_ORD; ++i)
        stream = emit_cmppd(0, stream, mk_a_direct(XMM0, XMM1), i);
    
    stream = emit_andps(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_andps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_andps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_addps(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_addps(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_addps(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_addss(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_addss(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_addss(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_addpd(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_addpd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_addpd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    stream = emit_addsd(0, stream, mk_a_direct(XMM0, XMM1));
    stream = emit_addsd(0, stream, mk_a_indirect(XMM0, RAX, 0, 64));
    stream = emit_addsd(0, stream, mk_a_indirect_sib(XMM0, RAX, RCX, SIB_X1, 0, 64));

    for (X64_SSE_Compare_Flag i = SSE_CMP_EQ; i <= SSE_CMP_ORD; ++i)
        stream = emit_cmpsd(0, stream, mk_a_direct(XMM0, XMM1), i);
    for (X64_SSE_Compare_Flag i = SSE_CMP_EQ; i <= SSE_CMP_ORD; ++i)
        stream = emit_cmpss(0, stream, mk_a_direct(XMM0, XMM1), i);
    for (X64_SSE_Compare_Flag i = SSE_CMP_EQ; i <= SSE_CMP_ORD; ++i)
        stream = emit_cmpps(0, stream, mk_a_direct(XMM0, XMM1), i);
    for (X64_SSE_Compare_Flag i = SSE_CMP_EQ; i <= SSE_CMP_ORD; ++i)
        stream = emit_cmppd(0, stream, mk_a_direct(XMM0, XMM1), i);
    
    return stream;
}

int main()
{
    #define FILENAME "test_sse.bin"
    FILE* out = fopen(FILENAME, "wb");
	u8* stream = (u8*)calloc(1, 1024*1024);
    u8* end = stream;
    {
        end = emit_test_sse(end);
    }
    fwrite(stream, 1, end - stream, out);
	fclose(out);

    system("objdump -D -Mintel,x86-64 -b binary -m i386 -w " FILENAME);
    
    return 0;
}