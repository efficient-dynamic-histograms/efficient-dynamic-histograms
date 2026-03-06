#include <utils/Math.hpp>

namespace utils {
double gauss_pdf(double mean, double var, double x) {
  return 1.0 / (sqrt(var * 2 * M_PI)) *
         exp(-((x - mean) * (x - mean)) / (2 * var));
}

double safe_gauss(double mean, double var, double x, double eps) {
  return 1.0 / (sqrt((var + eps) * 2 * M_PI)) *
         exp(-((x - mean) * (x - mean)) / (2 * (var + eps)));
}
double cdf(double x, double mean, double variance) {
  if (variance < min_var) {
    if (x < mean) {
      return 0.0;
    }
    return 1.0;
  }
  return 0.5 * (1 + erf((x - mean) / (sqrt(variance) * sqrt(2.))));
}
} // namespace utils
