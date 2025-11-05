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
    cfg.kaggleCsvPath = argval(args, "--kaggle-path").value_or("data/kaggle.csv");
  }
  return cfg;
}
int main(int argc, char** argv){
  vector<string> args(argv+1, argv+argc);
  string algo = argval(args, "--algo").value_or("both"); // default is both 
  string opt = argval(args, "--opt").value_or("both");
  int gens = stoi(argval(args, "--gens").value_or("2"));
  int pop = stoi(argval(args, "--pop").value_or("6"));
  int steps = stoi(argval(args, "--steps").value_or("20"));
  string out = argval(args, "--out").value_or("data/logs/run.csv");
  EvalConfig cfg = parse_cfg(args);
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
