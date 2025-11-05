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
