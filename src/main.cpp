#include "simulation_controller.hpp"
#include "simulation_config.hpp"
#include <iostream>
#include <iomanip>

void printReport(const SimulationReport& report) {
    std::cout << "\n=== Simulation Report ===\n";
    std::cout << "Execution time: " << report.executionTime.count() << "ms\n";
    std::cout << "Total steps: " << report.totalSteps << "\n";
    std::cout << "Final population:\n";
    std::cout << "  Predators: " << report.finalPredatorCount << "\n";
    std::cout << "  Prey: " << report.finalPreyCount << "\n\n";
    
    std::cout << "Population History:\n";
    std::cout << std::setw(8) << "Step" << std::setw(12) << "Predators" << std::setw(12) << "Prey\n";
    for (size_t i = 0; i < report.predatorCounts.size(); ++i) {
        std::cout << std::setw(8) << i 
                  << std::setw(12) << report.predatorCounts[i]
                  << std::setw(12) << report.preyCounts[i] << "\n";
    }
}

int main() {
    // Create simulation configuration
    SimulationConfig config;
    
    // World parameters
    config.worldWidth = 1.0;
    config.worldHeight = 1.0;
    
    // Agent parameters
    config.initialPredators = 30;  // f_init
    config.initialPrey = 500;      // r_init
    
    // Movement parameters
    config.MF = 0.05;  // Movement magnitude for predators (mf)
    config.MR = 0.03;  // Movement magnitude for prey (mr)
    
    // Spatial parameters
    config.interactionRadius = 0.02;  // Interaction radius (cd)
    config.cellSize = 0.02;  // Grid cell size should match interaction radius
    
    // Simulation parameters
    config.simulationSteps = 1000;  // max_timesteps
    config.randomizeInitialPositions = true;
    
    // Population dynamics parameters
    config.NR = 500.0;   // Carrying capacity of Prey (nr)
    config.RR = 0.1;     // Reproduction rate of Prey (rr)
    config.DR = 1.0;     // Death rate of prey when encountering predator (dr)
    config.DF = 0.1;     // Death rate of predator when no prey around (df)
    config.RF = 0.5;     // Reproduction rate of predator (rf)
    
    // Output parameters
    config.saveStatistics = true;
    config.outputFile = "simulation_stats.csv";
    SimulationController controller(config);
    
    // Create and run simulation

    controller.initialize();
    controller.runForTimesteps(config.simulationSteps);
    controller.end();
            
    // Get and print report
    SimulationReport report = controller.getReport();
    printReport(report);
            
    // Visualize the population history
    //std::cout << "\nPress Space to run again, or any other key to close the visualization window...\n";
    report.visualize();
    return 0;
}