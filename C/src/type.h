#pragma once
#include "lexer.h"
#include "ast.h"
#include <stdint.h>

extern Light_Type* global_type_empty_struct;
extern Light_Type* global_type_empty_union;
extern Light_Type* global_type_empty_enum;

void        type_tables_initialize();
void        type_table_print();
Light_Type* type_internalize(Light_Type* type);
Light_Type* type_alias_by_name(Light_Token* name);
Light_Type* type_primitive_from_token(Light_Token_Type token);
Light_Type* type_weak_primitive_from_literal(Light_Literal_Type literal);
Light_Type* type_from_token(Light_Token* t);
Light_Type* type_primitive_get(Light_Type_Primitive t);
Light_Type* type_alias_root(Light_Type* type);
Light_Type* type_get_bigger(Light_Type* t1, Light_Type* t2);
bool        type_primitive_sint(Light_Type* t);
bool        type_primitive_uint(Light_Type* t);
bool        type_primitive_int(Light_Type* t);
bool        type_primitive_float(Light_Type* t);
bool        type_primitive_numeric(Light_Type* t);
bool        type_primitive_bool(Light_Type* t);

u64         type_hash(Light_Type* type);
bool        type_check_equality(Light_Type* t1, Light_Type* t2);

// Create

// type_new_pointer returns an internalized pointer type if the argument type is internalized
Light_Type* type_new_pointer(Light_Type* pointer_to);
Light_Type* type_new_alias(Light_Token* name, Light_Type* alias_to);
Light_Type* type_new_array(Light_Ast* dimension, Light_Type* type, Light_Token* token_array);
Light_Type* type_new_function(Light_Type** arguments_types, Light_Type* return_type, s32 arguments_count, bool all_args_internalized);
Light_Type* type_new_enum(Light_Ast** fields, s32 fields_count, Light_Type* type_hint, Light_Scope* enum_scope);
Light_Type* type_new_struct(Light_Ast** fields, s32 fields_count, Light_Scope* struct_scope);
Light_Type* type_new_union(Light_Ast** fields, s32 fields_count, Light_Scope* union_scope);