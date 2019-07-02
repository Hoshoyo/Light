#include <common.h>
#include <assert.h>
#include <light_array.h>
#include "ast.h"
#include "type.h"
#include "bytecode.h"

static void
free_register(Bytecode_State* state, u8 reg) {
    state->iregs[reg].age = 0;
    state->iregs[reg].allocated = 0;
}

static void
free_fregister(Bytecode_State* state, u8 reg) {
    state->fregs[reg].age = 0;
    state->fregs[reg].allocated = 0;
}

static void
alloc_register_specified(Bytecode_State* state, u8 reg) {
    state->iregs[reg].age = 0;
    state->iregs[reg].allocated = 1;
}

static u8
alloc_register(Bytecode_State* state) {
    int oldest_age = -1;
    int oldest = -1;
    int al = -1;

    for(u8 i = 0; i < R_COUNT; ++i){
        if(!state->iregs[i].allocated) {
            state->iregs[i].allocated = 1;
            state->iregs[i].age = 0;
            al = i;
            break;
        }
    }

    for(u8 i = 0; i < R_COUNT; ++i) {
        if(state->iregs[i].age > oldest_age) {
            oldest_age = state->iregs[i].age;
            oldest = i;
        }
        state->iregs[i].age++;
    }

    if(al == -1) {
        al = oldest;
    }
    return al;
}

static u8
alloc_f32_register(Bytecode_State* state) {
    for(u8 i = 0; i < FR4; ++i){
        if(!state->fregs[i].allocated) {
            state->fregs[i].allocated = 1;
            return i;
        }
    }
    // Could not allocate
    assert(0);
    return 0;
}

static u8
alloc_f64_register(Bytecode_State* state) {
    for(u8 i = FR4; i < FREG_COUNT; ++i){
        if(!state->fregs[i].allocated) {
            state->fregs[i].allocated = 1;
            return i;
        }
    }
    // Could not allocate
    assert(0);
    return 0;
}

static u8
alloc_register_for_expr(Bytecode_State* state, Light_Ast* expr) {
    if(expr->type == type_primitive_get(TYPE_PRIMITIVE_R32)) {
        return alloc_f32_register(state);
    } else if(expr->type == type_primitive_get(TYPE_PRIMITIVE_R64)) {
        return alloc_f64_register(state);
    } else {
        return alloc_register(state);
    }
    return 0;
}

static void
free_register_for_expr(Bytecode_State* state, Light_Ast* expr, u8 reg) {
    if(type_primitive_float(expr->type)) {
        free_fregister(state, reg);
    } else {
        free_register(state, reg);
    }
}

static Light_VM_Instruction_Info
bytecode_gen_expr_literal_primitive(Bytecode_State* state, Light_Ast* expr, u8 reg) {
    if(expr->expr_literal_primitive.type == LITERAL_POINTER) {
        return light_vm_push_fmt(state->vmstate, "mov r%d, 0", reg);
    }

    switch(expr->type->primitive) {
        case TYPE_PRIMITIVE_BOOL: {
            if(expr->expr_literal_primitive.value_bool) {
                return light_vm_push_fmt(state->vmstate, "mov r%d, 1", reg);
            } else {
                return light_vm_push_fmt(state->vmstate, "mov r%d, 0", reg);
            }
        } break;

        case TYPE_PRIMITIVE_R32: {
            void* addr = light_vm_push_r32_to_datasegment(state->vmstate, expr->expr_literal_primitive.value_r32);
            s64 diff = (char*)addr - ((char*)state->vmstate->data.block);
            
            return light_vm_push_fmt(state->vmstate, "fmov fr%d, [rdp + 0x%lx]", reg, diff);
        } break;
        case TYPE_PRIMITIVE_R64: {
            void* addr = light_vm_push_r64_to_datasegment(state->vmstate, expr->expr_literal_primitive.value_r64);
            s64 diff = (char*)addr - ((char*)state->vmstate->data.block);

            return light_vm_push_fmt(state->vmstate, "fmov fr%d, [rdp + 0x%lx]", reg, diff);
        } break;

        case TYPE_PRIMITIVE_S8:
            return light_vm_push_fmt(state->vmstate, "mov r%db, 0x%x", reg, expr->expr_literal_primitive.value_s8);
        case TYPE_PRIMITIVE_S16:
            return light_vm_push_fmt(state->vmstate, "mov r%dw, 0x%x", reg, expr->expr_literal_primitive.value_s16);
        case TYPE_PRIMITIVE_S32:
            return light_vm_push_fmt(state->vmstate, "mov r%dd, 0x%x", reg, expr->expr_literal_primitive.value_s32);
        case TYPE_PRIMITIVE_S64:
            return light_vm_push_fmt(state->vmstate, "mov r%d, 0x%lx", reg, expr->expr_literal_primitive.value_s64);
        case TYPE_PRIMITIVE_U8:
            return light_vm_push_fmt(state->vmstate, "mov r%db, 0x%x", reg, expr->expr_literal_primitive.value_u8);
        case TYPE_PRIMITIVE_U16:
            return light_vm_push_fmt(state->vmstate, "mov r%dw, 0x%x", reg, expr->expr_literal_primitive.value_s16);
        case TYPE_PRIMITIVE_U32:
            return light_vm_push_fmt(state->vmstate, "mov r%dd, 0x%x", reg, expr->expr_literal_primitive.value_s32);
        case TYPE_PRIMITIVE_U64:
            return light_vm_push_fmt(state->vmstate, "mov r%d, 0x%lx", reg, expr->expr_literal_primitive.value_s64);
        case TYPE_PRIMITIVE_VOID:
            break;
        default: assert(0); break;
    }

    return (Light_VM_Instruction_Info){0};
}

static Light_VM_Instruction_Info
bytecode_gen_expr_binary(Bytecode_State* state, Light_Ast* expr, u8 reg) {
    Light_VM_Instruction_Info first = bytecode_gen_expr(state, expr->expr_binary.left, reg);

    u8 rr = alloc_register_for_expr(state, expr->expr_binary.right);
    bytecode_gen_expr(state, expr->expr_binary.right, rr);

    const char* suffix = 0;
    const char* op = 0;

    //bool signed_ = (expr->expr_binary.right)
    if(type_primitive_sint(expr->expr_binary.right->type)) {
        suffix = "s";
    }
    if(type_primitive_uint(expr->expr_binary.right->type)) {
        suffix = "u";
    }

    switch(expr->expr_binary.op) 
    {
        case OP_BINARY_PLUS:        op = "add"; break;
        case OP_BINARY_MINUS:       op = "sub"; break;
        case OP_BINARY_MULT:        op = "mul"; break;
        case OP_BINARY_DIV:         op = "div"; break;
        case OP_BINARY_MOD:         op = "mod"; break;
        case OP_BINARY_AND:         op = "and"; suffix = 0; break;
        case OP_BINARY_OR:          op = "or";  suffix = 0; break;
        case OP_BINARY_XOR:         op = "xor"; suffix = 0; break;
        case OP_BINARY_SHL:         op = "shl"; suffix = 0; break;
        case OP_BINARY_SHR:         op = "shr"; suffix = 0; break;

        case OP_BINARY_LT:          op = "movlt"; break;
        case OP_BINARY_GT:          op = "movgt"; break;
        case OP_BINARY_LE:          op = "movle"; break;
        case OP_BINARY_GE:          op = "movge"; break;

        case OP_BINARY_EQUAL:       op = "moveq"; suffix = 0; break;
        case OP_BINARY_NOT_EQUAL:   op = "movne"; suffix = 0; break;

        case OP_BINARY_LOGIC_AND:   op = "and"; suffix = 0; break;
        case OP_BINARY_LOGIC_OR:    op = "or";  suffix = 0; break;

        case OP_BINARY_VECTOR_ACCESS: break;
        default: assert(0); break;
    }

    if(expr->type->kind == TYPE_KIND_PRIMITIVE) {
        switch(expr->type->primitive) {
            case TYPE_PRIMITIVE_R32:
            case TYPE_PRIMITIVE_R64:
                light_vm_push_fmt(state->vmstate, "%s%s fr%d, fr%d", op, suffix, reg, rr);
                break;
            case TYPE_PRIMITIVE_BOOL:
                light_vm_push_fmt(state->vmstate, "%s%s r%d, r%d", op, suffix, reg, rr);
                break;
            case TYPE_PRIMITIVE_S8:
            case TYPE_PRIMITIVE_U8:
                light_vm_push_fmt(state->vmstate, "%s%s r%db, r%db", op, suffix, reg, rr);
                break;
            case TYPE_PRIMITIVE_S16:
            case TYPE_PRIMITIVE_U16:
                light_vm_push_fmt(state->vmstate, "%s%s r%dw, r%dw", op, suffix, reg, rr);
                break;
            case TYPE_PRIMITIVE_S32:
            case TYPE_PRIMITIVE_U32:
                light_vm_push_fmt(state->vmstate, "%s%s r%dd, r%dd", op, suffix, reg, rr);
                break;
            case TYPE_PRIMITIVE_S64:
            case TYPE_PRIMITIVE_U64:
                light_vm_push_fmt(state->vmstate, "%s%s r%d, r%d", op, suffix, reg, rr);
                break;
            case TYPE_PRIMITIVE_VOID:
                break;
            default: assert(0); break;
        }
    }
    
    return first;
}

Light_VM_Instruction_Info
bytecode_gen_expr(Bytecode_State* state, Light_Ast* expr, u8 reg) {
    Light_VM_Instruction_Info first = {0};

    switch(expr->kind) {
        case AST_EXPRESSION_LITERAL_PRIMITIVE: {
            return bytecode_gen_expr_literal_primitive(state, expr, reg);
        } break;
        case AST_EXPRESSION_BINARY:
            return bytecode_gen_expr_binary(state, expr, reg);
        case AST_EXPRESSION_DOT:
        case AST_EXPRESSION_LITERAL_ARRAY:
        case AST_EXPRESSION_LITERAL_STRUCT:
        case AST_EXPRESSION_PROCEDURE_CALL:
        case AST_EXPRESSION_UNARY:
        case AST_EXPRESSION_VARIABLE:
            break;
        case AST_EXPRESSION_DIRECTIVE:
        default:
            assert(0); break;
    }
    return first;
}

Light_VM_Instruction_Info
bytecode_gen_comm(Bytecode_State* state, Light_Ast* comm) {
    Light_VM_Instruction_Info first = {0};
    switch(comm->kind) {
        case AST_COMMAND_BLOCK: {
            for(s32 i = 0; i < comm->comm_block.command_count; ++i) {
                Light_VM_Instruction_Info inst = {0};
                Light_Ast* node = comm->comm_block.commands[i];

                if(node->flags & AST_FLAG_COMMAND) {
                    inst = bytecode_gen_comm(state, node);
                } else if(node->flags & AST_FLAG_DECLARATION) {
                    //inst = bytecode_gen_internal_decl(state, node);
                } else {
                    // Should not be possible to have expressions as commands
                    assert(0);
                }
                if(i == 0) {
                    first = inst;
                }
            }
        } break;

        case AST_COMMAND_ASSIGNMENT: {

        } break;

        case AST_COMMAND_IF: {

        } break;

        case AST_COMMAND_RETURN: {
            Light_Ast* ret_expr = comm->comm_return.expression;
            if(ret_expr) {
                if(ret_expr->type == type_primitive_get(TYPE_PRIMITIVE_R32)) {
                    alloc_register_specified(state, FR0);
                    first = bytecode_gen_expr(state, comm->comm_return.expression, FR0);
                } else if(ret_expr->type == type_primitive_get(TYPE_PRIMITIVE_R64)){
                    alloc_register_specified(state, FR4);
                    first = bytecode_gen_expr(state, comm->comm_return.expression, FR4);
                } else {
                    alloc_register_specified(state, R0);
                    first = bytecode_gen_expr(state, comm->comm_return.expression, R0);
                }
            }
            light_vm_push(state->vmstate, "ret");
        } break;

        case AST_COMMAND_FOR: {

        } break;

        case AST_COMMAND_WHILE: {

        } break;

        case AST_COMMAND_BREAK: {

        } break;

        case AST_COMMAND_CONTINUE: {

        } break;

        default: assert(0);
    }
    return first;
}

Light_VM_Instruction_Info
bytecode_gen_decl(Bytecode_State* state, Light_Ast* decl) {
    Light_VM_Instruction_Info first = {0};
    switch(decl->kind) {
        case AST_DECL_PROCEDURE:{
            // TODO(psv): Calculate stack size
            first = bytecode_gen_comm(state, decl->decl_proc.body);
        }break;
        case AST_DECL_VARIABLE:{

        }break;

        case AST_DECL_CONSTANT:
        case AST_DECL_TYPEDEF:
        default: break;
    }
    return first;
}

Light_VM_Instruction_Info
bytecode_gen_internal_decl(Bytecode_State* state, Light_Ast* decl) {
    Light_VM_Instruction_Info first = {0};

    return first;
}

Bytecode_State 
bytecode_gen_ast(Light_Ast** ast) {
    Bytecode_State state = {0};

    state.vmstate = light_vm_init();

    Light_VM_Instruction_Info call = 
        light_vm_push(state.vmstate, "call 0xff");
    light_vm_push(state.vmstate, "hlt");

    Light_VM_Instruction_Info entry = {0};
    for(s32 i = 0; i < array_length(ast); ++i) {
        entry = bytecode_gen_decl(&state, ast[i]);
    }

    light_vm_patch_immediate_distance(call, entry);

    return state;
}