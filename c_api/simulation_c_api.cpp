#define SIMULATION_C_API_EXPORTS 1
#include "simulation_c_api.h"
#include "simulation_controller.hpp"
#include "agent.hpp"
#include <memory>
#include <cstring>
#include <vector>
#include <iostream>

// Structure to hold simulation data
struct SimulationData {
    std::unique_ptr<SimulationController> controller;
    CSimulationConfig config;
    std::vector<int> predatorHistory;
    std::vector<int> preyHistory;
};

// Add debug output
#define DEBUG_PRINT(msg) std::cout << "[C-API DEBUG] " << msg << std::endl

extern "C" {

SimulationHandle simulation_create(CSimulationConfig cConfig) {
    DEBUG_PRINT("simulation_create called");
    
    // Create C++ SimulationConfig from C-compatible struct
    SimulationConfig config;
    
    // Copy all fields from C struct to C++ struct
    config.worldWidth = cConfig.worldWidth;
    config.worldHeight = cConfig.worldHeight;
    config.initialPredators = cConfig.initialPredators;
    config.initialPrey = cConfig.initialPrey;
    config.MF = cConfig.MF;
    config.MR = cConfig.MR;
    config.interactionRadius = cConfig.interactionRadius;
    config.cellSize = cConfig.cellSize;
    config.simulationSteps = cConfig.simulationSteps;
    config.NR = cConfig.NR;
    config.RR = cConfig.RR;
    config.DR = cConfig.DR;
    config.DF = cConfig.DF;
    config.RF = cConfig.RF;
    
    // Now create the SimulationData with the proper C++ config
    auto data = new SimulationData();
    data->config = cConfig;  // Store the original C config
    data->controller = std::make_unique<SimulationController>(config);  // Use the converted C++ config
    
    DEBUG_PRINT("simulation_create returning handle: " << data);
    return data;
}

void simulation_initialize(SimulationHandle simulation) {
    DEBUG_PRINT("simulation_initialize called with handle: " << simulation);
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->initialize();
    DEBUG_PRINT("simulation_initialize completed");
}

void simulation_step(SimulationHandle simulation) {
    DEBUG_PRINT("simulation_step called with handle: " << simulation);
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->run();
    DEBUG_PRINT("simulation_step completed");
}

void simulation_run(SimulationHandle simulation, int steps) {
    DEBUG_PRINT("simulation_run called with handle: " << simulation << ", steps: " << steps);
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->runForTimesteps(steps);
    DEBUG_PRINT("simulation_run completed");
}

void simulation_pause(SimulationHandle simulation) {
    DEBUG_PRINT("simulation_pause called with handle: " << simulation);
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->pause();
    DEBUG_PRINT("simulation_pause completed");
}

void simulation_resume(SimulationHandle simulation) {
    DEBUG_PRINT("simulation_resume called with handle: " << simulation);
    // This would need implementation in the controller
    DEBUG_PRINT("simulation_resume (not implemented)");
}

void simulation_end(SimulationHandle simulation) {
    DEBUG_PRINT("simulation_end called with handle: " << simulation);
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->end();
    DEBUG_PRINT("simulation_end completed");
}

void simulation_get_status(SimulationHandle simulation, SimulationStatus* status) {
    DEBUG_PRINT("simulation_get_status called with handle: " << simulation);
    auto data = static_cast<SimulationData*>(simulation);
    
    // Fill status structure
    status->predatorCount = data->controller->getCurrentPredatorCount();
    status->preyCount = data->controller->getCurrentPreyCount();
    status->currentStep = data->controller->getCurrentStep();
    status->isRunning = data->controller->isSimulationRunning() ? 1 : 0;
    status->isPaused = data->controller->isSimulationPaused() ? 1 : 0;
    
    DEBUG_PRINT("simulation_get_status completed - predators: " << status->predatorCount 
                << ", prey: " << status->preyCount);
}

SimulationResult simulation_get_results(SimulationHandle simulation) {
    DEBUG_PRINT("simulation_get_results called with handle: " << simulation);
    auto data = static_cast<SimulationData*>(simulation);
    SimulationReport report = data->controller->getReport();
    
    // Copy history data
    data->predatorHistory = report.getPredatorHistory();
    data->preyHistory = report.getPreyHistory();
    
    // Fill result structure
    SimulationResult result;
    result.finalPredatorCount = report.getPredatorCount();
    result.finalPreyCount = report.getPreyCount();
    result.normalizedPreyCount = report.getNormalizedPreyCount();
    result.executionTimeMs = report.executionTime.count();
    result.timeSteps = report.getTimeSteps();
    
    // Set history pointers
    result.historySize = static_cast<int>(data->predatorHistory.size());
    result.predatorHistory = data->predatorHistory.data();
    result.preyHistory = data->preyHistory.data();
    
    DEBUG_PRINT("simulation_get_results completed - predators: " << result.finalPredatorCount 
                << ", prey: " << result.finalPreyCount 
                << ", historySize: " << result.historySize);
    
    return result;
}

void simulation_free_results(SimulationResult* results) {
    DEBUG_PRINT("simulation_free_results called");
    // No need to free anything here since we're using vectors in SimulationData
    // that stay alive until the simulation is destroyed
    DEBUG_PRINT("simulation_free_results completed");
}

void simulation_destroy(SimulationHandle simulation) {
    DEBUG_PRINT("simulation_destroy called with handle: " << simulation);
    if (simulation) {
        auto data = static_cast<SimulationData*>(simulation);
        delete data;
        DEBUG_PRINT("simulation_destroy completed, handle deleted");
    } else {
        DEBUG_PRINT("simulation_destroy called with null handle");
    }
}

void simulation_reset_global_state() {
    DEBUG_PRINT("simulation_reset_global_state called");
    
    // Reset agent ID counter
    Agent::resetIdCounter();
    
    // Reset the static RNG in SimulationController
    SimulationController::rng = std::mt19937(std::random_device{}());
    
    DEBUG_PRINT("simulation_reset_global_state completed");
}

} // extern "C"