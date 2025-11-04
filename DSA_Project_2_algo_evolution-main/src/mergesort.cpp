#include "mergesort.hpp"
#include <algorithm>
#include <cassert>
static inline bool less_cmp(int a, int b, Metrics& m) { ++m.comparisons; return a < b; }
static inline void move_do(int& dst, int src, Metrics& m) { ++m.swaps; dst = src; }
static void insertion_sort(std::span<int> a, Metrics& m) {
  for (size_t i=1;i<a.size();++i) {
    int key = a[i];
    size_t j = i;
    while (j>0 && less_cmp(key, a[j-1], m)) {
      a[j] = a[j-1]; ++m.swaps; --j;
    }
    a[j] = key;
  }
}
static void merge_run(std::span<int> a, std::span<int> b, size_t left, size_t mid, size_t right, Metrics& m) {
  size_t i=left, j=mid, k=left;
  while (i<mid && j<right) {
    if (!less_cmp(b[j], b[i], m)) move_do(a[k++], b[i++], m);
    else                          move_do(a[k++], b[j++], m);
  }
  while (i<mid) move_do(a[k++], b[i++], m);
  while (j<right) move_do(a[k++], b[j++], m);
}
void mergesort(std::span<int> a, const MSDNA& dna, Metrics& m) {
  size_t n = a.size();
  if (n<=1) return;

  if (!dna.iterative) {
    // easy top-down with small-run insertion
    if (n <= (size_t)dna.runThreshold) { insertion_sort(a, m); return; }
    size_t mid = n/2;
    mergesort(a.first(mid), dna, m);
    mergesort(a.subspan(mid), dna, m);
    std::vector<int> tmp(a.begin(), a.end());
    std::span<int> b(tmp.data(), tmp.size());
    merge_run(a, b, 0, mid, n, m);
    return;
  }
