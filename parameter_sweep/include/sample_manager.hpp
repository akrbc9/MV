#pragma once

#include <vector>
#include <string>
#include <array>
#include <utility>
#include "simulation_config.hpp"

struct ParameterRange {
    double min;
    double max;
};

struct SampleStats {
    double avg_predators;
    double avg_prey;
    double std_predators;
    double std_prey;
};

class SampleManager {
public:
    SampleManager(const std::vector<ParameterRange>& ranges, int num_samples);
    
    void generateSamples();
    std::vector<double> getSample(int index) const;
    std::vector<std::vector<double>> getAllSamples() const;
    
    // Simulation methods
    void runSimulations(int num_reruns, int num_sims, int num_timesteps);
    void saveResults(const std::string& filename) const;
    
    // Getters
    const std::vector<std::array<double, 4>>& getSamples() const { return samples; }
    const std::vector<SampleStats>& getStats() const { return stats; }
    
private:
    std::vector<ParameterRange> ranges_;
    int num_samples_;
    std::vector<std::vector<double>> parameter_samples_;
    std::vector<std::array<double, 4>> samples;  // NR, DR, DF, RF
    std::vector<SampleStats> stats;
    
    // Helper methods
    void calculateStats(const std::vector<std::pair<int, int>>& results);
    SimulationConfig createConfig(const std::array<double, 4>& params) const;
}; 