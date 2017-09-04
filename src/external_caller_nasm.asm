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

	mov rbp, rsp

	sub rsp, 32				; IMPORTANT for debug mode this needs to be 0e8h
	sub rsp, rdx				; 32 of shadow space + r8 allocated
	
	mov rax, rdi			; copy the function pointer to rax to use rcx for arguments

	sub rdx, 32				; stack size is now 32 bytes less
	mov r9, 32
	mov r10, 20h
	jle label_call
label_copy_stack:			; copy all the additional arguments on to the stack
	mov rbx, [rsi + r9]
	mov [rsp + r10], rbx
	add r9, 8
	add r10, 8
	sub rdx, 8
	jnz label_copy_stack

label_call:					; copy first 4 arguments to rcx, rdx, r8 and r9 following the C calling convention
	mov rdi, [rsi]
	mov rdx, [rsi + 16]
	mov r9, [rsi + 24]
	mov rsi, [rsi + 8]

	call rax

	lea rsp, [rbp]
	pop rdx
	pop rdi
	pop rbp
	ret

global call_external_DEBUG
call_external_DEBUG:
	push rbp
	push rdi
	push rdx

	mov rbp, rsp

	sub rsp, 0e8h				; IMPORTANT for debug mode this needs to be 0e8h
	sub rsp, rdx				; 32 of shadow space + r8 allocated
	
	mov rax, rdi			; copy the function pointer to rax to use rcx for arguments

	sub rdx, 32				; stack size is now 32 bytes less
	mov r9, 32
	mov r10, 20h
	jle label_call_DEBUG
label_copy_stack_DEBUG:			; copy all the additional arguments on to the stack
	mov rbx, [rsi + r9]
	mov [rsp + r10], rbx
	add r9, 8
	add r10, 8
	sub rdx, 8
	jnz label_copy_stack_DEBUG

label_call_DEBUG:					; copy first 4 arguments to rcx, rdx, r8 and r9 following the C calling convention
	mov rdi, [rsi]
	mov rdx, [rsi + 16]
	mov r9, [rsi + 24]
	mov rsi, [rsi + 8]

	call rax

	lea rsp, [rbp]
	pop rdx
	pop rdi
	pop rbp
	ret

