#pragma once

#include <datasets/Column.hpp>
#include <datasets/Dataset.hpp>
#include <memory>
#include <random>
#include <sketches/Density.hpp>
#include <utility>

constexpr double EULER = 0.5772156649;

class IsolationTree {
public:
  virtual ~IsolationTree() = default;
};

class ExNode : public IsolationTree {
public:
  int m_size;
  ExNode(int size);
};

class InNode : public IsolationTree {
public:
  std::unique_ptr<IsolationTree> m_left;
  std::unique_ptr<IsolationTree> m_right;
  Column m_splitAttr;
  double m_splitValue;

  InNode(std::unique_ptr<IsolationTree> left,
         std::unique_ptr<IsolationTree> right, const Column &splitAttr,
         double splitValue);
};

template <typename T> class IsolationForest {
public:
  IsolationForest(const IDataset<T> &data, uint32_t numberOfTrees,
                  uint32_t sampleSize, std::mt19937_64 &engine);

  double score(const Instance &x) const;

protected:
  double pathLength(const Instance &x, const IsolationTree *tree,
                    double current) const;
  double anomalyScore(const Instance &x) const;
  double expected(double n) const;
  double H(double n) const;

  uint64_t m_dataset_size;
  uint32_t m_number_of_trees;
  uint32_t m_sample_size;
  uint16_t m_l;
  std::vector<std::unique_ptr<IsolationTree>> m_trees;
  static std::unique_ptr<IsolationTree> create(const IDataset<T> &data,
                                               int height, int limit,
                                               std::mt19937_64 &engine);
};