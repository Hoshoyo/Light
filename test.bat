@echo off

REM run the test
pushd bin
call light ../test/basic/simple_sum.li -x86coff -o ../test/basic/bin/simple_sum.exe -q
call test.exe simple_sum ../test/basic/bin/simple_sum.exe - 47

call light ../test/basic/simple_arithmetic.li -x86coff -o ../test/basic/bin/simple_arithmetic.exe -q
call test.exe simple_arithmetic ../test/basic/bin/simple_arithmetic.exe - 19

call light ../test/basic/simple_unary_minus.li -x86coff -o ../test/basic/bin/simple_unary_minus.exe -q
call test.exe simple_unary_minus ../test/basic/bin/simple_unary_minus.exe - 1

call light ../test/basic/simple_bitwise.li -x86coff -o ../test/basic/bin/simple_bitwise.exe -q
call test.exe simple_bitwise ../test/basic/bin/simple_bitwise.exe - 872

call light ../test/basic/simple_if_true.li -x86coff -o ../test/basic/bin/simple_if_true.exe -q
call test.exe simple_if_true ../test/basic/bin/simple_if_true.exe - 5

call light ../test/basic/simple_while.li -x86coff -o ../test/basic/bin/simple_while.exe -q
call test.exe simple_while ../test/basic/bin/simple_while.exe - 45

call light ../test/basic/simple_for.li -x86coff -o ../test/basic/bin/simple_for.exe -q
call test.exe simple_for ../test/basic/bin/simple_for.exe - 900

call light ../test/basic/simple_break.li -x86coff -o ../test/basic/bin/simple_break.exe -q
call test.exe simple_break ../test/basic/bin/simple_break.exe - 1001

call light ../test/basic/simple_continue.li -x86coff -o ../test/basic/bin/simple_continue.exe -q
call test.exe simple_continue ../test/basic/bin/simple_continue.exe - 50

call light ../test/basic/simple_proc_call.li -x86coff -o ../test/basic/bin/simple_proc_call.exe -q
call test.exe simple_proc_call ../test/basic/bin/simple_proc_call.exe - 42

call light ../test/basic/simple_proc_args.li -x86coff -o ../test/basic/bin/simple_proc_args.exe -q
call test.exe simple_proc_args ../test/basic/bin/simple_proc_args.exe - 56

call light ../test/basic/simple_recursion.li -x86coff -o ../test/basic/bin/simple_recursion.exe -q
call test.exe simple_recursion ../test/basic/bin/simple_recursion.exe - 120

call light ../test/basic/simple_struct.li -x86coff -o ../test/basic/bin/simple_struct.exe -q
call test.exe simple_struct ../test/basic/bin/simple_struct.exe - 15

call light ../test/basic/simple_union.li -x86coff -o ../test/basic/bin/simple_union.exe -q
call test.exe simple_union ../test/basic/bin/simple_union.exe - 305419896

call light ../test/basic/simple_array.li -x86coff -o ../test/basic/bin/simple_array.exe -q
call test.exe simple_array ../test/basic/bin/simple_array.exe - 10

call light ../test/basic/simple_array_literal.li -x86coff -o ../test/basic/bin/simple_array_literal.exe -q
call test.exe simple_array_literal ../test/basic/bin/simple_array_literal.exe - 10

call light ../test/basic/simple_multidim_array.li -x86coff -o ../test/basic/bin/simple_multidim_array.exe -q
call test.exe simple_multidim_array ../test/basic/bin/simple_multidim_array.exe - 7

call light ../test/basic/simple_sizeof.li -x86coff -o ../test/basic/bin/simple_sizeof.exe -q
call test.exe simple_sizeof ../test/basic/bin/simple_sizeof.exe - 8

call light ../test/basic/simple_nested_block.li -x86coff -o ../test/basic/bin/simple_nested_block.exe -q
call test.exe simple_nested_block ../test/basic/bin/simple_nested_block.exe - 35

call light ../test/basic/simple_dereference.li -x86coff -o ../test/basic/bin/simple_dereference.exe -q
call test.exe simple_dereference ../test/basic/bin/simple_dereference.exe - 12
popd