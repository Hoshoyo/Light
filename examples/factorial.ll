target triple = "x86_64-pc-windows-msvc"


declare cc 64 i32 @ExitProcess(i32 %r) #0
declare cc 64 i8* @malloc(i64 %size) #0
define i32 @factorial(i32 %v) #1 {
decls-0:
%0 = icmp eq i32 %v, 1
br i1 %0, label %if-stmt-0, label %if-end-0
if-stmt-0:
ret i32 1
br label %if-end-0
if-end-0:

%2 = sub i32 %v, 1
%3 = call i32 @factorial(i32  %2)

%4 = mul i32 %v, %3
ret i32 %4

}
define i32 @main() #1 {
decls-0:
%0 = alloca i8*, align 8

%1 = call i8* @malloc(i64 4)

store i8* %1, i8** %0
%2 = alloca i8*, align 8

store i8* %-1, i8** %2
%3 = alloca i32, align 8

%4 = call i32 @factorial(i32 6)

store i32 %4, i32* %3
%5 = load i32, i32* %3
%6 = call i32 @ExitProcess(i32  %5)
store i32 %6, i32* %3
ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
