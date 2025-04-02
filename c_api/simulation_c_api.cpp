#define SIMULATION_C_API_EXPORTS 1
#include "simulation_c_api.h"
#include "simulation_controller.hpp"
#include "agent.hpp"
#include <memory>
#include <cstring>
#include <vector>

// Structure to hold simulation data
struct SimulationData {
    std::unique_ptr<SimulationController> controller;
    CSimulationConfig config;
    std::vector<int> predatorHistory;
    std::vector<int> preyHistory;
};

SimulationHandle simulation_create(CSimulationConfig cConfig) {
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
    data->config = cConfig;
    data->controller = std::make_unique<SimulationController>(config);
    
    return data;
}

void simulation_initialize(SimulationHandle simulation) {
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->initialize();
}

void simulation_step(SimulationHandle simulation) {
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->run();
}

void simulation_run(SimulationHandle simulation, int steps) {
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->runForTimesteps(steps);
}

void simulation_pause(SimulationHandle simulation) {
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->pause();
}

void simulation_resume(SimulationHandle simulation) {
    // This would need implementation in the controller
}

void simulation_end(SimulationHandle simulation) {
    auto data = static_cast<SimulationData*>(simulation);
    data->controller->end();
}

void simulation_get_status(SimulationHandle simulation, SimulationStatus* status) {
    auto data = static_cast<SimulationData*>(simulation);
    
    // Fill status structure
    status->predatorCount = data->controller->getCurrentPredatorCount();
    status->preyCount = data->controller->getCurrentPreyCount();
    status->currentStep = data->controller->getCurrentStep();
    status->isRunning = data->controller->isSimulationRunning() ? 1 : 0;
    status->isPaused = data->controller->isSimulationPaused() ? 1 : 0;
}

SimulationResult simulation_get_results(SimulationHandle simulation) {
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
    
    return result;
}

void simulation_free_results(SimulationResult* results) {
    // No need to free anything here since we're using vectors in SimulationData
    // that stay alive until the simulation is destroyed
}

void simulation_destroy(SimulationHandle simulation) {
    auto data = static_cast<SimulationData*>(simulation);
    delete data;
}

// New function to reset global state
void simulation_reset_global_state() {
    // Reset agent ID counter
    Agent::resetIdCounter();
    
    // Reset the static RNG in SimulationController
    SimulationController::rng = std::mt19937(std::random_device{}());
    
    // Reset any other global state as needed
}