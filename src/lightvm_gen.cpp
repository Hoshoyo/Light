#include "lightvm_gen.h"
#include "symbol_table.h"
#include <dynamic_array.h>
#include "type.h"

typedef struct {
    bool    int_register;
    uint8_t register_;
} Expression_Return;

void lvm_generate_node(Light_VM_State* state, Ast* node);

static int32_t
stack_calculate_size(Ast* block) {
    int32_t stack_size = 0;
    Scope* scope = block->scope;
    if (scope->decl_count > 0) {
		for (int i = 0; i < scope->symb_table.entries_capacity; ++i) {
			if (scope->symb_table.entries[i].occupied) {
				Ast* decl_node = scope->symb_table.entries[i].decl_node;
				if (decl_node->node_type == AST_DECL_VARIABLE) {
					assert(decl_node->decl_variable.size_bytes != 0);
					stack_size += decl_node->decl_variable.size_bytes;
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

#define BINARY_INSTRUCTION_GEN(S, U, F) if(type_primitive_int_signed(expr->expr_binary.left->type_return)) { \
    light_vm_push_fmt(state, S " %s, %s", get_register_name(left.register_), get_register_name(right.register_)); \
} else if(type_primitive_int_unsigned(expr->expr_binary.left->type_return)) { \
    light_vm_push_fmt(state, U " %s, %s", get_register_name(left.register_), get_register_name(right.register_)); \
} else { \
    light_vm_push_fmt(state, F " %s, %s", get_float_register_name(left.register_), get_float_register_name(right.register_)); \
}

#define BINARY_INT_INSTRUCTION_GEN(S, U) if(type_primitive_int_signed(expr->expr_binary.left->type_return)) { \
    light_vm_push_fmt(state, S " %s %s", get_register_name(left.register_), get_register_name(right.register_)); \
} else { \
    light_vm_push_fmt(state, U " %s %s", get_register_name(left.register_), get_register_name(right.register_)); \
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
            light_vm_push_fmt(state, "and %s %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;
        case OP_BINARY_OR:			// |
        {
            light_vm_push_fmt(state, "or %s %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;
        case OP_BINARY_XOR:			// ^
        {
            light_vm_push_fmt(state, "xor %s %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;
        case OP_BINARY_SHL:			// <<
        {
            light_vm_push_fmt(state, "shl %s %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;
        case OP_BINARY_SHR:			// >>
        {
            light_vm_push_fmt(state, "shr %s %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;

        case OP_BINARY_LT:			// <
        {
            light_vm_push_fmt(state, "cmp %s %s", get_register_name(left.register_), get_register_name(right.register_));
        }break;
        case OP_BINARY_GT:			// >
        {

        }break;
        case OP_BINARY_LE:			// <=
        case OP_BINARY_GE:			// >=
        case OP_BINARY_EQUAL:		// ==
        case OP_BINARY_NOT_EQUAL:	// !=

        case OP_BINARY_LOGIC_AND:	// &&
        case OP_BINARY_LOGIC_OR:	// ||

        case OP_BINARY_DOT: 			// .
        case OP_BINARY_VECTOR_ACCESS:   // []
    }
}

Expression_Return
lvm_generate_expr(Light_VM_State* state, Ast* expr) {
    Expression_Return result = {0};

    result.int_register = true;
    result.register_ = R0;

    switch(expr->node_type) {
        case AST_EXPRESSION_BINARY:
        case AST_EXPRESSION_LITERAL:
        case AST_EXPRESSION_PROCEDURE_CALL:
        case AST_EXPRESSION_UNARY:
        case AST_EXPRESSION_VARIABLE:
        default: assert(0); break;
    }

    light_vm_push_fmt(state, "mov r0, 0");

    return result;
}

void
lvm_generate_block(Light_VM_State* state, Ast* block) {
    assert(block->node_type == AST_COMMAND_BLOCK);

    for(size_t i = 0; i < block->comm_block.command_count; ++i) {
        lvm_generate_node(state, block->comm_block.commands[i]);
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
    node->decl_variable.stack_offset = node->scope->stack_allocation_offset;
    node->scope->stack_allocation_offset += node->decl_variable.size_bytes;
}

void
lvm_generate_procedure(Light_VM_State* state, Ast* proc) {
    assert(proc->node_type == AST_DECL_PROCEDURE);

    Light_VM_Instruction_Info entry = 
    light_vm_push(state, "push rbp");
    light_vm_push(state, "mov rbp, rsp");

    int32_t stack_size = stack_calculate_size(proc->decl_procedure.body);
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

        
        case AST_COMMAND_RETURN:
            lvm_generate_return(state, node);
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

    light_vm_execute(state, 0, 1);

    return state;
}