#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <optional>
#include <random>
#include <string_view>
#include <unordered_map>
using namespace std; 


inline uint64_t now_ns() {
  using namespace chrono;
  return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
}

// Simple xorshift64* PRNG
struct XRand {
  uint64_t s;
  explicit XRand(uint64_t seed) : s(seed ? seed : 0x9E3779B97F4A7C15ull) {}
  uint64_t next() {
    uint64_t x = s;
    x ^= x >> 12; x ^= x >> 25; x ^= x << 27;
    s = x;
    return x * 2685821657736338717ull;
  }
  uint64_t uniform(uint64_t lo, uint64_t hi) { // inclusive
    return lo + (next() % (hi - lo + 1));
  }
  double uniform01() {
    return (next() >> 11) * (1.0/9007199254740992.0);
  }
};

inline double geometric_mean_ms(const vector<double>& ms) {
  double logsum = 0.0;
  for (double v : ms) logsum += log(max(v, 1e-12));
  return exp(logsum / max<size_t>(1, ms.size()));
}

inline optional<string> argval(const vector<string>& args, string_view key) {
  for (size_t i=0;i+1<args.size();++i)
    if (args[i] == (string)key) return args[i+1];
  for (auto& s : args) {
    auto pos = s.find('=');
    if (pos!=string::npos && s.substr(0,pos)==key) return s.substr(pos+1);
  }
  return nullopt;
}
inline bool hasflag(const vector<string>& args, string_view key) {
  for (auto& s: args) if (s == key) return true;
  return false;
}
