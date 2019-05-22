#include <stdlib.h>
#include "lightvm.h"

#define true 1
#define false 0

Light_VM_State*
light_vm_init() {
    Light_VM_State* state = (Light_VM_State*)calloc(1, sizeof(*state));

    state->data.block = calloc(1, 1024 * 1024); // 1MB
    state->data.size = 1024 * 1024;

    state->code.block = calloc(1, 1024 * 1024); // 1MB
    state->code.size = 1024 * 1024;

    state->stack.block = calloc(1, 1024 * 1024); // 1MB
    state->stack.size = 1024 * 1024;

    return state;
}

static void
push_immediate(Light_VM_State* state, u8 size_bytes, u64 imm) {
    switch(size_bytes) {
        case 1: *(u8*)(state->code.block + state->code_offset) = (u8)imm; break;
        case 2: *(u16*)(state->code.block + state->code_offset) = (u16)imm; break;
        case 4: *(u32*)(state->code.block + state->code_offset) = (u32)imm; break;
        case 8: *(u64*)(state->code.block + state->code_offset) = imm; break;
        default: assert(0); break;
    }
    state->code_offset += size_bytes;
}

Light_VM_Intruction_Info 
light_vm_push_instruction(Light_VM_State* vm_state, Light_VM_Instruction instr, u64 immediate) {
    Light_VM_Intruction_Info info = {0};
    info.byte_size = (u32)sizeof(Light_VM_Instruction);
    info.offset_address = vm_state->code_offset;
    info.absolute_address = vm_state->code.block + vm_state->code_offset;

    *(Light_VM_Instruction*)(vm_state->code.block + vm_state->code_offset) = instr;
    vm_state->code_offset += sizeof(Light_VM_Instruction);

    switch(instr.type) {
        case LVM_NOT: case LVM_PUSH: case LVM_POP:
        case LVM_NOP: case LVM_RET: case LVM_HLT: break;

        // Binary instructions
        case LVM_CMP:
        case LVM_SHL: case LVM_SHR:
        case LVM_OR: case LVM_AND:
        case LVM_XOR: case LVM_MOV:
        case LVM_ADD_S: case LVM_SUB_S:
        case LVM_MUL_S: case LVM_DIV_S:
        case LVM_MOD_S: case LVM_ADD_U:
        case LVM_SUB_U: case LVM_MUL_U:
        case LVM_DIV_U: case LVM_MOD_U:{
            u32 am = instr.binary.addr_mode;
            if(
                am == BIN_ADDR_MODE_IMM_TO_REG ||
                am == BIN_ADDR_MODE_MEM_IMM_TO_REG ||
                am == BIN_ADDR_MODE_REG_TO_IMM_MEM ||
                am == BIN_ADDR_MODE_REG_OFFSETED_TO_REG ||
                am == BIN_ADDR_MODE_REG_TO_MEM_OFFSETED)
            {
                info.immediate_byte_size = instr.imm_size_bytes;
                push_immediate(vm_state, info.immediate_byte_size, immediate);
            }   
        } break;

        // Floating point
        case LVM_FADD:
        case LVM_FSUB:
        case LVM_FMUL:
        case LVM_FDIV:
        case LVM_FCMP:
        case LVM_FMOV:{
            u32 am = instr.ifloat.addr_mode;
            if(
                am == FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG ||
                am == FLOAT_ADDR_MODE_REG_TO_MEM_OFFSETED)
            {
                info.immediate_byte_size = instr.imm_size_bytes;
                push_immediate(vm_state, info.immediate_byte_size, immediate);
            }
        } break;

        // Comparison/Branch
        case LVM_BEQ: case LVM_BNE: case LVM_BLT_S:
        case LVM_BGT_S: case LVM_BLE_S: case LVM_BGE_S:
        case LVM_BLT_U: case LVM_BGT_U: case LVM_BLE_U:
        case LVM_BGE_U: case LVM_JMP: {
            info.immediate_byte_size = instr.imm_size_bytes;
            push_immediate(vm_state, info.immediate_byte_size, immediate);
        } break;

        case LVM_EXTCALL: case LVM_CALL:
            if( instr.call.addr_mode == CALL_ADDR_MODE_IMMEDIATE_ABSOLUTE ||
                instr.call.addr_mode == CALL_ADDR_MODE_IMMEDIATE_RELATIVE) 
            {
                info.immediate_byte_size = instr.imm_size_bytes;
                push_immediate(vm_state, info.immediate_byte_size, immediate);
            }
            break;

        // TODO(psv):
        case LVM_COPY: case LVM_ASSERT:
        default: {
        }break;
    }

    return info;
}