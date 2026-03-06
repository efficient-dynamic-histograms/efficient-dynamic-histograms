#include <datasets/LabeledInstance.hpp>

LabeledInstance::LabeledInstance(const std::vector<feature_t> &features,
                                 const bool is_anomaly)
    : Instance(features), m_is_anomaly(is_anomaly) {
}

LabeledInstance::LabeledInstance(std::vector<feature_t> &&features,
                                 const bool is_anomaly)
    : Instance(std::move(features)), m_is_anomaly(is_anomaly) {
}
