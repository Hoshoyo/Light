target triple = "x86_64-linux-gnu"

%string = type { i64, i8* }

define i32 @pstr(double %v) #1 {
decls-0:
%0 = alloca i32, align 4
ret i32 67

}
define i32 @main() #1 {
decls-0:

%0 = call i32 @pstr(double 6.0)

ret i32 %0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }

