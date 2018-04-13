target triple = "x86_64-pc-windows-msvc"


declare cc 64 i32 @ExitProcess(i32 %v) #0
declare cc 64 i64 @print_s64(i64 %v) #0
define i32 @main() #1 {
decls-0:
%0 = alloca i64, align 8

%1 = call i64 @print_s64(i64 123)

store i64 %1, i64* %0
%2 = alloca i32, align 8

%3 = call i32 @ExitProcess(i32 0)

store i32 %3, i32* %2
ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
