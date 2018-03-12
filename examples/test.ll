target triple = "x86_64-pc-windows-msvc"

%string = type { i64, i8* }

declare cc 64 i32 @ExitProcess(i32 %exit_code) #0
declare cc 64 void @print_string(i64 %length,i8* %data) #0
declare cc 64 i64 @print_s64(i64 %v) #0
declare cc 64 void @print_double(double %v) #0
define void @main() #1 {
decls-0:
%0 = alloca double, align 4
%1 = fsub double 2.0, 3.0
store double %1, double* %0
%2 = load double, double* %0
%3 = fcmp ogt double %2, 0.0
br i1 %3, label %if-stmt-0, label %if-stmt-1
if-stmt-0:
%4 = load double, double* %0
call void @print_double(double  %4)

br label %if-end-0
if-stmt-1:

call void @print_double(double 99.9)
br label %if-end-0

if-end-0:


%5 = call i32 @ExitProcess(i32 0)

ret void
}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
