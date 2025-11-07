#include "mergesort.hpp"
#include <algorithm>
#include <cassert>
using namespace std; 
static inline bool less_cmp(int a, int b, Metrics& m) { ++m.comparisons; return a < b; }
static inline void move_do(int& dst, int src, Metrics& m) { ++m.swaps; dst = src; }

static void insertion_sort(span<int> a, Metrics& m) {
  for (size_t i=1;i<a.size();++i) {
    int key = a[i];
    size_t j = i;
    while (j>0 && less_cmp(key, a[j-1], m)) {
      a[j] = a[j-1]; ++m.swaps; --j;
    }
    a[j] = key;
  }
}

static void merge_run(span<int> a, span<int> b, size_t left, size_t mid, size_t right, Metrics& m) {
  size_t i=left, j=mid, k=left;
  while (i<mid && j<right) {
    if (!less_cmp(b[j], b[i], m)) move_do(a[k++], b[i++], m);
    else                          move_do(a[k++], b[j++], m);
  }
  while (i<mid) move_do(a[k++], b[i++], m);
  while (j<right) move_do(a[k++], b[j++], m);
}

void mergesort(span<int> a, const MSDNA& dna, Metrics& m) {
  size_t n = a.size();
  if (n<=1) return;

  if (!dna.iterative) {
    // Simple top-down with small-run insertion
    if (n <= (size_t)dna.runThreshold) { insertion_sort(a, m); return; }
    size_t mid = n/2;
    mergesort(a.first(mid), dna, m);
    mergesort(a.subspan(mid), dna, m);
    vector<int> tmp(a.begin(), a.end());
    span<int> b(tmp.data(), tmp.size());
    merge_run(a, b, 0, mid, n, m);
    return;
  }

  // Bottom-up iterative mergesort with optional reusable buffer
 vector<int> buf;
 span<int> A = a;
 span<int> B = A;
 vector<int> storage;
  if (dna.reuseBuffer) {
    storage.assign(a.begin(), a.end());
    B = span<int>(storage.data(), storage.size());
  } else {
    storage.resize(0); // will reallocate per pass below
  }

  // small-run insertion pre-pass
  if (dna.runThreshold > 0) {
    for (size_t i=0;i<n;i += (size_t) dna.runThreshold) {
      size_t r = min(n, i + (size_t)dna.runThreshold);
      insertion_sort(A.subspan(i, r-i), m);
    }
  }

  for (size_t width = max<size_t>(1, (size_t)dna.runThreshold); width < n; width *= 2) {
          if (!dna.reuseBuffer) {
        storage.assign(A.begin(), A.end());
      B = span<int>(storage.data(), storage.size());
    }
    // copy A -> B
    for (size_t i=0;i<n;++i) B[i] = A[i], ++m.swaps;

    for (size_t i=0;i<n; i += 2*width) {
      size_t left = i;
         size_t mid  = min(i+width, n);
        size_t right= min(i+2*width, n);
      merge_run(A, B, left, mid, right, m);
    }
  }
}
