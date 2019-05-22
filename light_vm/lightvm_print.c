#include "lightvm.h"

void
print_float_register(FILE* out, u8 reg) {
    switch(reg) {
        case FR0: fprintf(out, "FR0(32)"); break;
        case FR1: fprintf(out, "FR1(32)"); break;
        case FR2: fprintf(out, "FR2(32)"); break;
        case FR3: fprintf(out, "FR3(32)"); break;
        case FR4: fprintf(out, "FR4(64)"); break;
        case FR5: fprintf(out, "FR5(64)"); break;
        case FR6: fprintf(out, "FR6(64)"); break;
        case FR7: fprintf(out, "FR7(64)"); break;
        default: fprintf(out, "Invalid float register"); break;
    }
}

void
print_register(FILE* out, u8 reg, u8 byte_size) {
    switch(reg) {
        case R0: fprintf(out, "R0"); break;
        case R1: fprintf(out, "R1"); break;
        case R2: fprintf(out, "R2"); break;
        case R3: fprintf(out, "R3"); break;
        case R4: fprintf(out, "R4"); break;
        case R5: fprintf(out, "R5"); break;
        case R6: fprintf(out, "R6"); break;
        case R7: fprintf(out, "R7"); break;
        case RIP: fprintf(out, "RIP"); break;
        case RSP: fprintf(out, "RSP"); break;
        case RBP: fprintf(out, "RBP"); break;
        case RDP: fprintf(out, "RDP"); break;
        case R_FLAGS: fprintf(out, "R_FLAGS"); break;
        default: fprintf(out, "Invalid register"); break;
    }
    switch(byte_size) {
        case 1: fprintf(out, "b"); break;
        case 2: fprintf(out, "w"); break;
        case 4: fprintf(out, "d"); break;
        default: break;
    }
}

void print_immediate(FILE* out, u8 byte_size, u64 imm) {
    switch(byte_size) {
        case 1:
            fprintf(out, "0x%x", (u8)(imm & 0xff)); break;
        case 2:
            fprintf(out, "0x%x", (u16)(imm & 0xffff)); break;
        case 4:
            fprintf(out, "0x%x", (u32)(imm & 0xffffffff)); break;
        case 8:
            fprintf(out, "0x%llx", imm); break;
        default: fprintf(out, "Invalid immediate byte size");
    }
}

void
print_binary_instruction(FILE* out, Light_VM_Instruction instr, u64 immediate) {
    switch(instr.type) {
        case LVM_CMP:   fprintf(out, "CMP "); break;
        case LVM_SHL:   fprintf(out, "SHL "); break;
        case LVM_SHR:   fprintf(out, "SHR "); break;
        case LVM_OR:    fprintf(out, "OR "); break;
        case LVM_AND:   fprintf(out, "AND "); break;
        case LVM_XOR:   fprintf(out, "XOR "); break;
        case LVM_MOV:   fprintf(out, "MOV "); break;
        case LVM_ADD_S: fprintf(out, "ADDS "); break;
        case LVM_SUB_S: fprintf(out, "SUBS "); break;
        case LVM_MUL_S: fprintf(out, "MULS "); break;
        case LVM_DIV_S: fprintf(out, "DIVS "); break;
        case LVM_MOD_S: fprintf(out, "MODS "); break;
        case LVM_ADD_U: fprintf(out, "ADDU "); break;
        case LVM_SUB_U: fprintf(out, "SUBU "); break;
        case LVM_MUL_U: fprintf(out, "MULU "); break;
        case LVM_DIV_U: fprintf(out, "DIVU "); break;
        case LVM_MOD_U: fprintf(out, "MODU "); break;
        default: assert(0); break; // internal error
    }
    switch(instr.binary.addr_mode) {
        case BIN_ADDR_MODE_REG_TO_REG:{
            print_register(out, instr.binary.dst_reg, instr.binary.bytesize);
            fprintf(out, ", ");
            print_register(out, instr.binary.src_reg, instr.binary.bytesize);
        }break;
        case BIN_ADDR_MODE_REG_TO_MEM:{
            fprintf(out, "[");
            print_register(out, instr.binary.dst_reg, 8);
            fprintf(out, "], ");
            print_register(out, instr.binary.src_reg, instr.binary.bytesize);
        }break;
        case BIN_ADDR_MODE_REG_TO_IMM_MEM:{
            fprintf(out, "[");
            print_immediate(out, instr.imm_size_bytes, immediate);
            fprintf(out, "], ");
            print_register(out, instr.binary.src_reg, instr.binary.bytesize);
        }break;
        case BIN_ADDR_MODE_MEM_TO_REG:{
            print_register(out, instr.binary.dst_reg, instr.binary.bytesize);
            fprintf(out, ", [");
            print_register(out, instr.binary.src_reg, 8);
            fprintf(out, "]");
        }break;
        case BIN_ADDR_MODE_MEM_IMM_TO_REG:{
            print_register(out, instr.binary.dst_reg, instr.binary.bytesize);
            fprintf(out, ", [");
            print_immediate(out, instr.imm_size_bytes, immediate);
            fprintf(out, "]");
        }break;
        case BIN_ADDR_MODE_IMM_TO_REG:{
            print_register(out, instr.binary.dst_reg, instr.binary.bytesize);
            fprintf(out, ", ");
            print_immediate(out, instr.imm_size_bytes, immediate);
        }break;
        case BIN_ADDR_MODE_REG_TO_MEM_OFFSETED:{
            fprintf(out, "[");
            print_register(out, instr.binary.dst_reg, 8);
            fprintf(out, " + ");
            print_immediate(out, instr.imm_size_bytes, immediate);
            fprintf(out, "], ");
            print_register(out, instr.binary.src_reg, instr.binary.bytesize);
        }break;
        case BIN_ADDR_MODE_REG_OFFSETED_TO_REG: {
            print_register(out, instr.binary.dst_reg, instr.binary.bytesize);
            fprintf(out, ", [");
            print_register(out, instr.binary.src_reg, 8);
            fprintf(out, " + ");
            print_immediate(out, instr.imm_size_bytes, immediate);
            fprintf(out, "]");
        }break;
        default: {
            fprintf(out, "Invalid addressing mode");
        }break;
    }
}

void
print_unary_instruction(FILE* out, Light_VM_Instruction instr, u64 imm) {
    switch(instr.type) {
        case LVM_NOT:  fprintf(out, "NOT "); break;
        case LVM_PUSH: fprintf(out, "PUSH "); break;
        case LVM_POP:  fprintf(out, "POP "); break;
        default: fprintf(out, "Invalid unary instruction"); break;
    }
    print_register(out, instr.unary.reg, instr.imm_size_bytes);
}

void
print_call_instruction(FILE* out, Light_VM_Instruction instr, u64 imm) {
    switch(instr.type) {
        case LVM_CALL:    fprintf(out, "CALL "); break;
        case LVM_EXTCALL: fprintf(out, "EXTCALL "); break;
        default: fprintf(out, "Invalid call instruction"); break;
    }
    switch(instr.call.addr_mode) {
        case CALL_ADDR_MODE_IMMEDIATE_ABSOLUTE:{
            fprintf(out, "0x%llx", imm);
        } break;
        case CALL_ADDR_MODE_IMMEDIATE_RELATIVE:{
            print_immediate(out, instr.imm_size_bytes, imm);
        } break;
        case CALL_ADDR_MODE_REGISTER:{
            print_register(out, instr.call.reg, 8);
        } break;
        case CALL_ADDR_MODE_REGISTER_INDIRECT:{
            fprintf(out, "[");
            print_register(out, instr.call.reg, 8);
            fprintf(out, "]");
        } break;
        default: fprintf(out, "Invalid call addressing mode"); break;
    }
}

void
print_float_instruction(FILE* out, Light_VM_Instruction instr, u64 imm) {
    switch(instr.type) {
        case LVM_FADD: fprintf(out, "FADD "); break;
        case LVM_FSUB: fprintf(out, "FSUB "); break;
        case LVM_FDIV: fprintf(out, "FDIV "); break;
        case LVM_FMUL: fprintf(out, "FMUL "); break;
        case LVM_FCMP: fprintf(out, "FCMP "); break;
        case LVM_FMOV: fprintf(out, "FMOV "); break;
        default: fprintf(out, "Invalid float instruction"); break;
    }
    switch(instr.ifloat.addr_mode) {
        case FLOAT_ADDR_MODE_REG_TO_REG:{
            print_float_register(out, instr.ifloat.dst_reg);
            fprintf(out, ", ");
            print_float_register(out, instr.ifloat.src_reg);
        }break;
        case FLOAT_ADDR_MODE_REG_TO_MEM:{
            // dst reg must be a general purpose one
            fprintf(out, "[");
            print_register(out, instr.ifloat.dst_reg, 8);
            fprintf(out, "], ");
            print_float_register(out, instr.ifloat.src_reg);
        }break;
        case FLOAT_ADDR_MODE_MEM_TO_REG:{
            // src reg must be a general purpose one
            print_float_register(out, instr.ifloat.dst_reg);
            fprintf(out, ", [");
            print_register(out, instr.ifloat.src_reg, 8);
            fprintf(out, "]");
        }break;
        case FLOAT_ADDR_MODE_REG_TO_MEM_OFFSETED:{
            // dst reg must be a general purpose one
            fprintf(out, "[");
            print_register(out, instr.ifloat.dst_reg, 8);
            fprintf(out, " + ");
            print_immediate(out, instr.imm_size_bytes, imm);
            fprintf(out, "], ");
            print_float_register(out, instr.ifloat.src_reg);
        }break;
        case FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG:{
            print_float_register(out, instr.ifloat.dst_reg);
            fprintf(out, ", [");
            print_register(out, instr.ifloat.src_reg, 8);
            fprintf(out, " + ");
            print_immediate(out, instr.imm_size_bytes, imm);
            fprintf(out, "]");
        }break;
        default:{
            fprintf(out, "Invalid float addressing mode");
        }break;
    }
}

void
print_branch_instruction(FILE* out, Light_VM_Instruction instr, u64 imm) {
    switch(instr.type) {
        case LVM_BEQ:   fprintf(out, "BEQ "); break;
        case LVM_BNE:   fprintf(out, "BNE "); break;
        case LVM_BLT_S: fprintf(out, "BLTS "); break;
        case LVM_BGT_S: fprintf(out, "BGTS "); break;
        case LVM_BLE_S: fprintf(out, "BLES "); break;
        case LVM_BGE_S: fprintf(out, "BGES "); break;
        case LVM_BLT_U: fprintf(out, "BLTU "); break;
        case LVM_BGT_U: fprintf(out, "BGTU "); break;
        case LVM_BLE_U: fprintf(out, "BLEU "); break;
        case LVM_BGE_U: fprintf(out, "BGEU "); break;
        case LVM_JMP:   fprintf(out, "JMP "); break;
        default: fprintf(out, "Invalid comparison instruction"); break;
    }

    print_immediate(out, instr.imm_size_bytes, imm);
}

void 
print_instruction(FILE* out, Light_VM_Instruction instr, u64 imm) {
    
    switch(instr.type) {
        case LVM_NOP: fprintf(out, "NOP"); break;

        // Binary instructions
        case LVM_CMP:
        case LVM_SHL: case LVM_SHR:
        case LVM_OR: case LVM_AND:
        case LVM_XOR: case LVM_MOV:
        case LVM_ADD_S: case LVM_SUB_S:
        case LVM_MUL_S: case LVM_DIV_S:
        case LVM_MOD_S: case LVM_ADD_U:
        case LVM_SUB_U: case LVM_MUL_U:
        case LVM_DIV_U: case LVM_MOD_U:
            print_binary_instruction(out, instr, imm); 
            break;

        // Floating point
        case LVM_FADD:
        case LVM_FSUB:
        case LVM_FMUL:
        case LVM_FDIV:
        case LVM_FCMP:
        case LVM_FMOV:
            print_float_instruction(out, instr, imm); 
            break;

        // Unary instructions
        case LVM_NOT:
        case LVM_PUSH:
        case LVM_POP:
            print_unary_instruction(out, instr, imm);
            break;

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
            print_branch_instruction(out, instr, imm);
            break;

        case LVM_EXTCALL:
        case LVM_CALL:
            print_call_instruction(out, instr, imm);
            break;
        case LVM_RET:  fprintf(out, "RET"); break;

        // TODO(psv):
        case LVM_COPY:    fprintf(out, "COPY"); break;
        case LVM_ASSERT:  fprintf(out, "ASSERT"); break;

        // Halt
        case LVM_HLT: fprintf(out, "HLT"); break;

        default: {
            fprintf(out, "UNKNOWN");
        }break;
    }
    fprintf(out, "\n");
}


void 
light_vm_debug_dump_registers(FILE* out, Light_VM_State* state) {
    fprintf(out, "R0: 0x%llx \t R1: 0x%llx\n", state->registers[R0], state->registers[R1]);
    fprintf(out, "R2: 0x%llx \t R3: 0x%llx\n", state->registers[R2], state->registers[R3]);
    fprintf(out, "R4: 0x%llx \t R5: 0x%llx\n", state->registers[R4], state->registers[R5]);
    fprintf(out, "R6: 0x%llx \t R7: 0x%llx\n", state->registers[R6], state->registers[R7]);
    fprintf(out, "\n");
    fprintf(out, "RSP: 0x%llx \t RBP: 0x%llx\n", state->registers[RSP], state->registers[RBP]);
    fprintf(out, "RIP: 0x%llx \t RDP: 0x%llx\n", state->registers[RIP], state->registers[RDP]);
    fprintf(out, "\n");
    fprintf(out, "FR0: %f \t FR1: %f\n", state->f32registers[FR0], state->f32registers[FR1]);
    fprintf(out, "FR2: %f \t FR3: %f\n", state->f32registers[FR2], state->f32registers[FR3]);
    fprintf(out, "FR4: %f \t FR5: %f\n", state->f64registers[FR4], state->f64registers[FR5]);
    fprintf(out, "FR6: %f \t FR7: %f\n", state->f64registers[FR6], state->f64registers[FR7]);
    fprintf(out, "\n");
    fprintf(out, "Carry: %d ", ((Light_VM_Flags_Register*)&state->registers[R_FLAGS])->carry);
    fprintf(out, "Zero: %d ", ((Light_VM_Flags_Register*)&state->registers[R_FLAGS])->zerof);
    fprintf(out, "Sign: %d ", ((Light_VM_Flags_Register*)&state->registers[R_FLAGS])->sign);
    fprintf(out, "Overflow: %d", ((Light_VM_Flags_Register*)&state->registers[R_FLAGS])->overflow);
    fprintf(out, "\n");
}

void 
light_vm_debug_dump_registers_dec(FILE* out, Light_VM_State* state) {
    fprintf(out, "R0: %lld \t R1: %lld\n", state->registers[R0], state->registers[R1]);
    fprintf(out, "R2: %lld \t R3: %lld\n", state->registers[R2], state->registers[R3]);
    fprintf(out, "R4: %lld \t R5: %lld\n", state->registers[R4], state->registers[R5]);
    fprintf(out, "R6: %lld \t R7: %lld\n", state->registers[R6], state->registers[R7]);
    fprintf(out, "\n");
    fprintf(out, "RSP: 0x%llx \t RBP: 0x%llx\n", state->registers[RSP], state->registers[RBP]);
    fprintf(out, "RIP: 0x%llx \t RDP: 0x%llx\n", state->registers[RIP], state->registers[RDP]);
    fprintf(out, "\n");
    fprintf(out, "FR0: %f \t FR1: %f\n", state->f32registers[FR0], state->f32registers[FR1]);
    fprintf(out, "FR2: %f \t FR3: %f\n", state->f32registers[FR2], state->f32registers[FR3]);
    fprintf(out, "FR4: %f \t FR5: %f\n", state->f64registers[FR4], state->f64registers[FR5]);
    fprintf(out, "FR6: %f \t FR7: %f\n", state->f64registers[FR6], state->f64registers[FR7]);
    fprintf(out, "\n");
    fprintf(out, "Carry: %d ", ((Light_VM_Flags_Register*)&state->registers[R_FLAGS])->carry);
    fprintf(out, "Zero: %d ", ((Light_VM_Flags_Register*)&state->registers[R_FLAGS])->zerof);
    fprintf(out, "Sign: %d ", ((Light_VM_Flags_Register*)&state->registers[R_FLAGS])->sign);
    fprintf(out, "Overflow: %d", ((Light_VM_Flags_Register*)&state->registers[R_FLAGS])->overflow);
    fprintf(out, "\n");
}

void
light_vm_debug_dump_code(FILE* out, Light_VM_State* state) {
    for(u64 i = 0 ;;) {
        u64 imm = 0;
        Light_VM_Instruction inst = *(Light_VM_Instruction*)(state->code.block + i);
        i += sizeof(Light_VM_Instruction);

        if(inst.type == LVM_NOP) break;

        switch(inst.imm_size_bytes) {
            case 0: break;
            case 1: 
                imm = *(u8*)(state->code.block + i);
                break;
            case 2:
                imm = *(u16*)(state->code.block + i);
                break;
            case 4:
                imm = *(u32*)(state->code.block + i);
                break;
            case 8:
                imm = *(u64*)(state->code.block + i);
                break;
            default: break;
        }
        print_instruction(out, inst, imm);
        i += inst.imm_size_bytes;
    }
}