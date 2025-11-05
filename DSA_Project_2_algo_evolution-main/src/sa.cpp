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
