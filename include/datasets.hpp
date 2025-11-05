#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string>

enum class Dist { Uniform, NearlySorted, Reverse, ManyDup, Kaggle }; // Kaggle optional

std::vector<int> make_array(std::size_t n, Dist d, uint64_t seed);

// Kaggle importer: picks first numeric column; truncates/extends to n by cycling/seeding.
std::vector<int> load_kaggle_column_as_ints(const std::string& csv_path, std::size_t n);
