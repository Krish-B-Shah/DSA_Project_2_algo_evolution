#include "logging.hpp"
#include <iomanip>

void write_csv_header(std::ostream& os) {
  os << "run_id,step,algo,opt,"
     << "pivot,scheme,cutoff,depth,tail,"
     << "run_threshold,iterative,reuse_buffer,"
     << "fitness_ms,comparisons,swaps,"
     << "n,trials_per_dist,dist_mask,pop_idx,temp\n";
}
