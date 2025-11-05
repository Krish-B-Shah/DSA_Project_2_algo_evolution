#pragma once
#include <span>
#include "metrics.hpp"
#include "dna.hpp"

void quicksort(std::span<int> a, const QSDNA& dna, Metrics& m);
