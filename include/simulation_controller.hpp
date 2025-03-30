#pragma once

#include "simulation_context.hpp"
#include "grid.hpp"
#include "simulation_report.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include "simulation_config.hpp"
#include "agent.hpp"

struct SimulationStats {
    int predatorCount;
    int preyCount;
};

class SimulationController {
private:
    SimulationContext context;

    // The grid will be used by the controller to manage the agents spatially
    Grid grid;
    bool isRunning;
    bool isPaused;
    int currentStep;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    // Timing and history tracking
    std::vector<int> predatorHistory;
    std::vector<int> preyHistory;
    
    // Single timestep update
    void updateSingleTimestep();
    void updateSingleAgent(std::shared_ptr<Agent> agent);
    
    // Update history
    void updateHistory();
    void initializePopulation();


    //Track stats to make everything more efficient
    int predatorCount;
    int preyCount;

public:
    SimulationController(const SimulationConfig& config);
    
    // Static RNG
    static std::mt19937 rng;
    static std::uniform_real_distribution<double> reproDist;
    static std::uniform_real_distribution<double> deathDist;

    // Simulation control
    void initialize();
    void run();  // Run for one timestep
    void runForTimesteps(int numSteps);  // Run for specified number of timesteps
    void pause();
    void end();
    
    // Simulation status
    bool isSimulationRunning() const { return isRunning; }
    bool isSimulationPaused() const { return isPaused; }
    int getCurrentStep() const { return currentStep; }
    
    // Current simulation data
    SimulationStats getCurrentStats() const;
    void setPredatorCount(int count);
    void setPreyCount(int count);
    int getCurrentPredatorCount() const;
    int getCurrentPreyCount() const;
    int agentCount() const;

    void incrementPredatorCount();
    void decrementPredatorCount();
    void incrementPreyCount();
    void decrementPreyCount();
    void resetStats();

    size_t randomAgentIndex() const;

    // Reporting
    SimulationReport getReport() const;
    
    // Access
    Grid& getGrid() { return grid; }
    const SimulationContext& getContext() const { return context; }

    // Agent Management
    const std::vector<std::shared_ptr<Agent>>& getAgents() const { return grid.getAgents(); }
}; 