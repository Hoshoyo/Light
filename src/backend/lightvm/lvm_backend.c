#include "../../light_vm/lightvm.h"
#include "../../ast.h"
#include "../../type.h"
#include <assert.h>
#include <light_array.h>

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

static void lvm_mov_int_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg, bool eval_derefs);
static void lvm_mov_float_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_FRegisters reg, bool eval_deref);
static void lvm_mov_ptr_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg, bool eval_derefs);
static void lvm_mov_enum_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg);

typedef enum {
    EXPR_RES_FLOAT32,
    EXPR_RES_FLOAT64,
    EXPR_RES_REGISTER,
} Expr_Result_Type;

typedef struct {
    Expr_Result_Type type;
    union {
        Light_VM_Registers  reg;
        Light_VM_FRegisters freg;
    };
} Expr_Result;

typedef struct {
    Light_VM_Registers base;
    int offset;
} Location;

typedef struct {
    int offset;
    int index;
    int size_bytes;
} Stack_Info;

static Expr_Result lvm_eval(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg, bool eval_derefs);
void lvm_generate_command(Light_Ast* comm, Light_VM_State* state, Stack_Info* stack_info);

static int
align_(uint64_t v)
{
    v = v + (PTRSIZE - (v % PTRSIZE));
    return (int)v;
}
static int
stack_for_arguments(Light_Ast* proc)
{
    int offset = 2 * PTRSIZE;
    for(int i = 0; i < proc->decl_proc.argument_count; ++i)
    {
        Light_Ast* var = proc->decl_proc.arguments[i];
        assert(var->kind == AST_DECL_VARIABLE);

        var->decl_variable.stack_index = i;
        var->decl_variable.stack_offset = offset;
        var->decl_variable.stack_argument_offset = offset;
        offset += align_(var->type->size_bits / 8);
    }
    return offset; // size of the stack at the end
}

static char*
register_size_suffix(uint64_t size)
{
    switch(size)
    {
        case 1: return "b";
        case 2: return "w";
        case 4: return "d";
        default: return "";
    }
}

static Light_VM_Registers
return_reg_for_type(Light_Type* type)
{
    type = type_alias_root(type);
    assert(type->kind == TYPE_KIND_PRIMITIVE || type->kind == TYPE_KIND_POINTER || type->kind == TYPE_KIND_ENUM);
    if(type_primitive_int(type) || type_primitive_bool(type) || type->kind == TYPE_KIND_POINTER || type->kind == TYPE_KIND_ENUM)
        return R0;
    else if(type_primitive_float(type) && type->size_bits == 32)
        return FR0; // 32 bit
    else if(type_primitive_float(type) && type->size_bits == 64)
        return FR4; // 64 bit
    else
    {
        Unimplemented;
        return R0;
    }
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
lvm_gen_copy(Light_VM_State* state, Expr_Result rvalue, Location location, uint64_t size_bytes)
{
    if(rvalue.type == EXPR_RES_REGISTER)
    {
        if(size_bytes <= 8)
        {
            if(location.offset < 0)
                light_vm_push_fmt(state, "mov [r%d %s %d], r%d%s", 
                    location.base, "-", -location.offset, rvalue.reg, register_size_suffix(size_bytes));
            else
                light_vm_push_fmt(state, "mov [r%d %s %d], r%d%s", 
                    location.base, "+", location.offset, rvalue.reg, register_size_suffix(size_bytes));
        }
        else
            Unimplemented;
    }
    else if(rvalue.type == EXPR_RES_FLOAT32)
    {
        if(location.offset < 0)
            light_vm_push_fmt(state, "fmov [r%d %s %d], fr%d", 
                location.base, "-", -location.offset, rvalue.freg);
        else
            light_vm_push_fmt(state, "fmov [r%d %s %d], fr%d", 
                location.base, "+", location.offset, rvalue.freg);
    }
    else
    {
        Unimplemented;
    }
}

static void
lvm_gen_deref(Light_VM_State* state, Expr_Result rvalue, Location location, uint64_t size_bytes)
{
    if(rvalue.type == EXPR_RES_REGISTER)
    {
        if(size_bytes <= 8)
        {
            if(location.offset < 0)
                light_vm_push_fmt(state, "mov r%d%s, [r%d %d]", 
                    rvalue.reg, register_size_suffix(size_bytes), location.base, "-", -location.offset);
            else
                light_vm_push_fmt(state, "mov r%d%s, [r%d %s %d]", 
                    rvalue.reg, register_size_suffix(size_bytes), location.base, "+", location.offset);

        }
        else
            Unimplemented;
    }
    else
    {
        Unimplemented;
    }
}

static void
lvm_push_result(Light_VM_State* state, Expr_Result rvalue)
{
    if(rvalue.type == EXPR_RES_REGISTER)
        light_vm_push_fmt(state, "push r%d", rvalue.reg);
    else
        light_vm_push_fmt(state, "fpush fr%d", rvalue.freg);
}

static void
lvm_pop_result(Light_VM_State* state, Expr_Result rvalue)
{
    if(rvalue.type == EXPR_RES_REGISTER)
        light_vm_push_fmt(state, "pop r%d", rvalue.reg);
    else
        light_vm_push_fmt(state, "fpop fr%d", rvalue.freg);
}

static bool
lvm_result_equal(Expr_Result left, Expr_Result right)
{
    if (left.type == right.type)
    {
        if(left.type == EXPR_RES_REGISTER)
            return left.reg == right.reg;
        else
            return left.freg == right.freg;
    }
    return false;
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

static void
lvm_int_cast(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg)
{
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
                            lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, reg, true);
                            lvm_int_to_int_reg(state, operand_type, cast_type, reg);
                            break;
                        case TYPE_PRIMITIVE_R32:{
                            // R32 -> S32/S64
                            lvm_mov_float_expr_to_reg(state, expr->expr_unary.operand, FR0, true);
                            light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ? 
                                "cvtr32s64 r%d, fr0" : "cvtr32s32 r%d, fr0", reg);
                            lvm_truncate_int_reg(state, cast_type, reg);
                        } break;
                        case TYPE_PRIMITIVE_R64: {
                            // R64 -> S32/S64
                            lvm_mov_float_expr_to_reg(state, expr->expr_unary.operand, FR4, true);
                            light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ?
                                "cvtr32s64 r%d, fr4" : "cvtr32s32 r%d, fr4", reg);
                            lvm_truncate_int_reg(state, cast_type, reg);
                        } break;
                        default: Unreachable;
                    }
                }
                else if(operand_type->kind == TYPE_KIND_ENUM)
                {
                    lvm_mov_enum_expr_to_reg(state, expr->expr_unary.operand, reg);
                }
                else
                {
                    // pointer to s32/s64...
                    Unimplemented;
                }
            }break;
            default: Unreachable;
        }
    }
}

static uint32_t
size_mask(int size_bits)
{
    switch(size_bits)
    {
        case 8:  return 0xff;
        case 16: return 0xffff;
        case 32: return 0xffffffff;
        case 64: return 0xffffffffffffffff;
        default: return 0;
    }
}

static void
lvm_float_cast(Light_VM_State* state, Light_Ast* expr, Light_VM_FRegisters freg)
{
    Light_Type* operand_type = type_alias_root(expr->expr_unary.operand->type);
    Light_Type* cast_type = type_alias_root(expr->expr_unary.type_to_cast);
    assert(type_primitive_float(cast_type));
    if(operand_type != cast_type)
    {
        assert(operand_type->kind == TYPE_KIND_PRIMITIVE);
        switch(operand_type->primitive)
        {
            case TYPE_PRIMITIVE_U64:
                // TODO(psv): need to create CVTU64R32
                Unimplemented;
                break;
            case TYPE_PRIMITIVE_U32:
            case TYPE_PRIMITIVE_U16:
            case TYPE_PRIMITIVE_U8: {
                lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, R0, true);
                light_vm_push_fmt(state, "and r0, 0x%x", size_mask(operand_type->size_bits));
                light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_R32) ? "cvts64r32 fr%d, r0" : "cvts64r64 fr%d, r0", freg);
            } break;              
            case TYPE_PRIMITIVE_S64:
            case TYPE_PRIMITIVE_S32:
            case TYPE_PRIMITIVE_S16:
            case TYPE_PRIMITIVE_S8: {
                lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, R0, true);
                if((cast_type->primitive == TYPE_PRIMITIVE_R32))
                {
                    light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ? 
                        "cvts64r32 fr%d, r%d" : "cvts32r32 fr%d, r0", freg);
                }
                else
                {
                    light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ? 
                        "cvts64r64 fr%d, r%d" : "cvts32r64 fr%d, r0", freg);
                }
            } break;
            case TYPE_PRIMITIVE_R32: {
                if(cast_type->primitive == TYPE_PRIMITIVE_R64)
                {
                    lvm_mov_float_expr_to_reg(state, expr->expr_unary.operand, FR0, true);
                    light_vm_push_fmt(state, "cvtr32r64 fr%d, fr0", freg);
                }
            } break;
            case TYPE_PRIMITIVE_R64: {
                if(cast_type->primitive == TYPE_PRIMITIVE_R32)
                {
                    lvm_mov_float_expr_to_reg(state, expr->expr_unary.operand, FR4, true);
                    light_vm_push_fmt(state, "cvtr64r32 fr%d, fr4", freg);
                }
            } break;
            default: Unreachable;
        }
    }
}

static int
enum_value_index(Light_Ast* expr)
{
    Light_Type* type = type_alias_root(expr->type);
    if(expr->kind == AST_EXPRESSION_DOT)
    {
        for (int i = 0; i < array_length(type->enumerator.fields); ++i)
        {
            if(type->enumerator.fields[i]->decl_variable.name->data == expr->expr_dot.identifier->data)
            {
                return i;
            }
        }
    }
    Unreachable;
    return -1;
}

static void
lvm_mov_enum_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg)
{
    Light_Type* type = type_alias_root(expr->type);
    assert(type->kind == TYPE_KIND_ENUM);
    if(expr->kind == AST_EXPRESSION_DOT)
    {
        int index = enum_value_index(expr);
        int64_t value = type->enumerator.evaluated_values[index];
        light_vm_push_fmt(state, "mov r%d, %lld", reg, value);
    }
    else if(expr->kind == AST_EXPRESSION_VARIABLE)
    {
        lvm_mov_int_expr_to_reg(state, expr, reg, true);
    }
}

static void
lvm_eval_variable(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg)
{
    light_vm_push_fmt(state, "mov r%d, rbp", reg);
    if(expr->expr_variable.decl->decl_variable.stack_offset < 0)
        light_vm_push_fmt(state, "subs r%d, %d", reg, -expr->expr_variable.decl->decl_variable.stack_offset);
    else
        light_vm_push_fmt(state, "adds r%d, %d", reg, expr->expr_variable.decl->decl_variable.stack_offset);
}

// Given an integer register and a dot field accessing expression of a struct or union
// retreives the address of that struct's data in memory in the register.
// Performs dereferencing in case the left side of the expression is a pointer type.
// Example: value.x
static void
lvm_eval_field_access(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg)
{
    Light_Type* left_type = type_alias_root(expr->expr_dot.left->type);
    Expr_Result res = lvm_eval(state, expr->expr_dot.left, reg, left_type->kind == TYPE_KIND_POINTER);

    if(left_type->kind == TYPE_KIND_POINTER)
    {
        if(type_alias_root(left_type->pointer_to)->kind == TYPE_KIND_STRUCT)
        {
            s64 offset = type_struct_field_offset_bits(left_type->pointer_to, expr->expr_dot.identifier->data);
            light_vm_push_fmt(state, "adds r%d, %d", reg, (s32)offset);
        }
        else
        {
            // Offset of an union is always 0, therefore no need to add anything
            assert(type_alias_root(left_type->pointer_to)->kind == TYPE_KIND_UNION);
        }
    }
    else if(left_type->kind == TYPE_KIND_STRUCT)
    {
        s64 offset = type_struct_field_offset_bits(expr->expr_dot.left->type, expr->expr_dot.identifier->data);
        light_vm_push_fmt(state, "adds r%d, %d", reg, (s32)offset);
    }
    else
    {
        // Offset of an union is always 0, therefore no need to add anything
        assert(left_type->kind == TYPE_KIND_UNION);
    }
}

static void
lvm_eval_vector_access(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg);

static Expr_Result
lvm_eval(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg, bool eval_derefs)
{
    Expr_Result result = { 
        .reg = reg,
        .type = EXPR_RES_REGISTER,
    };
        
    if (type_primitive_bool(expr->type))
    {
        if(expr->kind == AST_EXPRESSION_BINARY && type_primitive_float(expr->expr_binary.left->type))
            lvm_mov_float_expr_to_reg(state, expr, result.reg, eval_derefs);
        else
            lvm_mov_int_expr_to_reg(state, expr, result.reg, eval_derefs);
    }
    else if(type_primitive_int(expr->type))
    {
        lvm_mov_int_expr_to_reg(state, expr, result.reg, eval_derefs);
    }
    else if(type_primitive_float(expr->type))
    {
        lvm_mov_float_expr_to_reg(state, expr, result.reg, eval_derefs);
        if(expr->flags & AST_FLAG_EXPRESSION_LVALUE && !eval_derefs)
            result.type = EXPR_RES_REGISTER;
        else
            result.type = (expr->type->size_bits == 32) ? EXPR_RES_FLOAT32 : EXPR_RES_FLOAT64;
    }
    else if(expr->type->kind == TYPE_KIND_POINTER)
    {
        lvm_mov_ptr_expr_to_reg(state, expr, result.reg, eval_derefs);
    }
    else if(type_alias_root(expr->type)->kind == TYPE_KIND_ENUM)
    {
        lvm_mov_enum_expr_to_reg(state, expr, result.reg);
    }
    else if(expr->kind == AST_EXPRESSION_VARIABLE)
    {
        lvm_eval_variable(state, expr, result.reg);
    }
    else if(expr->kind == AST_EXPRESSION_DOT)
    {
        lvm_eval_field_access(state, expr, result.reg);
    }
    else if(expr->kind == AST_EXPRESSION_BINARY && expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS)
    {
        lvm_eval_vector_access(state, expr, result.reg);
    }
    else
    {
        Unimplemented; 
    }
    return result;
}
static void
lvm_mov_arr_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg);

static void
lvm_eval_vector_access(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg)
{
    assert(expr->kind == AST_EXPRESSION_BINARY);
    assert(expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS);
    if(expr->expr_binary.left->type->kind == TYPE_KIND_POINTER)
        lvm_mov_ptr_expr_to_reg(state, expr->expr_binary.left, R0, true);
    else
        lvm_mov_arr_expr_to_reg(state, expr->expr_binary.left, R0);
    light_vm_push(state, "push r0");                
    lvm_mov_int_expr_to_reg(state, expr->expr_binary.right, R1, true);
    light_vm_push(state, "pop r0");
    if(expr->type->size_bits > 8)
    {
        light_vm_push(state, "xor r2, r2");
        light_vm_push_fmt(state, "mov r2, %d", expr->type->size_bits / 8);
        light_vm_push(state, "mulu r1, r2");
    }
    light_vm_push(state, "addu r0, r1");
    if(reg != R0)
        light_vm_push_fmt(state, "mov r%d, r0", reg);
}

static void
lvm_mov_arr_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg)
{
    assert(expr->type->kind == TYPE_KIND_ARRAY);

    if(expr->kind == AST_EXPRESSION_LITERAL_ARRAY)
    {
        Unimplemented;
    }
    else if(expr->kind == AST_EXPRESSION_BINARY)
    {
        // Multidimensional array
        lvm_eval_vector_access(state, expr, reg);
    }
    
    else if(expr->kind == AST_EXPRESSION_VARIABLE)
    {
        light_vm_push_fmt(state, "mov r%d, rbp", reg);
        if (expr->expr_variable.decl->decl_variable.stack_offset < 0)
            light_vm_push_fmt(state, "subs r%d, %d", reg, -expr->expr_variable.decl->decl_variable.stack_offset);
        else
            light_vm_push_fmt(state, "adds r%d, %d", reg, expr->expr_variable.decl->decl_variable.stack_offset);
    }
    else if(expr->kind == AST_EXPRESSION_DOT)
    {
        lvm_eval_field_access(state, expr, reg);
    }
    else if(expr->kind == AST_EXPRESSION_PROCEDURE_CALL)
    {
        Unimplemented;
    }
    else if(expr->kind == AST_EXPRESSION_UNARY)
    {
        // TODO(psv): verify if this is really unreachable
        Unreachable;
    }
    else
    {
        Unimplemented;
    }
}

static void
lvm_eval_proc_call(Light_VM_State* state, Light_Ast* expr)
{
    assert(expr->kind == AST_EXPRESSION_PROCEDURE_CALL);

    // TODO(psv): arguments
    
    Light_Ast* caller = expr->expr_proc_call.caller_expr;
    if(caller->kind == AST_EXPRESSION_VARIABLE && caller->expr_variable.decl->kind == AST_DECL_PROCEDURE)
    {
        Light_VM_Instruction_Info call_instr = light_vm_push(state, "call 0xffffffff");
        Patch_Procs pp = {
            .from = call_instr,
            .to_decl = caller->expr_variable.decl
        };
        array_push(state->proc_patch_calls, pp);
    }
    else
    {
        lvm_eval(state, expr->expr_proc_call.caller_expr, R0, true);
        light_vm_push(state, "call r0");
    }
}

static void
lvm_mov_ptr_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg, bool eval_derefs)
{
    assert(expr->type->kind == TYPE_KIND_POINTER);

    if(expr->kind == AST_EXPRESSION_LITERAL_PRIMITIVE)
    {
        assert(expr->expr_literal_primitive.type == LITERAL_POINTER);
        // the only literal pointer is null
        light_vm_push_fmt(state, "mov r%d, 0", reg);
    }
    else if(expr->kind == AST_EXPRESSION_BINARY)
    {
        // Pointer arithmetic
        Unimplemented;
    }
    else if(expr->kind == AST_EXPRESSION_UNARY)
    {
        switch(expr->expr_unary.op)
        {
            case OP_UNARY_CAST: {
                Unimplemented;
            } break;
            case OP_UNARY_ADDRESSOF: {
                lvm_eval(state, expr->expr_unary.operand, R0, false);
            } break;
            case OP_UNARY_DEREFERENCE: {
                Expr_Result res = lvm_eval(state, expr->expr_unary.operand, reg, false);
                if(eval_derefs)
                    lvm_gen_deref(state, res, (Location){.base = reg, .offset = 0}, expr->type->size_bits / 8);
            } break;
            default: Unreachable;
        }
        if(reg != R0)            
            light_vm_push_fmt(state, "mov r%d, r0", reg);
    }
    else if(expr->kind == AST_EXPRESSION_VARIABLE)
    {
        if(eval_derefs)
        {
            Light_VM_Instruction instr = {
                .type = LVM_MOV,
                .imm_size_bytes = 4,
                .binary.bytesize = expr->type->size_bits / 8,
                .binary.addr_mode = BIN_ADDR_MODE_REG_OFFSETED_TO_REG,
                .binary.dst_reg = reg,
                .binary.src_reg = RBP,
            };
            light_vm_push_instruction(state, instr, expr->expr_variable.decl->decl_variable.stack_offset);
        }
        else
        {
            light_vm_push_fmt(state, "mov r%d, rbp", reg);
            if (expr->expr_variable.decl->decl_variable.stack_offset < 0)
                light_vm_push_fmt(state, "subs r%d, %d", reg, -expr->expr_variable.decl->decl_variable.stack_offset);
            else
                light_vm_push_fmt(state, "adds r%d, %d", reg, expr->expr_variable.decl->decl_variable.stack_offset);
        }
    }
    else if(expr->kind == AST_EXPRESSION_DOT)
    {
        Unimplemented;
    }
    else if(expr->kind == AST_EXPRESSION_PROCEDURE_CALL)
    {
        Unimplemented;
    }
    else
    {
        Unimplemented;
    }
}

static void
lvm_mov_int_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_Registers reg, bool eval_derefs)
{
    assert(type_alias_root(expr->type)->kind == TYPE_KIND_PRIMITIVE || type_alias_root(expr->type)->kind == TYPE_KIND_ENUM);

    {
        Light_VM_Instruction instr = {0};
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
                    light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u8);                
                } break;
                case TYPE_PRIMITIVE_U16:
                case TYPE_PRIMITIVE_S16: {
                    instr.type = LVM_MOV;
                    instr.imm_size_bytes = 2;
                    instr.binary.bytesize = 2;
                    instr.binary.dst_reg = reg;
                    instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                    light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u16);                
                } break;
                case TYPE_PRIMITIVE_U32:
                case TYPE_PRIMITIVE_S32: {
                    instr.type = LVM_MOV;
                    instr.imm_size_bytes = 4;
                    instr.binary.bytesize = 4;
                    instr.binary.dst_reg = reg;
                    instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                    light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u32);                
                } break;
                case TYPE_PRIMITIVE_U64:
                case TYPE_PRIMITIVE_S64: {
                    instr.type = LVM_MOV;
                    instr.imm_size_bytes = 8;
                    instr.binary.bytesize = 8;
                    instr.binary.dst_reg = reg;
                    instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                    light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u64);                
                } break;
                default: Unreachable; break;
            }
        }
        else if(expr->kind == AST_EXPRESSION_BINARY)
        {
            if(expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS)
            {
                lvm_eval_vector_access(state, expr, reg); 
                if(eval_derefs)
                    light_vm_push_fmt(state, "mov r%d, [r%d]", reg, reg);
            }
            else if(state->short_circuit && (expr->expr_binary.op == OP_BINARY_LOGIC_OR || expr->expr_binary.op == OP_BINARY_LOGIC_AND))
            {
                if(expr->expr_binary.op == OP_BINARY_LOGIC_OR)
                {
                    int prev_false = state->short_circuit_current_false--;
                    int this_false = state->short_circuit_current_false;

                    lvm_mov_int_expr_to_reg(state, expr->expr_binary.left, R0, true);
                    if(expr->expr_binary.left->kind != AST_EXPRESSION_BINARY || 
                        (expr->expr_binary.left->expr_binary.op != OP_BINARY_LOGIC_AND && expr->expr_binary.left->expr_binary.op != OP_BINARY_LOGIC_OR))
                    {
                        light_vm_push(state, "cmp r0b, 0");
                        Light_VM_Instruction_Info info = light_vm_push(state, "bne 0xffffffff");
                        info.short_circuit_index = state->short_circuit_current_true;
                        array_push(state->short_circuit_jmps, info);
                    }
                    for(int i = 0; i < array_length(state->short_circuit_jmps); ++i)
                        if(state->short_circuit_jmps[i].short_circuit_index == this_false)
                            light_vm_patch_from_to_current_instruction(state, state->short_circuit_jmps[i]);
                    state->short_circuit_current_false = prev_false;

                    lvm_mov_int_expr_to_reg(state, expr->expr_binary.right, R0, true);
                    if(expr->expr_binary.left->kind != AST_EXPRESSION_BINARY || 
                        (expr->expr_binary.left->expr_binary.op != OP_BINARY_LOGIC_AND && expr->expr_binary.left->expr_binary.op != OP_BINARY_LOGIC_OR))
                    {
                        light_vm_push(state, "cmp r0b, 0");
                        Light_VM_Instruction_Info info = light_vm_push(state, "beq 0xffffffff");
                        info.short_circuit_index = state->short_circuit_current_false;
                        array_push(state->short_circuit_jmps, info);
                    }
                }
                else
                {
                    int prev_true = state->short_circuit_current_true++;
                    int this_true = state->short_circuit_current_true;

                    lvm_mov_int_expr_to_reg(state, expr->expr_binary.left, R0, true);
                    if(expr->expr_binary.left->kind != AST_EXPRESSION_BINARY || 
                        (expr->expr_binary.left->expr_binary.op != OP_BINARY_LOGIC_AND && expr->expr_binary.left->expr_binary.op != OP_BINARY_LOGIC_OR))
                    {
                        light_vm_push(state, "cmp r0b, 0");
                        Light_VM_Instruction_Info info = light_vm_push(state, "beq 0xffffffff");
                        info.short_circuit_index = state->short_circuit_current_false;
                        array_push(state->short_circuit_jmps, info);
                    }
                    for(int i = 0; i < array_length(state->short_circuit_jmps); ++i)
                        if(state->short_circuit_jmps[i].short_circuit_index == this_true)
                            light_vm_patch_from_to_current_instruction(state, state->short_circuit_jmps[i]);
                    state->short_circuit_current_true = prev_true;

                    lvm_mov_int_expr_to_reg(state, expr->expr_binary.right, R0, true);
                    if(expr->expr_binary.left->kind != AST_EXPRESSION_BINARY || 
                        (expr->expr_binary.left->expr_binary.op != OP_BINARY_LOGIC_AND && expr->expr_binary.left->expr_binary.op != OP_BINARY_LOGIC_OR))
                    {
                        light_vm_push(state, "cmp r0b, 0");
                        Light_VM_Instruction_Info info = light_vm_push(state, "beq 0xffffffff");
                        info.short_circuit_index = state->short_circuit_current_false;
                        array_push(state->short_circuit_jmps, info);
                    }
                }
            }
            else
            {
                lvm_mov_int_expr_to_reg(state, expr->expr_binary.left, R0, true);
                light_vm_push(state, "push r0");
                lvm_mov_int_expr_to_reg(state, expr->expr_binary.right, R1, true);
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
                    default: Unreachable;
                }
                if(reg != R0)            
                    light_vm_push_fmt(state, "mov r%d, r0", reg);
            }
        }
        else if(expr->kind == AST_EXPRESSION_UNARY)
        {
            switch(expr->expr_unary.op)
            {
                case OP_UNARY_PLUS: break;
                case OP_UNARY_MINUS: {
                    lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, R0, true);
                    light_vm_push(state, "neg r0");
                    if(reg != R0)            
                        light_vm_push_fmt(state, "mov r%d, r0", reg);
                } break;
                case OP_UNARY_LOGIC_NOT: {
                    lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, R0, true);
                    light_vm_push(state, "cmp r0, 0");
                    light_vm_push(state, "moveq r0");
                    if(reg != R0)            
                        light_vm_push_fmt(state, "mov r%d, r0", reg);
                } break;
                case OP_UNARY_BITWISE_NOT: {
                    lvm_mov_int_expr_to_reg(state, expr->expr_unary.operand, R0, true);
                    light_vm_push(state, "not r0");
                    if(reg != R0)            
                        light_vm_push_fmt(state, "mov r%d, r0", reg);
                } break;
                case OP_UNARY_CAST: {
                    lvm_int_cast(state, expr, reg);
                } break;
                case OP_UNARY_DEREFERENCE: {
                    Expr_Result res = lvm_eval(state, expr->expr_unary.operand, reg, false);
                    if(eval_derefs || type_alias_root(expr->expr_unary.operand->type)->kind == TYPE_KIND_POINTER)
                        lvm_gen_deref(state, res, (Location){.base = reg, .offset = 0}, expr->type->size_bits / 8);
                } break;
                default: Unreachable;
            }
        }
        else if(expr->kind == AST_EXPRESSION_VARIABLE)
        {
            if(eval_derefs)
            {
                Light_VM_Instruction instr = {
                    .type = LVM_MOV,
                    .imm_size_bytes = 4,
                    .binary.bytesize = expr->type->size_bits / 8,
                    .binary.addr_mode = BIN_ADDR_MODE_REG_OFFSETED_TO_REG,
                    .binary.dst_reg = reg,
                    .binary.src_reg = RBP,
                };
                light_vm_push_instruction(state, instr, expr->expr_variable.decl->decl_variable.stack_offset);
            }
            else
            {
                light_vm_push_fmt(state, "mov r%d, rbp", reg);
                if(expr->expr_variable.decl->decl_variable.stack_offset < 0)
                    light_vm_push_fmt(state, "subs r%d, %d", reg, -expr->expr_variable.decl->decl_variable.stack_offset);
                else
                    light_vm_push_fmt(state, "adds r%d, %d", reg, expr->expr_variable.decl->decl_variable.stack_offset);
            }
        }
        else if(expr->kind == AST_EXPRESSION_DOT)
        {            
            lvm_eval_field_access(state, expr, reg);
            if(eval_derefs)
                light_vm_push_fmt(state, "mov r%d, [r%d]", reg, reg);
        }
        else if(expr->kind == AST_EXPRESSION_PROCEDURE_CALL)
        {
            lvm_eval_proc_call(state, expr);
            if(reg != R0)
                light_vm_push_fmt(state, "mov r%d, r0", reg);
        }
    }
}

static void
lvm_mov_float_expr_to_reg(Light_VM_State* state, Light_Ast* expr, Light_VM_FRegisters reg, bool eval_derefs)
{
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
                    light_vm_push_instruction(state, instr, dataoff);                
                } break;
                case TYPE_PRIMITIVE_R64: {
                    instr.type = LVM_FMOV;
                    instr.ifloat.dst_reg = reg;
                    instr.ifloat.src_reg = RDP;
                    instr.imm_size_bytes = 8;
                    instr.ifloat.addr_mode = FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG;
                    uint64_t dataoff = state->data_offset;
                    void* addr = light_vm_push_r64_to_datasegment(state, expr->expr_literal_primitive.value_r64);
                    light_vm_push_instruction(state, instr, dataoff);
                } break;
                default: Unreachable; break;
            }
        }
        else if(expr->kind == AST_EXPRESSION_BINARY)
        {
            if(expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS)
            {
                lvm_eval_vector_access(state, expr, reg); 
                if(eval_derefs)
                    light_vm_push_fmt(state, "fmov fr%d, [r%d]", reg, reg);
            }
            else
            {
                Light_VM_FRegisters rleft = (Light_VM_FRegisters)return_reg_for_type(expr->expr_binary.left->type);
                Light_VM_FRegisters rright = rleft + 1;
                lvm_mov_float_expr_to_reg(state, expr->expr_binary.left, rleft, true);
                light_vm_push_fmt(state, "fpush fr%d", rleft);
                lvm_mov_float_expr_to_reg(state, expr->expr_binary.right, rright, true);
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
                    default: Unreachable;
                }
                if (type_primitive_bool(expr->type) && reg != R0)
                    light_vm_push_fmt(state, "mov r%d, r0", reg);
                else if (reg != rleft)
                    light_vm_push_fmt(state, "fmov fr%d, fr%d", reg, rleft);
            }
        }
        else if(expr->kind == AST_EXPRESSION_UNARY)
        {
            switch(expr->expr_unary.op)
            {
                case OP_UNARY_PLUS: break;
                case OP_UNARY_MINUS: {
                    lvm_mov_float_expr_to_reg(state, expr->expr_unary.operand, reg, true);
                    light_vm_push_fmt(state, "neg fr%d", reg);
                } break;
                case OP_UNARY_CAST: {
                    lvm_float_cast(state, expr, reg);
                } break;
                case OP_UNARY_DEREFERENCE: {
                    Expr_Result res = lvm_eval(state, expr->expr_unary.operand, reg, false);
                    if(eval_derefs)
                        lvm_gen_deref(state, res, (Location){.base = reg, .offset = 0}, expr->type->size_bits / 8);
                } break;
                default: Unreachable;
            }
        }
        else if(expr->kind == AST_EXPRESSION_VARIABLE)
        {
            if(eval_derefs)
            {
                Light_VM_Instruction instr = {
                    .type = LVM_FMOV,
                    .imm_size_bytes = 4,
                    .ifloat.addr_mode = FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG,
                    .ifloat.dst_reg = reg,
                    .ifloat.src_reg = RBP,
                };
                light_vm_push_instruction(state, instr, expr->expr_variable.decl->decl_variable.stack_offset);
            }
            else
            {
                light_vm_push_fmt(state, "mov r%d, rbp", reg);
                light_vm_push_fmt(state, "adds r%d, %d", reg, expr->expr_variable.decl->decl_variable.stack_offset);
            }
        }
        else if(expr->kind == AST_EXPRESSION_DOT)
        {            
            lvm_eval_field_access(state, expr, reg);
            if(eval_derefs)
                light_vm_push_fmt(state, "fmov fr%d, [r%d]", reg, reg);
        }
        else if(expr->kind == AST_EXPRESSION_PROCEDURE_CALL)
        {
            Unimplemented;
        }
    }
}

/* ---------------------------------------------------------------------*/
/* ------------------------ Commands -----------------------------------*/
/* ---------------------------------------------------------------------*/

void
lvm_generate_comm_return(Light_Ast* comm, Light_VM_State* state, Stack_Info* stack_info)
{
    Light_Ast* expr = comm->comm_return.expression;
    if(expr)
        lvm_eval(state, expr, return_reg_for_type(expr->type), true);

    if(stack_info->size_bytes > 0)
        light_vm_push_fmt(state, "adds rsp, %d", stack_info->size_bytes);
    light_vm_push(state, "pop rbp");
    light_vm_push(state, "ret");
}

void
lvm_generate_decl_variable(Light_Ast* comm, Light_VM_State* state, Stack_Info* stack_info)
{
    if(!(comm->decl_variable.flags & DECL_VARIABLE_FLAG_ADDR_CALCULATED))
    {
        comm->decl_variable.stack_index = stack_info->index++;
        comm->decl_variable.stack_offset = stack_info->offset;
        comm->decl_variable.flags |= DECL_VARIABLE_FLAG_ADDR_CALCULATED;
        stack_info->offset -= align_(comm->decl_variable.type->size_bits / 8);
        stack_info->size_bytes += align_(comm->decl_variable.type->size_bits / 8);

        Light_VM_Debug_Variable dbg_var = (Light_VM_Debug_Variable){
            .name = comm->decl_variable.name->data, 
            .name_length = comm->decl_variable.name->length,
            .rbp_offset = comm->decl_variable.stack_offset,
            .size_bytes = comm->decl_variable.type->size_bits / 8,
        };
        array_push(state->debug_vars, dbg_var);

        if(comm->decl_variable.assignment)
        {
            Expr_Result right_res = lvm_eval(state, comm->decl_variable.assignment, return_reg_for_type(comm->decl_variable.assignment->type), true);
            lvm_gen_copy(state, right_res, (Location){.base = RBP, .offset = comm->decl_variable.stack_offset}, comm->decl_variable.type->size_bits / 8);
        }
    }
}

void
lvm_generate_comm_block(Light_Ast* comm, Light_VM_State* state, Stack_Info* stack_info)
{
    for(int i = 0; i < comm->comm_block.command_count; ++i)
        lvm_generate_command(comm->comm_block.commands[i], state, stack_info);
}

void
lvm_generate_comm_assignment(Light_Ast* comm, Light_VM_State* state)
{
    Expr_Result right_res = lvm_eval(state, comm->comm_assignment.rvalue, return_reg_for_type(comm->comm_assignment.rvalue->type), true);
    lvm_push_result(state, right_res);
    Expr_Result left_res = lvm_eval(state, comm->comm_assignment.lvalue, R2, false);
    assert(left_res.type == EXPR_RES_REGISTER);
    assert(!lvm_result_equal(right_res, left_res));
    lvm_pop_result(state, right_res);

    lvm_gen_copy(state, right_res, (Location){.base = R2, .offset = 0}, comm->comm_assignment.rvalue->type->size_bits / 8);
}

void
lvm_generate_comm_if(Light_Ast* comm, Light_VM_State* state, Stack_Info* stack_info)
{
    state->short_circuit = true;
    state->short_circuit_current_true = 1;
    state->short_circuit_current_false = -1;
    lvm_mov_int_expr_to_reg(state, comm->comm_if.condition, R0, true);
    state->short_circuit = false;

    // If there is nothing in the array, that means no AND/OR were generated,
    // in this case, just evaluate the final result and branch according to that.
    // We can't do this by branching inside because there can ban casts to bool.
    Light_VM_Instruction_Info base_case = {0};
    if(array_length(state->short_circuit_jmps) == 0)
    {
        light_vm_push(state, "cmp r0, 0");
        base_case = light_vm_push(state, "beq 0xffffffff");
    }

    for(int i = 0; i < array_length(state->short_circuit_jmps); ++i)
    {
        if(state->short_circuit_jmps[i].short_circuit_index == 1)
        {
            light_vm_patch_from_to_current_instruction(state, state->short_circuit_jmps[i]);
        }
    }
    lvm_generate_command(comm->comm_if.body_true, state, stack_info);
    Light_VM_Instruction_Info skip_true = light_vm_push(state, "jmp 0xffffffff");

    Light_VM_Instruction_Info else_block = light_vm_current_instruction(state);
    if(array_length(state->short_circuit_jmps) == 0)
    {
        light_vm_patch_immediate_distance(base_case, else_block);
    }
    if(comm->comm_if.body_false)
    {
        lvm_generate_command(comm->comm_if.body_false, state, stack_info);
    }
    
    for(int i = 0; i < array_length(state->short_circuit_jmps); ++i)
    {
        if(state->short_circuit_jmps[i].short_circuit_index == -1)
        {
            light_vm_patch_immediate_distance(state->short_circuit_jmps[i], else_block);
        }
    }
    array_clear(state->short_circuit_jmps);
    light_vm_patch_from_to_current_instruction(state, skip_true);
}

void
lvm_generate_command(Light_Ast* comm, Light_VM_State* state, Stack_Info* stack_info)
{
    switch(comm->kind)
    {
        case AST_DECL_VARIABLE:      lvm_generate_decl_variable(comm, state, stack_info); break;
        case AST_COMMAND_RETURN:     lvm_generate_comm_return(comm, state, stack_info); break;
        case AST_COMMAND_ASSIGNMENT: lvm_generate_comm_assignment(comm, state); break;
        case AST_COMMAND_BLOCK:      lvm_generate_comm_block(comm, state, stack_info); break;
        case AST_COMMAND_IF:         lvm_generate_comm_if(comm, state, stack_info); break;
        case AST_COMMAND_FOR:
        case AST_COMMAND_BREAK:
        case AST_COMMAND_CONTINUE:
        case AST_COMMAND_WHILE:
        case AST_DECL_PROCEDURE: Unimplemented; break;
        default: Unimplemented;
    }
}

/* ---------------------------------------------------------------------*/
/* ------------------------ Declarations -------------------------------*/
/* ---------------------------------------------------------------------*/

void
lvm_generate_proc_decl(Light_Ast* proc, Light_Scope* global_scope, Light_VM_State* state)
{
    stack_for_arguments(proc);
    
    Light_Ast* body = proc->decl_proc.body;

    if(body)
    {
        // Saves the previous stack frame base to return
        Light_VM_Instruction_Info base = light_vm_push(state, "push rbp");
        light_vm_push(state, "mov rbp, rsp");

        // Allocate space in the stack for the temporary variables
        Light_VM_Instruction_Info stack_alloc = light_vm_push(state, "subs rsp, 0xffffffff");

        // Saves the first instruction for other procedures to call this.
        array_push(state->proc_bases, base);
        proc->decl_proc.lvm_base_instruction = &state->proc_bases[array_length(state->proc_bases) -1];

        Stack_Info stack_info = {0};
        for(int i = 0; i < body->comm_block.command_count; ++i)
        {
            Light_Ast* comm = body->comm_block.commands[i];
            lvm_generate_command(comm, state, &stack_info);
        }

        // Patch the value of the instruction
        light_vm_patch_instruction_immediate(stack_alloc, stack_info.size_bytes);

        // Equivalent to the 'leave' instruction in x86-64,
        // this puts the stack in the same state as it was before the function call.
        light_vm_push(state, "pop rbp");
        light_vm_push(state, "mov rsp, rbp");

        if(proc->decl_proc.flags & DECL_PROC_FLAG_MAIN)
            light_vm_push(state, "hlt");
        else
            light_vm_push(state, "ret");
    }
}

int
lvm_generate(Light_Ast** ast, Light_Scope* global_scope)
{
    Light_VM_State* state = light_vm_init();

    state->short_circuit_jmps = array_new(Light_VM_Instruction_Info);
    state->proc_bases = array_new(Light_VM_Instruction_Info);
    state->proc_patch_calls = array_new(Patch_Procs);

    //light_vm_push(state, "mov r0, 33");
    //light_vm_push(state, "push r0");
    //light_vm_push(state, "push r0");

    Light_VM_Instruction_Info start = light_vm_push(state, "call 0xff");
    light_vm_push(state, "hlt");
    light_vm_patch_from_to_current_instruction(state, start);

    // Generate all procedures
    for(int i = 0; i < array_length(ast); ++i)
    {
        if(ast[i]->kind == AST_DECL_PROCEDURE)
            lvm_generate_proc_decl(ast[i], global_scope, state);
    }

    for(int i = 0; i < array_length(state->proc_patch_calls); ++i)
    {
        Light_VM_Instruction_Info to = *(Light_VM_Instruction_Info*)((Light_Ast*)state->proc_patch_calls[i].to_decl)->decl_proc.lvm_base_instruction;
        light_vm_patch_immediate_distance(state->proc_patch_calls[i].from, to);
    }
    array_free(state->proc_bases);
    array_free(state->proc_patch_calls);

    light_vm_push(state, "hlt");
    light_vm_debug_dump_code(stdout, state);

    light_vm_execute(state, 0, true);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL|LVM_PRINT_FLOATING_POINT_REGISTERS);

    return 0;
}
