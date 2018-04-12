target triple = "x86_64-linux-gnu"


declare cc 64 i8* @malloc(i64 %size) #0
declare cc 64 i64 @write(i32 %fd,i8* %buffer,i64 %size) #0
declare cc 64 i64 @print_s64(i64 %value) #0
define i32 @main() #1 {
decls-0:
%0 = alloca i8*, align 8

%1 = call i8* @malloc(i64 10)

%2 = bitcast i8* %1 to i8*

store i8* %2, i8** %0
%3 = load i8*, i8** %0

store i8 48, i8* %3
%4 = sext i64 1 to i64
%5 = getelementptr i8*, i8** %0, i64 %4

%6 = load i8*, i8** %5

store i8 49, i8* %6
ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
