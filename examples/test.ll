%string = type { i64, i8* }

declare cc 64 i32 @ExitProcess(i32 %exit_code) #0
define void @main() #1 {
decls-0:
%0 = alloca %string, align 4
%1 = getelementptr [6 x i8], [6 x i8]* @__str$0, i64 0, i64 0

store %string %1, %string* %0

%2 = call i32 @ExitProcess(i32 0)

ret void
}
define void @print_double(double %value) #0 {
	%str = getelementptr[3 x i8], [3 x i8] * @__str$4, i64 0, i64 0
	%1 = call i32(i8*, ...) @printf(i8* %str, double %value)
	ret void
}

@__str$4 = private global [3 x i8] c"%f\00"

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
@__str$0 = private global [6 x i8] c"hello\00"
