#pragma once

#include "simulation_config.hpp"
#include <random>

class SimulationContext {
public:
    explicit SimulationContext(const SimulationConfig& cfg)
        : config(cfg), rng(std::random_device()()) {}
    
    // Prevent copying
    SimulationContext(const SimulationContext&) = delete;
    SimulationContext& operator=(const SimulationContext&) = delete;
    
    // Allow moving
    // SimulationContext(SimulationContext&&) noexcept = default;
    // SimulationContext& operator=(SimulationContext&&) = delete;  // Cannot move references
    
    const SimulationConfig& getConfig() const { return config; }
    std::mt19937& getRNG() { return rng; }
    int getPredatorCount() const { return predatorCount; }
    int getPreyCount() const { return preyCount; }
    void setPredatorCount(int count) { predatorCount = count; }
    void setPreyCount(int count) { preyCount = count; }
    
private:    
    const SimulationConfig& config;
    int predatorCount;
    int preyCount;
    std::mt19937 rng;
}; 