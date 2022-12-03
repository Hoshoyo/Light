#include <stdint.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define LVM_PTRSIZE 8

// 32-bit floating point registers
typedef enum {
    FR0, FR1, FR2, FR3, FR4, FR5, FR6, FR7,
    FREG_COUNT
} LVM_F32_Register;

// 64-bit floating point registers
typedef enum {
    EFR0, EFR1, EFR2, EFR3, EFR4, EFR5, EFR6, EFR7,
    EFREG_COUNT
} LVM_F64_Register;

typedef enum {
    // General purpose registers
    R0, R1, R2, R3, R4, R5, R6, R7,
    // Special registers
    LRSP, LRBP, LRIP, LRDP,
    R_COUNT,
} LVM_Register;

typedef enum {
    LVM_NOP = 0,
    // Arithmetic instructions
    LVM_ADD_S, LVM_SUB_S, LVM_MUL_S, LVM_DIV_S, LVM_MOD_S,
    LVM_ADD_U, LVM_SUB_U, LVM_MUL_U, LVM_DIV_U, LVM_MOD_U,
    LVM_NEG,
    // Mov
    LVM_MOV,
    // Bitwise
    LVM_SHL, LVM_SHR, LVM_OR, LVM_AND, LVM_XOR, 
    LVM_NOT,

    // Floating point
    LVM_FADD, LVM_FSUB, LVM_FMUL, LVM_FDIV, LVM_FMOV,
    LVM_FBEQ, LVM_FBNE, LVM_FBGT, LVM_FBLT, LVM_FBGE, LVM_FBLE,
    LVM_FNEG,

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

    LVM_MOVEQ,
    LVM_MOVNE,
    LVM_MOVLT_S,
    LVM_MOVGT_S,
    LVM_MOVLE_S,
    LVM_MOVGE_S,
    LVM_MOVLT_U,
    LVM_MOVGT_U,
    LVM_MOVLE_U,
    LVM_MOVGE_U,

    LVM_FMOVEQ, 
    LVM_FMOVNE, 
    LVM_FMOVGT, 
    LVM_FMOVLT, 
    LVM_FMOVGE, 
    LVM_FMOVLE,

    // Convert
    LVM_CVT_R32_S32,
    LVM_CVT_R32_S64,
    LVM_CVT_S32_R32,
    LVM_CVT_S32_R64,
    LVM_CVT_S64_R64,
    LVM_CVT_S64_R32,
    LVM_CVT_R32_R64,
    LVM_CVT_R64_R32,
    LVM_CVT_U64_R32,
    LVM_CVT_U64_R64,
    LVM_CVT_R64_S64,
    LVM_CVT_R64_S32,
    LVM_CVT_SEXT,

    // Jump
    LVM_JMP,   // Unconditionally

    // Proc
    LVM_CALL, LVM_PUSH, LVM_POP, LVM_RET,
    LVM_FPUSH, LVM_FPOP,
    LVM_EXPUSHI, LVM_EXPUSHF, LVM_EXPOP,
    LVM_EXTCALL,

    // Utils
    LVM_COPY, // copy r0, r1, r2 -> dst, src, size_bytes
    LVM_ALLOC,
    LVM_ASSERT,

    LVM_HLT, // Halt
} LVM_Instr_Type;

typedef enum {
    BIN_ADDR_MODE_REG_TO_REG,          // mov rax, rbx
    BIN_ADDR_MODE_REG_TO_MEM,          // mov [rax], rbx
    BIN_ADDR_MODE_REG_TO_IMM_MEM,      // mov [0x1234], rbx
    BIN_ADDR_MODE_REG_TO_MEM_OFFSETED, // mov [rax + 0x42], rbx
    BIN_ADDR_MODE_REG_OFFSETED_TO_REG, // mov rax, [rbx + 0x42]
    BIN_ADDR_MODE_MEM_TO_REG,          // mov rax, [rbx]
    BIN_ADDR_MODE_MEM_IMM_TO_REG,      // mov rax, [0x1234]
    BIN_ADDR_MODE_IMM_TO_REG,          // mov rax, 0x1234
} LVM_Binary_Addressing_Mode;

typedef enum {
    FLOAT_ADDR_MODE_REG_TO_REG,          // fadd fr0, fr1
    FLOAT_ADDR_MODE_REG_TO_MEM,          // fadd [r0], fr2
    FLOAT_ADDR_MODE_MEM_TO_REG,          // fadd fr0, [r1]
    FLOAT_ADDR_MODE_REG_OFFSETED_TO_REG, // fadd fr0, [r1 + 0x23]
    FLOAT_ADDR_MODE_REG_TO_MEM_OFFSETED, // fadd [r0 + 0x12], fr3
} LVM_Float_Addressing_Mode;

typedef enum {
    BRANCH_ADDR_MODE_IMMEDIATE_ABSOLUTE,  // call 0x1234567
    BRANCH_ADDR_MODE_IMMEDIATE_RELATIVE,  // call 0x32
    BRANCH_ADDR_MODE_REGISTER,            // call r0
    BRANCH_ADDR_MODE_REGISTER_INDIRECT,   // call [r0]
} LVM_Call_Addressing_Mode;

typedef enum {
    PUSH_ADDR_MODE_IMMEDIATE,
    PUSH_ADDR_MODE_IMMEDIATE_INDIRECT,
    PUSH_ADDR_MODE_REGISTER,
    PUSH_ADDR_MODE_REGISTER_INDIRECT,
} LVM_Push_Addressing_Mode;

typedef struct {
    uint32_t carry    : 1;
    uint32_t zerof    : 1;
    uint32_t sign     : 1;
    uint32_t overflow : 1;
} Light_VM_Flags_Register;

typedef struct {
    uint32_t bigger_than : 1;
    uint32_t less_than   : 1;
    uint32_t equal       : 1;
} Light_VM_Float_Flags_Register;

typedef struct {
    uint32_t src_reg     : 4;
    uint32_t dst_reg     : 4;
    uint32_t bytesize    : 4;
    uint32_t addr_mode   : 4;
} Light_VM_Instruction_Binary;

typedef struct {
    uint32_t reg       : 4;
    uint32_t byte_size : 4;
} Light_VM_Instruction_Unary;

typedef struct {
    // is always 64 bit
    uint32_t reg         : 4;
    uint32_t addr_mode   : 4;
} Light_VM_Instruction_Branch;

typedef struct {
    uint32_t src_reg     : 4;
    uint32_t dst_reg     : 4;
    uint32_t addr_mode   : 4;
    uint32_t is64bit     : 1;
} Light_VM_Instruction_Float;

typedef struct {
    uint32_t reg       : 4;
    uint32_t addr_mode : 4;
    uint32_t byte_size : 4;
} Light_VM_Instruction_Push;

typedef struct {
    uint32_t dst_reg        : 4;
    uint32_t src_reg        : 4;
    uint32_t size_bytes_reg : 4;
} Light_VM_Instruction_Copy;

typedef struct {
    uint32_t dst_reg   : 4;
    uint32_t size_reg  : 4;
    uint32_t byte_size : 4;
} Light_VM_Instruction_Alloc;

typedef struct {
    uint32_t dst_reg   : 4;
    uint32_t src_reg   : 4;
    uint32_t dst_size  : 4;
    uint32_t src_size  : 4;
} Light_VM_Instruction_SignExt;

typedef struct {
    uint8_t type;
    uint8_t imm_size_bytes;
    union {
        Light_VM_Instruction_SignExt    sext;
        Light_VM_Instruction_Binary     binary;
        Light_VM_Instruction_Unary      unary;
        Light_VM_Instruction_Float      ifloat;
        Light_VM_Instruction_Branch     branch;
        Light_VM_Instruction_Push       push;
        Light_VM_Instruction_Copy       copy;
        Light_VM_Instruction_Alloc    alloc;
    };
} Light_VM_Instruction;

// -------------------------------------
// ---------- Generation----------------
// -------------------------------------

typedef struct {
    uint8_t byte_size;
    union {
        uint8_t    unsigned_byte;
        uint16_t   unsigned_word;
        uint32_t   unsigned_dword;
        uint64_t   unsigned_qword;
        int8_t     signed_byte;
        int16_t    signed_word;
        int32_t    signed_dword;
        int64_t    signed_qword;
        float      float32;
        double     float64;
        void*      ptr;
    };
} Light_VM_Data;

// This struct cannot be changed
// It is directly linked to the external caller
// and therefore limits the number of arguments of
// an external call to 256
typedef struct {
    int32_t int_arg_count;
    int32_t float_arg_count;
    
    uint64_t int_values[256];
    uint64_t float_values[256];

    uint8_t  int_index[256];
    uint8_t  float_index[256];
} Light_VM_EXT_Stack;

typedef struct {
    int32_t size_bytes;
    void*   block;
} Memory;

typedef struct {
    uint64_t              offset_address;   // The address offset from the beginning of the code section.
    Light_VM_Instruction* absolute_address; // The absolute address when loaded into memory of the instruction.

    uint32_t   byte_size;                   // Size in bytes of the instruction only, without counting immediate values.
    uint32_t   immediate_byte_size;         // Size in bytes of the immediate value that follows the instruction, 0 if none follows.

    // Used in code generation
    int32_t    short_circuit_index;         // The short circuit index when the instruction is generated, later will be patched to jump to the appropriate place.
    int32_t    break_continue_level;        // The level of break or continue that the instruction needs to jump, is patched by the loop code generation.
} Light_VM_Instruction_Info;

typedef struct {
    uint64_t    address_offset;
    const char* lexical_start;
    int32_t     lexical_length;
} Light_VM_DebugInfo;

// State
typedef struct {
    Light_VM_Flags_Register       rflags;
    Light_VM_Float_Flags_Register rfloat_flags;
    uint64_t                      registers[R_COUNT];
    double                        f64registers[EFREG_COUNT];
    float                         f32registers[FREG_COUNT];
    Light_VM_EXT_Stack            ext_stack;
    Memory                        data;
    Memory                        stack;
    Memory                        heap;
    Memory                        code;

    // Used in code generation
    uint64_t                      data_offset;
    uint64_t                      code_offset;
} Light_VM_State;

Light_VM_State*           light_vm_init();
void                      light_vm_free(Light_VM_State* state);
Light_VM_Instruction_Info light_vm_push_instruction(Light_VM_State* vm_state, Light_VM_Instruction instr, uint64_t immediate);
Light_VM_Instruction_Info light_vm_push(Light_VM_State* vm_state, const char* instruction);
Light_VM_Instruction_Info light_vm_push_fmt(Light_VM_State* vm_state, const char* instruction, ...);
Light_VM_Instruction      light_vm_instruction_get(const char* s, uint64_t* immediate);
Light_VM_Instruction_Info light_vm_current_instruction(Light_VM_State* state);
void*                     light_vm_push_data_segment(Light_VM_State* vm_state, Light_VM_Data data);
void*                     light_vm_push_bytes_data_segment(Light_VM_State* vm_state, uint8_t* bytes, int32_t byte_count);
int64_t                   light_vm_patch_immediate_distance(Light_VM_Instruction_Info from, Light_VM_Instruction_Info to);
void                      light_vm_patch_instruction_immediate(Light_VM_Instruction_Info instr, int64_t value);
uint64_t                  light_vm_offset_from_current_instruction(Light_VM_State* state, Light_VM_Instruction_Info from);
uint8_t                   light_vm_patch_to_current_instruction(Light_VM_State* state, Light_VM_Instruction_Info to);
void                      light_vm_patch_from_to_current_instruction(Light_VM_State* state, Light_VM_Instruction_Info from);
void*                     light_vm_push_r32_to_datasegment(Light_VM_State* state, float f);
void*                     light_vm_push_r64_to_datasegment(Light_VM_State* state, double f);

// -------------------------------------
// ----------- Printing ----------------
// -------------------------------------
enum {
    LVM_PRINT_FLOATING_POINT_REGISTERS = (1 << 0),
    LVM_PRINT_DECIMAL                  = (1 << 1),
    LVM_PRINT_FLAGS_REGISTER           = (1 << 2),
}; 
void light_vm_print_instruction(FILE* out, Light_VM_Instruction instr, uint64_t imm);
void light_vm_debug_dump_registers(FILE* out, Light_VM_State* state, uint32_t flags);
void light_vm_debug_dump_code(FILE* out, Light_VM_State* state, Light_VM_DebugInfo* debug_info);
void light_vm_debug_dump_variables(FILE* out, Light_VM_State* state, uint32_t flags);

// -------------------------------------
// ----------- Execution ---------------
// -------------------------------------
void light_vm_execute(Light_VM_State* state, void* entry_point, int32_t print_steps);
void light_vm_execute_instruction(Light_VM_State* state, Light_VM_Instruction instr);
void light_vm_reset(Light_VM_State* state);

#if defined(__cplusplus)
} // extern "C"
#endif