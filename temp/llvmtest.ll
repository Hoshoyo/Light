declare cc 64 i32 @ExitProcess(i32) #0
define i32 @proc() #0
{
decls-0:
 %0 = alloca i32, align 4
 %1 = sub i32 5, 2
 %2 = sdiv i32 %1, 2
 %3 = add i32 3, %2
 store i32 %3, i32* %0
 %4 = alloca i32, align 4
 %5 = load i32, i32* %0
 %6 = add i32 %5, 5
 store i32 %6, i32* %4
 %7 = load i32, i32* %4
 call cc 64 i32 @ExitProcess(i32 %7)
 ret i32 %7
}
attributes #0 = {nounwind uwtable}