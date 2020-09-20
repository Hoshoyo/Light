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

/* Print */
void
iri_print_value(FILE* out, IR_Value value)
{
    int bytes = iri_value_byte_size(value);
    switch(value.type) {
        case IR_VALUE_U8:   fprintf(out, "0x%x", value.v_u8); break;
        case IR_VALUE_U16:  fprintf(out, "0x%x", value.v_u16); break;
        case IR_VALUE_U32:  fprintf(out, "0x%x", value.v_u32); break;
        case IR_VALUE_U64:  fprintf(out, "0x%lx", value.v_u64); break;
        case IR_VALUE_S8:   fprintf(out, "%d", value.v_s8); break;
        case IR_VALUE_S16:  fprintf(out, "%d", value.v_s16); break;
        case IR_VALUE_S32:  fprintf(out, "%d", value.v_s32); break;
        case IR_VALUE_S64:  fprintf(out, "%ld", value.v_s64); break;
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
        case -2: fprintf(out, "sp"); break;
        case -1: fprintf(out, "invalid"); break;
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
iri_print_instruction(FILE* out, IR_Instruction* instr)
{
    switch(instr->type)
    {
        case IR_LEA: {
            fprintf(out, "LEA ");
            iri_print_byte_size(out, instr->byte_size);
            if(instr->t1 != -1)
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
            if(instr->t1 != -1)
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
            if(instr->t1 != -1)
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
            if(instr->t1 != -1)
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
            if(instr->t1 != -1)
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
            if(instr->t2 != -1)
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
        default: fprintf(out, "<UNKNOWN>"); break;
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