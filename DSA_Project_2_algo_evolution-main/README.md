# 🧬 Algorithmic Evolution — Darwin for Sorting Algorithms

<div align="center">

**Watch sorting algorithms evolve in real-time using genetic algorithms and simulated annealing. See QuickSort and MergeSort optimize themselves on 100K+ element datasets.**

[![Visualization](https://img.shields.io/badge/Visualization-Live-green)](http://localhost:8010/viz/index.html)
[![C++](https://img.shields.io/badge/C++-17-blue)](https://en.cppreference.com/)
[![License](https://img.shields.io/badge/License-MIT-yellow)](LICENSE)

**[🚀 Try it Now](#quick-start) | [📊 Screenshots](#features) | [🧪 How It Works](#how-it-works)**

</div>

### ⚡ TL;DR - Quick Commands

**macOS/Linux:**
```bash
git clone https://github.com/Krish-B-Shah/DSA_Project_2_algo_evolution.git
cd DSA_Project_2_algo_evolution
mkdir -p build && cd build && cmake .. && cmake --build . --config Release && cd ..
./build/experiment --out=data/logs/sample.csv --pop=6 --gens=2 --n=1000 --trials-per-dist=1
python3 -m http.server 8010 --directory .
# Then open http://localhost:8010/viz/index.html
```

**Windows (PowerShell):**
```powershell
git clone https://github.com/Krish-B-Shah/DSA_Project_2_algo_evolution.git
cd DSA_Project_2_algo_evolution
mkdir build; cd build; cmake ..; cmake --build . --config Release; cd ..
.\build\Release\experiment.exe --out=data/logs/sample.csv --pop=6 --gens=2 --n=1000 --trials-per-dist=1
python -m http.server 8010 --directory .
# Then open http://localhost:8010/viz/index.html
```

---

## 🌟 What Is This?

Imagine if Charles Darwin studied sorting algorithms instead of finches. That's this project.

**Algorithmic Evolution** is an interactive visualization that evolves sorting algorithm parameters using evolutionary computation. Watch as QuickSort and MergeSort **mutate, compete, and evolve** to become faster, more efficient versions of themselves—all visualized in real-time as particles in a dynamic 3D-like space.

### 🎯 The Insane Idea

Traditional algorithm analysis is static. You study one implementation, measure it, and move on. But what if algorithms could **evolve**?

- 🧬 **Genetic Algorithms (GA)**: Simulate natural selection—mutate parameters, keep the fittest, evolve over generations
- 🔥 **Simulated Annealing (SA)**: Inspired by metallurgy—slowly "cool down" solutions to find global optima
- 📊 **100K+ Element Datasets**: Real-world scale testing on massive datasets from Kaggle
- 🎨 **Live Visualization**: Watch evolution happen in real-time with buttery-smooth 60fps rendering
- 🏆 **Leaderboards**: Track which algorithm-optimizer combinations dominate

This isn't just optimization—it's **algorithmic evolution in action**.

---

## 🎬 Features

### 🧬 Evolutionary Optimization
- **QuickSort Evolution**: Evolves pivot selection, partitioning schemes, cutoffs, depth limits, tail recursion
- **MergeSort Evolution**: Evolves run thresholds, iterative vs recursive, buffer reuse strategies
- **Dual Optimizers**: Compare Genetic Algorithms vs Simulated Annealing head-to-head

### 📊 Interactive Visualization
- **Real-Time Evolution**: Watch particles (algorithm configurations) evolve across generations
- **Fitness-Based Coloring**: Red (poor) → Yellow (medium) → Green (top-tier)
- **Progressive Reveal**: Start with 2 seed algorithms, watch them evolve into thousands
- **Performance Metrics**: Track comparisons, swaps, execution time, space complexity
- **Dynamic Leaderboards**: See which algorithm-optimizer combos dominate

### 🚀 Performance
- **Buttery Smooth 60fps**: Optimized rendering handles 100K+ particles effortlessly
- **100K Element Sorting**: Test on real-world scale datasets
- **Interactive Controls**: Play, pause, step through generations, adjust speed
- **Filter & Explore**: Focus on specific algorithms or optimizers

---

## 🚀 Quick Start

### Step 1: Prerequisites

**On macOS:**
```bash
# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake and build tools
brew install cmake

# Python 3 should already be installed, but verify:
python3 --version
```

**On Windows (using PowerShell):**
```powershell
# Install Chocolatey (if not installed - run as Administrator)
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install CMake and Visual Studio Build Tools
choco install cmake -y
choco install visualstudio2022buildtools -y

# Verify Python 3 is installed
python --version
```

### Step 2: Clone and Setup

**macOS/Linux:**
```bash
git clone https://github.com/Krish-B-Shah/DSA_Project_2_algo_evolution.git
cd DSA_Project_2_algo_evolution
```

**Windows (PowerShell):**
```powershell
git clone https://github.com/Krish-B-Shah/DSA_Project_2_algo_evolution.git
cd DSA_Project_2_algo_evolution
```

### Step 3: Build the Project

**macOS/Linux:**
```bash
mkdir -p build
cd build
cmake ..
cmake --build . --config Release
cd ..
```

**Windows (PowerShell):**
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
```

**Note:** On Windows, if you get errors, you may need to specify the generator:
```powershell
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

### Step 4: Install Python Dependencies (Optional - for Kaggle dataset)

**macOS/Linux:**
```bash
# Option 1: Using pip (recommended)
python3 -m pip install --user kagglehub

# Option 2: If you're using conda
conda install -c conda-forge kagglehub

# Option 3: If you get permission errors, use a virtual environment
python3 -m venv venv
source venv/bin/activate
pip install kagglehub
```

**Windows (PowerShell):**
```powershell
# Option 1: Using pip (recommended)
python -m pip install --user kagglehub

# Option 2: If you're using conda
conda install -c conda-forge kagglehub

# Option 3: If you get permission errors, use a virtual environment
python -m venv venv
.\venv\Scripts\Activate.ps1
pip install kagglehub
```

### Step 5: Download Kaggle Dataset (Optional)

**macOS/Linux:**
```bash
python3 scripts/download_kaggle.py
```

**Windows:**
```powershell
python scripts/download_kaggle.py
```

**Note:** If you don't have kagglehub installed or don't want to download the dataset, you can skip this step. The visualization will work with sample data you generate yourself.

### Step 6: Generate Sample Data (Quick Test)

**macOS/Linux:**
```bash
# Quick test with small dataset (takes ~1-2 minutes)
./build/experiment --out=data/logs/sample.csv --pop=6 --gens=2 --n=1000 --trials-per-dist=1
```

**Windows:**
```powershell
# Quick test with small dataset (takes ~1-2 minutes)
.\build\Release\experiment.exe --out=data/logs/sample.csv --pop=6 --gens=2 --n=1000 --trials-per-dist=1
```

**Note:** The `data/logs/` directory will be created automatically. On Windows, the executable is in `build\Release\` instead of `build/`.

### Step 7: Start the Web Server

**macOS/Linux:**
```bash
# Start HTTP server on port 8010
python3 -m http.server 8010 --directory .
```

**Windows:**
```powershell
# Start HTTP server on port 8010
python -m http.server 8010 --directory .
```

**Note:** If port 8010 is busy, use a different port (e.g., 8000, 8080) and update the URL below.

### Step 8: Open the Visualization

1. **Open your browser** and navigate to:
   ```
   http://localhost:8010/viz/index.html
   ```

2. **Load your data:**
   - Click **"Choose File"** and select `data/logs/sample.csv`
   - OR click **"Demo (Kaggle)"** if you downloaded the Kaggle dataset
   - OR paste a CSV URL in the text field and click **"Load"**

3. **Start exploring:**
   - Click **"Play"** to watch evolution in real-time
   - Use **"Prev/Next"** to step through generations
   - Adjust **"Speed"** slider to control playback
   - Click on particles to see detailed DNA parameters

### Troubleshooting

**Build errors on macOS:**
```bash
# Make sure you have Xcode command line tools
xcode-select --install
```

**Build errors on Windows:**
- Make sure Visual Studio Build Tools are installed
- Try using the Visual Studio Developer Command Prompt instead of PowerShell
- Run: `cmake -G "Visual Studio 17 2022" -A x64 ..`

**Python/kagglehub errors:**
- Skip the Kaggle dataset step if it's causing issues
- You can generate your own data using Step 6 above
- The visualization works with any CSV in the correct format

**Port already in use:**
```bash
# macOS/Linux - use different port
python3 -m http.server 8080 --directory .

# Windows
python -m http.server 8080 --directory .
# Then open: http://localhost:8080/viz/index.html
```

**Can't find executable:**
- On macOS/Linux: Check `./build/experiment`
- On Windows: Check `.\build\Release\experiment.exe` or `.\build\Debug\experiment.exe`

---

## 🧪 How It Works

### The Evolutionary Process

1. **Initialization**: Start with random algorithm parameter configurations (DNA)
2. **Evaluation**: Test each configuration on 100K element datasets
3. **Selection**: Keep the fastest configurations (fitness = execution time)
4. **Mutation/Crossover**: Create new configurations from the fittest
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
- Population-based search
- Mutation and crossover operators
- Selection pressure toward fitter solutions
- Best for: Exploring diverse solution spaces

**Simulated Annealing (SA):**
- Single-solution improvement
- Temperature-based acceptance of worse solutions
- Gradual "cooling" to find global optima
- Best for: Fine-tuning near-optimal solutions

---

## 📖 Usage

### Running Experiments

Generate your own evolutionary data:

**macOS/Linux:**
```bash
# Quick test (small dataset, fast)
./build/experiment --out=data/logs/my_results.csv --pop=20 --gens=5 --n=2000

# Full evolution on Kaggle dataset (100K elements)
./build/experiment --algo=both --opt=both --use-kaggle \
  --kaggle-path=./data/logs/viral_data.csv \
  --out=./data/logs/evolution.csv \
  --pop=500 --gens=10 --n=100000

# QuickSort only with Genetic Algorithm
./build/experiment --algo=qs --opt=ga --pop=100 --gens=5

# MergeSort only with Simulated Annealing
./build/experiment --algo=ms --opt=sa --pop=100 --gens=5
```

**Windows (PowerShell):**
```powershell
# Quick test (small dataset, fast)
.\build\Release\experiment.exe --out=data/logs/my_results.csv --pop=20 --gens=5 --n=2000

# Full evolution on Kaggle dataset (100K elements)
.\build\Release\experiment.exe --algo=both --opt=both --use-kaggle --kaggle-path=.\data\logs\viral_data.csv --out=.\data\logs\evolution.csv --pop=500 --gens=10 --n=100000

# QuickSort only with Genetic Algorithm
.\build\Release\experiment.exe --algo=qs --opt=ga --pop=100 --gens=5

# MergeSort only with Simulated Annealing
.\build\Release\experiment.exe --algo=ms --opt=sa --pop=100 --gens=5
```

**Note:** On Windows, replace `.\build\Release\experiment.exe` with `.\build\Debug\experiment.exe` if you built in Debug mode.

### Visualization Controls

- **Play/Pause**: Start/stop automatic generation progression
- **Prev/Next**: Step through generations manually
- **Speed Slider**: Adjust playback speed (0.1x - 4x)
- **Optimizer Filter**: Show All, GA only, or SA only
- **Algorithm Filter**: Show All, QuickSort only, or MergeSort only
- **Reveal Mode**: Progressive (2 → 100K), All at step, or Top 1K
- **Color Mode**: Fitness tiers or Algorithm-based coloring
- **Demo (Kaggle)**: Load pre-generated dataset with both algorithms and optimizers

### Loading Custom Data

1. **Generate CSV**: Run experiments (see above)
2. **Load in Viz**: 
   - Click "Choose File" and select your CSV
   - Or paste CSV URL in the text field and click "Load"
   - Or use "Demo (Kaggle)" for pre-loaded data

### CSV Format

The visualization accepts CSV files with the following columns:
- `run_id`, `step`, `algo`, `opt`
- `pivot`, `scheme`, `cutoff`, `depth`, `tail` (QuickSort)
- `run_threshold`, `iterative`, `reuse_buffer` (MergeSort)
- `fitness_ms`, `comparisons`, `swaps`, `n`

---

## 🎨 Screenshots & Examples

### Evolution in Action
Watch algorithms evolve from random configurations to optimized solutions:
- **Generation 0**: Scattered particles (random DNA)
- **Generation 5**: Clustering toward efficient regions
- **Generation 10**: Converged on optimal configurations

### Leaderboard Tracking
See which combinations dominate:
- **QS (time)**: Best QuickSort execution time
- **MS (time)**: Best MergeSort execution time
- **GA BEST**: Best Genetic Algorithm result
- **SA BEST**: Best Simulated Annealing result

### Interactive Exploration
- Click particles to see detailed DNA parameters
- Filter by algorithm or optimizer
- Track space complexity estimates
- Compare time vs space trade-offs

---

## 🏗️ Project Structure

```
algo-evolution/
├── include/              # Header files
│   ├── common.hpp        # Common utilities
│   ├── ga.hpp           # Genetic Algorithm
│   ├── sa.hpp           # Simulated Annealing
│   ├── evaluator.hpp    # Algorithm evaluation
│   ├── datasets.hpp     # Dataset loading
│   └── logging.hpp      # CSV output
├── src/                 # Source files
│   ├── main_experiment.cpp    # Main experiment runner
│   ├── massive_demo.cpp       # Quick dataset generation
│   ├── ga.cpp           # GA implementation
│   ├── sa.cpp           # SA implementation
│   ├── evaluator.cpp    # Evaluation logic
│   └── datasets.cpp     # Dataset utilities
├── viz/                 # Web visualization
│   ├── index.html       # Main UI
│   ├── viz.js          # Visualization logic
│   └── style.css       # Styling
├── scripts/             # Utility scripts
│   └── download_kaggle.py    # Dataset downloader
├── data/logs/           # Generated CSV files
└── build/               # Build artifacts
```

---

## 🔬 Technical Details

### Performance Optimizations
- **Filter Caching**: Avoids redundant particle filtering
- **Color Computation Caching**: Pre-computes particle colors
- **Viewport Culling**: Skips off-screen particles
- **Adaptive Draw Cap**: Scales with screen resolution
- **Context State Batching**: Minimizes canvas operations
- **60fps Rendering**: Smooth playback even with 100K+ particles

### Dataset
Uses the [Kaggle Benchmark Dataset for Sorting Algorithms](https://www.kaggle.com/datasets/bekiremirhanakay/benchmark-dataset-for-sorting-algorithms) for real-world testing on 100K+ element arrays.

### Algorithm Implementations
- **QuickSort**: Optimized with multiple pivot strategies and partitioning schemes
- **MergeSort**: Supports iterative/recursive modes and buffer reuse
- Both implementations track comparisons, swaps, and execution time

---

## 🤝 Contributing

Contributions welcome! Areas for improvement:
- Additional sorting algorithms (HeapSort, RadixSort, etc.)
- More optimization strategies (Particle Swarm, Ant Colony, etc.)
- 3D visualization enhancements
- Performance profiling tools
- Export/import of evolved configurations

---

## 📄 License

MIT License - feel free to use this for research, education, or just because evolution is cool.

---

## 🙏 Acknowledgments

- Kaggle dataset: [bekiremirhanakay/benchmark-dataset-for-sorting-algorithms](https://www.kaggle.com/datasets/bekiremirhanakay/benchmark-dataset-for-sorting-algorithms)
- Inspired by evolutionary computation and algorithmic analysis

---

## 🚀 Why This Is Insane

Most people study algorithms. We **evolve** them.

This project bridges three worlds:
1. **Computer Science**: Algorithm analysis and optimization
2. **Evolutionary Biology**: Natural selection and adaptation
3. **Data Visualization**: Real-time interactive exploration

It's not just code—it's a **digital ecosystem** where algorithms compete, mutate, and evolve. Watch as random QuickSort configurations become optimized powerhouses. See MergeSort adapt its strategy over generations. It's evolution, but for code.

**And it runs at 60fps with 100K+ particles.**

---

<div align="center">

**Made with 🧬 by evolution enthusiasts**

[⭐ Star this repo](https://github.com/yourusername/algo-evolution) | [🐛 Report Issues](https://github.com/yourusername/algo-evolution/issues) | [💡 Contribute](https://github.com/yourusername/algo-evolution/pulls)

</div>

