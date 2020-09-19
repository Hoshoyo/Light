#pragma once

typedef enum {
    IR_LOAD,                // load t1 -> t2        where t1 is an address
    IR_STORE,               // store t1 -> t2       where t2 is an address 
    IR_ADD, IR_SUB,         // add t1, t2 -> t3
    IR_MUL, IR_DIV, IR_MOD, // mul t1, t2 -> t3
    IR_SHL, IR_SHR,         // shl t1, t2 -> t3
    IR_OR, IR_XOR, IR_AND,  // xor t1, t2 -> t3
    IR_NOT,                 // not t1 -> t2
} IR_Instruction_Type;

typedef union {
    u8  v_u8;
    u16 v_u16;
    u32 v_u32;
    u64 v_u64;
    s8  v_s8;
    s16 v_s16;
    s32 v_s32;
    s64 v_s64;
    r32 v_r32;
    r64 v_r64;
} IR_Value;

typedef struct {
    IR_Instruction_Type type;
    int                 t1, t2, t3;
    IR_Value            imm;

    int byte_size;
} IR_Instruction;

typedef struct {
    int index;
    int offset; // also byte count at the end
} IR_Activation_Rec;

typedef struct {
    IR_Instruction*   instructions;
    IR_Activation_Rec ar;
    int temp;
} IR_Generator;

void ir_generate(Light_Ast** ast);