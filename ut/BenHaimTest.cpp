#include <cmath>
#include <gtest/gtest.h>
#include <sketches/BenHaim.hpp>

class AdvancedBenHaimTest : public AdvancedBenHaim {
public:
  AdvancedBenHaimTest(uint32_t number_of_points)
      : AdvancedBenHaim(number_of_points) {
  }
  double getUnnormalizedEstimate(double x) {
    return AdvancedBenHaim::getUnnormalizedEstimate(x);
  }
};

TEST(benhaim, empty) {
  AdvancedBenHaimTest hist(1);
  ASSERT_TRUE(std::isnan(hist.getUnnormalizedEstimate(1)));
}

TEST(benhaim, singleton) {
  AdvancedBenHaimTest hist(1);
  hist.addPoint(12);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(12), 1, 0.0001);
}

TEST(benhaim, singleton_multiple) {
  AdvancedBenHaimTest hist(1);
  hist.addPoint(12);
  hist.addPoint(12);
  hist.addPoint(12);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(12), 3, 0.0001);
}

TEST(benhaim, singleton_three) {
  AdvancedBenHaimTest hist(1);
  hist.addPoint(1);
  hist.addPoint(2);
  hist.addPoint(3);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(2), 3, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(1), 0, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(3), 0, 0.0001);
}

TEST(benhaim, three_points) {
  AdvancedBenHaimTest hist(3);
  hist.addPoint(1);
  hist.addPoint(2);
  hist.addPoint(3);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(2), 1, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(1), 1, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(3), 1, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(1.5), 1, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(2.5), 1, 0.0001);
}

TEST(benhaim, two_points) {
  AdvancedBenHaimTest hist(2);
  hist.addPoint(1);
  hist.addPoint(2);
  hist.addPoint(3);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(2), 1.11111, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(1), 0, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(3), 0.66666, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(1.5), 1.33333, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(2.5), 0.88888, 0.0001);
  ASSERT_NEAR(hist.getUnnormalizedEstimate(4), 0, 0.0001);
}
