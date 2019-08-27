#pragma once
#include "../../ast.h"
#include "../../global_tables.h"

typedef struct {
	struct User_Type_Info_t* array_of;
    u64 dimension;
} User_Type_Array;

typedef struct {
	string name;
	struct User_Type_Info_t** fields_types;
	string* fields_names;
	s32 fields_count;
	s32 alignment;
} User_Type_Union;

typedef struct {
	string name;
	struct User_Type_Info_t** fields_types;
	string* fields_names;
	s64* fields_offsets_bits;
	s32 fields_count;
	s32 alignment;
} User_Type_Struct;

typedef struct {
	struct User_Type_Info_t* return_type;
	struct User_Type_Info_t** arguments_type;
	string* arguments_name;
	s32 arguments_count;
} User_Type_Function;

typedef union {
    Light_Type_Primitive primitive;
    struct User_Type_Info_t* pointer_to;
    User_Type_Array array_desc;
    User_Type_Struct struct_desc;
    User_Type_Union union_desc;
    User_Type_Function function_desc;
} User_Type_Desc;

typedef struct User_Type_Info_t {
    Light_Type_Kind kind;
	u32 flags;
	s64 type_size_bytes;
    User_Type_Desc description;
} User_Type_Info;

// Table
typedef struct {
	u8* start_extra_mem;
	u8* start_extra_strings;
	s64 extra_strings_bytes;
	s64 extra_mem_bytes;

	User_Type_Info* type_table;
	s64 type_table_length;
} User_Type_Table;

void backend_c_generate_top_level(Light_Ast** ast, Type_Table type_table);
void backend_c_compile_with_gcc(Light_Ast** ast, const char* filename);