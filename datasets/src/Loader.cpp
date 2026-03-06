#include "datasets/Loader.hpp"

namespace loader {
LabeledDataset load_dataset(std::filesystem::path filename, char delimiter,
                            bool shuffle) {
  std::ifstream in(filename);
  if (!in.good()) {
    exit(-1);
  }
  std::string line;
  std::getline(in, line);
  std::vector<Column> columns;
  std::stringstream header_stream(line);
  std::string tmp;
  uint32_t num_features = 0;
  while (std::getline(header_stream, tmp, delimiter)) {
    columns.emplace_back(tmp, num_features);
    num_features++;
  }
  std::vector<LabeledInstance> instances;
  while (std::getline(in, line)) {
    std::vector<std::string> row;
    std::stringstream lineStream(line);
    std::vector<feature_t> features(num_features);
    bool anomaly = false;
    for (uint32_t i = 0; i < num_features; i++) {
      std::getline(lineStream, tmp, delimiter);
      std::istringstream(tmp) >> features[i];
    }
    std::getline(lineStream, tmp, delimiter);
    if (tmp.ends_with("\r")) {
      tmp = tmp.substr(0, tmp.length() - 1);
    }
    if (tmp == "true" || tmp == "0") {
      anomaly = true;
    }
    instances.emplace_back(features, anomaly);
  }
  if (shuffle) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::ranges::shuffle(instances, gen);
  }
  return LabeledDataset(DatasetContainer(instances), columns);
}
} // namespace loader