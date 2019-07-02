#pragma once
#include "light_vm/lightvm.h"

typedef struct {
    int age;
    int allocated;
} Bytecode_Register;

typedef struct {
    Light_VM_State* vmstate;

    Bytecode_Register iregs[R_COUNT];
    Bytecode_Register fregs[FREG_COUNT];
} Bytecode_State;

Light_VM_Instruction_Info bytecode_gen_internal_decl(Bytecode_State* state, Light_Ast* decl);
Light_VM_Instruction_Info bytecode_gen_decl(Bytecode_State* state, Light_Ast* decl);

Light_VM_Instruction_Info bytecode_gen_comm(Bytecode_State* state, Light_Ast* comm);
Light_VM_Instruction_Info bytecode_gen_expr(Bytecode_State* state, Light_Ast* expr, u8 reg);

Bytecode_State bytecode_gen_ast(Light_Ast** ast);