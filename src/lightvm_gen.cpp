#include "lightvm_gen.h"
#include "symbol_table.h"
#include <dynamic_array.h>
#include "type.h"

typedef struct {
    bool    int_register;
    uint8_t register_;
} Expression_Return;

static LVM_Gen_Register_Allocation reg_alloc;

Expression_Return lvm_generate_expr(Light_VM_State* state, Ast* expr);
void              lvm_generate_node(Light_VM_State* state, Ast* node);

static u8
lvm_allocate_f32_register(LVM_Gen_Register_Allocation* info) {
    for(u8 i = FR0; i < FR4; ++i) {
        if(!(info->f32_registers[i].flags & LVM_REGISTER_ALLOCATED)) {
            info->f32_registers[i].flags |= LVM_REGISTER_ALLOCATED;
            return i;
        }
    }
}
static void
lvm_free_f32_register(u8 reg, LVM_Gen_Register_Allocation* info) {
    info->f32_registers[reg].flags &= ~(LVM_REGISTER_ALLOCATED);
}

static u8
lvm_allocate_f64_register(LVM_Gen_Register_Allocation* info) {
    for(u8 i = FR4; i <= FR7; ++i) {
        if(!(info->f64_registers[i].flags & LVM_REGISTER_ALLOCATED)) {
            info->f64_registers[i].flags |= LVM_REGISTER_ALLOCATED;
            return i;
        }
    }
}
static void
lvm_free_f64_register(u8 reg, LVM_Gen_Register_Allocation* info) {
    info->f64_registers[reg].flags &= ~(LVM_REGISTER_ALLOCATED);
}

static u8
lvm_allocate_int_register(LVM_Gen_Register_Allocation* info) {
    for(u8 i = 0; i < R7+1; ++i) {
        if(!(info->int_registers[i].flags & LVM_REGISTER_ALLOCATED)) {
            info->int_registers[i].flags |= LVM_REGISTER_ALLOCATED;
            return i;
        }
    }
}

static void
lvm_free_int_register(u8 reg, LVM_Gen_Register_Allocation* info) {
    info->int_registers[reg].flags &= ~(LVM_REGISTER_ALLOCATED);
}

static int32_t
stack_calculate_size(Ast* block) {
    int32_t stack_size = 0;
    Scope* scope = block->comm_block.block_scope;
    if (scope->decl_count > 0) {
		for (int i = 0; i < scope->symb_table.entries_capacity; ++i) {
			if (scope->symb_table.entries[i].occupied) {
				Ast* decl_node = scope->symb_table.entries[i].decl_node;
				if (decl_node->node_type == AST_DECL_VARIABLE) {
					assert(decl_node->decl_variable.variable_type->type_size_bits != 0);
					stack_size += (decl_node->decl_variable.variable_type->type_size_bits / 8);
				}
			}
		}
	}

    return stack_size;
}

static Light_VM_Instruction_Info entry_point;

static char* 
get_float_register_name(u8 reg) {
    switch(reg) {
        case FR0: return "fr0";
        case FR1: return "fr1";
        case FR2: return "fr2";
        case FR3: return "fr3";
        case FR4: return "fr4";
        case FR5: return "fr5";
        case FR6: return "fr6";
        case FR7: return "fr7";
        default: assert(0); break;
    }
}

static char* 
get_register_name(u8 reg) {
    switch(reg) {
        case R0:  return "r0";
        case R1:  return "r1";
        case R2:  return "r2";
        case R3:  return "r3";
        case R4:  return "r4";
        case R5:  return "r5";
        case R6:  return "r6";
        case R7:  return "r7";
        case RSP: return "rsp";
        case RBP: return "rbp";
        case RDP: return "rdp";
        case RIP: return "rip";
        default: assert(0); break;
    }
}

#define COPY_STR_TO_BUFFER(B, S) memcpy(B, S, sizeof(S) - 1)

static void
get_register_name_sized(char* buffer, u8 reg, s32 size_bytes) {
    s32 index = 0;
    switch(reg) {
        case R0:  COPY_STR_TO_BUFFER(buffer, "r0"); index += 2; break;
        case R1:  COPY_STR_TO_BUFFER(buffer, "r1"); index += 2; break;
        case R2:  COPY_STR_TO_BUFFER(buffer, "r2"); index += 2; break;
        case R3:  COPY_STR_TO_BUFFER(buffer, "r3"); index += 2; break;
        case R4:  COPY_STR_TO_BUFFER(buffer, "r4"); index += 2; break;
        case R5:  COPY_STR_TO_BUFFER(buffer, "r5"); index += 2; break;
        case R6:  COPY_STR_TO_BUFFER(buffer, "r6"); index += 2; break;
        case R7:  COPY_STR_TO_BUFFER(buffer, "r7"); index += 2; break;
        case RSP: COPY_STR_TO_BUFFER(buffer, "rsp"); index += 3; break;
        case RBP: COPY_STR_TO_BUFFER(buffer, "rbp"); index += 3; break;
        case RDP: COPY_STR_TO_BUFFER(buffer, "rdp"); index += 3; break;
        case RIP: COPY_STR_TO_BUFFER(buffer, "rip"); index += 3; break;
        default: assert(0); break;
    }
    switch(size_bytes) {
        case 1: buffer[index] = 'b'; index++; break;
        case 2: buffer[index] = 'w'; index++; break;
        case 4: buffer[index] = 'd'; index++; break;
        case 8: break;
        default: assert(0); break;
    }
    buffer[index] = 0;
}

#define BINARY_INSTRUCTION_GEN(S, U, F) if(type_primitive_int_signed(expr->expr_binary.left->type_return)) { \
    light_vm_push_fmt(state, S " %s, %s", get_register_name(left.register_), get_register_name(right.register_)); \
} else if(type_primitive_int_unsigned(expr->expr_binary.left->type_return)) { \
    light_vm_push_fmt(state, U " %s, %s", get_register_name(left.register_), get_register_name(right.register_)); \
} else { \
    light_vm_push_fmt(state, F " %s, %s", get_float_register_name(left.register_), get_float_register_name(right.register_)); \
}

#define BINARY_INT_INSTRUCTION_GEN(S, U) if(type_primitive_int_signed(expr->expr_binary.left->type_return)) { \
    light_vm_push_fmt(state, S " %s, %s", get_register_name(left.register_), get_register_name(right.register_)); \
} else { \
    light_vm_push_fmt(state, U " %s, %s", get_register_name(left.register_), get_register_name(right.register_)); \
}

#define BINARY_INT_COMPARISON_GEN(S, U) light_vm_push_fmt(state, "cmp %s, %s", get_register_name(left.register_), get_register_name(right.register_)); \
if(type_primitive_int_signed(expr->type_return)) { \
    light_vm_push_fmt(state, " %s", get_register_name(left.register_)); \
} else { \
    light_vm_push_fmt(state, " %s", get_register_name(left.register_)); \
}

Expression_Return
lvm_generate_expr_literal(Light_VM_State* state, Ast* expr) {
    Expression_Return result = {0};
    switch(expr->expr_literal.type) {
        case LITERAL_UNKNOWN: assert(0); break;
        case LITERAL_BIN_INT:
        case LITERAL_HEX_INT:
        case LITERAL_SINT:
        case LITERAL_UINT:{
            u8 allocated = lvm_allocate_int_register(&reg_alloc);
            light_vm_push_fmt(state, "mov %s, 0x%lx", get_register_name(allocated), expr->expr_literal.value_u64);
            result.int_register = true;
            result.register_ = allocated;
        }break;

        case LITERAL_CHAR:
        case LITERAL_BOOL:
        case LITERAL_FLOAT:
        case LITERAL_POINTER:
        case LITERAL_STRUCT:
            assert(0);
            break;
        default: break;
    }

    return result;
}

Expression_Return
lvm_generate_expr_binary(Light_VM_State* state, Ast* expr) {
    Expression_Return left = lvm_generate_expr(state, expr->expr_binary.left);
    Expression_Return right = lvm_generate_expr(state, expr->expr_binary.right);

    switch(expr->expr_binary.op){
        case OP_BINARY_PLUS:		// +
        {
            BINARY_INSTRUCTION_GEN("adds", "addu", "fadd")
        }break;
        case OP_BINARY_MINUS:		// -
        {
            BINARY_INSTRUCTION_GEN("subs", "subu", "fsub")
        }break;
        case OP_BINARY_MULT:		// *
        {
            BINARY_INSTRUCTION_GEN("muls", "mulu", "fmul")
        }break;
        case OP_BINARY_DIV:			// /
        {
            BINARY_INSTRUCTION_GEN("divs", "divu", "fdiv")
        }break;
        case OP_BINARY_MOD:			// %
        {
            BINARY_INT_INSTRUCTION_GEN("mods", "modu")
        }break;
        case OP_BINARY_AND:			// &
        {
            light_vm_push_fmt(state, "and %s, %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;
        case OP_BINARY_OR:			// |
        {
            light_vm_push_fmt(state, "or %s, %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;
        case OP_BINARY_XOR:			// ^
        {
            light_vm_push_fmt(state, "xor %s, %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;
        case OP_BINARY_SHL:			// <<
        {
            light_vm_push_fmt(state, "shl %s, %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;
        case OP_BINARY_SHR:			// >>
        {
            light_vm_push_fmt(state, "shr %s, %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;

        case OP_BINARY_LT:			// <
        {
            BINARY_INT_COMPARISON_GEN("movlts", "movltu")
        }break;
        case OP_BINARY_GT:			// >
        {
            BINARY_INT_COMPARISON_GEN("movgts", "movgtu")
        }break;
        case OP_BINARY_LE:			// <=
        {
            BINARY_INT_COMPARISON_GEN("movles", "movleu")
        }break;
        case OP_BINARY_GE:			// >=
        {
            BINARY_INT_COMPARISON_GEN("movges", "movgeu")
        }break;
        case OP_BINARY_EQUAL:		// ==
        {
            light_vm_push_fmt(state, "cmp %s, %s", get_register_name(left.register_), get_register_name(right.register_));
            light_vm_push_fmt(state, "moveq %s", get_register_name(left.register_));
        }break;
        case OP_BINARY_NOT_EQUAL:	// !=
        {
            light_vm_push_fmt(state, "cmp %s, %s", get_register_name(left.register_), get_register_name(right.register_));
            light_vm_push_fmt(state, "movne %s", get_register_name(left.register_));
        }break;

        // Logic

        case OP_BINARY_LOGIC_AND:	// &&
        {
            light_vm_push_fmt(state, "cmp %s, %s", get_register_name(left.register_), get_register_name(right.register_));
            light_vm_push_fmt(state, "moveq %s", get_register_name(left.register_));
        }break;
        case OP_BINARY_LOGIC_OR:	// ||
        {
            light_vm_push_fmt(state, "or %s, %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;

        case OP_BINARY_DOT: 			// .
        {
            assert(0);
        }break;
        case OP_BINARY_VECTOR_ACCESS:   // []
        {
            assert(0);
        }break;
    }
    
    if(right.int_register) {
        lvm_free_int_register(right.register_, &reg_alloc);
    } else {
        //lvm_free_float_register(right.register_, &reg_alloc);
    }
    return left;
}

Expression_Return
lvm_generate_expr_variable(Light_VM_State* state, Ast* expr) {
    Expression_Return result = {0};

    Ast* decl = expr->expr_variable.decl;
    s32 stack_offset = decl->decl_variable.stack_offset;

    switch(expr->type_return->kind) {
        case KIND_POINTER:{
            u8 reg = lvm_allocate_int_register(&reg_alloc);
            if(stack_offset >= 0) {
                light_vm_push_fmt(state, "mov %s, [rsp + 0x%x]", get_register_name(reg), stack_offset);
            } else {
                light_vm_push_fmt(state, "mov %s, [rsp - 0x%x]", get_register_name(reg), -stack_offset);
            }
            result.int_register = true;
            result.register_ = reg;
        }break;
        case KIND_PRIMITIVE: {
            switch(expr->type_return->primitive) {
                case TYPE_PRIMITIVE_BOOL:
                case TYPE_PRIMITIVE_S8:
                case TYPE_PRIMITIVE_S16:
                case TYPE_PRIMITIVE_S32:
                case TYPE_PRIMITIVE_S64:
                case TYPE_PRIMITIVE_U8:
                case TYPE_PRIMITIVE_U16:
                case TYPE_PRIMITIVE_U32:
                case TYPE_PRIMITIVE_U64:{
                    u8 reg = lvm_allocate_int_register(&reg_alloc);
                    char register_name[8] = {0};
                    get_register_name_sized(register_name, reg, expr->type_return->type_size_bits / 8);
                    if(stack_offset >= 0) {
                        light_vm_push_fmt(state, "mov %s, [rsp + 0x%x]", register_name, stack_offset);
                    } else {
                        light_vm_push_fmt(state, "mov %s, [rsp - 0x%x]", register_name, -stack_offset);
                    }
                    result.int_register = true;
                    result.register_ = reg;
                }break;
                case TYPE_PRIMITIVE_R32:{
                    u8 reg = lvm_allocate_f32_register(&reg_alloc);
                    if(stack_offset >= 0) {
                        light_vm_push_fmt(state, "fmov %s, [rsp + 0x%x]", get_float_register_name(reg), stack_offset);
                    } else {
                        light_vm_push_fmt(state, "fmov %s, [rsp - 0x%x]", get_float_register_name(reg), -stack_offset);
                    }
                    result.int_register = false;
                    result.register_ = reg;
                }break;
                case TYPE_PRIMITIVE_R64:{
                    u8 reg = lvm_allocate_f64_register(&reg_alloc);
                    if(stack_offset >= 0) {
                        light_vm_push_fmt(state, "fmov %s, [rsp + 0x%x]", get_float_register_name(reg), stack_offset);
                    } else {
                        light_vm_push_fmt(state, "fmov %s, [rsp - 0x%x]", get_float_register_name(reg), -stack_offset);
                    }
                    result.int_register = false;
                    result.register_ = reg;
                }break;
                case TYPE_PRIMITIVE_VOID: assert(0); break;
            }
        }break;

        case KIND_FUNCTION: // TODO(psv):
        case KIND_STRUCT:
        case KIND_UNION:
        case KIND_UNKNOWN: assert(0); break;
    }
    return result;
}

Expression_Return
lvm_generate_expr_unary(Light_VM_State* state, Ast* expr) {
    Expression_Return result = {0};

    Expression_Return operand = lvm_generate_expr(state, expr->expr_unary.operand);

    switch(expr->expr_unary.op) {
        case OP_UNARY_UNKNOWN: assert(0); break;
        case OP_UNARY_BITWISE_NOT:{
            light_vm_push_fmt(state, "not %s", get_register_name(result.register_));
        } break;
        case OP_UNARY_LOGIC_NOT:{
            light_vm_push_fmt(state, "cmp %s, 0x0", get_register_name(result.register_));
            light_vm_push_fmt(state, "mov %s, 0x0", get_register_name(result.register_));
            light_vm_push_fmt(state, "movne %s", get_register_name(result.register_));
        }break;

        case OP_UNARY_MINUS:{
            if(type_primitive_int_signed(expr->type_return)) {
                u8 allocated = lvm_allocate_int_register(&reg_alloc);
                light_vm_push_fmt(state, "mov %s, 0x0", get_register_name(allocated));
                light_vm_push_fmt(state, "subs %s, %s", get_register_name(allocated), get_register_name(operand.register_));
                lvm_free_int_register(operand.register_, &reg_alloc);
                result.int_register = true;
                result.register_ = allocated;
            } else if(type_primitive_int_unsigned(expr->type_return)) {
                u8 allocated = lvm_allocate_int_register(&reg_alloc);
                light_vm_push_fmt(state, "mov %s, 0x0", get_register_name(allocated));
                light_vm_push_fmt(state, "subu %s, %s", get_register_name(allocated), get_register_name(operand.register_));
                lvm_free_int_register(operand.register_, &reg_alloc);
                result.int_register = true;
                result.register_ = allocated;
            } else if(type_primitive_float(expr->type_return)) {
                // TODO(psv):
            }
        }break;
        case OP_UNARY_PLUS: break; // do nothing

        case OP_UNARY_CAST:
        case OP_UNARY_ADDRESSOF:
        case OP_UNARY_DEREFERENCE:
            assert(0);
            break;
        default: assert(0); break;
    }

    return result;
}

Expression_Return
lvm_generate_expr(Light_VM_State* state, Ast* expr) {
    Expression_Return result = {0};

    switch(expr->node_type) {
        case AST_EXPRESSION_BINARY:
            result = lvm_generate_expr_binary(state, expr);
            break;
        case AST_EXPRESSION_LITERAL:
            result = lvm_generate_expr_literal(state, expr);
            break;
        case AST_EXPRESSION_UNARY:
            result = lvm_generate_expr_unary(state, expr);
            break;
        case AST_EXPRESSION_VARIABLE:
            result = lvm_generate_expr_variable(state, expr);
            break;
        case AST_EXPRESSION_PROCEDURE_CALL:
        default: assert(0); break;
    }

    return result;
}

void
lvm_generate_block(Light_VM_State* state, Ast* block) {
    assert(block->node_type == AST_COMMAND_BLOCK);

    for(size_t i = 0; i < block->comm_block.command_count; ++i) {
        lvm_generate_node(state, block->comm_block.commands[i]);
    }
}

void
lvm_generate_assignment(Light_VM_State* state, Ast* node) {
    assert(node->node_type == AST_COMMAND_VARIABLE_ASSIGNMENT);

    Expression_Return lvalue = lvm_generate_expr(state, node->comm_var_assign.lvalue);
    assert(lvalue.int_register); // must be a pointer

    if(node->type_return->type_size_bits <= 64) {
        Expression_Return rvalue = lvm_generate_expr(state, node->comm_var_assign.rvalue);
        light_vm_push_fmt(state, "mov [%s], %s", 
            get_register_name(lvalue.register_), 
            get_register_name(rvalue.register_));
    } else {
        // bigger than register size
        // TODO(psv):
        // generate the expression directly in the register memory
    }
}

void lvm_generate_for(Light_VM_State* state, Ast* node) {
    assert(node->node_type == AST_COMMAND_FOR);

    Expression_Return boolexpr = lvm_generate_expr(state, node->comm_for.condition);
    assert(boolexpr.int_register);

    light_vm_push_fmt(state, "cmp %s, 0x1", get_register_name(boolexpr.register_));
    Light_VM_Instruction_Info branch_condition = 
    light_vm_push_fmt(state, "bne 0xffffffff");
    lvm_free_int_register(boolexpr.register_, &reg_alloc);

    // if true
    lvm_generate_node(state, node->comm_for.body);

    // go back to the loop beginning
    Light_VM_Instruction_Info end_loop = light_vm_push(state, "jmp 0xffffffff");
    light_vm_patch_immediate_distance(end_loop, branch_condition);

    // after loop
    light_vm_patch_from_to_current_instruction(state, branch_condition);

    // TODO(psv): patch breaks and continues here
}

void lvm_generate_if(Light_VM_State* state, Ast* node) {
    assert(node->node_type == AST_COMMAND_IF);

    Expression_Return boolexpr = lvm_generate_expr(state, node->comm_if.condition);
    assert(boolexpr.int_register);

    light_vm_push_fmt(state, "cmp %s, 0x1", get_register_name(boolexpr.register_));
    Light_VM_Instruction_Info branch_condition = 
    light_vm_push_fmt(state, "bne 0xffffffff");
    lvm_free_int_register(boolexpr.register_, &reg_alloc);

    // if true
    lvm_generate_node(state, node->comm_if.body_true);

    Light_VM_Instruction_Info true_end = {0};
    if(node->comm_if.body_false) {
        true_end = light_vm_push(state, "jmp 0xffffffff");
    }

    light_vm_patch_from_to_current_instruction(state, branch_condition);

    // if false
    if(node->comm_if.body_false) {
        lvm_generate_node(state, node->comm_if.body_false);
        light_vm_patch_from_to_current_instruction(state, true_end);
    }
}

void lvm_generate_return(Light_VM_State* state, Ast* node) {
    assert(node->node_type == AST_COMMAND_RETURN);

    if(node->comm_return.expression) {
        Expression_Return r = lvm_generate_expr(state, node->comm_return.expression);
        if(r.int_register) {
            if(r.register_ != R0)
                light_vm_push_fmt(state, "mov r0, %s", get_register_name(r.register_));
        } else {
            if(r.register_ <= FR3){
                if(r.register_ != FR0)
                    light_vm_push_fmt(state, "mov fr0, %s", get_float_register_name(r.register_));
            } else {
                if(r.register_ != FR4)
                    light_vm_push_fmt(state, "mov fr4, %s", get_float_register_name(r.register_));
            }
        }
    }

    light_vm_push(state, "mov rsp, rbp");
    light_vm_push(state, "pop rbp");

    light_vm_push(state, "ret");
}

void
lvm_generate_decl_variable(Light_VM_State* state, Ast* node) {
    assert(node->node_type == AST_DECL_VARIABLE);
    //node->scope->stack_allocation_offset += 
    node->decl_variable.stack_offset = node->scope->stack_current_offset;
    node->scope->stack_current_offset += (node->decl_variable.variable_type->type_size_bits / 8);
}

void
lvm_generate_procedure(Light_VM_State* state, Ast* proc) {
    assert(proc->node_type == AST_DECL_PROCEDURE);

    Light_VM_Instruction_Info entry = 
    light_vm_push(state, "push rbp");
    light_vm_push(state, "mov rbp, rsp");

    Scope* scope = proc->decl_procedure.body->comm_block.block_scope;
    int32_t stack_size = scope->stack_allocation_offset;
    scope->stack_current_offset = 0;
    
    if(stack_size > 0)
        light_vm_push_fmt(state, "adds rsp, %d", stack_size);

    lvm_generate_block(state, proc->decl_procedure.body);

    light_vm_push(state, "mov rsp, rbp");
    light_vm_push(state, "pop rbp");

    light_vm_push(state, "ret");

    if(proc->decl_procedure.flags & DECL_PROC_FLAG_MAIN) {
        light_vm_patch_immediate_distance(entry_point, entry);
    }
}

void
lvm_generate_node(Light_VM_State* state, Ast* node) {
    switch(node->node_type) {
        // Declarations
        case AST_DECL_CONSTANT:
        case AST_DECL_STRUCT:
        case AST_DECL_UNION:
        case AST_DECL_TYPEDEF:
            break;
        case AST_DECL_PROCEDURE: {
            lvm_generate_procedure(state, node);
        } break;
        case AST_DECL_VARIABLE:
            lvm_generate_decl_variable(state, node);
            break;

        
        case AST_COMMAND_BLOCK:
            lvm_generate_block(state, node);
            break;
        case AST_COMMAND_RETURN:
            lvm_generate_return(state, node);
            break;
        case AST_COMMAND_IF: 
            lvm_generate_if(state, node);
            break;
        case AST_COMMAND_FOR:
            lvm_generate_for(state, node);
            break;
        case AST_COMMAND_VARIABLE_ASSIGNMENT:
            lvm_generate_assignment(state, node);
            break;
        default: break;
    }
}

Light_VM_State*
lvm_generate(Ast** ast) {
    Light_VM_State* state = light_vm_init();

    entry_point = 
    light_vm_push(state, "call 0xffffffff");
    light_vm_push(state, "hlt");

    for(size_t i = 0; i < array_get_length(ast); ++i) {
        Ast* node = ast[i];
        lvm_generate_node(state, node);
    }

    light_vm_debug_dump_code(stdout, state);
    light_vm_execute(state, 0, 1);
    light_vm_debug_dump_registers(stdout, state, LVM_PRINT_DECIMAL);

    return state;
}