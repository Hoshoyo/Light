target triple = "x86_64-linux-gnu"

%string = type { i64, i8* }

declare cc 64 void @print_string(i64 %length,i8* %data) #0
declare cc 64 void @print_double(double %v) #0
define i32 @main() #1 {
decls-0:
%0 = fadd double 6.3, 7.1
call void @print_double(double  %0)


ret i32 2

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
