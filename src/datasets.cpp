#include "datasets.hpp"
#include "common.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <charconv>
#include <cmath>
#include <random>
#include <string>
#include <climits>
using namespace std; 

// simple rng wrapper thing i made
class XRand {
public:
    mt19937 gen;
    XRand(uint64_t s) : gen(s) {}
    uint64_t next() { return gen(); }
    size_t uniform(size_t a, size_t b) {
        uniform_int_distribution<size_t> d(a, b);
        return d(gen);
    }
};

static vector<int> nearly_sorted(size_t n, uint64_t seed) {
    vector<int> a(n);
    for (size_t i=0;i<n;++i) a[i] = int(i);
    XRand rng(seed);
    size_t swaps = max<size_t>(1, n/100); // ~1%
    for (size_t k=0;k<swaps;++k) {
        size_t i = rng.uniform(0, n-1), j = rng.uniform(0, n-1);
        swap(a[i], a[j]);
    }
    return a;
}

static vector<int> reverse_sorted(size_t n) {
    vector<int> a(n);
    for (size_t i=0;i<n;++i) 
        a[i] = int(n-1-i);
    return a;
}

static vector<int> many_dups(size_t n, uint64_t seed) {
    XRand rng(seed);
    vector<int> a(n);
    int k = 100; // limited value range to force duplicates
    for (size_t i=0;i<n;++i) 
        a[i] = int(rng.uniform(0, k-1));
    return a;
}

static vector<int> uniform_random(size_t n, uint64_t seed) {
    XRand rng(seed);
    vector<int> a(n);
    for (size_t i=0;i<n;++i) 
        a[i] = int(rng.next() & 0x7fffffff);
    return a;
}

vector<int> load_kaggle_column_as_ints(const string& csv_path, size_t n) {
    ifstream in(csv_path);
    vector<int> vals;
    vals.reserve(n);
    if (!in) return vals; // empty => caller falls back
    string line;
    // Try to find first numeric column
    vector<size_t> numericCols;
    bool headerParsed = false;
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string cell;


        vector<string> cells;



        while (getline(ss, cell, ',')) cells.push_back(cell);
        if (!headerParsed) { headerParsed = true; continue; } // skip header
        // detect numeric columns on the fly
        if (numericCols.empty()) {
            for (size_t c=0;c<cells.size();++c) {
                double d; char* endp=nullptr;
                try {
                    d = stod(cells[c]);
                    (void)d;
                    numericCols.push_back(c);
                } catch (...) { /* not numeric */ }
            }
            if (numericCols.empty()) continue;
        }
        if (!numericCols.empty()) {
            try {
                double d = stod(cells[numericCols[0]]);
                long long asInt = (long long) llround(d);
                vals.push_back(int(asInt));
            } catch (...) {}
        }
        if (vals.size() >= n) break;
    }
    // stretch or cycle to size n
    if (vals.empty()){ 
        return vals;
    }
    if (vals.size() < n) {
        size_t m = vals.size();


        vals.reserve(n);

        
        
        for (size_t i=m;i<n;++i) vals.push_back(vals[i % m]);
    } 
    else if (vals.size() > n) {
        vals.resize(n);
    }
    return vals;
}

vector<int> make_array(size_t n, Dist d, uint64_t seed) {
    switch (d) {
        case Dist::Uniform:      return uniform_random(n, seed);
        case Dist::NearlySorted: return nearly_sorted(n, seed);
        case Dist::Reverse:      return reverse_sorted(n);
        case Dist::ManyDup:      return many_dups(n, seed);
        case Dist::Kaggle:       return load_kaggle_column_as_ints("data/kaggle.csv", n);
    }
    return uniform_random(n, seed);
}
