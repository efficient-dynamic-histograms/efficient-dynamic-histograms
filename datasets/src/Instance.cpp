#include <datasets/Instance.hpp>

Instance::Instance(const std::vector<feature_t> &features)
    : m_features(features) {
}

Instance::Instance(std::vector<feature_t> &&features)
    : m_features(std::move(features)) {
}

const std::vector<feature_t> &Instance::features() const {
  return m_features;
}

const feature_t &Instance::at(const uint32_t idx) const {
  return m_features.at(idx);
}

const feature_t &Instance::operator[](const uint32_t idx) const {
  return at(idx);
}

const feature_t &Instance::at(const Column &column) const {
  return at(column.m_index);
}

uint32_t Instance::size() const {
  return m_features.size();
}
