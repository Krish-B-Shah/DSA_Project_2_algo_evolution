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
static std::vector<int> reverse_sorted(std::size_t n) {
  std::vector<int> a(n);
  for (size_t i=0;i<n;++i) a[i] = int(n-1-i);
  return a;
}
static std::vector<int> many_dups(std::size_t n, uint64_t seed) {
  XRand rng(seed);
  std::vector<int> a(n);
  int k = 100; // limited value range
  for (size_t i=0;i<n;++i) a[i] = int(rng.uniform(0, k-1));
  return a;
}
static std::vector<int> uniform_random(std::size_t n, uint64_t seed) {
  XRand rng(seed);
  std::vector<int> a(n);
  for (size_t i=0;i<n;++i) a[i] = int(rng.next() & 0x7fffffff);
  return a;
}
std::vector<int> load_kaggle_column_as_ints(const std::string& csv_path, std::size_t n) {
  std::ifstream in(csv_path);
  std::vector<int> vals;
  vals.reserve(n);
  if (!in) return vals; // empty to the caller falls back
  std::string line;
  // tries to find the first column 
  std::vector<size_t> numericCols;
  bool headerParsed = false;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    std::stringstream ss(line);
    std::string cell;
    std::vector<std::string> cells;
    while (std::getline(ss, cell, ',')) cells.push_back(cell);
    if (!headerParsed) { headerParsed = true; continue; } // skips header
    // detect the numeric columns
    if (numericCols.empty()) {
      for (size_t c=0;c<cells.size();++c) {
        double d; char* endp=nullptr;
        try {
          d = std::stod(cells[c]);
          (void)d;
          numericCols.push_back(c);
        } catch (...) { /* not numeric */ }
      }
      if (numericCols.empty()) continue;
    }
    if (!numericCols.empty()) {
      try {
        double d = std::stod(cells[numericCols[0]]);
        long long asInt = (long long) std::llround(d);
        vals.push_back(int(asInt));
      } catch (...) {}
    }
    if (vals.size() >= n) break;
  }
  // stretch to size n
  if (vals.empty()) return vals;
  if (vals.size() < n) {
    size_t m = vals.size();
    vals.reserve(n);
    for (size_t i=m;i<n;++i) vals.push_back(vals[i % m]);
  } else if (vals.size() > n) {
    vals.resize(n);
  }
  return vals;
}
