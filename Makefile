CC=g++
CFLAGS=-I./include -g -I./src -std=c++11 -m64 -w -fpermissive
SRCDIR=./src
BINDIR=./bin/gcc

all: extcall 
	mkdir -p ./bin/gcc
	$(CC) $(CFLAGS) $(SRCDIR)/ast.cpp $(SRCDIR)/main.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/util.cpp $(SRCDIR)/memory.cpp $(SRCDIR)/type.cpp $(SRCDIR)/type_table.cpp $(SRCDIR)/parser.cpp $(SRCDIR)/decl_check.cpp $(SRCDIR)/type_infer.cpp $(SRCDIR)/symbol_table.cpp $(SRCDIR)/type_check.cpp $(SRCDIR)/hash_table.cpp $(SRCDIR)/error.cpp $(SRCDIR)/llvm_backend.cpp -o $(BINDIR)/light $(BINDIR)/external_caller.o 

extcall: 
	mkdir -p $(BINDIR)
	nasm -g -f elf64 $(SRCDIR)/external_caller_nasm.asm -o $(BINDIR)/external_caller.o
