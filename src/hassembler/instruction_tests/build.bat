@echo off 

SET CompileLine=cl /Zi /nologo /I../..

pushd bin

IF "%1"=="jmp"   (%CompileLine% ../test_jmp.c -Fe:test_jmp.exe     && IF "%2"=="run" test_jmp.exe) ELSE ^
IF "%1"=="arith" (%CompileLine% ../test_arith.c -Fe:test_arith.exe && IF "%2"=="run" test_arith.exe) ELSE ^
IF "%1"=="bit"   (%CompileLine% ../test_bit.c -Fe:test_bit.exe     && IF "%2"=="run" test_bit.exe) ELSE ^
IF "%1"=="mov"   (%CompileLine% ../test_mov.c -Fe:test_mov.exe     && IF "%2"=="run" test_mov.exe) ELSE ^
IF "%1"=="cvt"   (%CompileLine% ../test_cvt.c -Fe:test_cvt.exe     && IF "%2"=="run" test_cvt.exe) ELSE ^
IF "%1"=="mul"   (%CompileLine% ../test_mul.c -Fe:test_mul.exe     && IF "%2"=="run" test_mul.exe) ELSE ^
IF "%1"=="shift" (%CompileLine% ../test_shift.c -Fe:test_shift.exe && IF "%2"=="run" test_shift.exe) ELSE ^
IF "%1"=="sse"   (%CompileLine% ../test_sse.c -Fe:test_sse.exe     && IF "%2"=="run" test_sse.exe) ELSE ^
IF "%1"=="str"   (%CompileLine% ../test_str.c -Fe:test_str.exe     && IF "%2"=="run" test_str.exe) ELSE ^
IF "%1"=="checker" (cl /Zi /nologo ../checker.c) ELSE ^
IF "%1"=="run" (test_%2.exe) ELSE ^
IF "%1"=="all" (%CompileLine% ../test_arith.c && ^
%CompileLine% ../test_jmp.c && ^
%CompileLine% ../test_arith.c && ^
%CompileLine% ../test_bit.c && ^
%CompileLine% ../test_mov.c && ^
%CompileLine% ../test_cvt.c && ^
%CompileLine% ../test_mul.c && ^
%CompileLine% ../test_shift.c && ^
%CompileLine% ../test_sse.c && ^
%CompileLine% ../test_str.c) ELSE ^
echo usage: build ^<^<type^>^|checker^|all^|run ^<type^>^> && ^
echo   - test types: && ^
echo     jmp && ^
echo     arith && ^
echo     bit && ^
echo     mov && ^
echo     cvt && ^
echo     mul && ^
echo     shift && ^
echo     sse && ^
echo     str

popd