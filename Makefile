CC=gcc
BINDIR=./bin
LINKFLAGS=-ldl $(BINDIR)/lightvm.a
LIGHTVMDIR=../src/light_vm
DISABLE_WARNINGS=-Wno-unused-variable -Wno-unused-function

all: lightvm
	$(CC) -DLIGHTMAIN -Iinclude -Wall $(DISABLE_WARNINGS) -g -m64 ./src/*.c ./src/utils/*.c ./src/backend/c/*.c ./src/backend/x86/*.c ./src/backend/*.c ./src/backend/lvm/*.c -o $(BINDIR)/light $(LINKFLAGS)

lightvm:
	cd ./bin; nasm -felf64 $(LIGHTVMDIR)/lvm.asm -o lvm.o
	cd ./bin; $(CC) -g -c -I../include $(LIGHTVMDIR)/lightvm.c $(LIGHTVMDIR)/lightvm_parser.c $(LIGHTVMDIR)/lightvm_print.c
	cd ./bin; ar rcs lightvm.a lightvm.o lightvm_parser.o lightvm_print.o lvm.o

gen:
	objdump -D -b binary -M intel -m i386 test.bin
