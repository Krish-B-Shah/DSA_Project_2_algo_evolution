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
