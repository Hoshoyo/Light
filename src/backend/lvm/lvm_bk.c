#include "../../light_vm/lightvm.h"
#include "../../ast.h"
#include "../../type.h"
#include <assert.h>
#include <light_array.h>
#include "../../../include/hoht.h"
#include "../../top_typecheck.h"
#include "../../global_tables.h"
#include "../../utils/os.h"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#define HO_ASSEMBLER_IMPLEMENT
#include "../../hassembler/hoasm.h"

#define PRINT_LVM_INSTRUCTIONS 0
#define DUMP_LVM_CODE 0

typedef struct {
    const char* symbol;
    int         symbol_length;
    void*       proc_address;
} External_Symbol;

typedef struct {
    const char* name;
    int         name_length;
    Hoht_Table  symbols;

#if defined(_WIN32) || defined(_WIN64)
    HANDLE lib_handle;
#endif
} External_Lib;

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
    bool absolute;
} Patch_Procs;

typedef enum {
    PATCH_TYPE_IMMEDIATE,
    PATCH_TYPE_DISPLACEMENT,
} Patch_Type;

typedef struct {
    Light_VM_Instruction_Info instr;
    int32_t offset;
} LVM_Instruction_Patch;

typedef struct {
    bool                          short_circuit;
    bool                          short_circuit_if;
    int32_t                       short_circuit_current_true;
    int32_t                       short_circuit_current_false;
    Light_VM_Instruction_Info*    short_circuit_jmps;
} Short_Circuit_Helper;

typedef struct {
    // Current generation state for short circuit
    Short_Circuit_Helper*         short_circuit;

    Patch_Procs*                  proc_patch_calls;
    Light_VM_Instruction_Info*    proc_bases;
    Light_VM_Instruction_Info*    loop_breaks;
    Light_VM_Instruction_Info*    loop_continue;

    s32 release_size_bytes;
    s32 type_table_dataseg_offset;

    Light_VM_DebugInfo* debug_info;

    Hoht_Table external_table;
} LVM_Generator;

// Used to calculate the stack size and current offset
// during code generation of internal scopes.
typedef struct {
    int offset;
    int index;
    int size_bytes;

    int temp_offset;
    LVM_Instruction_Patch* temp_patch;
} Stack_Info;

typedef enum {
    EXPR_RESULT_REG,
    EXPR_RESULT_F64_REG,
    EXPR_RESULT_F32_REG,
} Expr_Result_Type;
typedef struct {
    Expr_Result_Type type;
    s32              size_bytes;
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

static s32 bytesize_to_pw2(s32 b) {
    switch(b) {
        case 1: return 0;
        case 2: return 1;
        case 4: return 2;
        case 8: return 3;
        default: return 0;
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
            instr.binary.bytesize_pw2 = bytesize_to_pw2(left.size_bytes);
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

static bool
return_type_requires_stackspace(Light_Type* ret_type)
{
    ret_type = type_alias_root(ret_type);
    return (ret_type->kind == TYPE_KIND_STRUCT || ret_type->kind == TYPE_KIND_ARRAY || ret_type->kind == TYPE_KIND_UNION);
}

// -------------------------------------------------
// Generation

static Expr_Result lvmgen_expr(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags);
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

static void
lvmg_truncate_reg(Light_VM_State* state, Light_Type* type, LVM_Register reg)
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

static Expr_Result
lvmg_next(Expr_Result r)
{
    r.reg = (r.reg + 1) % FREG_COUNT;
    return r;
}

static void
lvmg_pop_next(Light_VM_State* state, Expr_Result r, Expr_Result* out)
{
    if(r.type == out->type)
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
}

LVM_Register
lvmg_reg_new(Expr_Result r1, Expr_Result r2)
{
    assert(r1.type == r2.type);
    if(r1.type == EXPR_RESULT_REG)
    {
        for(LVM_Register r = R0; r <= R7; ++r)
            if(r != r1.reg && r != r2.reg)
                return r;
    }
    else if(r1.type == EXPR_RESULT_F32_REG)
    {
        for(LVM_F32_Register r = FR0; r <= FR7; ++r)
            if(r != r1.reg && r != r2.reg)
                return r;
    }
    else if(r1.type == EXPR_RESULT_F64_REG)
    {
        for(LVM_F64_Register r = EFR0; r <= EFR7; ++r)
            if(r != r1.reg && r != r2.reg)
                return r;
    }

    Unreachable;
    return R0;
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
lvmg_copy(Light_VM_State* state, LVM_Generator* gen, Expr_Result rvalue, Location location, Light_Type* rvaltype)
{
    rvaltype = type_alias_root(rvaltype);
    
    if(rvalue.type == EXPR_RESULT_REG)
    {
        if(rvaltype->kind == TYPE_KIND_PRIMITIVE || rvaltype->kind == TYPE_KIND_POINTER || rvaltype->kind == TYPE_KIND_FUNCTION)
        {
            if(location.offset < 0)
                light_vm_push_fmt(state, "mov [r%d %s %d], r%d%s", 
                    location.base, "-", -location.offset, rvalue.reg, register_size_suffix(rvaltype->size_bits / BITS_IN_BYTE));
            else
                light_vm_push_fmt(state, "mov [r%d %s %d], r%d%s", 
                    location.base, "+", location.offset, rvalue.reg, register_size_suffix(rvaltype->size_bits / BITS_IN_BYTE));
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

            light_vm_push_fmt(state, "mov r7, %lld", rvaltype->size_bits / BITS_IN_BYTE);
            light_vm_push_fmt(state, "copy r%d, r%d, r7", dst, rvalue.reg);
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
lvmg_deref_auto(Light_VM_State* state, Light_Type* type, LVM_Register ptr_reg)
{
    Expr_Result result = {
        .reg = ptr_reg,
        .size_bytes = LVM_PTRSIZE,
        .type = EXPR_RESULT_REG,
    };
    type = type_alias_root(type);
    if(type->kind == TYPE_KIND_PRIMITIVE || type->kind == TYPE_KIND_POINTER)
    {
        Expr_Result r = lvmg_reg_for_type(type);
        lvmg_deref(state, r, (Location){.base = ptr_reg, .offset = 0});
        return r;
    }
    return result;
}

static void
lvmg_int_to_int_reg(Light_VM_State* state, Light_Type* type, Light_Type* type_to, LVM_Register src, LVM_Register dst)
{
    // Upcast/safe, when signed, sign extend
    if(type_to->size_bits > type->size_bits)
    {
        Light_VM_Instruction instr = {
            .type = LVM_CVT_SEXT,
            .sext.dst_reg = dst,
            .sext.src_reg = src,
            .sext.dst_size = type_to->size_bits / 8,
            .sext.src_size = type->size_bits / 8
        };
        light_vm_push_instruction(state, instr, 0);
    }
    // Downcast/unsafe can lose data
    else if(type_to->size_bits < type->size_bits)
    {
        lvmg_truncate_reg(state, type_to, dst);
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
        switch(type_alias_root(expr->type)->primitive)
        {
            case TYPE_PRIMITIVE_BOOL:
            case TYPE_PRIMITIVE_U8:
            case TYPE_PRIMITIVE_S8: {
                instr.type = LVM_MOV;
                instr.imm_size_bytes = 1;
                instr.binary.bytesize_pw2 = 0;
                instr.binary.dst_reg = result.reg;
                instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u8);                
            } break;
            case TYPE_PRIMITIVE_U16:
            case TYPE_PRIMITIVE_S16: {
                instr.type = LVM_MOV;
                instr.imm_size_bytes = 2;
                instr.binary.bytesize_pw2 = 1;
                instr.binary.dst_reg = result.reg;
                instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u16);                
            } break;
            case TYPE_PRIMITIVE_U32:
            case TYPE_PRIMITIVE_S32: {
                instr.type = LVM_MOV;
                instr.imm_size_bytes = 4;
                instr.binary.bytesize_pw2 = 2;
                instr.binary.dst_reg = result.reg;
                instr.binary.addr_mode = BIN_ADDR_MODE_IMM_TO_REG;
                light_vm_push_instruction(state, instr, expr->expr_literal_primitive.value_u32);                
            } break;
            case TYPE_PRIMITIVE_U64:
            case TYPE_PRIMITIVE_S64: {
                instr.type = LVM_MOV;
                instr.imm_size_bytes = 8;
                instr.binary.bytesize_pw2 = 3;
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
lvmgen_expr_binary_compare(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_BINARY);
    Expr_Result result = {0};

    flags |= EXPR_FLAG_DEREFERENCE;

    Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, stack_info, flags);
    lvmg_push_result(state, left);

    Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, stack_info, flags);
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
lvmgen_expr_binary_arithmetic(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_BINARY);
    Expr_Result result = {0};

    flags |= EXPR_FLAG_DEREFERENCE;

    Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, stack_info, flags);
    lvmg_push_result(state, left);

    Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, stack_info, flags);
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
            case OP_BINARY_PLUS: {
                light_vm_push_fmt(state, "mulu r%d, %d", right.reg, expr->type->pointer_to->size_bits / 8);
                light_vm_push_instruction(state, mk_bin_instr(LVM_ADD_U, left, right), 0);
            } break;
            case OP_BINARY_MINUS: {
                light_vm_push_fmt(state, "mulu r%d, %d", right.reg, expr->type->pointer_to->size_bits / 8);
                light_vm_push_instruction(state, mk_bin_instr(LVM_SUB_U, left, right), 0);
            } break;
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
lvmgen_expr_binary_logical(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    Expr_Result result = {
        .reg = R0,
        .size_bytes = 1,
        .type = EXPR_RESULT_REG,
    };
    assert(type_primitive_bool(expr->expr_binary.left->type) && type_primitive_bool(expr->expr_binary.right->type));

    Short_Circuit_Helper* last_helper = gen->short_circuit;
    Short_Circuit_Helper sc_helper = {0};
    if(!gen->short_circuit || !gen->short_circuit->short_circuit)
    {
        gen->short_circuit = &sc_helper;
    }

    bool sc = gen->short_circuit->short_circuit;
    int prev_false;
    int this_false;
    int prev_true;
    int this_true;
    if(!sc) 
    { 
        gen->short_circuit->short_circuit = true;
        gen->short_circuit->short_circuit_current_true = 1;
        gen->short_circuit->short_circuit_current_false = -1;
        gen->short_circuit->short_circuit_jmps = array_new(Light_VM_Instruction_Info);
        this_true = 1;
        this_false = -1;
    }

    if(flags & EXPR_FLAG_INVERTED_SHORT_CIRCUIT)
    {
        flags &= ~EXPR_FLAG_INVERTED_SHORT_CIRCUIT;
        if(expr->expr_binary.op == OP_BINARY_LOGIC_OR)
        {
            prev_true = gen->short_circuit->short_circuit_current_true++;
            this_true = gen->short_circuit->short_circuit_current_true;

            Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 1", left.reg);
                Light_VM_Instruction_Info info = light_vm_push(state, "beq 0xffffffff");
                info.short_circuit_index = gen->short_circuit->short_circuit_current_false;
                array_push(gen->short_circuit->short_circuit_jmps, info);
            }

            for(int i = array_length(gen->short_circuit->short_circuit_jmps) - 1; i >= 0; --i)
                if(gen->short_circuit->short_circuit_jmps[i].short_circuit_index == this_true)
                {
                    light_vm_patch_from_to_current_instruction(state, gen->short_circuit->short_circuit_jmps[i]);
                    array_remove(gen->short_circuit->short_circuit_jmps, i);
                }
            gen->short_circuit->short_circuit_current_true = prev_true;

            Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 1", right.reg);
                Light_VM_Instruction_Info info2 = light_vm_push(state, "beq 0xffffffff");
                info2.short_circuit_index = gen->short_circuit->short_circuit_current_false;
                array_push(gen->short_circuit->short_circuit_jmps, info2);
            }
        }
        else
        {
            prev_false = gen->short_circuit->short_circuit_current_false--;
            this_false = gen->short_circuit->short_circuit_current_false;

            Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 1", left.reg);
                Light_VM_Instruction_Info info = light_vm_push(state, "bne 0xffffffff");
                info.short_circuit_index = gen->short_circuit->short_circuit_current_true;
                array_push(gen->short_circuit->short_circuit_jmps, info);
            }

            for(int i = array_length(gen->short_circuit->short_circuit_jmps)-1; i >= 0 ; --i)
                if(gen->short_circuit->short_circuit_jmps[i].short_circuit_index == this_false)
                {
                    light_vm_patch_from_to_current_instruction(state, gen->short_circuit->short_circuit_jmps[i]);
                    array_remove(gen->short_circuit->short_circuit_jmps, i);
                }
            gen->short_circuit->short_circuit_current_false = prev_false;

            Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 1", right.reg);
                Light_VM_Instruction_Info info2 = light_vm_push(state, "beq 0xffffffff");
                info2.short_circuit_index = gen->short_circuit->short_circuit_current_false;
                array_push(gen->short_circuit->short_circuit_jmps, info2);
            }
        }
    }
    else
    {
        if(expr->expr_binary.op == OP_BINARY_LOGIC_AND)
        {
            prev_true = gen->short_circuit->short_circuit_current_true++;
            this_true = gen->short_circuit->short_circuit_current_true;

            Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 0", left.reg);
                Light_VM_Instruction_Info info = light_vm_push(state, "beq 0xffffffff");
                info.short_circuit_index = gen->short_circuit->short_circuit_current_false;
                array_push(gen->short_circuit->short_circuit_jmps, info);
            }

            for(int i = array_length(gen->short_circuit->short_circuit_jmps) - 1; i >= 0; --i)
                if(gen->short_circuit->short_circuit_jmps[i].short_circuit_index == this_true)
                {
                    light_vm_patch_from_to_current_instruction(state, gen->short_circuit->short_circuit_jmps[i]);
                    array_remove(gen->short_circuit->short_circuit_jmps, i);
                }
            gen->short_circuit->short_circuit_current_true = prev_true;

            Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 0", right.reg);
                Light_VM_Instruction_Info info2 = light_vm_push(state, "beq 0xffffffff");
                info2.short_circuit_index = gen->short_circuit->short_circuit_current_false;
                array_push(gen->short_circuit->short_circuit_jmps, info2);
            }
        }
        else
        {
            prev_false = gen->short_circuit->short_circuit_current_false--;
            this_false = gen->short_circuit->short_circuit_current_false;

            Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 0", left.reg);
                Light_VM_Instruction_Info info = light_vm_push(state, "bne 0xffffffff");
                info.short_circuit_index = gen->short_circuit->short_circuit_current_true;
                array_push(gen->short_circuit->short_circuit_jmps, info);
            }

            for(int i = array_length(gen->short_circuit->short_circuit_jmps)-1; i >= 0 ; --i)
                if(gen->short_circuit->short_circuit_jmps[i].short_circuit_index == this_false)
                {
                    light_vm_patch_from_to_current_instruction(state, gen->short_circuit->short_circuit_jmps[i]);
                    array_remove(gen->short_circuit->short_circuit_jmps, i);
                }
            gen->short_circuit->short_circuit_current_false = prev_false;

            Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            {
                light_vm_push_fmt(state, "cmp r%db, 0", right.reg);
                Light_VM_Instruction_Info info2 = light_vm_push(state, "beq 0xffffffff");
                info2.short_circuit_index = gen->short_circuit->short_circuit_current_false;
                array_push(gen->short_circuit->short_circuit_jmps, info2);
            }
        }
    }

    if(!gen->short_circuit->short_circuit_if && !sc)
    {
        Light_VM_Instruction_Info end_branch = light_vm_current_instruction(state);
        for(int i = 0; i < array_length(gen->short_circuit->short_circuit_jmps); ++i)
            light_vm_patch_immediate_distance(gen->short_circuit->short_circuit_jmps[i], end_branch);
    }

    if (!sc) { 
        array_free(gen->short_circuit->short_circuit_jmps); 
        gen->short_circuit = 0;
    }

    gen->short_circuit = last_helper;

    return result;
}

static Expr_Result
lvmgen_expr_binary_vector_access(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_BINARY);
    assert(expr->expr_binary.op == OP_BINARY_VECTOR_ACCESS);
    Expr_Result left = lvmgen_expr(state, gen, expr->expr_binary.left, stack_info,  flags|EXPR_FLAG_DEREFERENCE);
    lvmg_push_result(state, left);

    Expr_Result right = lvmgen_expr(state, gen, expr->expr_binary.right, stack_info, flags|EXPR_FLAG_DEREFERENCE);
    lvmg_truncate_reg(state, expr->expr_binary.right->type, right.reg);

    lvmg_pop_next(state, right, &left);

    if(expr->type->size_bits > BITS_IN_BYTE)
    {
        LVM_Register idx = lvmg_reg_new(left, right);
        light_vm_push_fmt(state, "xor r%d, r%d", idx, idx);
        light_vm_push_fmt(state, "mov r%d, %d", idx, expr->type->size_bits / BITS_IN_BYTE);
        light_vm_push_fmt(state, "mulu r%d, r%d", right.reg, idx);
    }
    light_vm_push_fmt(state, "addu r%d, r%d", left.reg, right.reg);

    if (flags & EXPR_FLAG_DEREFERENCE)
    {
        return lvmg_deref_auto(state, expr->type, left.reg);
    }

    return left;
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

static Expr_Result
lvmgen_expr_cast(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_UNARY && expr->expr_unary.op == OP_UNARY_CAST);

    Light_Type* operand_type = type_alias_root(expr->expr_unary.operand->type);
    Light_Type* cast_type = type_alias_root(expr->expr_unary.type_to_cast);

    if(operand_type->kind == TYPE_KIND_ENUM)
    {
        operand_type = operand_type->enumerator.type_hint;
        if(operand_type == 0)
            operand_type = type_primitive_get(TYPE_PRIMITIVE_U32);
    }
    if(cast_type->kind == TYPE_KIND_ENUM)
    {
        cast_type = cast_type->enumerator.type_hint;
        if(cast_type == 0)
            cast_type = type_primitive_get(TYPE_PRIMITIVE_U32);
    }

    Expr_Result result = lvmg_reg_for_type(cast_type);

    if(operand_type != cast_type)
    {
        if(cast_type->kind == TYPE_KIND_PRIMITIVE)
        {
            switch(cast_type->primitive)
            {
                case TYPE_PRIMITIVE_R64:
                case TYPE_PRIMITIVE_R32: {
                    if(operand_type->kind == TYPE_KIND_PRIMITIVE)
                    {
                        switch(operand_type->primitive)
                        {
                            case TYPE_PRIMITIVE_U64: {
                                Expr_Result r = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                                light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_R32) ? "cvtu64r32 fr%d, r%d" : "cvtu64r64 efr%d, r%d", result.freg, r.reg);
                            } break;
                            case TYPE_PRIMITIVE_U32:
                            case TYPE_PRIMITIVE_U16:
                            case TYPE_PRIMITIVE_U8: {
                                Expr_Result r = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                                light_vm_push_fmt(state, "and r0, 0x%x", size_mask(operand_type->size_bits));
                                light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_R32) ? "cvts64r32 fr%d, r%d" : "cvts64r64 efr%d, r%d", result.freg, r.reg);
                            } break;              
                            case TYPE_PRIMITIVE_S64:
                            case TYPE_PRIMITIVE_S32:
                            case TYPE_PRIMITIVE_S16:
                            case TYPE_PRIMITIVE_S8: {
                                Expr_Result r = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                                if((cast_type->primitive == TYPE_PRIMITIVE_R32))
                                {
                                    light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ? 
                                        "cvts64r32 fr%d, r%d" : "cvts32r32 fr%d, r%d", result.freg, r.reg);
                                }
                                else
                                {
                                    light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ? 
                                        "cvts64r64 fr%d, r%d" : "cvts32r64 fr%d, r%d", result.freg, r.reg);
                                }
                            } break;
                            case TYPE_PRIMITIVE_R32: {
                                if(cast_type->primitive == TYPE_PRIMITIVE_R64)
                                {
                                    Expr_Result r = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                                    light_vm_push_fmt(state, "cvtr32r64 efr%d, fr%d", result.efreg, r.freg);
                                }
                            } break;
                            case TYPE_PRIMITIVE_R64: {
                                if(cast_type->primitive == TYPE_PRIMITIVE_R32)
                                {
                                    Expr_Result r = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                                    light_vm_push_fmt(state, "cvtr64r32 fr%d, efr%d", result.freg, r.efreg);
                                }
                            } break;
                            default: Unreachable;
                        }
                    }
                    else
                    {
                        // Cannot cast types other than integer types to floating point
                        Unreachable;
                    }
                } break;
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
                            case TYPE_PRIMITIVE_S8: {
                                Expr_Result r = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                                lvmg_int_to_int_reg(state, operand_type, cast_type, r.reg, result.reg);
                            } break;
                            case TYPE_PRIMITIVE_R32:{
                                // R32 -> S32/S64
                                Expr_Result r = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                                assert(r.type == EXPR_RESULT_F32_REG);
                                light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ? 
                                    "cvtr32s64 r%d, fr%d" : "cvtr32s32 r%d, fr%d", result.reg, r.freg);
                                lvmg_truncate_reg(state, cast_type, result.reg);
                            } break;
                            case TYPE_PRIMITIVE_R64: {
                                // R64 -> S32/S64
                                Expr_Result r = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                                assert(r.type == EXPR_RESULT_F64_REG);
                                light_vm_push_fmt(state, (cast_type->primitive == TYPE_PRIMITIVE_S64 || cast_type->primitive == TYPE_PRIMITIVE_U64) ?
                                    "cvtr64s64 r%d, efr%d" : "cvtr64s32 r%d, efr%d", result.reg, r.efreg);
                                lvmg_truncate_reg(state, cast_type, result.reg);
                            } break;
                            default: Unreachable;
                        }
                    }
                    else if (operand_type->kind == TYPE_KIND_POINTER)
                    {
                        // ptr -> int
                        Expr_Result r = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                        lvmg_int_to_int_reg(state, operand_type, cast_type, r.reg, result.reg);
                    }
                    else
                    {
                        Unreachable;
                    }
                }break;
                default: Unreachable;
            }
        }
        else if (cast_type->kind == TYPE_KIND_POINTER)
        {
            // int -> ptr this is a no-op.
            return lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
        }
        else if (cast_type->kind == TYPE_KIND_FUNCTION)
        {
            return lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
        }
        else if (cast_type->kind == TYPE_KIND_ARRAY)
        {
            return lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags | EXPR_FLAG_DEREFERENCE);
        }
    }
    else
    {
        return lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags);
    }

    return result;
}

static Expr_Result
lvmgen_expr_binary(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
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
            result = lvmgen_expr_binary_arithmetic(state, gen, expr, stack_info, flags);
            break;

        case OP_BINARY_LOGIC_AND:
        case OP_BINARY_LOGIC_OR:
            result = lvmgen_expr_binary_logical(state, gen, expr, stack_info, flags);
            break;

        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE:
        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL:
            result = lvmgen_expr_binary_compare(state, gen, expr, stack_info, flags);
            break;

        case OP_BINARY_VECTOR_ACCESS:
            result = lvmgen_expr_binary_vector_access(state, gen, expr, stack_info, flags);
            break;

        default: Unreachable;
    }
    return result;
}

static Expr_Result
lvmgen_expr_unary(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_UNARY);
    Expr_Result result = {0};

    switch(expr->expr_unary.op)
    {
        case OP_UNARY_PLUS: {
            result = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
        } break;
        case OP_UNARY_MINUS: {
            result = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            Light_VM_Instruction instr = mk_un_instr(type_primitive_float(expr->type) ? LVM_FNEG : LVM_NEG, result);
            light_vm_push_instruction(state, instr, 0);
        } break;
        case OP_UNARY_LOGIC_NOT: {
            assert(type_primitive_bool(expr->type));
            if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_LOGIC_NOT)
            {
                result = lvmgen_expr(state, gen, expr->expr_unary.operand->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            }
            else if(gen->short_circuit && gen->short_circuit->short_circuit && expr->expr_unary.operand->kind == AST_EXPRESSION_BINARY && 
                (expr->expr_unary.operand->expr_binary.op == OP_BINARY_LOGIC_AND || expr->expr_unary.operand->expr_binary.op == OP_BINARY_LOGIC_OR))
            {
                result = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE|EXPR_FLAG_INVERTED_SHORT_CIRCUIT);
            }
            else
            {
                result = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
                light_vm_push_fmt(state, "cmp r%db, 0", result.reg);
                light_vm_push_fmt(state, "moveq r%d", result.reg);
            }
        } break;
        case OP_UNARY_BITWISE_NOT: {
            assert(type_primitive_int(expr->type));
            result = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            Light_VM_Instruction instr = mk_un_instr(LVM_NOT, result);
            light_vm_push_instruction(state, instr, 0);
        } break;
        case OP_UNARY_CAST: {
            result = lvmgen_expr_cast(state, gen, expr, stack_info, flags);
        } break;
        case OP_UNARY_ADDRESSOF: {
            if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_DEREFERENCE)
                result = lvmgen_expr(state, gen, expr->expr_unary.operand->expr_unary.operand, stack_info, flags);
            else
            {
                result = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, flags & ~EXPR_FLAG_DEREFERENCE);
                result.type = EXPR_RESULT_REG;
            }
        } break;
        case OP_UNARY_DEREFERENCE: {
            if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && expr->expr_unary.operand->expr_unary.op == OP_UNARY_ADDRESSOF)
                result = lvmgen_expr(state, gen, expr->expr_unary.operand->expr_unary.operand, stack_info, (flags|EXPR_FLAG_DEREFERENCE) & ~EXPR_FLAG_ASSIGNMENT_ROOT);
            else
                result = lvmgen_expr(state, gen, expr->expr_unary.operand, stack_info, (flags|EXPR_FLAG_DEREFERENCE) & ~EXPR_FLAG_ASSIGNMENT_ROOT);
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
lvmgen_expr_variable(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_VARIABLE);
    Expr_Result result = {0};

    Light_Ast* decl = expr->expr_variable.decl;
    if (decl->kind == AST_DECL_VARIABLE)
    {
        s32 offset = 0;
        LVM_Register base_register = 0;

        if (decl->decl_variable.storage_class == STORAGE_CLASS_STACK)
        {
            offset = expr->expr_variable.decl->decl_variable.stack_offset;
            base_register = LRBP;
        }
        else if (decl->decl_variable.storage_class == STORAGE_CLASS_DATA_SEGMENT)
        {
            offset = expr->expr_variable.decl->decl_variable.datasegment_offset;
            base_register = LRDP;
        }
        else
            Unimplemented;

        Light_Type* expr_type = type_alias_root(expr->type);

        if((flags & EXPR_FLAG_DEREFERENCE) && (expr_type->kind == TYPE_KIND_PRIMITIVE || expr_type->kind == TYPE_KIND_POINTER))
        {
            if(type_primitive_float(expr_type))
            {
                if(type_primitive_float32(expr_type))
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
                    .binary.addr_mode = FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG,
                    .binary.dst_reg = result.reg,
                    .binary.src_reg = base_register,
                    .binary.bytesize_pw2 = 2,
                    .binary.is64bit = (result.type == EXPR_RESULT_F64_REG),
                };
                light_vm_push_instruction(state, instr, offset);
            }
            else
            {
                result.reg = R0;
                result.type = EXPR_RESULT_REG;
                result.size_bytes = expr_type->size_bits / BITS_IN_BYTE;

                Light_VM_Instruction instr = {
                    .type = LVM_MOV,
                    .imm_size_bytes = 4,
                    .binary.bytesize_pw2 = bytesize_to_pw2(expr_type->size_bits / BITS_IN_BYTE),
                    .binary.addr_mode = BIN_ADDR_MODE_REG_OFFSETED_TO_REG,
                    .binary.dst_reg = result.reg,
                    .binary.src_reg = base_register,
                };
                light_vm_push_instruction(state, instr, offset);
            }
        }
        else
        {
            result.reg = R0;
            result.type = EXPR_RESULT_REG;
            result.size_bytes = LVM_PTRSIZE;
            if (base_register == LRBP)
                light_vm_push_fmt(state, "mov r%d, rbp", result.reg);
            else if (base_register == LRDP)
                light_vm_push_fmt(state, "mov r%d, rdp", result.reg);
            else
                Unreachable;
            
            if(offset < 0)
                light_vm_push_fmt(state, "subs r%d, %d", result.reg, -offset);
            else
                light_vm_push_fmt(state, "adds r%d, %d", result.reg, offset);

            Light_Ast* dd = expr->expr_variable.decl;
            Light_Type* tt = type_alias_root(dd->decl_variable.type);
            if ((dd->decl_variable.flags & DECL_VARIABLE_FLAG_PROC_ARGUMENT) && (tt->kind == TYPE_KIND_STRUCT || tt->kind == TYPE_KIND_ARRAY || tt->kind == TYPE_KIND_UNION))
                light_vm_push_fmt(state, "mov r%d, [r%d]", result.reg, result.reg);
            if (tt->kind == TYPE_KIND_FUNCTION && flags & EXPR_FLAG_DEREFERENCE)
                light_vm_push_fmt(state, "mov r%d, [r%d]", result.reg, result.reg);
        }
    }
    else if (decl->kind == AST_DECL_CONSTANT) 
    {
        return lvmgen_expr(state, gen, decl->decl_constant.value, stack_info, flags);
    }
    else if (decl->kind == AST_DECL_PROCEDURE)
    {
        result.reg = R0;
        result.type = EXPR_RESULT_REG;
        result.size_bytes = LVM_PTRSIZE;

        if(decl->decl_proc.lvm_base_instruction)
            light_vm_push_fmt(state, "mov r%d, 0x%llx", result.reg, (uint64_t)decl->decl_proc.lvm_base_instruction);
        else
        {
            Light_VM_Instruction_Info call_instr = light_vm_push_fmt(state, "mov r%d, 0xffffffffffffffff", result.reg);
            Patch_Procs pp = {
                .from = call_instr,
                .to_decl = decl,
                .absolute = true
            };
            array_push(gen->proc_patch_calls, pp);
        }
    }
    else
    {
        Unimplemented;
    }

    return result;
}

static Expr_Result
lvmgen_expr_proc_call(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_PROCEDURE_CALL);

    if(expr->expr_proc_call.caller_expr->type->function.flags & TYPE_FUNCTION_STDCALL)
    {
        if(expr->expr_proc_call.caller_expr->kind == AST_EXPRESSION_VARIABLE)
        {
            Light_Ast* decl = expr->expr_proc_call.caller_expr->expr_variable.decl;
            if (decl->kind == AST_DECL_PROCEDURE && decl->decl_proc.extern_library_name)
            {
                Light_Token* token = decl->decl_proc.extern_library_name;

                u64 lib_hash = fnv_1_hash(token->raw_data, token->raw_data_length);
                External_Lib* lib = hoht_get_value_hashed(&gen->external_table, lib_hash);
                if(!lib)
                {
                    void* lname = calloc(1, token->raw_data_length + 1);
                    memcpy(lname, token->raw_data, token->raw_data_length);
                    External_Lib nlib = {
                        .name = token->raw_data,
                        .name_length = token->raw_data_length,
                        #if defined(_WIN32) || defined(_WIN64)
                        .lib_handle = LoadLibraryA(lname)
                        #endif
                    };
                    free(lname);
                    hoht_new(&nlib.symbols, 32, sizeof(External_Symbol), 0.5f, malloc, free);
                    int idx = hoht_push_hashed(&gen->external_table, lib_hash, &nlib);
                    lib = hoht_get_value_from_index(&gen->external_table, idx);
                }

                u64 symbol_hash = fnv_1_hash(decl->decl_proc.name->data, decl->decl_proc.name->length);
                External_Symbol* symbol = hoht_get_value_hashed(&lib->symbols, symbol_hash);
                if(!symbol)
                {
                    void* s = calloc(1, decl->decl_proc.name->length + 1);
                    memcpy(s, decl->decl_proc.name->data, decl->decl_proc.name->length);
                    External_Symbol sym = {
                        .symbol = decl->decl_proc.name->data,
                        .symbol_length = decl->decl_proc.name->length,
                        #if defined(_WIN32) || defined(_WIN64)
                        .proc_address = GetProcAddress(lib->lib_handle, s),
                        #endif
                    };
                    free(s);
                    int idx = hoht_push_hashed(&lib->symbols, symbol_hash, &sym);
                    symbol = hoht_get_value_from_index(&lib->symbols, idx);
                }

                s32 start_release = gen->release_size_bytes;
                s32 arg_size = 0;
                for(int i = 0; i < expr->expr_proc_call.arg_count; ++i)
                {
                    Light_Ast* arg = expr->expr_proc_call.args[i];
                    Expr_Result res = lvmgen_expr(state, gen, arg, stack_info, EXPR_FLAG_DEREFERENCE);

                    if(res.type == EXPR_RESULT_REG)
                        light_vm_push_fmt(state, "expushi r%d%s", res.reg, register_size_suffix(res.size_bytes));
                    else if(res.type == EXPR_RESULT_F32_REG)
                        light_vm_push_fmt(state, "expushf fr%d", res.freg);
                    else if(res.type == EXPR_RESULT_F64_REG)
                        light_vm_push_fmt(state, "expushf efr%d", res.efreg);
                }
            
                light_vm_push_fmt(state, "mov r2, 0x%llx", symbol->proc_address);
                light_vm_push(state, "extcall r2");

                for(int i = expr->expr_proc_call.arg_count-1; i >= 0; --i)
                    light_vm_push(state, "expop");

                light_vm_push_fmt(state, "adds rsp, %d", gen->release_size_bytes - start_release);
                gen->release_size_bytes = start_release;
            }
            else if(decl->kind == AST_DECL_VARIABLE || (decl->kind == AST_DECL_PROCEDURE && !decl->decl_proc.extern_library_name))
            {
                s32 start_release = gen->release_size_bytes;

                for (int i = 0; i < expr->expr_proc_call.arg_count; ++i)
                {
                    Light_Ast* arg = expr->expr_proc_call.args[i];
                    Expr_Result res = lvmgen_expr(state, gen, arg, stack_info, EXPR_FLAG_DEREFERENCE);

                    if (res.type == EXPR_RESULT_REG)
                        light_vm_push_fmt(state, "expushi r%d%s", res.reg, register_size_suffix(res.size_bytes));
                    else if (res.type == EXPR_RESULT_F32_REG)
                        light_vm_push_fmt(state, "expushf fr%d", res.freg);
                    else if (res.type == EXPR_RESULT_F64_REG)
                        light_vm_push_fmt(state, "expushf efr%d", res.efreg);
                }

                Expr_Result caller = lvmgen_expr(state, gen, expr->expr_proc_call.caller_expr, stack_info, EXPR_FLAG_DEREFERENCE);
                light_vm_push_fmt(state, "extcall r%d", caller.reg);

                for (int i = expr->expr_proc_call.arg_count - 1; i >= 0; --i)
                    light_vm_push(state, "expop");

                light_vm_push_fmt(state, "adds rsp, %d", gen->release_size_bytes - start_release);
                gen->release_size_bytes = start_release;
            }
        }
        else
        {
            Unimplemented;
        }
    }
    else
    {
        s32 arg_size = 0;

        for(int i = expr->expr_proc_call.arg_count-1; i >= 0; --i)
        {
            Light_Ast* arg = expr->expr_proc_call.args[i];
            Expr_Result res = lvmgen_expr(state, gen, arg, stack_info, EXPR_FLAG_DEREFERENCE);
            lvmg_push_result(state, res);
            arg_size += (LVM_PTRSIZE + gen->release_size_bytes);
        }

        Light_Type* return_type = type_alias_root(expr->type);
        if(return_type_requires_stackspace(return_type))
        {
            light_vm_push_fmt(state, "subs rsp, %d", return_type->size_bits / BITS_IN_BYTE);
            arg_size += (return_type->size_bits / BITS_IN_BYTE);
        }
        
        // Evaluate and generate the call instruction
        Light_Ast* caller = expr->expr_proc_call.caller_expr;
        if(caller->kind == AST_EXPRESSION_VARIABLE && caller->expr_variable.decl->kind == AST_DECL_PROCEDURE)
        {
            Light_VM_Instruction_Info call_instr = light_vm_push(state, "call 0xffffffff");
            Patch_Procs pp = {
                .from = call_instr,
                .to_decl = caller->expr_variable.decl
            };
            array_push(gen->proc_patch_calls, pp);
        }
        else
        {
            Expr_Result r = lvmgen_expr(state, gen, expr->expr_proc_call.caller_expr, stack_info, EXPR_FLAG_DEREFERENCE);
            assert(r.type == EXPR_RESULT_REG);
            light_vm_push_fmt(state, "call r%d", r.reg);
        }

        // restore the arguments space
        light_vm_push_fmt(state, "adds rsp, %d", arg_size);
        gen->release_size_bytes = 0;
    }

    return lvmg_reg_for_type(expr->type);
}

#include "../../symbol_table.h"

static Light_Ast*
decl_from_name(Light_Scope* scope, Light_Token* name) {
    Light_Symbol s = {0};
    s.token = name;

    Symbol_Table* symbol_table = (Symbol_Table*)scope->symb_table;

    if(!symbol_table) return 0;

    s32 index = 0;
    if(symbol_table_entry_exist(symbol_table, s, &index, 0)) {
        return symbol_table->entries[index].data.decl;
    }
    return 0;
}

// Given an integer register and a dot field accessing expression of a struct or union
// retreives the address of that struct's data in memory in the register.
// Performs dereferencing in case the left side of the expression is a pointer type.
// Example: value.x
static Expr_Result
lvmgen_expr_dot(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    Light_Type* left_type = type_alias_root(expr->expr_dot.left->type);

    if (left_type->kind == TYPE_KIND_ENUM)
    {
        Light_Ast* c = decl_from_name(left_type->enumerator.enum_scope, expr->expr_dot.identifier);
        assert(c->kind == AST_DECL_CONSTANT);
        //emit_expression(literal_decls, buffer, c->decl_constant.value);
        Expr_Result res = lvmgen_expr(state, gen, c->decl_constant.value, stack_info, EXPR_FLAG_DEREFERENCE);
        return res;
    }
    else
    {
        u32 f = (flags & ~EXPR_FLAG_DEREFERENCE) | ((left_type->kind == TYPE_KIND_POINTER) ? EXPR_FLAG_DEREFERENCE : 0);
        Expr_Result res = lvmgen_expr(state, gen, expr->expr_dot.left, stack_info, f);

        if (left_type->kind == TYPE_KIND_POINTER)
        {
            if (type_alias_root(left_type->pointer_to)->kind == TYPE_KIND_STRUCT)
            {
                s64 offset = type_struct_field_offset_bits(left_type->pointer_to, expr->expr_dot.identifier->data) / BITS_IN_BYTE;
                light_vm_push_fmt(state, "adds r%d, %d", res.reg, (s32)offset);
            }
            else
            {
                // Offset of an union is always 0, therefore no need to add anything
                assert(type_alias_root(left_type->pointer_to)->kind == TYPE_KIND_UNION);
            }
        }
        else if (left_type->kind == TYPE_KIND_STRUCT)
        {
            s64 offset = type_struct_field_offset_bits(expr->expr_dot.left->type, expr->expr_dot.identifier->data) / BITS_IN_BYTE;
            light_vm_push_fmt(state, "adds r%d, %d", res.reg, (s32)offset);
        }
        else
        {
            // Offset of an union is always 0, therefore no need to add anything
            assert(left_type->kind == TYPE_KIND_UNION);
        }

        if (flags & EXPR_FLAG_DEREFERENCE)
        {
            return lvmg_deref_auto(state, expr->type, res.reg);
        }

        return res;
    }
}

static Expr_Result
lvmgen_expr_literal_array(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_LITERAL_ARRAY);

    Expr_Result result = {0};

    if(expr->expr_literal_array.raw_data)
    {
        light_vm_push(state, "mov r0, rdp");
        light_vm_push_fmt(state, "adds r0, %d", state->data_offset);

        light_vm_push_bytes_data_segment(state, expr->expr_literal_array.data, expr->expr_literal_array.data_length_bytes);

        result = (Expr_Result){
            .reg = R0,
            .size_bytes = LVM_PTRSIZE,
            .type = EXPR_RESULT_REG,
        };
    }
    else if(expr->expr_literal_array.array_exprs)
    {
        stack_info->temp_offset += (expr->type->size_bits / BITS_IN_BYTE);
        stack_info->size_bytes += (expr->type->size_bits / BITS_IN_BYTE);
        s32 offset = -stack_info->temp_offset;
        expr->expr_literal_array.stack_offset = -offset;

        LVM_Register res_reg = {0};

        int64_t off = 0;
        for(int i = 0; i < array_length(expr->expr_literal_array.array_exprs); ++i)
        {
            Light_Ast* inexpr = expr->expr_literal_array.array_exprs[i];
            Light_Type* type = type_alias_root(inexpr->type);
            Expr_Result r = lvmgen_expr(state, gen, inexpr, stack_info, flags|EXPR_FLAG_DEREFERENCE);
            res_reg = lvmg_reg_new(r, r);

            light_vm_push_fmt(state, "mov r%d, rbp", res_reg);
            Light_VM_Instruction_Info info = light_vm_push_fmt(state, "subs r%d, 0xffffffff", res_reg);
            LVM_Instruction_Patch p = { .instr = info, .offset = offset };
            array_push(stack_info->temp_patch, p);
            lvmg_copy(state, gen, r, (Location){.base = res_reg, .offset = off }, inexpr->type);

            off += type->size_bits / BITS_IN_BYTE;
        }

        result = (Expr_Result){
            .reg = res_reg,
            .size_bytes = LVM_PTRSIZE,
            .type = EXPR_RESULT_REG,
        };
    }
    else
    {
        Unreachable;
    }

    return result;
}

typedef struct {
    u32 cap; 
    u32 length; 
    u8* data;
} lvm_string;

typedef struct {
    u32 kind;
    u32 flags;
    s64 type_size_bytes;
    union {
        u32   primitive;
        void* pointer_to_type_info;
        struct LVM_User_Type_Array {
            void* array_of_type_info;
            u64   dimension;
        } LVM_User_Type_Array;
        struct LVM_User_Type_Struct {
            void** fields_types_type_info;
            void* fields_names;
            s64* fields_offsets_bits;
            s32 fields_count;
            s32 alignment;
        } LVM_User_Type_Struct;
        struct LVM_User_Type_Union {
            void** fields_types_type_info;
            void* fields_names;
            s32 fields_count;
            s32 alignment;
        } LVM_User_Type_Union;
        struct LVM_User_Type_Function {
            void* return_type_type_info;
            void** arguments_type;
            void* arguments_name;
            s32 arguments_count;
        } LVM_User_Type_Function;
        struct LVM_User_Type_Alias {
            lvm_string name;
            void* alias_to_type_info;
        } LVM_User_Type_Alias;
    };
} LVM_User_Type_Info;

static void
lvm_emit_type_table(Light_VM_State* state, LVM_Generator* gen, Light_Type** type_table)
{
    s32 emit_count = 0;

    gen->type_table_dataseg_offset = state->data_offset;
    LVM_User_Type_Info* at = (LVM_User_Type_Info*)((char*)state->data.block + gen->type_table_dataseg_offset);
    LVM_User_Type_Info* type_table_start = at;

    for (int i = 0; i < array_length(type_table); ++i)
    {
        Light_Type* type = type_table[i];
        type->type_table_index = i;        

        at->kind = type->kind;
        at->flags = type->flags;
        at->type_size_bytes = type->size_bits / BITS_IN_BYTE;

        switch(type->kind) {
            case TYPE_KIND_PRIMITIVE: {
                at->primitive = type->primitive;
            } break;
            case TYPE_KIND_POINTER: {
                at->pointer_to_type_info = 0;                
            } break;
            case TYPE_KIND_ALIAS: {
                at->LVM_User_Type_Alias.name.cap = 0;
                at->LVM_User_Type_Alias.name.length = type->alias.name->length;
            } break;
            case TYPE_KIND_STRUCT: {
                at->LVM_User_Type_Struct.alignment = type->struct_info.alignment_bytes;
                at->LVM_User_Type_Struct.fields_count = type->struct_info.fields_count;
            } break;
            case TYPE_KIND_ARRAY: {
                at->LVM_User_Type_Array.dimension = type->array_info.dimension;
            } break;
            case TYPE_KIND_FUNCTION: {
                at->LVM_User_Type_Function.arguments_count = type->function.arguments_count;
            } break;
            default: break;
        }
        at++;
        emit_count++;
    }
    state->data_offset += (emit_count * sizeof(LVM_User_Type_Info));

    u8* scratch = (u8*)at;
    // patch all extra data
    for (int i = 0; i < array_length(type_table); ++i)
    {
        Light_Type* type = type_table[i];

        switch(type->kind) {
            case TYPE_KIND_POINTER: {
                type_table_start[i].pointer_to_type_info = type_table_start + type->pointer_to->type_table_index;
            } break;
            case TYPE_KIND_ALIAS: {
                type_table_start[i].LVM_User_Type_Alias.alias_to_type_info = type_table_start + type->alias.alias_to->type_table_index;
                
                memcpy(scratch, type->alias.name->data, type->alias.name->length);
                state->data_offset += type->alias.name->length;
                type_table_start[i].LVM_User_Type_Alias.name.data = scratch;
                scratch += type->alias.name->length;
            } break;
            case TYPE_KIND_STRUCT: {
                // Field names
                type_table_start[i].LVM_User_Type_Struct.fields_names = scratch;
                u8* start = scratch;
                for(int f = 0; f < type->struct_info.fields_count; ++f)
                {
                    lvm_string* ss = (lvm_string*)scratch;
                    ss->cap = 0;
                    ss->length = type->struct_info.fields[f]->decl_variable.name->length;
                    scratch += sizeof(lvm_string);
                }
                for(int f = 0; f < type->struct_info.fields_count; ++f)
                {
                    lvm_string* ss = (lvm_string*)scratch;
                    Light_Token* name = type->struct_info.fields[f]->decl_variable.name;
                    memcpy(scratch, name->data, name->length);
                    ((lvm_string*)start)[f].data = scratch;
                    scratch += name->length;
                }

                // Offsets
                type_table_start[i].LVM_User_Type_Struct.fields_offsets_bits = (s64*)scratch;
                for(int f = 0; f < type->struct_info.fields_count; ++f)
                {
                    *(s64*)scratch = type->struct_info.offset_bits[f];
                    scratch += sizeof(s64);
                }

                // Fields type infos
                type_table_start[i].LVM_User_Type_Struct.fields_types_type_info = (LVM_User_Type_Info**)scratch;
                for(int f = 0; f < type->struct_info.fields_count; ++f)
                {
                    *(LVM_User_Type_Info**)scratch = type_table_start + type->struct_info.fields[f]->decl_variable.type->type_table_index;
                    scratch += sizeof(LVM_User_Type_Info*);
                }

                state->data_offset += (scratch - start);
            } break;
            case TYPE_KIND_ARRAY: {
                type_table_start[i].LVM_User_Type_Array.array_of_type_info = type_table_start + type->array_info.array_of->type_table_index;
            } break;
            case TYPE_KIND_FUNCTION: {
                type_table_start[i].LVM_User_Type_Function.return_type_type_info = type_table_start + type->function.return_type->type_table_index;
                u8* start = scratch;
                type_table_start[i].LVM_User_Type_Function.arguments_type = (LVM_User_Type_Info**)scratch;
                for(int f = 0; f < type->function.arguments_count; ++f)
                {
                    *(LVM_User_Type_Info**)scratch = type_table_start + type->function.arguments_type[f]->type_table_index;
                    scratch += sizeof(LVM_User_Type_Info*);
                }
                state->data_offset += (scratch - start);
            } break;
            default: break;
        }        
    }
}

static Expr_Result
lvmgen_expr_compiler_gen(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    if (expr->expr_compiler_generated.kind == COMPILER_GENERATED_POINTER_TO_TYPE_INFO && expr->expr_compiler_generated.type_value)
    {
        Light_Type* type = expr->expr_compiler_generated.type_value;
        s32 table_index = type->type_table_index;

        light_vm_push(state, "mov r0, rdp");
        light_vm_push_fmt(state, "adds r0, %d", gen->type_table_dataseg_offset + table_index * sizeof(LVM_User_Type_Info));
    }
    else    
    {
        light_vm_push(state, "mov r0, 0");
    }

    Expr_Result result = { 0 };
    result.type = EXPR_RESULT_REG;
    result.reg = R0;
    result.size_bytes = LVM_PTRSIZE;

    return result;
}


static Expr_Result
lvmgen_expr_literal_struct(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_LITERAL_STRUCT);

    stack_info->temp_offset += (expr->type->size_bits / BITS_IN_BYTE);
    stack_info->size_bytes += (expr->type->size_bits / BITS_IN_BYTE);
    s32 offset = -stack_info->temp_offset;
    expr->expr_literal_array.stack_offset = -offset;

    LVM_Register res_reg = {0};

    if(!expr->expr_literal_struct.named)
    {
        Light_Type* type = type_alias_root(expr->type);
        for(int i = 0; i < type->struct_info.fields_count; ++i)
        {
            Light_Ast* inexpr = expr->expr_literal_struct.struct_exprs[i];
            Expr_Result r = lvmgen_expr(state, gen, inexpr, stack_info, flags|EXPR_FLAG_DEREFERENCE);

            int64_t off = type->struct_info.offset_bits[i] / BITS_IN_BYTE;
            res_reg = lvmg_reg_new(r, r);

            light_vm_push_fmt(state, "mov r%d, rbp", res_reg);
            Light_VM_Instruction_Info info = light_vm_push_fmt(state, "subs r%d, 0x7fffffff", res_reg);
            LVM_Instruction_Patch p = { .instr = info, .offset = offset };
            array_push(stack_info->temp_patch, p);
            
            lvmg_copy(state, gen, r, (Location){.base = res_reg, .offset = off }, inexpr->type);
        }
    }
    else
    {
        Unimplemented;
    }

    Expr_Result result = {
        .reg = res_reg,
        .size_bytes = LVM_PTRSIZE,
        .type = EXPR_RESULT_REG,
    };

    return result;
}

static void
push_lexical_range(LVM_Generator* gen, uint64_t code_offset, Lexical_Range* lrange)
{
    if(lrange->start && lrange->end)
    {
        Light_VM_DebugInfo info = {
            .address_offset = code_offset,
            .lexical_start = lrange->start->original_data,
            .lexical_length = (int)(lrange->end->original_data - lrange->start->original_data + lrange->end->length),
        };
        if (!info.lexical_start)
            info.lexical_start = lrange->start->data;
        array_push(gen->debug_info, info);
    }
}

static Expr_Result
lvmgen_expr(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, Stack_Info* stack_info, u32 flags)
{
    push_lexical_range(gen, state->code_offset, &expr->lexical_range);
    Expr_Result result = {0};
    switch(expr->kind)
    {
        case AST_EXPRESSION_LITERAL_PRIMITIVE:  result = lvmgen_expr_literal_primitive(state, gen, expr, flags); break;
        case AST_EXPRESSION_BINARY:             result = lvmgen_expr_binary(state, gen, expr, stack_info, flags); break;
        case AST_EXPRESSION_UNARY:              result = lvmgen_expr_unary(state, gen, expr, stack_info, flags); break;
        case AST_EXPRESSION_VARIABLE:           result = lvmgen_expr_variable(state, gen, expr, stack_info, flags); break;
        case AST_EXPRESSION_PROCEDURE_CALL:     result = lvmgen_expr_proc_call(state, gen, expr, stack_info, flags); break;
        case AST_EXPRESSION_DOT:                result = lvmgen_expr_dot(state, gen, expr, stack_info, flags); break;
        case AST_EXPRESSION_LITERAL_ARRAY:      result = lvmgen_expr_literal_array(state, gen, expr, stack_info, flags); break;
        case AST_EXPRESSION_LITERAL_STRUCT:     result = lvmgen_expr_literal_struct(state, gen, expr, stack_info, flags); break;
        case AST_EXPRESSION_COMPILER_GENERATED: result = lvmgen_expr_compiler_gen(state, gen, expr, flags); break;
        default: Unreachable;
    }
    return result;
}

static void
lvmgen_decl_variable(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* decl)
{
    if(decl->decl_variable.storage_class == STORAGE_CLASS_STACK)
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
                Expr_Result right_res = lvmgen_expr(state, gen, decl->decl_variable.assignment, stack_info, EXPR_FLAG_DEREFERENCE);                
                lvmg_copy(state, gen, right_res, (Location){ .base = LRBP, .offset = decl->decl_variable.stack_offset}, decl->decl_variable.type);                
            }
        }
    }
    else if(decl->decl_variable.storage_class == STORAGE_CLASS_DATA_SEGMENT)
    {
        Light_Type* type = decl->decl_variable.type;
        decl->decl_variable.datasegment_offset = state->data_offset;
        state->data_offset += (type->size_bits / BITS_IN_BYTE);
        decl->decl_variable.flags |= DECL_VARIABLE_FLAG_ADDR_CALCULATED;

        if(decl->decl_variable.assignment)
        {
            Expr_Result right_res = lvmgen_expr(state, gen, decl->decl_variable.assignment, stack_info, EXPR_FLAG_DEREFERENCE);
            lvmg_copy(state, gen, right_res, (Location) { .base = LRDP, .offset = decl->decl_variable.datasegment_offset }, decl->decl_variable.type);
        }
    }
    else
    {
        Unreachable;
    }
}

static void
lvmgen_comm_assignment(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    Expr_Result right_res = lvmgen_expr(state, gen, comm->comm_assignment.rvalue, stack_info, EXPR_FLAG_DEREFERENCE);

    if(comm->comm_assignment.lvalue)
    {
        lvmg_push_result(state, right_res);
        Expr_Result left_res = lvmgen_expr(state, gen, comm->comm_assignment.lvalue, stack_info, EXPR_FLAG_ASSIGNMENT_ROOT);
        assert(left_res.type == EXPR_RESULT_REG);
        lvmg_pop_next(state, left_res, &right_res);

        lvmg_copy(state, gen, right_res, (Location){.base = left_res.reg, .offset = 0}, comm->comm_assignment.lvalue->type);
    }
}

void
lvmgen_comm_if(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    Short_Circuit_Helper* last_helper = gen->short_circuit;
    Short_Circuit_Helper sc_helper = {
        .short_circuit = true,
        .short_circuit_if = true,
        .short_circuit_current_true = 1,
        .short_circuit_current_false = -1,
        .short_circuit_jmps = array_new(Light_VM_Instruction_Info),
    };
    gen->short_circuit = &sc_helper;

    Expr_Result res = lvmgen_expr(state, gen, comm->comm_if.condition, stack_info, EXPR_FLAG_DEREFERENCE);
    assert(res.type == EXPR_RESULT_REG);
    sc_helper.short_circuit = false;
    sc_helper.short_circuit_if = false;

    // If there is nothing in the array, that means no AND/OR were generated,
    // in this case, just evaluate the final result and branch according to that.
    // We can't do this by branching inside because there can ban casts to bool.
    Light_VM_Instruction_Info base_case = {0};
    bool no_short_circuit = array_length(sc_helper.short_circuit_jmps) == 0;
    if(no_short_circuit)
    {
        light_vm_push_fmt(state, "cmp r%db, 0", res.reg);
        base_case = light_vm_push(state, "beq 0xffffffff");
    }

    for(int i = 0; i < array_length(sc_helper.short_circuit_jmps); ++i)
        if (sc_helper.short_circuit_jmps[i].short_circuit_index == 1)
            light_vm_patch_from_to_current_instruction(state, gen->short_circuit->short_circuit_jmps[i]);

    lvmgen_command(state, gen, stack_info, comm->comm_if.body_true);
    Light_VM_Instruction_Info skip_true = light_vm_push(state, "jmp 0xffffffff");

    Light_VM_Instruction_Info else_block = light_vm_current_instruction(state);

    for (int i = 0; i < array_length(sc_helper.short_circuit_jmps); ++i)
        if (sc_helper.short_circuit_jmps[i].short_circuit_index == -1)
            light_vm_patch_immediate_distance(sc_helper.short_circuit_jmps[i], else_block);

    if(no_short_circuit)
    {
        light_vm_patch_immediate_distance(base_case, else_block);
    }
    if(comm->comm_if.body_false)
    {
        lvmgen_command(state, gen, stack_info, comm->comm_if.body_false);
    }    

    array_free(sc_helper.short_circuit_jmps);
    sc_helper.short_circuit_jmps = 0;
    gen->short_circuit = last_helper;
    light_vm_patch_from_to_current_instruction(state, skip_true);
}

static void
lvmgen_comm_while(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    Short_Circuit_Helper* last_helper = gen->short_circuit;
    Short_Circuit_Helper sc_helper = {
        .short_circuit = true,
        .short_circuit_if = false,
        .short_circuit_current_true = 1,
        .short_circuit_current_false = -1,
        .short_circuit_jmps = array_new(Light_VM_Instruction_Info),
    };
    gen->short_circuit = &sc_helper;

    Light_VM_Instruction_Info start = light_vm_current_instruction(state);
    Expr_Result res = lvmgen_expr(state, gen, comm->comm_while.condition, stack_info, EXPR_FLAG_DEREFERENCE);
    assert(res.type == EXPR_RESULT_REG);
    sc_helper.short_circuit = false;
    sc_helper.short_circuit_if = false;

    // If there is nothing in the array, that means no AND/OR were generated,
    // in this case, just evaluate the final result and branch according to that.
    // We can't do this by branching inside because there can ban casts to bool.
    Light_VM_Instruction_Info base_case = {0};
    bool no_short_circuit = array_length(sc_helper.short_circuit_jmps) == 0;
    if(no_short_circuit)
    {
        light_vm_push(state, "cmp r0, 0");
        base_case = light_vm_push(state, "beq 0xffffffff");
    }

    for(int i = 0; i < array_length(sc_helper.short_circuit_jmps); ++i)
        if (sc_helper.short_circuit_jmps[i].short_circuit_index == 1)
            light_vm_patch_from_to_current_instruction(state, sc_helper.short_circuit_jmps[i]);

    lvmgen_command(state, gen, stack_info, comm->comm_while.body);

    // Patch continue
    for(int i = array_length(gen->loop_continue) - 1; i >= 0; --i)
    {
        if(gen->loop_continue[i].break_continue_level == 1)
        {
            light_vm_patch_from_to_current_instruction(state, gen->loop_continue[i]);
            array_remove(gen->loop_continue, i);
        }
        else
        {
            gen->loop_continue[i].break_continue_level--;
        }
    }

    // Loop back to start at the end
    Light_VM_Instruction_Info loop_back = light_vm_push(state, "jmp 0xffffffff");
    light_vm_patch_immediate_distance(loop_back, start);

    // Outside the loop patch what it still needs to patch
    Light_VM_Instruction_Info while_end = light_vm_current_instruction(state);
    if(no_short_circuit)
        light_vm_patch_immediate_distance(base_case, while_end);
    for(int i = 0; i < array_length(sc_helper.short_circuit_jmps); ++i)
        if (sc_helper.short_circuit_jmps[i].short_circuit_index == -1)
            light_vm_patch_immediate_distance(sc_helper.short_circuit_jmps[i], while_end);
    //array_free(sc_helper.short_circuit_jmps);
    sc_helper.short_circuit_jmps = 0;
    gen->short_circuit = last_helper;

    // Patch breaks
    for(int i = array_length(gen->loop_breaks) - 1; i >= 0; --i)
    {
        if(gen->loop_breaks[i].break_continue_level == 1)
        {
            light_vm_patch_from_to_current_instruction(state, gen->loop_breaks[i]);
            array_remove(gen->loop_breaks, i);
        }
        else
        {
            gen->loop_breaks[i].break_continue_level--;
        }
    }
}

static void
lvmgen_comm_for(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    // Prologue
    if (comm->comm_for.prologue)
    {
        for(int i = 0; i < array_length(comm->comm_for.prologue); ++i)
            lvmgen_command(state, gen, stack_info, comm->comm_for.prologue[i]);
    }
    
    Short_Circuit_Helper* last_helper = gen->short_circuit;
    Short_Circuit_Helper sc_helper = {
        .short_circuit = true,
        .short_circuit_if = false,
        .short_circuit_current_true = 1,
        .short_circuit_current_false = -1,
        .short_circuit_jmps = array_new(Light_VM_Instruction_Info),
    };
    gen->short_circuit = &sc_helper;

    Light_VM_Instruction_Info start = light_vm_current_instruction(state);
    Expr_Result res = lvmgen_expr(state, gen, comm->comm_for.condition, stack_info, EXPR_FLAG_DEREFERENCE);
    sc_helper.short_circuit = false;

    // If there is nothing in the array, that means no AND/OR were generated,
    // in this case, just evaluate the final result and branch according to that.
    // We can't do this by branching inside because there can ban casts to bool.
    Light_VM_Instruction_Info base_case = {0};
    bool no_short_circuit = array_length(sc_helper.short_circuit_jmps) == 0;
    if(no_short_circuit)
    {
        light_vm_push(state, "cmp r0, 0");
        base_case = light_vm_push(state, "beq 0xffffffff");
    }

    for(int i = 0; i < array_length(sc_helper.short_circuit_jmps); ++i)
        if (sc_helper.short_circuit_jmps[i].short_circuit_index == 1)
            light_vm_patch_from_to_current_instruction(state, sc_helper.short_circuit_jmps[i]);

    lvmgen_command(state, gen, stack_info, comm->comm_for.body);

    // Patch continue
    for(int i = array_length(gen->loop_continue) - 1; i >= 0; --i)
    {
        if(gen->loop_continue[i].break_continue_level == 1)
        {
            light_vm_patch_from_to_current_instruction(state, gen->loop_continue[i]);
            array_remove(gen->loop_continue, i);
        }
        else
        {
            gen->loop_continue[i].break_continue_level--;
        }
    }

    // Epilogue
    for(int i = 0; i < array_length(comm->comm_for.epilogue); ++i)
        lvmgen_command(state, gen, stack_info, comm->comm_for.epilogue[i]);

    // Loop back to start at the end
    Light_VM_Instruction_Info loop_back = light_vm_push(state, "jmp 0xffffffff");
    light_vm_patch_immediate_distance(loop_back, start);

    // Outside the loop patch what it still needs to patch
    Light_VM_Instruction_Info while_end = light_vm_current_instruction(state);
    if(no_short_circuit)
        light_vm_patch_immediate_distance(base_case, while_end);
    for(int i = 0; i < array_length(sc_helper.short_circuit_jmps); ++i)
        if (sc_helper.short_circuit_jmps[i].short_circuit_index == -1)
            light_vm_patch_immediate_distance(sc_helper.short_circuit_jmps[i], while_end);

    array_free(sc_helper.short_circuit_jmps);
    sc_helper.short_circuit_jmps = 0;
    gen->short_circuit = last_helper;

    // Patch breaks
    for(int i = array_length(gen->loop_breaks) - 1; i >= 0; --i)
    {
        if(gen->loop_breaks[i].break_continue_level == 1)
        {
            light_vm_patch_from_to_current_instruction(state, gen->loop_breaks[i]);
            array_remove(gen->loop_breaks, i);
        }
        else
        {
            gen->loop_breaks[i].break_continue_level--;
        }
    }
}

static void
lvmgen_comm_break(Light_VM_State* state, LVM_Generator* gen, Light_Ast* comm)
{
    Light_VM_Instruction_Info info = light_vm_push(state, "jmp 0xffffffff");
    info.break_continue_level = (s32)comm->comm_break.level_value;
    array_push(gen->loop_breaks, info);
}

static void
lvmgen_comm_continue(Light_VM_State* state, LVM_Generator* gen, Light_Ast* comm)
{
    Light_VM_Instruction_Info info = light_vm_push(state, "jmp 0xffffffff");
    info.break_continue_level = (s32)comm->comm_break.level_value;
    array_push(gen->loop_continue, info);
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
        Expr_Result r = lvmgen_expr(state, gen, expr, stack_info, EXPR_FLAG_DEREFERENCE);
        if(return_type_requires_stackspace(expr->type))
        {
            lvmg_copy(state, gen, r, (Location){.base = LRBP, .offset = 2 * LVM_PTRSIZE }, expr->type);
            light_vm_push_fmt(state, "mov r%d, rbp", LVM_RETURN_REGISTER);
            light_vm_push_fmt(state, "addu r%d, %d", LVM_RETURN_REGISTER, 2 * LVM_PTRSIZE);
        }   
        else         
            lvmg_move_to(state, r, LVM_RETURN_REGISTER);
    }

    light_vm_push(state, "mov rsp, rbp");
    light_vm_push(state, "pop rbp");

    Light_Ast* proc = typecheck_decl_proc_from_scope(comm->scope_at);
    if(proc->decl_proc.proc_type->function.flags & TYPE_FUNCTION_STDCALL) {
        light_vm_push(state, "hlt");
    } else {
        light_vm_push(state, "ret");
    }
}

static void
lvmgen_command(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    push_lexical_range(gen, state->code_offset, &comm->lexical_range);

    switch(comm->kind)
    {
        case AST_COMMAND_RETURN:     lvmgen_comm_return(state, gen, stack_info, comm); break;
        case AST_DECL_VARIABLE:      lvmgen_decl_variable(state, gen, stack_info, comm); break;
        case AST_COMMAND_ASSIGNMENT: lvmgen_comm_assignment(state, gen, stack_info, comm); break;
        case AST_COMMAND_BLOCK:      lvmgen_comm_block(state, gen, stack_info, comm); break;
        case AST_COMMAND_IF:         lvmgen_comm_if(state, gen, stack_info, comm); break;
        case AST_COMMAND_WHILE:      lvmgen_comm_while(state, gen, stack_info, comm); break;
        case AST_COMMAND_FOR:        lvmgen_comm_for(state, gen, stack_info, comm); break;
        case AST_COMMAND_BREAK:      lvmgen_comm_break(state, gen, comm); break;
        case AST_COMMAND_CONTINUE:   lvmgen_comm_continue(state, gen, comm); break;
        case AST_DECL_CONSTANT: break;
        /*
        case AST_DECL_PROCEDURE:     Unimplemented; break;
        */
        default: Unimplemented;
    }
}

static void
stack_patch_instructions(Stack_Info* stack_info)
{
    for(s32 i = 0; i < array_length(stack_info->temp_patch); ++i)
    {        
        LVM_Instruction_Patch* patch = stack_info->temp_patch + i;
        Light_VM_Instruction* in = (Light_VM_Instruction*)patch->instr.absolute_address;
        switch(in->imm_size_bytes) {
            case 1: *(s8*)(in + 1)  = -(patch->offset + stack_info->offset); break;
            case 2: *(s16*)(in + 1) = -(patch->offset + stack_info->offset); break;
            case 4: *(s32*)(in + 1) = -(patch->offset + stack_info->offset); break;
            case 8: *(s64*)(in + 1) = -(patch->offset + stack_info->offset); break;
            default: Unreachable;
        }
    }
    array_free(stack_info->temp_patch);
}

// This functions receives any callbacks from the system or any other C libraries.
// it will switch back to the LVM interpreter and return to the caller appropriately.
u64 
#ifdef _WIN64
CALLBACK
#endif
lvm_process_callback(Light_VM_State* state, void* entry)
{
    light_vm_execute(state, entry, PRINT_LVM_INSTRUCTIONS, false);
    return state->registers[R0];
}

static void
lvmgem_proc_decl_lightcall(Light_VM_State* state, LVM_Generator* gen, Light_Ast* proc)
{
    push_lexical_range(gen, state->code_offset, &proc->decl_proc.decl_lexical_range);

    // Generate the stack for the arguments
    int offset = 2 * LVM_PTRSIZE;

    // There is stack space for holding the return value
    Light_Type* ret_type = type_alias_root(proc->decl_proc.return_type);
    if(return_type_requires_stackspace(ret_type))
    {
        offset += (ret_type->size_bits / BITS_IN_BYTE);
    }

    for(int i = 0; i < proc->decl_proc.argument_count; ++i)
    {
        Light_Ast* var = proc->decl_proc.arguments[i];
        assert(var->kind == AST_DECL_VARIABLE);

        var->decl_variable.stack_index = i;
        var->decl_variable.stack_offset = offset;
        var->decl_variable.stack_argument_offset = offset;
        offset += align_to_ptrsize(MIN(var->decl_variable.type->size_bits / BITS_IN_BYTE, LVM_PTRSIZE));
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
        proc->decl_proc.lvm_base_instruction = base.absolute_address;

        Stack_Info stack_info = { .offset = 0, .temp_offset = 0 };
        stack_info.temp_patch = array_new(LVM_Instruction_Patch);
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

        stack_patch_instructions(&stack_info);
    }
}

static void
lvmgen_proc_decl_stdcall(Light_VM_State* state, LVM_Generator* gen, Light_Ast* proc)
{
    u8* at = (u8*)state->code.block + state->code_offset;
    proc->decl_proc.lvm_base_instruction = at;
    
    s32 arg_size = 0;
    for(int i = 0; i < proc->decl_proc.argument_count; ++i)
    {
        Light_Ast* var = proc->decl_proc.arguments[i];
        arg_size += align_to_ptrsize(var->decl_variable.type->size_bits / BITS_IN_BYTE);
    }

    Light_Type* return_type = type_alias_root(proc->decl_proc.proc_type);
    if(return_type_requires_stackspace(return_type))
    {
        arg_size += (return_type->size_bits / BITS_IN_BYTE);
    }

    at = emit_mov(0, at, mk_oi(RSI, (uint64_t)&state->registers[LRSP], 64));
    at = emit_mov(0, at, mk_rm_indirect(RAX, RSI, 0, 64));
    at = emit_arithmetic(0, at, ARITH_SUB, mk_mi_direct(RAX, arg_size, 32));
    at = emit_mov(0, at, mk_mr_indirect(RSI, RAX, 0, 64));

    int offset = LVM_PTRSIZE;
    int add_offset = 0;
    for (int i = 0; i < proc->decl_proc.argument_count; ++i)
    {
        Light_Ast* var = proc->decl_proc.arguments[i];
        assert(var->kind == AST_DECL_VARIABLE);

        var->decl_variable.stack_index = i;
        var->decl_variable.stack_offset = offset;
        var->decl_variable.stack_argument_offset = offset;

        // TODO(psv): This is clearly incomplete.
        at = emit_arithmetic(0, at, ARITH_ADD, mk_mi_direct(RAX, add_offset, 32));
        if (i == 0)
            at = emit_mov(0, at, mk_mr_indirect(RAX, RCX, 0, 64));
        else if (i == 1)
            at = emit_mov(0, at, mk_mr_indirect(RAX, RDX, 0, 64));
        else if (i == 2)
            at = emit_mov(0, at, mk_mr_indirect(RAX, R8, 0, 64));
        else if (i == 3)
            at = emit_mov(0, at, mk_mr_indirect(RAX, R9, 0, 64));
        else
            Unimplemented;

        add_offset = align_to_ptrsize(var->decl_variable.type->size_bits / BITS_IN_BYTE);
        offset += add_offset;
    }

    at = emit_mov(0, at, mk_oi(RAX, (uint64_t)lvm_process_callback, 64));
    at = emit_mov(0, at, mk_oi(RCX, (uint64_t)state, 64)); // @Important, this is only for windows bro
    Instr_Emit_Result info = { 0 };
    u8* entry = at;
    at = emit_mov(&info, at, mk_oi(RDX, (uint64_t)0, 64)); // @Important, this is only for windows bro
    at = emit_arithmetic(0, at, ARITH_SUB, mk_mi_direct(RSP, 32, 8));
    at = emit_call(0, at, mk_m_direct(RAX));
    at = emit_arithmetic(0, at, ARITH_ADD, mk_mi_direct(RSP, 32, 8));
    at = emit_ret(0, at, RET_NEAR, 0);
   
    state->code_offset += (at - ((u8*)state->code.block + state->code_offset));
    
    *(uint64_t*)(entry + info.immediate_offset) = (uint64_t)((u8*)state->code.block + state->code_offset);

    // Generate body code
    Light_Ast* body = proc->decl_proc.body;
    if(body)
    {
        // Saves the previous stack frame base to return
        Light_VM_Instruction_Info base = light_vm_push(state, "push rbp");
        light_vm_push(state, "mov rbp, rsp");

        // Allocate space in the stack for the temporary variables
        Light_VM_Instruction_Info stack_alloc = light_vm_push(state, "subs rsp, 0xffffffff");

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
        light_vm_push_fmt(state, "adds rsp, %d", arg_size);
        light_vm_push(state, "hlt");
    }
}

static void
lvmgen_proc_decl(Light_VM_State* state, LVM_Generator* gen, Light_Ast* proc)
{
    assert(proc->kind == AST_DECL_PROCEDURE);
    if((proc->decl_proc.proc_type->function.flags & TYPE_FUNCTION_STDCALL) && (!proc->decl_proc.extern_library_name)) {
        lvmgen_proc_decl_stdcall(state, gen, proc);
    } else {
        lvmgem_proc_decl_lightcall(state, gen, proc);
    }
}

void
lvm_generate(Light_Ast** ast, Light_Scope* global_scope)
{
    // -------------------------------------
    // Initialization
    Light_VM_State* state = light_vm_init();

    LVM_Generator gen = {0};
    gen.proc_bases         = array_new(Light_VM_Instruction_Info);
    gen.loop_breaks        = array_new(Light_VM_Instruction_Info);
    gen.loop_continue      = array_new(Light_VM_Instruction_Info);
    gen.proc_patch_calls   = array_new(Patch_Procs);
    gen.debug_info         = array_new(Light_VM_DebugInfo);
    gen.release_size_bytes = 0;

    // Initialize external symbol hash table
    hoht_new(&gen.external_table, 512, sizeof(External_Lib), 0.5f, malloc, free);

    lvm_emit_type_table(state, &gen, global_type_array);
    // Generate all global variables first
    for(int i = 0; i < array_length(ast); ++i)
    {
        if(ast[i]->kind == AST_DECL_VARIABLE)
            lvmgen_decl_variable(state, &gen, 0, ast[i]);
    }

    // -------------------------------------
    // Generate the code
    Light_VM_Instruction_Info start = light_vm_push(state, "call 0xffffffff");
    light_vm_push(state, "hlt");

    // Generate code
    for(int i = 0; i < array_length(ast); ++i)
    {
        if(ast[i]->kind == AST_DECL_PROCEDURE)
        {
            if(ast[i]->decl_proc.flags & DECL_PROC_FLAG_MAIN)
                light_vm_patch_from_to_current_instruction(state, start);
            lvmgen_proc_decl(state, &gen, ast[i]);
        }
    }

    light_vm_push(state, "ret");

    // -------------------------------------
    // Perform all patching needed
    for(int i = 0; i < array_length(gen.proc_patch_calls); ++i)
    {
        void* to = (Light_VM_Instruction_Info*)((Light_Ast*)gen.proc_patch_calls[i].to_decl)->decl_proc.lvm_base_instruction;
        if (gen.proc_patch_calls[i].absolute)
        {
            if(to)
                light_vm_patch_instruction_immediate(gen.proc_patch_calls[i].from, (uint64_t)to);
            else
            {
                // TODO(psv): compress this in a function, there is code duplication 
                Light_Ast* decl = (Light_Ast*)gen.proc_patch_calls[i].to_decl;
                Light_Token* token = decl->decl_proc.extern_library_name;

                u64 lib_hash = fnv_1_hash(token->raw_data, token->raw_data_length);
                External_Lib* lib = hoht_get_value_hashed(&gen.external_table, lib_hash);
                if (!lib)
                {
                    void* lname = calloc(1, token->raw_data_length + 1);
                    memcpy(lname, token->raw_data, token->raw_data_length);
                    External_Lib nlib = {
                        .name = token->raw_data,
                        .name_length = token->raw_data_length,
                        #if defined(_WIN32) || defined(_WIN64)
                        .lib_handle = LoadLibraryA(lname)
                        #endif
                    };
                    free(lname);
                    hoht_new(&nlib.symbols, 32, sizeof(External_Symbol), 0.5f, malloc, free);
                    int idx = hoht_push_hashed(&gen.external_table, lib_hash, &nlib);
                    lib = hoht_get_value_from_index(&gen.external_table, idx);
                }

                u64 symbol_hash = fnv_1_hash(decl->decl_proc.name->data, decl->decl_proc.name->length);
                if (strncmp(decl->decl_proc.name->data, "glClearColor", decl->decl_proc.name->length) == 0)
                {
                    int x = 0;
                }
                External_Symbol* symbol = hoht_get_value_hashed(&lib->symbols, symbol_hash);
                if (!symbol)
                {
                    void* s = calloc(1, decl->decl_proc.name->length + 1);
                    memcpy(s, decl->decl_proc.name->data, decl->decl_proc.name->length);
                    External_Symbol sym = {
                        .symbol = decl->decl_proc.name->data,
                        .symbol_length = decl->decl_proc.name->length,
                        #if defined(_WIN32) || defined(_WIN64)
                        .proc_address = GetProcAddress(lib->lib_handle, s),
                        #endif
                    };
                    free(s);
                    int idx = hoht_push_hashed(&lib->symbols, symbol_hash, &sym);
                    symbol = hoht_get_value_from_index(&lib->symbols, idx);
                }

                light_vm_patch_instruction_immediate(gen.proc_patch_calls[i].from, (uint64_t)symbol->proc_address);
            }
        }
        else
        {
            light_vm_patch_immediate_distance_addr(gen.proc_patch_calls[i].from, to);
        }
    }

    // -------------------------------------
    // Free everything
    array_free(gen.proc_bases);
    array_free(gen.loop_breaks);
    array_free(gen.loop_continue);
    array_free(gen.proc_patch_calls);
    // -------------------------------------
    // Debug printout
#if DUMP_LVM_CODE
    light_vm_debug_dump_code(stdout, state, gen.debug_info);
#endif

    r64 start_exe_time = os_time_us(); 
    light_vm_execute(state, 0, PRINT_LVM_INSTRUCTIONS, true);    
    r64 end_exe_time = os_time_us(); 
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL|LVM_PRINT_FLOATING_POINT_REGISTERS);
}