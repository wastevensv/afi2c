# afi2c
A Forth Interpreter written in C.

## Dependencies
The AFI interpreter is designed to be cross platform, with system-specific frontends.

The AFI interpreter requires a C11 compiler with malloc.

The AFI standard frontend and test suite also requires the readline library.

## Building
To build the AFI static library and frontend, simply use `make`.

By default, debugging symbols are included, if a smaller binary is preferred, use `make RELEASE=y`.

To build a different frontend, use `make TARGET=<somefrontend>`. Currently only `test` and `linux` are valid by default.

To build only the static library, use `make afi.a`.

## Running
After building, the frontend binaries and static library should be in the `bin/` subdirectory. Simply run one of the programs from the shell of your choice.

## Usage
AFI is a stack based language, heavily based on [FORTH](https://en.wikipedia.org/wiki/Forth_(programming_language)). AFI currently works a lot like a [Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation) calculator.

Currently, AFI has a very small standard library. This consists of arithmetic operations, branching, and basic stack operations.

These include:

* `+`, `-`, `*`, `/`
    * Example: `2 4 + 3 - 4 *` will push 12 onto the stack.
* `.`, `.D`, `PEEK`
    * Prints the top element of the stack to stdio, without changing the stack.
* `POP`
    * Prints the top element of the stack to stdio, and removes the element from the stack.
* `DROP`
    * Discards the top element of the stack without printing.
* `IF`, `ELSE`, `END`
    * If the top of the stack is non-zero, any code between `IF` and `ELSE` is executed. Otherwise, the code between `ELSE` and `END` is executed.
