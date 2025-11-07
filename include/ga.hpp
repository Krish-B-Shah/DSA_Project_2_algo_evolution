#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include "dna.hpp"
#include "evaluator.hpp"

template<class DNA>
using EvalFn = std::function<EvalResult(const DNA&)>;

template<class DNA>
using LogFn  = std::function<void(int step, int pop_idx, const DNA& dna, const EvalResult& r, double aux)>;
// aux is unused for GA (keep 0.0 for symmetry with SA)

template<class DNA>
DNA run_ga(EvalFn<DNA> eval, int pop=24, int gens=12, uint64_t seed=123,
           std::vector<std::vector<double>>* history = nullptr,
           LogFn<DNA> on_eval = nullptr);
