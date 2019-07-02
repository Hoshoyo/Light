#include <common.h>
#include <assert.h>
#include <light_array.h>
#include "ast.h"
#include "type.h"
#include "bytecode.h"

static const char*
register_size_suffix(int size_bits, u8 reg) {
    switch(size_bits) {
        case 64: return "";
        case 32: return "d";
        case 16: return "w";
        case 8: return "b";
        default: assert(0); break;
    }
    return "";
}

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

static s32
decl_size_bits(Light_Ast* decl) {
    if(decl->kind == AST_DECL_VARIABLE) {
        return decl->decl_variable.type->size_bits;
    } else {
        return 0;
    }
}

static Light_VM_Instruction_Info
bytecode_emit_proc_epilogue(Bytecode_State* state) {
    Light_VM_Instruction_Info first = 
    light_vm_push(state->vmstate, "mov rsp, rbp");
    light_vm_push(state->vmstate, "pop rbp");
    light_vm_push(state->vmstate, "ret");
    return first;
}

static Light_VM_Instruction_Info
bytecode_gen_expr_literal_primitive(Bytecode_State* state, Light_Ast* expr, u8 reg) {
    
    if(expr->expr_literal_primitive.type == LITERAL_POINTER) {
        return light_vm_push_fmt(state->vmstate, "mov r%d, 0", reg);
    }

    Light_Type* type = type_alias_root(expr->type);
    switch(type->primitive) {
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
bytecode_gen_expr_vector_access(Bytecode_State* state, Light_Ast* expr, u8 left, u8 right) {
    // TODO(psv): ABC
    Light_VM_Instruction_Info first = {0};

    Light_Type* type = type_alias_root(expr->type);
    
    Light_Type* ptr_to = 0;

    if(type->kind == TYPE_KIND_ARRAY) {
        ptr_to = type->array_info.array_of;
    } else if(type->kind == TYPE_KIND_POINTER) {
        ptr_to = type->pointer_to;
    } else {
        assert(0);
    }

    switch(ptr_to->size_bits) {
        case 64:
            first = light_vm_push_fmt(state->vmstate, "mov r%d, [r%d]", left, right);
            break;
        case 32:
            first = light_vm_push_fmt(state->vmstate, "mov r%dd, [r%d]", left, right);
            break;
        case 16:
            first = light_vm_push_fmt(state->vmstate, "mov r%dw, [r%d]", left, right);
            break;
        case 8:
            first = light_vm_push_fmt(state->vmstate, "mov r%db, [r%d]", left, right);
            break;
        default:{
            // bigger than register size
            // TODO(psv):
            assert(0);
        }break;
    }
    return first;
}

static Light_VM_Instruction_Info
bytecode_gen_expr_unary(Bytecode_State* state, Light_Ast* expr, u8 reg) {
    Light_VM_Instruction_Info first = {0};
    
    Light_Type* type = type_alias_root(expr->type);

    switch(expr->expr_unary.op) {
        case OP_UNARY_PLUS:
            first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
        case OP_UNARY_MINUS: {
            first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
            if(type_primitive_float(type)) {
                light_vm_push_fmt(state->vmstate, "fneg fr%d", reg);
            } else {
                light_vm_push_fmt(state->vmstate, "neg r%d%s", reg, register_size_suffix(type->size_bits, reg));
            }
        } break;
        case OP_UNARY_LOGIC_NOT: {
            first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
            light_vm_push_fmt(state->vmstate, "cmp r%d, 0x0", reg);
            light_vm_push_fmt(state->vmstate, "mov r%d, 0", reg);
            light_vm_push_fmt(state->vmstate, "moveq r%d, 1", reg);
        } break;
        case OP_UNARY_BITWISE_NOT:{
            first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
            light_vm_push_fmt(state->vmstate, "not r%d%s", reg, register_size_suffix(type->size_bits, reg));
        } break;
        case OP_UNARY_DEREFERENCE:{
            // mov r0, [r0]
            if(type_primitive_float(type)) {
                u8 r = alloc_register(state);
                first = bytecode_gen_expr(state, expr->expr_unary.operand, r);
                light_vm_push_fmt(state->vmstate, "fmov fr%d, [r%d]", reg, r);
                free_register(state, r);
            } else if(type->size_bits <= 64) {
                first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
                light_vm_push_fmt(state->vmstate, "mov r%d, [r%d]", reg, reg);
            } else {
                // TODO(psv): dereference type bigger than 64 bits
                assert(0);
            }
        } break;
        case OP_UNARY_ADDRESSOF:
            // TODO(psv):
        case OP_UNARY_CAST:
            // TODO(psv):
            break;
        default: assert(0); break;
    }
    return first;
}

static Light_VM_Instruction_Info
bytecode_emit_comparison(Bytecode_State* state, Light_Ast* expr, u8 left, u8 right) {
    Light_VM_Instruction_Info first = {0};

    Light_Type* type = expr->type;

    const char* suffix = "";
    const char* op = 0;

    if(type_primitive_sint(expr->expr_binary.right->type)) {
        suffix = "s";
    }
    if(type_primitive_uint(expr->expr_binary.right->type)) {
        suffix = "u";
    }

    if(type->size_bits <= 64) {
        char* reg_prefix = "";
        if(type_primitive_float(type)) {
            first = light_vm_push_fmt(state->vmstate, "fcmp fr%d, fr%d", left, right);
            reg_prefix = "f";
        } else {
            first = light_vm_push_fmt(state->vmstate, "cmp r%d, r%d", left, right);
        }

        switch(expr->expr_binary.op) {
            case OP_BINARY_LT:          first = light_vm_push_fmt(state->vmstate, "movlt%s %sr%d", suffix, reg_prefix, left); break;
            case OP_BINARY_GT:          first = light_vm_push_fmt(state->vmstate, "movgt%s %sr%d", suffix, reg_prefix, left); break;
            case OP_BINARY_LE:          first = light_vm_push_fmt(state->vmstate, "movle%s %sr%d", suffix, reg_prefix, left); break;
            case OP_BINARY_GE:          first = light_vm_push_fmt(state->vmstate, "movge%s %sr%d", suffix, reg_prefix, left); break;
            case OP_BINARY_EQUAL:       first = light_vm_push_fmt(state->vmstate, "moveq %sr%d", reg_prefix, left); break;
            case OP_BINARY_NOT_EQUAL:   first = light_vm_push_fmt(state->vmstate, "movne %sr%d", reg_prefix, left); break;
            default: assert(0); break;
        }
    } else {
        // TODO(psv):
        assert(0);
    }
    return first;
}

static Light_VM_Instruction_Info
bytecode_gen_expr_binary(Bytecode_State* state, Light_Ast* expr, u8 reg) {
    Light_VM_Instruction_Info first = bytecode_gen_expr(state, expr->expr_binary.left, reg);

    u8 rr = alloc_register_for_expr(state, expr->expr_binary.right);
    bytecode_gen_expr(state, expr->expr_binary.right, rr);

    const char* suffix = "";
    const char* op = 0;

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

        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL: 
        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE: {
            bytecode_emit_comparison(state, expr, reg, rr);
            return first;
        }

        case OP_BINARY_LOGIC_AND:   op = "and"; suffix = 0; break;
        case OP_BINARY_LOGIC_OR:    op = "or";  suffix = 0; break;

        case OP_BINARY_VECTOR_ACCESS: {
            return bytecode_gen_expr_vector_access(state, expr, reg, rr);
        } break;
        default: assert(0); break;
    }

    Light_Type* type = type_alias_root(expr->type);
    if(type->kind == TYPE_KIND_PRIMITIVE) {
        switch(type->primitive) {
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
        case AST_EXPRESSION_BINARY:
            return bytecode_gen_expr_binary(state, expr, reg);
        case AST_EXPRESSION_UNARY:
            return bytecode_gen_expr_unary(state, expr, reg);
        case AST_EXPRESSION_LITERAL_PRIMITIVE: {
            return bytecode_gen_expr_literal_primitive(state, expr, reg);
        } break;
        case AST_EXPRESSION_LITERAL_ARRAY:
        case AST_EXPRESSION_LITERAL_STRUCT:
        case AST_EXPRESSION_DOT:
        case AST_EXPRESSION_PROCEDURE_CALL:
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
            Light_Scope* block_scope = comm->comm_block.block_scope;

            s32 stack_size = 0;
            
            // Allocate space in the stack for all block decls
            assert(array_length(block_scope->decls) == block_scope->decl_count);
            for(s32 i = 0; i < block_scope->decl_count; ++i) {
                Light_Ast* decl = block_scope->decls[i];
                if(decl->kind == AST_DECL_VARIABLE && decl->decl_variable.storage_class == STORAGE_CLASS_STACK) {
                    decl->decl_variable.stack_offset = stack_size;
                    stack_size += decl_size_bits(decl);
                }
            }

            if(stack_size > 0) {
                light_vm_push_fmt(state->vmstate, "adds rsp, 0x%x", stack_size / 8);
            }

            for(s32 i = 0; i < comm->comm_block.command_count; ++i) {
                Light_VM_Instruction_Info inst = {0};
                Light_Ast* node = comm->comm_block.commands[i];

                if(node->flags & AST_FLAG_COMMAND) {
                    inst = bytecode_gen_comm(state, node);
                } else if(node->flags & AST_FLAG_DECLARATION) {
                    // TODO(psv): scoped procedures
                    //inst = bytecode_gen_internal_decl(state, node);
                } else {
                    // Should not be possible to have expressions as commands
                    assert(0);
                }
                if(i == 0 && stack_size == 0) {
                    first = inst;
                }
            }
        } break;

        case AST_COMMAND_ASSIGNMENT: {

        } break;

        case AST_COMMAND_IF: {
            u8 reg = alloc_register(state);
            bytecode_gen_expr(state, comm->comm_if.condition, reg);
            light_vm_push_fmt(state->vmstate, "cmp r%d, 0", reg);
            free_register(state, reg);

            Light_VM_Instruction_Info cond_branch = 
            light_vm_push(state->vmstate, "beq 0xffffffff");

            bytecode_gen_comm(state, comm->comm_if.body_true);

            if(comm->comm_if.body_false) {
                Light_VM_Instruction_Info skip_false_body = 
                light_vm_push(state->vmstate, "jmp 0xffffffff");

                Light_VM_Instruction_Info false_body = 
                bytecode_gen_comm(state, comm->comm_if.body_false);
                light_vm_patch_immediate_distance(cond_branch, false_body);
                light_vm_patch_from_to_current_instruction(state->vmstate, skip_false_body);
            } else {
                light_vm_patch_from_to_current_instruction(state->vmstate, cond_branch);
            }
        } break;

        case AST_COMMAND_RETURN: {
            Light_Ast* ret_expr = comm->comm_return.expression;
            if(ret_expr) {
                Light_Type* type = type_alias_root(ret_expr->type);
                if(type == type_primitive_get(TYPE_PRIMITIVE_R32)) {
                    alloc_register_specified(state, FR0);
                    first = bytecode_gen_expr(state, comm->comm_return.expression, FR0);
                } else if(type == type_primitive_get(TYPE_PRIMITIVE_R64)){
                    alloc_register_specified(state, FR4);
                    first = bytecode_gen_expr(state, comm->comm_return.expression, FR4);
                } else {
                    alloc_register_specified(state, R0);
                    first = bytecode_gen_expr(state, comm->comm_return.expression, R0);
                }
            }
            bytecode_emit_proc_epilogue(state);
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
            first = light_vm_push(state->vmstate, "push rbp");
            light_vm_push(state->vmstate, "mov rbp, rsp");
            bytecode_gen_comm(state, decl->decl_proc.body);
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