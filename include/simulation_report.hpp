#pragma once

#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>
#include "simulation_config.hpp"
#include "simulation_context.hpp"

class SimulationReport {
public:
    // Initialize the simulation report;
    // prey_history and predator_history are vectors of size timeSteps
    // containing the number of prey and predators at each time step
    // finalPredatorCount and finalPreyCount are the final counts of predators and prey
    // simulationConfig is the configuration used for the simulation
    // timeSteps is the number of time steps in the simulation
    // The constructor initializes the report with the given data
    SimulationReport(
        const std::vector<int> predator_history,
        const std::vector<int> prey_history,
        int finalPredatorCount,
        int finalPreyCount,
        const SimulationConfig& config,
        int timeSteps,
        std::chrono::milliseconds executionTime,
        int normalizedPreyCount)
        :   prey_history_(prey_history),
            predator_history_(predator_history),
            finalPredatorCount(finalPredatorCount),
            finalPreyCount(finalPreyCount),
            simulationConfig(config),
            timeSteps(timeSteps),
            executionTime(executionTime),
            normalizedPreyCount(normalizedPreyCount) {}
    
    void visualize();
    int getPreyCount() const { return finalPreyCount; }
    int getPredatorCount() const { return finalPredatorCount; }
    int getNormalizedPreyCount() const {return normalizedPreyCount;}
    
    // Getters for history
    std::vector<int> getPreyHistory() const { return prey_history_; }
    std::vector<int> getPredatorHistory() const { return predator_history_; }

    SimulationConfig getSimulationConfig() const { return simulationConfig; }
    int getTimeSteps() const { return timeSteps; }

    // Simulation data
    std::vector<int> prey_history_;
    std::vector<int> predator_history_;
    int finalPredatorCount;
    int finalPreyCount;
    SimulationConfig simulationConfig; 
    int timeSteps; 
    std::chrono::milliseconds executionTime;
    int normalizedPreyCount; 


private:


    
    // Visualization constants
    static constexpr unsigned int WINDOW_WIDTH = 800;
    static constexpr unsigned int WINDOW_HEIGHT = 600;
    static constexpr float MARGIN = 50.f;

    // Helper methods for visualization
    void drawAxes(sf::RenderWindow& window) const;
    void drawGraph(sf::RenderWindow& window) const;
    void drawLegend(sf::RenderWindow& window) const;
    sf::Vector2f mapToScreen(float x, float y) const;
}; 