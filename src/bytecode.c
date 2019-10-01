#include <common.h>
#include <assert.h>
#include <light_array.h>
#include "ast.h"
#include "type.h"
#include "bytecode.h"
#include "utils/allocator.h"

u64 call_info_hash(Bytecode_CallInfo ci) {
    return fnv_1_hash(ci.name->data, ci.name->length);
}

int call_info_equal(Bytecode_CallInfo t1, Bytecode_CallInfo t2) {
    return t1.name->data == t2.name->data;
}

GENERATE_HASH_TABLE_IMPLEMENTATION(Bytecode_Calls, bytecode_calls, Bytecode_CallInfo, 
    call_info_hash, light_alloc, light_free, call_info_equal)

static const char*
register_name(Light_Register reg) {
    switch(reg.kind) {
        case LIGHT_REGISTER_F32:{
            switch(reg.code) {
                case FR0: return "fr0";
                case FR1: return "fr1";
                case FR2: return "fr2";
                case FR3: return "fr3";
                default: assert(0); break;
            }
        } break;
        case LIGHT_REGISTER_F64:{
            switch(reg.code) {
                case FR4: return "fr4";
                case FR5: return "fr5";
                case FR6: return "fr6";
                case FR7: return "fr7";
                default: assert(0); break;
            }
        } break;
        case LIGHT_REGISTER_INT: {
            switch(reg.code) {
                case R0: return "r0";
                case R1: return "r1";
                case R2: return "r2";
                case R3: return "r3";
                case R4: return "r4";
                case R5: return "r5";
                case R6: return "r6";
                case R7: return "r7";
                default: assert(0); break;
            }
        } break;
        default: assert(0); break;
    }
}

static const char*
register_size_suffix(s32 size_bits) {
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
free_register(Bytecode_State* state, Light_Register reg) {
    switch(reg.kind) {
        case LIGHT_REGISTER_F32:
        case LIGHT_REGISTER_F64:
            state->fregs[reg.code].age = 0;
            state->fregs[reg.code].allocated = 0;
            break;
        case LIGHT_REGISTER_INT:
        default:
            state->iregs[reg.code].age = 0;
            state->iregs[reg.code].allocated = 0;
            break;
    }
}

static void
alloc_register_specified(Bytecode_State* state, Light_Register reg) {
    switch(reg.kind) {
        case LIGHT_REGISTER_F32:
        case LIGHT_REGISTER_F64:
            state->fregs[reg.code].age = 0;
            state->fregs[reg.code].allocated = 1;
            break;
        case LIGHT_REGISTER_INT:
        default:
            state->iregs[reg.code].age = 0;
            state->iregs[reg.code].allocated = 1;
            break;
    }
}

static Light_Register
alloc_register(Bytecode_State* state, Light_Register_Type type, s32 size_bits) {
    Light_Register result = {0};

    int oldest_age = -1;
    int oldest = -1;
    int al = -1;

    if(type == LIGHT_REGISTER_INT) {
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
        result.kind = type;
        result.size_bits = size_bits;
        result.code = al;
    } else if(type == LIGHT_REGISTER_F32) {
        for(u8 i = 0; i < FR4; ++i){
            if(!state->fregs[i].allocated) {
                state->fregs[i].allocated = 1;
                result.code = i;
                result.kind = type;
                result.size_bits = 32;
                break;
            }
        }
        // TODO(psv): handle all registers allocated
    } else if(type == LIGHT_REGISTER_F64) {
        for(u8 i = FR4; i < FREG_COUNT; ++i){
            if(!state->fregs[i].allocated) {
                state->fregs[i].allocated = 1;
                result.code = i;
                result.kind = type;
                result.size_bits = 64;
                break;
            }
        }
        // TODO(psv): handle all registers allocated
    }

    return result;
}

static Light_Register
alloc_register_for_expr(Bytecode_State* state, Light_Ast* expr) {
    if(expr->type == type_primitive_get(TYPE_PRIMITIVE_R32)) {
        return alloc_register(state, LIGHT_REGISTER_F32, 32);
    } else if(expr->type == type_primitive_get(TYPE_PRIMITIVE_R64)) {
        return alloc_register(state, LIGHT_REGISTER_F64, 64);
    } else {
        return alloc_register(state, LIGHT_REGISTER_INT, expr->type->size_bits);
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
bytecode_gen_expr_literal_primitive(Bytecode_State* state, Light_Ast* expr, Light_Register reg) {
    
    if(expr->expr_literal_primitive.type == LITERAL_POINTER) {
        return light_vm_push_fmt(state->vmstate, "mov r%d, 0", reg.code);
    }

    Light_Type* type = type_alias_root(expr->type);
    switch(type->primitive) {
        case TYPE_PRIMITIVE_BOOL: {
            if(expr->expr_literal_primitive.value_bool) {
                return light_vm_push_fmt(state->vmstate, "mov r%d, 1", reg.code);
            } else {
                return light_vm_push_fmt(state->vmstate, "mov r%d, 0", reg.code);
            }
        } break;

        case TYPE_PRIMITIVE_R32: {
            void* addr = light_vm_push_r32_to_datasegment(state->vmstate, expr->expr_literal_primitive.value_r32);
            s64 diff = (char*)addr - ((char*)state->vmstate->data.block);
            
            return light_vm_push_fmt(state->vmstate, "fmov fr%d, [rdp + 0x%lx]", reg.code, diff);
        } break;
        case TYPE_PRIMITIVE_R64: {
            void* addr = light_vm_push_r64_to_datasegment(state->vmstate, expr->expr_literal_primitive.value_r64);
            s64 diff = (char*)addr - ((char*)state->vmstate->data.block);

            return light_vm_push_fmt(state->vmstate, "fmov fr%d, [rdp + 0x%lx]", reg.code, diff);
        } break;

        case TYPE_PRIMITIVE_S8:
            return light_vm_push_fmt(state->vmstate, "mov r%db, 0x%x", reg.code, expr->expr_literal_primitive.value_s8);
        case TYPE_PRIMITIVE_S16:
            return light_vm_push_fmt(state->vmstate, "mov r%dw, 0x%x", reg.code, expr->expr_literal_primitive.value_s16);
        case TYPE_PRIMITIVE_S32:
            return light_vm_push_fmt(state->vmstate, "mov r%dd, 0x%x", reg.code, expr->expr_literal_primitive.value_s32);
        case TYPE_PRIMITIVE_S64:
            return light_vm_push_fmt(state->vmstate, "mov r%d, 0x%lx", reg.code, expr->expr_literal_primitive.value_s64);
        case TYPE_PRIMITIVE_U8:
            return light_vm_push_fmt(state->vmstate, "mov r%db, 0x%x", reg.code, expr->expr_literal_primitive.value_u8);
        case TYPE_PRIMITIVE_U16:
            return light_vm_push_fmt(state->vmstate, "mov r%dw, 0x%x", reg.code, expr->expr_literal_primitive.value_s16);
        case TYPE_PRIMITIVE_U32:
            return light_vm_push_fmt(state->vmstate, "mov r%dd, 0x%x", reg.code, expr->expr_literal_primitive.value_s32);
        case TYPE_PRIMITIVE_U64:
            return light_vm_push_fmt(state->vmstate, "mov r%d, 0x%lx", reg.code, expr->expr_literal_primitive.value_s64);
        case TYPE_PRIMITIVE_VOID:
            break;
        default: assert(0); break;
    }

    return (Light_VM_Instruction_Info){0};
}

static Light_VM_Instruction_Info
bytecode_gen_expr_vector_access(Bytecode_State* state, Light_Ast* expr, Light_Register left, Light_Register right) {
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

    if(ptr_to->size_bits <= 64) {
        first = light_vm_push_fmt(state->vmstate, "mov %s%s, [%s]", 
            register_name(left), register_size_suffix(ptr_to->size_bits), register_name(right));
    } else {
        // bigger than register size
        // TODO(psv):
    }
    return first;
}

static Light_VM_Instruction_Info
bytecode_gen_expr_unary(Bytecode_State* state, Light_Ast* expr, Light_Register reg) {
    Light_VM_Instruction_Info first = {0};
    
    Light_Type* type = type_alias_root(expr->type);

    switch(expr->expr_unary.op) {
        case OP_UNARY_PLUS:
            first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
        case OP_UNARY_MINUS: {
            first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
            if(type_primitive_float(type)) {
                light_vm_push_fmt(state->vmstate, "fneg fr%d", reg.code);
            } else {
                light_vm_push_fmt(state->vmstate, "neg r%d%s", reg.code, register_size_suffix(type->size_bits));
            }
        } break;
        case OP_UNARY_LOGIC_NOT: {
            first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
            light_vm_push_fmt(state->vmstate, "cmp r%d, 0x0", reg.code);
            light_vm_push_fmt(state->vmstate, "mov r%d, 0", reg.code);
            light_vm_push_fmt(state->vmstate, "moveq r%d, 1", reg.code);
        } break;
        case OP_UNARY_BITWISE_NOT:{
            first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
            light_vm_push_fmt(state->vmstate, "not r%d%s", reg.code, register_size_suffix(type->size_bits));
        } break;
        case OP_UNARY_DEREFERENCE: {
            // mov r0, [r0]
            if(type_primitive_float(type)) {
                Light_Register r = alloc_register(state, LIGHT_REGISTER_INT, 64);
                first = bytecode_gen_expr(state, expr->expr_unary.operand, r);
                light_vm_push_fmt(state->vmstate, "fmov fr%d, [r%d]", reg.code, r.code);
                free_register(state, r);
            } else if(type->size_bits <= 64) {
                first = bytecode_gen_expr(state, expr->expr_unary.operand, reg);
                light_vm_push_fmt(state->vmstate, "mov r%d, [r%d]", reg.code, reg.code);
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
bytecode_emit_comparison(Bytecode_State* state, Light_Ast* expr, Light_Register left, Light_Register right) {
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
        if(type_primitive_float(type)) {
            first = light_vm_push_fmt(state->vmstate, "fcmp fr%d, fr%d", left.code, right.code);
        } else {
            first = light_vm_push_fmt(state->vmstate, "cmp r%d%s, r%d%s", 
                left.code, register_size_suffix(left.size_bits), right.code, register_size_suffix(right.size_bits));
        }

        switch(expr->expr_binary.op) {
            case OP_BINARY_LT:          first = light_vm_push_fmt(state->vmstate, "movlt%s %sr%d", suffix, register_name(left)); break;
            case OP_BINARY_GT:          first = light_vm_push_fmt(state->vmstate, "movgt%s %sr%d", suffix, register_name(left)); break;
            case OP_BINARY_LE:          first = light_vm_push_fmt(state->vmstate, "movle%s %sr%d", suffix, register_name(left)); break;
            case OP_BINARY_GE:          first = light_vm_push_fmt(state->vmstate, "movge%s %sr%d", suffix, register_name(left)); break;
            case OP_BINARY_EQUAL:       first = light_vm_push_fmt(state->vmstate, "moveq %sr%d", register_name(left)); break;
            case OP_BINARY_NOT_EQUAL:   first = light_vm_push_fmt(state->vmstate, "movne %sr%d", register_name(left)); break;
            default: assert(0); break;
        }
    } else {
        // TODO(psv):
        assert(0);
    }
    return first;
}

static Light_VM_Instruction_Info
bytecode_gen_expr_binary(Bytecode_State* state, Light_Ast* expr, Light_Register reg) {
    Light_VM_Instruction_Info first = bytecode_gen_expr(state, expr->expr_binary.left, reg);

    Light_Register rr = alloc_register_for_expr(state, expr->expr_binary.right);
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
                light_vm_push_fmt(state->vmstate, "%s%s fr%d, fr%d", op, suffix, reg.code, rr.code);
                break;
            case TYPE_PRIMITIVE_BOOL:
                light_vm_push_fmt(state->vmstate, "%s%s r%d, r%d", op, suffix, reg.code, rr.code);
                break;
            case TYPE_PRIMITIVE_S8:
            case TYPE_PRIMITIVE_U8:
                light_vm_push_fmt(state->vmstate, "%s%s r%db, r%db", op, suffix, reg.code, rr.code);
                break;
            case TYPE_PRIMITIVE_S16:
            case TYPE_PRIMITIVE_U16:
                light_vm_push_fmt(state->vmstate, "%s%s r%dw, r%dw", op, suffix, reg.code, rr.code);
                break;
            case TYPE_PRIMITIVE_S32:
            case TYPE_PRIMITIVE_U32:
                light_vm_push_fmt(state->vmstate, "%s%s r%dd, r%dd", op, suffix, reg.code, rr.code);
                break;
            case TYPE_PRIMITIVE_S64:
            case TYPE_PRIMITIVE_U64:
                light_vm_push_fmt(state->vmstate, "%s%s r%d, r%d", op, suffix, reg.code, rr.code);
                break;
            case TYPE_PRIMITIVE_VOID:
                break;
            default: assert(0); break;
        }
    }
    
    return first;
}

static Light_VM_Instruction_Info
bytecode_gen_expr_variable(Bytecode_State* state, Light_Ast* expr, Light_Register reg) {
    Light_VM_Instruction_Info first = {0};
    Light_Type* type = type_alias_root(expr->type);

    // Load its address
    first = light_vm_push_fmt(state->vmstate, "mov r%d, [rbp + 0x%x]", 
        reg.code, expr->expr_variable.decl->decl_variable.stack_offset);

/*
    switch(type->kind) {
        case TYPE_KIND_PRIMITIVE:
        case TYPE_KIND_POINTER:
        case TYPE_KIND_ARRAY:
        case TYPE_KIND_ENUM:
        case TYPE_KIND_FUNCTION:
        case TYPE_KIND_STRUCT:
        case TYPE_KIND_UNION:
        default: assert(0); break;
    }
 */

    return first;
}

Light_VM_Instruction_Info
bytecode_gen_expr(Bytecode_State* state, Light_Ast* expr, Light_Register reg) {
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
        case AST_EXPRESSION_VARIABLE: {
            return bytecode_gen_expr_variable(state, expr, reg);
        } break;
        case AST_EXPRESSION_DOT:
        case AST_EXPRESSION_PROCEDURE_CALL:
            break;
        case AST_EXPRESSION_DIRECTIVE:
        default:
            assert(0); break;
    }
    return first;
}

Light_VM_Instruction_Info
bytecode_gen_assignment(Bytecode_State* state, Light_Ast* comm) {
    Light_VM_Instruction_Info first = {0};

    Light_Ast* left = comm->comm_assignment.lvalue;
    Light_Ast* right = comm->comm_assignment.rvalue;
    switch(left->kind) {
        case AST_EXPRESSION_BINARY: {
            // can only be vector access
            assert(left->expr_binary.op == OP_BINARY_VECTOR_ACCESS);
        } break;
        case AST_EXPRESSION_DOT: {

        } break;
        case AST_EXPRESSION_UNARY: {

        } break;
        case AST_EXPRESSION_VARIABLE: {
            Light_Ast* decl = left->expr_variable.decl;
            s32 stack_offset = decl->decl_variable.stack_offset;
            Light_Register reg = alloc_register_for_expr(state, right);
            first = bytecode_gen_expr(state, right, reg);

            light_vm_push_fmt(state->vmstate, "mov [rbp + 0x%x], r%d", stack_offset, reg.code);

            free_register(state, reg);
        } break;
        default: assert(0); break;
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
                    
                    if(decl->decl_variable.assignment) {
                        Light_Register reg = alloc_register_for_expr(state, decl->decl_variable.assignment);
                        first = bytecode_gen_expr(state, decl->decl_variable.assignment, reg);
                        const char* prefix = (type_primitive_float(decl->decl_variable.assignment->type)) ? "f" : "";
                        light_vm_push_fmt(state->vmstate, "%smov [rbp + 0x%x], r%d", 
                            prefix, decl->decl_variable.stack_offset, reg.code);
                    }
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

            if(stack_size > 0) {
                light_vm_push_fmt(state->vmstate, "subs rsp, 0x%x", stack_size / 8);
            }
        } break;

        case AST_COMMAND_ASSIGNMENT: {
            return bytecode_gen_assignment(state, comm);
        } break;

        case AST_COMMAND_IF: {
            Light_Register reg = alloc_register_for_expr(state, comm->comm_if.condition);
            bytecode_gen_expr(state, comm->comm_if.condition, reg);
            light_vm_push_fmt(state->vmstate, "cmp r%d, 0", reg.code);
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

        case AST_COMMAND_WHILE: {
            Light_Register reg = alloc_register_for_expr(state, comm->comm_while.condition);
            
            Light_VM_Instruction_Info start = 
            bytecode_gen_expr(state, comm->comm_while.condition, reg);
            
            light_vm_push_fmt(state->vmstate, "cmp r%d, 0", reg.code);
            free_register(state, reg);

            Light_VM_Instruction_Info cond_branch = 
            light_vm_push(state->vmstate, "beq 0xffffffff");

            bytecode_gen_comm(state, comm->comm_while.body);

            Light_VM_Instruction_Info end_while = 
            light_vm_push_fmt(state->vmstate, "jmp 0xffffffff");

            light_vm_patch_immediate_distance(end_while, start);
            light_vm_patch_from_to_current_instruction(state->vmstate, cond_branch);
        } break;

        case AST_COMMAND_RETURN: {
            Light_Ast* ret_expr = comm->comm_return.expression;
            if(ret_expr) {
                if(ret_expr->type->size_bits <= 64) {
                    Light_Type* type = type_alias_root(ret_expr->type);
                    if(type == type_primitive_get(TYPE_PRIMITIVE_R32)) {
                        Light_Register fr0 = (Light_Register){LIGHT_REGISTER_F32, 32, FR0};
                        alloc_register_specified(state, fr0);
                        first = bytecode_gen_expr(state, comm->comm_return.expression, fr0);
                    } else if(type == type_primitive_get(TYPE_PRIMITIVE_R64)){
                        Light_Register fr4 = (Light_Register){LIGHT_REGISTER_F64, 64, FR4};
                        alloc_register_specified(state, fr4);
                        first = bytecode_gen_expr(state, comm->comm_return.expression, fr4);
                    } else {
                        Light_Register reg = alloc_register_for_expr(state, ret_expr);
                        first = bytecode_gen_expr(state, comm->comm_return.expression, reg);

                        if(reg.code != R0) {
                            Light_Register r0 = (Light_Register){LIGHT_REGISTER_INT, ret_expr->type->size_bits, R0};
                            alloc_register_specified(state, r0);
                            light_vm_push_fmt(state->vmstate, "mov r0, r%d", reg.code);
                        }
                    }
                } else {
                    // TODO(psv):
                }
            }
            bytecode_emit_proc_epilogue(state);
        } break;

        case AST_COMMAND_FOR: {
            
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
            // Prologue
            first = light_vm_push(state->vmstate, "push rbp");
            light_vm_push(state->vmstate, "mov rbp, rsp");
            
            // Put this procedure information in the calls table
            Bytecode_CallInfo call_info = {0};
            call_info.name = decl->decl_proc.name;
            call_info.info = first;

            bytecode_calls_table_add(&state->call_table, call_info, 0);
            
            bytecode_gen_comm(state, decl->decl_proc.body);
        }break;
        case AST_DECL_VARIABLE:{
            if(decl->decl_variable.assignment) {
                Light_Register reg = alloc_register_for_expr(state, decl->decl_variable.assignment);
                first = bytecode_gen_expr(state, decl->decl_variable.assignment, reg);
                light_vm_push_fmt(state->vmstate, "mov [rbp + 0x%x], r%d", reg.code);
            }
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
    // TODO(psv):
    return first;
}

Bytecode_State 
bytecode_gen_ast(Light_Ast** ast) {
    Bytecode_State state = {0};

    state.vmstate = light_vm_init();

    bytecode_calls_table_new(&state.call_table, 65536);

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