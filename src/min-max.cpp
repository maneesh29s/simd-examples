#include "helpers.hpp"
#include <cfloat>
#include <iostream>
#include <vector>
#include <climits>
#include <stddef.h>

#ifdef __x86_64__
#include <immintrin.h>
#endif

#ifdef __ARM_NEON__
#include "sse2neon.h"
#endif

void golden(const std::vector<float> &arr, size_t N, float &min, float &max) {
  min = FLT_MAX;
  max = FLT_MIN;
  for (size_t i = 0; i < N; i++) {
    if (min > arr[i]) {
      min = arr[i];
    }
    if (max < arr[i]) {
      max = arr[i];
    }
  }
}

// SSE code - 128 bit registers
void simd_sse(float *arr, size_t N, float &min, float &max) {

  assert(N < (size_t)INT_MAX);

  // 4 32-bit floats stored in 128-bit registers 
  const int simd_width = 4;
  __m128 arr_r = _mm_loadu_ps(arr);
  __m128 max_r = arr_r;
  __m128 min_r = arr_r;

  size_t quot = N / simd_width;
  size_t limit = quot * simd_width;

  for (size_t i = simd_width; i < limit; i += simd_width) {
    arr_r = _mm_loadu_ps(arr + i);

    min_r = _mm_min_ps(min_r, arr_r);
    max_r = _mm_max_ps(max_r, arr_r);
  }

  float max_tmp[simd_width];
  float min_tmp[simd_width];

  _mm_storeu_ps(min_tmp, min_r);
  _mm_storeu_ps(max_tmp, max_r);

  max = max_tmp[0];
  min = min_tmp[0];

  for (int i = 1; i < simd_width; i++) {
    if (max_tmp[i] > max) {
      max = max_tmp[i];
    }
    if (min_tmp[i] < min) {
      min = min_tmp[i];
    }
  }

  // Min max for reminder
  for (size_t i = limit; i < N; i++) {
    if (max < arr[i]) {
      max = arr[i];
    }
    if (min > arr[i]) {
      min = arr[i];
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << " usage: ./min-max <N>" << std::endl;
    return 1;
  }

  size_t N = atoi(argv[1]);
  std::vector<float> arr(N);

  random_vector_generator(arr);

  float minExpected = FLT_MAX, maxExpected = FLT_MIN;
  {
    Timer t;

    t.start_timer();

    golden(arr, N, minExpected, maxExpected);

    t.stop_timer();

    std::cout << "Elapsed time golden: " << t.time_elapsed() << " us"
              << std::endl;
  }

  std::cout << "min: " << minExpected << " max " << maxExpected << std::endl;

  {
    float minActual = FLT_MAX, maxActual = FLT_MIN;
    Timer t;

    t.start_timer();
    simd_sse(arr.data(), N, minActual, maxActual);
    t.stop_timer();
    std::cout << "Elapsed time SIMD SSE: " << t.time_elapsed() << " us"
              << std::endl;

    assert_float(maxExpected, maxActual, "maxSSE");
    assert_float(minExpected, minActual, "minSSE");
  }
}