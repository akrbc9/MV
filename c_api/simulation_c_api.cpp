#define SIMULATION_C_API_EXPORTS 1

#include "simulation_c_api.h"
#include "simulation_controller.hpp"
#include "simulation_config.hpp"
#include "simulation_report.hpp"
#include "agent.hpp"  // Include for Agent::resetIdCounter()
#include <cstring>
#include <memory>
#include <vector>

// Implementation of the simulation handle
struct Simulation_t {
    std::unique_ptr<SimulationController> controller;
    SimulationConfig config;
};

SimulationHandle simulation_create(CSimulationConfig cConfig) {
    // Reset static ID counter before creating a new simulation
    Agent::resetIdCounter();
    
    SimulationHandle handle = new Simulation_t();
    
    // Copy configuration values from C struct to C++ class
    SimulationConfig cppConfig;
    cppConfig.worldWidth = cConfig.worldWidth;
    cppConfig.worldHeight = cConfig.worldHeight;
    cppConfig.initialPredators = cConfig.initialPredators;
    cppConfig.initialPrey = cConfig.initialPrey;
    cppConfig.MF = cConfig.MF;
    cppConfig.MR = cConfig.MR;
    cppConfig.interactionRadius = cConfig.interactionRadius;
    cppConfig.cellSize = cConfig.cellSize;
    cppConfig.simulationSteps = cConfig.simulationSteps;
    cppConfig.NR = cConfig.NR;
    cppConfig.RR = cConfig.RR;
    cppConfig.DR = cConfig.DR;
    cppConfig.DF = cConfig.DF;
    cppConfig.RF = cConfig.RF;
    
    // Store config for later use
    handle->config = cppConfig;
    
    // Create controller
    handle->controller = std::make_unique<SimulationController>(cppConfig);
    
    return handle;
}

void simulation_initialize(SimulationHandle handle) {
    if (handle && handle->controller) {
        // First, ensure we have a clean grid
        handle->controller->getGrid().clearAll();
        handle->controller->initialize();
    }
}

void simulation_step(SimulationHandle handle) {
    if (handle && handle->controller) {
        handle->controller->run();
    }
}

void simulation_run(SimulationHandle handle, int steps) {
    if (handle && handle->controller) {
        handle->controller->runForTimesteps(steps);
    }
}

void simulation_pause(SimulationHandle handle) {
    if (handle && handle->controller) {
        handle->controller->pause();
    }
}

void simulation_resume(SimulationHandle handle) {
    if (handle && handle->controller) {
        // There's no explicit resume, but we can toggle pause state
        if (handle->controller->isSimulationPaused()) {
            handle->controller->pause(); // This will toggle the pause state
        }
    }
}

void simulation_end(SimulationHandle handle) {
    if (handle && handle->controller) {
        handle->controller->end();
    }
}

void simulation_get_status(SimulationHandle handle, SimulationStatus* status) {
    if (!handle || !handle->controller || !status) return;
    
    status->predatorCount = handle->controller->getCurrentPredatorCount();
    status->preyCount = handle->controller->getCurrentPreyCount();
    status->currentStep = handle->controller->getCurrentStep();
    status->isRunning = handle->controller->isSimulationRunning() ? 1 : 0;
    status->isPaused = handle->controller->isSimulationPaused() ? 1 : 0;
}

SimulationResult simulation_get_results(SimulationHandle handle) {
    SimulationResult result = {0};
    
    if (!handle || !handle->controller) return result;
    
    SimulationReport report = handle->controller->getReport();
    
    // Copy basic statistics
    result.finalPredatorCount = report.getPredatorCount();
    result.finalPreyCount = report.getPreyCount();
    result.normalizedPreyCount = report.getNormalizedPreyCount();
    result.executionTimeMs = report.executionTime.count();
    result.timeSteps = report.getTimeSteps();
    
    // Copy history arrays
    auto predatorHistory = report.getPredatorHistory();
    auto preyHistory = report.getPreyHistory();
    
    result.historySize = predatorHistory.size();
    
    // Use std::vector to manage memory automatically
    if (result.historySize > 0) {
        // Allocate memory on the heap with vector
        int* predHistPtr = new int[result.historySize];
        int* preyHistPtr = new int[result.historySize];
        
        // Copy data
        std::copy(predatorHistory.begin(), predatorHistory.end(), predHistPtr);
        std::copy(preyHistory.begin(), preyHistory.end(), preyHistPtr);
        
        result.predatorHistory = predHistPtr;
        result.preyHistory = preyHistPtr;
    }
    
    return result;
}

void simulation_free_results(SimulationResult* result) {
    if (!result) return;
    
    // Ensure safe deletion
    if (result->predatorHistory) {
        delete[] result->predatorHistory;
        result->predatorHistory = nullptr;
    }
    
    if (result->preyHistory) {
        delete[] result->preyHistory;
        result->preyHistory = nullptr;
    }
    
    // Reset other fields
    result->historySize = 0;
}

void simulation_reset_global_state() {
    // Reset any global or static state
    Agent::resetIdCounter();  // Assuming this method exists
}

void simulation_destroy(SimulationHandle handle) {
    if (handle) {
        if (handle->controller) {
            // Ensure grid is cleared to release all agent references
            handle->controller->getGrid().clearAll();
        }
        
        // Delete the handle (controller will be cleaned up by unique_ptr)
        delete handle;
    }
}