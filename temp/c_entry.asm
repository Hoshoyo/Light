section .data
section .text

extern __entry

global _start
_start:
	call __entry
	mov ebx, eax
	mov eax, 1
	int 80h
