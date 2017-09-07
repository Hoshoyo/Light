section .data
section .text

; rcx  procedure address to call
; rdx  pointer to stack
; r8   size of stack
;call_external proc
global call_external
call_external:
	push rbp
	push rdi
	push rdx
	push rbx

	mov rbp, rsp

	sub rsp, 8				; IMPORTANT for debug mode this needs to be 0e8h
	sub rsp, rdx				; 32 of shadow space + r8 allocated
	
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
	mov rdi, [rsi - 8]
	mov rdx, [rsi - 24]
	mov r9, [rsi - 32]
	mov rsi, [rsi - 16]

	call rax

	lea rsp, [rbp]
	pop rbx
	pop rdx
	pop rdi
	pop rbp
	ret


