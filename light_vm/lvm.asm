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

; u64 lvm_ext_call(void* stack, void* proc, u64* float_return)
; stack pointer in RDI
; sfunction ptr in RSI
lvm_ext_call:
    ; prologue
    push rbp
    push rdx ; save float return pointer
    push rdx ; just to align to 16bytes
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
    and r15, 0x1   ; r15 = count % 2
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
    call r10

    ; epilogue
    mov	rsp, rbp
    pop rdx ; retreive float return pointer
    pop rdx
    movq [rdx], xmm0 ; save the float return in the argument pointer(float_return)

	pop	rbp
	ret