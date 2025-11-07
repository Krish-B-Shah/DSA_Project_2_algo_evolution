#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <iomanip>
struct Particle {
  int generation;
  int dna_id;
  int pivot_choice;
  int partition_type;
  int cutoff;
  double fitness_ms;
  double comparisons;
  double swaps;
  bool isBest;
};
static std::vector<Particle> load_csv(const std::string& path) {
  std::ifstream in(path);
  std::string line;
  std::vector<Particle> particles;
  if (!in) { std::cerr << "No input: " << path << "\n"; return particles; }
  std::getline(in, line); // header
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    std::stringstream ss(line);
    std::string cell;
    std::vector<std::string> c;
    while (std::getline(ss, cell, ',')) c.push_back(cell);
    
    if (c.size() < 8) continue;
    Particle p;
    p.generation = std::stoi(c[0]);
    p.dna_id = std::stoi(c[1]);
    p.pivot_choice = std::stoi(c[2]);
    p.partition_type = std::stoi(c[3]);
    p.cutoff = std::stoi(c[4]);
    p.fitness_ms = std::stod(c[5]);
    p.comparisons = std::stod(c[6]);
    p.swaps = std::stod(c[7]);
    p.isBest = false;
    particles.push_back(p);
  }
  // marks the best for each geneation possible 
  std::unordered_map<int, double> bestFit;
  for (auto& p : particles) {
    if (bestFit.find(p.generation) == bestFit.end() || p.fitness_ms < bestFit[p.generation]) {
      bestFit[p.generation] = p.fitness_ms;
    }
  }
  for (auto& p : particles) {
    if (p.fitness_ms == bestFit[p.generation]) p.isBest = true;
  }
  return particles;
}

int main(int argc, char** argv) {
  std::string in = "data/logs/massive_evolution.csv";
  std::string out = "massive_viz.html";
  for (int i = 1; i < argc; ++i) {
    std::string s = argv[i];
    if (s.rfind("--in=", 0) == 0) in = s.substr(5);
    if (s.rfind("--out=", 0) == 0) out = s.substr(6);
  }
  auto particles = load_csv(in);
  if (particles.empty()) {
    std::cerr << "CSV had no rows: " << in << "\n";
    return 1;
  }
  // find the ranges for normalization
  double cmpMin = 1e9, cmpMax = 0;
  double swpMin = 1e9, swpMax = 0;
  double msMin = 1e9, msMax = 0;
  int maxGen = 0;
  int totalParticles = 0;
  
  for (const auto& p : particles) {
    cmpMin = std::min(cmpMin, p.comparisons);
    cmpMax = std::max(cmpMax, p.comparisons);
    swpMin = std::min(swpMin, p.swaps);
    swpMax = std::max(swpMax, p.swaps);
    msMin = std::min(msMin, p.fitness_ms);
    msMax = std::max(msMax, p.fitness_ms);
    maxGen = std::max(maxGen, p.generation);
    totalParticles++;
  }
  auto normalize = [](double v, double min, double max) {
    if (max == min) return 0.5;
    return (v - min) / (max - min);
  };
  
  // generates html visualization
  std::ofstream html(out);
  html << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
  html << "  <meta charset=\"UTF-8\">\n";
  html << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  html << "  <title>Algorithm Evolution - 100,000 Particles</title>\n";
