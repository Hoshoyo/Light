@echo off

if not exist bin mkdir bin
pushd bin
call ml64 /nologo /c /Fo./lvm.obj ../src/light_vm/lvm_masm.asm
call cl /DLIGHTMAIN=1 /MT /nologo /Zi /I../include ../src/light_vm/lightvm.c ../src/light_vm/lightvm_parser.c ../src/light_vm/lightvm_print.c ../src/*.c ../src/utils/*.c ../src/backend/c/*.c ../src/backend/x86/*.c ../src/backend/*.c ../src/backend/lvm/*.c /Felight.exe /link kernel32.lib lvm.obj
popd
