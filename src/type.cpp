#include "type.h"
#include "lexer.h"
#include "util.h"

s32 get_size_of_primitive_type(Type_Primitive primitive);

Type_Instance* get_primitive_type(Type_Primitive primitive_type)
{
	Type_Instance ti;
	ti.flags = 0 | TYPE_FLAG_IS_REGISTER_SIZE | TYPE_FLAG_IS_RESOLVED | TYPE_FLAG_IS_SIZE_RESOLVED;
	ti.type = TYPE_PRIMITIVE;
	ti.primitive = primitive_type;
	ti.type_size = get_size_of_primitive_type(primitive_type);
	s64 hash = -1;
	if (type_table.entry_exist(&ti, &hash)) {
		return type_table.entries[hash].entry;
	} else {
		Type_Instance* ti = new Type_Instance();
		ti->flags = 0 | TYPE_FLAG_IS_REGISTER_SIZE | TYPE_FLAG_IS_RESOLVED | TYPE_FLAG_IS_SIZE_RESOLVED;
		ti->type = TYPE_PRIMITIVE;
		ti->primitive = primitive_type;
		ti->type_size = get_size_of_primitive_type(primitive_type);
		create_type(&ti, true);
		return ti;
	}
}

Type_Instance* create_ptr_typeof(Type_Instance* inst)
{
	Type_Instance* ti = new Type_Instance();
	ti->flags = 0 | TYPE_FLAG_IS_REGISTER_SIZE | TYPE_FLAG_IS_RESOLVED | TYPE_FLAG_IS_SIZE_RESOLVED;
	ti->type = TYPE_POINTER;
	ti->pointer_to = inst;
	ti->type_size = get_size_of_pointer();
	create_type(&ti, true);
	return ti;
}

s32 get_size_of_pointer() {
	return 8;
}

s32 get_size_of_primitive_type(Type_Primitive primitive) {
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
Type_Table type_table;
static s64 DEBUG_type_entries[1024] = { 0 };
static s64 DEBUG_type_index = 0;

u32 Type_Table::type_hash(Type_Instance* instance)
{
	switch (instance->type) {
	case TYPE_PRIMITIVE: {
		u32 hash = djb2_hash((u8*)&instance->primitive, sizeof(Type_Primitive));
		return hash % max_entries;
	}break;
		// --
	case TYPE_POINTER: {
		u32 hash = type_hash(instance->pointer_to);
		hash = djb2_hash(hash, (u8*)&instance->type, sizeof(u32));
		return hash % max_entries;
	}break;
		// --
	case TYPE_ARRAY: {

	}break;
		// --
	case TYPE_STRUCT: {
		u32 hash = djb2_hash((u8*)instance->type_struct.name, instance->type_struct.name_length);
		return hash % max_entries;
	}break;
		// --
	case TYPE_FUNCTION: {
		u32 hash = type_hash(instance->type_function.return_type);
		hash = djb2_hash(hash, (u8*)&instance->type, sizeof(u32));
		int num_args = instance->type_function.num_arguments;
		for (int i = 0; i < num_args; ++i) {
			Type_Instance* argtype = instance->type_function.arguments_type[i];
			hash = djb2_hash(hash, type_hash(argtype));
		}
		return hash % max_entries;
	}break;
	}
}

bool types_equal(Type_Instance* i1, Type_Instance* i2)
{
	switch (i1->type) {
		case TYPE_PRIMITIVE: {
			if (i2->type != TYPE_PRIMITIVE) return false;
			if (i1->primitive != i2->primitive) return false;
			return true;
		}break;
			// --
		case TYPE_POINTER: {
			if (i2->type != TYPE_POINTER) return false;
			if (types_equal(i1->pointer_to, i2->pointer_to)) return true;
			return false;
		}break;
			// --
		case TYPE_ARRAY: {
			assert(0);	// @todo
		}break;
			// --
		case TYPE_STRUCT: {
			if (i2->type != TYPE_STRUCT) return false;
			return str_equal(i1->type_struct.name, i1->type_struct.name_length, i2->type_struct.name, i2->type_struct.name_length);
		}break;
			// --
		case TYPE_FUNCTION: {
			if (i2->type != TYPE_FUNCTION) return false;
			if (!types_equal(i1->type_function.return_type, i2->type_function.return_type)) return false;
			int num_args = i1->type_function.num_arguments;
			if (num_args != i2->type_function.num_arguments) return false;
			for (int i = 0; i < num_args; ++i) {
				if (!types_equal(i1->type_function.arguments_type[i], i2->type_function.arguments_type[i])) return false;
			}
			return true;
		}break;
	}
}

bool Type_Table::entry_exist(Type_Instance* instance, s64* hash)
{
	u32 h = type_table.type_hash(instance);

	if (type_table.entries[h].used) {
		// might be the same
		if (types_equal(instance, type_table.entries[h].entry)) {
			if (hash) *hash = h;
			return true;
		} else if(type_table.entries[h].collided) {
			// continue searching
			do {
				h += 1;
				if (h == type_table.max_entries) h = 0;
				if (types_equal(instance, type_table.entries[h].entry)) {
					if (hash) *hash = h;
					return true;
				}
			} while (type_table.entries[h].collided);
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool Type_Table::insert_type(Type_Instance* instance, s64* hash)
{
	u32 h = type_table.type_hash(instance);

	while (type_table.entries[h].used) {
		type_table.entries[h].collided = true;
		h += 1;
		if (h == max_entries) h = 0;
	}
	instance->flags |= TYPE_FLAG_NOT_DELETE;
	type_table.entries[h].entry = instance;
	type_table.entries[h].used = true;

	DEBUG_type_entries[DEBUG_type_index++] = h;

	if (hash) *hash = h;

	return true;
}

Type_Instance* Type_Table::get_entry(s64 hash)
{
	return entries[hash].entry;
}

void delete_type(Type_Instance* instance) 
{
	if (instance->flags & TYPE_FLAG_NOT_DELETE) {
		// @TODO @IMPORTANT dangling pointer here, the type that is supposed to be deleted is
		// lost because it will be subbed and needs to be alive until all the infer
		// queue is infered.
		// Hoshoyo 07/09/2017
		return;
	}

	switch (instance->type) {
		case TYPE_PRIMITIVE: {
			delete instance;
		}break;
			// --
		case TYPE_POINTER: {
			delete_type(instance->pointer_to);
			delete instance;
		}break;
			// --
		case TYPE_ARRAY: {
			assert(0);	// @todo
		}break;
			// --
		case TYPE_STRUCT: {
			delete instance;
		}break;

		case TYPE_FUNCTION: {
			int num_args = instance->type_function.num_arguments;
			for (int i = 0; i < num_args; ++i) {
				delete_type(instance->type_function.arguments_type[i]);
			}
			delete_type(instance->type_function.return_type);
			delete instance;
		}break;
	}
}

s64 create_type(Type_Instance** instance, bool swap_and_delete)
{
	s64 index = 0;
	if (!type_table.entry_exist(*instance, &index)) {
		// if there is no entry for this type, then fill it
		type_table.insert_type(*instance, &index);
	} else {
		// if there is then swap it
		if (swap_and_delete && type_table.entries[index].entry != *instance) {
			delete_type(*instance);
			*instance = type_table.entries[index].entry;
		}
	}
	return index;
}

int is_integer_type(Type_Instance* inst)
{
	if (!inst) return 0;
	if (inst->type == TYPE_PRIMITIVE) {
		if (inst->primitive >= TYPE_PRIMITIVE_S64 && inst->primitive <= TYPE_PRIMITIVE_S8)
			return 1;
		if (inst->primitive >= TYPE_PRIMITIVE_U64 && inst->primitive <= TYPE_PRIMITIVE_U8)
			return 2;
	}
	return 0;
}

int is_floating_point_type(Type_Instance* inst) {
	if (!inst) return 0;
	if (inst->type == TYPE_PRIMITIVE) {
		if (inst->primitive == TYPE_PRIMITIVE_R32 || inst->primitive == TYPE_PRIMITIVE_R64)
			return 1;
	}
	return 0;
}

#include "ast.h"
void DEBUG_print_type_table()
{
	printf("The type table has %d entries.\n", DEBUG_type_index);
	for (int i = 0; i < DEBUG_type_index; ++i) {
		s64 hash = DEBUG_type_entries[i];
		printf("entry[%d/%u]: %p\n", i, hash, type_table.get_entry(hash));
		DEBUG_print_type(stdout, type_table.get_entry(hash));
		printf("\n");
	}
}

int DEBUG_indent_level = 0;

void DEBUG_print_node_type(FILE* out, Ast* node, bool decl_only) 
{
	DEBUG_print_indent_level();
	switch (node->node) {
	case AST_NODE_BINARY_EXPRESSION: {
		if (decl_only) return;
		fprintf(out, "AST_NODE_BINARY_EXPRESSION: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
		DEBUG_indent_level += 1;
		DEBUG_print_node_type(out, node->expression.binary_exp.left, decl_only);
		DEBUG_print_node_type(out, node->expression.binary_exp.right, decl_only);
		DEBUG_indent_level -= 1;
	}break;
	case AST_NODE_BLOCK: {
		if (!decl_only) {
			fprintf(out, "AST_NODE_BLOCK: return_type = ");
			DEBUG_print_type(out, node->return_type);
			fprintf(out, "\n");
		}
		DEBUG_indent_level += 1;
		DEBUG_print_node_type(out, node->block.commands, decl_only);
		DEBUG_indent_level -= 1;
	}break;
	case AST_NODE_BREAK_STATEMENT: {
		if (decl_only) return;
		fprintf(out, "AST_NODE_BREAK_STATEMENT: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
	}break;
	case AST_NODE_CONTINUE_STATEMENT: {
		if (decl_only) return;
		fprintf(out, "AST_NODE_CONTINUE_STATEMENT: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
	}break;
	case AST_NODE_EXPRESSION_ASSIGNMENT: {
		assert(0);	// deprecated
	}break;
	case AST_NODE_IF_STATEMENT: {
		if (!decl_only) {
			fprintf(out, "AST_NODE_IF_STATEMENT: return_type = ");
			DEBUG_print_type(out, node->return_type);
			fprintf(out, "\n");
		}
		DEBUG_indent_level += 1;
		DEBUG_print_node_type(out, node->if_stmt.bool_exp, decl_only);
		DEBUG_print_node_type(out, node->if_stmt.body, decl_only);
		if (node->if_stmt.else_exp) {
			DEBUG_print_node_type(out, node->if_stmt.else_exp, decl_only);
		}
		DEBUG_indent_level -= 1;
	}break;
	case AST_NODE_LITERAL_EXPRESSION: {
		if (decl_only) return;
		fprintf(out, "AST_NODE_LITERAL_EXPRESSION: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
	}break;
	case AST_NODE_NAMED_ARGUMENT: {
		fprintf(out, "AST_NODE_NAMED_ARGUMENT: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
	}break;
	case AST_NODE_PROCEDURE_CALL: {
		if (!decl_only) {
			fprintf(out, "AST_NODE_PROCEDURE_CALL: return_type = ");
			DEBUG_print_type(out, node->return_type);
			fprintf(out, "\n");
		}
		DEBUG_indent_level += 1;
		DEBUG_print_node_type(out, node->expression.proc_call.args, decl_only);
		DEBUG_indent_level -= 1;
	}break;
	case AST_NODE_PROC_DECLARATION: {
		fprintf(out, "AST_NODE_PROCEDURE_DECLARATION: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
		DEBUG_indent_level += 1;
		DEBUG_print_node_type(out, node->proc_decl.arguments, decl_only);
		DEBUG_print_node_type(out, node->proc_decl.body, decl_only);
		DEBUG_indent_level -= 1;
	}break;
	case AST_NODE_RETURN_STATEMENT: {
		if (decl_only) return;
		fprintf(out, "AST_NODE_RETURN_STATEMENT: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
		DEBUG_indent_level += 1;
		DEBUG_print_node_type(out, node->ret_stmt.expr, decl_only);
		DEBUG_indent_level -= 1;
	}break;
	case AST_NODE_STRUCT_DECLARATION: {
		fprintf(out, "AST_NODE_STRUCT_DECLARATION: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
		DEBUG_indent_level += 1;
		DEBUG_print_node_type(out, node->struct_decl.fields, decl_only);
		DEBUG_indent_level -= 1;
	}break;
	case AST_NODE_UNARY_EXPRESSION: {
		if (decl_only) return;
		fprintf(out, "AST_NODE_UNARY_EXPRESSION: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
		DEBUG_indent_level += 1;
		DEBUG_print_node_type(out, node->expression.unary_exp.operand, decl_only);
		DEBUG_indent_level -= 1;
	}break;
	case AST_NODE_VARIABLE_DECL: {
		fprintf(out, "AST_NODE_VARIABLE_DECL: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
		if (decl_only) return;
		DEBUG_indent_level += 1;
		DEBUG_print_node_type(out, node->var_decl.assignment, decl_only);
		DEBUG_indent_level -= 1;
	}break;
	case AST_NODE_VARIABLE_EXPRESSION: {
		if (decl_only) return;
		fprintf(out, "AST_NODE_VARIABLE_EXPRESSION: return_type = ");
		DEBUG_print_type(out, node->return_type);
		fprintf(out, "\n");
	}break;
	}
}

void DEBUG_print_node_type(FILE* out, Ast** ast, bool decl_only)
{
	int num_nodes = get_arr_length(ast);
	for (int i = 0; i < num_nodes; ++i) {
		Ast* node = ast[i];
		DEBUG_print_node_type(out, node, decl_only);
	}
}