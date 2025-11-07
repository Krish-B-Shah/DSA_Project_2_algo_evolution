# Algorithmic Evolution — Darwin for Sorting

**Project Leads:**
- **Lead A (Algorithms & Benchmarks):** Krish Shah
- **Lead B (Optimizers & Evaluation):** Sivan Reddy Pushpagirti  
- **Lead C (Visualization & Presentation):** Sparsh Mogha

## What We're Building

We're evolving the parameters ("DNA") of QuickSort and MergeSort using two optimization strategies (Genetic Algorithm vs Simulated Annealing), benchmarking them on 100,000-element datasets, and visualizing the "evolution" over time with a minimal 3D-ish particle display.

**Public Dataset:** [Benchmark Dataset for Sorting Algorithms](https://www.kaggle.com/datasets/bekiremirhanakay/benchmark-dataset-for-sorting-algorithms)

## Success Criteria

✅ Two non-trivial, from-scratch algorithms: QuickSort (with multiple pivot/partition/cutoff variants) and MergeSort (iterative, buffer reuse, tunables)

✅ Two optimizers from scratch: Genetic Algorithm (GA) and Simulated Annealing (SA), each used to tune the algorithm DNA

✅ Dataset ≥ 100,000 elements; multiple distributions; fixed seeds; repeatable

✅ Metrics: runtime (ms), comparisons, swaps (and optionally aux memory)

✅ CSV logs of every step/generation; visualization renders the evolution

✅ 5–7 min video and short write-up comparing GA vs SA and QS vs MS

## Team Roles & Timeline

### Roles (stick to these lanes to move fast)

**Lead A (Algorithms & Benchmarks)**
- Owns QuickSort, MergeSort, dataset generator, and the timing harness

**Lead B (Optimizers & Evaluation)**
- Owns GA/SA, fitness evaluation loop, and CSV logging

**Lead C (Visualization & Presentation)**
- Owns SFML viz, plots, video capture, and final slides

### Timeline (~18 hours total)

- **Hours 0–1:** Design sync (all leads)
- **Hours 1–5:** Core engines (QuickSort, MergeSort, evaluator scaffold)
- **Hours 5–9:** Optimizers (GA, SA, CSV logging)
- **Hours 9–13:** Visualization + polish
- **Hours 13–16:** Experiments (run GA/SA for QS & MS)
- **Hours 16–18:** Plots, video, 2–3 page write-up

## Tech Stack

- **Language:** C++20 (focus is from-scratch + speed)
- **Build:** CMake (Release -O2), optional Ninja
- **Visualization:** SFML (fastest to ship)
- **Editor:** VS Code with C++ & CMake Tools extensions

## Project Structure

```
algo-evolution/
├─ CMakeLists.txt
├─ include/
│  ├─ common.hpp           # RNG, timing helpers
│  ├─ metrics.hpp          # comparisons/swaps counters
│  ├─ datasets.hpp         # data generators, CSV importer
│  ├─ dna.hpp              # DNA structs for QS/MS
│  ├─ quicksort.hpp
│  ├─ mergesort.hpp
│  ├─ evaluator.hpp        # evaluate DNA on algo across distributions
│  ├─ ga.hpp               # GA interface
│  ├─ sa.hpp               # SA interface
│  └─ logging.hpp          # CSV writer
├─ src/
│  ├─ main_experiment.cpp  # CLI: runs GA/SA on QS/MS, writes CSV
│  ├─ quicksort.cpp
│  ├─ mergesort.cpp
│  ├─ datasets.cpp
│  ├─ evaluator.cpp
│  ├─ ga.cpp
│  ├─ sa.cpp
│  ├─ logging.cpp
│  └─ viz_sfml.cpp         # reads CSV and renders evolution
├─ data/
│  ├─ seeds/               # optional: pre-saved arrays
│  └─ logs/                # CSV outputs (one per run)
└─ README.md
```

## Executables

- **`experiment`** → runs GA/SA on QS/MS and writes CSV
- **`viz`** → reads the CSV and animates it

## Dataset Plan

**Minimum set (all at n = 100,000):**
- Uniform Random (baseline)
- Nearly Sorted (~1% random swaps)
- Reverse Sorted (stress pivot choice)
- Many Duplicates (tests partition behavior)

**Reproducibility:**
- Choose k = 5 fixed seeds per distribution
- Total runs per DNA evaluation: 4 distributions × 5 seeds = 20 sorts
- Fitness = geometric mean runtime across the 20 runs

## DNA (The Tunable "Genes")

### QuickSort DNA (QSDNA)
- **Pivot:** {First, Last, Median-of-3}
- **PartitionScheme:** {Lomuto, Hoare}
- **InsertionCutoff:** integer [0..64]
- **DepthCap:** integer [⌊log₂n⌋ .. ⌊2·log₂n⌋]
- **TailRecursionElim:** bool

### MergeSort DNA (MSDNA)
- **RunThreshold:** integer [0..64] (use insertion sort for small runs)
- **Iterative:** bool (prefer true)
- **ReuseBuffer:** bool (prefer true for memory locality)

## Fitness & Metrics

**Primary:** Runtime (ms) in Release -O2; geometric mean across all trials

**Secondary:** comparisons, swaps counters (from your code)

**Optional:** peak auxiliary memory (esp. MergeSort buffer)

*Why geometric mean?* It dampens the effect of outliers (one slow run won't dominate) and treats multiplicative timing differences fairly.

## Optimizers

### Genetic Algorithm (GA)
- Population: ~24 DNAs (start random within bounds)
- Selection: tournament size 3
- Crossover: 1-point or per-gene choose-parent
- Mutation: low probability per gene
- Elitism: keep best 1–2
- Generations: ~12–15

### Simulated Annealing (SA)
- Start: random DNA
- Neighbor: change one random gene
- Schedule: T decreases from 1.0 → 0.001 over ~250 steps
- Accept: if better, always; if worse, with probability exp(-(Δ)/T)

## Command-Line Interface

### experiment flags:
```
--algo=qs|ms
--opt=ga|sa
--n=100000
--gens=15 (GA only)
--pop=24 (GA only)
--steps=250 (SA only)
--trials-per-dist=5
--seeds=12345 (master seed)
--out=data/logs/run_<timestamp>.csv
```

### viz flags:
```
--in=data/logs/run_*.csv
--speed=1.0
--mode=best-only|all-points
```

## Test Plan

- **Correctness:** small arrays (n=100), random seeds; assert `std::is_sorted`
- **Counters:** ensure comparisons/swaps increment correctly
- **Performance sanity:** one QS and one MS run at n=100k in Release build
- **GA/SA smoke:** tiny runs (GA gens=3 pop=8; SA steps=50) to verify logs & viz

## Experiment Design

1. **Tuning QuickSort** with GA (gens=12 pop=24) and SA (steps=250)
2. **Tuning MergeSort** with GA and SA with the same budgets
3. **Compare:**
   - GA vs SA: time to best, variance across 3 seeds
   - QuickSort vs MergeSort: best configurations by distribution

## Deliverables

- ✅ Source repo with experiment and viz executables
- ✅ README with build/run instructions and CLI examples
- ✅ `data/logs/*.csv` with runs for QS/MS × GA/SA
- ✅ 2–3 plots: time by distribution, GA vs SA convergence, comps vs swaps
- ✅ 60–90s visualization video (screen capture)
- ✅ 2–3 page PDF write-up (question → method → results → discussion)

## Build Instructions

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
```

## Run Instructions

```bash
# Run an experiment
./experiment --algo=qs --opt=ga --n=100000 --gens=15 --out=data/logs/run_ga_qs.csv

# Visualize the results
./viz --in=data/logs/run_ga_qs.csv --speed=1.0
```

## Analysis Questions

- Which search strategy (GA or SA) finds better tunings under the same compute budget?
- How do evolved configurations compare to textbook defaults?
- Which algorithm parameters matter most for different input distributions?

## Key Concepts (Quick Reference)

**Genetic Algorithm (GA):** Make a group of random DNAs. Test each. Keep the best. Make kids by mixing parents. Add tiny mutations. Repeat. Nature's way of hill-climbing with a crowd.

**Simulated Annealing (SA):** Start somewhere. Try small changes. If they help, keep them; if they hurt, sometimes keep them anyway—especially early—so you don't get stuck. Gradually become pickier until you settle on the best you've seen.

**Geometric Mean:** When times vary a lot, averaging can be skewed by one slow run. Multiplying and taking the nth root (geometric mean) treats ratios fairly and tames outliers.

---

**Project Status:** 🚧 In Progress

