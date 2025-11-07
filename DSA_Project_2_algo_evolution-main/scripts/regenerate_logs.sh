#!/bin/bash
# Quick regeneration of CSV logs with QS+MS and GA+SA
# Uses smaller parameters for speed

set -e

cd "$(dirname "$0")/.."
BUILD_DIR="./build"

echo "🔄 Regenerating CSV files with QS+MS and GA+SA (quick mode)..."
echo ""

# Quick parameters for speed
POP=20
GENS=3
N=2000
STEPS=15

# Function to regenerate a file
regenerate() {
    local file="$1"
    local kaggle_flag="$2"
    local kaggle_path="$3"
    
    echo "📝 Regenerating: $file"
    
    local cmd="$BUILD_DIR/experiment --algo=both --opt=both --out=$file --pop=$POP --gens=$GENS --n=$N --steps=$STEPS --trials-per-dist=1"
    
    if [ -n "$kaggle_flag" ] && [ -f "$kaggle_path" ]; then
        cmd="$cmd --use-kaggle --kaggle-path=$kaggle_path"
    fi
    
    $cmd 2>&1 | tail -2
    echo ""
}

# Regenerate key files
regenerate "data/logs/full_test.csv"
regenerate "data/logs/quick_test.csv"
regenerate "data/logs/run.csv"

# Regenerate viral_data.csv if the Kaggle dataset exists
if [ -f "data/logs/viral_data.csv" ] && [ -s "data/logs/viral_data.csv" ]; then
    # Check if it's actually a Kaggle dataset (has the right header)
    if head -1 "data/logs/viral_data.csv" | grep -q "run_id,step,algo"; then
        echo "⚠️  viral_data.csv appears to be a results file, not a Kaggle dataset"
        echo "   Skipping regeneration to avoid overwriting dataset"
    else
        regenerate "data/logs/viral_data_new.csv" "--use-kaggle" "data/logs/viral_data.csv"
        if [ -f "data/logs/viral_data_new.csv" ]; then
            mv "data/logs/viral_data_new.csv" "data/logs/viral_data_results.csv"
            echo "✅ Created viral_data_results.csv (experiment results)"
            echo "   (viral_data.csv kept as original Kaggle dataset)"
        fi
    fi
fi

echo "✅ Done! Files regenerated with QS+MS and GA+SA"
echo ""
echo "📊 Quick verification:"
for f in data/logs/full_test.csv data/logs/quick_test.csv data/logs/run.csv; do
    if [ -f "$f" ] && [ -s "$f" ]; then
        ms_count=$(grep -c ",MS," "$f" 2>/dev/null || echo "0")
        sa_count=$(grep -c ",SA," "$f" 2>/dev/null || echo "0")
        echo "  $f: $ms_count MS rows, $sa_count SA rows"
    fi
done

