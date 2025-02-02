
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

    ; if rbp % 16 != 0
    push rbp
    ; ((16 - (rax & 0xf)) & 0xf);
    mov rax, rsp
    add rax, 8
    mov rdi, 16
    and rax, 0fh
    sub rdi, rax
    and rdi, 0fh

    sub rsp, rdi
    push rdi
    mov rbp, rsp

    ; preserve registers
    sub rsp, 64
    mov [rbp - 64], r8
    mov [rbp - 56], rsi
    mov [rbp - 48], rdi
    mov [rbp - 40], r15
    mov [rbp - 32], r14
    mov [rbp - 24], r13
    mov [rbp - 16], r12
    mov [rbp - 8], rbx

    mov r10, rdx        ; r10 = funcptr
    mov rsi, rcx

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
    movlpd xmm0, qword ptr[rbx]
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
    movlpd xmm1, qword ptr[rbx]
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
    movlpd xmm2, qword ptr[rbx]
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
    movlpd xmm3, qword ptr[rbx]
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
    mov rdi, r15

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

    ; clear the vm temp stack
    mov dword ptr[rsi], 0
    mov dword ptr[rsi + 4], 0

    call r10

    ; restore registers saved
    mov r8,  [rbp - 64]
    mov rsi, [rbp - 56]
    mov rdi, [rbp - 48]
    mov r15, [rbp - 40]
    mov r14, [rbp - 32]
    mov r13, [rbp - 24]
    mov r12, [rbp - 16]
    mov rbx, [rbp - 8]

    movlpd qword ptr[r8], xmm0

    ; epilogue
    mov rsp, rbp
	pop rcx
    add rsp, rcx
    pop rbp

	ret
lvm_ext_call endp
end