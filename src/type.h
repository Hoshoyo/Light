#pragma once
#include "util.h"
#include "lexer.h"

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
	bool dimension_evaluated;
	union {
		u64    dimension;
		Token* constant_dimension_name;
	};
};

struct Type_Struct {
	Token*          name;
	Type_Instance** fields_types;
	string*         fields_names;
};

struct Type_Function {
	Type_Instance*  return_type;
	Type_Instance** arguments_type;
	string*         arguments_names;
	s32             num_arguments;
};

// Auxiliary for a single node, while is not resolved can't internalize only the node itself 
// can change this flag. Inferring nodes should not read nor write this flag.
const u32 TYPE_FLAG_RESOLVED = FLAG(0);

// Expected state of node from the outside, another node can only infer its type from this
// instance if the type is internalized, otherwise wait and depend on it in the infer queue.
const u32 TYPE_FLAG_INTERNALIZED = FLAG(1);

// When the type size is resolved this flag is set, can only be set by the internalizer node
const u32 TYPE_FLAG_SIZE_RESOLVED = FLAG(2);

// Indicates if a type is lvalue, meaning it has an address linked to it
const u32 TYPE_FLAG_LVALUE = FLAG(3);
struct Type_Instance {
	Type_Kind kind;
	u32 flags;
	s32 type_queue_index;
	s64 type_size_bits;
	union {
		Type_Primitive primitive;
		Type_Instance* pointer_to;
		Type_Array     array_desc;
		Type_Struct    struct_desc;
		Type_Function  function_desc;
	};
};


bool type_primitive_int_signed(Type_Primitive p);
bool type_primitive_int_unsigned(Type_Primitive p);
bool type_primitive_int(Type_Primitive p);
bool type_primitive_float(Type_Primitive p);