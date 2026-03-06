#pragma once

template <typename D> class SliceIterator {
public:
  using DataType = typename D::DataType;

public:
  SliceIterator(const DataType *begin, const uint64_t *indexes,
                uint64_t position, uint64_t slice_size)
      : m_begin(begin), m_indexes(indexes), m_position(position),
        m_slice_size(slice_size) {
  }

  SliceIterator &operator++() {
    m_position++;
    return *this;
  }

  SliceIterator operator++(int) {
    SliceIterator iterator = *this;
    ++(*this);
    return iterator;
  }

  SliceIterator &operator--() {
    m_position--;
  }

  SliceIterator operator--(int) {
    SliceIterator iterator = *this;
    --(*this);
    return iterator;
  }

  DataType &operator[](uint64_t index) {
    return *(m_begin + *(m_indexes + index));
  }

  DataType *operator->() {
    return m_begin + *(m_indexes + m_position);
  }

  const DataType &operator*() {
    return *(m_begin + *(m_indexes + m_position));
  }

  bool operator==(const SliceIterator &other) const {
    return m_indexes + m_position == other.m_indexes + other.m_position;
  }

  bool operator!=(const SliceIterator &other) const {
    return !(*this == other);
  }

private:
  const DataType *m_begin;
  const uint64_t *m_indexes;
  uint64_t m_position;
  uint64_t m_slice_size;
};

template <typename T> class Slice {
public:
  using DataType = T;
  using Iterator = SliceIterator<Slice<T>>;

public:
  Slice(const std::vector<T> &data, const std::vector<uint64_t> &indexes)
      : m_data(&data), m_indexes(indexes) {
  }

  Slice(const Slice<T> &data, const std::vector<uint64_t> &indexes)
      : m_data(data.m_data), m_indexes(indexes) {
  }

  Slice() : empty(true), m_indexes() {
  }

  Iterator begin() const {
    return SliceIterator<Slice<T>>(&m_data[0], &m_indexes[0], 0,
                                   m_indexes.size());
  }

  Iterator end() const {
    return SliceIterator<Slice<T>>(&m_data[0], &m_indexes[0], m_indexes.size(),
                                   m_indexes.size());
  }

  uint64_t size() const {
    return m_indexes.size();
  }

  const T &operator[](uint64_t idx) const {
    return (*m_data)[m_indexes[idx]];
  }

  const std::vector<T> &get_data() const {
    return *m_data;
  }

  const union {
    const std::vector<T> *m_data;
    bool empty = false;
  };

  const std::vector<uint64_t> m_indexes;
};