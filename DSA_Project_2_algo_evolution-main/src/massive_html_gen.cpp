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
  html << "  <style>\n";
  html << "    body { margin:0; padding:20px; background:#0a0a0a; color:#fff; font-family:Arial,sans-serif; }\n";
  html << "    h1 { text-align:center; color:#4ecdc4; }\n";
  html << "    .controls { text-align:center; margin:20px 0; }\n";
  html << "    button { background:#2196F3; color:#fff; border:none; padding:10px 20px; margin:5px; border-radius:5px; cursor:pointer; font-size:16px; }\n";
  html << "    button:hover { background:#1976D2; }\n";
  html << "    button.primary { background:#ff6b6b; }\n";
  html << "    button.primary:hover { background:#ff5252; }\n";
  html << "    #canvas { background:#1a1a2e; border:2px solid #4ecdc4; display:block; margin:20px auto; }\n";
  html << "    .info { text-align:center; margin:10px 0; }\n";
  html << "    #modal { display:none; position:fixed; top:50%; left:50%; transform:translate(-50%,-50%); background:#000; padding:20px; border:2px solid #4ecdc4; border-radius:10px; z-index:1000; max-width:400px; }\n";
  html << "  </style>\n</head>\n<body>\n";
  html << "  <h1>Algorithm Evolution - 100,000 Particles</h1>\n";
  html << "  <div class=\"controls\">\n";
  html << "    <button class=\"primary\" onclick=\"togglePlay()\" id=\"playBtn\">Play</button>\n";
  html << "    <button onclick=\"prevGen()\">Previous</button>\n";
  html << "    <button onclick=\"nextGen()\">Next</button>\n";
  html << "    <button onclick=\"reset()\">Reset</button>\n";
  html << "    <button class=\"primary\" onclick=\"showAll()\" id=\"showAllBtn\">Show All 100K</button>\n";
  html << "  </div>\n";
  html << "  <canvas id=\"canvas\" width=\"1400\" height=\"800\"></canvas>\n";
  html << "  <div class=\"info\">Generation: <span id=\"genNum\">0</span>/" << (maxGen + 1) << " | Particles: <span id=\"particleCount\">0</span></div>\n";
  html << "  <div id=\"modal\">\n";
  html << "    <h3 style=\"margin-top:0; color:#4ecdc4;\">DNA Details</h3>\n";
  html << "    <div id=\"dnaInfo\"></div>\n";
  html << "    <button onclick=\"closeModal()\" style=\"margin-top:10px;\">Close</button>\n";
  html << "  </div>\n";
  html << "  \n";
  html << "  <script>\n";
  html << "    const maxGenerations = " << (maxGen + 1) << ";\n";
  html << "    const data = [];\n";
  
  // embed ALL of the generations directly in HTML 
  for (int gen = 0; gen <= maxGen; ++gen) {
    html << "    data[" << gen << "] = [";
    bool first = true;
    for (const auto& p : particles) {
      if (p.generation == gen) {
        if (!first) html << ",";
        double x = normalize(p.comparisons, cmpMin, cmpMax);
        double y = normalize(p.swaps, swpMin, swpMax);
        html << "[" << std::fixed << std::setprecision(3) << x << "," 
             << std::setprecision(3) << y << "," 
             << std::setprecision(4) << p.fitness_ms << ","
             << (p.isBest ? "1" : "0") << ","
             << p.pivot_choice << "," << p.partition_type << ","
             << p.cutoff << "," << p.dna_id << ","
             << (int)p.comparisons << "," << (int)p.swaps << "]";
        first = false;
      }
    }
    html << "];\n";
  }
  html << "    \n";
  html << "    let currentGen = 0;\n";
  html << "    let playing = false;\n";
  html << "    let showAllMode = false;\n";
  html << "    let canvas = document.getElementById('canvas');\n";
  html << "    let ctx = canvas.getContext('2d');\n";
  html << "    \n";
  html << "    function draw() {\n";
  html << "      ctx.fillStyle = '#1a1a2e';\n";
  html << "      ctx.fillRect(0, 0, 1400, 800);\n";
  html << "      \n";
  html << "      if (!data[currentGen] || data[currentGen].length === 0) return;\n";
  html << "      \n";
  html << "      let genData = data[currentGen];\n";
  html << "      let particlesToShow = showAllMode ? genData : genData.slice(0, 1000);\n";
  html << "      \n";
  html << "      // Find best/worst for color coding\n";
  html << "      let best = 1e9, worst = 0;\n";
  html << "      for (let p of genData) {\n";
  html << "        if (p[2] < best) best = p[2];\n";
  html << "        if (p[2] > worst) worst = p[2];\n";
  html << "      }\n";
  html << "      let range = worst - best || 1;\n";
  html << "      \n";
  html << "      // Draw particles\n";
  html << "      for (let p of particlesToShow) {\n";
  html << "        let x = 100 + p[0] * 1200;\n";
  html << "        let y = 100 + p[1] * 600;\n";
  html << "        let ms = p[2];\n";
  html << "        let isBest = p[3] === 1;\n";
  html << "        \n";
  html << "        // Color coding: red (poor), yellow (medium), green (top tier)\n";
  html << "        let fitnessRatio = (worst - ms) / range;\n";
  html << "        let color;\n";
  html << "        if (isBest) {\n";
  html << "          color = '#ffff00';\n";
  html << "        } else if (fitnessRatio >= 0.75) {\n";
  html << "          color = '#4caf50';\n";
  html << "        } else if (fitnessRatio >= 0.5) {\n";
  html << "          color = '#8bc34a';\n";
  html << "        } else if (fitnessRatio >= 0.25) {\n";
  html << "          color = '#ffeb3b';\n";
