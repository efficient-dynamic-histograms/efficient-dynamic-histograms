#include <simulations/simutils.hpp>

std::vector<double> read_file(std::filesystem::path dataset_path) {
  std::ifstream in(dataset_path);
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

template <typename T>
void evaluate(const std::vector<double> &dataset, const json config,
              std::filesystem::path dataset_path, uint16_t run_idx) {
  std::string method_name = TypeName<T>::get();
  std::cerr << "Running " << method_name << std::endl;
  uint32_t number_of_points = config[method_name]["num_points"];
  std::unique_ptr<T> histogram = std::make_unique<T>(number_of_points);
  for (auto &elem : dataset) {
    histogram->addPoint(elem);
  }
  double start = config["range_start"];
  double end = config["range_end"];
  double increment = config["increment"];
  std::filesystem::path out_filepath =
      get_output_filename(config, dataset_path, method_name, run_idx);
  std::filesystem::create_directories(out_filepath.parent_path());
  std::ofstream out(out_filepath);
  for (double x = start; x < end; x += increment) {
    out << x << "," << histogram->getEstimate(x) << std::endl;
  }
  out.close();
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <dataset> <config>\n";
    exit(1);
  }
  std::filesystem::path dataset_path = argv[1];
  std::filesystem::path config_path = argv[2];

  std::ifstream config_file(config_path);
  json config;

  if (!config_file.is_open()) {
    exit(2);
  }
  config_file >> config;
  config_file.close();
  std::cerr << "Config has been loaded" << std::endl;

  std::vector<double> dataset = read_file(dataset_path);
  std::cerr << "Dataset has been loaded" << std::endl;
  std::uint16_t num_runs = config["num_runs"];

  std::initializer_list<void (*)(const std::vector<double> &, json,
                                 std::filesystem::path, uint16_t)>
      methods = {evaluate<SimpleBenHaim>,
                 evaluate<AdvancedBenHaim>,
                 evaluate<AllKDEBenHaim>,
                 evaluate<LimitedKDEBenHaim>,
                 evaluate<DADO>,
                 evaluate<DADO_Corrected>,
                 evaluate<IncorrectBenHaim>};

  std::random_device rd;
  std::array<std::uint32_t, 2> seed_data = {rd(), rd()};
  std::seed_seq seed_seq(seed_data.begin(), seed_data.end());
  std::mt19937_64 engine(seed_seq);

  for (uint16_t run_idx = 0; run_idx < num_runs; run_idx++) {
    for (auto elem : methods) {
      std::shuffle(dataset.begin(), dataset.end(), engine);
      elem(dataset, config, dataset_path, run_idx);
    }
  }
}