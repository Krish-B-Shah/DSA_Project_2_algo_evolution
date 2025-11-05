#include "quicksort.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <cassert>
#include <cmath>

static inline bool do_compare(int val_a, int val_b, Metrics& metrics) {
    metrics.comparisons = metrics.comparisons + 1;
    return val_a < val_b;
}

static inline void do_swap(int& elem_a, int& elem_b, Metrics& metrics) {
    metrics.swaps = metrics.swaps + 1;
    std::swap(elem_a, elem_b);
}

// Insertion sort works better for small arrays
static void run_insertion_sort(std::span<int> arr, Metrics& metrics) {
    for (size_t idx = 1; idx < arr.size(); ++idx) {
        int current_key = arr[idx];
        size_t pos = idx;
        
         while (pos > 0 && do_compare(current_key, arr[pos - 1], metrics)) {
            arr[pos] = arr[pos - 1];
            metrics.swaps = metrics.swaps + 1;
            pos = pos - 1;
        }
        
        arr[pos] = current_key;
    }
}

// Choose pivot using first, last, or median strategy
static int select_pivot(std::span<int> arr, Pivot strategy, Metrics& metrics) {
    if (strategy == Pivot::First) {
        return arr.front();
    }
    
    if (strategy == Pivot::Last) {
        return arr.back();
    }
    
    size_t left_idx = 0;
     size_t right_idx = arr.size() - 1;
    size_t mid_idx = (left_idx + right_idx) / 2;
    
    int left_val = arr[left_idx];
    int mid_val = arr[mid_idx];
    int right_val = arr[right_idx];
    
    bool left_less_mid = do_compare(left_val, mid_val, metrics);
    bool mid_less_right = do_compare(mid_val, right_val, metrics);
    bool left_less_right = do_compare(left_val, right_val, metrics);
    
    // Figure out which value is in the middle
    if ((left_less_mid && mid_less_right) || (!left_less_mid && !left_less_right)) {
        return mid_val;
    }
    
    if ((left_less_right && !mid_less_right) || (!left_less_right && mid_less_right)) {
        return right_val;
    }
    
    return left_val;
}

// Lomuto partition scheme - simpler implementation
static size_t partition_lomuto(std::span<int> arr, int pivot, Metrics& metrics) {
    size_t boundary = 0;
    
    for (size_t scan = 0; scan + 1 < arr.size(); ++scan) {
        if (do_compare(arr[scan], pivot, metrics)) {
            do_swap(arr[boundary], arr[scan], metrics);
            boundary = boundary + 1;
        }
    }
    
    do_swap(arr[boundary], arr[arr.size() - 1], metrics);
    
    return boundary;
}

// Hoare partition uses two pointers moving inward
static size_t partition_hoare(std::span<int> arr, int pivot, Metrics& metrics) {
    size_t left = 0;
    size_t right = arr.size() - 1;
    
    while (true) {
        while (do_compare(arr[left], pivot, metrics)) {
             left = left + 1;
        }
        
        while (do_compare(pivot, arr[right], metrics)) {
            right = right - 1;
        }

        if (left >= right) {
              return right;
        }
        
        do_swap(arr[left], arr[right], metrics);
        
        left = left + 1;
        right = right - 1;
    }
}

// Main recursive sorting function with various optimizations
static void quicksort_impl(std::span<int> arr, 
                             const QSDNA& dna, 
                          Metrics& metrics, 
                          int depth_remaining) {
    if (arr.size() <= 1) {
        return;
    }

    if ((int)arr.size() <= dna.insertionCutoff) {
        run_insertion_sort(arr, metrics);
        return;
    }
    
    if (depth_remaining <= 0) {
        run_insertion_sort(arr, metrics);
        return;
    }

    int pivot_val = select_pivot(arr, dna.pivot, metrics);
    size_t last_idx = arr.size() - 1;
    
    // Move the chosen pivot to the end
    for (size_t k = 0; k < arr.size(); ++k) {
        if (arr[k] == pivot_val) {
            do_swap(arr[k], arr[last_idx], metrics);
            break;
        }
    }

    size_t split_point;
    
    if (dna.scheme == PartitionScheme::Lomuto) {
        split_point = partition_lomuto(arr, pivot_val, metrics);
        
        auto left_part = arr.first(split_point);
        auto right_part = arr.subspan(split_point + 1);
        
        quicksort_impl(left_part, dna, metrics, depth_remaining - 1);
        quicksort_impl(right_part, dna, metrics, depth_remaining - 1);
    } else {
        size_t hoare_idx = partition_hoare(arr, pivot_val, metrics);
        
                

        
                auto left_part = arr.first(hoare_idx + 1);
        auto right_part = arr.subspan(hoare_idx + 1);
        
        // Tail recursion optimization reduces stack usage
        if (dna.tailRecElim) {
            while (true) {
                auto smaller_part = left_part;
                auto larger_part = right_part;
                
            if (left_part.size() >= right_part.size()) {
                    smaller_part = right_part;
                    larger_part = left_part;
                }
                
                quicksort_impl(smaller_part, dna, metrics, depth_remaining - 1);
                
                if (larger_part.size() <= 1) {
                    break;
                }

                arr = larger_part;
                pivot_val = select_pivot(arr, dna.pivot, metrics);
                last_idx = arr.size() - 1;
                
                for (size_t k = 0; k < arr.size(); ++k) {
                    if (arr[k] == pivot_val) {
                        do_swap(arr[k], arr[last_idx], metrics);
                                   break;
                    }
                }
                
                hoare_idx = partition_hoare(arr, pivot_val, metrics);
                left_part = arr.first(hoare_idx + 1);
                            right_part = arr.subspan(hoare_idx + 1);
                
                if (right_part.size() <= 1 && left_part.size() <= 1) {
                    break;
                }
                      }
        } else {
            quicksort_impl(left_part, dna, metrics, depth_remaining - 1);
            quicksort_impl(right_part, dna, metrics, depth_remaining - 1);
        }
    }
}

// Main entry point for quicksort algorithm
void quicksort(std::span<int> arr, const QSDNA& dna, Metrics& metrics) {
    int max_depth = 64;
    if (dna.depthCap > 0) {
        max_depth = dna.depthCap;
    }
    quicksort_impl(arr, dna, metrics, max_depth);
}