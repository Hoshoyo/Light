#include "ir.h"
#include <stdio.h>
#include "light_array.h"

/* Load */

int
iri_value_byte_size(IR_Value value)
{
    static int size[IR_VALUE_COUNT] = {
        0, 1, 2, 4, 8,
        1, 2, 4, 8, 4, 8,
    };
    return size[value.type];
}

static IR_Instruction
iri_new(IR_Instruction_Type type, IR_Reg t1, IR_Reg t2, IR_Reg t3, IR_Value imm, int byte_size)
{
    IR_Instruction inst = {0};
    inst.type = type;
    inst.t1 = t1;
    inst.t2 = t2;
    inst.t3 = t3;
    inst.imm = imm;
    inst.byte_size = byte_size;
    return inst;
}

void
iri_emit_cvt(IR_Generator* gen, IR_Instruction_Type type, IR_Reg t1, IR_Reg t2, int16_t src_byte_size, int16_t dst_byte_size)
{
    IR_Instruction inst = iri_new(type, t1, t2, -1, (IR_Value){0}, 0);
    inst.src_byte_size = src_byte_size;
    inst.dst_byte_size = dst_byte_size;
    array_push(gen->instructions, inst);
}

void
iri_emit_lea(IR_Generator* gen, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size)
{
    IR_Instruction inst = iri_new(IR_LEA, t1, t2, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_mov(IR_Generator* gen, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size, bool fp)
{
    IR_Instruction inst = iri_new((fp) ? IR_MOVF : IR_MOV, t1, t2, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

// LOAD t1+imm -> t2
void
iri_emit_load(IR_Generator* gen, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size, bool fp)
{
    IR_Instruction inst = iri_new((fp)? IR_LOADF : IR_LOAD, t1, t2, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

/* Store */

// STORE t1 -> t2+imm
void
iri_emit_store(IR_Generator* gen, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size, bool fp)
{
    IR_Instruction inst = iri_new((fp) ? IR_STOREF : IR_STORE, t1, t2, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

/* Arith */

// ADD t1, t2 -> t3
// ADD t1, imm -> t3
void
iri_emit_arith(IR_Generator* gen, IR_Instruction_Type type, IR_Reg t1, IR_Reg t2, IR_Reg t3, IR_Value imm, int byte_size)
{
    IR_Instruction inst = iri_new(type, t1, t2, t3, imm, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_logic_not(IR_Generator* gen, IR_Reg t1, IR_Reg t2, int byte_size)
{
    IR_Instruction inst = iri_new(IR_LNOT, t1, t2, IR_REG_NONE, (IR_Value){0}, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_not(IR_Generator* gen, IR_Reg t1, IR_Reg t2, int byte_size)
{
    IR_Instruction inst = iri_new(IR_NOT, t1, t2, IR_REG_NONE, (IR_Value){0}, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_neg(IR_Generator* gen, IR_Reg t1, IR_Reg t2, int byte_size, bool fp)
{
    IR_Instruction inst = iri_new((fp) ? IR_NEGF : IR_NEG, t1, t2, IR_REG_NONE, (IR_Value){0}, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_jrz(IR_Generator* gen, IR_Reg t1, IR_Value imm, int byte_size)
{
    IR_Instruction inst = iri_new(IR_JRZ, t1, IR_REG_NONE, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_jrnz(IR_Generator* gen, IR_Reg t1, IR_Value imm, int byte_size)
{
    IR_Instruction inst = iri_new(IR_JRNZ, t1, IR_REG_NONE, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_jr(IR_Generator* gen, IR_Value imm, int byte_size)
{
    IR_Instruction inst = iri_new(IR_JR, IR_REG_NONE, IR_REG_NONE, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_cmp(IR_Generator* gen, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size, bool fp)
{
    IR_Instruction inst = iri_new((fp) ? IR_CMPF : IR_CMP, t1, t2, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_cmov(IR_Generator* gen, IR_Instruction_Type type, IR_Reg t1, IR_Reg t2, IR_Value imm, int byte_size)
{
    IR_Instruction inst = iri_new(type, t1, t2, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_call(IR_Generator* gen, IR_Reg t, IR_Value imm, int byte_size)
{
    IR_Instruction inst = iri_new(IR_CALL, t, IR_REG_NONE, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

void
iri_emit_ret(IR_Generator* gen)
{
    IR_Instruction inst = iri_new(IR_RET, IR_REG_NONE, IR_REG_NONE, IR_REG_NONE, (IR_Value){0}, 0);
    array_push(gen->instructions, inst);
}

void
iri_emit_push(IR_Generator* gen, IR_Reg t, IR_Value imm, int byte_size)
{
    IR_Instruction inst = iri_new(IR_PUSH, t, IR_REG_NONE, IR_REG_NONE, imm, byte_size);
    array_push(gen->instructions, inst);
}

int
iri_current_instr_index(IR_Generator* gen)
{
    return array_length(gen->instructions);
}

IR_Instruction*
iri_get_temp_instr_ptr(IR_Generator* gen, int index)
{
    if(index < array_length(gen->instructions) && index >= 0)
        return gen->instructions + index;
    return 0;
}

/* Print */
void
iri_print_value(FILE* out, IR_Value value)
{
    int bytes = iri_value_byte_size(value);
    switch(value.type) {
        case IR_VALUE_U8:   fprintf(out, "0x%x", value.v_u8); break;
        case IR_VALUE_U16:  fprintf(out, "0x%x", value.v_u16); break;
        case IR_VALUE_U32:  fprintf(out, "0x%x", value.v_u32); break;
        case IR_VALUE_U64:  fprintf(out, "0x" PRINTF_X64, value.v_u64); break;
        case IR_VALUE_S8:   fprintf(out, "%d", value.v_s8); break;
        case IR_VALUE_S16:  fprintf(out, "%d", value.v_s16); break;
        case IR_VALUE_S32:  fprintf(out, "%d", value.v_s32); break;
        case IR_VALUE_S64:  fprintf(out, PRINTF_S64, value.v_s64); break;
        case IR_VALUE_R32:  fprintf(out, "%f", value.v_r32); break;
        case IR_VALUE_R64:  fprintf(out, "%f", value.v_r64); break;
        default: break;
    }
}

void
iri_print_register(FILE* out, IR_Reg reg, bool fp)
{
    switch(reg)
    {
        case IR_REG_PROC_RET:  fprintf(out, "r0"); break;
        case IR_REG_STACK_PTR: fprintf(out, "sp"); break;
        case IR_REG_NONE:      fprintf(out, "invalid"); break;
        default: {
            fprintf(out, (fp) ? "tf%d":"t%d", reg); 
        } break;
    }
}

void
iri_print_byte_size(FILE* out, int byte_size)
{
    switch(byte_size)
    {
        case 1: fprintf(out, "byte "); break;
        case 2: fprintf(out, "word "); break;
        case 4: fprintf(out, "dword "); break;
        case 8: fprintf(out, "qword "); break;
        default: break;
    }
}

static void
iri_print_instr_arith(FILE* out, IR_Instruction* instr)
{
    bool fp = false;
    switch(instr->type)
    {
        case IR_ADDF:   fprintf(out, "ADDF "); fp = true; break;
        case IR_SUBF:   fprintf(out, "SUBF "); fp = true; break;
        case IR_MULF:   fprintf(out, "MULF "); fp = true; break;
        case IR_DIVF:   fprintf(out, "DIVF "); fp = true; break;

        case IR_ADD:    fprintf(out, "ADD "); break;
        case IR_SUB:    fprintf(out, "SUB "); break;
        case IR_AND:    fprintf(out, "AND "); break;
        case IR_MUL:    fprintf(out, "MUL "); break;
        case IR_DIV:    fprintf(out, "DIV "); break;
        case IR_MOD:    fprintf(out, "MOD "); break;
        case IR_OR:     fprintf(out, "OR ");  break;
        case IR_XOR:    fprintf(out, "XOR "); break;
        case IR_SHL:    fprintf(out, "SHL "); break;
        case IR_SHR:    fprintf(out, "SHR "); break;
        case IR_LOR:    fprintf(out, "LOR "); break;
        case IR_LAND:    fprintf(out, "LAND "); break;
        default: break;
    }
    iri_print_byte_size(out, instr->byte_size);
    iri_print_register(out, instr->t1, fp);
    fprintf(out, ", ");
    if(instr->t2 == -1) iri_print_value(out, instr->imm);
    else iri_print_register(out, instr->t2, fp);
    fprintf(out, " -> ");
    iri_print_register(out, instr->t3, fp);
}

void
iri_print_cmov(FILE* out, IR_Instruction* instr)
{
    switch(instr->type)
    {
        case IR_CMOVEQ:    fprintf(out, "CMOVEQ "); break;
        case IR_CMOVNE:    fprintf(out, "CMOVEQ "); break;
        case IR_CMOVGS:    fprintf(out, "CMOVGS "); break;
        case IR_CMOVGES:   fprintf(out, "CMOVGES "); break;
        case IR_CMOVLS:    fprintf(out, "CMOVLS "); break;
        case IR_CMOVLES:   fprintf(out, "CMOVLES "); break;
        case IR_CMOVGU:    fprintf(out, "CMOVGU "); break;
        case IR_CMOVGEU:   fprintf(out, "CMOVGEU "); break;
        case IR_CMOVLU:    fprintf(out, "CMOVLU "); break;
        case IR_CMOVLEU:   fprintf(out, "CMOVLEU "); break;
        default: break;
    }
    if(instr->t1 != IR_REG_NONE)
    {
        iri_print_register(out, instr->t1, false);
        if(instr->imm.type != IR_VALUE_NONE)
            fprintf(out, " + ");
    }
    if(instr->imm.type != IR_VALUE_NONE)
        iri_print_value(out, instr->imm);
    fprintf(out, " -> ");
    iri_print_register(out, instr->t2, false);
}

void
iri_print_cvt(FILE* out, IR_Instruction* instr)
{
    bool t1fp = false;
    bool t2fp = false;
    switch(instr->type)
    {
        case IR_CVT_SI:         fprintf(out, "CVTSI ");    break;
        case IR_CVT_UI:         fprintf(out, "CVTUI ");    break;
        case IR_CVT_SI_R32:     fprintf(out, "CVTSI2SS "); t2fp = true; break;
        case IR_CVT_UI_R32:     fprintf(out, "CVTUI2SS "); t2fp = true; break;
        case IR_CVT_R32_I:      fprintf(out, "CVTSS2SI "); t1fp = true; break;
        case IR_CVT_R32_UI:     fprintf(out, "CVTSS2UI "); t1fp = true; break;
        case IR_CVT_SI_R64:     fprintf(out, "CVTSI2SD "); t2fp = true; break;
        case IR_CVT_UI_R64:     fprintf(out, "CVTUI2SD "); t2fp = true; break;
        case IR_CVT_R64_I:      fprintf(out, "CVTSD2SI "); t1fp = true; break;
        case IR_CVT_R64_UI:     fprintf(out, "CVTSD2UI "); t1fp = true; break;
        case IR_CVT_R32_R64:    fprintf(out, "CVTSS2SD "); t1fp = true; t2fp = true; break;
        case IR_CVT_R64_R32:    fprintf(out, "CVTSD2SS "); t1fp = true; t2fp = true; break;
        default: break;
    }
    iri_print_register(out, instr->t1, t1fp);
    fprintf(out, " -> ");
    iri_print_register(out, instr->t2, t2fp);
}

void
iri_print_instruction(FILE* out, IR_Instruction* instr)
{
    switch(instr->type)
    {
        case IR_LEA: {
            fprintf(out, "LEA ");
            iri_print_byte_size(out, instr->byte_size);
            if(instr->t1 != IR_REG_NONE)
            {
                iri_print_register(out, instr->t1, false);
                if(instr->imm.type != IR_VALUE_NONE)
                    fprintf(out, " + ");
            }
            if(instr->imm.type != IR_VALUE_NONE)
                iri_print_value(out, instr->imm);
            fprintf(out, " -> ");
            iri_print_register(out, instr->t2, false);
        } break;
        case IR_MOVF: {
            fprintf(out, "MOVF ");
            iri_print_byte_size(out, instr->byte_size);
            if(instr->t1 != IR_REG_NONE)
            {
                iri_print_register(out, instr->t1, true);
                if(instr->imm.type != IR_VALUE_NONE)
                    fprintf(out, " + ");
            }
            if(instr->imm.type != IR_VALUE_NONE)
                iri_print_value(out, instr->imm);
            fprintf(out, " -> ");
            iri_print_register(out, instr->t2, true);
        } break;
        case IR_MOV: {
            fprintf(out, "MOV ");
            iri_print_byte_size(out, instr->byte_size);
            if(instr->t1 != IR_REG_NONE)
            {
                iri_print_register(out, instr->t1, false);
                if(instr->imm.type != IR_VALUE_NONE)
                    fprintf(out, " + ");
            }
            if(instr->imm.type != IR_VALUE_NONE)
                iri_print_value(out, instr->imm);
            fprintf(out, " -> ");
            iri_print_register(out, instr->t2, false);
        } break;
        case IR_LOADF: {
            fprintf(out, "LOADF ");
            iri_print_byte_size(out, instr->byte_size);
            if(instr->t1 != IR_REG_NONE)
            {
                iri_print_register(out, instr->t1, false);
                if(instr->imm.type != IR_VALUE_NONE)
                    fprintf(out, " + ");
            }
            if(instr->imm.type != IR_VALUE_NONE)
                iri_print_value(out, instr->imm);
            fprintf(out, " -> ");
            iri_print_register(out, instr->t2, true);
        } break;
        case IR_LOAD: {
            fprintf(out, "LOAD ");
            iri_print_byte_size(out, instr->byte_size);
            if(instr->t1 != IR_REG_NONE)
            {
                iri_print_register(out, instr->t1, false);
                if(instr->imm.type != IR_VALUE_NONE)
                    fprintf(out, " + ");
            }
            if(instr->imm.type != IR_VALUE_NONE)
                iri_print_value(out, instr->imm);
            fprintf(out, " -> ");
            iri_print_register(out, instr->t2, false);
        } break;
        case IR_STORE: {
            fprintf(out, "STORE ");
            iri_print_byte_size(out, instr->byte_size);
            iri_print_register(out, instr->t1, false);
            fprintf(out, " -> ");
            if(instr->t2 != IR_REG_NONE)
            {
                iri_print_register(out, instr->t2, false);
                if(instr->imm.type != IR_VALUE_NONE)
                    fprintf(out, " + ");
            }
            if(instr->imm.type != IR_VALUE_NONE)
                iri_print_value(out, instr->imm);
        } break;
        case IR_STOREF: {
            fprintf(out, "STOREF ");
            iri_print_byte_size(out, instr->byte_size);
            iri_print_register(out, instr->t1, true);
            fprintf(out, " -> ");
            if(instr->t2 != IR_REG_NONE)
            {
                iri_print_register(out, instr->t2, false);
                if(instr->imm.type != IR_VALUE_NONE)
                    fprintf(out, " + ");
            }
            if(instr->imm.type != IR_VALUE_NONE)
                iri_print_value(out, instr->imm);
        } break;
        case IR_SUB: case IR_AND: case IR_MUL:
        case IR_DIV: case IR_MOD: case IR_OR:
        case IR_XOR: case IR_SHL: case IR_SHR:
        case IR_ADDF: case IR_SUBF: case IR_MULF:
        case IR_DIVF:
        case IR_LAND: case IR_LOR:
        case IR_ADD: {
            iri_print_instr_arith(out, instr);
        } break;
        case IR_LNOT: {
            fprintf(out, "LNOT ");
            iri_print_register(out, instr->t1, false);
            fprintf(out, " -> ");
            iri_print_register(out, instr->t2, false);
        } break;
        case IR_NOT: {
            fprintf(out, "NOT ");
            iri_print_register(out, instr->t1, false);
            fprintf(out, " -> ");
            iri_print_register(out, instr->t2, false);
        } break;
        case IR_NEG: {
            fprintf(out, "NEG ");
            iri_print_register(out, instr->t1, false);
            fprintf(out, " -> ");
            iri_print_register(out, instr->t2, false);
        } break;
        case IR_CVT_UI:{
            fprintf(out, "CVTUI ");
            iri_print_byte_size(out, instr->src_byte_size);
            iri_print_register(out, instr->t1, false);
            fprintf(out, " -> ");
            iri_print_byte_size(out, instr->dst_byte_size);
            iri_print_register(out, instr->t2, false);
        } break;
        case IR_CVT_SI: {
            fprintf(out, "CVTSI ");
            iri_print_byte_size(out, instr->src_byte_size);
            iri_print_register(out, instr->t1, false);
            fprintf(out, " -> ");
            iri_print_byte_size(out, instr->dst_byte_size);
            iri_print_register(out, instr->t2, false);
        } break;
        case IR_CMOVEQ: case IR_CMOVNE: case IR_CMOVGS: case IR_CMOVGES:
        case IR_CMOVLS: case IR_CMOVLES: case IR_CMOVGU: 
        case IR_CMOVGEU: case IR_CMOVLU: case IR_CMOVLEU:
            iri_print_cmov(out, instr);
            break;
        case IR_CMP: {
            fprintf(out, "CMP ");
            iri_print_register(out, instr->t1, false);
            fprintf(out, ", ");
            if(instr->t2 != IR_REG_NONE)
                iri_print_register(out, instr->t2, false);
            else
                iri_print_value(out, instr->imm);
        } break;
        case IR_CMPF: {
            fprintf(out, "CMPF ");
            iri_print_register(out, instr->t1, true);
            fprintf(out, ", ");
            if(instr->t2 != IR_REG_NONE)
                iri_print_register(out, instr->t2, false);
            else
                iri_print_value(out, instr->imm);
        } break;

        case IR_JRZ: {
            fprintf(out, "JRZ ");
            iri_print_register(out, instr->t1, false);
            fprintf(out, " -> ");
            iri_print_value(out, instr->imm);
        } break;
        case IR_JRNZ: {
            fprintf(out, "JRNZ ");
            iri_print_register(out, instr->t1, false);
            fprintf(out, " -> ");
            iri_print_value(out, instr->imm);
        } break;
        case IR_JR: {
            fprintf(out, "JR ");
            iri_print_value(out, instr->imm);
        } break;
        case IR_CALL: {
            fprintf(out, "CALL ");
            iri_print_register(out, instr->t1, false);
        } break;
        case IR_PUSH: {
            fprintf(out, "PUSH ");
            if(instr->t1 == IR_REG_NONE)
                iri_print_value(out, instr->imm);
            else
                iri_print_register(out, instr->t1, false);
        } break;
        case IR_RET: {
            fprintf(out, "RET ");
        } break;

        case IR_CVT_SI_R32:
        case IR_CVT_UI_R32:
        case IR_CVT_R32_I:
        case IR_CVT_R32_UI:
        case IR_CVT_SI_R64:
        case IR_CVT_UI_R64:
        case IR_CVT_R64_I:
        case IR_CVT_R64_UI:
        case IR_CVT_R32_R64:
        case IR_CVT_R64_R32:
            iri_print_cvt(out, instr);
            break;
        default: fprintf(out, "<UNKNOWN: %d>", instr->type); break;
    }
}

void
iri_print_instructions(IR_Generator* gen)
{
    for(int i = 0; i < array_length(gen->instructions); ++i)
    {
        IR_Instruction* instr =  gen->instructions + i;
        fprintf(stdout, "%d: ", i);
        iri_print_instruction(stdout, instr);
        fprintf(stdout, "\n");
    }
}