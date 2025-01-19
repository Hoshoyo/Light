@echo off

cat src/hoasm_header.h > hoasm.h
echo #ifdef HO_ASSEMBLER_IMPLEMENT >> hoasm.h
cat src/emit_base.c >> hoasm.h

echo: >> hoasm.h
cat src/instr_arithmetic.c >> hoasm.h
echo: >> hoasm.h
cat src/instr_bit.c >> hoasm.h
echo: >> hoasm.h
cat src/instr_cvt.c >> hoasm.h
echo: >> hoasm.h
cat src/instr_jmp.c >> hoasm.h
echo: >> hoasm.h
cat src/instr_mov.c >> hoasm.h
echo: >> hoasm.h
cat src/instr_mul.c >> hoasm.h
echo: >> hoasm.h
cat src/instr_shift.c >> hoasm.h
echo: >> hoasm.h
cat src/instr_sse.c >> hoasm.h
echo: >> hoasm.h
cat src/instr_stack.c >> hoasm.h
echo: >> hoasm.h
cat src/instr_str.c >> hoasm.h
echo: >> hoasm.h

echo: >> hoasm.h
echo #endif // HO_ASSEMBLER_IMPLEMENT >> hoasm.h