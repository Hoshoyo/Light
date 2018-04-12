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
%6 = ptrtoint i8* %5 to i64

%7 = call i64 @print_s64(i64  %6)

store i64 %7, i64* %4
%8 = alloca i64, align 8
%9 = load i8*, i8** %0
%10 = bitcast i8* %9 to i8*

%11 = call i64 @write(i32 1, i8*  %10, i64 1)

store i64 %11, i64* %8
ret i32 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
