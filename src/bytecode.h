#pragma once
#include "light_vm/lightvm.h"
#include "utils/hash.h"

typedef struct {
    Light_Token* name;
    Light_VM_Instruction_Info info;
} Bytecode_CallInfo;

GENERATE_HASH_TABLE(Bytecode_Calls, bytecode_calls, Bytecode_CallInfo)

typedef struct {
    int age;
    int allocated;
} Bytecode_Register;

typedef struct {
    Light_VM_State* vmstate;

    Bytecode_Register iregs[R_COUNT];
    Bytecode_Register fregs[FREG_COUNT];

    Bytecode_Calls_Table call_table;
} Bytecode_State;

typedef enum {
    LIGHT_REGISTER_F32,
    LIGHT_REGISTER_F64,
    LIGHT_REGISTER_INT,
} Light_Register_Type;

typedef struct {
    Light_Register_Type kind;
    s32                 size_bits;
    u8                  code;
} Light_Register;

Light_VM_Instruction_Info bytecode_gen_internal_decl(Bytecode_State* state, Light_Ast* decl);
Light_VM_Instruction_Info bytecode_gen_decl(Bytecode_State* state, Light_Ast* decl);

Light_VM_Instruction_Info bytecode_gen_comm(Bytecode_State* state, Light_Ast* comm);
Light_VM_Instruction_Info bytecode_gen_expr(Bytecode_State* state, Light_Ast* expr, Light_Register* out_reg);

Bytecode_State bytecode_gen_ast(Light_Ast** ast);