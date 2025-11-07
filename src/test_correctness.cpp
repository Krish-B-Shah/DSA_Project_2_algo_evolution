#include "quicksort.hpp"
#include "mergesort.hpp"
#include "datasets.hpp"
#include "metrics.hpp"
#include "dna.hpp"
#include "common.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include <span>

using namespace std;

// simple test helper
static void test_sort(const string& name, vector<int> arr, bool is_quicksort) {
    Metrics m;
    m.comparisons = 0;
    m.swaps = 0;
    
    vector<int> original = arr;  // keep copy for debugging
    
    if (is_quicksort) {
        QSDNA dna;
        dna.pivot = Pivot::Median3;
        dna.scheme = PartitionScheme::Hoare;
        dna.insertionCutoff = 16;
        dna.depthCap = 64;
        dna.tailRecElim = false;
        quicksort(span<int>(arr.data(), arr.size()), dna, m);
    } else {
        MSDNA dna;
        dna.runThreshold = 16;
        dna.iterative = true;
        dna.reuseBuffer = true;
        mergesort(span<int>(arr.data(), arr.size()), dna, m);
    }
    
    // check if sorted
    bool is_sorted = std::is_sorted(arr.begin(), arr.end());
    
    if (is_sorted) {
        cout << "✓ " << name << " passed (comps=" << m.comparisons << ", swaps=" << m.swaps << ")\n";
    } else {
        cout << "✗ " << name << " FAILED!\n";
        cout << "  Original size: " << original.size() << "\n";
        if (original.size() <= 20) {
            cout << "  Original: ";
            for (int x : original) cout << x << " ";
            cout << "\n";
            cout << "  Result: ";
            for (int x : arr) cout << x << " ";
            cout << "\n";
        }
        assert(false);
    }
}

int main() {
    cout << "Running correctness tests...\n\n";
    
    // test 1: small random array
    {
        vector<int> small = make_array(100, Dist::Uniform, 12345);
        test_sort("QuickSort: small random (n=100)", small, true);
    }
    
    {
        vector<int> small = make_array(100, Dist::Uniform, 12345);
        test_sort("MergeSort: small random (n=100)", small, false);
    }
    
    // test 2: duplicates
    {
        vector<int> dups = make_array(50, Dist::ManyDup, 999);
        test_sort("QuickSort: many duplicates (n=50)", dups, true);
    }
    
    {
        vector<int> dups = make_array(50, Dist::ManyDup, 999);
        test_sort("MergeSort: many duplicates (n=50)", dups, false);
    }
    
    // test 3: nearly sorted
    {
        vector<int> near = make_array(200, Dist::NearlySorted, 42);
        test_sort("QuickSort: nearly sorted (n=200)", near, true);
    }
    
    {
        vector<int> near = make_array(200, Dist::NearlySorted, 42);
        test_sort("MergeSort: nearly sorted (n=200)", near, false);
    }
    
    // test 4: reverse sorted
    {
        vector<int> rev = make_array(150, Dist::Reverse, 0);
        test_sort("QuickSort: reverse sorted (n=150)", rev, true);
    }
    
    {
        vector<int> rev = make_array(150, Dist::Reverse, 0);
        test_sort("MergeSort: reverse sorted (n=150)", rev, false);
    }
    
    // test 5: edge cases
    {
        vector<int> empty;
        test_sort("QuickSort: empty array", empty, true);
    }
    
    {
        vector<int> one = {42};
        test_sort("QuickSort: single element", one, true);
    }
    
    {
        vector<int> two = {2, 1};
        test_sort("QuickSort: two elements", two, true);
    }
    
    {
        vector<int> same = vector<int>(10, 5);  // all same
        test_sort("QuickSort: all same values", same, true);
    }
    
    // test different pivot strategies
    {
        vector<int> arr = make_array(100, Dist::Uniform, 123);
        Metrics m;
        QSDNA dna;
        dna.pivot = Pivot::First;
        dna.scheme = PartitionScheme::Lomuto;
        dna.insertionCutoff = 10;
        quicksort(span<int>(arr.data(), arr.size()), dna, m);
        assert(std::is_sorted(arr.begin(), arr.end()));
        cout << "✓ QuickSort: First pivot + Lomuto passed\n";
    }
    
    {
        vector<int> arr = make_array(100, Dist::Uniform, 123);
        Metrics m;
        QSDNA dna;
        dna.pivot = Pivot::Last;
        dna.scheme = PartitionScheme::Hoare;
        quicksort(span<int>(arr.data(), arr.size()), dna, m);
        assert(std::is_sorted(arr.begin(), arr.end()));
        cout << "✓ QuickSort: Last pivot + Hoare passed\n";
    }
    
    // test mergesort variants
    {
        vector<int> arr = make_array(100, Dist::Uniform, 456);
        Metrics m;
        MSDNA dna;
        dna.iterative = false;  // recursive
        dna.reuseBuffer = false;
        mergesort(span<int>(arr.data(), arr.size()), dna, m);
        assert(std::is_sorted(arr.begin(), arr.end()));
        cout << "✓ MergeSort: recursive variant passed\n";
    }
    
    cout << "\nAll tests passed! ✓\n";
    return 0;
}

