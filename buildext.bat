@echo off

pushd bin
pushd exported
set LIGHT_FILES=../../src/parser.c ../../src/lexer.c ../../src/utils/catstring.c ../../src/internal.c ../../src/global_tables.c ../../src/error.c ../../src/utils/os.c ../../src/symbol_table.c ../../src/type.c ../../src/type_infer.c ../../src/top_typecheck.c ../../src/type_table.c ../../src/eval.c ../../src/utils/allocator.c ../../src/utils/utils.c ../../src/utils/string_table.c
call cl /c /MD /nologo /Zi /I../../include ../../src/exported/*.c %LIGHT_FILES% ../../src/ast.c /Felight.exe
popd
popd
