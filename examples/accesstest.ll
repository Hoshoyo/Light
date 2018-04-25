target triple = "x86_64-linux-gnu"

%vec4 = type { float, float, float, float }
%Person = type { { float, float, float, float }, i32 }

define void @main() #1 {
decls-0:
%0 = alloca %vec4, align 8
%1 = alloca float, align 8
%2 = load float, float* %1
%3 = %vec4 %0, %2
store float %3, float* %1
}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
