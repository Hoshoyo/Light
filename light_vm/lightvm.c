#include <stdlib.h>
#include <string.h>
#include "lightvm.h"

#define true 1
#define false 0

extern u16 cmp_flags_8(u8 l, u8 r);
extern u16 cmp_flags_16(u16 l, u16 r);
extern u16 cmp_flags_32(u32 l, u32 r);
extern u16 cmp_flags_64(u64 l, u64 r);
extern u64 lvm_ext_call(void* stack, void* proc, u64* flt_ret);

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

void
light_vm_free(Light_VM_State* state) {
    free(state->data.block);
    free(state->code.block);
    free(state->stack.block);
    free(state);
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

Light_VM_Instruction_Info 
light_vm_push_instruction(Light_VM_State* vm_state, Light_VM_Instruction instr, u64 immediate) {
    Light_VM_Instruction_Info info = {0};
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
            if( instr.branch.addr_mode == BRANCH_ADDR_MODE_IMMEDIATE_ABSOLUTE ||
                instr.branch.addr_mode == BRANCH_ADDR_MODE_IMMEDIATE_RELATIVE) 
            {
                info.immediate_byte_size = instr.imm_size_bytes;
                push_immediate(vm_state, info.immediate_byte_size, immediate);
            }
            break;

        case LVM_ALLOC: case LVM_COPY: break;

        // TODO(psv):
        case LVM_ASSERT:

        default: break;
    }

    return info;
}

void* 
light_vm_push_r32_to_datasegment(Light_VM_State* state, r32 f) {
    Light_VM_Data data = {0};
    data.byte_size = 4;
    data.float32 = f;
    return light_vm_push_data_segment(state, data);
}
void* 
light_vm_push_r64_to_datasegment(Light_VM_State* state, r64 f) {
    Light_VM_Data data = {0};
    data.byte_size = 8;
    data.float64 = f;
    return light_vm_push_data_segment(state, data);
}

s32
light_vm_patch_immediate_distance(Light_VM_Instruction_Info from, Light_VM_Instruction_Info to) {
    switch(((Light_VM_Instruction*)from.absolute_address)->imm_size_bytes) {
        case 1: *(u8*)(from.absolute_address + sizeof(Light_VM_Instruction)) = to.absolute_address - from.absolute_address; break;
        case 2: *(u16*)(from.absolute_address + sizeof(Light_VM_Instruction)) = to.absolute_address - from.absolute_address; break;
        case 4: *(u32*)(from.absolute_address + sizeof(Light_VM_Instruction)) = to.absolute_address - from.absolute_address; break;
        case 8: *(u64*)(from.absolute_address + sizeof(Light_VM_Instruction)) = to.absolute_address - from.absolute_address; break;
        default: assert(0); break;
    }
    return (to.absolute_address - from.absolute_address);
}

static u64
get_value_of_register(Light_VM_State* state, u8 reg, u8 byte_size) {
    switch(byte_size) {
        case 1: return (u64)*(u8*)&state->registers[reg]; break;
        case 2: return (u64)*(u16*)&state->registers[reg]; break;
        case 4: return (u64)*(u32*)&state->registers[reg]; break;
        case 8: return (u64)state->registers[reg]; break;
        default: assert(0); break;
    }
    return 0;
}

static u64 
get_value_of_immediate(Light_VM_State* state, Light_VM_Instruction instr, void* address_of_imm) {
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

static s64 
get_signed_value_of_immediate(Light_VM_State* state, Light_VM_Instruction instr, void* address_of_imm) {
    s64 value = 0;
    switch(instr.imm_size_bytes) {
        case 1: value = (s64)*(s8*)address_of_imm; break;
        case 2: value = (s64)*(s16*)address_of_imm; break;
        case 4: value = (s64)*(s32*)address_of_imm; break;
        case 8: value = *(s64*)address_of_imm; break;
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
                dst = (void*)(state->registers[instr.binary.dst_reg] - get_value_of_immediate(state, instr, address_of_imm));
            } else {
                dst = (void*)(state->registers[instr.binary.dst_reg] + get_value_of_immediate(state, instr, address_of_imm));
            }
        }break;
        // mov r0, [r1 + 0x213]
        case BIN_ADDR_MODE_REG_OFFSETED_TO_REG:{
            if(instr.binary.sign) {
                src = (void*)(state->registers[instr.binary.src_reg] - get_value_of_immediate(state, instr, address_of_imm));
            } else {
                src = (void*)(state->registers[instr.binary.src_reg] + get_value_of_immediate(state, instr, address_of_imm));
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
            u64 volatile imm_value = get_value_of_immediate(state, instr, address_of_imm);
            src = &imm_value;
            dst = &state->registers[instr.binary.dst_reg];
        }break;
    }

    switch(instr.type) {
        u16 flags = 0;
        case LVM_CMP: {
            switch(instr.binary.bytesize) {
                case 1: flags = cmp_flags_8(*(u8*)dst, *(u8*)src); break;
                case 2: flags = cmp_flags_16(*(u16*)dst, *(u16*)src); break;
                case 4: flags = cmp_flags_32(*(u32*)dst, *(u32*)src); break;
                case 8: flags = cmp_flags_64(*(u64*)dst, *(u64*)src); break;
                default: assert(0); break;
            }
            state->rflags.carry = (flags >> 8) & 0x1;
            state->rflags.zerof = (flags >> 14) & 0x1;
            state->rflags.sign = (flags >> 15) & 0x1;
            state->rflags.overflow = (flags >> 19) & 0x1;
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
                src = (void*)(state->registers[instr.ifloat.src_reg] - get_value_of_immediate(state, instr, address_of_imm));
            } else {
                src = (void*)(state->registers[instr.ifloat.src_reg] + get_value_of_immediate(state, instr, address_of_imm));
            }
        } break;
        case FLOAT_ADDR_MODE_REG_TO_MEM_OFFSETED: { // fadd [r0 + 0x12], fr3
            if(float_32_register(instr.ifloat.src_reg)){
                src = &state->f32registers[instr.ifloat.src_reg];
            } else {
                src = &state->f64registers[instr.ifloat.src_reg];
            }
            if(instr.ifloat.sign) {
                dst = (void*)(state->registers[instr.ifloat.dst_reg] - get_value_of_immediate(state, instr, address_of_imm));
            } else {
                dst = (void*)(state->registers[instr.ifloat.dst_reg] + get_value_of_immediate(state, instr, address_of_imm));
            }
        } break;
    }

    switch(instr.type) {
        case LVM_FCMP:{
            if(float_32_register(instr.ifloat.dst_reg)) {
                state->rfloat_flags.bigger_than = *(r32*)dst > *(r32*)src;
                state->rfloat_flags.less_than = *(r32*)dst < *(r32*)src;
                state->rfloat_flags.equal = *(r32*)dst == *(r32*)src;
            } else {
                state->rfloat_flags.bigger_than = *(r64*)dst > *(r64*)src;
                state->rfloat_flags.less_than = *(r64*)dst < *(r64*)src;
                state->rfloat_flags.equal = *(r64*)dst == *(r64*)src;
            }
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

bool
light_vm_execute_float_branch_instruction(Light_VM_State* state, Light_VM_Instruction instr) {
    void* address_of_imm = ((void*)state->registers[RIP]) + sizeof(Light_VM_Instruction); // address of immediate
    u64 imm_val = get_value_of_immediate(state, instr, address_of_imm);

    bool branch = false;

    switch(instr.type) {
        case LVM_FBEQ:{
            branch = state->rfloat_flags.equal;
        }break;
        case LVM_FBNE:{
            branch = !state->rfloat_flags.equal;
        }break;
        case LVM_FBGT:{
            branch = state->rfloat_flags.bigger_than;
        }break;
        case LVM_FBLT:{
            branch = state->rfloat_flags.less_than;
        }break;
        default: assert(0); break;
    }

    if(branch){        
        switch(instr.branch.addr_mode) {
            case BRANCH_ADDR_MODE_IMMEDIATE_ABSOLUTE:
                state->registers[RIP] = imm_val;
                break;
            case BRANCH_ADDR_MODE_IMMEDIATE_RELATIVE:
                state->registers[RIP] += imm_val;
                break;
            case BRANCH_ADDR_MODE_REGISTER:
                state->registers[RIP] = state->registers[instr.branch.reg];
                break;
            case BRANCH_ADDR_MODE_REGISTER_INDIRECT:
                state->registers[RIP] = *(u64*)state->registers[instr.branch.reg];
                break;
            default: assert(0); break;
        }
    }

    return branch;
}

void
light_vm_execute_external_call_instruction(Light_VM_State* state, Light_VM_Instruction instr) {
    void* address_of_imm = ((void*)state->registers[RIP]) + sizeof(Light_VM_Instruction); // address of immediate
    void* jmp_address = 0;

    // Jump to destination address
    switch(instr.branch.addr_mode) {
        case BRANCH_ADDR_MODE_IMMEDIATE_ABSOLUTE:{
            u64 imm_val = get_value_of_immediate(state, instr, address_of_imm);
            jmp_address = (void*)imm_val;
        } break;
        case BRANCH_ADDR_MODE_IMMEDIATE_RELATIVE:{
            assert(0); // invalid
        } break;
        case BRANCH_ADDR_MODE_REGISTER:
            jmp_address = (void*)state->registers[instr.branch.reg];
            break;
        case BRANCH_ADDR_MODE_REGISTER_INDIRECT:
            jmp_address = (void*)*(u64*)state->registers[instr.branch.reg];
            break;
        default: assert(0); break;
    }

    u64 flt_ret = 0;
    u64 res = lvm_ext_call(&state->ext_stack, jmp_address, &flt_ret);
    state->registers[R0] = res;
    state->f32registers[FR0] = *(r32*)&flt_ret; // return value of r32 in FR0
    state->f64registers[FR4] = *(r64*)&flt_ret; // return value of r64 in FR4
}

void
light_vm_execute_push_instruction(Light_VM_State* state, Light_VM_Instruction instr) {
    void* address_of_imm = ((void*)state->registers[RIP]) + sizeof(Light_VM_Instruction); // address of immediate

    void* dst = (void*)state->registers[RSP];
    void* src = 0;

    switch(instr.push.addr_mode) {
        case PUSH_ADDR_MODE_IMMEDIATE:{
            u64 imm = get_value_of_immediate(state, instr, address_of_imm);
            src = &imm;
        } break;
        case PUSH_ADDR_MODE_IMMEDIATE_INDIRECT:{
            u64 imm = get_value_of_immediate(state, instr, address_of_imm);
            src = (void*)imm;
        } break;
        case PUSH_ADDR_MODE_REGISTER:{
            src = &state->registers[instr.push.reg];
        } break;
        case PUSH_ADDR_MODE_REGISTER_INDIRECT:{
            src = (void*)state->registers[instr.push.reg];
        } break;
        default: assert(0); break;
    }

    switch(instr.push.byte_size) {
        case 1: *(u8*)dst = *(u8*)src; break;
        case 2: *(u16*)dst = *(u16*)src; break;
        case 4: *(u32*)dst = *(u32*)src; break;
        case 8: *(u64*)dst = *(u64*)src; break;
        default: assert(0); break;
    }
    state->registers[RSP] += instr.push.byte_size;
}

void
light_vm_execute_expush_instruction(Light_VM_State* state, Light_VM_Instruction instr) {
    void* address_of_imm = ((void*)state->registers[RIP]) + sizeof(Light_VM_Instruction); // address of immediate

    void* src = 0;
    u64* dst = 0;

    switch(instr.push.addr_mode) {
        case PUSH_ADDR_MODE_IMMEDIATE:{
            u64 imm = get_value_of_immediate(state, instr, address_of_imm);
            src = &imm;
        } break;
        case PUSH_ADDR_MODE_IMMEDIATE_INDIRECT:{
            u64 imm = get_value_of_immediate(state, instr, address_of_imm);
            src = (void*)imm;
        } break;
        case PUSH_ADDR_MODE_REGISTER:{
            if(instr.type == LVM_EXPUSHI) {
                src = &state->registers[instr.push.reg];
            } else if(instr.type == LVM_EXPUSHF) {
                if(float_32_register(instr.push.reg)) {
                    src = &state->f32registers[instr.push.reg];
                } else {
                    src = &state->f64registers[instr.push.reg];
                }
            }
        } break;
        case PUSH_ADDR_MODE_REGISTER_INDIRECT:{
            src = (void*)state->registers[instr.push.reg];
        } break;
        default: assert(0); break;
    }

    s32 total_arg_count = state->ext_stack.int_arg_count + state->ext_stack.float_arg_count;
    switch(instr.type) {
        case LVM_EXPUSHI:{
            dst = &state->ext_stack.int_values[state->ext_stack.int_arg_count];
            state->ext_stack.int_index[state->ext_stack.int_arg_count] = total_arg_count;
            state->ext_stack.int_arg_count++;
            switch(instr.push.byte_size) {
                case 1: *dst = (u64)*(u8*)src; break;
                case 2: *dst = (u64)*(u16*)src; break;
                case 4: *dst = (u64)*(u32*)src; break;
                case 8: *dst = *(u64*)src; break;
                default: assert(0); break;
            }
        }break;
        case LVM_EXPUSHF:{
            dst = &state->ext_stack.float_values[state->ext_stack.float_arg_count];
            state->ext_stack.float_index[state->ext_stack.float_arg_count] = total_arg_count;
            state->ext_stack.float_arg_count++;
            if(float_32_register(instr.push.reg)) {
                *dst = (u64)*(u32*)src;
            } else {
                *dst = *(u64*)src;
            }
        }break;
        default: assert(0); break;
    }

}

void
light_vm_execute_call_instruction(Light_VM_State* state, Light_VM_Instruction instr) {
    void* address_of_imm = ((void*)state->registers[RIP]) + sizeof(Light_VM_Instruction); // address of immediate

    // Push return value
    u64* dst = (u64*)state->registers[RSP];
    *dst = (u64)((void*)state->registers[RIP] + sizeof(instr) + instr.imm_size_bytes);
    state->registers[RSP] += sizeof(u64);

    // Jump to destination address
    switch(instr.branch.addr_mode) {
        case BRANCH_ADDR_MODE_IMMEDIATE_ABSOLUTE:{
            s64 imm_val = get_signed_value_of_immediate(state, instr, address_of_imm);
            state->registers[RIP] = imm_val;
        } break;
        case BRANCH_ADDR_MODE_IMMEDIATE_RELATIVE:{
            s64 imm_val = get_signed_value_of_immediate(state, instr, address_of_imm);
            state->registers[RIP] += imm_val;
        } break;
        case BRANCH_ADDR_MODE_REGISTER:
            state->registers[RIP] = state->registers[instr.branch.reg];
            break;
        case BRANCH_ADDR_MODE_REGISTER_INDIRECT:
            state->registers[RIP] = *(u64*)state->registers[instr.branch.reg];
            break;
        default: assert(0); break;
    }
}

// Return if the branch is taken or not
bool
light_vm_execute_branch_instruction(Light_VM_State* state, Light_VM_Instruction instr) {
    void* address_of_imm = ((void*)state->registers[RIP]) + sizeof(Light_VM_Instruction); // address of immediate
    s64 imm_val = get_signed_value_of_immediate(state, instr, address_of_imm);

    bool branch = false;
    switch(instr.type) {
        case LVM_BEQ:{
            branch = (state->rflags.zerof);
        }break;
        case LVM_BNE:{
            branch = !(state->rflags.zerof);
        }break;
        case LVM_BLT_S: {
            branch = (state->rflags.sign != state->rflags.overflow);
        }break;
        case LVM_BGT_S:{
            branch = (!state->rflags.zerof && (state->rflags.sign == state->rflags.overflow));
        }break;
        case LVM_BLE_S:{
            branch = (state->rflags.zerof || (state->rflags.sign != state->rflags.overflow));
        }break;
        case LVM_BGE_S:{
            branch = state->rflags.sign == state->rflags.overflow;
        }break;
        case LVM_BLT_U:{
            branch = state->rflags.carry;
        }break;
        case LVM_BGT_U:{
            branch = !state->rflags.carry && !state->rflags.zerof;
        }break;
        case LVM_BLE_U:{
            branch = state->rflags.carry || state->rflags.zerof;
        }break;
        case LVM_BGE_U:{
            branch = !state->rflags.carry;
        }break;
        case LVM_JMP:{
            branch = true;
        }break;
        default: assert(0); break;
    }
    if(branch){        
        switch(instr.branch.addr_mode) {
            case BRANCH_ADDR_MODE_IMMEDIATE_ABSOLUTE:
                state->registers[RIP] = imm_val;
                break;
            case BRANCH_ADDR_MODE_IMMEDIATE_RELATIVE:
                state->registers[RIP] += imm_val;
                break;
            case BRANCH_ADDR_MODE_REGISTER:
                state->registers[RIP] = state->registers[instr.branch.reg];
                break;
            case BRANCH_ADDR_MODE_REGISTER_INDIRECT:
                state->registers[RIP] = *(u64*)state->registers[instr.branch.reg];
                break;
            default: assert(0); break;
        }
    }
    return branch;
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
            light_vm_execute_push_instruction(state, instr);
            advance_ip = true;
        }break;
        case LVM_POP:{
            switch(instr.unary.byte_size) {
                case 1: state->registers[instr.unary.reg] = (u64)*((u8*)state->registers[RSP] - 1); break;
                case 2: state->registers[instr.unary.reg] = (u64)*((u16*)state->registers[RSP] - 1); break;
                case 4: state->registers[instr.unary.reg] = (u64)*((u32*)state->registers[RSP] - 1); break;
                case 8: state->registers[instr.unary.reg] = *((u64*)state->registers[RSP] - 1); break;
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
        case LVM_JMP:{
            advance_ip = !light_vm_execute_branch_instruction(state, instr);
        } break;

        case LVM_FBEQ:
        case LVM_FBNE:
        case LVM_FBGT:
        case LVM_FBLT:
            advance_ip = !light_vm_execute_float_branch_instruction(state, instr);
            break;

        // External calls
        case LVM_EXTCALL:
            light_vm_execute_external_call_instruction(state, instr);
            advance_ip = true;
            break;
        
        case LVM_EXPUSHF:
        case LVM_EXPUSHI: {
            light_vm_execute_expush_instruction(state, instr);
            advance_ip = true;
        }break;
        case LVM_EXPOP: {
            // Clear external stack
            state->ext_stack.int_arg_count = 0;
            state->ext_stack.float_arg_count = 0;
            advance_ip = true;
        }break;

        case LVM_CALL:{
            light_vm_execute_call_instruction(state, instr);
        } break;
        case LVM_RET: {
            // Pop RIP
            state->registers[RIP] = *((u64*)state->registers[RSP] - 1);
            state->registers[RSP] -= sizeof(u64);
        } break;

        case LVM_COPY:{
            memcpy(
                (void*)state->registers[instr.copy.dst_reg], 
                (void*)state->registers[instr.copy.src_reg], 
                state->registers[instr.copy.size_bytes_reg]);
            advance_ip = true;
        } break;
        
        case LVM_ALLOC: {
            u64 val = get_value_of_register(state, instr.alloc.size_reg, instr.alloc.byte_size);
            void* mem = calloc(1, val);
            state->registers[instr.alloc.dst_reg] = (u64)mem;
            advance_ip = true;
        } break;

        // TODO(psv):
        case LVM_ASSERT:  break;

        // Halt
        case LVM_HLT: break;

        default: {
        }break;
    }
    if(advance_ip) {
        state->registers[RIP] += sizeof(Light_VM_Instruction) + instr.imm_size_bytes;
    }
}

void
light_vm_reset(Light_VM_State* state) {
    state->registers[RIP] = (u64)(Light_VM_Instruction*)(state->code.block);
    state->registers[RBP] = (u64)(Light_VM_Instruction*)(state->stack.block);
    state->registers[RSP] = state->registers[RBP];
    state->registers[RDP] = (u64)(Light_VM_Instruction*)(state->data.block);

    state->registers[R0] = 0;
    state->registers[R1] = 0;
    state->registers[R2] = 0;
    state->registers[R3] = 0;
    state->registers[R4] = 0;
    state->registers[R5] = 0;
    state->registers[R6] = 0;
    state->registers[R7] = 0;

    state->f32registers[FR0] = 0.0f;
    state->f32registers[FR1] = 0.0f;
    state->f32registers[FR2] = 0.0f;
    state->f32registers[FR3] = 0.0f;

    state->f64registers[FR4] = 0.0;
    state->f64registers[FR5] = 0.0;
    state->f64registers[FR6] = 0.0;
    state->f64registers[FR7] = 0.0;
}

void
light_vm_execute(Light_VM_State* state, void* entry_point, bool print_steps) {
    light_vm_reset(state);

    if(entry_point != 0) {
        state->registers[RIP] = (u64)entry_point;
    }

    for(u64 i = 0;; ++i) {
        Light_VM_Instruction in = *(Light_VM_Instruction*)(state->registers[RIP]);

        if(print_steps) {
            void* addr_of_imm = ((void*)state->registers[RIP]) + sizeof(Light_VM_Instruction); // address of immediate
            u64 imm = get_value_of_immediate(state, in, addr_of_imm);
            light_vm_print_instruction(stdout, in, imm);
        }

        if(in.type == LVM_HLT) break;

        light_vm_execute_instruction(state, in);
    }
}