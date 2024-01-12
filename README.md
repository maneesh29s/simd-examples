# SIMD Examples

This repo contains SIMD implementations of

1. Absolute value calculator

2. Minimum and Maximum value calculator

## to compile on m1 mac

```cpp
g++ -std=c++14 -O3 -march=armv8-a+fp+simd+crc -I include/ -o <output> <input>
```

## to compile on x86_64

```cpp
g++ -std=c++14 -O3 -march=native -I include/ -o <output> <input>
```
