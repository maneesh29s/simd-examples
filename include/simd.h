#ifndef include_simd_h
#define include_simd_h

#include <assert.h>
#include <limits.h>

#ifdef __x86_64__
#include <immintrin.h>
#endif

#ifdef __ARM_NEON__
#include "sse2neon.h"
#endif

static void absSSE(int const *const arr, size_t const N, int *const abs_arr) {
  assert(N < (size_t)INT_MAX);

  const int simd_width = 4;
  size_t quot = N / simd_width;
  size_t limit = quot * simd_width;

  for (size_t i = 0; i < limit; i += simd_width) {
    const __m128i temp_arr =
        _mm_load_si128(reinterpret_cast<const __m128i *>(arr + i));

    __m128i arr_r = _mm_abs_epi32(temp_arr);

    _mm_store_si128(reinterpret_cast<__m128i *>(abs_arr + i), arr_r);
  }

  // abs for the remainder
  for (size_t i = limit; i < N; i++) {
    abs_arr[i] = abs(arr[i]);
  }
}

// SSE code operating on 32-bit floats
static void minMaxSSE(float const *const arr, size_t const N, float *const min,
                      float *const max) {

  assert(N < (size_t)INT_MAX);

  // 4 32-bit floats stored in 128-bit registers
  const int simd_width = 4;
  size_t quot = N / simd_width;
  size_t limit = quot * simd_width;

  __m128 arr_r = _mm_loadu_ps(arr);
  __m128 max_r = arr_r;
  __m128 min_r = arr_r;

  for (size_t i = simd_width; i < limit; i += simd_width) {
    arr_r = _mm_loadu_ps(arr + i);

    min_r = _mm_min_ps(min_r, arr_r);
    max_r = _mm_max_ps(max_r, arr_r);
  }

  float max_tmp[simd_width];
  float min_tmp[simd_width];

  _mm_storeu_ps(min_tmp, min_r);
  _mm_storeu_ps(max_tmp, max_r);

  *max = max_tmp[0];
  *min = min_tmp[0];

  for (int i = 1; i < simd_width; i++) {
    if (max_tmp[i] > *max) {
      *max = max_tmp[i];
    }
    if (min_tmp[i] < *min) {
      *min = min_tmp[i];
    }
  }

  // Calculating min-max for remaining elements
  for (size_t i = limit; i < N; i++) {
    if (*max < arr[i]) {
      *max = arr[i];
    }
    if (*min > arr[i]) {
      *min = arr[i];
    }
  }
}

// Multithreaded SSE code operating on 32-bit floats
static void minMaxSSEOMP(float const *const arr, size_t const N,
                         float *const min, float *const max) {

  assert(N < (size_t)INT_MAX);

  // 4 32-bit floats stored in 128-bit registers
  const int simd_width = 4;
  size_t quot = N / simd_width;
  size_t limit = quot * simd_width;

#pragma omp parallel
  {
    float min_local = *min;
    float max_local = *max;
    __m128 arr_r = _mm_loadu_ps(arr);
    __m128 max_r = arr_r;
    __m128 min_r = arr_r;

#pragma omp for
    for (size_t i = simd_width; i < limit; i += simd_width) {
      arr_r = _mm_loadu_ps(arr + i);

      // sleep(1);

      min_r = _mm_min_ps(min_r, arr_r);
      max_r = _mm_max_ps(max_r, arr_r);
    }
    float max_tmp[simd_width];
    float min_tmp[simd_width];

    _mm_storeu_ps(min_tmp, min_r);
    _mm_storeu_ps(max_tmp, max_r);

    max_local = max_tmp[0];
    min_local = min_tmp[0];

    for (int i = 1; i < simd_width; i++) {
      if (max_tmp[i] > max_local) {
        max_local = max_tmp[i];
      }
      if (min_tmp[i] < min_local) {
        min_local = min_tmp[i];
      }
    }
#pragma omp critical
    {
      if (max_local > *max) {
        *max = max_local;
      }
      if (min_local < *min) {
        *min = min_local;
      }
    }
  }

  // Calculating min-max for remaining elements
  for (size_t i = limit; i < N; i++) {
    if (*max < arr[i]) {
      *max = arr[i];
    }
    if (*min > arr[i]) {
      *min = arr[i];
    }
  }
}

#ifdef __AVX2__
// AVX code operating on 32-bit floats
static void minMaxAVX(float const *const arr, size_t const N, float *const min,
                      float *const max) {

  assert(N < (size_t)INT_MAX);

  const int simd_width = 8;
  __m256 arr_r = _mm256_loadu_ps(arr);
  __m256 max_r = arr_r;
  __m256 min_r = arr_r;

  size_t quot = N / simd_width;
  size_t limit = quot * simd_width;

  for (size_t i = simd_width; i < limit; i += simd_width) {
    arr_r = _mm256_loadu_ps(arr + i);

    min_r = _mm256_min_ps(min_r, arr_r);
    max_r = _mm256_max_ps(max_r, arr_r);
  }

  float max_tmp[simd_width];
  float min_tmp[simd_width];

  _mm256_storeu_ps(min_tmp, min_r);

  _mm256_storeu_ps(max_tmp, max_r);

  *max = max_tmp[0];
  *min = min_tmp[0];

  for (int i = 1; i < simd_width; i++) {
    if (max_tmp[i] > *max) {
      *max = max_tmp[i];
    }
    if (min_tmp[i] < *min) {
      *min = min_tmp[i];
    }
  }

  // Min max for reminder
  for (size_t i = limit; i < N; i++) {
    if (*max < arr[i]) {
      *max = arr[i];
    }
    if (*min > arr[i]) {
      *min = arr[i];
    }
  }
}
#endif

#ifdef __AVX2__
// Multithreaded AVX code operating on 32-bit floats
static void minMaxAVXOMP(float const *const arr, size_t const N,
                         float *const min, float *const max) {

  assert(N < (size_t)INT_MAX);

  // 8 32-bit floats stored in 256-bit registers
  const int simd_width = 8;
  size_t quot = N / simd_width;
  size_t limit = quot * simd_width;

#pragma omp parallel
  {
    float min_local = *min;
    float max_local = *max;
    __m256 arr_r = _mm256_loadu_ps(arr);
    __m256 max_r = arr_r;
    __m256 min_r = arr_r;

#pragma omp for
    for (size_t i = simd_width; i < limit; i += simd_width) {
      arr_r = _mm256_loadu_ps(arr + i);

      // sleep(1);

      min_r = _mm256_min_ps(min_r, arr_r);
      max_r = _mm256_max_ps(max_r, arr_r);
    }
    float max_tmp[simd_width];
    float min_tmp[simd_width];

    _mm256_storeu_ps(min_tmp, min_r);
    _mm256_storeu_ps(max_tmp, max_r);

    max_local = max_tmp[0];
    min_local = min_tmp[0];

    for (int i = 1; i < simd_width; i++) {
      if (max_tmp[i] > max_local) {
        max_local = max_tmp[i];
      }
      if (min_tmp[i] < min_local) {
        min_local = min_tmp[i];
      }
    }
#pragma omp critical
    {
      if (max_local > *max) {
        *max = max_local;
      }
      if (min_local < *min) {
        *min = min_local;
      }
    }
  }

  // Calculating min-max for remaining elements
  for (size_t i = limit; i < N; i++) {
    if (*max < arr[i]) {
      *max = arr[i];
    }
    if (*min > arr[i]) {
      *min = arr[i];
    }
  }
}
#endif

#endif // include_simd_h