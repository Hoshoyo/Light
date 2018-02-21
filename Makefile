CC=g++
CFLAGS=-I./include -g -I./src -std=c++11 -m64 -w -fpermissive
SRCDIR=./src
BINDIR=./bin/gcc

all: extcall 
	mkdir -p ./bin/gcc
	$(CC) $(CFLAGS) $(SRCDIR)/ast.cpp $(SRCDIR)/main.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/util.cpp $(SRCDIR)/memory.cpp $(SRCDIR)/type.cpp $(SRCDIR)/parser.cpp $(SRCDIR)/semantic.cpp $(SRCDIR)/symbol_table.cpp $(SRCDIR)/interpreter.cpp $(SRCDIR)/hash_table.cpp -o $(BINDIR)/light $(BINDIR)/external_caller.o 

extcall: 
	nasm -g -f elf64 -o $(BINDIR)/external_caller.o $(SRCDIR)/external_caller_nasm.asm
