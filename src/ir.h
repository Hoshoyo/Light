#pragma once
#include <stdint.h>
#include "ast.h"

typedef enum {
    IR_REG_INSTR_PTR = -3,
    IR_REG_STACK_PTR = -2,
    IR_REG_NONE = -1
} IR_Reg;

typedef enum {
    IR_NONE = 0,
    IR_LEA,                 // lea t1 + imm -> t2
    IR_MOV,                 // mov t1 -> t2
    IR_LOAD,                // load t1 -> t2        where t1 is an address
    IR_STORE,               // store t1 -> t2       where t2 is an address 
    IR_ADD, IR_SUB,         // add t1, t2 -> t3
    IR_MUL, IR_DIV, IR_MOD, // mul t1, t2 -> t3
    IR_SHL, IR_SHR,         // shl t1, t2 -> t3
    IR_OR, IR_XOR, IR_AND,  // xor t1, t2 -> t3
    IR_LAND, IR_LOR,        // land t1, t2 -> t3
    IR_NOT, IR_NEG,         // not t1 -> t2
    IR_LNOT,                // lnot t1-> t2

    IR_MOVF,                // movf tf1 -> tf2
    IR_LOADF,               // loadf t -> tf
    IR_STOREF,              // storef tf -> t
    IR_ADDF, IR_SUBF,       // addf tf1, tf2 -> tf3
    IR_MULF, IR_DIVF,       // mulf tf1, tf2 -> tf3
    IR_NEGF,                // negf tf1 -> tf2

    // convert instructions
    IR_CVT_S32_R32,         // cvtsi2ss t -> tf
    IR_CVT_R32_S32,         // cvtss2si tf -> t
    IR_CVT_R32_R64,         // cvtss2ds tf1 -> tf2
    IR_CVT_R64_R32,         // cvtds2ss tf1 -> tf2

} IR_Instruction_Type;

typedef enum {
    IR_VALUE_NONE = 0,
    IR_VALUE_U8,
    IR_VALUE_U16,
    IR_VALUE_U32,
    IR_VALUE_U64,
    IR_VALUE_S8,
    IR_VALUE_S16,
    IR_VALUE_S32,
    IR_VALUE_S64,
    IR_VALUE_R32,
    IR_VALUE_R64,
    IR_VALUE_COUNT,
} IR_Value_Type;

typedef struct {
    IR_Value_Type type;
    union {
        uint8_t  v_u8;
        uint16_t v_u16;
        uint32_t v_u32;
        uint64_t v_u64;
        int8_t   v_s8;
        int16_t  v_s16;
        int32_t  v_s32;
        int64_t  v_s64;
        float    v_r32;
        double   v_r64;
    };
} IR_Value;

typedef struct {
    IR_Instruction_Type type;
    IR_Reg              t1, t2, t3;
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
    int temp_int;
    int temp_float;
} IR_Generator;

void ir_generate(Light_Ast** ast);

void iri_emit_store(IR_Generator* gen, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size, bool fp);
void iri_emit_load(IR_Generator* gen, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size, bool fp);
void iri_emit_arith(IR_Generator* gen, IR_Instruction_Type type, IR_Reg t1, IR_Reg t2, IR_Reg t3, IR_Value imm, int byte_size);
void iri_emit_not(IR_Generator* gen, IR_Reg t1, IR_Reg t2, int byte_size);
void iri_emit_logic_not(IR_Generator* gen, IR_Reg t1, IR_Reg t2, int byte_size);
void iri_emit_neg(IR_Generator* gen, IR_Reg t1, IR_Reg t2, int byte_size, bool fp);
void iri_emit_mov(IR_Generator* gen, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size, bool fp);
void iri_emit_lea(IR_Generator* gen, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size);

int  iri_value_byte_size(IR_Value value);
void iri_print_instructions(IR_Generator* gen);