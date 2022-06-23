#include "../../light_vm/lightvm.h"
#include "../../ast.h"
#include "../../type_table.h"
#include <assert.h>

/*
    ...
    arg2
    arg1
    arg0
    rbp
    ret
    temp0
    temp1
    temp2
*/

#define LVM_REG_RETURN R0
#define PTRSIZE 8
#define Unimplemented assert(0 && "Not implemented")
#define Unreachable assert(0 && "Unreachable")

static Light_VM_Instruction_Info lvm_mov_int_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg);
static Light_VM_Instruction_Info lvm_mov_float_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_FRegisters reg);

static int
align_(uint64_t v)
{
    v = v + (PTRSIZE - (v % PTRSIZE));
    return (int)v;
}
static int
stack_for_arguments(Light_Ast* proc)
{
    int offset = 0;
    for(int i = 0; i < proc->decl_proc.argument_count; ++i)
    {
        Light_Ast_Decl_Variable* var = (Light_Ast_Decl_Variable*)proc->decl_proc.arguments + i;
        assert(((Light_Ast*)var)->kind == AST_DECL_VARIABLE);

        var->stack_index = i;
        var->stack_offset = offset;
        var->stack_argument_offset = offset;
        offset += align_(var->type->size_bits / 8);
    }
    return offset; // size of the stack at the end
}

static Light_VM_Registers
return_reg_for_type(Light_Type* type)
{
    type = type_alias_root(type);
    assert(type->kind == TYPE_KIND_PRIMITIVE);
    if(type_primitive_int(type) || type_primitive_bool(type))
        return R0;
    else if(type_primitive_float(type) && type->size_bits == 32)
        return FR0; // 32 bit
    else
        return FR4; // 64 bit
}

bool
is_regsize_type(Light_Ast* expr)
{
    Light_Type* type = type_alias_root(expr->type);
    switch(type->kind)
    {
        case TYPE_KIND_FUNCTION: 
        case TYPE_KIND_ENUM:
        case TYPE_KIND_POINTER:
        case TYPE_KIND_PRIMITIVE:
            return true;
        case TYPE_KIND_STRUCT:
        case TYPE_KIND_UNION:
            // TODO
        default: 
            return false;
    }
}

static void
lvm_truncate_int_reg(Light_VM_State* state, Light_Type* type, Light_VM_Registers reg)
{
    switch(type->primitive)
    {
        case TYPE_PRIMITIVE_S32:
        case TYPE_PRIMITIVE_U32:
            light_vm_push_fmt(state, "and r%d, 0xffffffff", reg);
            break;
        case TYPE_PRIMITIVE_S16:
        case TYPE_PRIMITIVE_U16:
            light_vm_push_fmt(state, "and r%d, 0xffff", reg);
            break;
        case TYPE_PRIMITIVE_BOOL:
        case TYPE_PRIMITIVE_S8:
        case TYPE_PRIMITIVE_U8:
            light_vm_push_fmt(state, "and r%d, 0xff", reg);
            break;
        default: break;
    }
}

static void
lvm_int_to_int_reg(Light_VM_State* state, Light_Type* type, Light_Type* type_to, Light_VM_Registers reg)
{
    // Upcast/safe, when signed, sign extend
    if(type_to->size_bits > type->size_bits)
    {
        Light_VM_Instruction instr = {
            .type = LVM_CVT_SEXT,
            .sext.dst_reg = reg,
            .sext.src_reg = reg,
            .sext.dst_size = type_to->size_bits / 8,
            .sext.src_size = type->size_bits / 8
        };
        light_vm_push_instruction(state, instr, 0);
    }
    // Downcast/unsafe can lose data
    else if(type_to->size_bits < type->size_bits)
    {
        lvm_truncate_int_reg(state, type_to, reg);
    }
}

static Light_VM_Instruction_Info
lvm_int_cast(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg)
{
    Light_VM_Instruction_Info result = {0};
    Light_Type* operand_type = type_alias_root(expr->expr_unary.operand->type);
    Light_Type* cast_type = type_alias_root(expr->expr_unary.type_to_cast);
    assert(type_primitive_int(cast_type));
    if(operand_type != cast_type)
    {
        switch(cast_type->primitive)
        {
            case TYPE_PRIMITIVE_BOOL:
            case TYPE_PRIMITIVE_U8:
            case TYPE_PRIMITIVE_U16:
            case TYPE_PRIMITIVE_U32:
            case TYPE_PRIMITIVE_U64:
            case TYPE_PRIMITIVE_S8:
            case TYPE_PRIMITIVE_S16:
            case TYPE_PRIMITIVE_S32:
            case TYPE_PRIMITIVE_S64: {                
                if(operand_type->kind == TYPE_KIND_PRIMITIVE)
                {
                    switch(operand_type->primitive)
                    {   
                        case TYPE_PRIMITIVE_BOOL:
                        case TYPE_PRIMITIVE_U64:    // possible data loss
                        case TYPE_PRIMITIVE_U32:
                        case TYPE_PRIMITIVE_U16:
                        case TYPE_PRIMITIVE_U8:                 
                        case TYPE_PRIMITIVE_S64:    // possible data loss
                        case TYPE_PRIMITIVE_S32:
                        case TYPE_PRIMITIVE_S16:
                        case TYPE_PRIMITIVE_S8:
                            lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, reg);
                            lvm_int_to_int_reg(state, operand_type, cast_type, reg);
                            break;
                        case TYPE_PRIMITIVE_R32:{
                            // R32 -> S32/S64
                            lvm_mov_float_expr_to_reg(state, expr->expr_unary.operand, FR0);
                            light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ? 
                                "cvtr32s64 r%d, fr0" : "cvtr32s32 r%d, fr0", reg);
                            lvm_truncate_int_reg(state, cast_type, reg);
                        } break;
                        case TYPE_PRIMITIVE_R64: {
                            // R64 -> S32/S64
                            lvm_mov_float_expr_to_reg(state, expr->expr_unary.operand, FR4);
                            light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ?
                                "cvtr32s64 r%d, fr4" : "cvtr32s32 r%d, fr4", reg);
                            lvm_truncate_int_reg(state, cast_type, reg);
                        } break;
                        default: Unreachable;
                    }
                }
                else
                {
                    // Enum to s32/s64, pointer to s32/s64...
                    Unimplemented;
                }
            }break;
            default: Unimplemented;
        }
    }
    return result;
}

static Light_VM_Instruction_Info
lvm_mov_int_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg)
{
    Light_VM_Instruction_Info result = {0};
    Light_VM_Instruction instr = {0};
    if(expr->type->kind == TYPE_KIND_PRIMITIVE)
    {
        if(expr->kind == AST_EXPRESSION_LITERAL_PRIMITIVE)
        {
            switch(expr->type->primitive)
            {
                case TYPE_PRIMITIVE_BOOL:
                case TYPE_PRIMITIVE_U8:
                case TYPE_PRIMITIVE_S8: {
                    instr.type = LVM_MOV;
                    instr.imm_size_bytes = 1;
                    instr.binary.bytesize = 1;
                    instr.binary.dst_reg = reg;
                    instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                    result = light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u8);                
                } break;
                case TYPE_PRIMITIVE_U16:
                case TYPE_PRIMITIVE_S16: {
                    instr.type = LVM_MOV;
                    instr.imm_size_bytes = 2;
                    instr.binary.bytesize = 2;
                    instr.binary.dst_reg = reg;
                    instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                    result = light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u16);                
                } break;
                case TYPE_PRIMITIVE_U32:
                case TYPE_PRIMITIVE_S32: {
                    instr.type = LVM_MOV;
                    instr.imm_size_bytes = 4;
                    instr.binary.bytesize = 4;
                    instr.binary.dst_reg = reg;
                    instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                    result = light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u32);                
                } break;
                case TYPE_PRIMITIVE_U64:
                case TYPE_PRIMITIVE_S64: {
                    instr.type = LVM_MOV;
                    instr.imm_size_bytes = 8;
                    instr.binary.bytesize = 8;
                    instr.binary.dst_reg = reg;
                    instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                    result = light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u64);                
                } break;
                default: Unreachable; break;
            }
        }
        else if(expr->kind == AST_EXPRESSION_BINARY)
        {
            if(type_primitive_bool(expr->type) && type_primitive_float(expr->expr_binary.left->type))
                return lvm_mov_float_expr_to_reg(state, expr, reg);

            result = lvm_mov_int_expr_to_reg(state, expr->expr_binary.left, R0);
            light_vm_push(state, "push r0");
            lvm_mov_int_expr_to_reg(state, expr->expr_binary.right, R1);
            light_vm_push(state, "pop r0");
            bool signed_ = type_primitive_sint(expr->type);
            switch(expr->expr_binary.op)
            {
                case OP_BINARY_PLUS: {                    
                    light_vm_push(state, type_primitive_sint(expr->type) ? "adds r0, r1" : "addu r0, r1");
                } break;
                case OP_BINARY_MINUS: {
                    light_vm_push(state, type_primitive_sint(expr->type) ? "subs r0, r1" : "subu r0, r1");
                } break;
                case OP_BINARY_MULT: {
                    light_vm_push(state, type_primitive_sint(expr->type) ? "muls r0, r1" : "mulu r0, r1");
                } break;
                case OP_BINARY_DIV: {
                    light_vm_push(state, type_primitive_sint(expr->type) ? "divs r0, r1" : "divu r0, r1");
                } break;
                case OP_BINARY_MOD: {
                    light_vm_push(state, type_primitive_sint(expr->type) ? "mods r0, r1" : "modu r0, r1");
                } break;
                case OP_BINARY_LOGIC_OR:
                case OP_BINARY_OR: {
                    light_vm_push(state, "or r0, r1");
                } break;
                case OP_BINARY_LOGIC_AND:
                case OP_BINARY_AND: {
                    light_vm_push(state, "and r0, r1");
                } break;
                case OP_BINARY_XOR: {
                    light_vm_push(state, "xor r0, r1");
                } break;
                case OP_BINARY_SHL: {
                    light_vm_push(state, "shl r0, r1");
                } break;
                case OP_BINARY_SHR: {
                    light_vm_push(state, "shr r0, r1");
                } break;
                case OP_BINARY_EQUAL: {
                    light_vm_push(state, "cmp r0, r1");
                    light_vm_push(state, "moveq r0");
                } break;
                case OP_BINARY_NOT_EQUAL: {
                    light_vm_push(state, "cmp r0, r1");
                    light_vm_push(state, "movne r0");
                } break;
                case OP_BINARY_LE: {
                    light_vm_push(state, "cmp r0, r1");
                    light_vm_push(state, (signed_) ? "movles r0" : "movleu r0");
                } break;
                case OP_BINARY_GE: {
                    light_vm_push(state, "cmp r0, r1");
                    light_vm_push(state, (signed_) ? "movges r0" : "movgeu r0");
                } break;
                case OP_BINARY_GT: {
                    light_vm_push(state, "cmp r0, r1");
                    light_vm_push(state, (signed_) ? "movgts r0" : "movgtu r0");
                } break;
                case OP_BINARY_LT: {
                    light_vm_push(state, "cmp r0, r1");
                    light_vm_push(state, (signed_) ? "movlts r0" : "movltu r0");
                } break;
                case OP_BINARY_VECTOR_ACCESS: {
                    Unimplemented;
                    //light_vm_push(state, "addu r0, r1");
                } break;
                default: Unimplemented;
            }
            if(reg != R0)            
                light_vm_push_fmt(state, "mov r%d, r0", reg);
        }
        else if(expr->kind == AST_EXPRESSION_VARIABLE)
        {
            Unimplemented;
        }
        else if(expr->kind == AST_EXPRESSION_UNARY)
        {
            switch(expr->expr_unary.op)
            {
                case OP_UNARY_PLUS: break;
                case OP_UNARY_MINUS: {
                    result = lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, R0);
                    light_vm_push(state, "neg r0");
                } break;
                case OP_UNARY_LOGIC_NOT: {
                    result = lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, R0);
                    light_vm_push(state, "cmp r0, 0");
                    light_vm_push(state, "moveq r0");
                } break;
                case OP_UNARY_BITWISE_NOT: {
                    result = lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, R0);
                    light_vm_push(state, "not r0");
                } break;
                case OP_UNARY_CAST: {
                    result = lvm_int_cast(state, expr, reg);
                } break;
                case OP_UNARY_ADDRESSOF: {
                    Unimplemented;
                } break;
                case OP_UNARY_DEREFERENCE: {
                    Unimplemented;
                } break;
                default: Unimplemented;
            }
            if(reg != R0)            
                light_vm_push_fmt(state, "mov r%d, r0", reg);
        }
    }

    return result;
}

static Light_VM_Instruction_Info
lvm_mov_float_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg)
{
    Light_VM_Instruction_Info result = {0};
    Light_VM_Instruction instr = {0};
    if(expr->type->kind == TYPE_KIND_PRIMITIVE)
    {
        if(expr->kind == AST_EXPRESSION_LITERAL_PRIMITIVE)
        {
            switch(expr->type->primitive)
            {
                case TYPE_PRIMITIVE_R32: {
                    instr.type = LVM_FMOV;
                    instr.ifloat.dst_reg = reg;
                    instr.ifloat.src_reg = RDP;
                    instr.imm_size_bytes = 4;
                    instr.ifloat.addr_mode = FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG;
                    uint64_t dataoff = state->data_offset;
                    void* addr = light_vm_push_r32_to_datasegment(state, expr->expr_literal_primitive.value_r32);
                    result = light_vm_push_instruction(state, instr, dataoff);                
                } break;
                case TYPE_PRIMITIVE_R64: {
                    instr.type = LVM_FMOV;
                    instr.ifloat.dst_reg = reg;
                    instr.ifloat.src_reg = RDP;
                    instr.imm_size_bytes = 8;
                    instr.ifloat.addr_mode = FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG;
                    uint64_t dataoff = state->data_offset;
                    void* addr = light_vm_push_r64_to_datasegment(state, expr->expr_literal_primitive.value_r64);
                    result = light_vm_push_instruction(state, instr, dataoff);
                } break;
                default: Unreachable; break;
            }
        }
        else if(expr->kind == AST_EXPRESSION_BINARY)
        {
            Light_VM_FRegisters rleft = (Light_VM_FRegisters)return_reg_for_type(expr->expr_binary.left->type);
            Light_VM_FRegisters rright = rleft + 1;
            lvm_mov_float_expr_to_reg(state, expr->expr_binary.left, rleft);
            light_vm_push_fmt(state, "fpush fr%d", rleft);
            lvm_mov_float_expr_to_reg(state, expr->expr_binary.right, rright);
            light_vm_push_fmt(state, "fpop fr%d", rleft);

            switch(expr->expr_binary.op)
            {
                case OP_BINARY_PLUS: {                    
                    light_vm_push_fmt(state, "fadd fr%d, fr%d", rleft, rright);
                } break;
                case OP_BINARY_MINUS: {
                    light_vm_push_fmt(state, "fsub fr%d, fr%d", rleft, rright);
                } break;
                case OP_BINARY_MULT: {
                    light_vm_push_fmt(state, "fmul fr%d, fr%d", rleft, rright);
                } break;
                case OP_BINARY_DIV: {
                    light_vm_push_fmt(state, "fdiv fr%d, fr%d", rleft, rright);
                } break;
                case OP_BINARY_EQUAL: {
                    light_vm_push_fmt(state, "fcmp fr%d, fr%d", rleft, rright);
                    light_vm_push(state, "fmoveq r0");
                } break;
                case OP_BINARY_NOT_EQUAL: {
                    light_vm_push_fmt(state, "fcmp fr%d, fr%d", rleft, rright);
                    light_vm_push(state, "fmovne r0");
                } break;
                case OP_BINARY_LE: {
                    light_vm_push_fmt(state, "fcmp fr%d, fr%d", rleft, rright);
                    light_vm_push(state, "fmovle r0");
                } break;
                case OP_BINARY_GE: {
                    light_vm_push_fmt(state, "fcmp fr%d, fr%d", rleft, rright);
                    light_vm_push(state, "fmovge r0");
                } break;
                case OP_BINARY_GT: {
                    light_vm_push_fmt(state, "fcmp fr%d, fr%d", rleft, rright);
                    light_vm_push(state, "fmovgt r0");
                } break;
                case OP_BINARY_LT: {
                    light_vm_push_fmt(state, "fcmp fr%d, fr%d", rleft, rright);
                    light_vm_push(state, "fmovlt r0");
                } break;
                case OP_BINARY_VECTOR_ACCESS: {
                    Unimplemented;
                    //light_vm_push(state, "addu r0, r1");
                } break;
                default: Unimplemented;
            }
        }
    }

    return result;
}

void
lvm_generate_comm_return(Light_Ast* comm, Light_VM_State* state)
{
    Light_Ast* expr = comm->comm_return.expression;
    if(expr)
    {
        Light_VM_Instruction_Info info = {0};
        if(type_primitive_int(expr->type) || type_primitive_bool(expr->type))
            info = lvm_mov_int_expr_to_reg(state, expr, return_reg_for_type(expr->type));
        else if(type_primitive_float(expr->type))
            info = lvm_mov_float_expr_to_reg(state, expr, return_reg_for_type(expr->type));
        else
            Unimplemented;            
    }
}

void
lvm_generate_command(Light_Ast* comm, Light_VM_State* state)
{
    switch(comm->kind)
    {
        case AST_COMMAND_RETURN: lvm_generate_comm_return(comm, state); break;
        
        default: Unimplemented;
    }
}

void
lvm_generate_body(Light_Ast* body, Light_VM_State* state)
{
    for(int i = 0; i < body->comm_block.command_count; ++i)
        lvm_generate_command(body->comm_block.commands[i], state);
}

int
lvm_generate_proc_decl(Light_Ast* proc, Light_Scope* global_scope, Light_VM_State* state)
{
    stack_for_arguments(proc);
    
    lvm_generate_body(proc->decl_proc.body, state);

    return 0;
}

int
lvm_generate(Light_Ast** ast, Light_Scope* global_scope)
{
    Light_VM_State* state = light_vm_init();
    lvm_generate_proc_decl(ast[0], global_scope, state);

    light_vm_push(state, "hlt");
    light_vm_debug_dump_code(stdout, state);

    light_vm_execute(state, 0, 0);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL|LVM_PRINT_FLOATING_POINT_REGISTERS);

    return 0;
}
