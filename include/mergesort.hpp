#pragma once
#include <span>
#include <vector>
#include "metrics.hpp"
#include "dna.hpp"

void mergesort(std::span<int> a, const MSDNA& dna, Metrics& m);
