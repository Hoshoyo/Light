section .data
    printf_format: db '%lx', 10, 0

section .text

; rcx  procedure address to call
; rdx  pointer to stack
; r8   size of stack
;call_external proc
extern printf
extern print_register
global call_external
call_external:
	push rbp
	push rdi
	push rdx
	push rbx

	mov rbp, rsp

	sub rsp, 8				; IMPORTANT for debug mode this needs to be 0e8h
	sub rsp, rdx			; 32 of shadow space + r8 allocated
	
	mov rax, rdi			; copy the function pointer to rax to use rcx for arguments

	mov r9, -40
	mov r12, 32
	sub rdx, 32				; stack size is now 32 bytes less
	jle label_call
label_copy_stack:			; copy all the additional arguments on to the stack
	mov rbx, [rsi + r9]
	mov [rsp + r12], rbx
	sub r9, 8
	add r12, 8
	sub rdx, 8
	jnz label_copy_stack

label_call:					; copy first 4 arguments to rcx, rdx, r8 and r9 following the C calling convention
	;mov rdi, [rsi - 8]		; rcx
	;mov rdx, [rsi - 24]	; r8
	;mov r9, [rsi - 32]		; r9
	;mov rsi, [rsi - 16]	; rdx
	
	; -----

	; get type sizes from stack in the format 0xAABBCCDD
	mov r13, [rsi - 4]
	mov r12, -12

	mov r9, r13		; copy r13
	shr r9, 24		; put value in lower bits
	and r9, 0ffh	; mask higher bits out
	sub r12, r9		; get from this offset within stack
	neg r9			; transform to -x
	add r9, 8		; do -x + 8
	shl r9, 3		; multiply by 8
	mov r14, 0ffffffffffffffffh
	mov rcx, r9
	shr r14, cl		; shift 2^64-1 by that amount
	mov r9, [rsi + r12]		; get the value from stack
	and r9, r14		; mask by type size

	mov rdx, r13
	shr rdx, 16
	and rdx, 0ffh
	sub r12, rdx
	neg rdx
	add rdx, 8
	shl rdx, 3
	mov r14, 0ffffffffffffffffh
	mov rcx, rdx
	shr r14, cl
	mov rdx, [rsi + r12]
	and rdx, r14

	mov r15, r13
	shr r15, 8
	and r15, 0ffh
	sub r12, r15
	neg r15
	add r15, 8
	shl r15, 3
	mov r14, 0ffffffffffffffffh
	mov rcx, r15
	shr r14, cl
	mov r15, [rsi + r12]
	and r15, r14

	mov rdi, r13
	and rdi, 0ffh
	sub r12, rdi
	neg rdi
	add rdi, 8
	shl rdi, 3
	mov r14, 0ffffffffffffffffh
	mov rcx, rdi
	shr r14, cl
	mov rdi, [rsi + r12]
	and rdi, r14

	mov rsi, r15

	; ------

	call rax

	lea rsp, [rbp]
	pop rbx
	pop rdx
	pop rdi
	pop rbp
	ret