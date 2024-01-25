# SIMD Examples

This repo contains SIMD implementations of

1. Absolute value calculator

2. Minimum and Maximum value calculator

NOTE: We are able to run SSE instrinsics on ARM using a header file `sse2neon.h`, which converts the SSE intrinsics into equivalent ARM NEON intrinsics. 

Running AVX instrinsincs on ARM machine is not possible.


## How to compile

Run `make all` which will compile both min-max.cpp and abs.cpp and store the executable in `build/src` directory.
Have a look at the Makefile to see which options are included.

