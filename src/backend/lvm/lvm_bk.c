#include "../../light_vm/lightvm.h"
#include "../../ast.h"
#include "../../type.h"
#include <assert.h>
#include <light_array.h>

#define BITS_IN_BYTE 8
#define LVM_RETURN_REGISTER R0
#define LVM_RETURN_F32_REGISTER FR0
#define LVM_RETURN_F64_REGISTER EFR0

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
    EXPR_RESULT_REG_INDIRECT,
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

// -------------------------------------------------
// Generation

static Expr_Result lvmgen_expr(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags);

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
        case EXPR_RESULT_REG_INDIRECT:
            Unimplemented;
    }
}

static void
lvmg_push_result(Light_VM_State* state, Expr_Result r)
{
    switch(r.type)
    {
        case EXPR_RESULT_REG_INDIRECT:
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
        case EXPR_RESULT_REG_INDIRECT:
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

static void
mask_top_bits(Light_VM_State* state, Expr_Result r)
{
    if(r.type == EXPR_RESULT_REG || r.type == EXPR_RESULT_REG_INDIRECT)
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
        light_vm_push_fmt(state, "mov r0%s, %d", register_size_suffix(expr->type->size_bits / BITS_IN_BYTE), expr->expr_literal_primitive.value_bool);
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
lvmgen_expr_binary_arithmetic(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    assert(expr->kind == AST_EXPRESSION_BINARY);
    Expr_Result result = {0};

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

        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE:

        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL:

        case OP_BINARY_VECTOR_ACCESS:

        default: Unreachable;
    }
    return result;
}

static Expr_Result
lvmgen_expr(Light_VM_State* state, LVM_Generator* gen, Light_Ast* expr, u32 flags)
{
    Expr_Result result = {0};
    switch(expr->kind)
    {
        case AST_EXPRESSION_LITERAL_PRIMITIVE:
            result = lvmgen_expr_literal_primitive(state, gen, expr, flags); break;
        case AST_EXPRESSION_BINARY:
            result = lvmgen_expr_binary(state, gen, expr, flags); break;
        case AST_EXPRESSION_UNARY:
        case AST_EXPRESSION_VARIABLE:
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
lvmgen_comm_return(Light_VM_State* state, LVM_Generator* gen, Stack_Info* stack_info, Light_Ast* comm)
{
    Light_Ast* expr = comm->comm_return.expression;
    if(expr)
    {
        Expr_Result r = lvmgen_expr(state, gen, expr, 0);
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
        /*
        case AST_DECL_VARIABLE:      lvmgen_decl_variable(comm, state, stack_info); break;
        case AST_COMMAND_ASSIGNMENT: lvmgen_comm_assignment(comm, state); break;
        case AST_COMMAND_BLOCK:      lvmgen_comm_block(comm, state, stack_info); break;
        case AST_COMMAND_IF:         lvmgen_comm_if(comm, state, stack_info); break;
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