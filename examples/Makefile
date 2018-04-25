all:
	llc -filetype=obj -march=x86-64 test.ll -o test.obj
	ld test.obj print_string.o ../temp/entry.o -o test -s -dynamic-linker /lib64/ld-linux-x86-64.so.2 -lc
clean:
	rm *.ll
