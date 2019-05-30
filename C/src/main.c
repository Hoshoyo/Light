#include <stdio.h>
#include "lexer.h"

int main() {
    char str[] = "main :: () -> s32 { return 0; break; }";

    Light_Lexer lexer = {0};
    Light_Token* tokens = lexer_cstr(&lexer, str, sizeof(str), 0);
    return 0;
}