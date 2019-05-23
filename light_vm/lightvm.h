#include "ast.h"
#include <stdio.h>
#include <assert.h>

typedef enum {
    // Floating point registers
    FR0, FR1, FR2, FR3, // 32 bit
    FR4, FR5, FR6, FR7, // 64 bit
    FREG_COUNT,
} Light_VM_FRegisters;

typedef enum {
    // General purpose registers
    R0, R1, R2, R3, R4, R5, R6, R7,
    // Special registers
    RSP, RBP, RIP, RDP,
    R_COUNT,
} Light_VM_Registers;

typedef enum {
    LVM_NOP = 0,
    // Arithmetic instructions
    LVM_ADD_S, LVM_SUB_S, LVM_MUL_S, LVM_DIV_S, LVM_MOD_S,
    LVM_ADD_U, LVM_SUB_U, LVM_MUL_U, LVM_DIV_U, LVM_MOD_U,
    // Mov
    LVM_MOV,
    // Bitwise
    LVM_SHL, LVM_SHR, LVM_OR, LVM_AND, LVM_XOR, 
    LVM_NOT,

    // Floating point
    LVM_FADD, LVM_FSUB, LVM_FMUL, LVM_FDIV, LVM_FMOV,
    LVM_FBEQ, LVM_FBNE, LVM_FBGT, LVM_FBLT,

    // Comparison/Branch
    LVM_FCMP,
    LVM_CMP,
    LVM_BEQ,   // Zero flag = 1
    LVM_BNE,   // Zero flag = 0
    LVM_BLT_S, // Sign Flag != Overflow Flag.
    LVM_BGT_S, // Zero Flag = 0 and Sign Flag = Overflow Flag
    LVM_BLE_S, // Zero Flag = 1 or Sign Flag != Overflow Flag
    LVM_BGE_S, // Sign Flag == Overflow Flag
    LVM_BLT_U, // Carry Flag = 1
    LVM_BGT_U, // Carry Flag = 0 and Zero Flag = 0
    LVM_BLE_U, // Carry Flag = 1 or Zero Flag = 1
    LVM_BGE_U, // Carry Flag = 0
    // Jump
    LVM_JMP, // Unconditionally

    // Proc
    LVM_CALL, LVM_PUSH, LVM_POP, LVM_RET,

    // Utils
    LVM_COPY, LVM_ASSERT, LVM_EXTCALL,

    LVM_HLT, // Halt
} Light_VM_Instruction_Type;

typedef struct {
    u32 carry    : 1;
    u32 zerof    : 1;
    u32 sign     : 1;
    u32 overflow : 1;
} Light_VM_Flags_Register;

typedef struct {
    u32 bigger_than : 1;
    u32 less_than   : 1;
    u32 equal       : 1;
} Light_VM_Float_Flags_Register;

typedef enum {
    BIN_ADDR_MODE_REG_TO_REG,     // mov rax, rbx
    BIN_ADDR_MODE_REG_TO_MEM,     // mov [rax], rbx
    BIN_ADDR_MODE_REG_TO_IMM_MEM, // mov [0x1234], rbx
    BIN_ADDR_MODE_REG_TO_MEM_OFFSETED, // mov [rax + 0x42], rbx
    BIN_ADDR_MODE_REG_OFFSETED_TO_REG, // mov rax, [rbx + 0x42]
    BIN_ADDR_MODE_MEM_TO_REG,     // mov rax, [rbx]
    BIN_ADDR_MODE_MEM_IMM_TO_REG, // mov rax, [0x1234]
    BIN_ADDR_MODE_IMM_TO_REG,     // mov rax, 0x1234
} Light_VM_Binary_Addressing_Mode;

typedef enum {
    FLOAT_ADDR_MODE_REG_TO_REG, // fadd fr0, fr1
    FLOAT_ADDR_MODE_REG_TO_MEM, // fadd [r0], fr2
    FLOAT_ADDR_MODE_MEM_TO_REG, // fadd fr0, [r1]
    FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG, // fadd fr0, [r1 + 0x23]
    FLOAT_ADDR_MODE_REG_TO_MEM_OFFSETED, // fadd [r0 + 0x12], fr3
} Light_VM_Float_Addressing_Mode;

typedef enum {
    BRANCH_ADDR_MODE_IMMEDIATE_ABSOLUTE,  // call 0x1234567
    BRANCH_ADDR_MODE_IMMEDIATE_RELATIVE,  // call 0x32
    BRANCH_ADDR_MODE_REGISTER,            // call r0
    BRANCH_ADDR_MODE_REGISTER_INDIRECT,   // call [r0]
} Light_VM_Call_Addressing_Mode;

typedef struct {
    u32 src_reg     : 4; // 4
    u32 dst_reg     : 4; // 8
    u32 bytesize    : 4; // 12
    u32 addr_mode   : 4; // 20
    u32 sign        : 1; // 1 negative, 0 positive
} Light_VM_Instruction_Binary;

typedef struct {
    u32 reg       : 4; // 4
    u32 byte_size : 4; // 8
} Light_VM_Instruction_Unary;

typedef struct {
    // is always 64 bit
    u32 reg         : 4; // 4
    u32 addr_mode   : 4; // 8
} Light_VM_Instruction_Branch;

typedef struct {
    u32 src_reg     : 4; // 4
    u32 dst_reg     : 4; // 8
    u32 sign        : 4; // 12 -> 0 positive, 1 negative
    u32 addr_mode   : 4; // 16
} Light_VM_Instruction_Float;

typedef struct {
    u8 type;
    u8 imm_size_bytes;
    union {
        Light_VM_Instruction_Binary     binary;
        Light_VM_Instruction_Unary      unary;
        Light_VM_Instruction_Float      ifloat;
        Light_VM_Instruction_Branch       branch;
    };
} Light_VM_Instruction;

// -------------------------------------
// ---------- Generation----------------
// -------------------------------------
typedef struct {
    s32   size;
    void* block;
} Memory;

// State
typedef struct {
    Light_VM_Flags_Register       rflags;
    Light_VM_Float_Flags_Register rfloat_flags;
    u64    registers[R_COUNT];
    r64    f64registers[FREG_COUNT];
    r32    f32registers[FREG_COUNT];
    Memory data;
    u64 data_offset;
    Memory stack;
    Memory heap;
    Memory code;
    u64 code_offset;
} Light_VM_State;

typedef struct {
    u64   offset_address;
    void* absolute_address;
    u32   byte_size;           // instruction only
    u32   immediate_byte_size; // immediate value only
} Light_VM_Instruction_Info;

typedef struct {
    u8 byte_size;
    union {
        u8  unsigned_byte;
        u16 unsigned_word;
        u32 unsigned_dword;
        u64 unsigned_qword;
        s8  signed_byte;
        s16 signed_word;
        s32 signed_dword;
        s64 signed_qword;
        r32 float32;
        r64 float64;
    };
} Light_VM_Data;

Light_VM_State*           light_vm_init();
Light_VM_Instruction_Info light_vm_push_instruction(Light_VM_State* vm_state, Light_VM_Instruction instr, u64 immediate);
Light_VM_Instruction_Info light_vm_push(Light_VM_State* vm_state, const char* instruction);
Light_VM_Instruction_Info light_vm_push_fmt(Light_VM_State* vm_state, const char* instruction, ...);
Light_VM_Instruction      light_vm_instruction_get(const char* s, u64* immediate);
void*                     light_vm_push_data_segment(Light_VM_State* vm_state, Light_VM_Data data);
s32                       light_vm_patch_immediate_distance(Light_VM_Instruction_Info from, Light_VM_Instruction_Info to);
void*                     light_vm_push_r32_to_datasegment(Light_VM_State* state, r32 f);
void*                     light_vm_push_r64_to_datasegment(Light_VM_State* state, r64 f);

// -------------------------------------
// ----------- Printing ----------------
// -------------------------------------
enum {
    LVM_PRINT_FLOATING_POINT_REGISTERS = FLAG(0),
    LVM_PRINT_DECIMAL                  = FLAG(1),
    LVM_PRINT_FLAGS_REGISTER           = FLAG(2),
}; 
void light_vm_print_instruction(FILE* out, Light_VM_Instruction instr, u64 imm);
void light_vm_debug_dump_registers(FILE* out, Light_VM_State* state, u32 flags);
void light_vm_debug_dump_code(FILE* out, Light_VM_State* state);

// -------------------------------------
// ----------- Execution ---------------
// -------------------------------------
void light_vm_execute(Light_VM_State* state, bool print_steps);
void light_vm_execute_instruction(Light_VM_State* state, Light_VM_Instruction instr);