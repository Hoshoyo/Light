#include "type.h"
#include "lexer.h"

static Type prim_types[NUM_PRIMITIVE_TYPES] = {};

void setup_types() {
	for (int i = 0; i < NUM_PRIMITIVE_TYPES; ++i) {
		prim_types[i].flags = 0 | TYPE_FLAG_PRIMITIVE;
		prim_types[i].primitive = (Type_Primitive)i;
	}
}

s32 get_type_size_bytes(Type* t)
{
	if (t->flags & TYPE_FLAG_PRIMITIVE) {
		switch (t->primitive) {
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
	} else if (t->flags & TYPE_FLAG_POINTER) {
		return 8;
	}
}

Type* get_type(Token_Type t) {
	Type* type;
	switch (t) {
	case TOKEN_FLOAT:
	case TOKEN_REAL32:	type = &prim_types[TYPE_PRIMITIVE_R32];		break;
	case TOKEN_REAL64:	type = &prim_types[TYPE_PRIMITIVE_R64];		break;
	case TOKEN_UINT8:	type = &prim_types[TYPE_PRIMITIVE_U8];		break;
	case TOKEN_UINT16:	type = &prim_types[TYPE_PRIMITIVE_U16];		break;
	case TOKEN_UINT32:	type = &prim_types[TYPE_PRIMITIVE_U32];		break;
	case TOKEN_UINT64:	type = &prim_types[TYPE_PRIMITIVE_U64];		break;
	case TOKEN_CHAR:
	case TOKEN_SINT8:	type = &prim_types[TYPE_PRIMITIVE_S8];		break;
	case TOKEN_SINT16:	type = &prim_types[TYPE_PRIMITIVE_S16];		break;
	case TOKEN_INT:
	case TOKEN_SINT32:	type = &prim_types[TYPE_PRIMITIVE_S32];		break;
	case TOKEN_SINT64:	type = &prim_types[TYPE_PRIMITIVE_S64];		break;
	case TOKEN_VOID:	type = &prim_types[TYPE_PRIMITIVE_VOID];	break;
	case TOKEN_BOOL:	type = &prim_types[TYPE_PRIMITIVE_BOOL];	break;
	default: type = 0; break;
	}

	return type;
}