#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <sketches/DADO.hpp>

class DADOTest : public DADO {
public:
  using DADO::DADO;
  uint32_t getNumberOfBins() const {
    return m_number_of_bins;
  }
  uint64_t getTotalPoints() const {
    return total_points;
  }
  double getMaxPoint() const {
    return m_max_point;
  }
  void setNumberOfBins(uint32_t bins) {
    m_number_of_bins = bins;
  }
  void setTotalPoints(uint64_t points) {
    total_points = points;
  }
  void setMaxPoint(double max_point) {
    m_max_point = max_point;
  }
  const std::map<double, DADOBin> &getBins() const {
    return m_bins;
  }
  std::map<double, DADOBin> &accessBins() {
    return m_bins;
  }
  std::pair<std::map<double, DADOBin>::const_iterator, double>
  callFindBucketToSplit() {
    return find_bucket_to_split();
  }
  std::pair<std::map<double, DADOBin>::const_iterator, double>
  callFindBucketToMerge() {
    return find_bucket_to_merge();
  }
  void callMerge(std::map<double, DADOBin>::const_iterator b1,
                 std::map<double, DADOBin>::const_iterator b2) {
    merge(b1, b2);
  }
  void callSplit(std::map<double, DADOBin>::const_iterator it) {
    split(it);
  }
  double callCompareAndAdd(double start, double end, double count, double c2) {
    return compare_and_add(start, end, count, c2);
  }
  std::pair<double, double>
  callGetMergedCounts(std::map<double, DADOBin>::iterator b1,
                      std::map<double, DADOBin>::iterator b2) {
    return get_merged_counts(b1, b2);
  }
};

DADOTest prepare_data_paper() {
  DADOTest hist(4);
  hist.setTotalPoints(100);
  auto &bins = hist.accessBins();
  bins[1] = {20, 4};
  bins[3] = {10, 9};
  bins[6] = {9, 11};
  bins[11] = {18, 19};
  hist.setMaxPoint(15);
  return hist;
}

DADOTest prepare_data_freq() {
  DADOTest hist(2);
  hist.setTotalPoints(100);
  auto &bins = hist.accessBins();
  bins[1] = {8, 10};
  bins[3] = {7, 11};
  hist.setMaxPoint(11);
  return hist;
}

TEST(benhaim, paper) {
  DADOTest hist = prepare_data_paper();
  auto s = hist.callFindBucketToSplit();
  ASSERT_EQ(s.first->first, 1);
  auto m = hist.callFindBucketToMerge();
  ASSERT_EQ(m.first->first, 3);
  hist.callSplit(s.first);
  hist.callMerge(m.first, std::next(m.first));
  std::map<double, DADOBin> map = {
      {1, {10, 10}}, {2, {2, 2}}, {3, {22.6, 16.4}}, {11, {18, 19}}};
  ASSERT_EQ(hist.getBins(), map);
}

TEST(benhaim, freq) {
  DADOTest hist = prepare_data_freq();
  auto s = hist.callFindBucketToSplit();
  ASSERT_EQ(s.first->first, 3);
}
