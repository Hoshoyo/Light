.data
.text

; rcx  procedure address to call
; rdx  pointer to stack
; r8   size of stack
call_external proc
	push rbp
	push rdi
	push r8

	mov rbp, rsp
	mov rdi, rsp	

	sub rsp, 32				; IMPORTANT for debug mode this needs to be 0e8h
	sub rsp, r8				; 32 of shadow space + r8 allocated
	
	mov rax, rcx			; copy the function pointer to rax to use rcx for arguments

	sub r8, 32				; stack size is now 32 bytes less
	mov r9, 32
	mov r10, 20h
	jle label_call
label_copy_stack:			; copy all the additional arguments on to the stack
	mov rbx, [rdx + r9]
	mov [rsp + r10], rbx
	add r9, 8
	add r10, 8
	sub r8, 8
	jnz label_copy_stack

label_call:					; copy first 4 arguments to rcx, rdx, r8 and r9 following the C calling convention
	mov rcx, [rdx]
	mov r8, [rdx + 16]
	mov r9, [rdx + 24]
	mov rdx, [rdx + 8]

	call rax

	lea rsp, [rbp]
	pop r8
	pop rdi
	pop rbp
	ret
call_external endp

call_external_DEBUG proc
	push rbp
	push rdi
	push r8

	mov rbp, rsp
	mov rdi, rsp	

	sub rsp, 0e8h			; IMPORTANT for debug mode this needs to be 0e8h
	sub rsp, r8				; 32 of shadow space + r8 allocated
	
	mov rax, rcx			; copy the function pointer to rax to use rcx for arguments

	sub r8, 32				; stack size is now 32 bytes less
	mov r9, 32
	mov r10, 20h
	jle label_call
label_copy_stack:			; copy all the additional arguments on to the stack
	mov rbx, [rdx + r9]
	mov [rsp + r10], rbx
	add r9, 8
	add r10, 8
	sub r8, 8
	jnz label_copy_stack

label_call:					; copy first 4 arguments to rcx, rdx, r8 and r9 following the C calling convention
	mov rcx, [rdx]
	mov r8, [rdx + 16]
	mov r9, [rdx + 24]
	mov rdx, [rdx + 8]

	call rax

	lea rsp, [rbp]
	pop r8
	pop rdi
	pop rbp
	ret
call_external_DEBUG endp

end
