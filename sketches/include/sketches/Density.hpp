#pragma once

#include <vector>

class IDensity {
public:
  virtual double getEstimate(double x) const = 0;
  virtual void addPoint(double x) = 0;
  virtual void addPoints(std::vector<double> points) = 0;
};