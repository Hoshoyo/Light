@echo off

pushd bin
call ml64 /c ../src/light_vm/lvm_masm.asm
call cl -c -nologo ../src/light_vm/lightvm.c ../src/light_vm/lightvm_print.c ../src/light_vm/lightvm_parser.c /link kernel32.lib
call cl -Zi -nologo -I../include ../src/*.c ../src/utils/*.c /link kernel32.lib lvm_masm.obj /OUT:light.exe
popd