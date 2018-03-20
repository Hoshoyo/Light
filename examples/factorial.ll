target triple = "x86_64-pc-windows-msvc"

%string = type { i64, i8* }

declare cc 64 i32 @ExitProcess(i32 %v) #0
define i32 @factorial(i32 %value) #1 {
decls-0:
%0 = icmp eq i32 %value, 1
br i1 %0, label %if-stmt-0, label %if-end-0
if-stmt-0:
ret i32 1

br label %if-end-0
if-end-0:

%2 = sub i32 %value, 1
%3 = call i32 @factorial(i32  %2)

%4 = mul i32 %value, %3
ret i32 %4

}
define i32 @main() #1 {
decls-0:

%0 = call i32 @ExitProcess(i32 32)


ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
