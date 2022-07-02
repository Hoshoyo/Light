#include "../../light_vm/lightvm.h"
#include "../../ast.h"
#include "../../type.h"
#include <assert.h>
#include <light_array.h>

#define BITS_IN_BYTE 8
#define LVM_RETURN_REGISTER R0
#define LVM_RETURN_F32_REGISTER FR0
#define LVM_RETURN_F64_REGISTER EFR0

#define EXPR_FLAG_DEREFERENCE (1 << 0)
#define EXPR_FLAG_ASSIGNMENT_ROOT (1 << 1) // Indicates any dereference operations should not dereference
#define EXPR_FLAG_INVERTED_SHORT_CIRCUIT (1 << 2)

#define Unimplemented assert(0 && "Unimplemented")
#define Unreachable assert(0 && "Unreachable")

// This holds a procedure entry for calls to refer to
// when calling the code for the procedure.
typedef struct {
    Light_VM_Instruction_Info from;
    void* to_decl;
} Patch_Procs;

typedef struct {
    // Current generation state for short circuit
    bool                          short_circuit;
    bool                          short_circuit_if;
    int32_t                       short_circuit_current_true;
    int32_t                       short_circuit_current_false;

    Patch_Procs*                  proc_patch_calls;
    Light_VM_Instruction_Info*    short_circuit_jmps;
    Light_VM_Instruction_Info*    proc_bases;
    Light_VM_Instruction_Info*    loop_breaks;
    Light_VM_Instruction_Info*    loop_continue;
} LVM_Generator;

// Used to calculate the stack size and current offset
// during code generation of internal scopes.
typedef struct {
    int offset;
    int index;
    int size_bytes;
} Stack_Info;

typedef enum {
    EXPR_RESULT_REG,
    EXPR_RESULT_F64_REG,
    EXPR_RESULT_F32_REG,
} Expr_Result_Type;
typedef struct {
    Expr_Result_Type type;
    s32              size_bytes;
    s32              temp_release_size_bytes;
    union {
        LVM_Register     reg;
        LVM_F32_Register freg;
        LVM_F64_Register efreg;
    };
} Expr_Result;

typedef struct {
    LVM_Register base;
    s32          offset;
} Location;

// -------------------------------------------------
// Misc
static int
align_to_ptrsize(uint64_t v)
{
    v = v + ((LVM_PTRSIZE - (v % LVM_PTRSIZE)) % LVM_PTRSIZE);
    return (int)v;
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

static Light_VM_Instruction
mk_bin_instr(LVM_Instr_Type type, Expr_Result left, Expr_Result right)
{
    assert(left.type == right.type && left.size_bytes == right.size_bytes);

    Light_VM_Instruction instr = { .type = type };
    switch(left.type)
    {
        case EXPR_RESULT_REG: {
            instr.binary.addr_mode = BIN_ADDR_MODE_REG_TO_REG;
            instr.binary.bytesize = left.size_bytes;
            instr.binary.dst_reg = left.reg;
            instr.binary.src_reg = right.reg;            
        } break;
        case EXPR_RESULT_F32_REG: {
            instr.ifloat.addr_mode = FLOAT_ADDR_MODE_REG_TO_REG;
            instr.ifloat.is64bit = false;
            instr.ifloat.dst_reg = left.reg;
            instr.ifloat.src_reg = right.reg; 
        } break;
        case EXPR_RESULT_F64_REG: {
            instr.ifloat.addr_mode = FLOAT_ADDR_MODE_REG_TO_REG;
            instr.ifloat.is64bit = true;
            instr.ifloat.dst_reg = left.reg;
            instr.ifloat.src_reg = right.reg;  
        } break;
        default: Unreachable;
    }
    return instr;
}

static Light_VM_Instruction
mk_un_instr(LVM_Instr_Type type, Expr_Result r)
{
    return (Light_VM_Instruction) { 
        .type = type,
        .unary.byte_size = r.size_bytes,
        .unary.reg = r.reg,
    };
}

// -------------------------------------------------
// Generation

static Expr_Result lvmgen_expr(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags);
static void        lvmgen_command(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm);

static void
mask_top_bits(Light_VM_State* state, Expr_Result r)
{
    if(r.type == EXPR_RESULT_REG)
    {
        switch(r.size_bytes)
        {
            case 1: light_vm_push_fmt(state, "and r%d, 0xff", r.reg); break;
            case 2: light_vm_push_fmt(state, "and r%d, 0xffff", r.reg); break;
            case 4: light_vm_push_fmt(state, "and r%d, 0xffffffff", r.reg); break;
            default: break;
        }
    }
}

static void
lvmg_move_to(Light_VM_State* state, Expr_Result from, int to_reg_index)
{
    assert(to_reg_index >= R0 && to_reg_index <= R7);
    switch(from.type)
    {
        case EXPR_RESULT_REG: {
            if(from.reg != to_reg_index)
                light_vm_push_fmt(state, "mov r%d, r%d", to_reg_index, from.reg);
        } break;
        case EXPR_RESULT_F32_REG: {
            if(from.freg != to_reg_index)
                light_vm_push_fmt(state, "fmov fr%d, fr%d", to_reg_index, from.freg);
        } break;
        case EXPR_RESULT_F64_REG: {
            if(from.efreg != to_reg_index)
                light_vm_push_fmt(state, "fmov efr%d, efr%d", to_reg_index, from.efreg);
        } break;
        default: Unreachable;
    }
}

static void
lvmg_push_result(Light_VM_State* state, Expr_Result r)
{
    switch(r.type)
    {
        case EXPR_RESULT_REG: {
            light_vm_push_fmt(state, "push r%d", r.reg);
        } break;
        case EXPR_RESULT_F32_REG: {
            light_vm_push_fmt(state, "fpush fr%d", r.freg);
        } break;
        case EXPR_RESULT_F64_REG: {
            light_vm_push_fmt(state, "fpush efr%d", r.efreg);
        } break;
        default: Unreachable;
    }
}

static Expr_Result
lvmg_next(Expr_Result r)
{
    r.reg = (r.reg + 1) % FREG_COUNT;
    return r;
}

static void
lvmg_pop_next(Light_VM_State* state, Expr_Result r, Expr_Result* out)
{
    r = lvmg_next(r);
    switch(r.type)
    {
        case EXPR_RESULT_REG: {
            out->reg = r.reg;
            light_vm_push_fmt(state, "pop r%d", r.reg);
        } break;
        case EXPR_RESULT_F32_REG: {
            out->freg = r.freg;
            light_vm_push_fmt(state, "fpop fr%d", r.freg);
        } break;
        case EXPR_RESULT_F64_REG: {
            out->efreg = r.efreg;
            light_vm_push_fmt(state, "fpop efr%d", r.efreg);
        } break;
        default: Unreachable;
    }
}

static Expr_Result
lvmg_reg_for_type(Light_Type* type)
{
    Expr_Result result = {0};
    if(type_primitive_float32(type))
    {
        result.reg = FR0;
        result.type = EXPR_RESULT_F32_REG;
        result.size_bytes = 4;
    }
    else if(type_primitive_float64(type))
    {
        result.reg = EFR0;
        result.type = EXPR_RESULT_F64_REG;
        result.size_bytes = 8;
    }
    else
    {
        result.reg = R0;
        result.type = EXPR_RESULT_REG;
        result.size_bytes = type->size_bits / BITS_IN_BYTE;
    }
    return result;
}

static void
lvmg_copy(Light_VM_State* state, Expr_Result rvalue, Location location)
{
    if(rvalue.type == EXPR_RESULT_REG)
    {
        if(rvalue.size_bytes <= 8)
        {
            if(location.offset < 0)
                light_vm_push_fmt(state, "mov [r%d %s %d], r%d%s", 
                    location.base, "-", -location.offset, rvalue.reg, register_size_suffix(rvalue.size_bytes));
            else
                light_vm_push_fmt(state, "mov [r%d %s %d], r%d%s", 
                    location.base, "+", location.offset, rvalue.reg, register_size_suffix(rvalue.size_bytes));
        }
        else
        {
            LVM_Register dst = R5;
            if(rvalue.reg == dst)
                dst = R6;

            light_vm_push_fmt(state, "mov r%d, r%d", dst, location.base);
            if(location.offset < 0)
                light_vm_push_fmt(state, "subs r%d, %d", dst, -location.offset);
            else
                light_vm_push_fmt(state, "adds r%d, %d", dst, location.offset);

            light_vm_push_fmt(state, "mov r7, %lld", rvalue.size_bytes);
            light_vm_push_fmt(state, "copy r%d, r%d, r7", dst, rvalue.reg);

            if(rvalue.temp_release_size_bytes > 0)
                light_vm_push_fmt(state, "adds rsp, %d", rvalue.temp_release_size_bytes);
        }
    }
    else if(rvalue.type == EXPR_RESULT_F32_REG)
    {
        if(location.offset < 0)
            light_vm_push_fmt(state, "fmov [r%d %s %d], fr%d", 
                location.base, "-", -location.offset, rvalue.freg);
        else
            light_vm_push_fmt(state, "fmov [r%d %s %d], fr%d", 
                location.base, "+", location.offset, rvalue.freg);
    }
    else if(rvalue.type == EXPR_RESULT_F64_REG)
    {
        if(location.offset < 0)
            light_vm_push_fmt(state, "fmov [r%d %s %d], efr%d", 
                location.base, "-", -location.offset, rvalue.efreg);
        else
            light_vm_push_fmt(state, "fmov [r%d %s %d], efr%d", 
                location.base, "+", location.offset, rvalue.efreg);
    }
    else
    {
        Unreachable;
    }
}

static void
lvmg_deref(Light_VM_State* state, Expr_Result rvalue, Location location)
{
    if(rvalue.type == EXPR_RESULT_REG)
    {
        if(rvalue.size_bytes <= 8)
        {
            if(location.offset < 0)
                light_vm_push_fmt(state, "mov r%d%s, [r%d %s %d]", 
                    rvalue.reg, register_size_suffix(rvalue.size_bytes), location.base, "-", -location.offset);
            else
                light_vm_push_fmt(state, "mov r%d%s, [r%d %s %d]", 
                    rvalue.reg, register_size_suffix(rvalue.size_bytes), location.base, "+", location.offset);
        }
        else
        {
            // Keep the way that it is, someone will make a copy to the
            // proper location later.
        }
    }
    else if(rvalue.type == EXPR_RESULT_F32_REG)
    {
        if(location.offset < 0)
            light_vm_push_fmt(state, "fmov fr%d, [r%d %s %d]", 
                rvalue.freg, location.base, "-", -location.offset);
        else
            light_vm_push_fmt(state, "fmov fr%d, [r%d %s %d]", 
                rvalue.freg, location.base, "+", location.offset);
    }
    else if(rvalue.type == EXPR_RESULT_F64_REG)
    {
        if(location.offset < 0)
            light_vm_push_fmt(state, "fmov efr%d, [r%d %s %d]", 
                rvalue.efreg, location.base, "-", -location.offset);
        else
            light_vm_push_fmt(state, "fmov efr%d, [r%d %s %d]", 
                rvalue.efreg, location.base, "+", location.offset);
    }
    else
    {
        Unreachable;
    }
}

static Expr_Result
lvmgen_expr_literal_primitive(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    Expr_Result result = {0};

    if(type_primitive_bool(expr->type))
    {
        light_vm_push_fmt(state, "mov r0b, %d", expr->expr_literal_primitive.value_bool);
        result.type = EXPR_RESULT_REG;
        result.reg = R0;
        result.size_bytes = 1;
    }
    else if(type_primitive_int(expr->type))
    {
        Light_VM_Instruction instr = {0};
        switch(expr->type->primitive)
        {
            case TYPE_PRIMITIVE_BOOL:
            case TYPE_PRIMITIVE_U8:
            case TYPE_PRIMITIVE_S8: {
                instr.type = LVM_MOV;
                instr.imm_size_bytes = 1;
                instr.binary.bytesize = 1;
                instr.binary.dst_reg = result.reg;
                instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u8);                
            } break;
            case TYPE_PRIMITIVE_U16:
            case TYPE_PRIMITIVE_S16: {
                instr.type = LVM_MOV;
                instr.imm_size_bytes = 2;
                instr.binary.bytesize = 2;
                instr.binary.dst_reg = result.reg;
                instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u16);                
            } break;
            case TYPE_PRIMITIVE_U32:
            case TYPE_PRIMITIVE_S32: {
                instr.type = LVM_MOV;
                instr.imm_size_bytes = 4;
                instr.binary.bytesize = 4;
                instr.binary.dst_reg = result.reg;
                instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u32);                
            } break;
            case TYPE_PRIMITIVE_U64:
            case TYPE_PRIMITIVE_S64: {
                instr.type = LVM_MOV;
                instr.imm_size_bytes = 8;
                instr.binary.bytesize = 8;
                instr.binary.dst_reg = result.reg;
                instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u64);                
            } break;
            default: Unreachable; break;
        }
        result.type = EXPR_RESULT_REG;
        result.reg = R0;
        result.size_bytes = expr->type->size_bits / BITS_IN_BYTE;
    }
    else if(type_primitive_float32(expr->type))
    {
        light_vm_push_fmt(state, "fmov fr0, [rdp + %d]", state->data_offset);
        light_vm_push_r32_to_datasegment(state, expr->expr_literal_primitive.value_r32);
        result.type = EXPR_RESULT_F32_REG;
        result.reg = FR0;
        result.size_bytes = 4;
    }
    else if(type_primitive_float64(expr->type))
    {
        light_vm_push_fmt(state, "fmov efr0, [rdp + %d]", state->data_offset);
        light_vm_push_r64_to_datasegment(state, expr->expr_literal_primitive.value_r64);
        result.type = EXPR_RESULT_F64_REG;
        result.reg = EFR0;
        result.size_bytes = 8;
    }
    else if(type_pointer(expr->type))
    {
        light_vm_push(state, "mov r0, 0");
        result.type = EXPR_RESULT_REG;
        result.reg = R0;
        result.size_bytes = LVM_PTRSIZE;
    }
    else
    {
        Unreachable;
    }

    return result;
}

static Expr_Result
lvmgen_expr_binary_compare(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_BINARY);
    Expr_Result result = {0};

    flags |= EXPR_FLAG_DEREFERENCE;

    Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, flags);
    lvmg_push_result(state, left);

    Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, flags);
    lvmg_pop_next(state, right, &left);

    result = (Expr_Result) {
        .type = EXPR_RESULT_REG,
        .size_bytes = 1,
        .reg = R0,
    };

    Light_Type* subtype = type_alias_root(expr->expr_binary.left->type);
    if(type_primitive_sint(subtype) || subtype->kind == TYPE_KIND_ENUM)
    {
        light_vm_push_instruction(state, mk_bin_instr(LVM_CMP, left, right), 0); 
        switch(expr->expr_binary.op)
        {
            case OP_BINARY_LT:         light_vm_push_fmt(state, "movlts r%d", result.reg); break;
            case OP_BINARY_LE:         light_vm_push_fmt(state, "movles r%d", result.reg); break;
            case OP_BINARY_GT:         light_vm_push_fmt(state, "movgts r%d", result.reg); break;
            case OP_BINARY_GE:         light_vm_push_fmt(state, "movges r%d", result.reg); break;
            case OP_BINARY_EQUAL:      light_vm_push_fmt(state, "moveq r%d", result.reg);  break;
            case OP_BINARY_NOT_EQUAL:  light_vm_push_fmt(state, "movne r%d", result.reg);  break;
            default: Unreachable;
        }
    }
    else if(type_primitive_uint(subtype))
    {
        light_vm_push_instruction(state, mk_bin_instr(LVM_CMP, left, right), 0); 
        switch(expr->expr_binary.op)
        {
            case OP_BINARY_LT:         light_vm_push_fmt(state, "movltu r%d", result.reg); break;
            case OP_BINARY_LE:         light_vm_push_fmt(state, "movleu r%d", result.reg); break;
            case OP_BINARY_GT:         light_vm_push_fmt(state, "movgtu r%d", result.reg); break;
            case OP_BINARY_GE:         light_vm_push_fmt(state, "movgeu r%d", result.reg); break;
            case OP_BINARY_EQUAL:      light_vm_push_fmt(state, "moveq r%d", result.reg);  break;
            case OP_BINARY_NOT_EQUAL:  light_vm_push_fmt(state, "movne r%d", result.reg);  break;
            default: Unreachable;
        }
    }
    else if(type_primitive_float32(subtype) || type_primitive_float64(subtype))
    {
        light_vm_push_instruction(state, mk_bin_instr(LVM_FCMP, left, right), 0); 
        switch(expr->expr_binary.op)
        {
            case OP_BINARY_LT:         light_vm_push_fmt(state, "fmovlt r%d", result.reg); break;
            case OP_BINARY_LE:         light_vm_push_fmt(state, "fmovle r%d", result.reg); break;
            case OP_BINARY_GT:         light_vm_push_fmt(state, "fmovgt r%d", result.reg); break;
            case OP_BINARY_GE:         light_vm_push_fmt(state, "fmovge r%d", result.reg); break;
            case OP_BINARY_EQUAL:      light_vm_push_fmt(state, "fmoveq r%d", result.reg);  break;
            case OP_BINARY_NOT_EQUAL:  light_vm_push_fmt(state, "fmovne r%d", result.reg);  break;
            default: Unreachable;
        }
    }
    else if(type_pointer(subtype) || subtype->kind == TYPE_KIND_FUNCTION || type_primitive_bool(subtype))
    {
        light_vm_push_instruction(state, mk_bin_instr(LVM_CMP, left, right), 0); 
        switch(expr->expr_binary.op)
        {
            case OP_BINARY_EQUAL:      light_vm_push_fmt(state, "moveq r%d", result.reg); break;
            case OP_BINARY_NOT_EQUAL:  light_vm_push_fmt(state, "movne r%d", result.reg); break;
            default: Unreachable;
        }
    }

    return result;
}

static Expr_Result
lvmgen_expr_binary_arithmetic(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_BINARY);
    Expr_Result result = {0};

    flags |= EXPR_FLAG_DEREFERENCE;

    Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, flags);
    lvmg_push_result(state, left);

    Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, flags);
    lvmg_pop_next(state, right, &left);

    result = left;

    if(type_primitive_sint(expr->type))
    {
        switch(expr->expr_binary.op)
        {
            case OP_BINARY_PLUS:  light_vm_push_instruction(state, mk_bin_instr(LVM_ADD_S, left, right), 0); break;
            case OP_BINARY_MINUS: light_vm_push_instruction(state, mk_bin_instr(LVM_SUB_S, left, right), 0); break;
            case OP_BINARY_MULT:  light_vm_push_instruction(state, mk_bin_instr(LVM_MUL_S, left, right), 0); break;
            case OP_BINARY_DIV:   light_vm_push_instruction(state, mk_bin_instr(LVM_DIV_S, left, right), 0); break;
            case OP_BINARY_MOD:   light_vm_push_instruction(state, mk_bin_instr(LVM_MOD_S, left, right), 0); break;
            case OP_BINARY_SHL:   light_vm_push_instruction(state, mk_bin_instr(LVM_SHL, left, right), 0); break;
            case OP_BINARY_SHR:   light_vm_push_instruction(state, mk_bin_instr(LVM_SHR, left, right), 0); break;
            case OP_BINARY_AND:   light_vm_push_instruction(state, mk_bin_instr(LVM_AND, left, right), 0); break;
            case OP_BINARY_OR:    light_vm_push_instruction(state, mk_bin_instr(LVM_OR,  left, right), 0); break;
            case OP_BINARY_XOR:   light_vm_push_instruction(state, mk_bin_instr(LVM_XOR, left, right), 0); break;
            default: Unreachable;
        }
    }
    else if(type_primitive_uint(expr->type))
    {
        switch(expr->expr_binary.op)
        {
            case OP_BINARY_PLUS:  light_vm_push_instruction(state, mk_bin_instr(LVM_ADD_U, left, right), 0); break;
            case OP_BINARY_MINUS: light_vm_push_instruction(state, mk_bin_instr(LVM_SUB_U, left, right), 0); break;
            case OP_BINARY_MULT:  light_vm_push_instruction(state, mk_bin_instr(LVM_MUL_U, left, right), 0); break;
            case OP_BINARY_DIV:   light_vm_push_instruction(state, mk_bin_instr(LVM_DIV_U, left, right), 0); break;
            case OP_BINARY_MOD:   light_vm_push_instruction(state, mk_bin_instr(LVM_MOD_U, left, right), 0); break;
            case OP_BINARY_SHL:   light_vm_push_instruction(state, mk_bin_instr(LVM_SHL, left, right), 0); break;
            case OP_BINARY_SHR:   light_vm_push_instruction(state, mk_bin_instr(LVM_SHR, left, right), 0); break;
            case OP_BINARY_AND:   light_vm_push_instruction(state, mk_bin_instr(LVM_AND, left, right), 0); break;
            case OP_BINARY_OR:    light_vm_push_instruction(state, mk_bin_instr(LVM_OR,  left, right), 0); break;
            case OP_BINARY_XOR:   light_vm_push_instruction(state, mk_bin_instr(LVM_XOR, left, right), 0); break;
            default: Unreachable;
        }
    }
    else if(type_primitive_float32(expr->type) || type_primitive_float64(expr->type))
    {
        switch(expr->expr_binary.op)
        {
            case OP_BINARY_PLUS:  light_vm_push_instruction(state, mk_bin_instr(LVM_FADD, left, right), 0); break;
            case OP_BINARY_MINUS: light_vm_push_instruction(state, mk_bin_instr(LVM_FSUB, left, right), 0); break;
            case OP_BINARY_MULT:  light_vm_push_instruction(state, mk_bin_instr(LVM_FMUL, left, right), 0); break;
            case OP_BINARY_DIV:   light_vm_push_instruction(state, mk_bin_instr(LVM_FDIV, left, right), 0); break;
            default: Unreachable;
        }
    }
    else if(type_pointer(expr->type))
    {
        mask_top_bits(state, right);
        right.size_bytes = left.size_bytes;
        switch(expr->expr_binary.op)
        {
            case OP_BINARY_PLUS:  light_vm_push_instruction(state, mk_bin_instr(LVM_ADD_U, left, right), 0); break;
            case OP_BINARY_MINUS: light_vm_push_instruction(state, mk_bin_instr(LVM_SUB_U, left, right), 0); break;
            default: Unreachable;
        }
    }
    else
    {
        Unreachable;
    }

    return result;
}

static Expr_Result
lvmgen_expr_binary_logical(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    Expr_Result result = {
        .reg = R0,
        .size_bytes = 1,
        .type = EXPR_RESULT_REG,
    };
    assert(type_primitive_bool(expr->expr_binary.left->type) && type_primitive_bool(expr->expr_binary.right->type));

    bool sc = gen->short_circuit;
    int prev_false;
    int this_false;
    int prev_true;
    int this_true;
    if(!sc) 
    { 
        gen->short_circuit = true;
        gen->short_circuit_current_true = 1;
        gen->short_circuit_current_false = -1;
        this_true = 1;
        this_false = -1;
    }

    if(flags & EXPR_FLAG_INVERTED_SHORT_CIRCUIT)
    {
        flags &= ~EXPR_FLAG_INVERTED_SHORT_CIRCUIT;
        if(expr->expr_binary.op == OP_BINARY_LOGIC_OR)
        {
            prev_true = gen->short_circuit_current_true++;
            this_true = gen->short_circuit_current_true;

            Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 1", left.reg);
                Light_VM_Instruction_Info info = light_vm_push(state, "beq 0xffffffff");
                info.short_circuit_index = gen->short_circuit_current_false;
                array_push(gen->short_circuit_jmps, info);
            }

            for(int i = array_length(gen->short_circuit_jmps) - 1; i >= 0; --i)
                if(gen->short_circuit_jmps[i].short_circuit_index == this_true)
                {
                    light_vm_patch_from_to_current_instruction(state, gen->short_circuit_jmps[i]);
                    array_remove(gen->short_circuit_jmps, i);
                }
            gen->short_circuit_current_true = prev_true;

            Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 1", right.reg);
                Light_VM_Instruction_Info info2 = light_vm_push(state, "beq 0xffffffff");
                info2.short_circuit_index = gen->short_circuit_current_false;
                array_push(gen->short_circuit_jmps, info2);
            }
        }
        else
        {
            prev_false = gen->short_circuit_current_false--;
            this_false = gen->short_circuit_current_false;

            Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 1", left.reg);
                Light_VM_Instruction_Info info = light_vm_push(state, "bne 0xffffffff");
                info.short_circuit_index = gen->short_circuit_current_true;
                array_push(gen->short_circuit_jmps, info);
            }

            for(int i = array_length(gen->short_circuit_jmps)-1; i >= 0 ; --i)
                if(gen->short_circuit_jmps[i].short_circuit_index == this_false)
                {
                    light_vm_patch_from_to_current_instruction(state, gen->short_circuit_jmps[i]);
                    array_remove(gen->short_circuit_jmps, i);
                }
            gen->short_circuit_current_false = prev_false;

            Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 1", right.reg);
                Light_VM_Instruction_Info info2 = light_vm_push(state, "beq 0xffffffff");
                info2.short_circuit_index = gen->short_circuit_current_false;
                array_push(gen->short_circuit_jmps, info2);
            }
        }
    }
    else
    {
        if(expr->expr_binary.op == OP_BINARY_LOGIC_AND)
        {
            prev_true = gen->short_circuit_current_true++;
            this_true = gen->short_circuit_current_true;

            Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 0", left.reg);
                Light_VM_Instruction_Info info = light_vm_push(state, "beq 0xffffffff");
                info.short_circuit_index = gen->short_circuit_current_false;
                array_push(gen->short_circuit_jmps, info);
            }

            for(int i = array_length(gen->short_circuit_jmps) - 1; i >= 0; --i)
                if(gen->short_circuit_jmps[i].short_circuit_index == this_true)
                {
                    light_vm_patch_from_to_current_instruction(state, gen->short_circuit_jmps[i]);
                    array_remove(gen->short_circuit_jmps, i);
                }
            gen->short_circuit_current_true = prev_true;

            Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 0", right.reg);
                Light_VM_Instruction_Info info2 = light_vm_push(state, "beq 0xffffffff");
                info2.short_circuit_index = gen->short_circuit_current_false;
                array_push(gen->short_circuit_jmps, info2);
            }
        }
        else
        {
            prev_false = gen->short_circuit_current_false--;
            this_false = gen->short_circuit_current_false;

            Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 0", left.reg);
                Light_VM_Instruction_Info info = light_vm_push(state, "bne 0xffffffff");
                info.short_circuit_index = gen->short_circuit_current_true;
                array_push(gen->short_circuit_jmps, info);
            }

            for(int i = array_length(gen->short_circuit_jmps)-1; i >= 0 ; --i)
                if(gen->short_circuit_jmps[i].short_circuit_index == this_false)
                {
                    light_vm_patch_from_to_current_instruction(state, gen->short_circuit_jmps[i]);
                    array_remove(gen->short_circuit_jmps, i);
                }
            gen->short_circuit_current_false = prev_false;

            Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 0", right.reg);
                Light_VM_Instruction_Info info2 = light_vm_push(state, "beq 0xffffffff");
                info2.short_circuit_index = gen->short_circuit_current_false;
                array_push(gen->short_circuit_jmps, info2);
            }
        }
    }

    if(!gen->short_circuit_if && !sc)
    {
        Light_VM_Instruction_Info end_branch = light_vm_current_instruction(state);
        for(int i = 0; i < array_length(gen->short_circuit_jmps); ++i)
            light_vm_patch_immediate_distance(gen->short_circuit_jmps[i], end_branch);
    }

    if(!sc) array_clear(gen->short_circuit_jmps);

    return result;
}

static Expr_Result
lvmgen_expr_binary(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_BINARY);
    Expr_Result result = {0};

    switch(expr->expr_binary.op)
    {
        case OP_BINARY_PLUS:
        case OP_BINARY_MINUS:
        case OP_BINARY_MULT:
        case OP_BINARY_DIV:
        case OP_BINARY_MOD:
        case OP_BINARY_SHL:
        case OP_BINARY_SHR:
        case OP_BINARY_AND:
        case OP_BINARY_OR:
        case OP_BINARY_XOR:
            result = lvmgen_expr_binary_arithmetic(state, gen, expr, flags);
            break;

        case OP_BINARY_LOGIC_AND:
        case OP_BINARY_LOGIC_OR:
            result = lvmgen_expr_binary_logical(state, gen, expr, flags);
            break;

        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE:
        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL:
            result = lvmgen_expr_binary_compare(state, gen, expr, flags);
            break;

        case OP_BINARY_VECTOR_ACCESS:
            Unimplemented;

        default: Unreachable;
    }
    return result;
}

static Expr_Result
lvmgen_expr_unary(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_UNARY);
    Expr_Result result = {0};

    switch(expr->expr_unary.op)
    {
        case OP_UNARY_PLUS: {
            result = lvmgen_expr(state, gen, expr->expr_unary.operand, flags|EXPR_FLAG_DEREFERENCE);
        } break;
        case OP_UNARY_MINUS: {
            result = lvmgen_expr(state, gen, expr->expr_unary.operand, flags|EXPR_FLAG_DEREFERENCE);
            Light_VM_Instruction instr = mk_un_instr(type_primitive_float(expr->type) ? LVM_FNEG : LVM_NEG, result);
            light_vm_push_instruction(state, instr, 0);
        } break;
        case OP_UNARY_LOGIC_NOT: {
            assert(type_primitive_bool(expr->type));
            if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_LOGIC_NOT)
            {
                result = lvmgen_expr(state, gen, expr->expr_unary.operand->expr_unary.operand, flags|EXPR_FLAG_DEREFERENCE);
            }
            else if(gen->short_circuit && expr->expr_unary.operand->kind == AST_EXPRESSION_BINARY && 
                (expr->expr_unary.operand->expr_binary.op == OP_BINARY_LOGIC_AND || expr->expr_unary.operand->expr_binary.op == OP_BINARY_LOGIC_OR))
            {
                result = lvmgen_expr(state, gen, expr->expr_unary.operand, flags|EXPR_FLAG_DEREFERENCE|EXPR_FLAG_INVERTED_SHORT_CIRCUIT);
            }
            else
            {
                result = lvmgen_expr(state, gen, expr->expr_unary.operand, flags|EXPR_FLAG_DEREFERENCE);
                light_vm_push_fmt(state, "cmp r%db, 0", result.reg);
                light_vm_push_fmt(state, "moveq r%d", result.reg);
            }
        } break;
        case OP_UNARY_BITWISE_NOT: {
            assert(type_primitive_int(expr->type));
            result = lvmgen_expr(state, gen, expr->expr_unary.operand, flags|EXPR_FLAG_DEREFERENCE);
            Light_VM_Instruction instr = mk_un_instr(LVM_NOT, result);
            light_vm_push_instruction(state, instr, 0);
        } break;
        case OP_UNARY_CAST: {
            Unimplemented;
        } break;
        case OP_UNARY_ADDRESSOF: {
            if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_DEREFERENCE)
                result = lvmgen_expr(state, gen, expr->expr_unary.operand->expr_unary.operand, flags);
            else
            {
                result = lvmgen_expr(state, gen, expr->expr_unary.operand, flags & ~EXPR_FLAG_DEREFERENCE);
                result.type = EXPR_RESULT_REG;
            }
        } break;
        case OP_UNARY_DEREFERENCE: {
            if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_ADDRESSOF)
                result = lvmgen_expr(state, gen, expr->expr_unary.operand->expr_unary.operand, (flags|EXPR_FLAG_DEREFERENCE) & ~EXPR_FLAG_ASSIGNMENT_ROOT);
            else
                result = lvmgen_expr(state, gen, expr->expr_unary.operand, (flags|EXPR_FLAG_DEREFERENCE) & ~EXPR_FLAG_ASSIGNMENT_ROOT);
            if(flags & EXPR_FLAG_DEREFERENCE)
            {
                Expr_Result r = lvmg_reg_for_type(expr->type);
                lvmg_deref(state, r, (Location){.base = result.reg, .offset = 0});
                result = r;
            }
        } break;
        default: Unreachable;
    }

    return result;
}

static Expr_Result
lvmgen_expr_variable(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_VARIABLE);
    Expr_Result result = {0};

    Light_Ast* decl = expr->expr_variable.decl;
    if(decl->decl_variable.storage_class == STORAGE_CLASS_STACK)
    {
        if((flags & EXPR_FLAG_DEREFERENCE) && (expr->type->size_bits <= (LVM_PTRSIZE * BITS_IN_BYTE)))
        {
            if(type_primitive_float(expr->type))
            {
                if(type_primitive_float32(expr->type))
                {
                    result.reg = FR0;
                    result.type = EXPR_RESULT_F32_REG;
                    result.size_bytes = 4;
                }
                else
                {
                    result.reg = EFR0;
                    result.type = EXPR_RESULT_F64_REG;
                    result.size_bytes = 8;
                }
                Light_VM_Instruction instr = {
                    .type = LVM_FMOV,
                    .imm_size_bytes = 4,
                    .ifloat.addr_mode = FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG,
                    .ifloat.dst_reg = result.reg,
                    .ifloat.src_reg = LRBP,
                };
                light_vm_push_instruction(state, instr, expr->expr_variable.decl->decl_variable.stack_offset);
            }
            else
            {
                result.reg = R0;
                result.type = EXPR_RESULT_REG;
                result.size_bytes = expr->type->size_bits / BITS_IN_BYTE;

                Light_VM_Instruction instr = {
                    .type = LVM_MOV,
                    .imm_size_bytes = 4,
                    .binary.bytesize = expr->type->size_bits / BITS_IN_BYTE,
                    .binary.addr_mode = BIN_ADDR_MODE_REG_OFFSETED_TO_REG,
                    .binary.dst_reg = result.reg,
                    .binary.src_reg = LRBP,
                };
                light_vm_push_instruction(state, instr, expr->expr_variable.decl->decl_variable.stack_offset);
            }
        }
        else
        {
            result.reg = R0;
            result.type = EXPR_RESULT_REG;
            result.size_bytes = LVM_PTRSIZE;
            light_vm_push_fmt(state, "mov r%d, rbp", result.reg);
            if(expr->expr_variable.decl->decl_variable.stack_offset < 0)
                light_vm_push_fmt(state, "subs r%d, %d", result.reg, -expr->expr_variable.decl->decl_variable.stack_offset);
            else
                light_vm_push_fmt(state, "adds r%d, %d", result.reg, expr->expr_variable.decl->decl_variable.stack_offset);
        }
    }
    else
    {
        Unimplemented;
    }

    return result;
}

static Expr_Result
lvmgen_expr(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    Expr_Result result = {0};
    switch(expr->kind)
    {
        case AST_EXPRESSION_LITERAL_PRIMITIVE: result = lvmgen_expr_literal_primitive(state, gen, expr, flags); break;
        case AST_EXPRESSION_BINARY:            result = lvmgen_expr_binary(state, gen, expr, flags); break;
        case AST_EXPRESSION_UNARY:             result = lvmgen_expr_unary(state, gen, expr, flags); break;
        case AST_EXPRESSION_VARIABLE:          result = lvmgen_expr_variable(state, gen, expr, flags); break;
        case AST_EXPRESSION_PROCEDURE_CALL:
        case AST_EXPRESSION_DOT:
        case AST_EXPRESSION_LITERAL_ARRAY:
        case AST_EXPRESSION_LITERAL_STRUCT:
            light_vm_push(state, "mov r0, 1");
            break;
        default: Unreachable;
    }
    return result;
}

static void
lvmgen_decl_variable(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* decl)
{
    if(!(decl->decl_variable.flags & DECL_VARIABLE_FLAG_ADDR_CALCULATED))
    {
        decl->decl_variable.stack_index  = stack_info->index++;
        decl->decl_variable.stack_offset = stack_info->offset - align_to_ptrsize(decl->decl_variable.type->size_bits / 8);
        decl->decl_variable.flags        |= DECL_VARIABLE_FLAG_ADDR_CALCULATED;
        stack_info->offset               -= align_to_ptrsize(decl->decl_variable.type->size_bits / 8);
        stack_info->size_bytes           += align_to_ptrsize(decl->decl_variable.type->size_bits / 8);

        if(decl->decl_variable.assignment)
        {
            Expr_Result right_res = lvmgen_expr(state, gen, decl->decl_variable.assignment, EXPR_FLAG_DEREFERENCE);
            if(decl->decl_variable.storage_class == STORAGE_CLASS_STACK)
            {
                lvmg_copy(state, right_res, (Location){ .base = LRBP, .offset = decl->decl_variable.stack_offset});
            }
            else
            {
                Unimplemented;
            }
        }
    }
}

static void
lvmgen_comm_assignment(Light_VM_State* state, LVM_Generator* gen, Light_Ast* comm)
{
    Expr_Result right_res = lvmgen_expr(state, gen, comm->comm_assignment.rvalue, EXPR_FLAG_DEREFERENCE);

    if(comm->comm_assignment.lvalue)
    {
        lvmg_push_result(state, right_res);
        Expr_Result left_res = lvmgen_expr(state, gen, comm->comm_assignment.lvalue, EXPR_FLAG_ASSIGNMENT_ROOT);
        assert(left_res.type == EXPR_RESULT_REG);
        lvmg_pop_next(state, right_res, &right_res);

        lvmg_copy(state, right_res, (Location){.base = left_res.reg, .offset = 0});
    }
}

void
lvmgen_comm_if(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    gen->short_circuit = true;
    gen->short_circuit_if = true;
    gen->short_circuit_current_true = 1;
    gen->short_circuit_current_false = -1;
    Expr_Result res = lvmgen_expr(state, gen, comm->comm_if.condition, EXPR_FLAG_DEREFERENCE);
    assert(res.type == EXPR_RESULT_REG);
    gen->short_circuit = false;
    gen->short_circuit_if = false;

    // If there is nothing in the array, that means no AND/OR were generated,
    // in this case, just evaluate the final result and branch according to that.
    // We can't do this by branching inside because there can ban casts to bool.
    Light_VM_Instruction_Info base_case = {0};
    if(array_length(gen->short_circuit_jmps) == 0)
    {
        light_vm_push_fmt(state, "cmp r%db, 0", res.reg);
        base_case = light_vm_push(state, "beq 0xffffffff");
    }

    for(int i = 0; i < array_length(gen->short_circuit_jmps); ++i)
        if(gen->short_circuit_jmps[i].short_circuit_index == 1)
            light_vm_patch_from_to_current_instruction(state, gen->short_circuit_jmps[i]);

    lvmgen_command(state, gen, stack_info, comm->comm_if.body_true);
    Light_VM_Instruction_Info skip_true = light_vm_push(state, "jmp 0xffffffff");

    Light_VM_Instruction_Info else_block = light_vm_current_instruction(state);
    if(array_length(gen->short_circuit_jmps) == 0)
    {
        light_vm_patch_immediate_distance(base_case, else_block);
    }
    if(comm->comm_if.body_false)
    {
        lvmgen_command(state, gen, stack_info, comm->comm_if.body_false);
    }
    
    for(int i = 0; i < array_length(gen->short_circuit_jmps); ++i)
        if(gen->short_circuit_jmps[i].short_circuit_index == -1)
            light_vm_patch_immediate_distance(gen->short_circuit_jmps[i], else_block);

    array_clear(gen->short_circuit_jmps);
    light_vm_patch_from_to_current_instruction(state, skip_true);
}

static void
lvmgen_comm_block(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    for(int i = 0; i < comm->comm_block.command_count; ++i)
        lvmgen_command(state, gen, stack_info, comm->comm_block.commands[i]);
}

static void
lvmgen_comm_return(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    Light_Ast* expr = comm->comm_return.expression;
    if(expr)
    {
        Expr_Result r = lvmgen_expr(state, gen, expr, EXPR_FLAG_DEREFERENCE);
        lvmg_move_to(state, r, LVM_RETURN_REGISTER);
    }

    if(stack_info->size_bytes > 0)
        light_vm_push(state, "mov rsp, rbp");
    light_vm_push(state, "pop rbp");
    light_vm_push(state, "ret");
}

static void
lvmgen_command(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    switch(comm->kind)
    {
        case AST_COMMAND_RETURN:     lvmgen_comm_return(state, gen, stack_info, comm); break;
        case AST_DECL_VARIABLE:      lvmgen_decl_variable(state, gen, stack_info, comm); break;
        case AST_COMMAND_ASSIGNMENT: lvmgen_comm_assignment(state, gen, comm); break;
        case AST_COMMAND_BLOCK:      lvmgen_comm_block(state, gen, stack_info, comm); break;
        case AST_COMMAND_IF:         lvmgen_comm_if(state, gen, stack_info, comm); break;
        /*
        case AST_COMMAND_WHILE:      lvmgen_comm_while(comm, state, stack_info); break;
        case AST_COMMAND_FOR:        lvmgen_comm_for(comm, state, stack_info); break;
        case AST_COMMAND_BREAK:      lvmgen_comm_break(comm, state); break;
        case AST_COMMAND_CONTINUE:   lvmgen_comm_continue(comm, state); break;
        case AST_DECL_PROCEDURE:     Unimplemented; break;
        */
        default: Unimplemented;
    }
}

static void
lvmgen_proc_decl(Light_VM_State* state, LVM_Generator* gen, Light_Ast* proc)
{
    assert(proc->kind == AST_DECL_PROCEDURE);

    // Generate the stack for the arguments
    int offset = 2 * LVM_PTRSIZE;
    for(int i = 0; i < proc->decl_proc.argument_count; ++i)
    {
        Light_Ast* var = proc->decl_proc.arguments[i];
        assert(var->kind == AST_DECL_VARIABLE);

        var->decl_variable.stack_index = i;
        var->decl_variable.stack_offset = offset;
        var->decl_variable.stack_argument_offset = offset;
        offset += align_to_ptrsize(var->type->size_bits / BITS_IN_BYTE);
    }
    
    // Generate body code
    Light_Ast* body = proc->decl_proc.body;
    if(body)
    {
        // Saves the previous stack frame base to return
        Light_VM_Instruction_Info base = light_vm_push(state, "push rbp");
        light_vm_push(state, "mov rbp, rsp");

        // Allocate space in the stack for the temporary variables
        Light_VM_Instruction_Info stack_alloc = light_vm_push(state, "subs rsp, 0xffffffff");

        // Saves the first instruction for other procedures to call this.
        array_push(gen->proc_bases, base);
        proc->decl_proc.lvm_base_instruction = &gen->proc_bases[array_length(gen->proc_bases) -1];

        Stack_Info stack_info = { .offset = 0 };
        for(int i = 0; i < body->comm_block.command_count; ++i)
        {
            Light_Ast* comm = body->comm_block.commands[i];
            lvmgen_command(state, gen, &stack_info, comm);
        }

        // Patch the value of the instruction
        light_vm_patch_instruction_immediate(stack_alloc, stack_info.size_bytes);

        // Equivalent to the 'leave' instruction in x86-64,
        // this puts the stack in the same state as it was before the function call.
        light_vm_push_fmt(state, "mov rsp, rbp");
        light_vm_push(state, "pop rbp");
        light_vm_push(state, "ret");
    }
}

void
lvm_generate(Light_Ast** ast, Light_Scope* global_scope)
{
    // -------------------------------------
    // Initialization
    Light_VM_State* state = light_vm_init();

    LVM_Generator gen = {0};
    gen.short_circuit_jmps = array_new(Light_VM_Instruction_Info);
    gen.proc_bases         = array_new(Light_VM_Instruction_Info);
    gen.loop_breaks        = array_new(Light_VM_Instruction_Info);
    gen.loop_continue      = array_new(Light_VM_Instruction_Info);
    gen.proc_patch_calls   = array_new(Patch_Procs);

    // -------------------------------------
    // Generate the code
    Light_VM_Instruction_Info start = light_vm_push(state, "call 0xff");
    light_vm_push(state, "hlt");
    light_vm_patch_from_to_current_instruction(state, start);

    for(int i = 0; i < array_length(ast); ++i)
    {
        if(ast[i]->kind == AST_DECL_PROCEDURE)
            lvmgen_proc_decl(state, &gen, ast[i]);
    }
    
    // @Temporary
    light_vm_push(state, "mov r0, 1");
    light_vm_push(state, "ret");

    // -------------------------------------
    // Perform all patching needed
    for(int i = 0; i < array_length(gen.proc_patch_calls); ++i)
    {
        Light_VM_Instruction_Info to = *(Light_VM_Instruction_Info*)((Light_Ast*)gen.proc_patch_calls[i].to_decl)->decl_proc.lvm_base_instruction;
        light_vm_patch_immediate_distance(gen.proc_patch_calls[i].from, to);
    }

    // -------------------------------------
    // Free everything
    array_free(gen.short_circuit_jmps);
    array_free(gen.proc_bases);
    array_free(gen.loop_breaks);
    array_free(gen.loop_continue);
    array_free(gen.proc_patch_calls);

    // -------------------------------------
    // Debug printout
    light_vm_debug_dump_code(stdout, state);
    light_vm_execute(state, 0, true);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL|LVM_PRINT_FLOATING_POINT_REGISTERS);
}