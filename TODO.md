## TODO

TODO from toplevel.c:719 - this is the implementation of string literals inside if statements for C backend.
need to cleanup naming with hoassembler, otherwise is gonna be hell
Fix the external caller for linux

// This causes a crash
m := (index % 3) == 2 && index > 0;
if m { }

This cast is busted!
main : () -> s64 {    
    return 0x123456789abcd -> s64;
}

This is f*'d
l += s64_to_str(v -> s64, (buffer->^u8 + l) -> [32]u8);

comparing a casted external procedure to a null gives an assertion
if glClearColor -> ^void != null {

This without the cast to u64 causes an assert
INVALID_HANDLE_VALUE :: ((-1 -> u64) -> ^void);

this causes a strange error
if (size != null) *size = total_read -> u64;

### Backend x86
- x86 functions returning void are causing a crash when not explicitly using return
- x86 backend passing array as an argument is doing it by copy, i think

### Others
- taking address of a function type gives wrong result
- returning integer in a function that returns ^u8 causes circular dependencies
- function returning wrong type causes circular dependencies
- null should be cast automatically to proc type
- fix bug where passing array to function causes gcc warning and is also wrong
- investigate dot operator when struct type is still not internalized
- strings escape sequences in C backend are wrong
- numeric literals better type inference
- type max size now is 32bit
- enum reflecting
- incomplete struct literals
- implement #type_value and #type
- implement all language in bytecode

## Done

- lexical range
- size_of struct with only a character is 4
- internalization of pointers to aliases that are not declared do not cause an error
- referencing a declared variable inside a for loop does not work
- referencing main in a variable causes error since we renamed it to __light_main
- <<= not working
- variable declared void ( a := arr[0]; where arr is ^void) 
