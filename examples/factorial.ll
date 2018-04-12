target triple = "x86_64-linux-gnu"

%vec2 = type { float, float }

declare cc 64 i8* @malloc(i64 %size) #0
define i32 @main() #1 {
decls-0:
%0 = alloca i32*, align 8
%1 = alloca i32*, align 8
%2 = load i32*, i32** %0
%3 = add i32 2, 0

%4 = zext i32 %3 to i64
%5 = getelementptr i32, i32* %2, i64 %4

store i32* %5, i32** %1
ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
