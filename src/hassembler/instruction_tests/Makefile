all:
	gcc -o bin/test_mul test_mul.c -I..
	gcc -o bin/test_arith test_arith.c -I..
	gcc -o bin/test_mov test_mov.c -I..
	gcc -o bin/test_shift test_shift.c -I..
	gcc -o bin/test_jmp test_jmp.c -I..
	gcc -o bin/test_sse test_sse.c -I..
	gcc -o bin/test_bit test_bit.c -I..
	gcc -o bin/test_cvt test_cvt.c -I..
check: checker
	./bin/checker
checker:
	gcc checker.c -o bin/checker