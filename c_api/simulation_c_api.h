#ifndef SIMULATION_C_API_H
#define SIMULATION_C_API_H

// Define export macro for proper symbol visibility
#if defined(_MSC_VER)
    // Windows/MSVC
    #ifdef SIMULATION_C_API_EXPORTS
        #define API_EXPORT __declspec(dllexport)
    #else
        #define API_EXPORT __declspec(dllimport)
    #endif
#else
    // GCC/Clang
    #define API_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include<stdio.h>
/**
 * Simulation handle - an opaque pointer to the internal simulation object
 */
typedef struct Simulation_t* SimulationHandle;

/**
 * Simulation configuration parameters - C API version
 */
typedef struct {
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
    double interactionRadius;
    double cellSize;
    
    // Simulation parameters
    int simulationSteps;
    
    // Population dynamics parameters
    double NR;    // Carrying capacity of Prey
    double RR;    // Reproduction rate of Prey
    double DR;    // Death rate of prey when encountering predator
    double DF;    // Death rate of predator when no prey around
    double RF;    // Reproduction rate of predator
} CSimulationConfig;

/**
 * Result data structure for final simulation state
 */
typedef struct {
    int finalPredatorCount;
    int finalPreyCount;
    double normalizedPreyCount;
    int64_t executionTimeMs;
    int timeSteps;
    
    // Pointers to history arrays (caller must free these)
    int* predatorHistory;
    int* preyHistory;
    int historySize;
} SimulationResult;

/**
 * Status structure for checking simulation state
 */
typedef struct {
    int predatorCount;
    int preyCount;
    int currentStep;
    int isRunning;
    int isPaused;
} SimulationStatus;

/**
 * Reset all global/static state in the simulation.
 * Call this before creating a new simulation to ensure clean state.
 */
API_EXPORT void simulation_reset_global_state();

/**
 * Create a new simulation with the given configuration
 */
API_EXPORT SimulationHandle simulation_create(CSimulationConfig config);

/**
 * Initialize the simulation, creating the initial population
 */
API_EXPORT void simulation_initialize(SimulationHandle handle);

/**
 * Run the simulation for a single timestep
 */
API_EXPORT void simulation_step(SimulationHandle handle);

/**
 * Run the simulation for the specified number of timesteps
 */
API_EXPORT void simulation_run(SimulationHandle handle, int steps);

/**
 * Pause the simulation
 */
API_EXPORT void simulation_pause(SimulationHandle handle);

/**
 * Resume the simulation from a paused state
 */
API_EXPORT void simulation_resume(SimulationHandle handle);

/**
 * End the simulation
 */
API_EXPORT void simulation_end(SimulationHandle handle);

/**
 * Get the current simulation status
 */
API_EXPORT void simulation_get_status(SimulationHandle handle, SimulationStatus* status);

/**
 * Get the simulation results
 */
API_EXPORT SimulationResult simulation_get_results(SimulationHandle handle);

/**
 * Free a simulation result's dynamically allocated memory
 */
API_EXPORT void simulation_free_results(SimulationResult* result);

/**
 * Destroy a simulation and free all associated resources
 */
API_EXPORT void simulation_destroy(SimulationHandle handle);

#ifdef __cplusplus
}
#endif

#endif // SIMULATION_C_API_H