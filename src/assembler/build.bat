@echo off

pushd bin
call cl /nologo /Zi ../*.c /Fe:asmtest.exe
popd