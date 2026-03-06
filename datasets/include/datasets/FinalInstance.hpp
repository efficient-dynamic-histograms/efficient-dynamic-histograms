#pragma once

#include <datasets/LabeledInstance.hpp>

class FinalInstance : public LabeledInstance {
public:
  FinalInstance(const LabeledInstance &in, bool marked_anomaly);

  FinalInstance(LabeledInstance &&in, bool marked_anomaly);

  const bool m_marked_anomaly;
};
