What is this?
=============

This code consists of wrapper functions that filter parameters of potentially
"unsafe" libc functions (like `gets`) to make them usable without the risk of
of buffer overflows.

How does it work?
=================

Code is instrumented with AddressSanitizer and wrapper functions around
(potentially) unsafe libc functions use information from AddressSanitizer to
prevent buffer overflows. Unterminated strings, wrong length information and
unsafe functions like `gets` are no problem anymore.

How to build the toolchain
==========================

```
git clone https://github.com/llvm-mirror/llvm
(cd llvm/tools && git clone https://github.com/llvm-mirror/clang)
(cd llvm/projects && git clone https://github.com/pekd/compiler-rt)
(cd llvm/projects && git checkout safe)
mkdir llvm-build && cd llvm-build
CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_ASSERTIONS=ON -DLLVM_TARGETS_TO_BUILD=X86 ~/git/llvm
export PATH=$(pwd)/bin:$PATH
```

How to build a program using the safe libc
==========================================

Use the `compile` script (and edit the `CLANG_PATH` if necessary):

```
./compile -o gets examples/gets.c
```
