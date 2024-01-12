#include "helpers.hpp"
#include <cfloat>
#include <chrono>
#include <climits>
#include <iostream>
#include <stddef.h>
#include <vector>

#ifdef __x86_64__
#include <immintrin.h>
#endif

#ifdef __ARM_NEON__
#include "sse2neon.h"
#endif

void golden(const std::vector<float> &arr, float &min, float &max) {
  min = max = arr[0];
  for (size_t i = 1; i < arr.size(); i++) {
    if (min > arr[i]) {
      min = arr[i];
    }
    if (max < arr[i]) {
      max = arr[i];
    }
  }
}

void openmp(const std::vector<float> &arr, float &min, float &max) {
  min = max = arr[0];
  size_t N = arr.size();
#pragma omp parallel
  {
    float min_local = min;
    float max_local = max;
#pragma omp for nowait
    for (size_t i = 1; i < N; i++) {
      float tmp = arr[i];
      if (tmp < min_local) {
        min_local = tmp;
      }
      if (tmp > max_local) {
        max_local = tmp;
      }
    }
#pragma omp critical
    {
      if (min_local < min) {
        min = min_local;
      }
      if (max_local > max) {
        max = max_local;
      }
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
    std::cerr << "Usage: ./min-max size-exponent" << std::endl;
    std::cerr << "Size of the array generated will be 2^(size-exponent)"
              << std::endl;
    return 1;
  }

  int exponent = std::atoi(argv[1]);
  const size_t N = std::pow(2, exponent);

  std::vector<float> arr = generateRandomData<float>(N, -10000.0, 10000.0, 10);

  Timer<std::chrono::microseconds> t;

  float minExpected = FLT_MAX, maxExpected = FLT_MIN;
  {
    t.start_timer();

    golden(arr, minExpected, maxExpected);

    t.stop_timer();

    std::cout << "Elapsed time golden: " << t.time_elapsed() << std::endl;
  }

  {
    float minActual = FLT_MAX, maxActual = FLT_MIN;

    t.start_timer();
    simd_sse(arr.data(), N, minActual, maxActual);
    t.stop_timer();
    std::cout << "Elapsed time SIMD SSE: " << t.time_elapsed() << std::endl;

    assertFloat(maxExpected, maxActual, "maxSSE");
    assertFloat(minExpected, minActual, "minSSE");

    std::cout << "Assertion is successful for SSE" << std::endl;
  }

  {
    float minActual = FLT_MAX, maxActual = FLT_MIN;

    t.start_timer();
    openmp(arr, minActual, maxActual);
    t.stop_timer();
    std::cout << "Elapsed time openmp: " << t.time_elapsed() << std::endl;

    assertFloat(maxExpected, maxActual, "openmp");
    assertFloat(minExpected, minActual, "openmp");

    std::cout << "Assertion is successful for openmp" << std::endl;
  }
}
