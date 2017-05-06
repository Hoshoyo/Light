#pragma once
#include "util.h"
#include "lexer.h"

enum Type_Primitive
{
	TYPE_PRIMITIVE_UNKNOWN,

	TYPE_NOT_PRIMITIVE,

	TYPE_PRIMITIVE_S64,
	TYPE_PRIMITIVE_S32,
	TYPE_PRIMITIVE_S16,
	TYPE_PRIMITIVE_S8,
	TYPE_PRIMITIVE_U64,
	TYPE_PRIMITIVE_U32,
	TYPE_PRIMITIVE_U16,
	TYPE_PRIMITIVE_U8,
	TYPE_PRIMITIVE_BOOL,

	TYPE_PRIMITIVE_R64,
	TYPE_PRIMITIVE_R32,

	TYPE_PRIMITIVE_VOID,

	NUM_PRIMITIVE_TYPES,
};

const u32 TYPE_FLAG_PRIMITIVE = FLAG(0);
const u32 TYPE_FLAG_POINTER = FLAG(1);

struct Array_Type;

struct Type {
	u32 flags;
	union {
		Type_Primitive primitive;
		Type* pointer_to;
		Array_Type* array_to;
		Type* struct_descriptor;
	};
};

struct Array_Type {
	u64* dimensions;
	Type* root_type;
};

void setup_types();
Type* get_type(Token_Type t);
s32 get_type_size_bytes(Type* t);