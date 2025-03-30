#include "sample_manager.hpp"
#include "simulation_controller.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <iostream>

SampleManager::SampleManager(const std::vector<ParameterRange>& ranges, int num_samples)
    : ranges_(ranges), num_samples_(num_samples) {
    generateSamples();
}

void SampleManager::generateSamples() {
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 rng(rd());
    
    // Generate samples for each parameter
    for (const auto& range : ranges_) {
        std::vector<double> samples(num_samples_);
        
        // Generate uniform random samples
        std::uniform_real_distribution<double> dist(range.min, range.max);
        for (int i = 0; i < num_samples_; ++i) {
            samples[i] = dist(rng);
        }
        
        // Shuffle the samples to ensure better distribution
        std::shuffle(samples.begin(), samples.end(), rng);
        parameter_samples_.push_back(samples);
    }
    
    // Convert parameter samples to the format expected by the simulation
    samples.clear();
    for (int i = 0; i < num_samples_; ++i) {
        auto sample = getSample(i);
        if (sample.size() >= 4) {
            samples.push_back({sample[0], sample[1], sample[2], sample[3]});
        }
    }
}

std::vector<double> SampleManager::getSample(int index) const {
    std::vector<double> sample;
    for (const auto& param_samples : parameter_samples_) {
        if (index < param_samples.size()) {
            sample.push_back(param_samples[index]);
        }
    }
    return sample;
}

std::vector<std::vector<double>> SampleManager::getAllSamples() const {
    std::vector<std::vector<double>> samples(num_samples_);
    for (int i = 0; i < num_samples_; ++i) {
        samples[i] = getSample(i);
    }
    return samples;
}

void SampleManager::runSimulations(int num_reruns, int num_sims, int num_timesteps) {
    stats.clear();
    
    std::cout << "\nStarting parameter sweep with " << samples.size() << " samples...\n";
    
    for (size_t sample_idx = 0; sample_idx < samples.size(); ++sample_idx) {
        const auto& sample = samples[sample_idx];
        std::cout << "\nRunning sample " << (sample_idx + 1) << "/" << samples.size() 
                  << " (NR=" << sample[0] << ", DR=" << sample[1] 
                  << ", DF=" << sample[2] << ", RF=" << sample[3] << ")\n";
        
        std::vector<std::pair<int, int>> results;
        
        // Run multiple simulations for this sample
        for (int rerun = 0; rerun < num_reruns; ++rerun) {
            std::cout << "  Rerun " << (rerun + 1) << "/" << num_reruns << ": ";
            
            for (int sim = 0; sim < num_sims; ++sim) {
                std::cout << (sim + 1) << " ";
                std::cout.flush();  // Ensure immediate output
                
                SimulationConfig config = createConfig(sample);
                SimulationController controller(config);
                
                controller.initialize();
                controller.runForTimesteps(num_timesteps);
                controller.end();
                
                auto report = controller.getReport();
                results.push_back({
                    report.finalPredatorCount,
                    report.finalPreyCount
                });
            }
            std::cout << "\n";
        }
        
        calculateStats(results);
        
        // Print summary for this sample
        const auto& stat = stats.back();
        std::cout << "  Results: Predators=" << stat.avg_predators 
                  << " (±" << stat.std_predators << "), "
                  << "Prey=" << stat.avg_prey 
                  << " (±" << stat.std_prey << ")\n";
    }
    
    std::cout << "\nParameter sweep completed!\n";
}

void SampleManager::calculateStats(const std::vector<std::pair<int, int>>& results) {
    // Calculate averages
    double sum_predators = 0.0;
    double sum_prey = 0.0;
    for (const auto& result : results) {
        sum_predators += result.first;
        sum_prey += result.second;
    }
    
    double avg_predators = sum_predators / results.size();
    double avg_prey = sum_prey / results.size();
    
    // Calculate standard deviations
    double sum_sq_diff_predators = 0.0;
    double sum_sq_diff_prey = 0.0;
    for (const auto& result : results) {
        sum_sq_diff_predators += std::pow(result.first - avg_predators, 2);
        sum_sq_diff_prey += std::pow(result.second - avg_prey, 2);
    }
    
    double std_predators = std::sqrt(sum_sq_diff_predators / results.size());
    double std_prey = std::sqrt(sum_sq_diff_prey / results.size());
    
    stats.push_back({
        avg_predators,
        avg_prey,
        std_predators,
        std_prey
    });
}

SimulationConfig SampleManager::createConfig(const std::array<double, 4>& params) const {
    SimulationConfig config;
    
    // World parameters
    config.worldWidth = 1.0;
    config.worldHeight = 1.0;
    
    // Agent parameters
    config.initialPredators = 30;
    config.initialPrey = 100;
    
    // Movement parameters
    config.MF = 0.05;
    config.MR = 0.03;
    
    // Spatial parameters
    config.interactionRadius = 0.02;
    config.cellSize = 0.02;
    
    // Simulation parameters
    config.simulationSteps = 300;  // Default value
    config.randomizeInitialPositions = true;
    
    // Population dynamics parameters
    config.NR = params[0];  // NR
    config.RR = 0.1;
    config.DR = params[1];  // DR
    config.DF = params[2];  // DF
    config.RF = params[3];  // RF
    
    // Output parameters
    config.saveStatistics = false;
    
    return config;
}

void SampleManager::saveResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    // Write header
    file << "sample_id,nr,dr,df,rf,avg_predators,avg_prey,std_predators,std_prey\n";
    
    // Write data
    for (size_t i = 0; i < samples.size(); ++i) {
        const auto& sample = samples[i];
        const auto& stat = stats[i];
        
        file << i << ","
             << sample[0] << ","  // NR
             << sample[1] << ","  // DR
             << sample[2] << ","  // DF
             << sample[3] << ","  // RF
             << stat.avg_predators << ","
             << stat.avg_prey << ","
             << stat.std_predators << ","
             << stat.std_prey << "\n";
    }
} 