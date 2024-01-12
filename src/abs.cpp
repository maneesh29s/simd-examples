#include "helpers.hpp"
#include <chrono>
#include <climits>
#include <iostream>
#include <vector>

#ifdef __x86_64__
#include <immintrin.h>
#endif

#ifdef __ARM_NEON__
#include "sse2neon.h"
#endif

Timer<std::chrono::microseconds> t;

void abs_golden(std::vector<int> &arr, std::vector<int> &expected) {
  for (size_t i = 0; i < arr.size(); i++) {
    expected[i] = abs(arr[i]);
  }
}

void abs_sse(std::vector<int> &arr, std::vector<int> &abs_arr) {
  assert(arr.size() < (size_t)INT_MAX);

  const int simd_width = 4;
  size_t quot = arr.size() / simd_width;
  size_t limit = quot * simd_width;

  for (size_t i = 0; i < limit; i += simd_width) {
    const __m128i temp_arr =
        _mm_load_si128(reinterpret_cast<const __m128i *>(arr.data() + i));

    __m128i arr_r = _mm_abs_epi32(temp_arr);

    _mm_store_si128(reinterpret_cast<__m128i *>(abs_arr.data() + i), arr_r);
  }

  // abs for the remainder
  for (size_t i = limit; i < arr.size(); i++) {
    abs_arr[i] = abs(arr[i]);
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " size-exponent" << std::endl;
    std::cerr << "Size of the array generated will be 2^(size-exponent)"
              << std::endl;
    return 1;
  }

  // generate random data vector of size N
  int exponent = std::atoi(argv[1]);
  size_t N = std::pow(2, exponent);

  std::vector<int> inputData = generateRandomData<int>(N, -10000, 10000, 2);

  // GOLDEN Approach. Calculates "expected" values
  std::vector<int> expected(N);
  {
    t.start_timer();
    abs_golden(inputData, expected);
    t.stop_timer();
    std::cout << "Elapsed time GOLDEN " << t.time_elapsed() << std::endl;
  }
  // SSE Approach
  {
    std::vector<int> sse_actual(N);
    t.start_timer();
    abs_sse(inputData, sse_actual);
    t.stop_timer();
    std::cout << "Elapsed time SSE " << t.time_elapsed() << std::endl;

    for (size_t i = 0; i < N; i++) {
      assertInt(expected[i], sse_actual[i], "SSE");
    }
    std::cout << "Assertion is successful" << std::endl;
  }
}