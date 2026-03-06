#pragma once

#include <datasets/Instance.hpp>

class LabeledInstance : public Instance {
public:
  LabeledInstance(const std::vector<feature_t> &features, bool is_anomaly);

  LabeledInstance(std::vector<feature_t> &&features, bool is_anomaly);

  bool m_is_anomaly;
};
