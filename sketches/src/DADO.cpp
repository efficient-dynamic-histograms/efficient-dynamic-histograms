#include <sketches/DADO.hpp>

double DADOBin::count() const {
  return left_cnt + right_cnt;
}

DADO::DADO(uint32_t number_of_bins) : m_number_of_bins(number_of_bins) {
}

void DADO::addPoint(double x) {
  total_points++;
  if (m_bins.size() < m_number_of_bins) {
    m_bins[x].left_cnt += 0.5;
    m_bins[x].right_cnt += 0.5;
    if (x > m_max_point) {
      m_max_point = x;
    }
  } else {
    if (x < m_bins.begin()->first || x > m_max_point) {
      if (x > m_max_point) {
        m_max_point = x;
      }
      m_bins[x] = DADOBin(0.5, 0.5);
      std::pair<std::map<double, DADOBin>::const_iterator, double> b_pair =
          find_bucket_to_merge();
      merge(b_pair.first, std::next(b_pair.first));
    } else {
      std::map<double, DADOBin>::iterator right = m_bins.upper_bound(x);
      // right is not m_bins.begin()
      std::map<double, DADOBin>::iterator left = std::prev(right);
      // this is the bucket we fall into
      double left_x = left->first;
      double right_x = (right == m_bins.end()) ? m_max_point : right->first;
      if (left_x == right_x) {
        left->second.left_cnt += 0.5;
        left->second.right_cnt += 0.5;
      } else if (x <= (right_x + left_x) / 2) {
        left->second.left_cnt += 1;
      } else {
        left->second.right_cnt += 1;
      }
      std::pair<std::map<double, DADOBin>::const_iterator, double> s_pair =
          find_bucket_to_split();
      std::pair<std::map<double, DADOBin>::const_iterator, double> m_pair =
          find_bucket_to_merge();

      if (m_pair.second - s_pair.second < 0) {
        split(s_pair.first);
        merge(m_pair.first, std::next(m_pair.first));
      }
    }
  }
}

std::pair<std::map<double, DADOBin>::const_iterator, double>
DADO::find_bucket_to_split() const {
  double max_e = -INFINITY;
  std::map<double, DADOBin>::const_iterator best;
  for (std::map<double, DADOBin>::const_iterator left = m_bins.begin();
       left != m_bins.end(); left++) {
    double e = std::abs((left->second.left_cnt) - (left->second.right_cnt));
    if (e > max_e) {
      max_e = e;
      best = left;
    }
  }
  return {best, max_e};
}

std::pair<std::map<double, DADOBin>::const_iterator, double>
DADO::find_bucket_to_merge() const {
  double min_e = INFINITY;
  std::map<double, DADOBin>::const_iterator last = std::prev(m_bins.end());
  std::map<double, DADOBin>::const_iterator best_left;
  for (std::map<double, DADOBin>::const_iterator left = m_bins.begin();
       left != last; left++) {
    std::map<double, DADOBin>::const_iterator right = std::next(left);

    double avg = (left->second.count() + right->second.count());
    auto counts = get_merged_counts(left, right);
    double e = std::abs(counts.first - avg) + std::abs(counts.second - avg);

    // double e1 = std::abs(left->second.left_cnt - left->second.right_cnt);
    // double e2 = std::abs(right->second.left_cnt - right->second.right_cnt);
    // e -= e1;
    // e -= e2;
    if (e < min_e) {
      min_e = e;
      best_left = left;
    }
  }
  return {best_left, min_e};
}

void DADO::addPoints(std::vector<double> points) {
  for (double &x : points) {
    addPoint(x);
  }
}

void DADO::split(std::map<double, DADOBin>::const_iterator s) {
  std::map<double, DADOBin>::const_iterator right = std::next(s);
  double right_x = (right == m_bins.end()) ? m_max_point : right->first;
  double left_x = s->first;
  double mid_x = (left_x + right_x) / 2;
  double right_cnt = s->second.right_cnt;
  m_bins[left_x].left_cnt = m_bins[left_x].left_cnt / 2;
  m_bins[left_x].right_cnt = m_bins[left_x].left_cnt;
  m_bins[mid_x].left_cnt = right_cnt / 2;
  m_bins[mid_x].right_cnt = right_cnt / 2;
}

double DADO::getEstimate(const double x) const {
  if (x > m_max_point || x < m_bins.begin()->first) {
    return 0.0;
  }
  if (x == m_max_point) {
    return m_bins.rbegin()->second.count() / total_points;
  }
  const std::map<double, DADOBin>::const_iterator right = m_bins.upper_bound(x);
  const std::map<double, DADOBin>::const_iterator left = std::prev(right);
  double right_x = (right == m_bins.end()) ? m_max_point : right->first;
  if (x <= (right_x + left->first) / 2) {
    return left->second.left_cnt / (total_points * (right_x - left->first) / 2);
  } else {
    return left->second.right_cnt /
           (total_points * (right_x - left->first) / 2);
  }
}

void DADO::merge(std::map<double, DADOBin>::const_iterator bin1,
                 std::map<double, DADOBin>::const_iterator bin2) {
  auto counts = get_merged_counts(bin1, bin2);
  m_bins.erase(bin2);
  m_bins[bin1->first].left_cnt = counts.first;
  m_bins[bin1->first].right_cnt = counts.second;
}

std::pair<double, double>
DADO::get_merged_counts(std::map<double, DADOBin>::const_iterator bin1,
                        std::map<double, DADOBin>::const_iterator bin2) const {
  std::map<double, DADOBin>::const_iterator right = std::next(bin2);
  double e = (right == m_bins.end()) ? m_max_point : right->first;
  double a = bin1->first;
  double c = bin2->first;
  double b = (a + c) / 2;
  double d = (c + e) / 2;
  double c2 = (a + e) / 2;
  double left_cnt = 0;
  left_cnt += compare_and_add(a, b, bin1->second.left_cnt, c2);
  left_cnt += compare_and_add(b, c, bin1->second.right_cnt, c2);
  left_cnt += compare_and_add(c, d, bin2->second.left_cnt, c2);
  left_cnt += compare_and_add(d, e, bin2->second.right_cnt, c2);
  double right_cnt = bin1->second.left_cnt + bin1->second.right_cnt +
                     bin2->second.left_cnt + bin2->second.right_cnt - left_cnt;
  return {left_cnt, right_cnt};
}

double DADO::compare_and_add(double start, double end, double count,
                             double c2) const {
  if (start > c2) {
    return 0.0;
  }
  if (end <= c2) {
    return count;
  }
  return count * (c2 - start) / (end - start);
}

std::map<double, DADOBin> DADO::getHistogram() const {
  return m_bins;
}

DADO_Corrected::DADO_Corrected(uint32_t number_of_bins) : DADO(number_of_bins) {
}

std::pair<std::map<double, DADOBin>::const_iterator, double>
DADO_Corrected::find_bucket_to_split() const {
  return DADO::find_bucket_to_split();
}

std::pair<std::map<double, DADOBin>::const_iterator, double>
DADO_Corrected::find_bucket_to_merge() const {
  double min_e = INFINITY;
  std::map<double, DADOBin>::const_iterator last = std::prev(m_bins.end());
  std::map<double, DADOBin>::const_iterator best_left;
  for (std::map<double, DADOBin>::const_iterator left = m_bins.begin();
       left != last; left++) {
    std::map<double, DADOBin>::const_iterator right = std::next(left);

    double avg = (left->second.count() + right->second.count());
    auto counts = get_merged_counts(left, right);
    double e = std::abs(counts.first - avg) + std::abs(counts.second - avg);

    double e1 = std::abs(left->second.left_cnt - left->second.right_cnt);
    double e2 = std::abs(right->second.left_cnt - right->second.right_cnt);
    e -= e1;
    e -= e2;
    if (e < min_e) {
      min_e = e;
      best_left = left;
    }
  }
  return {best_left, min_e};
}