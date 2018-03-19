target triple = "x86_64-linux-gnu"

%string = type { i64, i8* }

define i32 @gcd(i32 %a,i32 %b) #1 {
decls-0:
%0 = icmp ne i32 %b, 0
br i1 %0, label %if-stmt-0, label %if-end-0
if-stmt-0:
%1 = srem i32 %a, %b
%2 = call i32 @gcd(i32 %b, i32  %1)

ret i32 %2

br label %if-end-0
if-end-0:

ret i32 %a

}
define i32 @main() #1 {
decls-0:

%0 = call i32 @gcd(i32 27, i32 9)

ret i32 %0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
