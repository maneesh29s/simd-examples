#ifndef include_helpers_hpp
#define include_helpers_hpp

#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

class Timer {
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> end;

public:
  void start_timer() { this->start = std::chrono::high_resolution_clock::now(); }
  void stop_timer() { this->end = std::chrono::high_resolution_clock::now(); }

  auto time_elapsed() {
    return std::chrono::duration_cast<std::chrono::microseconds>(this->end - this->start)
        .count();
  }
};

template < typename T >
void random_vector_generator(std::vector<T> &inputData) {
  time_t seed = time(NULL);
  std::cout << "Seed: " << seed << std::endl;
  srand(seed);

  std::cout << "Array dim : " << inputData.size() << std::endl;

  T offset = -5;
  T range = 10;
  for (size_t i = 0; i < inputData.size(); i++) {
    inputData[i] = offset + range * (rand() / (T)RAND_MAX);
  }
}

void assert_int(size_t expected, size_t actual, std::string str) {
  if (expected != actual) {
    std::cerr << str << " expected: " << expected << " actual: " << actual
              << std::endl;
    assert(expected == actual);
  }
}

void assert_float(float expected, float actual, std::string str) {
  float diff = fabs(expected - actual);
  float loss = (diff / expected) * 100;
  if (loss > 0.1) {
    std::cerr << str << " expected: " << expected << " actual: " << actual
              << " loss: " << loss << std::endl;
    assert(loss < 0.1);
  }
}

#endif /* include_helpers_hpp */
