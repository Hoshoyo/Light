#include "semantic.h"
//#include "type.h"
#include <stdarg.h>

static int scope_error = 0;
static int declaration_ratio = 8;

#define TOKEN_STR(X) X->value.length, X->value.data

Infer_Node* infer_queue = 0;

static int report_semantic_error(Decl_Site* site, char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	if (site) {
		fprintf(stderr, "%.*s:%d:%d: Semantic Error: ", site->filename.length, site->filename.data, site->line, site->column);
	}
	vfprintf(stderr, msg, args);
	va_end(args);
	return 0;
}

static int report_semantic_warning(Decl_Site* site, char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	if (site) {
		fprintf(stderr, "%.*s:%d:%d: Warning: ", site->filename.length, site->filename.data, site->line, site->column);
	}
	vfprintf(stderr, msg, args);
	va_end(args);
	return 0;
}

static void report_declaration_site(Ast* node) {
	assert(node->is_decl);
	Decl_Site* site = 0;
	Token* name = 0;
	switch (node->node) {
		case AST_NODE_VARIABLE_DECL: {
			site = &node->var_decl.site;
			name = node->var_decl.name;
		}break;
		case AST_NODE_PROC_DECLARATION: {
			site = &node->proc_decl.site;
			name = node->proc_decl.name;
		}break;
		case AST_NODE_STRUCT_DECLARATION: {
			site = &node->struct_decl.site;
			name = node->struct_decl.name;
		}break;
	}
	int num = fprintf(stderr, "%.*s:%d:%d: ", site->filename.length, site->filename.data, site->line, site->column);
	fprintf(stderr, "%.*s\n", name->value.length, name->value.data);
	while (num > 0) {
		fprintf(stderr, "-");
		num -= 1;
	}
	fprintf(stderr, "^   Previously defined here.\n");
}

int check_declarations(Ast* node, Scope* scope) {
	if (node->is_decl) {
		switch (node->node) {
		case AST_NODE_VARIABLE_DECL: {
			assert(scope->symb_table);
			//
			// check if variable exist on this scope
			//
			s64 hash = scope->symb_table->entry_exist(node->var_decl.name);
			if (hash == -1) {
				hash = scope->symb_table->insert(scope, node->var_decl.name, node);
			} else {
				report_semantic_error(&node->var_decl.site, "Variable %.*s redefinition.\n", node->var_decl.name->value.length, node->var_decl.name->value.data);
				report_declaration_site(scope->symb_table->entries[hash].node);
				return 0;
			}
		}break;
		case AST_NODE_PROC_DECLARATION: {
			assert(scope->symb_table);
			//
			// check if the proc decl is not redefined in the same scope
			//
			s64 hash = scope->symb_table->entry_exist(node->proc_decl.name);
			if (hash == -1) {
				hash = scope->symb_table->insert(scope, node->proc_decl.name, node);
			} else {
				report_semantic_error(&node->proc_decl.site, "Variable %.*s redefinition.\n", node->proc_decl.name->value.length, node->proc_decl.name->value.data);
				report_declaration_site(scope->symb_table->entries[hash].node);
				return 0;
			}
			//
			// check for duplicate arguments
			//
			int num_args = node->proc_decl.num_args;
			int error = DECL_CHECK_PASSED;
			if (num_args > 0) {
				node->proc_decl.scope->symb_table = new Symbol_Table(num_args * declaration_ratio);
				printf("Proc (%d - level[%d]) has %d arguments.\n", node->proc_decl.scope->id, node->proc_decl.scope->level, node->proc_decl.scope->num_declarations);
				for (int i = 0; i < num_args; ++i) {
					int ret = check_declarations(node->proc_decl.arguments[i], node->proc_decl.scope);
					if (ret != DECL_CHECK_PASSED) error = DECL_CHECK_FAILED;
				}
			}
			int ret = check_declarations(node->proc_decl.body, node->proc_decl.scope);
			if (ret != DECL_CHECK_PASSED) error = DECL_CHECK_FAILED;
			if (error == DECL_CHECK_FAILED) return DECL_CHECK_FAILED;
		}break;
		case AST_NODE_STRUCT_DECLARATION: {
			Scope* struct_scope = node->struct_decl.scope;
			printf("Struct (%d - level[%d]) has %d declarations.\n", struct_scope->id, struct_scope->level, struct_scope->num_declarations);
		}break;

		case AST_NODE_NAMED_ARGUMENT: {
			//
			// check for duplicate named arguments from within a function
			//
			assert(scope->symb_table);
			s64 hash = scope->symb_table->entry_exist(node->named_arg.arg_name);
			if (hash == -1) {
				hash = scope->symb_table->insert(scope, node->named_arg.arg_name, node);
			} else {
				report_semantic_error(&node->named_arg.site, "Procedure argument %.*s redefinition.\n", node->named_arg.arg_name->value.length, node->named_arg.arg_name->value.data);
				fprintf(stderr, "Previously defined as argument #%d in the procedure.\n", scope->symb_table->entries[hash].node->named_arg.index + 1);
				return 0;
			}
		}break;

		default: {
			// should not be reached
			report_semantic_error(0, "Internal compiler error: unknown declaration node type.\n");
			scope_error = 1;
			return 0;
		}break;
		}
	}
	if (node->node == AST_NODE_BLOCK) {
		//
		// check recursively for all the declarations and commands of this block
		//
		Scope* block_scope = node->block.scope;
		printf("Block (%d - level[%d]) has %d declarations.\n", block_scope->id, block_scope->level, block_scope->num_declarations);

		if (block_scope->num_declarations) {
			// if there is at least 1 declaration, then create a symbol table (declaration ratio) times
			// bigger than the number of declarations

			block_scope->symb_table = new Symbol_Table(declaration_ratio * block_scope->num_declarations);
		}
		return check_declarations(node->block.commands, block_scope);
	}
	return 1;
}

Type_Instance* get_variable_type(Ast* node, Scope* scope)
{
	assert(node->node == AST_NODE_VARIABLE_EXPRESSION);
	s64 entry = scope->symb_table->entry_exist(node->expression.variable_exp.name);
	if (entry == -1) {
		// search on previous scopes
		while(true) {
			if (!scope->symb_table) {
				scope = scope->parent;
				continue;
			}
			entry = scope->symb_table->entry_exist(node->expression.variable_exp.name);
			if (entry == -1) {
				scope = scope->parent;
				continue;
			}
			else break;
			if (scope->level == 0) break;
			scope = scope->parent;
		}
		if (entry == -1) {
			return 0;
		}
	}
	assert(scope->symb_table->entries[entry].node->node == AST_NODE_VARIABLE_DECL);
	return scope->symb_table->entries[entry].node->var_decl.type;
}

int check_declarations(Ast** ast, Scope* global_scope)
{
	size_t num_nodes = get_arr_length(ast);

	global_scope->symb_table = new Symbol_Table(global_scope->num_declarations * declaration_ratio);

	for (size_t i = 0; i < num_nodes; ++i) {
		Ast* node = ast[i];
		if (check_declarations(node, global_scope) == 0) {
			return DECL_CHECK_FAILED;
		}
	}
	return DECL_CHECK_PASSED;
}

Type_Instance* infer_node_expression_type(Ast* node, Scope* scope, Type_Table* table)
{
	switch (node->node) {
		case AST_NODE_PROCEDURE_CALL: {
			assert(0);
		}break;
		case AST_NODE_BINARY_EXPRESSION: {
			Type_Instance* left = infer_node_expression_type(node->expression.binary_exp.left, scope, table);
			Type_Instance* right = infer_node_expression_type(node->expression.binary_exp.right, scope, table);
			switch (node->expression.binary_exp.op) {
			case BINARY_OP_PLUS:			return left;
			case BINARY_OP_MINUS:			return left;
			case BINARY_OP_MULT:			return left;
			case BINARY_OP_DIV:				return left;
			case BINARY_OP_AND:				return left;
			case BINARY_OP_OR:				return left;
			case BINARY_OP_XOR:				return left;
			case BINARY_OP_MOD:				return left;
			case BINARY_OP_LOGIC_AND:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
			case BINARY_OP_LOGIC_OR:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
			case BINARY_OP_BITSHIFT_LEFT:	return left;
			case BINARY_OP_BITSHIFT_RIGHT:	return left;
			case BINARY_OP_LESS_THAN:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
			case BINARY_OP_GREATER_THAN:	return get_primitive_type(TYPE_PRIMITIVE_BOOL);
			case BINARY_OP_LESS_EQUAL:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
			case BINARY_OP_GREATER_EQUAL:	return get_primitive_type(TYPE_PRIMITIVE_BOOL);
			case BINARY_OP_EQUAL_EQUAL:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
			case BINARY_OP_NOT_EQUAL:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);

			case BINARY_OP_DOT:				return right;

			case ASSIGNMENT_OPERATION_EQUAL:		return left;
			case ASSIGNMENT_OPERATION_PLUS_EQUAL:	return left;
			case ASSIGNMENT_OPERATION_MINUS_EQUAL:	return left;
			case ASSIGNMENT_OPERATION_TIMES_EQUAL:	return left;
			case ASSIGNMENT_OPERATION_DIVIDE_EQUAL:	return left;
			case ASSIGNMENT_OPERATION_AND_EQUAL:	return left;
			case ASSIGNMENT_OPERATION_OR_EQUAL:		return left;
			case ASSIGNMENT_OPERATION_XOR_EQUAL:	return left;
			case ASSIGNMENT_OPERATION_SHL_EQUAL:	return left;
			case ASSIGNMENT_OPERATION_SHR_EQUAL:	return left;
			}
		}break;
		case AST_NODE_UNARY_EXPRESSION: {
			switch (node->expression.unary_exp.op) {
				case UNARY_OP_MINUS:			return node->expression.unary_exp.operand->return_type;
				case UNARY_OP_PLUS:				return node->expression.unary_exp.operand->return_type;
				case UNARY_OP_DEREFERENCE: {
					Type_Instance* inst = node->expression.unary_exp.operand->return_type;
					s64 hash = create_type(&inst->pointer_to, false);
					return table->entries[hash].entry;
				}break;
				case UNARY_OP_ADDRESS_OF: {
					Type_Instance* inst = node->expression.unary_exp.operand->return_type;
					Type_Instance* ti = create_ptr_typeof(inst);
					return ti;
				}break;
				case UNARY_OP_VECTOR_ACCESS: assert(0); break;
				case UNARY_OP_NOT_LOGICAL:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
				case UNARY_OP_NOT_BITWISE:		return node->expression.unary_exp.operand->return_type;
				case UNARY_OP_CAST:				return node->expression.unary_exp.cast_type;
			}
		}break;
		case AST_NODE_EXPRESSION_ASSIGNMENT: {
			// this should be deprecated
			assert(0);
		}break;
		case AST_NODE_LITERAL_EXPRESSION: {
			u32 tok_type = node->expression.literal_exp.lit_tok->type;
			if (tok_type == TOKEN_FLOAT_LITERAL) {
				return get_primitive_type(TYPE_PRIMITIVE_R32);
			} else if (tok_type == TOKEN_INT_LITERAL) {
				return get_primitive_type(TYPE_PRIMITIVE_S32);
			} else if (tok_type == TOKEN_CHAR_LITERAL) {
				return get_primitive_type(TYPE_PRIMITIVE_S32);
			} else if (tok_type == TOKEN_BOOL_LITERAL) {
				return get_primitive_type(TYPE_PRIMITIVE_BOOL);
			} else if (tok_type == TOKEN_STRING_LITERAL) {
				assert(0);
			}
		}break;
		case AST_NODE_VARIABLE_EXPRESSION: {
			return get_variable_type(node, scope);
		}break;
	}
}

void push_infer_queue(Ast* node, Scope* scope)
{
	Infer_Node infer_node;
	node->return_type->flags |= TYPE_FLAG_QUEUED;
	infer_node.node = node;
	infer_node.scope = scope;
	push_array(infer_queue, &node);
}

// returns 0 if failed to resolve
// returns non-zero if resolved
// this function only changed type flags
int resolve_type(Type_Instance* instance, Type_Table* type_table)
{
	//assert(!(instance->flags & TYPE_FLAG_IS_RESOLVED));

	switch (instance->type) {
	case TYPE_PRIMITIVE: {
		return 1;
	}break;
	case TYPE_POINTER: {
		assert(instance->flags & TYPE_FLAG_IS_SIZE_RESOLVED);
		assert(instance->flags & TYPE_FLAG_IS_REGISTER_SIZE);
		if (!resolve_type(instance->pointer_to, type_table)) {
			return 0;
		}
		instance->flags |= TYPE_FLAG_IS_RESOLVED;
		return 1;
	}break;

	case TYPE_FUNCTION: {
		// if could not resolve type of return, fail

		if (!(instance->type_function.return_type->flags & TYPE_FLAG_IS_RESOLVED) &&
			!resolve_type(instance->type_function.return_type, type_table)) {
			return 0;
		}
		// try resolving the type for all the arguments
		int num_args = instance->type_function.num_arguments;
		for (int i = 0; i < num_args; ++i) {
			Type_Instance* in = instance->type_function.arguments_type[i];
			if (!(in->flags & TYPE_FLAG_IS_RESOLVED) && !resolve_type(in, type_table)) {
				return 0;
			}
			in->flags |= TYPE_FLAG_IS_RESOLVED;
		}
		instance->flags |= TYPE_FLAG_IS_RESOLVED;
		return 1;
	}break;

	case TYPE_STRUCT: {
		s64 hash = -1;
		// if the entry exist, that means the structure was declared and it is on the type table
		if (type_table->entry_exist(instance, &hash)) {
			Type_Instance* in = type_table->get_entry(hash);
			assert(in->flags & TYPE_FLAG_IS_RESOLVED);
			assert(in->flags & TYPE_FLAG_IS_SIZE_RESOLVED);
			instance->flags |= TYPE_FLAG_IS_SIZE_RESOLVED;
			instance->flags |= TYPE_FLAG_IS_RESOLVED;
		} else {
			return 0;
		}
	}break;

	default: 
		assert(0); break;	// @todo
	}
}

Type_Instance* get_decl_type(Ast* node)
{
	assert(node->is_decl);
	switch (node->node) {
	case AST_NODE_VARIABLE_DECL:      return node->var_decl.type;
	case AST_NODE_NAMED_ARGUMENT:     return node->named_arg.arg_type;
	case AST_NODE_PROC_DECLARATION:   assert(0); break; //@todo
	case AST_NODE_STRUCT_DECLARATION: return node->struct_decl.type_info;

	default: assert(0); // @todo
	}
}

Token* get_decl_name(Ast* node)
{
	assert(node->is_decl);
	switch (node->node) {
	case AST_NODE_VARIABLE_DECL:      return node->var_decl.name;
	case AST_NODE_NAMED_ARGUMENT:     return node->named_arg.arg_name; 
	case AST_NODE_PROC_DECLARATION:   return node->proc_decl.name;
	case AST_NODE_STRUCT_DECLARATION: return node->struct_decl.name;

	default: assert(0); // @todo
	}
	return 0;
}

// return 0 if infered
// return 1 if queued
// return -1 if errored

int infer_node_types(Ast* node, Scope* scope, Type_Table* table)
{
	bool already_queued = (node->return_type && node->return_type->flags & TYPE_FLAG_QUEUED);
	if (node->is_decl) {
		// if this is a declaration, the return type of this node is always void
		node->return_type = get_primitive_type(TYPE_PRIMITIVE_VOID);
		switch (node->node)
		{
			case AST_NODE_NAMED_ARGUMENT: {
				if (node->named_arg.arg_type->flags & TYPE_FLAG_IS_RESOLVED) return 0;

				assert(node->named_arg.arg_type);
				if (node->named_arg.arg_type->flags & TYPE_FLAG_IS_RESOLVED) {
					// create the type, since it is resolved
					create_type(&node->named_arg.arg_type, true);
				} else {
					if (resolve_type(node->named_arg.arg_type, table)) {
						create_type(&node->named_arg.arg_type, true);
						return 0;
					} else {
						// put it on the queue to be resolved
						if(!already_queued)
							push_infer_queue(node, scope);
						return 1;
					}
				}
			} break;

			case AST_NODE_PROC_DECLARATION: {
				// proc return type creation
				assert(node->proc_decl.proc_ret_type);

				bool infered = true;

				if (!(node->proc_decl.proc_ret_type->flags & TYPE_FLAG_IS_RESOLVED) &&
					!resolve_type(node->proc_decl.proc_ret_type, table)) {
					infered = false;
				} 
				if (infered) {
					create_type(&node->proc_decl.proc_ret_type, true);
				}
				
				// arguments
				int num_args = node->proc_decl.num_args;
				for (int i = 0; i < num_args; ++i) {
					Ast* arg = node->proc_decl.arguments[i];
					if (infer_node_types(arg, node->proc_decl.scope, table) != 0) {
						infered = false;
					}
				}

				int err = infer_node_types(node->proc_decl.body, node->proc_decl.scope, table);

				if (infered) {
					Type_Instance* instance = new Type_Instance();
					instance->type = TYPE_FUNCTION;
					instance->flags = TYPE_FLAG_IS_RESOLVED | TYPE_FLAG_IS_SIZE_RESOLVED | TYPE_FLAG_IS_REGISTER_SIZE;
					instance->type_size = get_size_of_pointer();

					instance->type_function.num_arguments = num_args;
					instance->type_function.return_type = node->proc_decl.proc_ret_type;
					instance->type_function.arguments_type = create_array(Type_Instance*, num_args);

					for (int i = 0; i < num_args; ++i) {
						Type_Instance* in = get_decl_type(node->proc_decl.arguments[i]);
						push_array(instance->type_function.arguments_type, &in);
					}

					create_type(&instance, true);
				} else {
					if (!already_queued)
						push_infer_queue(node, scope);
					return 1;
				}

			}break;

			case AST_NODE_VARIABLE_DECL: {
				if (node->var_decl.type->flags & TYPE_FLAG_IS_RESOLVED) return 0;

				if (node->var_decl.type) {
					if (node->var_decl.type->flags & TYPE_FLAG_IS_RESOLVED) {
						create_type(&node->var_decl.type, true);
					} else {
						if (resolve_type(node->var_decl.type, table)) {
							create_type(&node->var_decl.type, true);
						} else {
							if (!already_queued)
								push_infer_queue(node, scope);
							return 1;
						}
					}
				} else if(node->var_decl.assignment) {
					// infer type from rvalue
					Type_Instance* inst = infer_node_expression_type(node->var_decl.assignment, node->var_decl.scope, table);
					if (inst == 0) {
						// could not infer, put it on the queue
						if (!already_queued)
							push_infer_queue(node, scope);
					} else {
						if (inst->flags & TYPE_FLAG_IS_RESOLVED || resolve_type(inst, table)) {
							create_type(&inst, true);
							node->var_decl.assignment->return_type = inst;
							node->var_decl.type = inst;
						} else {
							if (!already_queued)
								push_infer_queue(node, scope);
							return 1;
						}
					}
				} else {
					report_semantic_error(&node->var_decl.site, "type of variable %.*s could not be inferred, since there is no rvalue assignment.\n", TOKEN_STR(node->var_decl.name));
					return -1;
				}
			}break;

			case AST_NODE_STRUCT_DECLARATION: {
				bool infered = true;
				int num_fields = node->struct_decl.num_fields;
				for (int i = 0; i < num_fields; ++i) {
					Ast* field = node->struct_decl.fields[i];
					if (infer_node_types(field, node->struct_decl.scope, table) != 0) {
						infered = false;
					}
				}
				if (infered) {
					Type_Instance** ftypes = create_array(Type_Instance*, num_fields);
					string* fnames = create_array(string, num_fields);
					s64 size_bytes = 0;
					for (int i = 0; i < num_fields; ++i) {
						Type_Instance* ti = get_decl_type(node->struct_decl.fields[i]);
						Token* name = get_decl_name(node->struct_decl.fields[i]);
						string s = string();
						make_immutable_string(s, name->value.data, name->value.length);
						push_array(fnames, &s);
						size_bytes += ti->type_size;
						push_array(ftypes, &ti);
					}
					Type_Instance* struct_instance = new Type_Instance();
					struct_instance->type = TYPE_STRUCT;
					struct_instance->type_struct.name = node->struct_decl.name->value.data;
					struct_instance->type_struct.name_length = node->struct_decl.name->value.length;
					struct_instance->type_struct.struct_descriptor = node;
					struct_instance->type_struct.fields_types = ftypes;
					struct_instance->type_struct.fields_names = fnames;

					struct_instance->flags = TYPE_FLAG_IS_RESOLVED | TYPE_FLAG_IS_SIZE_RESOLVED;
					struct_instance->type_size = size_bytes;

					create_type(&struct_instance, false);

					node->struct_decl.type_info = struct_instance;
					node->struct_decl.size_bytes = size_bytes;
				} else {
					if (!already_queued)
						push_infer_queue(node, scope);
					return 1;
				}

			}break;
		}
	} else {
		switch (node->node)
		{
			case AST_NODE_PROCEDURE_CALL: {

			}break;

			case AST_NODE_BINARY_EXPRESSION: {

			}break;
				
			case AST_NODE_IF_STATEMENT: {

			}break;

			case AST_NODE_BLOCK: {
				type_inference(node->block.commands, scope, table);
			}break;
		}
	}
	return 0;
}

// return 0 if infered correctly
// return 1 if queued
// return -1 if errored
int type_inference(Ast** ast, Scope* global_scope, Type_Table* table)
{
	int ret_val = 0;
	size_t num_nodes = get_arr_length(ast);
	for (size_t i = 0; i < num_nodes; ++i) {
		Ast* node = ast[i];
		int err = infer_node_types(node, global_scope, table);
		if (err == -1) {
			return -1;
		} else if (err == 1) {
			ret_val = 1;
		}
	}
	return ret_val;
}

int do_type_inference(Ast** ast, Scope* global_scope, Type_Table* type_table)
{
	infer_queue = create_array(Infer_Node, 16);
	if (type_inference(ast, global_scope, type_table) == DECL_CHECK_PASSED) {
		int qsize = get_arr_length(infer_queue);
		while (qsize != 0) {
			for(int i = 0; i < qsize; ++i) {
				Infer_Node in = infer_queue[i];
				if (infer_node_types(in.node, in.scope, type_table) == 0) {
					int n = get_arr_length(infer_queue);
					array_remove(infer_queue, i);
				} else {
					continue;
				}
			}
			int newsize = get_arr_length(infer_queue);
			if (qsize == newsize) {
				report_semantic_error(0, "Detected circulary dependency on type inference, exiting.\n");
				return -1;
			} else if (newsize < qsize) {
				qsize = newsize;
				continue;
			} else {
				report_semantic_error(0, "Internal compiler error, the number of entries in the infer_queue grew, which is an unintender behaviour.\n");
				return -1;
			}
		}
	}
	return 0;
}

int require_expression_type(Ast* node, Type_Instance* type, bool coerce) 
{
	int sum = 0;
	switch (node->node) {
	case AST_NODE_BINARY_EXPRESSION: {
		switch (node->expression.binary_exp.op) {
		case BINARY_OP_PLUS:
		case BINARY_OP_MINUS:
		case BINARY_OP_MULT:
		case BINARY_OP_DIV:
		case BINARY_OP_AND:
		case BINARY_OP_OR:
		case BINARY_OP_XOR:
		case BINARY_OP_MOD:
			sum += require_expression_type(node->expression.binary_exp.left, type, true);
			sum += require_expression_type(node->expression.binary_exp.right, type, true);
			break;
		//case BINARY_OP_LOGIC_AND:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
		//case BINARY_OP_LOGIC_OR:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
		//case BINARY_OP_BITSHIFT_LEFT:
		//case BINARY_OP_BITSHIFT_RIGHT:
		//case BINARY_OP_LESS_THAN:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
		//case BINARY_OP_GREATER_THAN:	return get_primitive_type(TYPE_PRIMITIVE_BOOL);
		//case BINARY_OP_LESS_EQUAL:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
		//case BINARY_OP_GREATER_EQUAL:	return get_primitive_type(TYPE_PRIMITIVE_BOOL);
		//case BINARY_OP_EQUAL_EQUAL:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
		//case BINARY_OP_NOT_EQUAL:		return get_primitive_type(TYPE_PRIMITIVE_BOOL);
		//
		//case BINARY_OP_DOT:				return right;
		//
		//case ASSIGNMENT_OPERATION_EQUAL:		return left;
		//case ASSIGNMENT_OPERATION_PLUS_EQUAL:	return left;
		//case ASSIGNMENT_OPERATION_MINUS_EQUAL:	return left;
		//case ASSIGNMENT_OPERATION_TIMES_EQUAL:	return left;
		//case ASSIGNMENT_OPERATION_DIVIDE_EQUAL:	return left;
		//case ASSIGNMENT_OPERATION_AND_EQUAL:	return left;
		//case ASSIGNMENT_OPERATION_OR_EQUAL:		return left;
		//case ASSIGNMENT_OPERATION_XOR_EQUAL:	return left;
		//case ASSIGNMENT_OPERATION_SHL_EQUAL:	return left;
		//case ASSIGNMENT_OPERATION_SHR_EQUAL:	return left;
		}
	}break;
	case AST_NODE_EXPRESSION_ASSIGNMENT: {

	}break;
	case AST_NODE_LITERAL_EXPRESSION: {

	}break;
	case AST_NODE_PROCEDURE_CALL: {

	}break;
	case AST_NODE_UNARY_EXPRESSION: {

	}break;
	case AST_NODE_VARIABLE_EXPRESSION: {

	}break;

	default: assert(0); break;
	}
	return 1;
}

// return 1 if passed
// return 0 if failed
int type_check_node(Ast* node, Scope* scope, Type_Table* table)
{
	assert(node->return_type);
	assert(node->return_type->flags & TYPE_FLAG_IS_RESOLVED);
	assert(node->return_type->flags & TYPE_FLAG_IS_SIZE_RESOLVED);

	if (node->is_decl) {
		switch (node->node) {
		case AST_NODE_PROC_DECLARATION:	
		case AST_NODE_STRUCT_DECLARATION:
			assert(types_equal(node->return_type, get_primitive_type(TYPE_PRIMITIVE_VOID)));
			break;
		case AST_NODE_NAMED_ARGUMENT: {
			if (node->named_arg.default_value) {
				// check the rvalue to match
			}
		}break;

		case AST_NODE_VARIABLE_DECL: {

		}break;

		default: assert(0); break;
		}
	} else {
		assert(0);	// @todo
	}
	return 1;
}

// return 1 if passed
// return 0 if failed
int do_type_check(Ast** ast, Scope* global_scope, Type_Table* table)
{
	int err = 1;
	size_t num_nodes = get_arr_length(ast);
	for (size_t i = 0; i < num_nodes; ++i) {
		Ast* node = ast[i];
		int ret = type_check_node(node, global_scope, table);
		if (ret == 0) {
			err = 0;
		}
	}
	return err;
}