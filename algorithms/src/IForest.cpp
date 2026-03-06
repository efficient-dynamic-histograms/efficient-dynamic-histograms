#include "algorithms/IForest.hpp"

ExNode::ExNode(int size) : m_size(size) {
}
InNode::InNode(std::unique_ptr<IsolationTree> left,
               std::unique_ptr<IsolationTree> right, const Column &splitAttr,
               double splitValue)
    : m_left(std::move(left)), m_right(std::move(right)),
      m_splitAttr(std::move(splitAttr)), m_splitValue(splitValue) {
}

template <typename T> 
double IsolationForest<T>::H(double n) const {
  if (n <= 0)
    return 0;
  return std::log(n) + EULER;
}

template <typename T>
std::unique_ptr<IsolationTree>
IsolationForest<T>::create(const IDataset<T> &data, int height, int limit,
                           std::mt19937_64 &engine) {
  if (height >= limit || data.size_le(1)) {
    return std::make_unique<ExNode>(data.size());
  }
  std::uniform_int_distribution<uint32_t> columnDist(0,
                                                     data.num_dimensions() - 1);
  const Column &column = data.m_columns[columnDist(engine)];
  double minValue = data.min(column);
  double maxValue = data.max(column);

  if (minValue == maxValue) {
    return std::make_unique<ExNode>(data.size());
  }

  std::uniform_real_distribution<double> splitDist(minValue, maxValue);
  double split = splitDist(engine);

  auto [left, right] = data.split(column, split);
  return std::make_unique<InNode>(create(left, height + 1, limit, engine),
                                  create(right, height + 1, limit, engine),
                                  column, split);
}
template <typename T> double IsolationForest<T>::expected(double n) const {
  if (n <= 0)
    return 0;
  return 2 * H(n - 1) - (2 * (n - 1) / n);
}
template <typename T>
double IsolationForest<T>::anomalyScore(const Instance &x) const {
  double sum = 0;
  for (const auto &tree : m_trees) {
    sum += pathLength(x, tree.get(), 0);
  }
  double avg = sum / m_number_of_trees;
  double exponent = -avg / expected(m_dataset_size);
  return std::pow(2.0, exponent);
}

template <typename T>
double IsolationForest<T>::score(const Instance &x) const {
  return anomalyScore(x);
}
template <typename T>
double IsolationForest<T>::pathLength(const Instance &x,
                                      const IsolationTree *tree,
                                      double current) const {
  if (const auto *ex = dynamic_cast<const ExNode *>(tree)) {
    return current + expected(ex->m_size);
  } else if (const auto *in = dynamic_cast<const InNode *>(tree)) {
    double v = x.at(in->m_splitAttr.m_index);
    if (v < in->m_splitValue) {
      return pathLength(x, in->m_left.get(), current + 1);
    } else {
      return pathLength(x, in->m_right.get(), current + 1);
    }
  }
  return current;
}
template <typename T>
IsolationForest<T>::IsolationForest(const IDataset<T> &data,
                                    uint32_t number_of_trees,
                                    uint32_t sample_size,
                                    std::mt19937_64 &engine)
    : m_dataset_size(data.size()), m_number_of_trees(number_of_trees),
      m_sample_size(sample_size) {
  m_l = uint16_t(std::ceil(std::log(sample_size)));

  for (uint32_t i = 0; i < number_of_trees; ++i) {
    IDataset<T> subset = data.sample(sample_size, engine);
    m_trees.emplace_back(create(subset, 0, m_l, engine));
  }
}

template class IsolationForest<Instance>;
template class IsolationForest<LabeledInstance>;
template class IsolationForest<FinalInstance>;