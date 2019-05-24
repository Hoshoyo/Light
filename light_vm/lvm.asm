global cmp_flags_8
global cmp_flags_16
global cmp_flags_32
global cmp_flags_64

global lvm_ext_call

section .text

cmp_flags_8:
    cmp dil, sil
    lahf
    ret

cmp_flags_16:
    cmp di, si
    lahf
    ret

cmp_flags_32:
    cmp edi, esi
    lahf
    ret

cmp_flags_64:
    cmp rdi, rsi
    lahf
    ret

; stack pointer in RDI
; sfunction ptr in RSI
lvm_ext_call:
    ; prologue
    push rbp
    mov rbp, rsp

    mov r10, rsi        ; r10 = funcptr

    mov rax, rdi        ; rax = struct*
    mov rbx, rdi
    add rbx, 2056       ; rbx = float_values*

    mov r13, rax
    add r13, 4104
    mov r14, rax
    add r14, 4360

    mov r11d, [rax]     ; int_arg_count
    mov r12d, [rax + 4] ; float_arg_count
    add rax, 8

    ; Put all register integers
    cmp r11d, 0
    je no_more_int_reg_args

    mov rdi, [rax]
    add rax, 8
    dec r11d
    inc r13

    cmp r11d, 0
    je no_more_int_reg_args

    mov rsi, [rax]
    add rax, 8
    dec r11d
    inc r13

    cmp r11d, 0
    je no_more_int_reg_args
    
    mov rdx, [rax]
    add rax, 8
    dec r11d
    inc r13

    cmp r11d, 0
    je no_more_int_reg_args

    mov rcx, [rax]
    add rax, 8
    dec r11d
    inc r13

    cmp r11d, 0
    je no_more_int_reg_args

    mov r8, [rax]
    add rax, 8
    dec r11d
    inc r13

    cmp r11d, 0
    je no_more_int_reg_args

    mov r9, [rax]
    add rax, 8
    dec r11d
    inc r13

no_more_int_reg_args:
    cmp r12d, 0
    je no_more_float_reg_args

    movq xmm0, [rbx]
    add rbx, 8
    dec r12d
    inc r14

    cmp r12d, 0
    je no_more_float_reg_args

    movq xmm1, [rbx]
    add rbx, 8
    dec r12d
    inc r14

    cmp r12d, 0
    je no_more_float_reg_args

    movq xmm2, [rbx]
    add rbx, 8
    dec r12d
    inc r14

    cmp r12d, 0
    je no_more_float_reg_args

    movq xmm3, [rbx]
    add rbx, 8
    dec r12d
    inc r14

    cmp r12d, 0
    je no_more_float_reg_args

    movq xmm4, [rbx]
    add rbx, 8
    dec r12d
    inc r14

    cmp r12d, 0
    je no_more_float_reg_args

    movq xmm5, [rbx]
    add rbx, 8
    dec r12d
    inc r14

    cmp r12d, 0
    je no_more_float_reg_args

    movq xmm6, [rbx]
    add rbx, 8
    dec r12d
    inc r14

    cmp r12d, 0
    je no_more_float_reg_args

    movq xmm7, [rbx]
    add rbx, 8
    dec r12d
    inc r14

no_more_float_reg_args:
    ; rax = current int_value*
    ; rbx = current float_value*
    ; r11d = current int arg count
    ; r12d = current float arg count
    ; r13 = current int index*
    ; r14 = current float index*

    add r11d, r12d ; count = int_count + float_count
    mov r12, r11   ; r12 = count
    and r12, 0x1   ; r12 = count % 2

pushing_args_to_stack:
    cmp r11d, 0
    je no_more_args

    mov r15b, [r13]
    cmp r15b, [r14]

    jg push_float ; int_index > float_index
push_integer:
    mov r15, [rax]
    push r15
    add rax, 8
    dec r11d
    jmp pushing_args_to_stack

push_float:
    mov r15, [rbx]
    push r15
    add rbx, 8
    dec r11d
    jmp pushing_args_to_stack

no_more_args:
    shl r12, 8   ; 1 << 8 or 0 << 8
    sub rsp, r12 ; stack must be 16 byte aligned, so if odd number of pushed arguments, adjust.
    call r10

    ; epilogue
    mov	rsp, rbp
	pop	rbp
	ret