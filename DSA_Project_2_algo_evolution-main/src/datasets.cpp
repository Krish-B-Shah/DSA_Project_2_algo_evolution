#include "datasets.hpp"
#include "evaluator.hpp"  // for thee Dist enum definition
#include "common.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <charconv>
#include <cmath>

static std::vector<int> nearly_sorted(std::size_t n, uint64_t seed) {
  std::vector<int> a(n);
  for (size_t i=0;i<n;++i) a[i] = int(i);
  XRand rng(seed);
  size_t swaps = std::max<std::size_t>(1, n/100); // ~1%
  for (size_t k=0;k<swaps;++k) {
    size_t i = rng.uniform(0, n-1), j = rng.uniform(0, n-1);
    std::swap(a[i], a[j]);
  }
  return a;
}
