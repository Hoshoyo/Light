CC=gcc
BINDIR=./bin
LINKFLAGS=-ldl $(BINDIR)/lightvm.a
LIGHTVMDIR=../src/light_vm

all: lightvm
	$(CC) -Iinclude -g -m64 ./src/*.c ./src/utils/*.c -o $(BINDIR)/light $(LINKFLAGS)

lightvm:
	cd ./bin; nasm -felf64 $(LIGHTVMDIR)/lvm.asm -o lvm.o
	cd ./bin; $(CC) -g -c $(LIGHTVMDIR)/lightvm.c $(LIGHTVMDIR)/lightvm_parser.c $(LIGHTVMDIR)/lightvm_print.c
	cd ./bin; ar rcs lightvm.a lightvm.o lightvm_parser.o lightvm_print.o lvm.o