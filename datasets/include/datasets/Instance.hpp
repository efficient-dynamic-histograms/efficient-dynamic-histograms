#pragma once

#include <datasets/Column.hpp>
#include <sys/types.h>
#include <utils/Types.hpp>
#include <vector>

class Instance {
public:
  explicit Instance(const std::vector<feature_t> &features);

  explicit Instance(std::vector<feature_t> &&features);

  [[nodiscard]] const std::vector<feature_t> &features() const;

  [[nodiscard]] const feature_t &at(uint32_t idx) const;

  [[nodiscard]] const feature_t &at(const Column &column) const;

  [[nodiscard]] const feature_t &operator[](uint32_t idx) const;

  [[nodiscard]] uint32_t size() const;

protected:
  std::vector<feature_t> m_features;
};
