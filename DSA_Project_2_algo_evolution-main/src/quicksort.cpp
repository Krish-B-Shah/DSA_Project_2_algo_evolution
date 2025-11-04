#include "quicksort.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <cassert>
#include <cmath>
static inline bool less_cmp(int a, int b, Metrics& m) { ++m.comparisons; return a < b; }
static inline void swap_do(int& a, int& b, Metrics& m) { ++m.swaps; std::swap(a,b); }
static void insertion_sort(std::span<int> a, Metrics& m) {
  for (size_t i=1;i<a.size();++i) {
    int key = a[i];
    size_t j = i;
    while (j>0 && less_cmp(key, a[j-1], m)) {
      a[j] = a[j-1];
      ++m.swaps; // count moves as swaps to keep a simple metric
      --j;
    }
    a[j] = key;
  }
}
