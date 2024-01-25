#!/usr/bin/env bash

if [[ $# -lt 2 ]]
then
  echo "Usage: $0 host benchmark_to_run"
  exit 1
fi

set -e
set -x

SIZE=(27 28 29 30 31)

host=$1
bench=$2

mkdir -p stat/$host

rm -f stat/$host/$bench-output.txt stat/$host/$bench-stats.csv stat/$host/$bench-performance.png

make src/$bench

for sz in "${SIZE[@]}"
do
  echo "./build/src/${bench} $sz" >> stat/$host/$bench-output.txt
  ./build/src/${bench} $sz >> stat/$host/$bench-output.txt
done

#  TODO

cat stat/$host/$bench-output.txt | awk '                          \
  /build\/src/ {                              \
    size = $NF;                               \
  }                                           \
  /Elapsed time golden :/ {                     \
    golden = $(NF-1);                         \
  }                                           \
  /Elapsed time openmp :/ {                     \
    omp = $(NF-1);                         \
  }                                           \
  /Elapsed time SIMD SSE :/ {                   \
    sse = $(NF-1);                            \
  }                                           \
  /Elapsed time SIMD SSE\+openmp :/ {                   \
    sseomp = $(NF-1);                            \
  }                                           \
  /Elapsed time SIMD AVX :/ {                   \
    avx = $(NF-1);                            \
  }                                           \
  /Elapsed time SIMD AVX\+openmp :/ {                   \
    avxomp = $(NF-1);                            \
    printf("%s, %s, %s, %s, %s, %s, %s\n", size, golden, omp, sse, sseomp, avx, avxomp); \
  }                                           \
' > stat/$host/$bench-stats.csv

# echo "                                            \
#   reset;                                          \
#   set terminal png enhanced large font \"Helvetica,10\"; \
#                                                          \
#   set title \"$bench Benchmark\";                        \
#   set xlabel \"Matrix Dim\";                             \
#   set ylabel \"Execution time(us)\";                     \
#   set key left top;                                      \
#   set logscale x;                                        \
#   set logscale y;                                        \
#                                                          \
#   plot \"stat/$host/$bench-stats.csv\" using 1:2 with linespoint title \"Golden\", \
#        \"stat/$host/$bench-stats.csv\" using 1:3 with linespoint title \"SSE\",    \
#        \"stat/$host/$bench-stats.csv\" using 1:4 with linespoint title \"AVX\";    \
# " | gnuplot > stat/$host/$bench-performance.png
