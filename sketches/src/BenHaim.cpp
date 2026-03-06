#include <sketches/BenHaim.hpp>

bool CompareBin::operator()(const Bin &a, const Bin &b) const {
  return a.x < b.x;
}

inline double pdf_formula_advanced(uint64_t m1, uint64_t m2, double p1,
                                   double p2, double b) {
  return (b * (double(m2) - double(m1)) - m2 * p1 + p2 * m1) /
         ((p2 - p1) * (p2 - p1));
}

inline double pdf_formula_simple(uint64_t m1, uint64_t m2, double p1, double p2,
                                 double) {
  return (m1 + m2) / (2 * (p2 - p1));
}

inline double pdf_formula_incorrect(uint64_t m1, uint64_t m2, double p1,
                                    double p2, double) {
  return (m1 * p1 + m2 * p2) / (2 * (p2 - p1));
}

template <typename AbstractBin, typename AbstractBinCompare>
IBenHaim<AbstractBin, AbstractBinCompare>::IBenHaim(uint32_t number_of_points)
    : m_number_of_points(number_of_points) {
}

template <typename AbstractBin, typename AbstractBinCompare>
void IBenHaim<AbstractBin, AbstractBinCompare>::addPoint(double x) {
  single_addition(x);
  total_points++;

  if (m_points.size() > m_number_of_points) {
    merge_pair();
  }

  if (x < m_min_point) {
    m_min_point = x;
  }
  if (x > m_max_point) {
    m_max_point = x;
  }
}
template <typename AbstractBin, typename AbstractBinCompare>
void IBenHaim<AbstractBin, AbstractBinCompare>::addPoints(
    std::vector<double> points) {
  if (points.empty()) {
    return;
  }
  for (double &x : points) {
    single_addition(x);
  }
  total_points += points.size();
  if (std::vector<double>::iterator x =
          std::max_element(points.begin(), points.end());
      *x > m_max_point) {
    m_max_point = *x;
  }
  if (std::vector<double>::iterator x =
          std::min_element(points.begin(), points.end());
      *x < m_min_point) {
    m_min_point = *x;
  }
  while (m_points.size() > m_number_of_points) {
    merge_pair();
  }
}

template <typename AbstractBin, typename AbstractBinCompare>
void IBenHaim<AbstractBin, AbstractBinCompare>::addHistogram(
    const IBenHaim &other) {
  m_points.insert(other.m_points.begin(), other.m_points.end());

  if (other.m_max_point > m_max_point) {
    m_max_point = other.m_max_point;
  }
  if (other.m_min_point < m_min_point) {
    m_min_point = other.m_min_point;
  }
  while (m_points.size() > m_number_of_points) {
    merge_pair();
  }
}

template <typename AbstractBin, typename AbstractBinCompare>
std::multiset<AbstractBin, AbstractBinCompare>
IBenHaim<AbstractBin, AbstractBinCompare>::getHistogram() const {
  return m_points;
}

double BenHaim::getEstimate(double x) const {
  return getUnnormalizedEstimate(x) / total_points;
}

double BenHaim::getUnnormalizedEstimate(double x) const {
  if (m_points.empty()) {
    return NAN;
  }

  std::multiset<Bin>::iterator right = m_points.lower_bound({x, 1});
  if (right == m_points.begin()) {
    if (x < m_min_point) {
      return 0.0;
    }
    if (x == right->x) {
      // exactly cords of first bin
      std::multiset<Bin>::iterator rright = std::next(right);
      if (rright == m_points.end()) {
        return right->count; // only one bin exists;
      }
      return pdf_formula(right->count, rright->count, right->x, rright->x, x);
    }
    return pdf_formula(0, right->count, m_min_point, right->x, x);
  }
  // now we know that there exist more bins than one
  if (right == m_points.end()) {
    if (x > m_max_point) {
      return 0.0;
    }
    std::multiset<Bin>::reverse_iterator last = m_points.rend();
    if (x == last->x) {
      // exactly last bin
      std::multiset<Bin>::reverse_iterator llast = std::next(last);
      if (llast == m_points.rend()) {
        return last->count;
      }
      return pdf_formula(llast->count, last->count, llast->x, last->x, x);
    }
    return pdf_formula(last->count, 0, last->x, m_max_point, x);
  }
  std::multiset<Bin>::iterator left = std::prev(right);
  return pdf_formula(left->count, right->count, left->x, right->x, x);
}

BenHaim::BenHaim(uint32_t number_of_points) : IBenHaim(number_of_points) {
}

void BenHaim::single_addition(double x) {
  std::multiset<Bin>::iterator candidate = m_points.lower_bound({x, 1});
  if (candidate != m_points.end() && candidate->x == x) {
    uint64_t count = candidate->count;
    m_points.erase(candidate);
    m_points.insert({x, count + 1});
  } else {
    m_points.insert({x, 1});
  }
}

void BenHaim::merge_pair() {
  std::multiset<Bin, CompareBin>::iterator end = prev(m_points.end());
  double min_dist = INFINITY;
  std::multiset<Bin, CompareBin>::iterator left = m_points.begin();
  for (std::multiset<Bin, CompareBin>::iterator it = m_points.begin();
       it != end; it++) {
    if (double dist = std::next(it)->x - it->x; dist < min_dist) {
      left = it;
      min_dist = dist;
    }
  }
  std::multiset<Bin, CompareBin>::iterator right = next(left);
  u_int64_t new_count = left->count + right->count;
  double new_x =
      (left->x * left->count + right->x * right->count) / (new_count);
  m_points.erase(left, std::next(right));
  m_points.insert({new_x, new_count});
}

SimpleBenHaim::SimpleBenHaim(uint32_t number_of_points)
    : BenHaim(number_of_points) {
}

AdvancedBenHaim::AdvancedBenHaim(uint32_t number_of_points)
    : BenHaim(number_of_points) {
}

IncorrectBenHaim::IncorrectBenHaim(uint32_t number_of_points)
    : BenHaim(number_of_points) {
}

inline double SimpleBenHaim::pdf_formula(uint64_t m1, uint64_t m2, double p1,
                                         double p2, double b) const {
  return pdf_formula_simple(m1, m2, p1, p2, b);
}

inline double AdvancedBenHaim::pdf_formula(uint64_t m1, uint64_t m2, double p1,
                                           double p2, double b) const {
  return pdf_formula_advanced(m1, m2, p1, p2, b);
}

inline double IncorrectBenHaim::pdf_formula(uint64_t m1, uint64_t m2, double p1,
                                            double p2, double b) const {
  return pdf_formula_incorrect(m1, m2, p1, p2, b);
}