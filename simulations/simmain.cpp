#include <simulations/simutils.hpp>

template <typename T>
duration_t call_loda_method(const LabeledDataset &df, json config,
                            std::filesystem::path dataset_path,
                            std::string method_name, uint16_t run_idx) {
  std::cerr << "Running " << method_name << std::endl;
  uint32_t loda_sketches = config["loda"]["num_sketches"];
  uint32_t number_of_points = config[method_name]["num_points"];
  std::filesystem::path out_filepath =
      get_output_filename(config, dataset_path, method_name, run_idx);
  std::vector<std::unique_ptr<IDensity>> sketches;
  for (uint32_t idx = 0; idx < loda_sketches; idx++) {
    sketches.emplace_back(std::make_unique<T>(number_of_points));
  }
  InstantLoda loda(df.num_dimensions(), std::move(sketches));

  std::vector<double> results(df.size());

  auto start = std::chrono::high_resolution_clock::now();
  for (uint64_t i = 0; i < df.size(); i++) {
    results[i] = loda.add_estimate(df.at(i));
  }
  auto end = std::chrono::high_resolution_clock::now();
  duration_t duration = end - start;
  std::filesystem::create_directories(out_filepath.parent_path());
  std::ofstream of(out_filepath);
  for (uint64_t i = 0; i < df.size(); i++) {
    of << results[i] << "," << df.at(i).m_is_anomaly << "\n";
  }
  of.close();
  return duration;
}

std::pair<std::string, duration_t>
isolation_forest(const LabeledDataset &df, json config,
                 std::filesystem::path dataset_path, uint16_t run_idx) {
  std::string method_name = TypeName<IsolationForest<LabeledInstance>>::get();
  std::cerr << "Running " << method_name << std::endl;
  uint32_t num_trees = config["iforest"]["num_trees"];
  uint32_t sample_size = config["iforest"]["sample_size"];
  std::filesystem::path out_filepath =
      get_output_filename(config, dataset_path, method_name, run_idx);

  std::mt19937_64 engine;
  std::vector<double> results(df.size());
  auto start = std::chrono::high_resolution_clock::now();
  IsolationForest<LabeledInstance> forest(df, num_trees, sample_size, engine);
  for (uint64_t i = 0; i < df.size(); i++) {
    results[i] = forest.score(df.at(i));
  }
  auto end = std::chrono::high_resolution_clock::now();
  duration_t duration = end - start;
  std::filesystem::create_directories(out_filepath.parent_path());
  std::ofstream of(out_filepath);
  for (uint64_t i = 0; i < df.size(); i++) {
    of << results[i] << "," << df.at(i).m_is_anomaly << "\n";
  }
  of.close();
  return {method_name, duration};
}

template <typename T>
std::pair<std::string, duration_t>
call_wrapper(const LabeledDataset &df, json config,
             std::filesystem::path dataset_path, uint16_t idx) {
  return {TypeName<T>::get(), call_loda_method<T>(df, config, dataset_path,
                                                  TypeName<T>::get(), idx)};
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

  std::uint16_t num_runs = config["num_runs"];
  std::string delimiter_string = config["delimiter"];
  bool shuffle = config["shuffle"];
  char delimiter = delimiter_string[0];

  std::initializer_list<std::pair<std::string, duration_t> (*)(
      const LabeledDataset &, json, std::filesystem::path, uint16_t)>
      methods = {call_wrapper<SimpleBenHaim>,
                 call_wrapper<AdvancedBenHaim>,
                 call_wrapper<IncorrectBenHaim>,  call_wrapper<AllKDEBenHaim>,
                 call_wrapper<LimitedKDEBenHaim>,
                 call_wrapper<DADO>,
                 call_wrapper<DADO_Corrected>,
                 isolation_forest};
  std::map<std::string, std::vector<duration_t>> durations;
  for (uint16_t run_idx = 0; run_idx < num_runs; run_idx++) {
    const LabeledDataset df =
        loader::load_dataset(dataset_path, delimiter, shuffle);
    std::cerr << "Dataset has been loaded" << std::endl;
    for (auto elem : methods) {
      auto [name, duration] = elem(df, config, dataset_path, run_idx);
      durations[name].push_back(duration);
    }
  }

  std::filesystem::path reports_directory = config["reports_directory"];
  std::filesystem::create_directories(reports_directory);

  json durations_json = durations;
  std::ofstream file(reports_directory.append(
      "result_" + getFilenameWithoutExtension(dataset_path)));
  file << durations_json.dump(4);
  file.close();
}