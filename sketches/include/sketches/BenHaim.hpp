#pragma once

#include <algorithm>
#include <cmath>
#include <iterator>
#include <set>
#include <sketches/Density.hpp>
#include <utils/Math.hpp>
#include <utils/Types.hpp>
#include <vector>

struct Bin {
  double x;
  uint64_t count;
};

struct VarianceBin {
  double x;
  uint64_t count;
  double m2;
  double variance() const;
};

struct CompareBin {
  bool operator()(const Bin &a, const Bin &b) const;
};

struct CompareVarianceBin {
  bool operator()(const VarianceBin &a, const VarianceBin &b) const;
};

template <typename AbstractBin, typename AbstractBinCompare>
class IBenHaim : public IDensity {
public:
  IBenHaim(uint32_t number_of_points);
  virtual double getEstimate(double x) const override = 0;
  void addPoint(double x);
  void addPoints(std::vector<double> points);
  void addHistogram(const IBenHaim &other);
  std::multiset<AbstractBin, AbstractBinCompare> getHistogram() const;

protected:
  uint32_t m_number_of_points;
  uint64_t total_points = 0;
  double m_min_point = INFINITY;
  double m_max_point = -INFINITY;
  std::multiset<AbstractBin, AbstractBinCompare> m_points;
  virtual void merge_pair() = 0;
  virtual void single_addition(double x) = 0;
};

class BenHaim : public IBenHaim<Bin, CompareBin> {
public:
  BenHaim(uint32_t number_of_points);
  double getEstimate(double x) const override;
  double getUnnormalizedEstimate(double x) const; // It should be protected

protected:
  virtual double pdf_formula(uint64_t m1, uint64_t m2, double p1, double p2,
                             double b) const = 0;
  void merge_pair() override;
  void single_addition(double x) override;
};

class SimpleBenHaim : public BenHaim {
public:
  SimpleBenHaim(uint32_t number_of_points);

protected:
  double pdf_formula(uint64_t m1, uint64_t m2, double p1, double p2,
                     double b) const override;
};

class AdvancedBenHaim : public BenHaim {
public:
  AdvancedBenHaim(uint32_t number_of_points);

protected:
  double pdf_formula(uint64_t m1, uint64_t m2, double p1, double p2,
                     double b) const override;
};

class IncorrectBenHaim : public BenHaim {
public:
  IncorrectBenHaim(uint32_t number_of_points);

protected:
  double pdf_formula(uint64_t m1, uint64_t m2, double p1, double p2,
                     double b) const override;
};

class KDEBenHaim : public IBenHaim<VarianceBin, CompareVarianceBin> {
public:
  KDEBenHaim(uint32_t number_of_points, double stds);
  KDEBenHaim(uint32_t number_of_points);
  virtual double getEstimate(double x) const override = 0;

protected:
  double m_stds = 3.0;
  void merge_pair() override;
  void single_addition(double x) override;
  [[deprecated]]
  double get_single_pdf(std::multiset<VarianceBin>::iterator bin,
                        double x) const;
  double get_single_pdf(std::multiset<VarianceBin>::iterator bin, double x,
                        double min_var) const;
  double get_min_var() const;
};

class AllKDEBenHaim : public KDEBenHaim {
public:
  AllKDEBenHaim(uint32_t number_of_points, double stds);
  AllKDEBenHaim(uint32_t number_of_points);
  double getEstimate(double x) const override;
};

class LimitedKDEBenHaim : public KDEBenHaim {
public:
  LimitedKDEBenHaim(uint32_t number_of_points, double stds);
  LimitedKDEBenHaim(uint32_t number_of_points);
  double getEstimate(double x) const override;

protected:
  inline double get_denominator() const;
};

template class IBenHaim<Bin, CompareBin>;
template class IBenHaim<VarianceBin, CompareVarianceBin>;
