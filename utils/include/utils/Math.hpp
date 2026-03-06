#pragma once

#include <cmath>

namespace utils {
double gauss_pdf(double mean, double var, double x);
double safe_gauss(double mean, double var, double x, double eps);
constexpr double min_var = 10e-8;
constexpr double safe_multiplier = 10e-3;
double cdf(double x, double mean, double variance);
}