%string = type { i64, i8* }
%vec4 = type { float, float, float, float }

declare ccc i32 i32 @proc(float %v1,i32 %v2,i32 %v3) #1 {
	
}
declare ccc i32 void @main() #1 {
	%0 = alloca i32, align 4
	%1 = call @proc(float 1.1, i32 2, i32 3)

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
