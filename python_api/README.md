# Predator-Prey Simulation Python API

This package provides a Python API for the C++ Predator-Prey simulation. It allows you to initialize, run, and analyze simulation results with a simple interface.

## Installation

### Prerequisites
- A C++ compiler (GCC, Clang, or MSVC)
- CMake (3.10 or higher)
- Python 3.6 or higher

### Building the C++ Library

1. Add the provided C API files to your existing project:
   - `simulation_c_api.h`
   - `simulation_c_api.cpp`

2. Update your CMakeLists.txt with the provided additions:
   ```cmake
   # Add shared library for C API
   add_library(simulation_c_api SHARED
       simulation_c_api.cpp
       simulation_c_api.h
   )
   
   # Link against the core simulation code
   target_link_libraries(simulation_c_api
       simulation_core  # Replace with your actual core library name
   )
   
   # Make sure the C API is exported properly
   set_target_properties(simulation_c_api PROPERTIES
       CXX_VISIBILITY_PRESET hidden
       VISIBILITY_INLINES_HIDDEN ON
       POSITION_INDEPENDENT_CODE ON
       MACOSX_RPATH ON
   )
   ```

3. Build the library:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. Install the Python module:
   ```bash
   # Copy the built library to the same directory as predator_prey_sim.py
   cp build/libsimulation_c_api.* /path/to/python/module/
   ```

## Usage

### Basic Simulation

```python
from predator_prey_sim import PredatorPreySimulation

# Create a simulation with default parameters
sim = PredatorPreySimulation()

# Initialize and run
sim.initialize()
sim.run()

# Get results
results = sim.get_results()
print(f"Final Predator Count: {results['finalPredatorCount']}")
print(f"Final Prey Count: {results['finalPreyCount']}")

# Plot the population history
sim.plot_results()
```

### Custom Configuration

```python
from predator_prey_sim import PredatorPreySimulation

# Create a simulation with custom parameters
config = {
    "initialPredators": 50,
    "initialPrey": 300,
    "MF": 0.07,             # Predator movement rate
    "MR": 0.05,             # Prey movement rate
    "interactionRadius": 0.03,
    "NR": 1000.0,           # Prey carrying capacity
    "RR": 0.15,             # Prey reproduction rate
    "DR": 0.8,              # Prey death rate
    "DF": 0.15,             # Predator death rate
    "RF": 0.6,              # Predator reproduction rate
    "simulationSteps": 2000,
}

sim = PredatorPreySimulation(config)
sim.initialize()
sim.run()

# Get and plot results
results = sim.get_results()
print(f"Final populations: {results['finalPredatorCount']} predators, {results['finalPreyCount']} prey")
sim.plot_results()
```

### Parameter Sweeps

```python
from predator_prey_sim import PredatorPreySimulation
import numpy as np

# Create an array of parameter values to test
predator_rates = np.linspace(0.1, 0.9, 5)  # RF values
prey_rates = np.linspace(0.05, 0.2, 5)     # RR values

results = []

# Run simulations for each parameter combination
for rf in predator_rates:
    for rr in prey_rates:
        config = {
            "RF": rf,  # Predator reproduction rate
            "RR": rr,  # Prey reproduction rate
            "simulationSteps": 500,
        }
        
        sim = PredatorPreySimulation(config)
        sim.initialize()
        sim.run()
        
        # Store results
        sim_results = sim.get_results()
        results.append({
            "RF": rf,
            "RR": rr,
            "finalPredatorCount": sim_results["finalPredatorCount"],
            "finalPreyCount": sim_results["finalPreyCount"],
        })
        
        print(f"Completed RF={rf:.2f}, RR={rr:.2f}")

# Now you can analyze the results
```

## API Reference

### PredatorPreySimulation

- `__init__(config=None)` - Create a new simulation with optional configuration
- `initialize()` - Initialize the simulation with initial agent populations
- `step()` - Run a single timestep
- `run(steps=None)` - Run multiple timesteps (default: all remaining steps)
- `pause()` - Pause the simulation
- `resume()` - Resume from a paused state
- `end()` - End the simulation and finalize results
- `get_status()` - Get current simulation status
- `get_results()` - Get final simulation results
- `plot_results(save_path=None)` - Plot population histories

### Configuration Parameters

- `worldWidth` - Width of the simulation world (default: 1.0)
- `worldHeight` - Height of the simulation world (default: 1.0)
- `initialPredators` - Initial number of predators (default: 30)
- `initialPrey` - Initial number of prey (default: 500)
- `MF` - Movement magnitude for predators (default: 0.05)
- `MR` - Movement magnitude for prey (default: 0.03)
- `interactionRadius` - Radius for agent interactions (default: 0.02)
- `cellSize` - Size of spatial grid cells (default: 0.02)
- `simulationSteps` - Maximum simulation steps (default: 1000)
- `NR` - Carrying capacity of prey (default: 500.0)
- `RR` - Reproduction rate of prey (default: 0.1)
- `DR` - Death rate of prey when encountering predator (default: 1.0)
- `DF` - Death rate of predator when no prey around (default: 0.1)
- `RF` - Reproduction rate of predator (default: 0.5)

## Troubleshooting

### Library Loading Issues

If you encounter problems loading the shared library:

1. Make sure the library is in the same directory as the Python script or in a directory in your system's library path
2. For conda environments, try setting `LD_LIBRARY_PATH` (Linux) or `DYLD_LIBRARY_PATH` (macOS):
   ```bash
   export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH
   ```

3. Use `find_library()` to locate the library path:
   ```python
   from ctypes.util import find_library
   print(find_library("simulation_c_api"))
   ```

### Memory Management

The simulation object automatically cleans up resources when it's destroyed, but for large simulations or parameter sweeps, you may want to explicitly release resources:

```python
sim.end()
del sim  # Force cleanup
```