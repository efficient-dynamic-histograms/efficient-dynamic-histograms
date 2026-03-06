#include <cmath>
#include <topology/Metrics.hpp>

inline double TaxiMetric::distance(const Instance &p, const Instance &o) const {
  double result = 0;
  for (uint idx = 0; idx < p.size(); idx++) {
    result += std::abs(p[idx] - o[idx]);
  }
  return result;
}

inline double EuclideanMetric::distance(const Instance &p,
                                        const Instance &o) const {
  double result = 0;
  for (uint idx = 0; idx < p.size(); idx++) {
    result += (p[idx] - o[idx]) * (p[idx] - o[idx]);
  }
  return std::sqrt(result);
}

inline double MaxMetric::distance(const Instance &p, const Instance &o) const {
  double result = 0;
  for (uint idx = 0; idx < p.size(); idx++) {
    result = std::max(result, std::abs(p[idx] - o[idx]));
  }
  return result;
}
