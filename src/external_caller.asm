.data
.code

; rcx  procedure address to call
; rdx  pointer to stack
; r8   size of stack

call_external proc
	push rbp
	push rbx

	mov rbp, rsp
	mov rdi, rsp	

	;sub rsp, 0e8h			; IMPORTANT for debug mode this needs to be 0e8h
	sub rsp, 8				; IMPORTANT 8 bytes for return address
	sub rsp, r8				; 32 of shadow space + r8 allocated
	
	mov rax, rcx			; copy the function pointer to rax to use rcx for arguments

	mov r9, -40
	mov r12, 32
	sub r8, 32				; stack size is now 32 bytes less
	jle label_call
label_copy_stack:			; copy all the additional arguments on to the stack
	mov rbx, [rdx + r9]
	mov [rsp + r12], rbx
	sub r9, 8
	add r12, 8
	sub r8, 8
	jnz label_copy_stack

label_call:					; copy first 4 arguments to rcx, rdx, r8 and r9 following the C calling convention
	mov rcx, [rdx - 8]
	mov r8, [rdx - 24]
	mov r9, [rdx - 32]
	mov rdx, [rdx - 16]

	call rax

	lea rsp, [rbp]
	pop rbx
	pop rbp
	ret
call_external endp

end
