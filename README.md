# SIMD Examples

This repo contains SIMD implementations of

1. Absolute value calculator

2. Minimum and Maximum value calculator

NOTE: 

We are able to run SSE instrinsics on ARM using a header file `sse2neon.h`, which converts the SSE intrinsics into equivalent ARM NEON intrinsics. Running AVX instrinsincs on ARM machine is not possible.

We are also using OpenMP to run multi-threaded implementation of the benchmarks.

## How to compile

Run `make all` which will compile both min-max.cpp and abs.cpp and store the executable in `build/src` directory.
Have a look at the Makefile to see which options are included.

## How to benchmark

Simply run `scripts/bench.sh` ( or `scripts/benchM1Arm.sh` if running on Apple M1 / any ARM machine) with 

1. hostname: output files will be stored in `stat/hostname` directory

2. benchmark to run: currently `min-max` or `abs`

For example:

```bash
./scripts/bench.sh my-machine min-max
```

Output of the script is a csv file containing the time taken for different scenarios, and a PNG image of the plot. The output will be stored in `stat/hostname` directory.
