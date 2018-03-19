target triple = "x86_64-linux-gnu"

%string = type { i64, i8* }

declare cc 64 void @print_string(i64 %length,i8* %data) #0
declare cc 64 void @print_double(double %v) #0
define double @hello(double %a,double %b) #1 {
decls-0:
%0 = fadd double %a, %b
ret double %0

}
define i32 @factorial(i32 %val) #1 {
decls-0:
%0 = 
br i1 %0, label %if-stmt-0, label %if-end-0
if-stmt-0:
ret i32 1

br label %if-end-0
if-end-0:

%1 = 
%2 = call i32 @factorial(i32  %1)

%3 = 
ret i32 %3

}
define i32 @main() #1 {
decls-0:
%0 = alloca double, align 4

%1 = call double @hello(double 1.2, double 2.3)

store double %1, double* %0

%2 = load double, double* %0
call void @print_double(double  %2)


ret i32 2

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
