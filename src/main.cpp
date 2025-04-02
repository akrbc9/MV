#include "simulation_controller.hpp"
#include "simulation_config.hpp"
#include <iostream>
#include <iomanip>

void printReport(const SimulationReport& report) {
    std::cout << "\n=== Simulation Report ===\n";
    std::cout << "Execution time: " << report.executionTime.count() << "ms\n";
    std::cout << "Total steps: " << report.timeSteps << "\n";
    std::cout << "Final population:\n";
    std::cout << "  Predators: " << report.finalPredatorCount << "\n";
    std::cout << "  Prey: " << report.finalPreyCount << "\n\n";
    
    std::cout << "Population History:\n";
    std::cout << std::setw(8) << "Step" << std::setw(12) << "Predators" << std::setw(12) << "Prey\n";

    std::vector<int> reportPreyHistory = report.getPreyHistory();
    std::vector<int> reportPredatorHistory = report.getPredatorHistory();
    // for (size_t i = 0; i < reportPreyHistory.size(); ++i) {
    //     std::cout << std::setw(8) << i 
    //               << std::setw(12) << reportPredatorHistory[i]
    //               << std::setw(12) << reportPreyHistory[i] << "\n";
    // }
    std::cout << "\nSimulation Configuration:\n";
    std::cout << "  World Size: " << report.getSimulationConfig().worldWidth << " x " 
              << report.getSimulationConfig().worldHeight << "\n";
    std::cout << "  Initial Predators: " << report.getSimulationConfig().initialPredators << "\n";
    std::cout << "  Initial Prey: " << report.getSimulationConfig().initialPrey << "\n";
    std::cout << "  Movement Magnitude (Predators): " << report.getSimulationConfig().MF << "\n";
    std::cout << "  Movement Magnitude (Prey): " << report.getSimulationConfig().MR << "\n";
    std::cout << "  Interaction Radius: " << report.getSimulationConfig().interactionRadius << "\n";
    std::cout << "  Cell Size: " << report.getSimulationConfig().cellSize << "\n";
    std::cout << "  Simulation Steps: " << report.getSimulationConfig().simulationSteps << "\n";

    std::cout << "  Carrying Capacity of Prey: " << report.getSimulationConfig().NR << "\n";
    std::cout << "  Reproduction Rate of Prey: " << report.getSimulationConfig().RR << "\n";
    std::cout << "  Death Rate of Prey: " << report.getSimulationConfig().DR << "\n";
    std::cout << "  Death Rate of Predator: " << report.getSimulationConfig().DF << "\n";
    std::cout << "  Reproduction Rate of Predator: " << report.getSimulationConfig().RF << "\n";
    std::cout << "  Save Statistics: " << (report.getSimulationConfig().saveStatistics ? "Yes" : "No") << "\n";
    std::cout << "  Output File: " << report.getSimulationConfig().outputFile << "\n";
    std::cout << "=========================\n";
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
    config.NR = 446;   // Carrying capacity of Prey (nr)
    config.RR = 0.1;     // Reproduction rate of Prey (rr)
    config.DR = 1;     // Death rate of prey when encountering predator (dr)
    config.DF = 0.0733333;     // Death rate of predator when no prey around (df)
    config.RF = 0.443333;     // Reproduction rate of predator (rf)
    
    // Problem @ 446,0.0533333,0.0733333,0.443333. NR, DR, DF, RF
    // Output parameters
    config.saveStatistics = true;
    config.outputFile = "simulation_stats.csv";
    SimulationController controller(config);
    
    // Create and run simulation

    controller.initialize();
    std::cout << "After Initialize" << std::endl;
    controller.runForTimesteps(config.simulationSteps);
    controller.end();
    std::cout << "After END" << std::endl;
    // Get and print report
    SimulationReport report = controller.getReport();
    printReport(report);
            
    // Visualize the population history
    //std::cout << "\nPress Space to run again, or any other key to close the visualization window...\n";
    report.visualize();
    return 0;
}