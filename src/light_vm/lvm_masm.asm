
.data
.code

cmp_flags_8 proc
    cmp cl, dl
    lahf
    ret
cmp_flags_8 endp

cmp_flags_16 proc
    cmp cx, dx
    lahf
    ret
cmp_flags_16 endp

cmp_flags_32 proc
    cmp ecx, edx
    lahf
    ret
cmp_flags_32 endp

cmp_flags_64 proc
    cmp rcx, rdx
    lahf
    ret
cmp_flags_64 endp

; preserve rbx, rsp, rbp, rdi, rsi, r12, r13, r14, and r15
; preserve xmm6 - xmm7
; u64 lvm_ext_call(void* stack, void* proc, u64* float_return)
; stack pointer in RDI
; sfunction ptr in RSI
lvm_ext_call proc

    ; prologue
    push rbp
    mov rbp, rsp

    ; preserve registers
    sub rsp, 64
    mov [rsp], r8
    mov [rsp + 8], rdi
    mov [rsp + 16], rsi
    mov [rsp + 24], r15
    mov [rsp + 32], r14
    mov [rsp + 40], r13
    mov [rsp + 48], r12
    mov [rsp + 56], rbx

    mov r10, rdx        ; r10 = funcptr

    mov rax, rcx        ; rax = struct*
    mov rbx, rcx
    add rbx, 2056       ; rbx = float_values*

    mov r13, rax
    add r13, 4104
    mov r14, rax
    add r14, 4360

    mov r11d, [rax]     ; int_arg_count
    mov r12d, [rax + 4] ; float_arg_count
    add rax, 8

arg0:
    ; Put first 4 arguments to registers
    cmp r11d, 0
    je arg0_float
    cmp byte ptr[r13], 0
    jne arg0_float

    mov rcx, [rax]
    add rax, 8
    dec r11d
    inc r13
    jmp arg1
arg0_float:
    ; assume that if we got here arg0 is float
    cmp r12d, 0
    je arg1
    movdqa xmm0, xmmword ptr[rbx]
    add rbx, 8
    dec r12d
    inc r14

arg1:
    cmp r11d, 0
    je arg1_float
    cmp byte ptr[r13], 1
    jne arg1_float

    mov rdx, [rax]
    add rax, 8
    dec r11d
    inc r13
    jmp arg2
arg1_float:
    cmp r12d, 0
    je arg2
    movdqa xmm1, xmmword ptr[rbx]
    add rbx, 8
    dec r12d
    inc r14

arg2:
    cmp r11d, 0
    je arg2_float
    cmp byte ptr[r13], 2
    jne arg2_float

    mov r8, [rax]
    add rax, 8
    dec r11d
    inc r13
    jmp arg3
arg2_float:
    cmp r12d, 0
    je arg3
    movdqa xmm2, xmmword ptr[rbx]
    add rbx, 8
    dec r12d
    inc r14

arg3:
    cmp r11d, 0
    je arg3_float
    cmp byte ptr[r13], 3
    jne arg3_float

    mov r9, [rax]
    add rax, 8
    dec r11d
    inc r13
    jmp end_reg_args
arg3_float:
    cmp r12d, 0
    je end_reg_args
    movdqa xmm2, xmmword ptr[rbx]
    add rbx, 8
    dec r12d
    inc r14

end_reg_args:
    ; rax = current int_value*
    ; rbx = current float_value*
    ; r11d = current int arg count
    ; r12d = current float arg count
    ; r13 = current int index*
    ; r14 = current float index*

    xor r15, r15
    mov r15d, r11d
    dec r15
    shl r15d, 3    ; multiply by 8
    add rax, r15   ; put rax at the end of the int arguments

    xor r15, r15
    mov r15d, r12d
    dec r15
    shl r15d, 3
    add rbx, r15   ; put rbx at the end of the float arguments

    xor r15, r15
    mov r15d, r11d ; r15 = int_count
    add r15d, r12d ; r15 = int_count + float_count
    and r15, 1   ; r15 = count % 2
    shl r15, 3   ; 1 * 8 or 0 * 8
    sub rsp, r15 ; stack must be 16 byte aligned, so if odd number of pushed arguments, adjust.

pushing_args_to_stack:
    mov r15d, r11d
    add r15d, r12d

    cmp r15d, 0
    je no_more_args

    cmp r11b, 0
    je push_float   ; there are no more integers

    cmp r12b, 0
    je push_integer ; there are no more floats

    mov r15b, [r14]
    cmp r15b, [r13] ; cmp float_index, int_index

    jg push_float ; float_index > int_index
push_integer:
    mov r15, [rax]
    push r15
    sub rax, 8
    dec r11d
    jmp pushing_args_to_stack

push_float:
    mov r15, [rbx]
    push r15
    sub rbx, 8
    dec r12d
    jmp pushing_args_to_stack

no_more_args:
    sub rsp, 32 ; 32 bytes of shadow space
    call r10
    add rsp, 32

    ; restore registers saved
    mov r8, [rsp]
    mov rsi, [rsp + 8]
    mov rdi, [rsp + 16]
    mov r15, [rsp + 24]
    mov r14, [rsp + 32]
    mov r13, [rsp + 40]
    mov r12, [rsp + 48]
    mov rbx, [rsp + 56]

    movdqa xmmword ptr[r8], xmm0

    ; epilogue
    mov	rsp, rbp
	pop	rbp

	ret
lvm_ext_call endp
end