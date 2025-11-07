#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "dna.hpp"

// Simple CSV writer for experiment logging
// Note: CSV parsing/reading is handled by viz_sfml.cpp
class CSVLogger {
public:
    CSVLogger(const std::string& filename) : file_(filename) {
        if (!file_.is_open()) {
            // Create directory if needed (basic implementation)
            // For now, just try to open - caller should ensure directory exists
        }
    }
    
    ~CSVLogger() {
        if (file_.is_open()) {
            file_.close();
        }
    }
    
    bool is_open() const {
        return file_.is_open();
    }
    
    // Write CSV header for QuickSort DNA
    void write_qs_header() {
        if (!file_.is_open()) return;
        file_ << "generation,individual,fitness_ms,pivot,scheme,insertion_cutoff,depth_cap,tail_rec_elim\n";
        file_.flush();
    }
    
    // Write CSV header for MergeSort DNA
    void write_ms_header() {
        if (!file_.is_open()) return;
        file_ << "generation,individual,fitness_ms,run_threshold,iterative,reuse_buffer\n";
        file_.flush();
    }
    
    // Write a row for QuickSort DNA
    void write_qs_row(int generation, int individual, double fitness_ms, const QSDNA& dna) {
        if (!file_.is_open()) return;
        
        std::string pivot_str;
        switch (dna.pivot) {
            case Pivot::First: pivot_str = "First"; break;
            case Pivot::Last: pivot_str = "Last"; break;
            case Pivot::Median3: pivot_str = "Median3"; break;
        }
        
        std::string scheme_str;
        switch (dna.scheme) {
            case PartitionScheme::Lomuto: scheme_str = "Lomuto"; break;
            case PartitionScheme::Hoare: scheme_str = "Hoare"; break;
        }
        
        file_ << generation << ","
              << individual << ","
              << fitness_ms << ","
              << pivot_str << ","
              << scheme_str << ","
              << dna.insertionCutoff << ","
              << dna.depthCap << ","
              << (dna.tailRecElim ? "true" : "false") << "\n";
        file_.flush();
    }
    
    // Write a row for MergeSort DNA
    void write_ms_row(int generation, int individual, double fitness_ms, const MSDNA& dna) {
        if (!file_.is_open()) return;
        
        file_ << generation << ","
              << individual << ","
              << fitness_ms << ","
              << dna.runThreshold << ","
              << (dna.iterative ? "true" : "false") << ","
              << (dna.reuseBuffer ? "true" : "false") << "\n";
        file_.flush();
    }
    
    // For SA: write step instead of generation
    void write_sa_qs_row(int step, double fitness_ms, const QSDNA& dna) {
        write_qs_row(step, 0, fitness_ms, dna);
    }
    
    void write_sa_ms_row(int step, double fitness_ms, const MSDNA& dna) {
        write_ms_row(step, 0, fitness_ms, dna);
    }

private:
    std::ofstream file_;
};

