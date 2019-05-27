CC=g++
CFLAGS=-I./include -g -I./light_vm -I./src -std=c++11 -w -m64 -DDEBUG=0
LINKFLAGS=-ldl light_vm/lightvm.a
SRCDIR=./src
BINDIR=./bin/gcc
UNAME_S=$(shell uname -s)

all: extcall entry
	mkdir -p ./bin/gcc
	$(CC) $(CFLAGS) $(SRCDIR)/ast.cpp $(SRCDIR)/lightvm_gen.cpp $(SRCDIR)/interpreter.cpp $(SRCDIR)/bytecode_gen.cpp $(SRCDIR)/main.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/util.cpp $(SRCDIR)/type.cpp $(SRCDIR)/type_table.cpp $(SRCDIR)/parser.cpp $(SRCDIR)/decl_check.cpp $(SRCDIR)/type_infer.cpp $(SRCDIR)/type_check.cpp $(SRCDIR)/symbol_table.cpp $(SRCDIR)/hash_table.cpp $(SRCDIR)/error.cpp $(SRCDIR)/c_backend.cpp $(SRCDIR)/file_table.cpp -o $(BINDIR)/light $(BINDIR)/external_caller.o $(LINKFLAGS)

extcall:
	mkdir -p $(BINDIR)
ifeq ($(UNAME_S),Darwin)
	nasm -dOSX -g -f macho64 $(SRCDIR)/external_caller_nasm.asm -o $(BINDIR)/external_caller.o
endif
ifeq ($(UNAME_S),Linux)
	nasm -g -f elf64 $(SRCDIR)/external_caller_nasm.asm -o $(BINDIR)/external_caller.o
endif

entry:
ifeq ($(UNAME_S),Darwin)
	nasm -dOSX -g -f macho64 temp/c_entry.asm -o temp/c_entry.o
endif
ifeq ($(UNAME_S),Linux)
	nasm -g -f elf64 temp/c_entry.asm -o temp/c_entry.o
endif