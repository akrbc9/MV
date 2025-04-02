#pragma once

#include "simulation_context.hpp"
#include "grid.hpp"
#include "simulation_report.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include <numeric>

// Forward declarations
struct SimulationConfig;
class Agent;

struct SimulationStats {
    int predatorCount;
    int preyCount;
};

/**
 * Optimized SimulationController for managing the predator-prey simulation.
 * This version features improved data structures and batch processing.
 */
class SimulationController {
private:
    // Simulation context and configuration
    SimulationContext context;
    Grid grid;
    
    // Simulation state
    bool isRunning;
    bool isPaused;
    int currentStep;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    // Population history tracking
    std::vector<int> predatorHistory;
    std::vector<int> preyHistory;
    
    // Population counters
    int predatorCount;
    int preyCount;
    
    // Helper methods
    void updateSingleTimestep();
    void updateSingleAgent(std::shared_ptr<Agent> agent);
    void updateHistory();
    void initializePopulation();
    
    // Random distributions for movement and agent selection
    std::uniform_real_distribution<double> positionDist;
    std::uniform_real_distribution<double> directionDist;

public:
    // Static RNG (thread-safe)
    static std::mt19937 rng;
    static std::uniform_real_distribution<double> reproDist;
    static std::uniform_real_distribution<double> deathDist;

    /**
     * Constructor initializes the simulation with the given configuration.
     * 
     * @param config The simulation configuration
     */
    SimulationController(const SimulationConfig& config);
    
    /**
     * Initializes the simulation, setting up initial agent populations.
     */
    void initialize();
    
    /**
     * Runs the simulation for a single timestep.
     */
    void run();
    
    /**
     * Runs the simulation for the specified number of timesteps.
     * 
     * @param numSteps The number of timesteps to run
     */
    void runForTimesteps(int numSteps);
    
    /**
     * Pauses the simulation.
     */
    void pause();
    
    /**
     * Ends the simulation and prepares the final report.
     */
    void end();
    
    /**
     * Gets the current state of the simulation.
     * 
     * @return True if the simulation is running, false otherwise
     */
    bool isSimulationRunning() const { return isRunning; }
    
    /**
     * Gets the paused state of the simulation.
     * 
     * @return True if the simulation is paused, false otherwise
     */
    bool isSimulationPaused() const { return isPaused; }
    
    /**
     * Gets the current simulation timestep.
     * 
     * @return The current timestep
     */
    int getCurrentStep() const { return currentStep; }
    
    /**
     * Gets the current simulation statistics.
     * 
     * @return The current statistics
     */
    SimulationStats getCurrentStats() const;
    
    /**
     * Sets the predator count.
     * 
     * @param count The new predator count
     */
    void setPredatorCount(int count);
    
    /**
     * Sets the prey count.
     * 
     * @param count The new prey count
     */
    void setPreyCount(int count);
    
    /**
     * Gets the current predator count.
     * 
     * @return The current predator count
     */
    int getCurrentPredatorCount() const;
    
    /**
     * Gets the current prey count.
     * 
     * @return The current prey count
     */
    int getCurrentPreyCount() const;
    
    /**
     * Gets the total agent count.
     * 
     * @return The total number of agents
     */
    int agentCount() const;
    
    /**
     * Increments the predator count by one.
     */
    void incrementPredatorCount();
    
    /**
     * Decrements the predator count by one.
     */
    void decrementPredatorCount();
    
    /**
     * Increments the prey count by one.
     */
    void incrementPreyCount();
    
    /**
     * Decrements the prey count by one.
     */
    void decrementPreyCount();
    
    /**
     * Resets all population statistics.
     */
    void resetStats();
    
    /**
     * Gets a random agent index.
     * 
     * @return A random valid agent index
     */
    size_t randomAgentIndex() const;
    
    /**
     * Generates a random position within the world bounds.
     * 
     * @return A random position
     */
    Position randomPosition();
    
    /**
     * Generates a random direction vector.
     * 
     * @return A random direction vector
     */
    Position randomDirection();
    
    /**
     * Gets the final simulation report.
     * 
     * @return The simulation report
     */
    SimulationReport getReport() const;
    
    /**
     * Gets the grid used by the simulation.
     * 
     * @return A reference to the grid
     */
    Grid& getGrid() { return grid; }
    
    /**
     * Gets the simulation context.
     * 
     * @return A const reference to the context
     */
    const SimulationContext& getContext() const { return context; }
};