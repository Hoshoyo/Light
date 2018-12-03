# Light Programming Language
----

The light programming language focuses on simplicity, while offering the maximum amount of control to the programmer. It is a low-level, statically typed and meta-programmable language. The syntax is very similar to [Go](https://golang.org/) and [JAI](https://www.youtube.com/watch?v=TH9VCN6UkyQ&list=PLmV5I2fxaiCKfxMBrNsU1kgKJXD3PkyxO), but also various other more modern languages.

The language is currently under development and is very unstable.

## Planned Features
* Simple language core
* Type inference
* Compile time execution of code
* Code AST modification
* Meta-programming support
* Code introspection
* Embedded build system support

## Example
```go

main :: () -> s32 {
  print("Hello World\n");
  return 0;
}

```

You can find more examples in the `examples` directory.

## Build

### Linux Build

* Dependencies: `nasm` and `gcc`.
  - To install dependencies on Ubuntu run: sudo apt-get install nasm.
* To build run `make` at the root directory.
* The executable is build at `bin/gcc/light`

### Windows

Dependencies: Visual Studio 2017. (masm and cl)
