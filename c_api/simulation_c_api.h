#pragma once

#ifdef _WIN32
    #ifdef SIMULATION_C_API_EXPORTS
        #define SIMULATION_API __declspec(dllexport)
    #else
        #define SIMULATION_API __declspec(dllimport)
    #endif
#else
    #define SIMULATION_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Opaque handle to the simulation
typedef void* SimulationHandle;

// Simulation configuration
typedef struct {
    double worldWidth;
    double worldHeight;
    int initialPredators;
    int initialPrey;
    double MF;  // Movement magnitude for predators
    double MR;  // Movement magnitude for prey
    double interactionRadius;
    double cellSize;
    int simulationSteps;
    double NR;    // Carrying capacity of Prey
    double RR;    // Reproduction rate of Prey
    double DR;    // Death rate of prey when encountering predator
    double DF;    // Death rate of predator when no prey around
    double RF;    // Reproduction rate of predator
} CSimulationConfig;

// Simulation status
typedef struct {
    int predatorCount;
    int preyCount;
    int currentStep;
    int isRunning;
    int isPaused;
} SimulationStatus;

// Simulation results
typedef struct {
    int finalPredatorCount;
    int finalPreyCount;
    double normalizedPreyCount;   // Final prey count normalized by carrying capacity
    long long executionTimeMs;    // Execution time in milliseconds
    int timeSteps;                // Number of timesteps executed
    int* predatorHistory;         // Array of predator counts for each timestep
    int* preyHistory;             // Array of prey counts for each timestep
    int historySize;              // Size of the history arrays
} SimulationResult;

// API Functions

// Create a new simulation with the given configuration
SIMULATION_API SimulationHandle simulation_create(CSimulationConfig config);

// Initialize the simulation
SIMULATION_API void simulation_initialize(SimulationHandle simulation);

// Run a single step of the simulation
SIMULATION_API void simulation_step(SimulationHandle simulation);

// Run the simulation for a specified number of steps
SIMULATION_API void simulation_run(SimulationHandle simulation, int steps);

// Pause the simulation
SIMULATION_API void simulation_pause(SimulationHandle simulation);

// Resume a paused simulation
SIMULATION_API void simulation_resume(SimulationHandle simulation);

// End the simulation
SIMULATION_API void simulation_end(SimulationHandle simulation);

// Get the current simulation status
SIMULATION_API void simulation_get_status(SimulationHandle simulation, SimulationStatus* status);

// Get the simulation results (memory is owned by the C API and must be freed with simulation_free_results)
SIMULATION_API SimulationResult simulation_get_results(SimulationHandle simulation);

// Free the memory used by simulation results
SIMULATION_API void simulation_free_results(SimulationResult* results);

// Destroy the simulation and free all resources
SIMULATION_API void simulation_destroy(SimulationHandle simulation);

// Reset global state (agent IDs, RNGs, etc.) to prepare for a fresh simulation
SIMULATION_API void simulation_reset_global_state();

#ifdef __cplusplus
}
#endif