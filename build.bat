@echo off

pushd bin
set ASSEMBLER_FILES=../src/assembler/instr_arith.c ../src/assembler/instr_float.c ../src/assembler/instr_mov.c ../src/assembler/instr_mul.c ../src/assembler/instr_shift.c ../src/assembler/instr_jmp.c ../src/assembler/instr_cmov.c
call ml64 /nologo /c /Fo./lvm.obj ../src/light_vm/lvm_masm.asm
call cl /DLIGHTMAIN=1 /MT /nologo /Zi /I../include ../src/light_vm/lightvm.c ../src/light_vm/lightvm_parser.c ../src/light_vm/lightvm_print.c ../src/*.c ../src/utils/*.c ../src/backend/c/*.c ../src/backend/x86/*.c ../src/backend/*.c ../src/backend/lvm/*.c %ASSEMBLER_FILES% /Felight.exe /link kernel32.lib lvm.obj

call cl /nologo /Zi ../test/test.c
popd