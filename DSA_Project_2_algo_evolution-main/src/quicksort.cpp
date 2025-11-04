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

// Lomuto partition below
static size_t partition_lomuto(std::span<int> a, int pivot, Metrics& m) {
  size_t i=0;
  for (size_t j=0;j+1<a.size();++j) {
    if (less_cmp(a[j], pivot, m)) { swap_do(a[i], a[j], m); ++i; }
  }
  // places the pivot at i by swapping with last one
  swap_do(a[i], a[a.size()-1], m);
  return i;
}

// Hoare partition
static size_t partition_hoare(std::span<int> a, int pivot, Metrics& m) {
  size_t i=0, j=a.size()-1;
  while (true) {
    while (less_cmp(a[i], pivot, m)) ++i;
    while (less_cmp(pivot, a[j], m)) --j;
    if (i>=j) return j;
    swap_do(a[i], a[j], m);
    ++i; --j;
  }
}
static void qs_impl(std::span<int> a, const QSDNA& dna, Metrics& m, int depthLeft) {
  if (a.size() <= 1) return;
  if ((int)a.size() <= dna.insertionCutoff) { insertion_sort(a, m); return; }
  if (depthLeft <= 0) { insertion_sort(a, m); return; } // simple cap fallback
  // picks the pivot by moving chosen pivot to end for Lomuto 
  int pv = pivot_choose(a, dna.pivot, m);
  // place the pivot at end
  size_t last = a.size()-1;
  // find a position equal to pv and swap to end
  for (size_t k=0;k<a.size();++k) if (a[k]==pv) { swap_do(a[k], a[last], m); break; }
  size_t cut;
  if (dna.scheme == PartitionScheme::Lomuto) {
    cut = partition_lomuto(a, pv, m);
    auto L = a.first(cut);
    auto R = a.subspan(cut+1);
    qs_impl(L, dna, m, depthLeft-1);
    qs_impl(R, dna, m, depthLeft-1);
  } else {
    size_t idx = partition_hoare(a, pv, m);
    auto L = a.first(idx+1);
    auto R = a.subspan(idx+1);
    if (dna.tailRecElim) {
      // Recurse smaller part first and then loop on larger part
      while (true) {
        auto left = (L.size() < R.size()) ? L : R;
        auto right= (L.size() < R.size()) ? R : L;
        qs_impl(left, dna, m, depthLeft-1);
        if (right.size() <= 1) break;
        // tail call elimination by reassigning a slice
        a = right;
        pv = pivot_choose(a, dna.pivot, m);
        last = a.size()-1;
        for (size_t k=0;k<a.size();++k) if (a[k]==pv) { swap_do(a[k], a[last], m); break; }
        idx = partition_hoare(a, pv, m);
        L = a.first(idx+1); R = a.subspan(idx+1);
        if (R.size() <= 1 && L.size() <= 1) break;
      }
    } else {
      qs_impl(L, dna, m, depthLeft-1);
      qs_impl(R, dna, m, depthLeft-1);
    }
  }
}
void quicksort(std::span<int> a, const QSDNA& dna, Metrics& m) {
  int depth = dna.depthCap > 0 ? dna.depthCap : 64;
  qs_impl(a, dna, m, depth);
}
