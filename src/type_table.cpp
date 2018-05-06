#include "type_table.h"
#include "util.h"
#include "memory.h"
#include "decl_check.h"

Type_Instance** g_type_table = 0;
static Hash_Table type_table;

struct Internalize_Queue {
	Type_Instance* type;
	Scope* scope;
};
static Internalize_Queue* type_internalize_queue;

// patches the hanging non-internalized pointers of recursive defined structs. 
void resolve_type_internalize_queue() {
	size_t length = array_get_length(type_internalize_queue);
	for(size_t i = 0; i < length; ++i) {
		Type_Instance* type = type_internalize_queue[i].type;
		Scope* scope = type_internalize_queue[i].scope;
		assert(type->kind == KIND_POINTER);
		u64 hash = type_hash(type->pointer_to);
		s64 index = hash_table_entry_exist(&type_table, type, hash);
		assert(index != -1);
		type->pointer_to = type_table.entries[index].data;
	}
}

#define ALLOC_TYPE(ARENA) (Type_Instance*)(ARENA).allocate(sizeof(Type_Instance))

static Memory_Arena types_internal(65536);
static Memory_Arena types_temporary(65536);

static Type_Instance* type_s8;
static Type_Instance* type_s16;
static Type_Instance* type_s32;
static Type_Instance* type_s64;
static Type_Instance* type_u8;
static Type_Instance* type_u16;
static Type_Instance* type_u32;
static Type_Instance* type_u64;
static Type_Instance* type_r32;
static Type_Instance* type_r64;
static Type_Instance* type_bool;
static Type_Instance* type_void;

inline Type_Instance* type_setup_primitive(Type_Primitive p);

#if defined(_WIN32) || defined(_WIN64)
constexpr 
#endif
u64 type_primitive_hash(Type_Primitive p) {
	u64 hash = 0;
	switch (p) {
		case TYPE_PRIMITIVE_S64:
			hash = fnv_1_hash((const u8*)"s64", sizeof("s64") - 1); break;
		case TYPE_PRIMITIVE_S32:
			hash = fnv_1_hash((const u8*)"s32", sizeof("s32") - 1); break;
		case TYPE_PRIMITIVE_S16:
			hash = fnv_1_hash((const u8*)"s16", sizeof("s16") - 1); break;
		case TYPE_PRIMITIVE_S8:
			hash = fnv_1_hash((const u8*)"s8", sizeof("s8") - 1); break;
		case TYPE_PRIMITIVE_U64:
			hash = fnv_1_hash((const u8*)"u64", sizeof("u64") - 1); break;
		case TYPE_PRIMITIVE_U32:
			hash = fnv_1_hash((const u8*)"u32", sizeof("u32") - 1); break;
		case TYPE_PRIMITIVE_U16:
			hash = fnv_1_hash((const u8*)"u16", sizeof("u16") - 1); break;
		case TYPE_PRIMITIVE_U8:
			hash = fnv_1_hash((const u8*)"u8", sizeof("u8") - 1); break;
		case TYPE_PRIMITIVE_R32:
			hash = fnv_1_hash((const u8*)"r32", sizeof("r32") - 1); break;
		case TYPE_PRIMITIVE_R64:
			hash = fnv_1_hash((const u8*)"r64", sizeof("r64") - 1); break;
		case TYPE_PRIMITIVE_BOOL:
			hash = fnv_1_hash((const u8*)"bool", sizeof("bool") - 1); break;
		case TYPE_PRIMITIVE_VOID:
			hash = fnv_1_hash((const u8*)"void", sizeof("void") - 1); break;
	}
	return hash;
}

u64 type_hash(Type_Instance* type) {
	u64 hash = 0;
	if(!type) return 0;
	switch (type->kind) {
		case KIND_PRIMITIVE:
			hash = type_primitive_hash(type->primitive); break;
		case KIND_POINTER:
			hash = fnv_1_hash_from_start(type_hash(type->pointer_to), (const u8*)"pointer", sizeof("pointer") - 1); break;
		case KIND_STRUCT:
			hash = fnv_1_hash(type->struct_desc.name->value.data, type->struct_desc.name->value.length); break;
			//size_t n_fields = 0;
			//if(type->struct_desc.fields_types) n_fields = array_get_length(type->struct_desc.fields_types);
			//for(size_t i = 0; i < n_args; ++i) {
			//	
			//}
		case KIND_FUNCTION: {
			u64 return_type_hash = type_hash(type->function_desc.return_type);
			size_t n_args = 0;
			if(type->function_desc.arguments_type) n_args = array_get_length(type->function_desc.arguments_type);
			for (size_t i = 0; i < n_args; ++i) {
				return_type_hash = fnv_1_hash_combine(return_type_hash, type_hash(type->function_desc.arguments_type[i]));
			}
			hash = fnv_1_hash_from_start(return_type_hash, (const u8*)"proc", sizeof("proc") - 1);
		} break;
		case KIND_ARRAY:
			hash = fnv_1_hash_combine(type_hash(type->array_desc.array_of), type->array_desc.dimension); break;
		default:
			assert(0);
	}
	return hash;
}

void type_table_init() {
	// TODO(psv): no compare function between types yet
	hash_table_init(&type_table, 1024 * 1024 * 4, (hash_function_type*)type_hash, (hash_entries_equal_type*)0);
	g_type_table = array_create(Type_Instance*, 1024);
	type_internalize_queue = array_create(Internalize_Queue, 1024);

	type_s64 = type_setup_primitive(TYPE_PRIMITIVE_S64);
	type_s32 = type_setup_primitive(TYPE_PRIMITIVE_S32);
	type_s16 = type_setup_primitive(TYPE_PRIMITIVE_S16);
	type_s8 = type_setup_primitive(TYPE_PRIMITIVE_S8);
	type_u64 = type_setup_primitive(TYPE_PRIMITIVE_U64);
	type_u32 = type_setup_primitive(TYPE_PRIMITIVE_U32);
	type_u16 = type_setup_primitive(TYPE_PRIMITIVE_U16);
	type_u8 = type_setup_primitive(TYPE_PRIMITIVE_U8);
	type_r32 = type_setup_primitive(TYPE_PRIMITIVE_R32);
	type_r64 = type_setup_primitive(TYPE_PRIMITIVE_R64);
	type_bool = type_setup_primitive(TYPE_PRIMITIVE_BOOL);
	type_void = type_setup_primitive(TYPE_PRIMITIVE_VOID);
}

// Deep copy of types, this function follow the pointers of all Type_Instance* in the type description
Type_Instance* type_copy_internal(Type_Instance* type, Scope* scope) {
	Type_Instance* result = ALLOC_TYPE(types_internal);
	memcpy(result, type, sizeof(Type_Instance));

	switch (type->kind) {
	case KIND_PRIMITIVE: break;
	case KIND_POINTER:
		if(type->pointer_to->kind == KIND_STRUCT) {
			result->kind = KIND_POINTER;
			result->pointer_to = type->pointer_to;
			Internalize_Queue q = {result, scope};
			array_push(type_internalize_queue, &q);
		} else {
			internalize_type(&type->pointer_to, scope, true);
		}
		//result->pointer_to = type_copy_internal(type->pointer_to);
		break;
	case KIND_STRUCT: {
		size_t num_args = array_get_length(type->struct_desc.fields_types);
		result->struct_desc.fields_types = (Type_Instance**)types_internal.allocate(array_get_header_size() + num_args * sizeof(Type_Instance*));
		result->struct_desc.fields_types = (Type_Instance**)((u8*)result->struct_desc.fields_types + array_get_header_size());
		array_set_capacity(result->struct_desc.fields_types, num_args);
		array_set_length(result->struct_desc.fields_types, num_args);
		array_set_element_size(result->struct_desc.fields_types, sizeof(Type_Instance*));
		for (size_t i = 0; i < num_args; ++i) {
			result->struct_desc.fields_types[i] = internalize_type(&type->struct_desc.fields_types[i], scope, true);//type_copy_internal(type->struct_desc.fields_types[i]);
		}
	}break;
	case KIND_FUNCTION: {
		result->function_desc.return_type = internalize_type(&type->function_desc.return_type, scope, true);//type_copy_internal(type->function_desc.return_type);
		size_t num_args = type->function_desc.num_arguments;
		result->function_desc.arguments_type = (Type_Instance**)types_internal.allocate(array_get_header_size() + num_args * sizeof(Type_Instance*));
		result->function_desc.arguments_type = (Type_Instance**)((u8*)result->function_desc.arguments_type + array_get_header_size());
		array_set_capacity(result->function_desc.arguments_type, num_args);
		array_set_length(result->function_desc.arguments_type, num_args);
		array_set_element_size(result->function_desc.arguments_type, sizeof(Type_Instance*));
		for (size_t i = 0; i < num_args; ++i) {
			result->function_desc.arguments_type[i] = internalize_type(&type->function_desc.arguments_type[i], scope, true);//type_copy_internal(type->function_desc.arguments_type[i]);
		}
	}break;
	case KIND_ARRAY:
		internalize_type(&type->array_desc.array_of, scope, true);
		//result->array_desc.array_of = type_copy_internal(type->array_desc.array_of);
		break;
	default:
		assert(0);
	}
	return result;
}

Type_Instance* internalize_type(Type_Instance** type, Scope* scope, bool copy) {
	assert((*type)->flags & TYPE_FLAG_RESOLVED);
	u64 hash = type_hash(*type);

	s64 index = hash_table_entry_exist(&type_table, *type, hash);

	if (index == -1) {
		if (copy) {
			// copy into internal mem space
			Type_Instance* copied = type_copy_internal(*type, scope);
			index = hash_table_add(&type_table, copied, sizeof(copied), hash);
		} else {
			index = hash_table_add(&type_table, *type, sizeof(*type), hash);
		}
		array_push(g_type_table, type);
	}
	*type = (Type_Instance*)type_table.entries[index].data;
	(*type)->flags |= TYPE_FLAG_INTERNALIZED | TYPE_FLAG_STRONG;

	return *type;
}

inline Type_Instance* type_setup_primitive(Type_Primitive p) {
	Type_Instance* res = ALLOC_TYPE(types_internal);
	res->kind = KIND_PRIMITIVE;
	res->flags = TYPE_FLAG_RESOLVED | TYPE_FLAG_INTERNALIZED | TYPE_FLAG_SIZE_RESOLVED;
	res->primitive = p;
	switch (p) {
		case TYPE_PRIMITIVE_S64:  res->type_size_bits = 64; break;
		case TYPE_PRIMITIVE_S32:  res->type_size_bits = 32; break;
		case TYPE_PRIMITIVE_S16:  res->type_size_bits = 16; break;
		case TYPE_PRIMITIVE_S8:   res->type_size_bits = 8; break;
		case TYPE_PRIMITIVE_U64:  res->type_size_bits = 64; break;
		case TYPE_PRIMITIVE_U32:  res->type_size_bits = 32; break;
		case TYPE_PRIMITIVE_U16:  res->type_size_bits = 16; break;
		case TYPE_PRIMITIVE_U8:   res->type_size_bits = 8; break;
		case TYPE_PRIMITIVE_R32:  res->type_size_bits = 32; break;
		case TYPE_PRIMITIVE_R64:  res->type_size_bits = 64; break;
		case TYPE_PRIMITIVE_BOOL: res->type_size_bits = 8; break;
		case TYPE_PRIMITIVE_VOID: res->type_size_bits = 0; break;
	}
	internalize_type(&res, false);
	return res;
}

Type_Instance* type_primitive_get(Type_Primitive p) {
	switch (p) {
	case TYPE_PRIMITIVE_S64:  return type_s64;
	case TYPE_PRIMITIVE_S32:  return type_s32;
	case TYPE_PRIMITIVE_S16:  return type_s16;
	case TYPE_PRIMITIVE_S8:   return type_s8;
	case TYPE_PRIMITIVE_U64:  return type_u64;
	case TYPE_PRIMITIVE_U32:  return type_u32;
	case TYPE_PRIMITIVE_U16:  return type_u16;
	case TYPE_PRIMITIVE_U8:   return type_u8;
	case TYPE_PRIMITIVE_R32:  return type_r32;
	case TYPE_PRIMITIVE_R64:  return type_r64;
	case TYPE_PRIMITIVE_BOOL: return type_bool;
	case TYPE_PRIMITIVE_VOID: return type_void;
	default: report_internal_compiler_error(__FILE__, __LINE__, "tried to get unknown primitive type from type table\n"); break;
	}
}

Type_Instance* type_new_temporary() {
	Type_Instance* t = ALLOC_TYPE(types_temporary);
	t->type_queue_index = -1;
	t->flags = 0;
	t->kind = KIND_UNKNOWN;
	t->type_size_bits = 0;
	return t;
}

s64 type_pointer_size() {
	return 8;
}
s64 type_pointer_size_bits() {
	return 64;
}

#include "ast.h"
#include <math.h>
void DEBUG_print_type_table() {
	size_t len = array_get_length(g_type_table);
	for (size_t i = 0; i < len; ++i) {
		Type_Instance* t = g_type_table[i];
		s32 c = DEBUG_print_type(stdout, t, true);
		s32 tn = floorf((r32)((40 - c) / 8));
		{
			char buffer[64] = {0};
			for(int n = 0; n < tn; ++n)
				buffer[n] = '\t';
			fprintf(stdout, "%s", buffer);
		}

		fprintf(stdout, "size: %lld bits\n", t->type_size_bits);
	}
}