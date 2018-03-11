all:
	llc -filetype=obj -march=x86-64 llvmtest.ll -o llvmtest.o
	ld llvmtest.o -lc -o main.exe
