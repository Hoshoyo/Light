target triple = "x86_64-pc-windows-msvc"


declare cc 64 i32 @ExitProcess(i32 %v) #0
declare cc 64 i8* @GetStdHandle(i32 %std_handle) #0
declare cc 64 i8 @WriteConsoleA(i8* %console_output,i8* %lp_buffer,i32 %num_chars_to_write,i32* %num_chars_written,i8* %reserved) #0
declare cc 64 i8* @malloc(i64 %size) #0
define i32 @main() #1 {
decls-0:
%0 = alloca i32*, align 8

%1 = call i8* @malloc(i64 256)

%2 = bitcast i8* %1 to i32*

store i32* %2, i32** %0
%3 = load i32*, i32** %0

store i32 0, i32* %3
%4 = load i32*, i32** %0

%5 = getelementptr i32, i32* %4, i64 1

store i32 1, i32* %5
%6 = load i32*, i32** %0

%7 = load i32*, i32** %0

%8 = getelementptr i32, i32* %7, i64 1

%9 = load i32, i32* %8

%10 = zext i32 %9 to i64
%11 = getelementptr i32, i32* %6, i64 %10

store i32 5, i32* %11
%12 = alloca i32, align 8
%13 = load i32*, i32** %0
%14 = getelementptr i32, i32* %13, i64 1

%15 = load i32, i32* %14

%16 = call i32 @ExitProcess(i32  %15)

store i32 %16, i32* %12
ret i32 0

}
declare cc 64 i64 @print_s64(i64 %v) #0

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
