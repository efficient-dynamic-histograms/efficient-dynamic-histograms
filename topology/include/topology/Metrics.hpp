#pragma once

#include <datasets/Instance.hpp>

class IMetric {
public:
  virtual ~IMetric() = default;

  [[nodiscard]] virtual double distance(const Instance &p,
                                        const Instance &o) const = 0;
};

class TaxiMetric final : public IMetric {
public:
  [[nodiscard]] double distance(const Instance &p,
                                const Instance &o) const override;
};

class EuclideanMetric final : public IMetric {
public:
  [[nodiscard]] double distance(const Instance &p,
                                const Instance &o) const override;
};

class MaxMetric final : public IMetric {
public:
  [[nodiscard]] double distance(const Instance &p,
                                const Instance &o) const override;
};
