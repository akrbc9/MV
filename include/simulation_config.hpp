#pragma once

#include <string>

struct SimulationConfig {
    // World parameters
    double worldWidth;
    double worldHeight;
    
    // Agent parameters
    int initialPredators;
    int initialPrey;
    
    // Movement parameters
    double MF;  // Movement magnitude for predators
    double MR;  // Movement magnitude for prey
    
    // Spatial parameters
    double interactionRadius;  // CD in the requirements
    double cellSize;
    
    // Simulation parameters
    int simulationSteps;
    bool randomizeInitialPositions;
    
    // Population dynamics parameters
    int NR;    // Carrying capacity of Prey
    double RR;    // Reproduction rate of Prey
    double DR;    // Death rate of prey when encountering predator
    double DF;    // Death rate of predator when no prey around (formerly PDR)
    double RF;    // Reproduction rate of predator (formerly PRR)
    
    // Output parameters
    bool saveStatistics = true;
    std::string outputFile;
    
    // Load configuration from a file
    static SimulationConfig loadFromFile(const std::string& filename);
    
    // Save configuration to a file
    void saveToFile(const std::string& filename) const;

    // New fields from the code block
    int numTimesteps;  // Number of timesteps to run the simulation
}; 