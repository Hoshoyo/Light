## TODO

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