section .data
section .text

extern main

global _start
_start:
	call main
	mov ebx, eax
	mov eax, 1
	int 80h
