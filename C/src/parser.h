#include <common.h>
#include "ast.h"
#include "lexer.h"
#include "utils/catstring.h"

typedef enum {
    PARSER_OK            = 0,
    PARSER_ERROR_FATAL   = (1 << 0),
    PARSER_ERROR_WARNING = (1 << 1),
} Light_Parser_Error;

typedef struct Light_Parser_t{
    Light_Lexer* lexer;
    Light_Scope* scope_global;

    Light_Ast**  top_level;

    String_Table parse_queue;
    string*      parse_queue_array;
} Light_Parser;

// General
Light_Ast** parse_top_level(Light_Parser* parser, Light_Lexer* lexer, Light_Scope* global_scope, u32* error);
void        parse_init(Light_Parser* parser, Light_Lexer* lexer, Light_Scope* global_scope, const char* main_file);

// Type
Light_Type* parse_type(Light_Parser* parser, Light_Scope* scope, u32* error);

// Declaration
Light_Ast* parse_declaration(Light_Parser* parser, Light_Scope* scope, bool require_semicolon, u32* error);

// Command
Light_Ast* parse_command(Light_Parser* parser, Light_Scope* scope, u32* error, bool require_semicolon);

// Expression
Light_Ast* parse_expression(Light_Parser* parser, Light_Scope* scope, u32* error);

// Directives