#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string>

// Forward declare Dist - defined in evaluator.hpp
enum class Dist;

std::vector<int> make_array(std::size_t n, Dist d, uint64_t seed);

// Kaggle importer: picks first numeric column; truncates/extends to n by cycling/seeding.
std::vector<int> load_kaggle_column_as_ints(const std::string& csv_path, std::size_t n);
