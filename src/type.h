#pragma once
#include "util.h"

enum Type_Kind {
	KIND_UNKNOWN = 0,

	KIND_PRIMITIVE,
	KIND_POINTER,
	KIND_STRUCT,
	KIND_ARRAY,
	KIND_FUNCTION,
};

enum Type_Primitive {
	TYPE_UNKNOWN = 0,

	TYPE_PRIMITIVE_S8,
	TYPE_PRIMITIVE_S16,
	TYPE_PRIMITIVE_S32,
	TYPE_PRIMITIVE_S64,
	TYPE_PRIMITIVE_U8,
	TYPE_PRIMITIVE_U16,
	TYPE_PRIMITIVE_U32,
	TYPE_PRIMITIVE_U64,
	TYPE_PRIMITIVE_R32,
	TYPE_PRIMITIVE_R64,
	TYPE_PRIMITIVE_BOOL,
	TYPE_PRIMITIVE_VOID,
};

struct Type_Instance;

struct Type_Array {
	Type_Instance* array_of;
	u64            dimension;		// LITERAL or CONSTANT
};

struct Type_Struct {
	string          name;
	Type_Instance** fields_types;
	string*         fields_names;
};

struct Type_Function {
	Type_Instance*  return_type;
	Type_Instance** arguments_type;
	string*         arguments_names;
	s32             num_arguments;
};

const u32 TYPE_INSTANCE_RESOLVED     = FLAG(0);
const u32 TYPE_INSTANCE_INTERNALIZED = FLAG(1);
struct Type_Instance {
	Type_Kind type;
	u32 flags;
	s32 type_size;
	union {
		Type_Primitive primitive;
		Type_Instance* pointer_to;
		Type_Array     array_desc;
		Type_Struct    struct_desc;
	};
};