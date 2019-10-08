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

void
expression_tag_evaluated_literals() {
    /*
        If a literal is in the form
        a := "hello";
        b := "hello".length;
        c := foo["hello".length];
    */
}

static bool
register_equal(Light_Register r1, Light_Register r2) {
    return (r1.code == r2.code && r1.kind == r2.kind && r1.size_bits == r2.size_bits);
}

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
        for(u8 i = 0; i <= R7; ++i){
            if(!state->iregs[i].allocated) {
                state->iregs[i].allocated = 1;
                state->iregs[i].age = 0;
                al = i;
                break;
            }
        }

        for(u8 i = 0; i <= R7; ++i) {
            if(state->iregs[i].age > oldest_age) {
                oldest_age = state->iregs[i].age;
                oldest = i;
            }
            state->iregs[i].age++;
        }

        if(al == -1) {
            // Allocate oldest, none is free
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
alloc_return_register_for_expr(Bytecode_State* state, Light_Ast* expr) {
    Light_Register r = {0};
    if(expr->type == type_primitive_get(TYPE_PRIMITIVE_R32)) {
        r.code = FR0;
        r.kind = LIGHT_REGISTER_F32;
        r.size_bits = 32;
        alloc_register_specified(state, r);
        return r;
    } else if(expr->type == type_primitive_get(TYPE_PRIMITIVE_R64)) {
        r.code = FR4;
        r.kind = LIGHT_REGISTER_F64;
        r.size_bits = 64;
        alloc_register_specified(state, r);
    } else {
        r.code = R0;
        r.kind = LIGHT_REGISTER_INT;
        r.size_bits = expr->type->size_bits;
    }
    return r;
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

// Primitive meaning is register size, or of type
// TYPE_KIND_POINTER, TYPE_KIND_PRIMITIVE, TYPE_KIND_ENUM
static bool
bytecode_expr_is_primitive(Light_Ast* expr) {
    Light_Type_Kind k = expr->type->kind;
    return k == TYPE_KIND_POINTER || k == TYPE_KIND_PRIMITIVE || k == TYPE_KIND_ENUM;
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
bytecode_gen_expr_literal_primitive(Bytecode_State* state, Light_Ast* expr, Light_Register* out_reg) {
    Light_Register reg = alloc_register_for_expr(state, expr);
    if(out_reg) *out_reg = reg;
    
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
bytecode_gen_expr_unary(Bytecode_State* state, Light_Ast* expr, Light_Register* out_reg) {
    Light_VM_Instruction_Info first = bytecode_gen_expr(state, expr->expr_unary.operand, out_reg);
    
    Light_Type* type = type_alias_root(expr->type);

    switch(expr->expr_unary.op) {
        case OP_UNARY_PLUS:
            break;
        case OP_UNARY_MINUS: {
            if(type_primitive_float(type)) {
                light_vm_push_fmt(state->vmstate, "fneg fr%d", out_reg->code);
            } else {
                light_vm_push_fmt(state->vmstate, "neg r%d%s", out_reg->code, register_size_suffix(type->size_bits));
            }
        } break;
        case OP_UNARY_LOGIC_NOT: {
            light_vm_push_fmt(state->vmstate, "cmp r%d, 0x0", out_reg->code);
            light_vm_push_fmt(state->vmstate, "mov r%d, 0", out_reg->code);
            light_vm_push_fmt(state->vmstate, "moveq r%d, 1", out_reg->code);
        } break;
        case OP_UNARY_BITWISE_NOT:{
            light_vm_push_fmt(state->vmstate, "not r%d%s", out_reg->code, register_size_suffix(type->size_bits));
        } break;
        case OP_UNARY_DEREFERENCE: {
            // Only dereference if operand is also a dereference operation
            if(expr->expr_unary.operand->kind == AST_EXPRESSION_UNARY && 
                expr->expr_unary.operand->expr_unary.op == OP_UNARY_DEREFERENCE)
            {
                // The type here is guaranteed to be a pointer type
                assert(type->kind == TYPE_KIND_POINTER);
                light_vm_push_fmt(state->vmstate, "mov r%d, [r%d]", out_reg->code, out_reg->code);
            } else {
                // Don't do anything, we assume the caller wants the address
                // and the caller can copy the value around if needed.
            }
        } break;
        case OP_UNARY_ADDRESSOF:
            // Nothing to do, the operand expression should already
            // be an address
            break;
        case OP_UNARY_CAST:
            // TODO(psv): check if we need a conversion or not
            break;
        default: assert(0); break;
    }
    return first;
}

static Light_VM_Instruction_Info
bytecode_emit_comparison(Bytecode_State* state, Light_Ast* expr, Light_Register left, Light_Register right, Light_Register* out_reg) {
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

    Light_Register result = alloc_register_for_expr(state, expr);
    *out_reg = result;

    if(type->size_bits <= 64) {
        if(type_primitive_float(type)) {
            first = light_vm_push_fmt(state->vmstate, "fcmp fr%d, fr%d", left.code, right.code);
        } else {
            first = light_vm_push_fmt(state->vmstate, "cmp r%d%s, r%d%s", 
                left.code, register_size_suffix(left.size_bits), right.code, register_size_suffix(right.size_bits));
        }

        switch(expr->expr_binary.op) {
            case OP_BINARY_LT:          first = light_vm_push_fmt(state->vmstate, "movlt%s %s", suffix, register_name(result)); break;
            case OP_BINARY_GT:          first = light_vm_push_fmt(state->vmstate, "movgt%s %s", suffix, register_name(result)); break;
            case OP_BINARY_LE:          first = light_vm_push_fmt(state->vmstate, "movle%s %s", suffix, register_name(result)); break;
            case OP_BINARY_GE:          first = light_vm_push_fmt(state->vmstate, "movge%s %s", suffix, register_name(result)); break;
            case OP_BINARY_EQUAL:       first = light_vm_push_fmt(state->vmstate, "moveq %s", register_name(result)); break;
            case OP_BINARY_NOT_EQUAL:   first = light_vm_push_fmt(state->vmstate, "movne %s", register_name(result)); break;
            default: assert(0); break;
        }
    } else {
        // TODO(psv):
        assert(0);
    }
    return first;
}

static Light_VM_Instruction_Info
bytecode_deref_expr(Bytecode_State* state, Light_Ast* expr, Light_Register addr_reg, Light_Register* out_reg, bool* dereferenced) {
    Light_VM_Instruction_Info first = {0};
    switch(expr->kind) {
        case AST_EXPRESSION_VARIABLE: {
            Light_Register result = alloc_register_for_expr(state, expr);
            *out_reg = result;
            first = light_vm_push_fmt(state->vmstate, "mov %s, [%s]", register_name(result), register_name(addr_reg));
            if(dereferenced) *dereferenced = true;
        } break;
        case AST_EXPRESSION_BINARY: // vector access?
        case AST_EXPRESSION_DOT:
        case AST_EXPRESSION_UNARY:
        default: break;
    }

    return first;
}

static Light_VM_Instruction_Info
bytecode_gen_expr_binary_comparison(Bytecode_State* state, Light_Ast* expr, Light_Register* out_reg) {

    // Evaluate left expression which must be a comparable type
    Light_Register lr = {0};
    Light_VM_Instruction_Info first = bytecode_gen_expr(state, expr->expr_binary.left, &lr);

    Light_Register rr = {0};
    Light_VM_Instruction_Info second = bytecode_gen_expr(state, expr->expr_binary.right, &rr);

    // Dereference all addresses
    if(bytecode_expr_is_primitive(expr)) 
    {
        Light_Register expr_lr = {0};
        bool left_dereferenced = false;
        bytecode_deref_expr(state, expr->expr_binary.left, lr, &expr_lr, &left_dereferenced);
        if(left_dereferenced) {
            free_register(state, lr);
            lr = expr_lr;
        }

        Light_Register expr_rr = {0};
        bool right_dereferenced = false;
        bytecode_deref_expr(state, expr->expr_binary.right, rr, &expr_rr, &right_dereferenced);
        if(right_dereferenced) {
            free_register(state, rr);
            rr = expr_rr;
        }

        bytecode_emit_comparison(state, expr, lr, rr, out_reg);
    } else {
        // TODO(psv): bigger comparisons (indirect)
    }

    free_register(state, lr);
    free_register(state, rr);

    return first;
}

static Light_VM_Instruction_Info
bytecode_gen_expr_binary_operation(Bytecode_State* state, Light_Ast* expr, Light_Register* out_reg, const char* op, const char* suffix) {
    Light_Type* ltype = type_alias_root(expr->expr_binary.left->type);
    Light_Type* rtype = type_alias_root(expr->expr_binary.right->type);

    Light_Register lr = {0};
    Light_Register rr = {0};
    
    Light_VM_Instruction_Info first = bytecode_gen_expr(state, expr->expr_binary.left, &lr);
    Light_VM_Instruction_Info second = bytecode_gen_expr(state, expr->expr_binary.right, &rr);

    Light_Register deref_left = {0};
    bool left_dereferenced = false;
    bytecode_deref_expr(state, expr->expr_binary.left, lr, &deref_left, &left_dereferenced);
    if(left_dereferenced) {
        free_register(state, lr);
        lr = deref_left;
    }

    Light_Register deref_right = {0};
    bool right_dereferenced = false;
    bytecode_deref_expr(state, expr->expr_binary.right, rr, &deref_right, &right_dereferenced);
    if(right_dereferenced) {
        free_register(state, rr);
        rr = deref_right;
    }

    Light_Type* type = type_alias_root(expr->type);
    if(type->kind == TYPE_KIND_PRIMITIVE) 
    {
        switch(type->primitive) {
            case TYPE_PRIMITIVE_R32:
            case TYPE_PRIMITIVE_R64:
                light_vm_push_fmt(state->vmstate, "%s%s fr%d, fr%d", op, suffix, lr.code, rr.code);
                break;
            case TYPE_PRIMITIVE_BOOL:
                light_vm_push_fmt(state->vmstate, "%s%s r%d, r%d", op, suffix, lr.code, rr.code);
                break;
            case TYPE_PRIMITIVE_S8:
            case TYPE_PRIMITIVE_U8:
                light_vm_push_fmt(state->vmstate, "%s%s r%db, r%db", op, suffix, lr.code, rr.code);
                break;
            case TYPE_PRIMITIVE_S16:
            case TYPE_PRIMITIVE_U16:
                light_vm_push_fmt(state->vmstate, "%s%s r%dw, r%dw", op, suffix, lr.code, rr.code);
                break;
            case TYPE_PRIMITIVE_S32:
            case TYPE_PRIMITIVE_U32:
                light_vm_push_fmt(state->vmstate, "%s%s r%dd, r%dd", op, suffix, lr.code, rr.code);
                break;
            case TYPE_PRIMITIVE_S64:
            case TYPE_PRIMITIVE_U64:
                light_vm_push_fmt(state->vmstate, "%s%s r%d, r%d", op, suffix, lr.code, rr.code);
                break;
            case TYPE_PRIMITIVE_VOID:
                break;
            default: assert(0); break;
        }
    } else if(type->kind == TYPE_KIND_POINTER) {
        // TODO(psv):
        assert(0);
    }

    // Transfer value to the result  register
    *out_reg = alloc_register_for_expr(state, expr);
    light_vm_push_fmt(state->vmstate, "mov r%d, r%d", out_reg->code, lr.code);

    free_register(state, lr);
    free_register(state, rr);

    return first;
}


static Light_VM_Instruction_Info
bytecode_gen_expr_binary(Bytecode_State* state, Light_Ast* expr, Light_Register* out_reg) {
    const char* suffix = ""; // must be empty string
    const char* op = "";

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
        case OP_BINARY_AND:         op = "and"; suffix = ""; break;
        case OP_BINARY_OR:          op = "or";  suffix = ""; break;
        case OP_BINARY_XOR:         op = "xor"; suffix = ""; break;
        case OP_BINARY_SHL:         op = "shl"; suffix = ""; break;
        case OP_BINARY_SHR:         op = "shr"; suffix = ""; break;
        case OP_BINARY_LOGIC_AND:   op = "and"; suffix = ""; break;
        case OP_BINARY_LOGIC_OR:    op = "or";  suffix = ""; break;

        case OP_BINARY_EQUAL:
        case OP_BINARY_NOT_EQUAL: 
        case OP_BINARY_LT:
        case OP_BINARY_GT:
        case OP_BINARY_LE:
        case OP_BINARY_GE: {
            assert(expr->type == type_primitive_get(TYPE_PRIMITIVE_BOOL));
            return bytecode_gen_expr_binary_comparison(state, expr, out_reg);
        } break;

        case OP_BINARY_VECTOR_ACCESS: {
            //return bytecode_gen_expr_vector_access(state, expr, lr, rr);
        } break;
        default: assert(0); break;
    }

    return bytecode_gen_expr_binary_operation(state, expr, out_reg, op, suffix);
}

static Light_VM_Instruction_Info
bytecode_gen_expr_variable(Bytecode_State* state, Light_Ast* expr, Light_Register* out_reg) {
    Light_VM_Instruction_Info first = {0};
    Light_Type* type = type_alias_root(expr->type);

    Light_Register reg = alloc_register(state, LIGHT_REGISTER_INT, 64);
    if(out_reg) *out_reg = reg;

    // Load its address
    first = light_vm_push_fmt(state->vmstate, "mov r%d, rbp", reg.code);
    light_vm_push_fmt(state->vmstate, "adds r%d, 0x%x", reg.code, expr->expr_variable.decl->decl_variable.stack_offset);

    return first;
}

Light_VM_Instruction_Info
bytecode_gen_expr(Bytecode_State* state, Light_Ast* expr, Light_Register* out_reg) {
    Light_VM_Instruction_Info first = {0};

    switch(expr->kind) {
        case AST_EXPRESSION_BINARY:
            return bytecode_gen_expr_binary(state, expr, out_reg);
        case AST_EXPRESSION_UNARY:
            return bytecode_gen_expr_unary(state, expr, out_reg);
        case AST_EXPRESSION_LITERAL_PRIMITIVE: {
            return bytecode_gen_expr_literal_primitive(state, expr, out_reg);
        } break;
        case AST_EXPRESSION_LITERAL_ARRAY:
        case AST_EXPRESSION_LITERAL_STRUCT:
        case AST_EXPRESSION_VARIABLE: {
            return bytecode_gen_expr_variable(state, expr, out_reg);
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
bytecode_gen_assignment_to_variable(Bytecode_State* state, Light_Ast* decl) {
    assert(decl->kind == AST_DECL_VARIABLE);
    Light_VM_Instruction_Info first = {0};

    Light_Ast* expr = decl->decl_variable.assignment;
    Light_Type* expr_type = type_alias_root(expr->type);

    Light_Register result = {0};
    first = bytecode_gen_expr(state, expr, &result);

    // TODO(psv): verify if the variable is in the stack

    switch(expr_type->kind) {
        case TYPE_KIND_ENUM: // TODO(psv):
            assert(0); break;
        case TYPE_KIND_PRIMITIVE:
        case TYPE_KIND_POINTER:
        case TYPE_KIND_FUNCTION:{
            // all register size direct copies
            light_vm_push_fmt(state->vmstate, "mov [rbp + 0x%x], %s", decl->decl_variable.stack_offset, register_name(result));
        }break;
        case TYPE_KIND_ARRAY:
        case TYPE_KIND_STRUCT:
        case TYPE_KIND_UNION: {
            // copy r0, r1, r2 -> dst, src, size_bytes
            Light_Register variable_addr = alloc_register(state, LIGHT_REGISTER_INT, 64);
            Light_Register size = alloc_register(state, LIGHT_REGISTER_INT, 64);
            light_vm_push_fmt(state->vmstate, "mov %s, 0x%x", register_name(size), expr_type->size_bits / 8);

            light_vm_push_fmt(state->vmstate, "copy %s, %s, %s", 
                register_name(variable_addr), register_name(result), register_name(size));
            
            free_register(state, variable_addr);
            free_register(state, size);
        } break;
        default: assert(0); break;
    }

    free_register(state, result);
    return first;
}

Light_VM_Instruction_Info
bytecode_gen_assignment(Bytecode_State* state, Light_Ast* comm) {
    Light_VM_Instruction_Info first = {0};

    Light_Ast* left = comm->comm_assignment.lvalue;
    Light_Ast* right = comm->comm_assignment.rvalue;

    Light_Register lr = {0};
    Light_Register rr = {0};
    first = bytecode_gen_expr(state, left, &lr);
    bytecode_gen_expr(state, right, &rr);

    if(bytecode_expr_is_primitive(left)) {
        Light_Register expr_r = {0};
        bool r_dereferenced = false;
        bytecode_deref_expr(state, right, rr, &expr_r, &r_dereferenced);
        if(r_dereferenced) {
            free_register(state, rr);
            rr = expr_r;
        }
        // rr register contains the value dereferenced already
        light_vm_push_fmt(state->vmstate, "mov [%s], %s", register_name(lr), register_name(rr));
    } else {
        // TODO(psv): bigger types
    }

    return first;
}

// This function generates the code for expression evaluation into the
// specified register, the expression type must be of primitive or pointer types.
Light_VM_Instruction_Info
bytecode_copy_regsize_value(Bytecode_State* state, Light_Register* out_reg, Light_Ast* expr) {
    Light_VM_Instruction_Info first = bytecode_gen_expr(state, expr, out_reg);
    bool dereferenced = false;
    Light_Register result_reg = {0};
    bytecode_deref_expr(state, expr, *out_reg, &result_reg, &dereferenced);
    if(dereferenced) {
        free_register(state, *out_reg);
        *out_reg = result_reg;
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
                        bytecode_gen_assignment_to_variable(state, decl);
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
            Light_Register reg = {0};
            bytecode_copy_regsize_value(state, &reg, comm->comm_if.condition);
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
            Light_Register reg = {0};
            
            Light_VM_Instruction_Info start = 
            bytecode_gen_expr(state, comm->comm_while.condition, &reg);
            
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
                Light_Register ret_register = alloc_return_register_for_expr(state, ret_expr);
                Light_Register result = {0};

                Light_Type* expr_type = type_alias_root(ret_expr->type);
                switch(expr_type->kind) 
                {
                    case TYPE_KIND_PRIMITIVE:
                    case TYPE_KIND_POINTER:
                    case TYPE_KIND_FUNCTION:{
                        // Just copy to the return register the value
                        first = bytecode_copy_regsize_value(state, &result, ret_expr);
                        if(!register_equal(result, ret_register)) {
                            light_vm_push_fmt(state->vmstate, "mov %s, %s", register_name(ret_register), register_name(result));
                        }
                    } break;
                    case TYPE_KIND_ARRAY:
                    case TYPE_KIND_STRUCT:{
                        // TODO(psv):
                        assert(0);
                    } break;

                    case TYPE_KIND_UNION:
                    case TYPE_KIND_ENUM:
                        // TODO(psv):
                        assert(0);
                        break;
                    case TYPE_KIND_DIRECTIVE:
                    case TYPE_KIND_NONE:
                    case TYPE_KIND_ALIAS: assert(0); break;
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
                Light_Register reg = {0};
                first = bytecode_gen_expr(state, decl->decl_variable.assignment, &reg);
                light_vm_push_fmt(state->vmstate, "mov [rbp + 0x%x], r%d", reg.code);
                free_register(state, reg);
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
    light_vm_patch_from_to_current_instruction(state.vmstate, call);

    for(s32 i = 0; i < array_length(ast); ++i) {
        bytecode_gen_decl(&state, ast[i]);
    }

    //light_vm_patch_immediate_distance(call, entry);

    return state;
}