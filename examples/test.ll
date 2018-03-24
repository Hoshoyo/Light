target triple = "x86_64-pc-windows-msvc"

%string = type { i64, i8* }

declare cc 64 i32 @ExitProcess(i32 %exit_code) #0
declare cc 64 void @print_string(i64 %length,i8* %data) #0
declare cc 64 void @print_double(double %v) #0
define i32 @factorial(i32 %val) #1 {
decls-0:
%0 = icmp eq i32 %val, 1
br i1 %0, label %if-stmt-0, label %if-end-0
if-stmt-0:
ret i32 1

br label %if-end-0
if-end-0:

%2 = sub i32 %val, 1
%3 = call i32 @factorial(i32  %2)

%4 = mul i32 %val, %3
ret i32 %4

}
define i32 @fibonacci(i32 %val) #1 {
decls-0:
%0 = icmp eq i32 %val, 0
br i1 %0, label %if-stmt-0, label %if-stmt-1
if-stmt-0:
ret i32 0

br label %if-end-0
if-stmt-1:%2 = icmp eq i32 %val, 1
br i1 %2, label %if-stmt-2, label %if-stmt-3
if-stmt-2:
ret i32 1

br label %if-end-2
if-stmt-3:%4 = sub i32 %val, 1
%5 = call i32 @fibonacci(i32  %4)

%6 = sub i32 %val, 2
%7 = call i32 @fibonacci(i32  %6)

%8 = add i32 %5, %7
ret i32 %8

br label %if-end-2
if-end-2:

br label %if-end-0
if-end-0:

ret i32 0

}
define i32 @main() #1 {
decls-0:
%0 = alloca i32, align 4

%1 = call i32 @fibonacci(i32 4)

store i32 %1, i32* %0
%2 = load i32, i32* %0
%3 = call i32 @ExitProcess(i32  %2)


ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
