#include "type.h"
#include <assert.h>
#include "global_tables.h"

static u64 primitive_table_hash[TYPE_PRIMITIVE_COUNT] = {0};
static u64 struct_hash = 0;
static u64 union_hash = 0;
static u64 proc_hash = 0;
static u64 pointer_hash = 0;

static Light_Type* primitive_type_table[TYPE_PRIMITIVE_COUNT] = {0};

static Light_Type*
type_alloc() {
    return arena_alloc(global_type_arena, sizeof(Light_Type));
}

bool type_primitive_sint(Light_Type* t) {
    return (t->kind == TYPE_KIND_PRIMITIVE) &&
        (t->primitive >= TYPE_PRIMITIVE_S8 && t->primitive <= TYPE_PRIMITIVE_S64);
}
bool type_primitive_uint(Light_Type* t) {
    return (t->kind == TYPE_KIND_PRIMITIVE) &&
        (t->primitive >= TYPE_PRIMITIVE_U8 && t->primitive <= TYPE_PRIMITIVE_U64);
}
bool type_primitive_int(Light_Type* t) {
    return (t->kind == TYPE_KIND_PRIMITIVE) &&
        (t->primitive >= TYPE_PRIMITIVE_S8 && t->primitive <= TYPE_PRIMITIVE_U64);
}
bool type_primitive_float(Light_Type* t) {
    return (t->kind == TYPE_KIND_PRIMITIVE) &&
        (t->primitive == TYPE_PRIMITIVE_R32 || t->primitive == TYPE_PRIMITIVE_R64);
}
bool type_primitive_numeric(Light_Type* t) {
    return (type_primitive_int(t) || type_primitive_float(t));
}
bool type_primitive_bool(Light_Type* t) {
    return t == type_primitive_get(TYPE_PRIMITIVE_BOOL);
}

static Light_Type*
type_new_primitive(Light_Type_Primitive p) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_PRIMITIVE;
    result->flags = TYPE_FLAG_SIZE_RESOLVED;
    switch(p) {
        case TYPE_PRIMITIVE_VOID:   result->size_bits = 0; break;
        case TYPE_PRIMITIVE_S8:     result->size_bits = 8; break;
        case TYPE_PRIMITIVE_S16:    result->size_bits = 16; break;
        case TYPE_PRIMITIVE_S32:    result->size_bits = 32; break;
        case TYPE_PRIMITIVE_S64:    result->size_bits = 64; break;
        case TYPE_PRIMITIVE_U8:     result->size_bits = 8; break;
        case TYPE_PRIMITIVE_U16:    result->size_bits = 16; break;
        case TYPE_PRIMITIVE_U32:    result->size_bits = 32; break;
        case TYPE_PRIMITIVE_U64:    result->size_bits = 64; break;
        case TYPE_PRIMITIVE_R32:    result->size_bits = 32; break;
        case TYPE_PRIMITIVE_R64:    result->size_bits = 64; break;
        case TYPE_PRIMITIVE_BOOL:   result->size_bits = 32; break;
        default: assert(0); break;
    }
    result->primitive = p;

    return type_internalize(result);
}

static uint32_t
type_pointer_size_bits() {
    return 64;
}

Light_Type* 
type_new_pointer(Light_Type* pointer_to) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_POINTER;
    result->flags = TYPE_FLAG_SIZE_RESOLVED;
    result->size_bits = type_pointer_size_bits();
    result->pointer_to = pointer_to;

    if(pointer_to->flags & TYPE_FLAG_INTERNALIZED)
        result = type_internalize(result);

    return result;
}

Light_Type* 
type_new_alias(Light_Token* name, Light_Type* alias_to) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_ALIAS;
    result->flags = 0;
    result->size_bits = 0;
    result->alias.name = name;
    result->alias.alias_to = alias_to;

    s32 index = -1;
    if(type_table_entry_exist(&global_type_table, result, &index, 0)) {
        assert(index >= 0);
        return type_table_get(&global_type_table, index);
    }

    return result;
}

Light_Type* 
type_new_array(Light_Ast* dimension, Light_Type* type) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_ARRAY;
    result->size_bits = 0;
    if(type->flags & TYPE_FLAG_WEAK)
        result->flags |= TYPE_FLAG_WEAK;
    
    result->array_info.array_of = type;
    result->array_info.const_expr = dimension;

    return result;
}

Light_Type* 
type_new_function(Light_Type** arguments_types, Light_Type* return_type, s32 arguments_count, bool all_arguments_internalized) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_FUNCTION;
    result->size_bits = type_pointer_size_bits();
    result->flags = TYPE_FLAG_SIZE_RESOLVED;

    result->function.arguments_count = arguments_count;
    result->function.arguments_type = arguments_types;
    result->function.return_type = return_type;

    if(return_type->flags & TYPE_FLAG_INTERNALIZED && all_arguments_internalized) {
        result = type_internalize(result);
    }

    return result;
}

Light_Type* 
type_new_enum(Light_Token** fields_names, Light_Ast** fields_values, s32 fields_count, Light_Type* type_hint, Light_Scope* scope) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_ENUM;
    result->size_bits = 0;

    result->enumerator.field_count = fields_count;
    result->enumerator.fields_values = fields_values;
    result->enumerator.fields_names = fields_names;
    result->enumerator.type_hint = type_hint;
    result->enumerator.enum_scope = scope;

    if(type_hint) {
        if(type_hint->flags & TYPE_FLAG_WEAK) {
            result->flags |= TYPE_FLAG_WEAK;
        }
        if(type_hint->flags & TYPE_FLAG_SIZE_RESOLVED) {
            result->flags |= TYPE_FLAG_SIZE_RESOLVED;
        }
    }
    
    return result;
}

Light_Type*
type_new_struct(Light_Ast** fields, s32 fields_count, Light_Scope* struct_scope) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_STRUCT;
    result->size_bits = 0;
    result->flags = 0;

    result->struct_info.fields = fields;
    result->struct_info.fields_count = fields_count;
    result->struct_info.flags = 0;
    result->struct_info.struct_scope = struct_scope;

    return result;
}

Light_Type* 
type_new_union(Light_Ast** fields, s32 fields_count, Light_Scope* union_scope) {
    Light_Type* result = type_alloc();

    result->kind = TYPE_KIND_UNION;
    result->size_bits = 0;
    result->flags = 0;

    result->union_info.fields = fields;
    result->union_info.fields_count = fields_count;
    result->union_info.flags = 0;
    result->union_info.union_scope = union_scope;

    return result;
}

static u64 
type_primitive_hash(Light_Type_Primitive p) {
	return primitive_table_hash[p];
}

u64 
type_hash(Light_Type* type) {
	u64 hash = 0;
	if(!type) return 0;
	switch (type->kind) {
		case TYPE_KIND_PRIMITIVE:
			hash = type_primitive_hash(type->primitive); 
            break;
		case TYPE_KIND_POINTER:
			hash = fnv_1_hash_combine(type_hash(type->pointer_to), pointer_hash); 
            break;
		case TYPE_KIND_STRUCT:
            hash = struct_hash;
            if(type->struct_info.fields_count > 0) {
                for(s32 i = 0; i < type->struct_info.fields_count; ++i) {
                    Light_Ast* field_node = type->struct_info.fields[i];
                    u64 rhash = type_hash(field_node->decl_variable.type);
                    hash = fnv_1_hash_combine(hash, rhash);
                }
            }
            break;
		case TYPE_KIND_UNION:
            hash = union_hash;
            if(type->union_info.fields_count > 0) {
                for(s32 i = 0; i < type->union_info.fields_count; ++i) {
                    Light_Ast* field_node = type->union_info.fields[i];
                    hash = fnv_1_hash_combine(hash, type_hash(field_node->decl_variable.type));
                }
            }
			break;
		case TYPE_KIND_FUNCTION: {
			u64 return_type_hash = type_hash(type->function.return_type);
            if(type->function.arguments_count > 0) {
                for (size_t i = 0; i < type->function.arguments_count; ++i) {
                    return_type_hash = fnv_1_hash_combine(return_type_hash, type_hash(type->function.arguments_type[i]));
                }
            }
			hash = fnv_1_hash_combine(return_type_hash, proc_hash);
		} break;
		case TYPE_KIND_ARRAY:
			hash = fnv_1_hash_combine(type_hash(type->array_info.array_of), type->array_info.dimension); break;
            break;
        case TYPE_KIND_ALIAS:
            hash = fnv_1_hash(type->alias.name->data, type->alias.name->length);
            break;
		default:
			assert(0);
	}
	return hash;
}

void
type_tables_initialize() {
    // Initialize hashes
    pointer_hash = fnv_1_hash("pointer", sizeof("pointer") - 1);
    struct_hash = fnv_1_hash("struct", sizeof("struct") - 1);
    union_hash = fnv_1_hash("union", sizeof("union") - 1);
    proc_hash = fnv_1_hash("proc", sizeof("proc") - 1);

    primitive_table_hash[TYPE_PRIMITIVE_VOID] = fnv_1_hash("void", sizeof("void") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_S8]   = fnv_1_hash("s8", sizeof("s8") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_S16]  = fnv_1_hash("s16", sizeof("s16") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_S32]  = fnv_1_hash("s32", sizeof("s32") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_S64]  = fnv_1_hash("s64", sizeof("s64") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_U8]   = fnv_1_hash("u8", sizeof("u8") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_U16]  = fnv_1_hash("u16", sizeof("u16") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_U32]  = fnv_1_hash("u32", sizeof("u32") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_U64]  = fnv_1_hash("u64", sizeof("u64") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_R32]  = fnv_1_hash("r32", sizeof("r32") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_R64]  = fnv_1_hash("r64", sizeof("r64") - 1);
    primitive_table_hash[TYPE_PRIMITIVE_BOOL] = fnv_1_hash("bool", sizeof("bool") - 1);

    // Initialize primitive types
    primitive_type_table[TYPE_PRIMITIVE_R32]   = type_new_primitive(TYPE_PRIMITIVE_R32);
    primitive_type_table[TYPE_PRIMITIVE_R64]   = type_new_primitive(TYPE_PRIMITIVE_R64);
    primitive_type_table[TYPE_PRIMITIVE_S8]    = type_new_primitive(TYPE_PRIMITIVE_S8);
    primitive_type_table[TYPE_PRIMITIVE_S16]   = type_new_primitive(TYPE_PRIMITIVE_S16);
    primitive_type_table[TYPE_PRIMITIVE_S32]   = type_new_primitive(TYPE_PRIMITIVE_S32);
    primitive_type_table[TYPE_PRIMITIVE_S64]   = type_new_primitive(TYPE_PRIMITIVE_S64);
    primitive_type_table[TYPE_PRIMITIVE_U8]    = type_new_primitive(TYPE_PRIMITIVE_U8);
    primitive_type_table[TYPE_PRIMITIVE_U16]   = type_new_primitive(TYPE_PRIMITIVE_U16);
    primitive_type_table[TYPE_PRIMITIVE_U32]   = type_new_primitive(TYPE_PRIMITIVE_U32);
    primitive_type_table[TYPE_PRIMITIVE_U64]   = type_new_primitive(TYPE_PRIMITIVE_U64);
    primitive_type_table[TYPE_PRIMITIVE_VOID]  = type_new_primitive(TYPE_PRIMITIVE_VOID);
    primitive_type_table[TYPE_PRIMITIVE_BOOL]  = type_new_primitive(TYPE_PRIMITIVE_BOOL);
}

Light_Type*
type_internalize(Light_Type* type) {
    s32 index = 0;
    type_table_add(&global_type_table, type, &index);
    // Works even if the type already exist in the table.
    Light_Type* internalized = type_table_get(&global_type_table, index);
    internalized->flags |= TYPE_FLAG_INTERNALIZED;
    internalized->flags &= ~(TYPE_FLAG_WEAK);
    return internalized;
}

Light_Type*
type_primitive_get(Light_Type_Primitive t) {
    return primitive_type_table[t];
}

Light_Type* 
type_primitive_from_token(Light_Token_Type token) {
    switch(token) {
        case TOKEN_REAL32:  return primitive_type_table[TYPE_PRIMITIVE_R32];
        case TOKEN_REAL64:  return primitive_type_table[TYPE_PRIMITIVE_R64];
        case TOKEN_SINT8:   return primitive_type_table[TYPE_PRIMITIVE_S8];
        case TOKEN_SINT16:  return primitive_type_table[TYPE_PRIMITIVE_S16];
        case TOKEN_SINT32:  return primitive_type_table[TYPE_PRIMITIVE_S32];
        case TOKEN_SINT64:  return primitive_type_table[TYPE_PRIMITIVE_S64];
        case TOKEN_UINT8:   return primitive_type_table[TYPE_PRIMITIVE_U8];
        case TOKEN_UINT16:  return primitive_type_table[TYPE_PRIMITIVE_U16];
        case TOKEN_UINT32:  return primitive_type_table[TYPE_PRIMITIVE_U32];
        case TOKEN_UINT64:  return primitive_type_table[TYPE_PRIMITIVE_U64];
        case TOKEN_VOID:    return primitive_type_table[TYPE_PRIMITIVE_VOID];
        case TOKEN_BOOL:    return primitive_type_table[TYPE_PRIMITIVE_BOOL];
        default: assert(0); break;
    }
    return 0;
}

Light_Type* 
type_weak_primitive_from_literal(Light_Literal_Type literal) {
    Light_Type* type = type_alloc();
    type->kind = TYPE_KIND_PRIMITIVE;
    type->flags = TYPE_FLAG_WEAK;
    
    switch(literal) {
        case LITERAL_CHAR:          type->primitive = TYPE_PRIMITIVE_U32; break;
        case LITERAL_FLOAT:         type->primitive = TYPE_PRIMITIVE_R32; break;
        case LITERAL_DEC_SINT:      type->primitive = TYPE_PRIMITIVE_S32; break;
        case LITERAL_BIN_INT:       type->primitive = TYPE_PRIMITIVE_U32; break;
        case LITERAL_HEX_INT:       type->primitive = TYPE_PRIMITIVE_U32; break;
        case LITERAL_DEC_UINT:      type->primitive = TYPE_PRIMITIVE_S32; break;
        default: assert(0); break;
    }
    return type;
}

// Could be a WEAK type
Light_Type* 
type_from_token(Light_Token* t) {
    // TODO(psv): implement
    return 0;
}

Light_Type* 
type_alias_by_name(Light_Token* name) {
    Light_Type t = {0};
    t.kind = TYPE_KIND_ALIAS;
    t.alias.name = name;

    s32 index = -1;
    if(type_table_entry_exist(&global_type_table, &t, &index, 0)) {
        assert(index >= 0);
        return type_table_get(&global_type_table, index);
    }
    return 0;
}

#include <stdio.h>
void
type_table_print() {
    FILE* out = stdout;
    for(int i = 0; i < global_type_table.entries_capacity; ++i) {
        Type_Table_Entry* entry = &global_type_table.entries[i];
        if(entry->flags & HASH_TABLE_OCCUPIED){
            ast_print_type(entry->data, LIGHT_AST_PRINT_STDOUT);
            fprintf(out, "\n");
        }
    }
}