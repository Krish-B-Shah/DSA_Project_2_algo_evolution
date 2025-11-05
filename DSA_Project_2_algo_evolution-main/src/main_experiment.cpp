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
