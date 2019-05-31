#include <common.h>
#include "ast.h"
#include "lexer.h"

typedef enum {
    PARSER_OK            = 0,
    PARSER_ERROR_FATAL   = (1 << 0),
    PARSER_ERROR_WARNING = (1 << 1),
} Light_Parser_Error;

typedef struct {
    Light_Lexer* lexer;
    Light_Scope* scope_global;

    Light_Ast**  top_level;
} Light_Parser;

// General
Light_Ast** parse_top_level(Light_Parser* parser, Light_Lexer* lexer, Light_Scope* global_scope, u32* error);

// Type
Light_Ast* parse_type(Light_Parser* parser, Light_Scope* scope);

// Declaration
Light_Ast* parse_declaration(Light_Parser* parser, Light_Scope* scope);

// Command
Light_Ast* parse_command(Light_Parser* parser, Light_Scope* scope);

// Expression
Light_Ast* parse_expression(Light_Parser* parser, Light_Scope* scope);

// Directives