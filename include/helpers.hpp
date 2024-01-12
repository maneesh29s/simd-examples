#ifndef include_helpers_hpp
#define include_helpers_hpp

#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

#define MAXGENRAND 0x100000000 // 2^32

template <typename T> class Timer {
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> end;

public:
  void start_timer() {
    this->start = std::chrono::high_resolution_clock::now();
  }
  void stop_timer() { this->end = std::chrono::high_resolution_clock::now(); }

  std::string time_elapsed() {
    auto time_taken =
        std::chrono::duration_cast<T>(this->end - this->start).count();

    std::string unit = "";
    if (std::is_same<T, std::chrono::seconds>::value)
      unit = "s";
    else if (std::is_same<T, std::chrono::milliseconds>::value)
      unit = "ms";
    else if (std::is_same<T, std::chrono::microseconds>::value)
      unit = "us";
    else if (std::is_same<T, std::chrono::nanoseconds>::value)
      unit = "ns";

    return std::to_string(time_taken) + " " + unit;
  }
};

template <typename T>
std::vector<T> generateRandomData(const size_t arrSize, T minVal, T maxVal,
                                  uint32_t seed) {
  std::mt19937 gen(seed);

  std::vector<T> arr(arrSize);

  double range = maxVal - minVal;

  for (size_t i = 0; i < arr.size(); i++) {
    arr[i] = minVal + static_cast<T>(range * gen() / MAXGENRAND);
  }

  return arr;
}

template <typename T>
std::vector<T> generateSequentialData(const size_t arrSize, T start, T diff) {
  std::vector<T> arr(arrSize);

  for (size_t i = 0; i < arr.size(); i++) {
    arr[i] = start + (T)(i * diff);
  }
  return arr;
}

void assertInt(size_t expected, size_t actual, std::string str) {
  if (expected != actual) {
    std::cerr << str << " expected: " << expected << " actual: " << actual
              << std::endl;
    assert(expected == actual);
  }
}

void assertFloat(float expected, float actual, std::string str) {
  float diff = fabs(expected - actual);
  float loss = (diff / expected) * 100;
  if (loss > 0.1) {
    std::cerr << str << " expected: " << expected << " actual: " << actual
              << " loss: " << loss << std::endl;
    assert(loss < 0.1);
  }
}

template <typename T> void printVector(const std::vector<T> &vec) {
  for (T value : vec) {
    std::cout << value << " ";
  }
  std::cout << std::endl;
}

#endif // include_helpers_hpp
