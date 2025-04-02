# PredPrey: High-Performance Predator-Prey Simulation

A cache-efficient, multi-threaded C++ simulation engine for predator-prey dynamics with Python integration for visualization and analysis.

## Purpose

PredPrey is a computational ecology toolkit designed to model the complex dynamics between predator and prey populations over time. The simulation is based on a spatial agent-based model where:

- Agents (predators and prey) move randomly within a 2D space
- Interactions occur when agents are within a specified radius of each other
- Population dynamics follow customizable birth and death rates
- Prey growth is limited by carrying capacity
- Predator survival depends on successful predation

Key features:

- High-performance C++ core using spatial partitioning for O(1) neighbor lookups
- Multi-threaded parameter sweeps to explore model sensitivity (!! Mutli-threads currently disabled in main branch.)
- Integration with Python for data analysis and visualization
- Configurable simulation parameters for flexible modeling
- Visualization of population dynamics with SFML (Very basic, just for context in the main C++ component.)

## Project Structure

```
predprey/
├── include/                          # C++ header files
│   ├── agent.hpp                     # Base agent class and derivatives (Predator, Prey)
│   ├── agent_container.hpp           # Optimized container for agent storage
│   ├── forward_decls.hpp             # Forward declarations
│   ├── grid.hpp                      # Spatial grid for efficient agent lookups
│   ├── position.hpp                  # 2D position utilities
│   ├── simulation_config.hpp         # Configuration parameters
│   ├── simulation_context.hpp        # Simulation state and settings
│   ├── simulation_controller.hpp     # Main simulation controller
│   ├── simulation_report.hpp         # Results reporting
│   └── spatial_grid.hpp              # Spatial partitioning
├── src/                              # C++ implementation files
│   ├── agent.cpp                     # Agent behavior implementation
│   ├── grid.cpp                      # Grid implementation
│   ├── main.cpp                      # Entry point for simulation executable
│   ├── simulation_controller.cpp     # Simulation logic
│   └── simulation_report.cpp         # Reporting and visualization
├── parameter_sweep/                  # Parameter sweep functionality
│   ├── include/                      # Parameter sweep headers
│   │   ├── lhs_sampler.hpp           # Latin Hypercube Sampling
│   │   ├── parameter_sweep.hpp       # Parameter sweep controller
│   │   ├── sample_manager.hpp        # Sample generation and management
│   │   └── thread_pool.hpp           # Thread pool for parallel execution
│   └── src/                          # Parameter sweep implementation
│       ├── lhs_sampler.cpp           # LHS implementation
│       ├── parameter_sweep.cpp       # Parameter sweep implementation
│       └── sample_manager.cpp        # Sample management implementation
├── python/                           # Python analysis tools
│   └── analyze_sweep.py              # Script for analyzing parameter sweep results
├── results/                          # Output directory for simulation results
│   └── sweep_*.csv                   # CSV files with parameter sweep results
├── build.sh                          # Build script
├── CMakeLists.txt                    # CMake configuration
└── requirements.txt                  # Python dependencies
```

## Installation

### Prerequisites

- C++17 compatible compiler
- CMake 3.14+
- SFML 3.0+ (for visualization)
- Python 3.7+ (for analysis)
- macOS (current optimization focuses on Apple Silicon)

### Python Dependencies

```
numpy>=1.21.0
pandas>=1.3.0
matplotlib>=3.4.0
seaborn>=0.11.0
```

### Building on macOS (Apple Silicon)

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/predprey.git
   cd predprey
   ```

2. Install dependencies:
   ```bash
   # Install SFML
   brew install sfml

   # Install Python dependencies
   pip install -r requirements.txt
   ```

3. Build the project:
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

   Or manually:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

### Building on Other Platforms

For platforms other than macOS, you'll need to modify the CMakeLists.txt file to remove Apple-specific optimizations:

1. Remove the following lines from CMakeLists.txt:
   ```cmake
   set(DCMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch arm64")
   set(CMAKE_OSX_ARCHITECTURES "arm64")
   set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment version")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8.5-a+crypto+sha3 -mcpu=apple-m1")
   ```

2. Then build as normal:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

## Usage Guide

### Running a Basic Simulation

To run a basic predator-prey simulation:

```bash
./build/bin/predator_prey
```

This will execute the simulation with default parameters (defined in `src/main.cpp`) and display a visualization of the results.

### Core Simulation Parameters

The simulation is controlled by several key parameters:

| Parameter | Description |
|-----------|-------------|
| NR | Carrying capacity for prey population |
| RR | Reproduction rate of prey |
| DR | Death rate of prey when encountering predators |
| DF | Death rate of predators when no prey is nearby |
| RF | Reproduction rate of predators |
| initialPredators | Initial number of predators |
| initialPrey | Initial number of prey |
| worldWidth, worldHeight | Dimensions of the simulation world (!! Currently broken, hard-coded as a 1x1 grid in parts of the code)|
| interactionRadius | Distance within which agents interact |
| MF | Movement magnitude for predators |
| MR | Movement magnitude for prey |
| simulationSteps | Number of steps to run the simulation |

### Running a Parameter Sweep

The parameter sweep functionality allows you to explore how different combinations of parameters affect the simulation outcomes:

```bash
./build/bin/parameter_sweep --samples 100 --reruns 5 --sims 10 --timesteps 300 --output ./results
```

Parameters:
- `--samples`: Number of different parameter combinations to test
- `--reruns`: Number of times to rerun each combination
- `--sims`: Number of simulations per rerun
- `--timesteps`: Number of timesteps per simulation
- `--output`: Directory to save results

### Analyzing Parameter Sweep Results

Use the provided Python script to analyze and visualize parameter sweep results:

```bash
python python/analyze_sweep.py
```

This will:
1. Run a parameter sweep (if not already done)
2. Create visualizations showing relationships between parameters and outcomes
3. Calculate summary statistics
4. Save visualizations to a `plots` directory

## Key C++ Methods

### Agent Behavior

```cpp
// In agent.cpp
AgentAction::Action Predator::getAction(bool hasInteraction) const
```
Determines predator behavior based on interactions with prey. Handles reproduction and death probabilities.

```cpp
AgentAction::Action Prey::getAction(bool hasInteraction) const
```
Controls prey behavior, including reproduction (limited by carrying capacity) and death when encountering predators.

### Simulation Control

```cpp
// In simulation_controller.cpp
void SimulationController::initialize()
```
Sets up the initial state of the simulation, including agent populations.

```cpp
void SimulationController::updateSingleTimestep()
```
Progresses the simulation by one timestep, updating all agents in random order.

```cpp
void SimulationController::runForTimesteps(int numSteps)
```
Runs the simulation for a specified number of timesteps.

### Spatial Grid

```cpp
// In grid.cpp
bool Grid::hasOppositeTypeNeighbor(const std::shared_ptr<Agent>& agent, double radius)
```
Efficiently checks if an agent has neighbors of the opposite type within interaction radius.

```cpp
std::vector<std::shared_ptr<Agent>> Grid::getNearbyAgentsOfType(
    const Position& pos, double radius, Agent::Type type)
```
Retrieves all agents of a specific type within a given radius.

### Parameter Sweep

```cpp
// In parameter_sweep.cpp
void ParameterSweep::run(int num_samples, int num_reruns, int num_sims, int num_timesteps)
```
Runs a full parameter sweep with Latin Hypercube Sampling to efficiently explore parameter space.

## Python Integration

The Python code (`analyze_sweep.py`) provides functionality to:

1. Call the C++ parameter sweep executable:
   ```python
   def run_parameter_sweep(num_samples=100, num_reruns=5, num_sims=10, num_timesteps=300, output_dir="./results")
   ```

2. Create visualizations of the results:
   ```python
   def plot_results(df)
   ```

3. Calculate statistics and summarize findings:
   ```python
   # In main()
   print("\nSummary Statistics:")
   print(df[['normalized_prey', 'avg_predators']].describe())
   ```

## Future Improvements

### Short-term Enhancements

1. **Cross-platform Support**
   - Refactor CMakeLists.txt to better support Linux and Windows
   - Create platform-specific optimization flags

2. **Simulation Capabilities**
   - Add support for multiple predator and prey species
   - Implement more complex movement patterns (directed, flocking)
   - Add environmental factors (resource patches, barriers)

3. **Performance Optimization**
   - Further optimize spatial partitioning for very large simulations
   - Implement SIMD vectorization for agent updates
   - Improve multi-threading with work stealing

### Long-term Roadmap

1. **Extended Features**
   - Evolving agent attributes through genetic algorithms
   - Seasonal/cyclical environmental changes
   - Landscape importation from GIS data
   - Food web support (beyond simple predator-prey)

2. **User Interface**
   - Interactive web-based visualization
   - Real-time parameter adjustment
   - 3D visualization option

3. **Analysis Tools**
   - Machine learning integration for pattern detection
   - Sensitivity analysis toolkit
   - Time-series analysis for cyclic behavior detection

## License

This project is distributed under the MIT License.

## Citation

If you use this code in your research, please cite:

```
Kerbec, A. (2025). PredPrey: A High-Performance Predator-Prey Simulation.
GitHub repository: https://github.com/akrbc9/predprey
```
