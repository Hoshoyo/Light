target triple = "x86_64-linux-gnu"

%string = type { i64, i8* }

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
define i32 @main() #1 {
decls-0:
%0 = alloca %string, align 4
%1 = getelementptr [6 x i8], [6 x i8]* @__str$0, i64 0, i64 0
%2 = getelementptr %string, %string* %0, i64 0, i32 0
%3 = getelementptr %string, %string* %0, i64 0, i32 1
store i64 6, i64* %2
store i8* %1, i8** %3


%4 = call i32 @factorial(i32 4)

ret i32 %4

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
@__str$0 = private global [6 x i8] c"hello\00"
