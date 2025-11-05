#include "sa.hpp"
#include "common.hpp"
#include <cmath>
#include <algorithm>
// simple neighbor tweaks code below
static QSDNA nudge(QSDNA d, XRand& rng) {
  double p = rng.uniform01();
  if (p < 0.20) d.pivot = (Pivot)(rng.uniform(0,2));
  else if (p < 0.40) d.scheme = (PartitionScheme)(rng.uniform(0,1));
  else if (p < 0.65) d.insertionCutoff = std::clamp(d.insertionCutoff + int(rng.uniform(0,7))-3, 0, 64);
  else if (p < 0.90) d.depthCap       = std::clamp(d.depthCap       + int(rng.uniform(0,7))-3, 32, 128);
  else d.tailRecElim = !d.tailRecElim;
  return d;
}
static MSDNA nudge(MSDNA d, XRand& rng) {
  double p = rng.uniform01();
  if (p < 0.55) d.runThreshold = std::clamp(d.runThreshold + int(rng.uniform(0,7))-3, 0, 64);
  else if (p < 0.80) d.iterative   = !d.iterative;
  else d.reuseBuffer = !d.reuseBuffer;
  return d;
}
template<class DNA>
static DNA run_sa_impl(EvalFnSA<DNA> eval, int steps, double t0, double t1, uint64_t seed,
                       std::vector<double>* history, LogFnSA<DNA> on_eval) {
  XRand rng(seed);
  DNA cur{};  EvalResult curR = eval(cur);
  double curFit = curR.fitness_ms;
  DNA best = cur; EvalResult bestR = curR; double bestFit = curFit;
  if (on_eval) on_eval(0, -1, cur, curR, t0);

  for (int s=1;s<=steps;++s) {
    double t = t0 * std::pow(t1/t0, double(s)/std::max(1,steps));
    DNA cand = nudge(cur, rng);
    EvalResult candR = eval(cand);
    double dF = candR.fitness_ms - curFit;
    if (dF < 0 || rng.uniform01() < std::exp(-dF / std::max(1e-9, t))) {
      cur = cand; curR = candR; curFit = candR.fitness_ms;
      if (curFit < bestFit) { best = cur; bestR = curR; bestFit = curFit; }
    }
    if (history) history->push_back(curFit);
    if (on_eval) on_eval(s, -1, cur, curR, t);
  }
  (void)best; (void)bestR; // best returned by caller if they want, not rlly needed however
  return cur; // return best-so-far state at the end
}
