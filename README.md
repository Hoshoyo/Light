# Light Programming Language
----

The light programming language focuses on simplicity, while offering the maximum amount of control to the programmer. It is a low-level, statically typed and meta-programmable language. The syntax is very similar to [Go](https://golang.org/) and [JAI](https://www.youtube.com/watch?v=TH9VCN6UkyQ&list=PLmV5I2fxaiCKfxMBrNsU1kgKJXD3PkyxO), but also various other more modern languages.

The language is currently under development and is very unstable.

## Planned Features
* [x] Simple language core
* [x] Type inference
* [x] Runtime type information
* [ ] Compile time execution of code
* [ ] Code AST modification
* [ ] Meta-programming support
* [ ] Code introspection
* [ ] Embedded build system support

## [Simple Example](./examples/windows/factorial.li)
```go
// No ifdef equivalents for now so we must do this for Windows
#import "../../modules/windows/print.li"

// Linux equivalent
// #import "../../modules/linux/print.li"

factorial : (num : s32) -> s32 {
    if num <= 1 {
        return 1;
    } else {
        return num * factorial(num - 1);
    }
}

main : () -> s32 {
    number := 5;
    result := factorial(number);
    print("Factorial of % is %\n", number, result);
    return result;
}
```

You can find more examples for windows and linux on [examples/windows](./examples/windows) and [examples/linux](./examples/linux)

To compile them, run from the root directory (on Windows):

```bash
bin\light.exe examples\windows\game\game.li
examples\windows\game\game.exe
```

Or for linux:

```bash
bin/light examples/linux/game/game.li
./examples/linux/game/game
```

## Build

### Linux Build

* Dependencies: `nasm` and `gcc`.
  - To install dependencies on Ubuntu/Debian run: sudo apt install nasm.
* To build run `make` at the root directory.
* The executable is build at `bin/light`

### Windows

* Run `build.bat` on the Visual studio developer console
* Dependencies: Visual Studio 2017. (masm and cl)
