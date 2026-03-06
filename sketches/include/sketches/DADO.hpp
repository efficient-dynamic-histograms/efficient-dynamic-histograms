#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <sketches/Density.hpp>
#include <utils/Types.hpp>
#include <vector>

struct DADOBin {
  double left_cnt = 0;
  double right_cnt = 0;
  double count() const;
  constexpr bool operator==(const DADOBin &) const = default;
};

class DADO : public IDensity {
public:
  DADO(uint32_t number_of_bins);
  void addPoint(double x);
  void addPoints(std::vector<double> points);
  double getEstimate(double x) const override;
  std::map<double, DADOBin> getHistogram() const;

protected:
  uint32_t m_number_of_bins;
  uint64_t total_points = 0;
  double m_max_point = -INFINITY;
  std::map<double, DADOBin> m_bins;
  virtual std::pair<std::map<double, DADOBin>::const_iterator, double>
  find_bucket_to_split() const;
  virtual std::pair<std::map<double, DADOBin>::const_iterator, double>
  find_bucket_to_merge() const;
  void merge(std::map<double, DADOBin>::const_iterator b1,
             std::map<double, DADOBin>::const_iterator b2);
  void split(std::map<double, DADOBin>::const_iterator s);
  double compare_and_add(double start, double end, double count,
                         double c2) const;
  std::pair<double, double>
  get_merged_counts(std::map<double, DADOBin>::const_iterator b1,
                    std::map<double, DADOBin>::const_iterator b2) const;
};

class DADO_Corrected : public DADO {
public:
  DADO_Corrected(uint32_t number_of_bins);

protected:
  std::pair<std::map<double, DADOBin>::const_iterator, double>
  find_bucket_to_split() const override;
  std::pair<std::map<double, DADOBin>::const_iterator, double>
  find_bucket_to_merge() const override;
};