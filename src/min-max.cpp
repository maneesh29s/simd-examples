#include "helpers.hpp"
#include "simd.h"
#include <cfloat>
#include <chrono>
#include <iostream>
#include <vector>

/*
"Golden" algorithm to find minimum and maximum value
Takes a floating vector "arr" as input.
Writes the minimum and maximum value in the "min" and "max" variables
respectively, which are passed as reference.
*/
void minMaxGolden(const std::vector<float> &arr, float &min, float &max) {
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


void minMaxOMP(const std::vector<float> &arr, float &min, float &max) {
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
    minMaxGolden(arr, minExpected, maxExpected);
    t.stop_timer();
    std::cout << "Elapsed time golden : " << t.time_elapsed() << std::endl;
  }

  {
    float minActual = FLT_MAX, maxActual = FLT_MIN;

    t.start_timer();
    minMaxOMP(arr, minActual, maxActual);
    t.stop_timer();
    std::cout << "Elapsed time openmp : " << t.time_elapsed() << std::endl;

    assertFloat(maxExpected, maxActual, "maxopenmp");
    assertFloat(minExpected, minActual, "minopenmp");
    std::cout << "Assertion is successful for openmp" << std::endl;
  }

  {
    float minActual = FLT_MAX, maxActual = FLT_MIN;

    t.start_timer();
    minMaxSSE(arr.data(), N, &minActual, &maxActual);
    t.stop_timer();
    std::cout << "Elapsed time SIMD SSE : " << t.time_elapsed() << std::endl;

    assertFloat(maxExpected, maxActual, "maxSSE");
    assertFloat(minExpected, minActual, "minSSE");
    std::cout << "Assertion is successful for SSE" << std::endl;
  }

  {
    float minActual = FLT_MAX, maxActual = FLT_MIN;

    t.start_timer();
    minMaxSSEOMP(arr.data(), N, &minActual, &maxActual);
    t.stop_timer();
    std::cout << "Elapsed time SIMD SSE+openmp : " << t.time_elapsed() << std::endl;

    assertFloat(maxExpected, maxActual, "maxSSEOMP");
    assertFloat(minExpected, minActual, "minSSEOMP");
    std::cout << "Assertion is successful for SSE+openmp" << std::endl;
  }

#ifdef __AVX__
  {
    float minActual = FLT_MAX, maxActual = FLT_MIN;

    t.start_timer();
    minMaxAVX(arr.data(), N, &minActual, &maxActual);
    t.stop_timer();
    std::cout << "Elapsed time SIMD AVX : " << t.time_elapsed() << std::endl;

    assertFloat(maxExpected, maxActual, "maxAVX");
    assertFloat(minExpected, minActual, "minAVX");
    std::cout << "Assertion is successful for AVX" << std::endl;
  }

  {
    float minActual = FLT_MAX, maxActual = FLT_MIN;

    t.start_timer();
    minMaxAVXOMP(arr.data(), N, &minActual, &maxActual);
    t.stop_timer();
    std::cout << "Elapsed time SIMD AVX+openmp : " << t.time_elapsed() << std::endl;

    assertFloat(maxExpected, maxActual, "maxAVXOMP");
    assertFloat(minExpected, minActual, "minAVXOMP");
    std::cout << "Assertion is successful for AVX+openmp" << std::endl;
  }
#endif
}
