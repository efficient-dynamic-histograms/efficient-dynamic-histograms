#pragma once

#include <algorithm>
#include <datasets/Dataset.hpp>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>

namespace loader {
LabeledDataset load_dataset(std::filesystem::path filename, char delimiter,
                            bool shuffle = false);
}