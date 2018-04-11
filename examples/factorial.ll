target triple = "x86_64-linux-gnu"

%vec2 = type { float, float }

declare cc 64 i8* @malloc(i64 %size) #0
define i32 @main() #1 {
decls-0:
%0 = alloca i32, align 8
%1 = alloca i32, align 8

store i32 %-1, i32* %1
ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
