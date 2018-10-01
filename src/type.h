#pragma once
#include "util.h"
#include "lexer.h"

enum Type_Kind {
	KIND_UNKNOWN = 0,

	KIND_PRIMITIVE,
	KIND_POINTER,
	KIND_STRUCT,
	KIND_UNION,
	KIND_ARRAY,
	KIND_FUNCTION,

	KIND_ALIAS,
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

struct Type_Union {
	Token*          name;
	Type_Instance** fields_types;
	string*         fields_names;
	s32             fields_count;
	s32             alignment;
};

struct Type_Struct {
	Token*          name;
	Type_Instance** fields_types;
	string*         fields_names;
	s64*            offset_bits;
	s32             fields_count;
	s32             alignment;
};

struct Type_Function {
	Type_Instance*  return_type;
	Type_Instance** arguments_type;
	string*         arguments_names;
	s32             num_arguments;
};

struct Type_Alias {
	Token* name;
};

// Expected state of node from the outside, another node can only infer its type from this
// instance if the type is internalized, otherwise wait and depend on it in the infer queue.
const u32 TYPE_FLAG_INTERNALIZED = FLAG(1);

// When the type size is resolved this flag is set, can only be set by the internalizer node
const u32 TYPE_FLAG_SIZE_RESOLVED = FLAG(2);

// Indicates if a type is lvalue, meaning it has an address linked to it
//const u32 TYPE_FLAG_LVALUE = FLAG(3);
const u32 TYPE_FLAG_WEAK   = FLAG(4);
const u32 TYPE_FLAG_STRONG = FLAG(5);
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
		Type_Union     union_desc;
		Type_Function  function_desc;
		Type_Alias     alias;
	};
};

inline bool type_primitive_int_signed(Type_Primitive p) {
	return (p >= TYPE_PRIMITIVE_S8 && p <= TYPE_PRIMITIVE_S64);
}

inline bool type_primitive_int_unsigned(Type_Primitive p) {
	return (p >= TYPE_PRIMITIVE_U8 && p <= TYPE_PRIMITIVE_U64);
}

inline bool type_primitive_int(Type_Primitive p) {
	return (p >= TYPE_PRIMITIVE_S8 && p <= TYPE_PRIMITIVE_U64);
}

inline bool type_primitive_float(Type_Primitive p) {
	return (p == TYPE_PRIMITIVE_R32 || p == TYPE_PRIMITIVE_R64);
}

inline bool type_primitive_int_signed(Type_Instance* p) {
	if (p->kind != KIND_PRIMITIVE)
		return false;
	return (p->primitive >= TYPE_PRIMITIVE_S8 && p->primitive <= TYPE_PRIMITIVE_S64);
}

inline bool type_primitive_int_unsigned(Type_Instance* p) {
	if (p->kind != KIND_PRIMITIVE)
		return false;
	return (p->primitive >= TYPE_PRIMITIVE_U8 && p->primitive <= TYPE_PRIMITIVE_U64);
}

inline bool type_primitive_int(Type_Instance* p) {
	if (p->kind != KIND_PRIMITIVE)
		return false;
	return (p->primitive >= TYPE_PRIMITIVE_S8 && p->primitive <= TYPE_PRIMITIVE_U64);
}

inline bool type_primitive_float(Type_Instance* p) {
	if (p->kind != KIND_PRIMITIVE)
		return false;
	return (p->primitive == TYPE_PRIMITIVE_R32 || p->primitive == TYPE_PRIMITIVE_R64);
}

inline bool type_primitive_bool(Type_Instance* p) {
	if (p->kind != KIND_PRIMITIVE)
		return false;
	return (p->primitive == TYPE_PRIMITIVE_BOOL);
}

inline bool type_primitive_numeric(Type_Instance* p) {
	return (type_primitive_int(p) || type_primitive_float(p));
}

inline bool type_primitive_numeric(Type_Primitive p) {
	return (type_primitive_int(p) || type_primitive_float(p));
}

inline bool type_regsize(Type_Instance* type) {
	return (type->kind == KIND_POINTER || type->kind == KIND_PRIMITIVE);
}

inline s64 type_size_primitive(Type_Primitive primitive) {
	switch (primitive) {
	case TYPE_PRIMITIVE_S64:	return 8;
	case TYPE_PRIMITIVE_S32:	return 4;
	case TYPE_PRIMITIVE_S16:	return 2;
	case TYPE_PRIMITIVE_S8:		return 1;
	case TYPE_PRIMITIVE_U64:	return 8;
	case TYPE_PRIMITIVE_U32:	return 4;
	case TYPE_PRIMITIVE_U16:	return 2;
	case TYPE_PRIMITIVE_U8:		return 1;
	case TYPE_PRIMITIVE_BOOL:	return 1;
	case TYPE_PRIMITIVE_R64:	return 8;
	case TYPE_PRIMITIVE_R32:	return 4;
	case TYPE_PRIMITIVE_VOID:	return 0;
	}
}