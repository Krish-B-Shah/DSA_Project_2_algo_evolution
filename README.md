# Algorithmic Evolution

Watch sorting algorithms evolve in real-time using genetic algorithms and simulated annealing. QuickSort and MergeSort optimize themselves on 100K+ element datasets.

## Quick Commands

**macOS/Linux:**
```bash
git clone https://github.com/Krish-B-Shah/DSA_Project_2_algo_evolution.git
cd DSA_Project_2_algo_evolution
mkdir -p build && cd build && cmake .. && cmake --build . --config Release && cd ..

./build/experiment --out=data/logs/sample.csv --pop=6 --gens=2 --n=100000 --trials-per-dist=1
#if this fails to work, resort to steps below:
# download Kaggle dataset first, then use it:
# python3 -m pip install --user kagglehub
# python3 scripts/download_kaggle.py
# ./build/experiment --out=data/logs/sample.csv --use-kaggle --kaggle-path=data/logs/viral_data.csv --pop=6 --gens=2 --n=100000 --trials-per-dist=1

python3 -m http.server 8010 --directory .
# Then open http://localhost:8010/viz/index.html
```

**Windows (PowerShell):**
```powershell
git clone https://github.com/Krish-B-Shah/DSA_Project_2_algo_evolution.git
cd DSA_Project_2_algo_evolution
mkdir build; cd build; cmake ..; cmake --build . --config Release; cd ..

# Demo: Kaggle dataset with 100K elements (auto-detects if dataset exists)
.\build\Release\experiment.exe --out=data/logs/sample.csv --pop=6 --gens=2 --n=100000 --trials-per-dist=1

python -m http.server 8010 --directory .
# Then open http://localhost:8010/viz/index.html
```

## What is this?

We built a system that uses evolutionary computation to optimize sorting algorithm parameters. Instead of manually tweaking parameters, we let genetic algorithms and simulated annealing evolve them automatically.

You can watch QuickSort and MergeSort mutate, compete, and evolve over generations to find better configurations. Everything is visualized in real-time with particles representing different algorithm configurations.

### How it works

Normal algorithm analysis is pretty static - you implement something, benchmark it, maybe tweak a few things, and that's it. We wanted to see what happens if you let algorithms evolve themselves.

- **Genetic Algorithms**: Simulate natural selection - mutate parameters, keep the best ones, evolve over generations
- **Simulated Annealing**: Inspired by metallurgy - slowly cool down solutions to find optimal configurations
- **100K+ Element Datasets**: Test on real-world scale data from Kaggle
- **Live Visualization**: Watch evolution happen in real-time
- **Leaderboards**: Track which algorithm-optimizer combinations perform best

## Features

- **QuickSort Evolution**: Evolves pivot selection, partitioning schemes, insertion sort cutoffs, depth limits, tail recursion elimination
- **MergeSort Evolution**: Evolves run thresholds, iterative vs recursive modes, buffer reuse strategies
- **Two Optimizers**: Compare Genetic Algorithms vs Simulated Annealing side by side
- **Interactive Visualization**: Real-time particle visualization with color coding (red = slow, green = fast)
- **Performance Metrics**: Track comparisons, swaps, execution time, space complexity

## Quick Start

### Prerequisites

**macOS:**
```bash
brew install cmake
python3 --version  # Verify Python 3 is installed
```

**Windows (PowerShell as Administrator):**
```powershell
# Install Chocolatey if not installed
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

choco install cmake -y
choco install visualstudio2022buildtools -y
python --version  # Verify Python 3 is installed
```

### Build

**macOS/Linux:**
```bash
mkdir -p build && cd build
cmake ..
cmake --build . --config Release
cd ..
```

**Windows:**
```powershell
mkdir build; cd build
cmake ..
cmake --build . --config Release
cd ..
```

**Note:** On Windows, if you get errors, try: `cmake -G "Visual Studio 17 2022" ..`

### Generate Sample Data

**macOS/Linux:**
```bash
# Demo: Uses Kaggle dataset with 100K elements if available, else generated arrays
./build/experiment --out=data/logs/sample.csv --pop=6 --gens=2 --n=100000 --trials-per-dist=1

# Quick test (1000 elements, generated arrays - fastest)
./build/experiment --out=data/logs/sample.csv --no-kaggle --pop=6 --gens=2 --n=1000 --trials-per-dist=1

# Full test (both algorithms, both optimizers - slower)
./build/experiment --out=data/logs/sample.csv --algo=both --opt=both --pop=6 --gens=2 --n=100000 --trials-per-dist=1

# Note: Defaults to --algo=qs --opt=ga for speed. Kaggle dataset auto-detected if data/logs/viral_data.csv exists.
```

**Windows:**
```powershell
.\build\Release\experiment.exe --out=data/logs/sample.csv --pop=6 --gens=2 --n=1000 --trials-per-dist=1
```

### Run Visualization

**macOS/Linux:**
```bash
python3 -m http.server 8010 --directory .
```

**Windows:**
```powershell
python -m http.server 8010 --directory .
```

Then open `http://localhost:8010/viz/index.html` in your browser.

1. Click "Choose File" and select `data/logs/sample.csv`
2. OR click "Demo (Kaggle)" if you downloaded the Kaggle dataset (optional)
3. Click "Play" to watch evolution in real-time

### Optional: Kaggle Dataset

**macOS/Linux:**
```bash
python3 -m pip install --user kagglehub
python3 scripts/download_kaggle.py
```

**Windows:**
```powershell
python -m pip install --user kagglehub
python scripts/download_kaggle.py
```

**Note:** This is optional. You can skip it and generate your own data.

## Usage

### Running Experiments

**macOS/Linux:**
```bash
# Quick test
./build/experiment --out=data/logs/my_results.csv --pop=20 --gens=5 --n=2000

# Full evolution on Kaggle dataset
./build/experiment --algo=both --opt=both --use-kaggle \
  --kaggle-path=./data/logs/viral_data.csv \
  --out=./data/logs/evolution.csv \
  --pop=500 --gens=10 --n=100000

# QuickSort only with Genetic Algorithm
./build/experiment --algo=qs --opt=ga --pop=100 --gens=5

# MergeSort only with Simulated Annealing
./build/experiment --algo=ms --opt=sa --pop=100 --gens=5
```

**Windows:**
```powershell
# Quick test
.\build\Release\experiment.exe --out=data/logs/my_results.csv --pop=20 --gens=5 --n=2000

# Full evolution on Kaggle dataset
.\build\Release\experiment.exe --algo=both --opt=both --use-kaggle --kaggle-path=.\data\logs\viral_data.csv --out=.\data\logs\evolution.csv --pop=500 --gens=10 --n=100000

# QuickSort only with Genetic Algorithm
.\build\Release\experiment.exe --algo=qs --opt=ga --pop=100 --gens=5

# MergeSort only with Simulated Annealing
.\build\Release\experiment.exe --algo=ms --opt=sa --pop=100 --gens=5
```

### Visualization Controls

- **Play/Pause**: Start/stop automatic generation progression
- **Prev/Next**: Step through generations manually
- **Speed Slider**: Adjust playback speed (0.1x - 4x)
- **Optimizer Filter**: Show All, GA only, or SA only
- **Algorithm Filter**: Show All, QuickSort only, or MergeSort only
- **Demo (Kaggle)**: Load pre-generated dataset

## How It Works

1. **Initialization**: Start with random algorithm parameter configurations (DNA)
2. **Evaluation**: Test each configuration on 100K element datasets
3. **Selection**: Keep the fastest configurations (fitness = execution time)
4. **Mutation/Crossover**: Create new configurations from the fittest ones
5. **Evolution**: Repeat for multiple generations
6. **Visualization**: Watch as particles evolve and converge toward optimal solutions

### Algorithm Parameters (DNA)

**QuickSort DNA:**
- `pivot_choice`: First, Last, or Median-of-3
- `partition_type`: Lomuto or Hoare scheme
- `cutoff`: Insertion sort threshold (8-64)
- `depth`: Recursion depth limit (16-128)
- `tail_recursion`: Enable/disable tail call elimination

**MergeSort DNA:**
- `run_threshold`: Natural run detection threshold (0-64)
- `iterative`: Iterative vs recursive implementation
- `reuse_buffer`: Reuse temporary buffer across calls

### Optimization Strategies

**Genetic Algorithm (GA):**
- Population-based search with mutation and crossover
- Selection pressure toward fitter solutions
- Best for exploring diverse solution spaces

**Simulated Annealing (SA):**
- Single-solution improvement with temperature-based acceptance
- Gradual "cooling" to find global optima
- Best for fine-tuning near-optimal solutions

## Troubleshooting

**Build errors on macOS:**
```bash
xcode-select --install
```

**Build errors on Windows:**
- Make sure Visual Studio Build Tools are installed
- Try using the Visual Studio Developer Command Prompt
- Run: `cmake -G "Visual Studio 17 2022" -A x64 ..`

**Port already in use:**
```bash
# Use a different port
python3 -m http.server 8080 --directory .
# Then open: http://localhost:8080/viz/index.html
```

**Can't find executable:**
- macOS/Linux: Check `./build/experiment`
- Windows: Check `.\build\Release\experiment.exe` or `.\build\Debug\experiment.exe`

## Acknowledgments

Uses the [Kaggle Benchmark Dataset for Sorting Algorithms](https://www.kaggle.com/datasets/bekiremirhanakay/benchmark-dataset-for-sorting-algorithms) for real-world testing on 100K+ element arrays.
