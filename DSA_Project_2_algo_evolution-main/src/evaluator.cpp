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
static std::mutex g_pre_mtx;
static std::unordered_map<PrecompKey, PrecompSet, PrecompKeyHash> g_pre;
static const PrecompSet& get_pre(const EvalConfig& cfg){
  if (!cfg.precompute) { static PrecompSet dummy; return dummy; }
  PrecompKey key{cfg.n, cfg.trialsPerDist, cfg.masterSeed, cfg.useKaggle};
  std::scoped_lock lk(g_pre_mtx);
  auto it = g_pre.find(key);
  if (it != g_pre.end()) return it->second;
  // builds new
  PrecompSet set;
  XRand rng(cfg.masterSeed);
  auto make_all_for = [&](Dist d){
    vector<vector<int>> vv(cfg.trialsPerDist);
    for (int t=0; t<cfg.trialsPerDist; ++t){
      uint64_t seed = cfg.masterSeed + 1337ull*uint64_t(d) + uint64_t(t);
      vector<int> arr;
      if (d == Dist::Kaggle && cfg.useKaggle) {
        arr = load_kaggle_column_as_ints(cfg.kaggleCsvPath, cfg.n);
      } else {
        arr = make_array(cfg.n, d, seed);
      }
      vv[t] = std::move(arr);
    }
    set.base[int(d)] = std::move(vv);
  };
  for (auto d: cfg.dists) make_all_for(d);
  if (cfg.useKaggle) make_all_for(Dist::Kaggle);
  auto [it2, _] = g_pre.emplace(key, std::move(set));
  return it2->second;
}
// helpers for program
struct Accum {
  double geo_sum = 0.0; // sum of the log(ms)
  int count = 0;
  uint64_t comps = 0, swaps = 0;
};
static inline double geo_mean_from_logsum(double s, int n){
  return std::exp(s / std::max(1,n));
}
