CC=g++
CFLAGS=-I./include -I./src -std=c++11 -m64 -w
SRCDIR=./src
BINDIR=./bin/gcc

all:
	mkdir -p ./bin/gcc
	$(CC) $(CFLAGS) $(SRCDIR)/ast.cpp $(SRCDIR)/main.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/util.cpp $(SRCDIR)/memory.cpp $(SRCDIR)/type.cpp $(SRCDIR)/parser.cpp -o $(BINDIR)/prism