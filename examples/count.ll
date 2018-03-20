target triple = "x86_64-linux-gnu"

%string = type { i64, i8* }

declare cc 64 void @print_s64(i64 %v) #0
define void @count(i64 %v) #1 {
decls-0:
%0 = icmp eq i64 %v, 0
br i1 %0, label %if-stmt-0, label %if-end-0
if-stmt-0:
ret void

br label %if-end-0
if-end-0:


call void @print_s64(i64 %v)


%2 = sub i64 %v, 1
call void @count(i64  %2)


ret void

}
define i32 @main() #1 {
decls-0:

call void @count(i64 10)


ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
