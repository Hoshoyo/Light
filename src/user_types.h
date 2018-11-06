#pragma once
#include "type.h"

struct User_Type_Info;

struct User_Type_Array {
	User_Type_Info* array_of;
    u64 dimension;
};

struct User_Type_Union {
	string name;
	User_Type_Info** fields_types;
	string* fields_names;
	s32 fields_count;
	s32 alignment;
};

struct User_Type_Struct {
	string name;
	User_Type_Info** fields_types;
	string* fields_names;
	s64* fields_offsets_bits;
	s32 fields_count;
	s32 alignment;
};

struct User_Type_Function {
	User_Type_Info* return_type;
	User_Type_Info** arguments_type;
	string* arguments_name;
	s32 arguments_count;
};

union User_Type_Desc {
    Type_Primitive primitive;
    User_Type_Info* pointer_to;
    User_Type_Array array_desc;
    User_Type_Struct struct_desc;
    User_Type_Union union_desc;
    User_Type_Function function_desc;
};

struct User_Type_Info {
    Type_Kind kind;
	u32 flags;
	s64 type_size_bytes;
    User_Type_Desc description;
};
