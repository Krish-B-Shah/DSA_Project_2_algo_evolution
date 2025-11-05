#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include "dna.hpp"
#include "evaluator.hpp"

template<class DNA>
using EvalFnSA = std::function<EvalResult(const DNA&)>;

template<class DNA>
using LogFnSA  = std::function<void(int step, int pop_idx, const DNA& dna, const EvalResult& r, double temperature)>;

template<class DNA>
DNA run_sa(EvalFnSA<DNA> eval, int steps=250, double t0=1.0, double t1=1e-3, uint64_t seed=123,
           std::vector<double>* history = nullptr,
           LogFnSA<DNA> on_eval = nullptr);
