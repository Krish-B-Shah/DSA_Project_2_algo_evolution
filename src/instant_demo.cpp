#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>

struct Metrics {
  uint64_t comparisons = 0;
  uint64_t swaps = 0;
};
void quicksort_simple(std::vector<int>& arr, int left, int right, Metrics& m) {
  if (left >= right) return;
  // select the pivot for use
  int pivot = arr[(left + right) / 2];
  int i = left, j = right;
  while (i <= j) {
    while (++m.comparisons && arr[i] < pivot) i++;
    while (++m.comparisons && arr[j] > pivot) j--;
    if (i <= j) {
      std::swap(arr[i], arr[j]);
      m.swaps++;
      i++; j--;
    }
  }
  quicksort_simple(arr, left, j, m);
  quicksort_simple(arr, i, right, m);
}
int main() {
  std::cout << "🚀 INSTANT Algorithm Evolution Demo!\n";
  
  std::ofstream csv("data/logs/instant_results.csv");
  csv << "generation,dna_id,fitness_ms,comparisons,swaps\n";
  
  std::mt19937 rng(12345);
  std::vector<int> testArray(500); // small erray to help with time 
  for (int gen = 0; gen < 5; gen++) {
    std::cout << "Generation " << gen << "...\n";
    for (int dna = 0; dna < 8; dna++) {
      // make random test data to utilize
      for (int i = 0; i < 500; i++) {
        testArray[i] = rng() % 1000;
      }
      // sorting
      Metrics m{};
      auto start = std::chrono::high_resolution_clock::now();
      quicksort_simple(testArray, 0, 499, m);
      auto end = std::chrono::high_resolution_clock::now();
      double ms = std::chrono::duration<double, std::milli>(end - start).count();
      csv << gen << "," << dna << "," << ms << "," << m.comparisons << "," << m.swaps << "\n";
      std::cout << "  DNA " << dna << ": " << ms << "ms, " << m.comparisons << " comparisons\n";
    }
  }
  csv.close();
  std::cout << "Done! Results saved to data/logs/instant_results.csv\n";
  std::cout << "Run: ./build/viz --in=data/logs/instant_results.csv\n";
  
  return 0;
}
