%string = type { i64, i8* }
%vec4 = type { float, float, float, float }

declare ccc i32 i32 @proc(float %v1,i32 %v2,i32 %v3) #1 {
%0 = alloca i64, align 4
store i64 3, i64* %0
%1 = load i64, i64* %0
}
declare ccc i32 void @main() #1 {
%0 = alloca i32, align 4
%1 = call @proc()

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
