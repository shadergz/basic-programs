# basic-programs

A collection of quick applications made for personal use

#### entropy
    - Calculate the entropy of the files passed via the command line

#### hexdump
    - Display the file content in hexadecimal and octal

#### today
    - Display the files modified today in the specified directories

#### disasop
    - Decompiles the specified instructions from a variety of RISC architectures
    - Supported processors: "thumb", "a32", "a64", "mips32", "mipsel", "mips64", "mips64el"

#### bin2array
    - Dump the content of the specified file into a C++ header file

### Compile all the tools with the following commands
- mkdir build
- cd build
- cmake -DCMAKE_BUILD_TYPE=Release -GNinja ..
- ninja

### Third licenses
- cmake/FindLLVM.cmake was grab from [ldc](https://github.com/ldc-developers/ldc/tree/master)