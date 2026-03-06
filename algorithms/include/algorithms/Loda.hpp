#pragma once

#include <array>
#include <datasets/Instance.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <random>
#include <sketches/Density.hpp>
#include <utils/AlignedAllocator.hpp>
#include <utils/Sample.hpp>
#include <vector>

class ProjectionManager {
public:
  ProjectionManager(uint32_t num_dimensions, uint16_t num_sketches,
                    std::optional<uint64_t> seed = std::nullopt);
  void create_projections();
  double get_projection(const Instance &instance, uint32_t idx) const;

private:
  std::vector<double, AlignedAllocator<double, 64>> m_data;
  uint32_t m_num_dimensions;
  uint16_t m_num_sketches;
  std::mt19937_64 engine;
  std::vector<uint32_t> sample_without_replacement(uint32_t max_size);
};

class ILoda {
public:
  ILoda(uint32_t num_dimensions,
        std::vector<std::unique_ptr<IDensity>> &&sketches,
        std::optional<uint64_t> seed = std::nullopt);
  virtual void add(const Instance &instance) = 0;
  double HBOS(const Instance &instance) const;
  double add_estimate(const Instance &instance);

protected:
  std::vector<std::unique_ptr<IDensity>> m_estimators;
  uint16_t m_num_sketches;
  ProjectionManager m_projections;
};

class InstantLoda : public ILoda {
public:
  InstantLoda(uint32_t num_dimensions,
              std::vector<std::unique_ptr<IDensity>> &&sketches,
              std::optional<uint64_t> seed = std::nullopt);
  void add(const Instance &instance) override;
};

class BufferedLoda : public ILoda {
public:
  BufferedLoda(uint32_t num_dimensions,
               std::vector<std::unique_ptr<IDensity>> &&sketches,
               uint32_t buffer_size,
               std::optional<uint64_t> seed = std::nullopt);
  void add(const Instance &instance) override;

protected:
  std::vector<std::reference_wrapper<const Instance>> buffer;
  uint32_t m_buffer_size;
};