%string = type { i64, i8* }
%vec4 = type { float, float, float, float }

define void @_start() #1 {
	module asm "mov eax, 1"
	module asm "xor ebx, ebx"
	module asm "int 0x80"
unreachable
}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
