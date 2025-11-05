#pragma once
#include <ostream>
#include <string>
#include "dna.hpp"
#include "evaluator.hpp"

enum class Algo { QS, MS };
enum class Opt  { GA, SA };

void write_csv_header(std::ostream& os);

void write_csv_row(std::ostream& os,
                   const std::string& run_id, int step,
                   Algo algo, Opt opt,
                   const QSDNA* qs, const MSDNA* ms,
                   const EvalResult& r,
                   std::size_t n, int trials_per_dist,
                   unsigned dist_mask, // bitmask of distributions used
                   int pop_idx, double temp);
