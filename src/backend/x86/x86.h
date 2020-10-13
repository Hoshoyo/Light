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
    bool generate_relocation;
} X86_Patch;

typedef struct {
    int   length_bytes;
    char* large_data;           // when data is more than 8 bytes
    char  reg_size_data[8];     // when data is <= 8 bytes

    // patch address offset from beginning
    int patch_offset;
} X86_Data;

int X86_generate(IR_Generator* gen);