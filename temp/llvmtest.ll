%string = type { i64, i8* }
%vec4 = type { float, float, float, float }

declare ccc i32 i32 @proc(float %v1,i32 %v2,i32 %v3) #1 {
%0 = icmp ult i8 1, 2
br i1 %0, label %if-stmt-0, label %if-stmt-1
if-stmt-0:
%1 = add i32 1, 1
ret i32 %1
if-stmt-1:
ret i32 4
}
declare ccc i32 void @main() #1 {
%0 = alloca i32, align 4
%1 = call @proc()

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
