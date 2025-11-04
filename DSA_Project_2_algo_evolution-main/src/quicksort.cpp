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

static int pivot_choose(std::span<int> a, Pivot p, Metrics& m) {
  if (p==Pivot::First) return a.front();
  if (p==Pivot::Last)  return a.back();
  // Median-of-3
  size_t l=0, r=a.size()-1, mid=(l+r)/2;
  int x=a[l], y=a[mid], z=a[r];
  // compare counts
  bool xy = less_cmp(x,y,m), yz = less_cmp(y,z,m), xz = less_cmp(x,z,m);
  // simple median logic beloww
  if ((xy && yz) || (!xy && !xz)) return y;
  if ((xz && !yz) || (!xz && yz)) return z;
  return x;
}
