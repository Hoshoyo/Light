section .data
section .text

%ifdef OSX
extern ___entry

global __start
__start:
	call ___entry
%else
extern __entry

global _start
_start:
  call __entry
%endif
	mov ebx, eax
	mov eax, 1
	int 80h
