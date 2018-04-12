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
%4 = alloca i64, align 8
%5 = load i8*, i8** %0
%6 = load i8, i8* %5

%7 = zext i8 %6 to i64

%8 = call i64 @print_s64(i64  %7)

store i64 %8, i64* %4
%9 = alloca i64, align 8
%10 = load i8*, i8** %0
%11 = getelementptr i8, i8* %10, i64 1

%12 = load i8, i8* %11

%13 = zext i8 %12 to i64

%14 = call i64 @print_s64(i64  %13)

store i64 %14, i64* %9
ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
