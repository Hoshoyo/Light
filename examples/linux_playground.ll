target triple = "x86_64-linux-gnu"


declare cc 64 i8* @malloc(i64 %size) #0
declare cc 64 i64 @write(i32 %fd,i8* %buffer,i64 %size) #0
declare cc 64 i64 @print_s64(i64 %value) #0
define void @repeat(i64 %n) #1 {
decls-0:
%0 = icmp eq i64 %n, 0
br i1 %0, label %if-stmt-0, label %if-end-0
if-stmt-0:
ret void
br label %if-end-0
if-end-0:

%2 = alloca i8*, align 8

%3 = call i8* @malloc(i64 512)

%4 = bitcast i8* %3 to i8*

store i8* %4, i8** %2
%5 = load i8*, i8** %2

store i8 72, i8* %5
%6 = load i8*, i8** %2

%7 = getelementptr i8, i8* %6, i64 1

store i8 101, i8* %7
%8 = load i8*, i8** %2

%9 = getelementptr i8, i8* %8, i64 2

store i8 108, i8* %9
%10 = load i8*, i8** %2

%11 = getelementptr i8, i8* %10, i64 3

store i8 108, i8* %11
%12 = load i8*, i8** %2

%13 = getelementptr i8, i8* %12, i64 4

store i8 111, i8* %13
%14 = load i8*, i8** %2

%15 = getelementptr i8, i8* %14, i64 5

store i8 10, i8* %15
%16 = alloca i64, align 8
%17 = load i8*, i8** %2
%18 = bitcast i8* %17 to i8*

%19 = call i64 @write(i32 1, i8*  %18, i64 6)

store i64 %19, i64* %16
%20 = sub i64 %n, 1
call void @repeat(i64  %20)


ret void

}
define i64 @main() #1 {
decls-0:

call void @repeat(i64 10)


ret i64 0

}

attributes #0 = { nounwind uwtable }
attributes #1 = { nounwind uwtable }
