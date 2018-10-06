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
	mov r11, 32
	sub r8, 32				; stack size is now 32 bytes less
	jle label_call
label_copy_stack:			; copy all the additional arguments on to the stack
	mov r10, [rdx + r9]
	mov [rsp + r11], r10
	sub r9, 8
	add r11, 8
	sub r8, 8
	jnz label_copy_stack

label_call:					; copy first 4 arguments to rcx, rdx, r8 and r9 following the C calling convention
	; get type sizes from stack in the format 0xAABBCCDD
	mov r13, [rdx - 4]
	mov r12, -12

	mov r9, r13	; copy r13
	shr r9, 24		; put value in lower bits
	and r9, 0ffh	; mask higher bits out
	sub r12, r9		; get from this offset within stack
	neg r9			; transform to -x
	add r9, 8		; do -x + 8
	shl r9, 3		; multiply by 8
	mov r14, 0ffffffffffffffffh
	shr r14, cl		; shift 2^64-1 by that amount
	mov r9, [rdx + r12]		; get the value from stack
	and r9, r14	; mask by type size

	mov r8, r13
	shr r8, 16
	and r8, 0ffh
	sub r12, r8
	neg r8
	add r8, 8
	shl r8, 3
	mov r14, 0ffffffffffffffffh
	shr r14, cl
	mov r8, [rdx + r12]
	and r8, r14

	mov r15, r13
	shr r15, 8
	and r15, 0ffh
	sub r12, r15
	neg r15
	add r15, 8
	shl r15, 3
	mov r14, 0ffffffffffffffffh
	shr r14, cl
	mov r15, [rdx + r12]
	and r15, r14
	
	mov rcx, r13
	and rcx, 0ffh
	sub r12, rcx
	neg rcx
	add rcx, 8
	shl rcx, 3
	mov r14, 0ffffffffffffffffh
	shr r14, cl
	mov rcx, [rdx + r12]
	and rcx, r14

	mov rdx, r15

	call rax

	lea rsp, [rbp]
	pop rbx
	pop rbp
	ret
call_external endp

end
