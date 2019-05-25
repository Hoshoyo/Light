all:
	@nasm -felf64 lvm.asm
	@gcc -g *.c lvm.o -o lightvm -ldl
	@./lightvm