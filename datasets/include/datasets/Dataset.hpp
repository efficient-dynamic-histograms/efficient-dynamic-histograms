#pragma once

#include <algorithm>
#include <datasets/Column.hpp>
#include <datasets/FinalInstance.hpp>
#include <datasets/Instance.hpp>
#include <datasets/LabeledInstance.hpp>
#include <random>
#include <ranges>
#include <sys/types.h>
#include <tuple>
#include <type_traits>
#include <utils/Container.hpp>
#include <utils/Types.hpp>

template <typename AbstractInstance> class IDataset {
public:
  static_assert(std::is_base_of_v<Instance, AbstractInstance>,
                "AbstractInstance must be derived from Instance");

  IDataset(const DatasetContainer<AbstractInstance> &rows,
           const std::vector<Column> &columns);

  IDataset(DatasetContainer<AbstractInstance> &&rows,
           const std::vector<Column> &columns);

  [[nodiscard]] uint64_t size() const;

  [[nodiscard]] uint32_t num_dimensions() const;

  [[nodiscard]] bool size_le(uint64_t threshold) const;

  [[nodiscard]] const AbstractInstance &at(uint64_t idx) const;

  std::tuple<IDataset, IDataset> split(const Column &column,
                                       const feature_t &value) const;

  DatasetContainer<AbstractInstance> m_rows;
  const std::vector<Column> m_columns;

  feature_t min(const Column &column) const;
  feature_t max(const Column &column) const;

  IDataset<AbstractInstance> sample(uint64_t sample_size,
                                    std::mt19937_64 &engine) const;
};

using Dataset = IDataset<Instance>;
using LabeledDataset = IDataset<LabeledInstance>;
using FinalDataset = IDataset<FinalInstance>;