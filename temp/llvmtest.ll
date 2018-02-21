%..string = type {i64, i8*}

declare cc 64 i32 @ExitProcess(i32) #0
declare cc 64 i32 @MessageBoxA(i8*, i8*, i8*, i32) #0
declare cc 64 i8* @GetStdHandle(i32) #0
declare cc 64 i32 @WriteConsoleA(i8*, i8*, i32, i32*, i8*) #0
define i32 @main() #0
{
decls-0:
 %0 = alloca  %..string, align 4
 %1 = getelementptr %..string,  %..string* %0, i64 0, i32 0
 store i64 6, i64* %1
 %2 = getelementptr  %..string,  %..string* %0, i64 0, i32 1
 %str1 = getelementptr [6 x i8], [6 x i8]* @__str.0, i64 0, i64 0
 store i8* %str1, i8** %2
  
 %3 = call cc 64 i8* @GetStdHandle(i32 -11)
  
 %written = alloca i32, align 4  
 %4 = load i8*, i8** %2
 %5 = load i64, i64* %1
 %6 = trunc i64 %5 to i32
 %7 = call cc 64 i32 @WriteConsoleA(i8* %3, i8* %4, i32 %6, i32* %written, i8* zeroinitializer)
 
 call cc 64 i32 @ExitProcess(i32 0)
 ret i32 0
}

@__str.0 = private global [6 x i8] c"Hello\00"
attributes #0 = {nounwind uwtable}