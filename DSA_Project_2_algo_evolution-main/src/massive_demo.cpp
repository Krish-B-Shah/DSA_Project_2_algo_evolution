#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <string>
#include <future>
struct Metrics {
  uint64_t comparisons = 0;
  uint64_t swaps = 0;
};
// quicksort
void quicksort_simple(std::vector<int>& arr, int left, int right, Metrics& m, 
                      int pivot_choice, int partition_type, int cutoff){
  if(left >= right) return;
  if(right - left + 1 <= cutoff){
    // insertion sort implementation beloww
    for(int i = left + 1; i <= right; i++){
      int key = arr[i];
      int j = i - 1;
      while(j >= left && (++m.comparisons, arr[j] > key)){
        arr[j + 1] = arr[j];
        m.swaps++;
        j--;
      }
      arr[j + 1] = key;
    }
    return;
  }
  int pivot_val;
  if(pivot_choice == 0) pivot_val = arr[left];
  else if(pivot_choice == 1) pivot_val = arr[right];
  else pivot_val = arr[(left + right) / 2];
  if(partition_type == 0){
    int pivot_idx = left;
    for(int i = left; i < right; i++){
      if(++m.comparisons, arr[i] < pivot_val){
        std::swap(arr[pivot_idx], arr[i]);
        m.swaps++;
        pivot_idx++;
      }
    }
    std::swap(arr[pivot_idx], arr[right]);
    m.swaps++;
    quicksort_simple(arr, left, pivot_idx - 1, m, pivot_choice, partition_type, cutoff);
    quicksort_simple(arr, pivot_idx + 1, right, m, pivot_choice, partition_type, cutoff);
  }else{
    int i = left, j = right;
    while(i <= j){
      while(++m.comparisons, arr[i] < pivot_val) i++;
      while(++m.comparisons, arr[j] > pivot_val) j--;
      if(i <= j){
      std::swap(arr[i], arr[j]);
      m.swaps++;
      i++; j--;
    }
    }
    quicksort_simple(arr, left, j, m, pivot_choice, partition_type, cutoff);
    quicksort_simple(arr, i, right, m, pivot_choice, partition_type, cutoff);
  }
}
