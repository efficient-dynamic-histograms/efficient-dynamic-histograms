#include <datasets/FinalInstance.hpp>

FinalInstance::FinalInstance(const LabeledInstance &in,
                             const bool marked_anomaly)
    : LabeledInstance(in), m_marked_anomaly(marked_anomaly) {
}

FinalInstance::FinalInstance(LabeledInstance &&in, const bool marked_anomaly)
    : LabeledInstance(std::move(in)), m_marked_anomaly(marked_anomaly) {
}
