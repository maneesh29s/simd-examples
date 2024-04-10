#!/usr/bin/env bash

if [[ $# -lt 2 ]]
then
  echo "Usage: $0 host benchmark_to_run"
  exit 1
fi

set -e
set -x

# Different array sizes for which the benchmark should run
SIZE_EXPONENTS=(27 28 29 30 31)
# DATASIZE=

host=$1
bench=$2

# Running the benchmarks

mkdir -p stat/$host
rm -f stat/$host/$bench-output.txt stat/$host/$bench-stats.csv stat/$host/$bench-performance.png
make src/$bench

for sz in "${SIZE_EXPONENTS[@]}"
do
  echo "./build/src/${bench} $sz" >> stat/$host/$bench-output.txt
  ./build/src/${bench} $sz >> stat/$host/$bench-output.txt
done

# writing to CSV

echo "size, golden, omp, sse, sseomp" > stat/$host/$bench-stats.csv

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
    printf("%s, %s, %s, %s, %s\n", size, golden, omp, sse, sseomp); \
  }                                           \
' >> stat/$host/$bench-stats.csv

# Plotting the graph

echo "                                            \
  reset;                                          \
  set terminal png enhanced large font \"Times New Roman,12\"; \
                                                         \
  set title \"$bench Benchmark\";                        \
  set xlabel \"Number of elements in array (taken as 2^n) \";                             \
  set ylabel \"Execution time (us)\";                     \
  set datafile separator ',';                           \
  set key autotitle columnhead;                         \
  set key left top;                                      \
  set logscale y;                                        \
  set xrange [((${SIZE_EXPONENTS[0]} - 1)):((${SIZE_EXPONENTS[-1]} + 1))]
                                                         \
  plot \"stat/$host/$bench-stats.csv\" using 1:2 with linespoint, \
       \"stat/$host/$bench-stats.csv\" using 1:3 with linespoint,  \
       \"stat/$host/$bench-stats.csv\" using 1:4 with linespoint,   \
       \"stat/$host/$bench-stats.csv\" using 1:5 with linespoint;   \
" | gnuplot > stat/$host/$bench-performance.png
