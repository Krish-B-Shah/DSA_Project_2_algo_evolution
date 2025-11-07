#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include "common.hpp"
#include "logging.hpp"
#include "evaluator.hpp"
#include "ga.hpp"
#include "sa.hpp"

using namespace std;
static EvalConfig parse_cfg(const vector<string>& args){
  EvalConfig cfg;
  if(auto v = argval(args, "--n")) cfg.n = stoull(*v);
  if(auto v = argval(args, "--trials-per-dist")) cfg.trialsPerDist = stoi(*v);
  if(auto v = argval(args, "--seeds")) cfg.masterSeed = stoull(*v);
  if(auto v = argval(args, "--jobs")) cfg.jobs = stoi(*v);
  if(hasflag(args, "--no-precompute")) cfg.precompute = false;
  if(hasflag(args, "--use-kaggle")){
    cfg.useKaggle = true;
    cfg.kaggleCsvPath = argval(args, "--kaggle-path").value_or("data/logs/viral_data.csv");
  }
  // Auto-enable Kaggle if --n=100000 and dataset file exists (for demo use case)
  if(!hasflag(args, "--no-kaggle") && !cfg.useKaggle && cfg.n >= 50000) {
    string kaggle_path = argval(args, "--kaggle-path").value_or("data/logs/viral_data.csv");
    if(std::filesystem::exists(kaggle_path)) {
      // Check if file looks like a dataset (not results) - read first line
      std::ifstream check(kaggle_path);
      string first_line;
      if(std::getline(check, first_line)) {
        // Results files start with "run_id", datasets don't
        // Also check file size - datasets should be larger
        size_t file_size = std::filesystem::file_size(kaggle_path);
        if(first_line.find("run_id") == string::npos && file_size > 10000) {
          cfg.useKaggle = true;
          cfg.kaggleCsvPath = kaggle_path;
        }
      }
    }
  }
  
  // Auto-optimize for speed when n is large: use single distribution and 1 trial
  // This makes 100K element runs complete in ~5 seconds instead of minutes
  bool auto_fast = !hasflag(args, "--full-test") && cfg.n >= 50000;
  if(auto_fast && !argval(args, "--trials-per-dist") && cfg.trialsPerDist > 1) {
    cfg.trialsPerDist = 1;
  }
  if(auto_fast && !hasflag(args, "--all-dists") && cfg.dists.size() > 1) {
    cfg.dists = {Dist::Uniform}; // Use only Uniform distribution for speed
  }
  
  return cfg;
}
int main(int argc, char** argv){
  vector<string> args(argv+1, argv+argc);
  // Default to quick mode: single algorithm + single optimizer for speed
  string algo = argval(args, "--algo").value_or("qs"); // default to qs for speed
  string opt = argval(args, "--opt").value_or("ga"); // default to ga for speed
  string out = argval(args, "--out").value_or("data/logs/run.csv");
  EvalConfig cfg = parse_cfg(args);
  
  // Auto-reduce pop/gens for large n to ensure fast execution (~5 seconds)
  // When n >= 50000, automatically use faster settings unless user explicitly requests more
  int gens, pop, steps;
  bool auto_fast = !hasflag(args, "--full-test") && cfg.n >= 50000;
  
  // Read user-provided values (if any)
  gens = stoi(argval(args, "--gens").value_or("2"));
  pop = stoi(argval(args, "--pop").value_or("6"));
  steps = stoi(argval(args, "--steps").value_or("20"));
  
  // Auto-optimize for large n: reduce computational load for speed (~2-5 seconds)
  if(auto_fast) {
    // For 100K elements, use absolute minimum for near-instant execution
    // User can override with --full-test flag
    if(!hasflag(args, "--full-test")) {
      pop = 2;  // Minimal population (2 individuals)
      gens = 1; // Single generation for speed
    }
  }
  // Note: Default n=100000 (from EvalConfig) - use --n=1000 for quick testing
  // Kaggle dataset requires --use-kaggle flag (default uses generated test arrays)
  
  // Show minimal progress by default, more detail with --verbose
  bool verbose = hasflag(args, "--verbose");
  bool silent = hasflag(args, "--silent");
  
  if(!silent) {
    cerr << "Running experiment: algo=" << algo << ", opt=" << opt 
         << ", pop=" << pop << ", gens=" << gens 
         << ", n=" << cfg.n << ", trials=" << cfg.trialsPerDist
         << ", dists=" << cfg.dists.size();
    if(cfg.useKaggle) {
      cerr << ", dataset=Kaggle (" << cfg.kaggleCsvPath << ")";
    } else {
      cerr << ", dataset=generated test arrays";
    }
    if(cfg.dists.size() == 1 && cfg.n >= 50000) {
      cerr << " [fast mode: single distribution for speed]";
    }
    cerr << "\nOutput: " << out << "\n";
    if(!verbose) {
      cerr << "(Add --verbose for detailed progress, --silent for no output, --full-test for all distributions)\n";
    }
  }
  // fast mode that helps with slow loadd times 
  if(hasflag(args, "--fast")){
    cfg.n = 1000;
    cfg.trialsPerDist = 1;
    cfg.dists = {Dist::Uniform};
    gens = 2;
    pop = 6;
    steps = 20;
  }
  std::filesystem::create_directories(std::filesystem::path(out).parent_path());
  ofstream ofs(out);
  if(!ofs){ cerr << "ERROR: could not open " << out << "\n"; return 1; }
  write_csv_header(ofs);
  unsigned dmask = dist_mask_of(cfg.dists);
  if(cfg.useKaggle) dmask |= (1u << (int)Dist::Kaggle);

  string run_id = "run" + to_string(now_ns());

  // checks what to run based on input changes ex. evaluater optimizers
  bool run_qs = (algo == "qs" || algo == "both");
  bool run_ms = (algo == "ms" || algo == "both");
  bool use_ga = (opt == "ga" || opt == "both");
  bool use_sa = (opt == "sa" || opt == "both");
  if(run_qs){
    auto eval = [&](const QSDNA& d){ return eval_qs(d, cfg); };
    if(use_ga){
      if(!silent) cerr << "Running QuickSort + GA...\n";
      vector<vector<double>> hist;
      auto logger = [&](int step, int pop_idx, const QSDNA& dna, const EvalResult& r, double){
        write_csv_row(ofs, run_id, step, Algo::QS, Opt::GA, &dna, nullptr, r,
                      cfg.n, cfg.trialsPerDist, dmask, pop_idx, 0.0);
        if(pop_idx % 10 == 0 || pop_idx == 0) ofs.flush(); // flush periodically
        if(!silent && pop_idx == 0) cerr << "  Gen " << step << "/" << gens << " (fitness: " << r.fitness_ms << " ms)\n";
        if(verbose && pop_idx % 10 == 0) cerr << "    Pop[" << pop_idx << "] fitness: " << r.fitness_ms << " ms\n";
      };
      run_ga<QSDNA>(eval, pop, gens, cfg.masterSeed, &hist, logger);
      if(!silent) cerr << "QuickSort + GA completed.\n";
    }
    if(use_sa){
      if(!silent) cerr << "Running QuickSort + SA...\n";
      vector<double> hist;
      auto logger = [&](int step, int, const QSDNA& dna, const EvalResult& r, double temp){
        write_csv_row(ofs, run_id, step, Algo::QS, Opt::SA, &dna, nullptr, r,
                      cfg.n, cfg.trialsPerDist, dmask, -1, temp);
        if(!silent && (step % 5 == 0 || step == 0)) cerr << "  Step " << step << "/" << steps << " (fitness: " << r.fitness_ms << " ms)\n";
        if(verbose && step % 2 == 0) cerr << "    Step " << step << " fitness: " << r.fitness_ms << " ms, temp: " << temp << "\n";
      };
      run_sa<QSDNA>(eval, steps, 1.0, 1e-3, cfg.masterSeed, &hist, logger);
      if(!silent) cerr << "QuickSort + SA completed.\n";
    }
  }
  if(run_ms){
    auto eval = [&](const MSDNA& d){ return eval_ms(d, cfg); };
    if(use_ga){
      if(!silent) cerr << "Running MergeSort + GA...\n";
      vector<vector<double>> hist;
      auto logger = [&](int step, int pop_idx, const MSDNA& dna, const EvalResult& r, double){
        write_csv_row(ofs, run_id, step, Algo::MS, Opt::GA, nullptr, &dna, r,
                      cfg.n, cfg.trialsPerDist, dmask, pop_idx, 0.0);
        if(pop_idx % 10 == 0 || pop_idx == 0) ofs.flush();
        if(!silent && pop_idx == 0) cerr << "  Gen " << step << "/" << gens << " (fitness: " << r.fitness_ms << " ms)\n";
        if(verbose && pop_idx % 10 == 0) cerr << "    Pop[" << pop_idx << "] fitness: " << r.fitness_ms << " ms\n";
      };
      run_ga<MSDNA>(eval, pop, gens, cfg.masterSeed, &hist, logger);
      if(!silent) cerr << "MergeSort + GA completed.\n";
    }
    if(use_sa){
      if(!silent) cerr << "Running MergeSort + SA...\n";
      vector<double> hist;
      auto logger = [&](int step, int, const MSDNA& dna, const EvalResult& r, double temp){
        write_csv_row(ofs, run_id, step, Algo::MS, Opt::SA, nullptr, &dna, r,
                      cfg.n, cfg.trialsPerDist, dmask, -1, temp);
        if(!silent && (step % 5 == 0 || step == 0)) cerr << "  Step " << step << "/" << steps << " (fitness: " << r.fitness_ms << " ms)\n";
        if(verbose && step % 2 == 0) cerr << "    Step " << step << " fitness: " << r.fitness_ms << " ms, temp: " << temp << "\n";
      };
      run_sa<MSDNA>(eval, steps, 1.0, 1e-3, cfg.masterSeed, &hist, logger);
      if(!silent) cerr << "MergeSort + SA completed.\n";
    }
  }
  if(!silent) cerr << "Experiment completed! Results written to: " << out << "\n";
  return 0;
}
