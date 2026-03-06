#include <cmath>
#include <datasets/Dataset.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(datasets, slices) {
  Instance a1({1, 1, 9});
  Instance a2({1, 3, 10});
  Instance a3({4, 2, 11});
  std::vector<Column> columns = {Column("a", 0), Column("b", 1),
                                 Column("c", 2)};
  Dataset ds(std::vector({a1, a2, a3}), columns);
  std::cout << ds.size() << std::endl;

  std::cout << "splitds" << std::endl;
  auto [left, right] = ds.split(columns[0], 3);
  std::cout << left.size() << std::endl;
  std::cout << right.size() << std::endl;
  std::cout << left.at(0)[0] << "," << left.at(0)[1] << "," << left.at(0)[2]
            << std::endl;
  std::cout << left.at(1)[0] << "," << left.at(1)[1] << "," << left.at(1)[2]
            << std::endl;
  std::cout << right.at(0)[0] << "," << right.at(0)[1] << "," << right.at(0)[2]
            << std::endl;

  std::cout << "splitslice" << std::endl;

  auto [left1, right1] = left.split(columns[1], 2);
  std::cout << left1.size() << std::endl;
  std::cout << right1.size() << std::endl;
  std::cout << left1.at(0)[0] << "," << left1.at(0)[1] << "," << left1.at(0)[2]
            << std::endl;
  std::cout << right1.at(0)[0] << "," << right1.at(0)[1] << ","
            << right1.at(0)[2] << std::endl;
  ASSERT_EQ(&(ds.at(0)[0]), &(left1.at(0)[0]));
}