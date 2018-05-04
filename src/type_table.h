#pragma once
#include "type.h"

void type_table_init();

u64 type_hash(Type_Instance* type);
Type_Instance* type_primitive_get(Type_Primitive p);
Type_Instance* type_new_temporary();
s64            type_pointer_size();
s64            type_pointer_size_bits();

Type_Instance* internalize_type(Type_Instance** type, Scope* scope, bool copy = true);
void resolve_type_internalize_queue();

extern Type_Instance** g_type_table;

void DEBUG_print_type_table();