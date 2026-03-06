#include <sketches/BenHaim.hpp>

double VarianceBin::variance() const {
  return count == 1 ? 0 : m2 / (count - 1);
}

bool CompareVarianceBin::operator()(const VarianceBin &a,
                                    const VarianceBin &b) const {
  return a.x < b.x;
}

KDEBenHaim::KDEBenHaim(uint32_t number_of_points) : IBenHaim(number_of_points) {
}

KDEBenHaim::KDEBenHaim(uint32_t number_of_points, double stds)
    : IBenHaim(number_of_points), m_stds(stds) {
}

void KDEBenHaim::merge_pair() {
  std::multiset<VarianceBin, CompareVarianceBin>::iterator end =
      prev(m_points.end());
  double min_dist = INFINITY;
  std::multiset<VarianceBin, CompareVarianceBin>::iterator left =
      m_points.begin();
  for (std::multiset<VarianceBin, CompareVarianceBin>::iterator it =
           m_points.begin();
       it != end; it++) {
    if (double dist = std::next(it)->x - it->x; dist < min_dist) {
      left = it;
      min_dist = dist;
    }
  }
  std::multiset<VarianceBin, CompareVarianceBin>::iterator right = next(left);
  u_int64_t new_count = left->count + right->count;
  double new_x =
      (left->x * left->count + right->x * right->count) / (new_count);
  double sigma = (right->x - left->x) * (right->x - left->x);
  double new_m2 =
      left->m2 + right->m2 +
      sigma * (double(right->count) * double(left->count) / double(new_count));
  m_points.erase(left, std::next(right));
  m_points.insert({new_x, new_count, new_m2});
}

void KDEBenHaim::single_addition(double x) {
  std::multiset<VarianceBin>::iterator candidate =
      m_points.lower_bound({x, 1, 1});
  if (candidate != m_points.end() && candidate->x == x) {
    uint64_t count = candidate->count;
    double m2 = candidate->m2;
    m_points.erase(candidate);
    m_points.insert({x, count + 1, m2});
  } else {
    m_points.insert({x, 1, 0});
  }
}

AllKDEBenHaim::AllKDEBenHaim(uint32_t number_of_points, double stds)
    : KDEBenHaim(number_of_points, stds) {
}

AllKDEBenHaim::AllKDEBenHaim(uint32_t number_of_points)
    : KDEBenHaim(number_of_points) {
}

double KDEBenHaim::get_min_var() const {
  std::vector<Bin> vars;
  vars.reserve(m_points.size());
  for (std::multiset<VarianceBin, CompareVarianceBin>::iterator iter =
           m_points.begin();
       iter != m_points.end(); iter++) {
    if (double variance = iter->variance();
        variance * m_stds >= utils::min_var) {
      vars.push_back({variance, iter->count});
    }
  }
  if (vars.empty()) {
    return NAN;
  }
  uint64_t mx_count = m_number_of_points * 0.2;
  std::sort(vars.begin(), vars.end(), CompareBin());
  uint64_t cumulative = 0;
  std::vector<Bin>::iterator it = std::find_if(
      vars.begin(), vars.end(), [mx_count, &cumulative](const Bin &bin) {
        cumulative += bin.count;
        return cumulative >= mx_count;
      });
  return it->x;
}

double AllKDEBenHaim::getEstimate(double x) const {
  if (m_points.empty()) {
    return NAN;
  }
  double pdf = 0.0;
  double min_var = get_min_var();
  if (std::isnan(min_var)) {
    return NAN; // TODO validate
  }
  for (std::multiset<VarianceBin, CompareVarianceBin>::iterator iter =
           m_points.begin();
       iter != m_points.end(); iter++) {
    pdf += get_single_pdf(iter, x, min_var);
  }
  return pdf / total_points;
}

LimitedKDEBenHaim::LimitedKDEBenHaim(uint32_t number_of_points, double stds)
    : KDEBenHaim(number_of_points, stds) {
}

LimitedKDEBenHaim::LimitedKDEBenHaim(uint32_t number_of_points)
    : KDEBenHaim(number_of_points) {
}

[[deprecated]]
double KDEBenHaim::get_single_pdf(std::multiset<VarianceBin>::iterator bin,
                                  double x) const {
  double variance = bin->variance() * m_stds;
  if (variance < utils::min_var) {
    double left = (bin == m_points.begin()) ? m_min_point : std::prev(bin)->x;
    std::multiset<VarianceBin>::iterator right_bin = std::next(bin);
    double right = (right_bin == m_points.end()) ? m_max_point : right_bin->x;
    double distance = right - left;
    if (distance < utils::min_var) {
      return bin->count;
    }
    return utils::safe_gauss(bin->x, variance, x,
                             distance * utils::safe_multiplier) *
           bin->count;
  }
  return utils::gauss_pdf(bin->x, variance, x) * bin->count;
}

double KDEBenHaim::get_single_pdf(std::multiset<VarianceBin>::iterator bin,
                                  double x, double min_var) const {
  double variance = bin->variance();
  if (variance * m_stds < utils::min_var) {
    variance = min_var;
  }
  return utils::gauss_pdf(bin->x, variance * m_stds, x) * bin->count;
}

double LimitedKDEBenHaim::getEstimate(double x) const {
  if (m_points.empty()) {
    return NAN;
  }
  double min_var = get_min_var();
  if (std::isnan(min_var)) {
    return NAN; // TODO validate
  }
  double denominator = get_denominator();
  std::multiset<VarianceBin>::iterator right = m_points.lower_bound({x, 1, 1});
  if (right == m_points.begin()) {
    return get_single_pdf(right, x, min_var) / denominator;
  }
  if (right == m_points.end()) {
    return get_single_pdf(std::prev(right), x, min_var) / denominator;
  }
  std::multiset<VarianceBin>::iterator left = prev(right);
  return (get_single_pdf(left, x, min_var) +
          get_single_pdf(right, x, min_var)) /
         denominator;
}

inline double LimitedKDEBenHaim::get_denominator() const {
  if (m_points.size() < 2) {
    return 1.0;
  }
  double total = 0.0;
  std::multiset<VarianceBin>::iterator left = m_points.begin();
  std::multiset<VarianceBin>::iterator iter = std::next(left);
  std::multiset<VarianceBin>::iterator right = std::next(iter);

  total +=
      utils::cdf(iter->x, left->x, left->variance() * m_stds) * left->count;

  while (right != m_points.end()) {
    total +=
        utils::cdf(right->x, iter->x, iter->variance() * m_stds) * iter->count;
    total -=
        utils::cdf(left->x, iter->x, iter->variance() * m_stds) * iter->count;

    left++;
    iter++;
    right++;
  }

  // right==end()
  // iter==std::prev(end) - last bucket
  total += 1;
  total -=
      utils::cdf(left->x, iter->x, iter->variance() * m_stds) * iter->count;
  return total;
}