all:
	@nasm -felf64 lvm.asm
	@gcc -Wall -g *.c lvm.o -o lightvm -ldl -fno-strict-aliasing
	@./lightvm

lib:
	nasm -felf64 lvm.asm
	gcc -g -c lightvm.c lightvm_parser.c lightvm_print.c
	ar rcs lightvm.a lightvm.o lightvm_parser.o lightvm_print.o lvm.o

clean:
	rm *.o