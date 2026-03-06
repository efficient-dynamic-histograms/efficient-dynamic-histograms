#pragma once

#include <random>
#include <vector>
namespace Sample {
template <typename T, typename Container = std::vector<T>>
Container draw_sample(T range, T sample_size, std::mt19937_64 engine) {
  std::vector<uint32_t> values(range);
  for (T i = 0; i < range; ++i)
    values[i] = i;
  for (T i = 0; i < sample_size; ++i) {
    std::uniform_int_distribution<T> dist(i, range - 1);
    std::swap(values[i], values[dist(engine)]);
  }
  return std::vector<T>(values.begin(), values.begin() + sample_size);
}
}; // namespace Sample