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

static u64 
get_value_off_immediate(Light_VM_State* state, Light_VM_Instruction instr, void* address_of_imm) {
    u64 value = 0;
    switch(instr.imm_size_bytes) {
        case 1: value = (u64)*(u8*)address_of_imm; break;
        case 2: value = (u64)*(u16*)address_of_imm; break;
        case 4: value = (u64)*(u32*)address_of_imm; break;
        case 8: value = *(u64*)address_of_imm; break;
        default: break;
    }
    return value;
}

#define EXECUTE_ARITH_UNSIGNED(OP) switch(instr.binary.bytesize) { \
        case 1: *(u8*)dst = *(u8*)dst OP *(u8*)src; break; \
        case 2: *(u16*)dst = *(u16*)dst OP *(u16*)src; break; \
        case 4: *(u32*)dst = *(u32*)dst OP *(u32*)src; break; \
        case 8: *(u64*)dst = *(u64*)dst OP *(u64*)src; break; \
        default: assert(0); break; \
    } \

#define EXECUTE_ARITH_SIGNED(OP) switch(instr.binary.bytesize) { \
        case 1: *(s8*)dst = *(s8*)dst OP *(s8*)src; break; \
        case 2: *(s16*)dst = *(s16*)dst OP *(s16*)src; break; \
        case 4: *(s32*)dst = *(s32*)dst OP *(s32*)src; break; \
        case 8: *(s64*)dst = *(s64*)dst OP *(s64*)src; break; \
        default: assert(0); break; \
    } \

void
light_vm_execute_binary_arithmetic_instruction(Light_VM_State* state, Light_VM_Instruction instr) {
    void* address_of_imm = ((void*)state->registers[RIP]) + sizeof(Light_VM_Instruction); // address of immediate

    void* dst = 0;
    void* src = 0;
    switch(instr.binary.addr_mode) {
        // mov r0, r1
        case BIN_ADDR_MODE_REG_TO_REG:{
            dst = &state->registers[instr.binary.dst_reg];
            src = &state->registers[instr.binary.src_reg];
        }break;
        // mov [r0], r1
        case BIN_ADDR_MODE_REG_TO_MEM:{
            src = &state->registers[instr.binary.src_reg];
            dst = (void*)state->registers[instr.binary.dst_reg];
        }break;
        // mov [0x1234], r1
        case BIN_ADDR_MODE_REG_TO_IMM_MEM:{
            src = &state->registers[instr.binary.src_reg];
            dst = *(void**)address_of_imm;
        }break;
        // mov [r0 + 0x42], r1
        case BIN_ADDR_MODE_REG_TO_MEM_OFFSETED:{
            src = &state->registers[instr.binary.src_reg];
            if(instr.binary.sign) {
                dst = (void*)(state->registers[instr.binary.dst_reg] - get_value_off_immediate(state, instr, address_of_imm));
            } else {
                dst = (void*)(state->registers[instr.binary.dst_reg] + get_value_off_immediate(state, instr, address_of_imm));
            }
        }break;
        // mov r0, [r1 + 0x213]
        case BIN_ADDR_MODE_REG_OFFSETED_TO_REG:{
            if(instr.binary.sign) {
                src = (void*)(state->registers[instr.binary.src_reg] - get_value_off_immediate(state, instr, address_of_imm));
            } else {
                src = (void*)(state->registers[instr.binary.src_reg] + get_value_off_immediate(state, instr, address_of_imm));
            }
            dst = &state->registers[instr.binary.dst_reg];
        }break;
        // mov r0, [r1]
        case BIN_ADDR_MODE_MEM_TO_REG:{
            src = (void*)state->registers[instr.binary.src_reg];
            dst = &state->registers[instr.binary.dst_reg];
        }break;
        // mov r0, [0x1234]
        case BIN_ADDR_MODE_MEM_IMM_TO_REG:{
            src = *(void**)address_of_imm;
            dst = &state->registers[instr.binary.dst_reg];
        }break;
        // mov r0, 0x123
        case BIN_ADDR_MODE_IMM_TO_REG:{
            u64 imm_value = get_value_off_immediate(state, instr, address_of_imm);
            src = &imm_value;
            dst = &state->registers[instr.binary.dst_reg];
        }break;
    }

    switch(instr.type) {
        case LVM_CMP: {

        }break;

        case LVM_MOV: {
            switch(instr.binary.bytesize) {
                case 1: *(u8*)dst  = *(u8*)src; break;
                case 2: *(u16*)dst = *(u16*)src; break;
                case 4: *(u32*)dst = *(u32*)src; break;
                case 8: *(u64*)dst = *(u64*)src; break;
                default: assert(0); break;
            }
        }break;
        case LVM_ADD_S: {
            EXECUTE_ARITH_SIGNED(+)
        }break;
        case LVM_SUB_S: {
            EXECUTE_ARITH_SIGNED(-)
        }break;
        case LVM_MUL_S:{
            EXECUTE_ARITH_SIGNED(*)
        } break;
        case LVM_DIV_S:{
            EXECUTE_ARITH_SIGNED(/)
        } break;
        case LVM_MOD_S:{
            EXECUTE_ARITH_SIGNED(%)
        } break; 
        case LVM_ADD_U:{
            EXECUTE_ARITH_UNSIGNED(+)
        } break;
        case LVM_SUB_U:{
            EXECUTE_ARITH_UNSIGNED(-)
        } break; 
        case LVM_MUL_U:{
            EXECUTE_ARITH_UNSIGNED(*)
        } break;
        case LVM_DIV_U:{
            EXECUTE_ARITH_UNSIGNED(/)
        } break; 
        case LVM_MOD_U:{
            EXECUTE_ARITH_UNSIGNED(%)
        } break;
        case LVM_SHL: {
            EXECUTE_ARITH_UNSIGNED(<<)
        } break; 
        case LVM_SHR: {
            EXECUTE_ARITH_UNSIGNED(>>)
        } break;
        case LVM_OR: {
            EXECUTE_ARITH_UNSIGNED(|)
        } break; 
        case LVM_AND: {
            EXECUTE_ARITH_UNSIGNED(&)
        } break;
        case LVM_XOR: {
            EXECUTE_ARITH_UNSIGNED(^)
        } break;
        default: break;
    }
}

static bool
float_32_register(u8 r) {
    return (r >= FR0 && r <= FR3);
}

#define EXECUTE_FLOAT_OP(OP) if(float_32_register(instr.ifloat.dst_reg)) \
        *(r32*)dst = *(r32*)dst OP *(r32*)src; \
    else \
        *(r64*)dst = *(r64*)dst OP *(r64*)src;

void
light_vm_execute_float_instruction(Light_VM_State* state, Light_VM_Instruction instr) {
    void* address_of_imm = ((void*)state->registers[RIP]) + sizeof(Light_VM_Instruction); // address of immediate

    void* dst = 0;
    void* src = 0;
    switch(instr.ifloat.addr_mode) {
        case FLOAT_ADDR_MODE_REG_TO_REG: { // fadd fr0, fr1
            if(float_32_register(instr.ifloat.dst_reg)) {
                dst = &state->f32registers[instr.ifloat.dst_reg];
                src = &state->f32registers[instr.ifloat.src_reg];
            } else {
                dst = &state->f64registers[instr.ifloat.dst_reg];
                src = &state->f64registers[instr.ifloat.src_reg];
            }
        } break;
        case FLOAT_ADDR_MODE_REG_TO_MEM: { // fadd [r0], fr2
            dst = (void*)state->registers[instr.ifloat.dst_reg];
            if(float_32_register(instr.ifloat.src_reg)){
                src = &state->f32registers[instr.ifloat.src_reg];
            } else {
                src = &state->f64registers[instr.ifloat.src_reg];
            }
        } break;
        case FLOAT_ADDR_MODE_MEM_TO_REG: { // fadd fr0, [r1]
            if(float_32_register(instr.ifloat.dst_reg)){
                dst = &state->f32registers[instr.ifloat.dst_reg];
            } else {
                dst = &state->f64registers[instr.ifloat.dst_reg];
            }
            src = (void*)state->registers[instr.ifloat.src_reg];
        } break;
        case FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG: { // fadd fr0, [r1 + 0x23]
            if(float_32_register(instr.ifloat.dst_reg)){
                dst = &state->f32registers[instr.ifloat.dst_reg];
            } else {
                dst = &state->f64registers[instr.ifloat.dst_reg];
            }
            if(instr.ifloat.sign) {
                src = (void*)(state->registers[instr.ifloat.src_reg] - get_value_off_immediate(state, instr, address_of_imm));
            } else {
                src = (void*)(state->registers[instr.ifloat.src_reg] + get_value_off_immediate(state, instr, address_of_imm));
            }
        } break;
        case FLOAT_ADDR_MODE_REG_TO_MEM_OFFSETED: { // fadd [r0 + 0x12], fr3
            if(float_32_register(instr.ifloat.src_reg)){
                src = &state->f32registers[instr.ifloat.src_reg];
            } else {
                src = &state->f64registers[instr.ifloat.src_reg];
            }
            if(instr.ifloat.sign) {
                dst = (void*)(state->registers[instr.ifloat.dst_reg] - get_value_off_immediate(state, instr, address_of_imm));
            } else {
                dst = (void*)(state->registers[instr.ifloat.dst_reg] + get_value_off_immediate(state, instr, address_of_imm));
            }
        } break;
    }

    switch(instr.type) {
        case LVM_FCMP:{

        }break;
        case LVM_FMOV:{
            if(float_32_register(instr.ifloat.dst_reg))
                *(r32*)dst = *(r32*)src;
            else
                *(r64*)dst = *(r64*)src;
        }break;
        case LVM_FADD:{
            EXECUTE_FLOAT_OP(+);
        } break;
        case LVM_FSUB:{
            EXECUTE_FLOAT_OP(-);
        }break;
        case LVM_FMUL:{
            EXECUTE_FLOAT_OP(*);
        }break;
        case LVM_FDIV:{
            EXECUTE_FLOAT_OP(/);
        }break;
        default: assert(0); break;
    }
}

void
light_vm_execute_instruction(Light_VM_State* state, Light_VM_Instruction instr) {
    bool advance_ip = false;
    switch(instr.type) {
         case LVM_NOP: advance_ip = true; break;

        // Binary instructions
        case LVM_CMP:
        case LVM_SHL: 
        case LVM_SHR:
        case LVM_OR: 
        case LVM_AND:
        case LVM_XOR:
        case LVM_MOV:
        case LVM_ADD_S: 
        case LVM_SUB_S:
        case LVM_MUL_S: 
        case LVM_DIV_S:
        case LVM_MOD_S: 
        case LVM_ADD_U:
        case LVM_SUB_U: 
        case LVM_MUL_U:
        case LVM_DIV_U: 
        case LVM_MOD_U:
            light_vm_execute_binary_arithmetic_instruction(state, instr);
            advance_ip = true;
            break;

        // Floating point
        case LVM_FADD:
        case LVM_FSUB:
        case LVM_FMUL:
        case LVM_FDIV:
        case LVM_FCMP:
        case LVM_FMOV:
            light_vm_execute_float_instruction(state, instr);
            advance_ip = true;
            break;

        // Unary instructions
        case LVM_NOT:{
            state->registers[instr.unary.reg] = ~state->registers[instr.unary.reg];
            advance_ip = true;
        }break;
        case LVM_PUSH:{
            void* dst = (void*)state->registers[RSP];
            switch(instr.unary.byte_size) {
                case 1: *(u8*)dst = *(u8*)&state->registers[instr.unary.reg]; break;
                case 2: *(u16*)dst = *(u16*)&state->registers[instr.unary.reg]; break;
                case 4: *(u32*)dst = *(u32*)&state->registers[instr.unary.reg]; break;
                case 8: *(u64*)dst = *(u64*)&state->registers[instr.unary.reg]; break;
                default: assert(0); break;
            }
            state->registers[RSP] += instr.unary.byte_size;
            advance_ip = true;
        }break;
        case LVM_POP:{
            switch(instr.unary.byte_size) {
                case 1: state->registers[instr.unary.reg] = (u64)*(u8*)state->registers[RSP]; break;
                case 2: state->registers[instr.unary.reg] = (u64)*(u16*)state->registers[RSP]; break;
                case 4: state->registers[instr.unary.reg] = (u64)*(u32*)state->registers[RSP]; break;
                case 8: state->registers[instr.unary.reg] = *(u64*)state->registers[RSP]; break;
                default: assert(0); break;
            }
            state->registers[RSP] -= instr.unary.byte_size;
            advance_ip = true;
        } break;

        // Comparison/Branch
        case LVM_BEQ:
        case LVM_BNE:
        case LVM_BLT_S:
        case LVM_BGT_S:
        case LVM_BLE_S:
        case LVM_BGE_S:
        case LVM_BLT_U:
        case LVM_BGT_U:
        case LVM_BLE_U:
        case LVM_BGE_U:
        case LVM_JMP:
            break;

        case LVM_EXTCALL:
        case LVM_CALL:
            break;
        case LVM_RET:  break;

        // TODO(psv):
        case LVM_COPY:    break;
        case LVM_ASSERT:  break;

        // Halt
        case LVM_HLT:  break;

        default: {
        }break;
    }
    if(advance_ip) {
        state->registers[RIP] += sizeof(Light_VM_Instruction) + instr.imm_size_bytes;
    }
}

void
light_vm_execute(Light_VM_State* state) {
    state->registers[RIP] = (u64)(Light_VM_Instruction*)(state->code.block);
    state->registers[RBP] = (u64)(Light_VM_Instruction*)(state->stack.block);
    state->registers[RSP] = state->registers[RBP];
    state->registers[RDP] = (u64)(Light_VM_Instruction*)(state->data.block);

    for(u64 i = 0;; ++i) {
        Light_VM_Instruction in = *(Light_VM_Instruction*)(state->registers[RIP]);

        if(in.type == LVM_HLT) break;
        light_vm_execute_instruction(state, in);
    }
}