#include <algorithms/Loda.hpp>

double ILoda::HBOS(const Instance &instance) const {
  double acc = 0;
  for (uint16_t i = 0; i < m_num_sketches; i++) {
    double z = m_projections.get_projection(instance, i);
    double p = m_estimators[i]->getEstimate(z);
    p = p < 0.0 ? 0.0 : p; // forIncorrectBenHaim
    acc += log(p);
  }
  return -acc / m_num_sketches;
}

double ILoda::add_estimate(const Instance &instance) {
  add(instance);
  return HBOS(instance);
}

ILoda::ILoda(uint32_t num_dimensions,
             std::vector<std::unique_ptr<IDensity>> &&sketches,
             std::optional<uint64_t> seed)
    : m_estimators(std::move(sketches)), m_num_sketches(m_estimators.size()),
      m_projections(num_dimensions, m_estimators.size(), seed) {
  m_projections.create_projections();
}

InstantLoda::InstantLoda(uint32_t num_dimensions,
                         std::vector<std::unique_ptr<IDensity>> &&sketches,
                         std::optional<uint64_t> seed)
    : ILoda(num_dimensions, std::move(sketches), seed) {
}

void InstantLoda::add(const Instance &instance) {
  for (uint16_t i = 0; i < m_num_sketches; i++) {
    double z = m_projections.get_projection(instance, i);
    m_estimators[i]->addPoint(z);
  }
}

BufferedLoda::BufferedLoda(uint32_t num_dimensions,
                           std::vector<std::unique_ptr<IDensity>> &&sketches,
                           uint32_t buffer_size, std::optional<uint64_t> seed)
    : ILoda(num_dimensions, std::move(sketches), seed),
      m_buffer_size(buffer_size) {
  buffer.reserve(buffer_size + 1);
}

void BufferedLoda::add(const Instance &instance) {
  buffer.push_back(instance);

  if (buffer.size() >= m_buffer_size) {
    for (const auto &elem : buffer) {
      for (uint16_t i = 0; i < m_num_sketches; i++) {
        double z = m_projections.get_projection(elem, i);
        m_estimators[i]->addPoint(z);
      }
    }
    buffer.clear();
  }
}

ProjectionManager::ProjectionManager(uint32_t num_dimensions,
                                     uint16_t num_sketches,
                                     std::optional<uint64_t> seed)
    : m_data(AlignedAllocator<double, 64>()), m_num_dimensions(num_dimensions),
      m_num_sketches(num_sketches) {
  if (seed.has_value()) {
    engine.seed(seed.value());
  } else {
    std::random_device rd;
    std::array<std::uint32_t, 2> seed_data = {rd(), rd()};
    std::seed_seq seed_seq(seed_data.begin(), seed_data.end());
    engine.seed(seed_seq);
  }
  m_data.resize(num_dimensions * num_sketches);
}

void ProjectionManager::create_projections() {
  uint32_t non_zeros = ceil(sqrt(m_num_dimensions));
  std::normal_distribution dist{0.0, 1.0};
  for (uint16_t idx = 0; idx < m_num_sketches; idx++) {
    uint32_t offset = idx * m_num_dimensions;
    for (uint32_t &dim_idx : sample_without_replacement(non_zeros)) {
      double d = dist(engine);
      m_data[offset + dim_idx] = d;
    }
  }
}

double ProjectionManager::get_projection(const Instance &instance,
                                         uint32_t idx) const {
  double sum = 0.0;
  uint32_t offset = idx * m_num_dimensions;
#pragma omp simd reduction(+ : sum)
  for (uint32_t i = 0; i < m_num_dimensions; ++i) {
    sum += m_data[offset + i] * instance[i];
  }
  return sum;
}

std::vector<uint32_t>
ProjectionManager::sample_without_replacement(uint32_t max_size) {
  return Sample::draw_sample(m_num_dimensions, max_size, engine);
}