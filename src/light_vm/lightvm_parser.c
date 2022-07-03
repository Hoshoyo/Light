#define _CRT_SECURE_NO_WARNINGS
#include "lightvm.h"
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include "common.h"

static bool start_with(const char* start, const char* str, s32* count) {
    *count = 0;
    while(1) {
        if(!start[*count])
            return 1;
        if(start[*count] != str[*count])
            return 0;
        (*count)++;
    }
}

static bool is_number(char c) {
    return (c >= '0' && c <= '9');
}

static bool is_hex_number(char c) {
    return (
        (c >= '0' && c <= '9') || 
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F')
        );
}

static u8
get_float_register(const char** at, bool* is64bit) {
    u8 reg = 0;
    if(**at == 'e')
    {
        (*at)++;
        *is64bit = true;
    }
    else
        *is64bit = false;
    assert(**at == 'f');
    (*at)++;
    assert(**at == 'r');
    (*at)++;
    assert(is_number(**at));
    reg = **at + FR0 - '0';
    (*at)++;
    assert(reg >= FR0 && reg <= FR7);

    return reg;
}

// r0  64
// r0d 32
// r0w 16
// r0b 8
static u8
get_register(const char** at, u8* byte_size) {
    u8 reg = 0;
    assert(**at == 'r');
    (*at)++;
    if(is_number(**at)) {
        reg = **at + R0 - '0';
        (*at)++;
    } else {
        if(**at == 's') { // rsp
            (*at)++;
            reg = LRSP;
        } else if(**at == 'b') { // rbp
            (*at)++;
            reg = LRBP;
        } else if(**at == 'i') { // rip
            (*at)++;
            reg = LRIP;
        } else if(**at == 'd') { // rdp
            (*at)++;
            reg = LRDP;
        }
        assert(**at == 'p');
        (*at)++;
    }
    if(byte_size) {
        switch(**at) {
            case 'b': (*at)++; *byte_size = 1; break;
            case 'w': (*at)++; *byte_size = 2; break;
            case 'd': (*at)++; *byte_size = 4; break;
            default:  *byte_size = 8; break;
        }
    } else {
        assert(**at != 'b' && **at != 'w' && **at != 'd');
    }

    return reg;
}

static void
eat_whitespace(const char** at) {
    while(**at == ' ') {
        (*at)++;
    }
}

static u8
instruction_type(const char** at) {
    s32 count = 0;
    u8 type = 0;
    if(start_with("adds", *at, &count)) {
        type = LVM_ADD_S;
    } else if(start_with("subs", *at, &count)) {
        type = LVM_SUB_S;
    } else if(start_with("muls", *at, &count)) {
        type = LVM_MUL_S;
    } else if(start_with("divs", *at, &count)) {
        type = LVM_DIV_S;
    } else if(start_with("mods", *at, &count)) {
        type = LVM_MOD_S;
    } else if(start_with("addu", *at, &count)) {
        type = LVM_ADD_U;
    } else if(start_with("subu", *at, &count)) {
        type = LVM_SUB_U;
    } else if(start_with("mulu", *at, &count)) {
        type = LVM_MUL_U;
    } else if(start_with("divu", *at, &count)) {
        type = LVM_DIV_U;
    } else if(start_with("modu", *at, &count)) {
        type = LVM_MOD_U;
    } else if(start_with("neg", *at, &count)) {
        type = LVM_NEG;
    } else if(start_with("moveq", *at, &count)) {
        type = LVM_MOVEQ;
    } else if(start_with("movne", *at, &count)) {
        type = LVM_MOVNE;
    } else if(start_with("movlts", *at, &count)) {
        type = LVM_MOVLT_S;
    } else if(start_with("movgts", *at, &count)) {
        type = LVM_MOVGT_S;
    } else if(start_with("movles", *at, &count)) {
        type = LVM_MOVLE_S;
    } else if(start_with("movges", *at, &count)) {
        type = LVM_MOVGE_S;
    } else if(start_with("movltu", *at, &count)) {
        type = LVM_MOVLT_U;
    } else if(start_with("movgtu", *at, &count)) {
        type = LVM_MOVGT_U;
    } else if(start_with("movleu", *at, &count)) {
        type = LVM_MOVLE_U;
    } else if(start_with("movgeu", *at, &count)) {
        type = LVM_MOVGE_U;
    } else if(start_with("fmoveq", *at, &count)) {
        type = LVM_FMOVEQ;
    } else if(start_with("fmovne", *at, &count)) {
        type = LVM_FMOVNE;
    } else if(start_with("fmovlt", *at, &count)) {
        type = LVM_FMOVLT;
    } else if(start_with("fmovgt", *at, &count)) {
        type = LVM_FMOVGT;
    } else if(start_with("fmovle", *at, &count)) {
        type = LVM_FMOVLE;
    } else if(start_with("fmovge", *at, &count)) {
        type = LVM_FMOVGE;
    } else if(start_with("mov", *at, &count)) {
        type = LVM_MOV;
    } else if(start_with("shl", *at, &count)) {
        type = LVM_SHL;
    } else if(start_with("shr", *at, &count)) {
        type = LVM_SHR;
    } else if(start_with("or", *at, &count)) {
        type = LVM_OR;
    } else if(start_with("and", *at, &count)) {
        type = LVM_AND;
    } else if(start_with("xor", *at, &count)) {
        type = LVM_XOR;
    } else if(start_with("not", *at, &count)) {
        type = LVM_NOT;
    } else if(start_with("fadd", *at, &count)) {
        type = LVM_FADD;
    } else if(start_with("fsub", *at, &count)) {
        type = LVM_FSUB;
    } else if(start_with("fmul", *at, &count)) {
        type = LVM_FMUL;
    } else if(start_with("fdiv", *at, &count)) {
        type = LVM_FDIV;
    } else if(start_with("fneg", *at, &count)) {
        type = LVM_FNEG;
    } else if(start_with("fmov", *at, &count)) {
        type = LVM_FMOV;
    } else if(start_with("fcmp", *at, &count)) {
        type = LVM_FCMP;
    } else if(start_with("cmp", *at, &count)) {
        type = LVM_CMP;
    } else if(start_with("cvtr32s32", *at, &count)) {
        type = LVM_CVT_R32_S32;
    } else if(start_with("cvtr32s64", *at, &count)) {
        type = LVM_CVT_R32_S64;
    } else if(start_with("cvts32r32", *at, &count)) {
        type = LVM_CVT_S32_R32;
    } else if(start_with("cvts32r64", *at, &count)) {
        type = LVM_CVT_S32_R64;
    } else if(start_with("cvts64r64", *at, &count)) {
        type = LVM_CVT_S64_R64;
    } else if(start_with("cvts64r32", *at, &count)) {
        type = LVM_CVT_S64_R32;
    } else if(start_with("cvtr32r64", *at, &count)) {
        type = LVM_CVT_R32_R64;
    } else if(start_with("cvtr64r32", *at, &count)) {
        type = LVM_CVT_R64_R32;
    } else if(start_with("cvtu64r32", *at, &count)) {
        type = LVM_CVT_U64_R32;
    } else if(start_with("cvtu64r64", *at, &count)) {
        type = LVM_CVT_U64_R64;
    } else if(start_with("cvtsext", *at, &count)) {
        type = LVM_CVT_SEXT;
    } else if(start_with("beq", *at, &count)) {
        type = LVM_BEQ;
    } else if(start_with("bne", *at, &count)) {
        type = LVM_BNE;
    } else if(start_with("blts", *at, &count)) {
        type = LVM_BLT_S;
    } else if(start_with("bgts", *at, &count)) {
        type = LVM_BGT_S;
    } else if(start_with("bles", *at, &count)) {
        type = LVM_BLE_S;
    } else if(start_with("bges", *at, &count)) {
        type = LVM_BGE_S;
    } else if(start_with("bltu", *at, &count)) {
        type = LVM_BLT_U;
    } else if(start_with("bgtu", *at, &count)) {
        type = LVM_BGT_U;
    } else if(start_with("bleu", *at, &count)) {
        type = LVM_BLE_U;
    } else if(start_with("bgeu", *at, &count)) {
        type = LVM_BGE_U;
    } else if(start_with("jmp", *at, &count)) {
        type = LVM_JMP;
    } else if(start_with("fbeq", *at, &count)) {
        type = LVM_FBEQ;
    } else if(start_with("fbne", *at, &count)) {
        type = LVM_FBNE;
    } else if(start_with("fbgt", *at, &count)) {
        type = LVM_FBGT;
    } else if(start_with("fblt", *at, &count)) {
        type = LVM_FBLT;
    } else if(start_with("fbge", *at, &count)) {
        type = LVM_FBGT;
    } else if(start_with("fble", *at, &count)) {
        type = LVM_FBLT;
    } else if(start_with("call", *at, &count)) {
        type = LVM_CALL;
    } else if(start_with("ret", *at, &count)) {
        type = LVM_RET;
    } else if(start_with("pop", *at, &count)) {
        type = LVM_POP;
    } else if(start_with("push", *at, &count)) {
        type = LVM_PUSH;    
    } else if(start_with("fpop", *at, &count)) {
        type = LVM_FPOP;
    } else if(start_with("fpush", *at, &count)) {
        type = LVM_FPUSH;
    } else if(start_with("expushi", *at, &count)) {
        type = LVM_EXPUSHI;
    } else if(start_with("expushf", *at, &count)) {
        type = LVM_EXPUSHF;
    } else if(start_with("expop", *at, &count)) {
        type = LVM_EXPOP;
    } else if(start_with("extcall", *at, &count)) {
        type = LVM_EXTCALL;
    } else if(start_with("copy", *at, &count)) {
        type = LVM_COPY;
    } else if(start_with("alloc", *at, &count)) {
        type = LVM_ALLOC;
    } else if(start_with("hlt", *at, &count)) {
        type = LVM_HLT;
    } else {
        assert(0);
    }
    (*at) += count;
    return type;
}

#define EAT_COMMA assert(*at == ','); \
at++; \
assert(*at == ' '); \
at++ \

static u64 str_to_u64(const char* text, int length)
{
	u64 result = 0;
	u64 tenths = 1;
	for (int i = length - 1; i >= 0; --i, tenths *= 10)
		result += (text[i] - 0x30) * tenths;
	return result;
}

static
u8 hexdigit_to_u8(u8 d) {
    if (d >= 'A' && d <= 'F')
        return d - 'A' + 10;
    if (d >= 'a' && d <= 'f')
        return d - 'a' + 10;
    return d - '0';
}

static u64
parse_int_hex(const char* text, int length) {
    u64 res = 0;
    u64 count = 0;
    for (s64 i = length - 1; i >= 0; --i, ++count) {
            if (text[i] == 'x') break;
            char c = hexdigit_to_u8(text[i]);
            res += (u64)c << (count * 4);
    }
    return res;
}

static u64 
parse_number(const char** at, u8* size_bytes) {
    u64 result = 0;
    if(**at == '0' && (*at)[1] == 'x') {
        // hex
        int len = 2;
        for(; is_hex_number((*at)[len]); ++len);
        result = parse_int_hex(*at, len);
        (*at) += len;
    } else {
        // decimal
        int len = 0;
        for(; is_number((*at)[len]); ++len);
        result = str_to_u64(*at, len);
        (*at) += len;
    }
    if(size_bytes){
        if(result <= 0xff)
            *size_bytes = 1;
        else if(result <= 0xffff)
            *size_bytes = 2;
        else if(result <= 0xffffffff)
            *size_bytes = 4;
        else
            *size_bytes = 8;
    }

    return result;
}            

static bool
is_positive(uint64_t value, uint8_t size_bytes) {
    switch(size_bytes)
    {
        case 1: return (value & 0x80) == 0;
        case 2: return (value & 0x8000) == 0;
        case 4: return (value & 0x8000000) == 0;
        case 8: return (value & 0x800000000000000) == 0;
        default: assert(0);
    }
    return 0;
}

Light_VM_Instruction
light_vm_instruction_get(const char* s, uint64_t* immediate) {
    Light_VM_Instruction instruction = {0};

    const char* at = s;

    // get type
    u8 type = instruction_type(&at);

    // skip space    
    eat_whitespace(&at);

    instruction.type = type;

    switch(type) {
        case LVM_NOP:
        case LVM_RET:
        case LVM_HLT:  break;

        // Binary instructions
        case LVM_CMP: case LVM_SHL: case LVM_SHR:
        case LVM_OR: case LVM_AND:
        case LVM_XOR: case LVM_MOV:
        case LVM_ADD_S: case LVM_SUB_S:
        case LVM_MUL_S: case LVM_DIV_S:
        case LVM_MOD_S: case LVM_ADD_U:
        case LVM_SUB_U: case LVM_MUL_U:
        case LVM_DIV_U: case LVM_MOD_U:{
            if(*at == '[') {
                at++;
                if(is_number(*at)) {
                    *immediate = parse_number(&at, &instruction.imm_size_bytes);
                    instruction.binary.addr_mode = BIN_ADDR_MODE_REG_TO_IMM_MEM;
                } else {
                    instruction.binary.dst_reg = get_register(&at, 0);
                    eat_whitespace(&at);
                    if(*at == '+' || *at == '-') {
                        bool negative = (*at == '-');
                        at++;
                        eat_whitespace(&at);
                        *immediate = parse_number(&at, &instruction.imm_size_bytes);
                        assert(instruction.imm_size_bytes <= 4);
                        if(negative) {
                            assert(is_positive(*immediate, instruction.imm_size_bytes));
                            *immediate *= -1;
                        }
                        instruction.binary.addr_mode = BIN_ADDR_MODE_REG_TO_MEM_OFFSETED;
                    } else {
                        instruction.binary.addr_mode = BIN_ADDR_MODE_REG_TO_MEM;
                    }
                }
                eat_whitespace(&at);
                assert(*at == ']');
                at++;
                eat_whitespace(&at);
                EAT_COMMA;
                u8 byte_size = 0;
                instruction.binary.src_reg = get_register(&at, &byte_size);
                instruction.binary.bytesize = byte_size;
            } else {
                u8 byte_size = 0;
                instruction.binary.dst_reg = get_register(&at, &byte_size);
                instruction.binary.bytesize = byte_size;
                EAT_COMMA;
                if(*at == '[') {
                    at++;
                    if(is_number(*at)) {
                        *immediate = parse_number(&at, &instruction.imm_size_bytes);
                        instruction.binary.addr_mode = BIN_ADDR_MODE_MEM_IMM_TO_REG;
                    } else {
                        instruction.binary.src_reg = get_register(&at, 0);
                        eat_whitespace(&at);
                        if(*at == '+' || *at == '-') {
                            bool negative = (*at == '-');
                            at++;
                            eat_whitespace(&at);
                            *immediate = parse_number(&at, &instruction.imm_size_bytes);
                            assert(instruction.imm_size_bytes <= 4);
                            if(negative) {
                                assert(is_positive(*immediate, instruction.imm_size_bytes));
                                *immediate *= -1;
                            }
                            instruction.binary.addr_mode = BIN_ADDR_MODE_REG_OFFSETED_TO_REG;
                        } else {
                            instruction.binary.addr_mode = BIN_ADDR_MODE_MEM_TO_REG;
                        }
                    }
                    assert(*at == ']');
                    at++;
                } else if(is_number(*at)) {
                    *immediate = parse_number(&at, &instruction.imm_size_bytes);
                    instruction.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                } else {
                    u8 src_byte_size = 0;
                    instruction.binary.src_reg = get_register(&at, &src_byte_size);
                    instruction.binary.addr_mode = BIN_ADDR_MODE_REG_TO_REG;
                    assert(src_byte_size == byte_size);
                }
            }
            
        } break;

        // Floating point
        case LVM_FADD: case LVM_FSUB:
        case LVM_FMUL: case LVM_FDIV:
        case LVM_FCMP: case LVM_FMOV: {
            if(*at == '[') {    
                at++;
                instruction.ifloat.dst_reg = get_register(&at, 0);
                eat_whitespace(&at);
                if(*at == '+' || *at == '-') {
                    bool negative = (*at == '-');
                    at++;
                    eat_whitespace(&at);
                    *immediate = parse_number(&at, &instruction.imm_size_bytes);
                    assert(instruction.imm_size_bytes <= 4);
                    if(negative) {
                        assert(is_positive(*immediate, instruction.imm_size_bytes));
                        *immediate *= -1;
                    }
                    instruction.ifloat.addr_mode = FLOAT_ADDR_MODE_REG_TO_MEM_OFFSETED;
                } else {
                    instruction.ifloat.addr_mode = FLOAT_ADDR_MODE_REG_TO_MEM;
                }
                assert(*at == ']');
                at++;
                EAT_COMMA;
                bool is64bit = false;
                instruction.ifloat.src_reg = get_float_register(&at, &is64bit);
                instruction.ifloat.is64bit = is64bit;
            } else {
                bool is64bit = false;
                instruction.ifloat.dst_reg = get_float_register(&at, &is64bit);
                instruction.ifloat.is64bit = is64bit;
                EAT_COMMA;
                if(*at == '[') {
                    at++;
                    instruction.ifloat.src_reg = get_register(&at, 0);
                    eat_whitespace(&at);
                    if(*at == '+' || *at == '-') {
                        bool negative = (*at == '-');
                        at++;
                        eat_whitespace(&at);
                        *immediate = parse_number(&at, &instruction.imm_size_bytes);
                        assert(instruction.imm_size_bytes <= 4);
                        if(negative) {
                            assert(is_positive(*immediate, instruction.imm_size_bytes));
                            *immediate *= -1;
                        }
                        instruction.ifloat.addr_mode = FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG;
                    } else {
                        instruction.ifloat.addr_mode = FLOAT_ADDR_MODE_MEM_TO_REG;
                    }
                    assert(*at == ']');
                    at++;
                } else {
                    bool is64bit = false;
                    instruction.ifloat.src_reg = get_float_register(&at, &is64bit);
                    instruction.ifloat.is64bit = is64bit;
                    instruction.ifloat.addr_mode = FLOAT_ADDR_MODE_REG_TO_REG;
                }
            }
        } break;

        case LVM_CVT_R32_S64:
        case LVM_CVT_R32_S32: {
            bool is64bit = false;
            instruction.binary.dst_reg = get_register(&at, 0);
            eat_whitespace(&at); EAT_COMMA; eat_whitespace(&at);
            instruction.binary.src_reg = get_float_register(&at, &is64bit);
            assert(!is64bit);
        } break;
        case LVM_CVT_U64_R64: 
        case LVM_CVT_U64_R32: 
        case LVM_CVT_S64_R32: 
        case LVM_CVT_S64_R64: 
        case LVM_CVT_S32_R64:  
        case LVM_CVT_S32_R32: {
            bool is64bit = false;
            instruction.binary.dst_reg = get_float_register(&at, &is64bit);
            eat_whitespace(&at); EAT_COMMA; eat_whitespace(&at);
            instruction.binary.src_reg = get_register(&at, 0);
            assert((is64bit && LVM_CVT_S64_R64) || (is64bit && LVM_CVT_S32_R64) || (!is64bit && LVM_CVT_S64_R32) || (!is64bit && LVM_CVT_S32_R32) ||
                (is64bit && LVM_CVT_U64_R64) || (!is64bit && LVM_CVT_U64_R32));
        } break;
        case LVM_CVT_R32_R64: 
        case LVM_CVT_R64_R32: {
            bool lis64bit = false, ris64bit = false;
            instruction.binary.dst_reg = get_float_register(&at, &lis64bit);
            eat_whitespace(&at); EAT_COMMA; eat_whitespace(&at);
            instruction.binary.src_reg = get_float_register(&at, &ris64bit);
            assert((lis64bit && LVM_CVT_R32_R64) || (ris64bit && LVM_CVT_R64_R32));
        } break;
        case LVM_CVT_SEXT: {
            u8 dst_size, src_size;
            instruction.sext.dst_reg = get_register(&at, &dst_size);
            eat_whitespace(&at); EAT_COMMA; eat_whitespace(&at);
            instruction.sext.src_reg = get_register(&at, &src_size);
            instruction.sext.dst_size = dst_size;
            instruction.sext.src_size = src_size;
        } break;

        // Unary instructions
        case LVM_MOVEQ:
        case LVM_MOVNE:
        case LVM_MOVLT_S:
        case LVM_MOVGT_S:
        case LVM_MOVLE_S:
        case LVM_MOVGE_S:
        case LVM_MOVLT_U:
        case LVM_MOVGT_U:
        case LVM_MOVLE_U:
        case LVM_MOVGE_U:
        case LVM_NOT: case LVM_POP: {
            u8 byte_size = 0;
            instruction.unary.reg = get_register(&at, &byte_size);
            instruction.unary.byte_size = byte_size;
        } break;

        case LVM_FMOVEQ:
        case LVM_FMOVNE:
        case LVM_FMOVLT:
        case LVM_FMOVGT:
        case LVM_FMOVLE:
        case LVM_FMOVGE: {
            u8 byte_size = 0;
            instruction.unary.reg = get_register(&at, &byte_size);
            instruction.unary.byte_size = byte_size;
        } break;

        case LVM_FPUSH:
        case LVM_FPOP: {
            bool is64bit = false;
            instruction.push.reg = get_float_register(&at, &is64bit);
            instruction.push.byte_size = (is64bit) ? 8 : 4;
            instruction.push.addr_mode = PUSH_ADDR_MODE_REGISTER;
        } break;

        case LVM_NEG: {
            u8 byte_size = 0;
            instruction.unary.reg = get_register(&at, &byte_size);
            instruction.unary.byte_size = byte_size;
        } break;
        case LVM_FNEG: {
            bool is64bit = false;
            instruction.unary.reg = get_float_register(&at, &is64bit);
            instruction.unary.byte_size = (is64bit) ? 8 : 4;
        } break;

        case LVM_PUSH: case LVM_EXPUSHI: case LVM_EXPUSHF: {
            u8 byte_size = 0;
            if(is_number(*at)) {
                *immediate = parse_number(&at, &instruction.imm_size_bytes);
                instruction.push.addr_mode = PUSH_ADDR_MODE_IMMEDIATE;
                instruction.push.byte_size = instruction.imm_size_bytes;
            } else if(*at == '[') {
                at++;
                if(is_number(*at)) {
                    *immediate = parse_number(&at, &instruction.imm_size_bytes);
                    instruction.imm_size_bytes = 8; // force an address size
                    instruction.push.addr_mode = PUSH_ADDR_MODE_IMMEDIATE_INDIRECT;
                } else {
                    if(type == LVM_EXPUSHF) {
                        bool is64bit = false;
                        instruction.push.reg = get_float_register(&at, &is64bit);
                        byte_size = (is64bit) ? 8 : 4;
                    } else {
                        instruction.push.reg = get_register(&at, &byte_size);
                    }
                    instruction.push.addr_mode = PUSH_ADDR_MODE_REGISTER_INDIRECT;
                    instruction.push.byte_size = byte_size;
                }
                assert(*at == ']');
                at++;
            } else {
                if(type == LVM_EXPUSHF) {
                    bool is64bit = false;
                    instruction.push.reg = get_float_register(&at, &is64bit);
                    byte_size = (is64bit) ? 8 : 4;
                } else {
                    instruction.push.reg = get_register(&at, &byte_size);
                }
                instruction.push.addr_mode = PUSH_ADDR_MODE_REGISTER;
                instruction.push.byte_size = byte_size;
            }
        } break;

        // Comparison/Branch
        case LVM_CALL: case LVM_EXTCALL:
        case LVM_FBEQ: case LVM_FBNE: case LVM_FBGT: case LVM_FBLT:
        case LVM_FBGE: case LVM_FBLE:
        case LVM_BEQ: case LVM_BNE: case LVM_BLT_S:
        case LVM_BGT_S: case LVM_BLE_S: case LVM_BGE_S:
        case LVM_BLT_U: case LVM_BGT_U: case LVM_BLE_U:
        case LVM_BGE_U: case LVM_JMP:{
            if(is_number(*at)) {
                *immediate = parse_number(&at, &instruction.imm_size_bytes);
                instruction.branch.addr_mode = BRANCH_ADDR_MODE_IMMEDIATE_RELATIVE;
                // TODO(psv): always relative for now, figure out a syntax for absolute
            } else if(*at == '[') {
                at++;
                instruction.branch.reg = get_register(&at, 0);
                instruction.branch.addr_mode = BRANCH_ADDR_MODE_REGISTER_INDIRECT;
                assert(*at == ']');
                at++;
            } else {
                instruction.branch.reg = get_register(&at, 0);
                instruction.branch.addr_mode = BRANCH_ADDR_MODE_REGISTER;
            }
        } break;

        case LVM_COPY:{
            u8 dst = get_register(&at, 0);
            EAT_COMMA; eat_whitespace(&at);
            u8 src = get_register(&at, 0);
            EAT_COMMA; eat_whitespace(&at);
            u8 size = get_register(&at, 0);
            instruction.copy.dst_reg = dst;
            instruction.copy.src_reg = src;
            instruction.copy.size_bytes_reg = size;
        } break;

        case LVM_ALLOC: {
            // alloc r0, r1 -> dst, size
            u8 byte_size = 0;
            u8 dst = get_register(&at, 0);
            EAT_COMMA;
            u8 size_reg = get_register(&at, &byte_size);
            instruction.alloc.byte_size = byte_size;
            instruction.alloc.dst_reg = dst;
            instruction.alloc.size_reg = size_reg;
        } break;

        // TODO(psv):
        case LVM_ASSERT: break;

    }

    return instruction;
}

Light_VM_Instruction_Info 
light_vm_push(Light_VM_State* vm_state, const char* instruction) {
    uint64_t imm = 0;
    Light_VM_Instruction i = light_vm_instruction_get(instruction, &imm);
    return light_vm_push_instruction(vm_state, i, imm);
}

Light_VM_Instruction_Info 
light_vm_push_fmt(Light_VM_State* vm_state, const char* instruction, ...) {
    char buffer[256] = {0};
    va_list args;
    va_start(args, instruction);
    vsprintf(buffer, instruction, args);
    va_end(args);

    uint64_t imm = 0;
    Light_VM_Instruction i = light_vm_instruction_get(buffer, &imm);
    return light_vm_push_instruction(vm_state, i, imm);
}

void*
light_vm_push_data_segment(Light_VM_State* vm_state, Light_VM_Data data) {
    void* ptr = ((u8*)vm_state->data.block +vm_state->data_offset);

    switch(data.byte_size) {
        case 1: *(u8*)ptr = data.unsigned_byte; break;
        case 2: *(u16*)ptr = data.unsigned_word; break;
        case 4: *(u32*)ptr = data.unsigned_dword; break;
        case 8: *(u64*)ptr = data.unsigned_qword; break;
        default: assert(0); break;
    }

    vm_state->data_offset += data.byte_size;

    return ptr;
}

void*
light_vm_push_bytes_data_segment(Light_VM_State* vm_state, u8* bytes, s32 byte_count) {
    void* ptr = ((u8*)vm_state->data.block + vm_state->data_offset);
    memcpy(ptr, bytes, byte_count);
    vm_state->data_offset += byte_count;
    return ptr;
}