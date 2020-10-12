#pragma once
#include "../../ir.h"

typedef struct {
    u8* issuer_addr;    // address of the instruction to calculate the relative offset
    u8* addr;           // address to be patched
    int instr_byte_size;
    int bytes;          // number of bytes to be replaced
    int issuer_index;       // index
    int rel_index_offset;   // relative offset from the instruction that issued to the target
    int issuer_offset;
    int issuer_imm_offset;

    int extra_offset;
    bool sum;
} X86_Patch;

int X86_generate(IR_Generator* gen);