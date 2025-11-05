#include "ga.hpp"
#include "common.hpp"
#include <algorithm>
// mutations and crossover helpers 
template<class DNA> static DNA mutateDNA(DNA d, XRand& rng);
template<> QSDNA mutateDNA(QSDNA d, XRand& rng) {
  if (rng.uniform01() < 0.20) d.pivot = (Pivot) (rng.uniform(0,2));
  if (rng.uniform01() < 0.20) d.scheme = (PartitionScheme) (rng.uniform(0,1));
  if (rng.uniform01() < 0.40) d.insertionCutoff = std::clamp(d.insertionCutoff + int(rng.uniform(0,7))-3, 0, 64);
  if (rng.uniform01() < 0.40) d.depthCap       = std::clamp(d.depthCap       + int(rng.uniform(0,7))-3, 32, 128);
  if (rng.uniform01() < 0.30) d.tailRecElim = !d.tailRecElim;
  return d;
}
template<> MSDNA mutateDNA(MSDNA d, XRand& rng) {
  if (rng.uniform01() < 0.40) d.runThreshold = std::clamp(d.runThreshold + int(rng.uniform(0,7))-3, 0, 64);
  if (rng.uniform01() < 0.20) d.iterative   = !d.iterative;
  if (rng.uniform01() < 0.20) d.reuseBuffer = !d.reuseBuffer;
  return d;
}
template<class DNA> static DNA crossover(const DNA& a, const DNA& b, XRand& rng);
template<> QSDNA crossover(const QSDNA& a, const QSDNA& b, XRand&) {
  QSDNA c = a;
  if (XRand(0).uniform01() < 0.5) c.pivot = b.pivot;
  if (XRand(0).uniform01() < 0.5) c.scheme = b.scheme;
  if (XRand(0).uniform01() < 0.5) c.insertionCutoff = b.insertionCutoff;
  if (XRand(0).uniform01() < 0.5) c.depthCap = b.depthCap;
  if (XRand(0).uniform01() < 0.5) c.tailRecElim = b.tailRecElim;
  return c;
}
template<> MSDNA crossover(const MSDNA& a, const MSDNA& b, XRand&) {
  MSDNA c = a;
  if (XRand(0).uniform01() < 0.5) c.runThreshold = b.runThreshold;
  if (XRand(0).uniform01() < 0.5) c.iterative = b.iterative;
  if (XRand(0).uniform01() < 0.5) c.reuseBuffer = b.reuseBuffer;
  return c;
}
// ga evaluator implementationn
template<class DNA>
static DNA run_ga_impl(EvalFn<DNA> eval, int pop, int gens, uint64_t seed,
                       std::vector<std::vector<double>>* history,
                       LogFn<DNA> on_eval) {
  XRand rng(seed);
  struct Item { DNA dna; double fit; EvalResult r; };
  std::vector<Item> P(pop);
  auto rand_dna = [&]() -> DNA { DNA d{}; return mutateDNA(d, rng); };
  // population  
  for (int i=0;i<pop;++i) {
    P[i].dna = rand_dna();
    P[i].r   = eval(P[i].dna);
    P[i].fit = P[i].r.fitness_ms;
    if (on_eval) on_eval(0, i, P[i].dna, P[i].r, 0.0);
  }
  auto tournament_idx = [&](int k)->int{
    int best = rng.uniform(0, pop-1);
    for (int i=1;i<k;++i) {
      int j = rng.uniform(0, pop-1);
      if (P[j].fit < P[best].fit) best = j;
    }
    return best;
  };
  for (int g=1; g<=gens; ++g) {
    std::vector<Item> next; next.reserve(pop);
    std::sort(P.begin(), P.end(), [](auto& x, auto& y){ return x.fit < y.fit; });
    next.push_back(P[0]);
    while ((int)next.size() < pop) {
      int i1 = tournament_idx(3), i2 = tournament_idx(3);
      DNA child = crossover<DNA>(P[i1].dna, P[i2].dna, rng);
      if (rng.uniform01() < 0.7) child = mutateDNA<DNA>(child, rng);
      EvalResult r = eval(child);
      next.push_back({child, r.fitness_ms, r});
    }
    P.swap(next);
    if (history) {
      std::vector<double> genfits; genfits.reserve(pop);
      for (auto& it: P) genfits.push_back(it.fit);
      history->push_back(std::move(genfits));
    }
    if (on_eval) {
      for (int i=0;i<pop;++i) on_eval(g, i, P[i].dna, P[i].r, 0.0);
    }
  }

  std::sort(P.begin(), P.end(), [](auto& x, auto& y){ return x.fit < y.fit; });
  return P[0].dna;
}
template<class DNA>
DNA run_ga(EvalFn<DNA> eval, int pop, int gens, uint64_t seed,
           std::vector<std::vector<double>>* history,
           LogFn<DNA> on_eval) {
  return run_ga_impl<DNA>(eval, pop, gens, seed, history, on_eval);
}
template QSDNA run_ga<QSDNA>(EvalFn<QSDNA>, int, int, uint64_t, std::vector<std::vector<double>>*, LogFn<QSDNA>);
template MSDNA run_ga<MSDNA>(EvalFn<MSDNA>, int, int, uint64_t, std::vector<std::vector<double>>*, LogFn<MSDNA>);
