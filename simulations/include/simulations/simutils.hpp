#pragma once

#include <algorithms/IForest.hpp>
#include <algorithms/Loda.hpp>
#include <chrono>
#include <datasets/Dataset.hpp>
#include <datasets/Loader.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <random>
#include <simulations/typenames.hpp>

using json = nlohmann::json;
using duration_t = std::chrono::duration<double, std::milli>;

std::string getFilenameWithoutExtension(const std::string &path) {
  size_t lastSlash = path.find_last_of("/\\");
  size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
  size_t end = path.find_last_of('.');
  if (end == std::string::npos || end < start) {
    end = path.length();
  }
  return path.substr(start, end - start);
}

namespace nlohmann {
template <> struct adl_serializer<duration_t> {
  static void to_json(json &j, const duration_t &d) {
    j = json{{"milliseconds", d.count()}};
  }

  static void from_json(const json &j, duration_t &d) {
    d = duration_t(j.get<double>());
  }
};
} // namespace nlohmann

std::filesystem::path get_output_filename(json config, std::string dataset_path,
                                          std::string method_name,
                                          uint16_t idx) {
  std::string out_dir = config["out_directory"];
  return out_dir + "/" + getFilenameWithoutExtension(dataset_path) + "/" +
         method_name + "_" + std::to_string(idx) + ".csv";
}