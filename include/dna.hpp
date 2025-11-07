#pragma once

// DNA structs for quicksort and mergesort configs

enum class Pivot {
    First,
    Last,
    Median3
};

enum class PartitionScheme {
    Lomuto,
    Hoare
};

struct QSDNA {
    Pivot pivot = Pivot::Median3;  // default to median
    PartitionScheme scheme = PartitionScheme::Hoare;
    int insertionCutoff = 16;  // switch to insertion sort for small arrays
    int depthCap = 0;  // 0 means use default, otherwise cap recursion depth
    bool tailRecElim = false;  // tail recursion optimization
};

struct MSDNA {
    int runThreshold = 16;  // use insertion sort for runs smaller than this
    bool iterative = true;  // use iterative instead of recursive
    bool reuseBuffer = true;  // reuse buffer to save memory allocations
};

