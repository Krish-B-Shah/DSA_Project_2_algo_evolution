#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "dna.hpp"
#include "metrics.hpp"

// Input distributions
enum class Dist { Uniform=0, NearlySorted=1, Reverse=2, Duplicates=3, Kaggle=4 };

struct EvalConfig {
  uint64_t n = 100000;
  int trialsPerDist = 5;
  uint64_t masterSeed = 12345;
  std::vector<Dist> dists = {Dist::Uniform, Dist::NearlySorted, Dist::Reverse, Dist::Duplicates};
  bool useKaggle = false;
  std::string kaggleCsvPath = "data/kaggle.csv";
  int jobs = 0;                 // 0 => auto (hardware threads)
  bool precompute = true;       // precompute base arrays and reuse
};

struct EvalResult {
  double fitness_ms = 0.0;
  uint64_t comparisons = 0;
  uint64_t swaps = 0;
};

inline unsigned dist_mask_of(const std::vector<Dist>& v){
  unsigned m=0; for (auto d: v) m |= (1u<<int(d)); return m;
}

// Evaluate one DNA
EvalResult eval_qs(const QSDNA& d, const EvalConfig& cfg);
EvalResult eval_ms(const MSDNA& d, const EvalConfig& cfg);
