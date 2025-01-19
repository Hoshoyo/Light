@echo off

pushd bin
test_jmp.exe > jmp_result.res
test_arith.exe > arith_result.res
test_bit.exe > bit_result.res
test_mov.exe > mov_result.res
test_mul.exe > mul_result.res
test_shift.exe > shift_result.res
test_cvt.exe > cvt_result.res
test_sse.exe > sse_result.res
test_str.exe > str_result.res
popd