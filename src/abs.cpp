#include "helpers.hpp"
#include "simd.h"
#include <chrono>
#include <climits>
#include <iostream>
#include <vector>

Timer<std::chrono::microseconds> t;

void absGolden(std::vector<int> &arr, std::vector<int> &expected) {
  for (size_t i = 0; i < arr.size(); i++) {
    expected[i] = abs(arr[i]);
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
    absGolden(inputData, expected);
    t.stop_timer();
    std::cout << "Elapsed time GOLDEN " << t.time_elapsed() << std::endl;
  }
  // SSE Approach
  {
    std::vector<int> sse_actual(N);
    t.start_timer();
    absSSE(inputData.data(), N, sse_actual.data());
    t.stop_timer();
    std::cout << "Elapsed time SSE " << t.time_elapsed() << std::endl;

    for (size_t i = 0; i < N; i++) {
      assertInt(expected[i], sse_actual[i], "SSE");
    }
    std::cout << "Assertion is successful" << std::endl;
  }
}