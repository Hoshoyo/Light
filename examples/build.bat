@echo off

gcc -c print_string.cpp -o print_string.obj
llc -filetype=obj -march=x86-64 test.ll -o test.obj
ld test.obj print_string.obj -emain -nostdlib -o test.exe ../lib/kernel32.lib ../lib/msvcrt.lib

