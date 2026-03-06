#pragma once

#include <iostream>
#include <utils/Sample.hpp>
#include <utils/Slice.hpp>
#include <variant>

template <typename T> class DatasetContainer {
  using StandardVector = std::vector<T>;
  using SlicedVector = Slice<T>;

public:
  enum class StorageKind { Vector, Slice };

  DatasetContainer()
      : m_vector(StandardVector()), m_kind(StorageKind::Vector),
        get_function_const(&DatasetContainer::get_from_vector_const) {
  }

  DatasetContainer(const std::vector<T> &data)
      : m_vector(data), m_kind(StorageKind::Vector),
        get_function_const(&DatasetContainer::get_from_vector_const) {
  }

  DatasetContainer(std::vector<T> &&rows)
      : m_vector(rows), m_kind(StorageKind::Vector),
        get_function_const(&DatasetContainer::get_from_vector_const) {
  }

  DatasetContainer(const std::vector<T> &data,
                   const std::vector<uint64_t> &indexes)
      : m_slice(Slice(data, indexes)), m_kind(StorageKind::Slice),
        get_function_const(&DatasetContainer::get_from_slice_const) {
  }

  DatasetContainer(const Slice<T> &slice)
      : m_slice(slice), m_kind(StorageKind::Slice),
        get_function_const(&DatasetContainer::get_from_slice_const) {
  }

  uint64_t size() const {
    if (m_kind == StorageKind::Vector) {
      return m_vector.size();
    }
    return m_slice.size();
  }

  const T &operator[](uint64_t idx) const {
    return get_function_const(*this, idx);
  }

  const T &at(uint64_t idx) const {
    return get_function_const(*this, idx);
  }

  const std::vector<T> &full_data() const {
    if (m_kind == StorageKind::Vector) {
      return m_vector;
    }
    return m_slice.get_data();
  }

  StorageKind kind() const {
    return m_kind;
  }
  DatasetContainer<T> sample(uint64_t sample_size,
                             std::mt19937_64 &engine) const {
    uint64_t max_size = size();
    std::vector<u_int64_t> sample_idx =
        Sample::draw_sample(max_size, sample_size, engine);
    if (m_kind == StorageKind::Vector) {
      return DatasetContainer(Slice(m_vector, sample_idx));
    }
    std::vector<u_int64_t> slice_idx(sample_size);
    for (uint64_t i = 0; i < sample_size; i++) {
      sample_idx[i] = m_slice.m_indexes[sample_idx[i]];
    }
    return DatasetContainer(*m_slice.m_data, sample_idx);
  }

private:
  using IndexFnConst = const T &(*)(const DatasetContainer &, size_t);

  static const T &get_from_vector_const(const DatasetContainer &self,
                                        uint64_t idx) {
    return self.m_vector[idx];
  }

  static const T &get_from_slice_const(const DatasetContainer &self,
                                       uint64_t idx) {
    return self.m_slice[idx];
  }
  StandardVector m_vector;
  SlicedVector m_slice;
  StorageKind m_kind;
  IndexFnConst get_function_const;
};
