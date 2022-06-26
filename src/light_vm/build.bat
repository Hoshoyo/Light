@echo off

pushd bin
call ml64 -nologo -c ../lvm_masm.asm
call cl -nologo -W3 -Zi ../*.c -I../../../include lvm_masm.obj /link kernel32.lib user32.lib
popd
