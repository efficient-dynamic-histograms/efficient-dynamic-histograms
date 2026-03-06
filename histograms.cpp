#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <sketches/BenHaim.hpp>
#include <sketches/DADO.hpp>
#include <vector>

std::vector<double> read_file() {
  // std::ifstream in("../../files/stream.csv");
  std::ifstream in("stream.csv");
  if (!in.good()) {
    std::cerr << "File not opened" << std::endl;
    exit(-1);
  }
  int num_lines;
  in >> num_lines;
  std::vector<double> stream;
  stream.reserve(num_lines);
  double elem;
  while (!in.eof()) {
    in >> elem;
    stream.push_back(elem);
  }
  in.close();
  return stream;
}

std::vector<double> read_file_shuffled() {
  std::vector<double> stream = read_file();
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::ranges::shuffle(stream, gen);
  return stream;
}

int main() {
  std::vector<double> stream = read_file();
  // AllKDEBenHaim bh(30, 3.0);
  // LimitedKDEBenHaim bh(30, 3.0);
  // AdvancedBenHaim bh(30);
  // SimpleBenHaim bh(30);
  DADO bh(30);
  for (auto &elem : stream) {
    bh.addPoint(elem);
  }
  double start = -10.0;
  double end = 10.0;
  std::ofstream out("estimated.csv");
  for (double x = start; x <= end; x += 0.00005) {
    out << x << "," << bh.getEstimate(x) << std::endl;
  }
  out.close();
  out.open("histogram.csv");
  auto histogram = bh.getHistogram();
  std::cout << histogram.size() << std::endl;
  for (auto it = histogram.begin(); it != std::prev(histogram.end()); it++) {
    // out << it->x << "," << it->count << std::endl;
    out << it->first << "," << std::next(it)->first << ","
        << it->second.left_cnt << "," << it->second.right_cnt << ","
        << std::next(it)->first - it->first << std::endl;
  }
  out.close();
}