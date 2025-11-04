#pragma once

enum class Pivot { First, Last, Median3 };
enum class PartitionScheme { Lomuto, Hoare };

struct QSDNA {
  Pivot pivot{Pivot::Median3};
  PartitionScheme scheme{PartitionScheme::Hoare};
  int insertionCutoff{16};     // [0..64]
  int depthCap{64};            // ~ [floor(log2 n) .. floor(2*log2 n)]
  bool tailRecElim{true};
};

struct MSDNA {
  int runThreshold{16};        // [0..64]
  bool iterative{true};
  bool reuseBuffer{true};
};
