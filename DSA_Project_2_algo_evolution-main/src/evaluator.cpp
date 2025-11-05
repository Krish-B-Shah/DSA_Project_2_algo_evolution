#include "evaluator.hpp"
#include "datasets.hpp"
#include "quicksort.hpp"
#include "mergesort.hpp"
#include "common.hpp"
#include <future>
#include <mutex>
#include <numeric>
#include <cmath>
#include <unordered_map>

using std::vector;
// precompute caching for efficiency
struct PrecompKey {
  uint64_t n; int trials; uint64_t seed; bool kaggle;
  bool operator==(const PrecompKey& o) const {
    return n==o.n && trials==o.trials && seed==o.seed && kaggle==o.kaggle;
  }
};
struct PrecompKeyHash {
  size_t operator()(const PrecompKey& k) const {
    size_t h = std::hash<uint64_t>{}(k.n);
    h ^= std::hash<uint64_t>{}(k.seed + 0x9e3779b97f4a7c15ull + (h<<6)+(h>>2));
    h ^= std::hash<int>{}(k.trials + 13);
    h ^= std::hash<bool>{}(k.kaggle + 29);
    return h;
  }
};
// for each of the dist,trial pairs we store base array that is reused and copied
struct PrecompSet {
  // map Dist -> base arrays [trial]
  std::unordered_map<int, vector<vector<int>>> base;
};
