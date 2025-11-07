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
  return cfg;
}
int main(int argc, char** argv){
  vector<string> args(argv+1, argv+argc);
  // Default to quick mode: single algorithm + single optimizer for speed
  string algo = argval(args, "--algo").value_or("qs"); // default to qs for speed
  string opt = argval(args, "--opt").value_or("ga"); // default to ga for speed
  int gens = stoi(argval(args, "--gens").value_or("2"));
  int pop = stoi(argval(args, "--pop").value_or("6"));
  int steps = stoi(argval(args, "--steps").value_or("20"));
  string out = argval(args, "--out").value_or("data/logs/run.csv");
  EvalConfig cfg = parse_cfg(args);
  // Note: Default n=100000 (from EvalConfig) - use --n=1000 for quick testing
  // Kaggle dataset requires --use-kaggle flag (default uses generated test arrays)
  
  // Show what we're running
  cerr << "Running: algo=" << algo << ", opt=" << opt 
       << ", pop=" << pop << ", gens=" << gens 
       << ", n=" << cfg.n << ", trials=" << cfg.trialsPerDist;
  if(cfg.useKaggle) {
    cerr << ", dataset=Kaggle (" << cfg.kaggleCsvPath << ")";
  } else {
    cerr << ", dataset=generated test arrays";
  }
  cerr << "\nOutput: " << out << "\n";
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
      cerr << "Running QuickSort + GA...\n";
      vector<vector<double>> hist;
      auto logger = [&](int step, int pop_idx, const QSDNA& dna, const EvalResult& r, double){
        write_csv_row(ofs, run_id, step, Algo::QS, Opt::GA, &dna, nullptr, r,
                      cfg.n, cfg.trialsPerDist, dmask, pop_idx, 0.0);
        if(pop_idx % 10 == 0 || pop_idx == 0) ofs.flush(); // flush periodically
        if(pop_idx == 0) cerr << "  Gen " << step << "/" << gens << " (fitness: " << r.fitness_ms << " ms)\n";
      };
      run_ga<QSDNA>(eval, pop, gens, cfg.masterSeed, &hist, logger);
      cerr << "QuickSort + GA completed.\n";
    }
    if(use_sa){
      cerr << "Running QuickSort + SA...\n";
      vector<double> hist;
      auto logger = [&](int step, int, const QSDNA& dna, const EvalResult& r, double temp){
        write_csv_row(ofs, run_id, step, Algo::QS, Opt::SA, &dna, nullptr, r,
                      cfg.n, cfg.trialsPerDist, dmask, -1, temp);
        if(step % 5 == 0 || step == 0) cerr << "  Step " << step << "/" << steps << " (fitness: " << r.fitness_ms << " ms)\n";
      };
      run_sa<QSDNA>(eval, steps, 1.0, 1e-3, cfg.masterSeed, &hist, logger);
      cerr << "QuickSort + SA completed.\n";
    }
  }
  if(run_ms){
    auto eval = [&](const MSDNA& d){ return eval_ms(d, cfg); };
    if(use_ga){
      cerr << "Running MergeSort + GA...\n";
      vector<vector<double>> hist;
      auto logger = [&](int step, int pop_idx, const MSDNA& dna, const EvalResult& r, double){
        write_csv_row(ofs, run_id, step, Algo::MS, Opt::GA, nullptr, &dna, r,
                      cfg.n, cfg.trialsPerDist, dmask, pop_idx, 0.0);
        if(pop_idx % 10 == 0 || pop_idx == 0) ofs.flush();
        if(pop_idx == 0) cerr << "  Gen " << step << "/" << gens << " (fitness: " << r.fitness_ms << " ms)\n";
      };
      run_ga<MSDNA>(eval, pop, gens, cfg.masterSeed, &hist, logger);
      cerr << "MergeSort + GA completed.\n";
    }
    if(use_sa){
      cerr << "Running MergeSort + SA...\n";
      vector<double> hist;
      auto logger = [&](int step, int, const MSDNA& dna, const EvalResult& r, double temp){
        write_csv_row(ofs, run_id, step, Algo::MS, Opt::SA, nullptr, &dna, r,
                      cfg.n, cfg.trialsPerDist, dmask, -1, temp);
        if(step % 5 == 0 || step == 0) cerr << "  Step " << step << "/" << steps << " (fitness: " << r.fitness_ms << " ms)\n";
      };
      run_sa<MSDNA>(eval, steps, 1.0, 1e-3, cfg.masterSeed, &hist, logger);
      cerr << "MergeSort + SA completed.\n";
    }
  }
  cerr << "Experiment completed! Results written to: " << out << "\n";
  return 0;
}
