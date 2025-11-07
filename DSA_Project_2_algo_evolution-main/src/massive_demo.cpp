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
// mergesort
void mergesort_simple(std::vector<int>& arr, int left, int right, Metrics& m,
                      int run_threshold, bool iterative, bool reuse_buffer){
  if(left >= right) return;
  if(right - left + 1 <= run_threshold){
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
  if(!iterative){
    int mid = (left + right) / 2;
    mergesort_simple(arr, left, mid, m, run_threshold, iterative, reuse_buffer);
    mergesort_simple(arr, mid + 1, right, m, run_threshold, iterative, reuse_buffer);
    std::vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while(i <= mid && j <= right){
      if(++m.comparisons, arr[i] <= arr[j]){
        temp[k++] = arr[i++];
        m.swaps++;
      }else{
        temp[k++] = arr[j++];
        m.swaps++;
      }
    }
    while(i <= mid){ temp[k++] = arr[i++]; m.swaps++; }
    while(j <= right){ temp[k++] = arr[j++]; m.swaps++; }
    for(int idx = 0; idx < k; idx++){
      arr[left + idx] = temp[idx];
      m.swaps++;
    }
  }else{
    int n = right - left + 1;
    std::vector<int> aux;
    if(reuse_buffer) aux.resize(n);
    
    for(int width = run_threshold; width < n; width *= 2){
      if(!reuse_buffer) aux.resize(n);
      for(int i = left; i <= right; i += 2 * width){
        int mid = std::min(i + width - 1, right);
        int end = std::min(i + 2 * width - 1, right);
        
        for(int j = i; j <= end; j++){
          aux[j - left] = arr[j];
          m.swaps++;
        }
        
        int l = i, r = mid + 1, k = i;
        while(l <= mid && r <= end){
          if(++m.comparisons, aux[l - left] <= aux[r - left]){
            arr[k++] = aux[l++ - left];
            m.swaps++;
          }else{
            arr[k++] = aux[r++ - left];
            m.swaps++;
          }
        }
        while(l <= mid){ arr[k++] = aux[l++ - left]; m.swaps++; }
        while(r <= end){ arr[k++] = aux[r++ - left]; m.swaps++; }
      }
    }
  }
}

struct QSDNA {
  int pivot_choice;    // 0 is first, 1 is last, 2 is Median3
  int partition_type;  // 0=Lomuto, 1=Hoare
  int cutoff;          // insertion sort cutoff iss [8..64]
  int depth;           // depth cap is[16..128]
  bool tail;           // tail recursion elimination
  double fitness_ms;
  uint64_t comparisons;
  uint64_t swaps;
};
struct MSDNA {
  int run_threshold;   // [0..64]
  bool iterative;      // true=iterative, false=recursive
  bool reuse_buffer;   // reuse the buffer
  double fitness_ms;
  uint64_t comparisons;
  uint64_t swaps;
};
QSDNA create_random_qs_dna(std::mt19937& rng){
  QSDNA dna;
  dna.pivot_choice = rng() % 3;
  dna.partition_type = rng() % 2;
  dna.cutoff = 8 + (rng() % 57);
  dna.depth = 16 + (rng() % 113);
  dna.tail = (rng() % 2) == 1;
  return dna;
}
MSDNA create_random_ms_dna(std::mt19937& rng){
  MSDNA dna;
  dna.run_threshold = rng() % 65;
  dna.iterative = (rng() % 2) == 1;
  dna.reuse_buffer = (rng() % 2) == 1;
  return dna;
}
QSDNA evaluate_qs_dna(const QSDNA& dna, std::mt19937& rng){
  QSDNA result = dna;
  std::vector<int> testArray(100000);
  for(int i = 0; i < 100000; i++){
    testArray[i] = rng() % 1000000;
  }
  Metrics m{};
  auto start = std::chrono::high_resolution_clock::now();
  quicksort_simple(testArray, 0, 99999, m, dna.pivot_choice, dna.partition_type, dna.cutoff);
  auto end = std::chrono::high_resolution_clock::now();
  double ms = std::chrono::duration<double, std::milli>(end - start).count();
  result.fitness_ms = ms;
  result.comparisons = m.comparisons;
  result.swaps = m.swaps;
  return result;
}
MSDNA evaluate_ms_dna(const MSDNA& dna, std::mt19937& rng){
  MSDNA result = dna;
  std::vector<int> testArray(100000);
  for(int i = 0; i < 100000; i++){
    testArray[i] = rng() % 1000000;
  }
  Metrics m{};
  auto start = std::chrono::high_resolution_clock::now();
  mergesort_simple(testArray, 0, 99999, m, dna.run_threshold, dna.iterative, dna.reuse_buffer);
  auto end = std::chrono::high_resolution_clock::now();
  double ms = std::chrono::duration<double, std::milli>(end - start).count();
  result.fitness_ms = ms;
  result.comparisons = m.comparisons;
  result.swaps = m.swaps;
  return result;
}
QSDNA crossover_qs(const QSDNA& p1, const QSDNA& p2, std::mt19937& rng){
  QSDNA child;
  child.pivot_choice = (rng() % 2) ? p1.pivot_choice : p2.pivot_choice;
  child.partition_type = (rng() % 2) ? p1.partition_type : p2.partition_type;
  child.cutoff = (p1.cutoff + p2.cutoff) / 2 + (rng() % 5) - 2;
  child.cutoff = std::max(8, std::min(64, child.cutoff));
  child.depth = (p1.depth + p2.depth) / 2 + (rng() % 7) - 3;
  child.depth = std::max(16, std::min(128, child.depth));
  child.tail = (rng() % 2) ? p1.tail : p2.tail;
  return child;
}

MSDNA crossover_ms(const MSDNA& p1, const MSDNA& p2, std::mt19937& rng){
  MSDNA child;
  child.run_threshold = (p1.run_threshold + p2.run_threshold) / 2 + (rng() % 5) - 2;
  child.run_threshold = std::max(0, std::min(64, child.run_threshold));
  child.iterative = (rng() % 2) ? p1.iterative : p2.iterative;
  child.reuse_buffer = (rng() % 2) ? p1.reuse_buffer : p2.reuse_buffer;
  return child;
}

void mutate_qs(QSDNA& dna, std::mt19937& rng){
  if(rng() % 100 < 10) dna.pivot_choice = rng() % 3;
  if(rng() % 100 < 10) dna.partition_type = rng() % 2;
  if(rng() % 100 < 15){
    dna.cutoff += (rng() % 5) - 2;
    dna.cutoff = std::max(8, std::min(64, dna.cutoff));
  }
  if(rng() % 100 < 10){
    dna.depth += (rng() % 7) - 3;
    dna.depth = std::max(16, std::min(128, dna.depth));
  }
  if(rng() % 100 < 5) dna.tail = !dna.tail;
}

void mutate_ms(MSDNA& dna, std::mt19937& rng){
  if(rng() % 100 < 15){
    dna.run_threshold += (rng() % 5) - 2;
    dna.run_threshold = std::max(0, std::min(64, dna.run_threshold));
  }
  if(rng() % 100 < 10) dna.iterative = !dna.iterative;
  if(rng() % 100 < 10) dna.reuse_buffer = !dna.reuse_buffer;
}

std::string pivot_name(int p){
  if(p == 0) return "First";
  if(p == 1) return "Last";
  return "Median3";
}

std::string scheme_name(int s){
  return s == 0 ? "Lomuto" : "Hoare";
}

int main(int argc, char** argv){
  std::cout << "MASSIVE Algorithm Evolution - QuickSort + MergeSort!\n";
  std::cout << "Starting with 1 QS + 1 MS, evolving to 100k particles...\n";
  
  const int TOTAL_POPULATION = 1000;
  const int GENERATIONS = 10;
  const int ELITE_SIZE = 1000;
  
  std::string outfile = "data/logs/massive_evolution.csv";
  for(int i = 1; i < argc; i++){
    if(std::string(argv[i]) == "--out" && i + 1 < argc){
      outfile = argv[i + 1];
      break;
    }
  }
  
  std::ofstream csv(outfile);
  csv << "run_id,step,algo,opt,pivot,scheme,cutoff,depth,tail,run_threshold,iterative,reuse_buffer,fitness_ms,comparisons,swaps,n,trials_per_dist,dist_mask,pop_idx,temp\n";
  
  std::mt19937 rng(12345);
  std::string run_id = "massive_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()).count());
  std::vector<QSDNA> qs_ga_pop, qs_sa_pop;
  std::vector<MSDNA> ms_ga_pop, ms_sa_pop;
  
  QSDNA initial_qs_ga = create_random_qs_dna(rng);
  MSDNA initial_ms_ga = create_random_ms_dna(rng);
  QSDNA initial_qs_sa = create_random_qs_dna(rng);
  MSDNA initial_ms_sa = create_random_ms_dna(rng);
  
  qs_ga_pop.push_back(initial_qs_ga);
  ms_ga_pop.push_back(initial_ms_ga);
  qs_sa_pop.push_back(initial_qs_sa);
  ms_sa_pop.push_back(initial_ms_sa);
  for(int gen = 0; gen < GENERATIONS; gen++){
    int total_qs = qs_ga_pop.size() + qs_sa_pop.size();
    int total_ms = ms_ga_pop.size() + ms_sa_pop.size();
    std::cout << "\nGeneration " << gen << " - QS: " << total_qs 
              << " (GA:" << qs_ga_pop.size() << " SA:" << qs_sa_pop.size() << ")"
              << ", MS: " << total_ms 
              << " (GA:" << ms_ga_pop.size() << " SA:" << ms_sa_pop.size() << ")\n";
