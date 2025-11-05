#include "logging.hpp"
#include <iomanip>

void write_csv_header(std::ostream& os) {
  os << "run_id,step,algo,opt,"
     << "pivot,scheme,cutoff,depth,tail,"
     << "run_threshold,iterative,reuse_buffer,"
     << "fitness_ms,comparisons,swaps,"
     << "n,trials_per_dist,dist_mask,pop_idx,temp\n";
}
static const char* algo_name(Algo a) { return a==Algo::QS ? "QS" : "MS"; }
static const char* opt_name(Opt o) { return o==Opt::GA ? "GA" : "SA"; }
static const char* pivot_name(Pivot p) {
  switch(p){case Pivot::First:return "First";case Pivot::Last:return "Last";default:return "Median3";}
}
static const char* scheme_name(PartitionScheme s) {
  return s==PartitionScheme::Lomuto ? "Lomuto" : "Hoare";
}
void write_csv_row(std::ostream& os,
                   const std::string& run_id, int step,
                   Algo algo, Opt opt,
                   const QSDNA* qs, const MSDNA* ms,
                   const EvalResult& r,
                   std::size_t n, int trials_per_dist,
                   unsigned dist_mask, int pop_idx, double temp) {
  os << run_id << "," << step << "," << algo_name(algo) << "," << opt_name(opt) << ",";
  if (qs) {
    os << pivot_name(qs->pivot) << "," << scheme_name(qs->scheme) << ","
       << qs->insertionCutoff << "," << qs->depthCap << ","
       << (qs->tailRecElim?1:0) << ",";
  } else {
    os << ",,,,,"; // blank quicksort fields
  }
  if (ms) {
    os << ms->runThreshold << "," << (ms->iterative?1:0) << "," << (ms->reuseBuffer?1:0) << ",";
  } else {
    os << ",,,"; // blank mergesort fields
  }
  os << std::fixed << std::setprecision(6) << r.fitness_ms << ","
     << r.comparisons << "," << r.swaps << ","
     << n << "," << trials_per_dist << "," << dist_mask << ","
     << pop_idx << "," << temp << "\n";
}
