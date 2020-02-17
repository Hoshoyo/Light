#pragma once
#include "ast.h"

typedef enum {
    TAC_NONE = 0,
    TAC_EXPRESSION_BINARY_INT,
    TAC_EXPRESSION_BINARY_FLOAT,
    TAC_EXPRESSION_UNARY,
    TAC_EXPRESSION_ASSIGN_INT,
    TAC_EXPRESSION_ASSIGN_FLOAT,
} Tac_Node_Kind;

typedef enum {
    TAC_ATOM_NONE = 0,
    TAC_ATOM_VARIABLE,
    TAC_ATOM_TEMPORARY,
    TAC_ATOM_CONSTANT_INTEGER,
    TAC_ATOM_CONSTANT_FLOAT,
} Tac_Atom_Type;

typedef struct {
    s32 byte_size;
    union {
        u8  val_u8;
        u16 val_u16;
        u32 val_u32;
        u64 val_u64;
        s8  val_s8;
        s16 val_s16;
        s32 val_s32;
        s64 val_s64;
    };
} Tac_Constant_Integer;

typedef struct {
    s32 byte_size;
    union {
        r32 val_r32;
        r64 val_r64;
    };
} Tac_Constant_Float;

typedef struct {
    Tac_Atom_Type type;
    u32           flags;
    bool          is_signed;
    s32           byte_size;
    s32           offset_multiplier;
    union {
        Tac_Constant_Integer const_int;
        Tac_Constant_Float const_float;
        
        // variable
        struct {
            u64 variable_hash;
            Light_Token* variable;
        };

        s64 temporary_number;
    };
} Tac_Atom;

typedef struct {
    // + - * / < > <= >= == !=
    Light_Operator_Binary op;
    Tac_Atom left, e1, e2;
    s32 size_bytes;
} Tac_Binary_Assign_Float;

typedef struct {
    // + - * / % & | ^ << >>
    // < > <= >= == !=
    // && ||
    Light_Operator_Binary op;
    Tac_Atom left, e1, e2;
    s32 size_bytes;
} Tac_Binary_Assign_Integer;

typedef struct {
    // + - ~ !
    Light_Operator_Unary op;
    Tac_Atom left, e;
    s32 size_bytes;
} Tac_Unary_Assign_Integer;

typedef struct {
    // a = 1
    Tac_Atom left, e;
    s32 size_bytes;
} Tac_Simple_Assign_Integer;

typedef struct {
    // a = 1.0
    Tac_Atom left, e;
    s32 size_bytes;
} Tac_Simple_Assign_Float;

typedef struct {
    s32 byte_size;
    union {
        r32 val_r32;
        r64 val_r64;
    };
} Tac_Literal_Float;

typedef struct {
    s32 size_bytes;
    s32 variable_count;
    s32 temporary_count;
} Activation_Record;

typedef struct {
    Tac_Node_Kind kind;
    u32           flags;
    union {
        Tac_Binary_Assign_Integer binop_int;
        Tac_Binary_Assign_Float binop_float;
        Tac_Unary_Assign_Integer unop;
        Tac_Simple_Assign_Integer assign_int;
        Tac_Simple_Assign_Float assign_float;
    };
} Tac_Node;

typedef struct {
    Tac_Node** nodes;
} Tac_State;

Tac_State tac_generate(Light_Ast** ast);