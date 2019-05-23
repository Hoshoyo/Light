global cmp_flags_8
global cmp_flags_16
global cmp_flags_32
global cmp_flags_64

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