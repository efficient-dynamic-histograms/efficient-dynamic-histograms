#include <datasets/Dataset.hpp>

#include <datasets/FinalInstance.hpp>

template <typename AbstractInstance>
IDataset<AbstractInstance>::IDataset(
    const DatasetContainer<AbstractInstance> &rows,
    const std::vector<Column> &columns)
    : m_rows(rows), m_columns(columns) {
}

template <typename AbstractInstance>
IDataset<AbstractInstance>::IDataset(DatasetContainer<AbstractInstance> &&rows,
                                     const std::vector<Column> &columns)
    : m_rows(std::move(rows)), m_columns(columns) {
}

template <typename AbstractInstance>
uint64_t IDataset<AbstractInstance>::size() const {
  return m_rows.size();
}

template <typename AbstractInstance>
uint32_t IDataset<AbstractInstance>::num_dimensions() const {
  return m_columns.size();
}

template <typename AbstractInstance>
bool IDataset<AbstractInstance>::size_le(const uint64_t threshold) const {
  return size() <= threshold;
}

template <typename AbstractInstance>
const AbstractInstance &IDataset<AbstractInstance>::at(uint64_t idx) const {
  return m_rows.at(idx);
}
template <typename AbstractInstance>
std::tuple<IDataset<AbstractInstance>, IDataset<AbstractInstance>>
IDataset<AbstractInstance>::split(const Column &column,
                                  const feature_t &value) const {
  std::vector<uint64_t> left;
  std::vector<uint64_t> right;
  left.reserve(m_rows.size());
  right.reserve(m_rows.size());

  for (uint64_t idx = 0; idx < m_rows.size(); idx++) {
    if (m_rows[idx].at(column.m_index) < value) {
      left.push_back(idx);
    } else {
      right.push_back(idx);
    }
  }

  IDataset<AbstractInstance> left_dataset(
      Slice<AbstractInstance>(m_rows.full_data(), left), m_columns);
  IDataset<AbstractInstance> right_dataset(
      Slice<AbstractInstance>(m_rows.full_data(), right), m_columns);

  return {left_dataset, right_dataset};
}
template <typename AbstractInstance>
feature_t IDataset<AbstractInstance>::min(const Column &column) const {
  auto range = std::views::iota(uint64_t{0}, size());
  return m_rows
      .at(*std::min_element(range.begin(), range.end(),
                            [this, &column](uint64_t a, uint64_t b) {
                              return m_rows.at(a).at(column) <
                                     m_rows.at(b).at(column);
                            }))
      .at(column);
}

template <typename AbstractInstance>
feature_t IDataset<AbstractInstance>::max(const Column &column) const {
  auto range = std::views::iota(uint64_t{0}, size());
  return m_rows
      .at(*std::max_element(range.begin(), range.end(),
                            [this, &column](uint64_t a, uint64_t b) {
                              return m_rows.at(a).at(column) <
                                     m_rows.at(b).at(column);
                            }))
      .at(column);
}
template <typename AbstractInstance>
IDataset<AbstractInstance>
IDataset<AbstractInstance>::sample(uint64_t sample_size,
                                   std::mt19937_64 &engine) const {
  DatasetContainer<AbstractInstance> new_rows =
      m_rows.sample(sample_size, engine);
  return IDataset<AbstractInstance>(new_rows, m_columns);
}

template class IDataset<Instance>;
template class IDataset<LabeledInstance>;
template class IDataset<FinalInstance>;
