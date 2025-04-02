import ctypes
import os
import platform
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Tuple, Optional
import gc 

# Determine the library extension based on platform
if platform.system() == "Windows":
    lib_extension = ".dll"
elif platform.system() == "Darwin":  # macOS
    lib_extension = ".dylib"
else:  # Linux
    lib_extension = ".so"

# Try to find the library in common locations
def find_library(name: str) -> str:
    possible_paths = [
        os.path.join(os.path.dirname(__file__), f"lib{name}{lib_extension}"),
        os.path.join(os.path.dirname(__file__), f"{name}{lib_extension}"),
        os.path.join("/usr/lib", f"lib{name}{lib_extension}"),
        os.path.join("/usr/local/lib", f"lib{name}{lib_extension}"),
    ]
    
    # Add conda environment lib path if available
    if "CONDA_PREFIX" in os.environ:
        conda_lib = os.path.join(os.environ["CONDA_PREFIX"], "lib", f"lib{name}{lib_extension}")
        possible_paths.insert(0, conda_lib)
    
    for path in possible_paths:
        if os.path.exists(path):
            return path
    
    # If not found, return the default name and let the loader handle it
    return f"lib{name}{lib_extension}"

# Load the C API library
try:
    lib_path = find_library("simulation_c_api")
    sim_lib = ctypes.CDLL(lib_path)
except OSError as e:
    raise ImportError(f"Could not load simulation library: {e}")

# Define structures to match C API
class CSimulationConfig(ctypes.Structure):
    _fields_ = [
        ("worldWidth", ctypes.c_double),
        ("worldHeight", ctypes.c_double),
        ("initialPredators", ctypes.c_int),
        ("initialPrey", ctypes.c_int),
        ("MF", ctypes.c_double),
        ("MR", ctypes.c_double),
        ("interactionRadius", ctypes.c_double),
        ("cellSize", ctypes.c_double),
        ("simulationSteps", ctypes.c_int),
        ("NR", ctypes.c_double),
        ("RR", ctypes.c_double),
        ("DR", ctypes.c_double),
        ("DF", ctypes.c_double),
        ("RF", ctypes.c_double),
    ]

class SimulationStatus(ctypes.Structure):
    _fields_ = [
        ("predatorCount", ctypes.c_int),
        ("preyCount", ctypes.c_int),
        ("currentStep", ctypes.c_int),
        ("isRunning", ctypes.c_int),
        ("isPaused", ctypes.c_int),
    ]

class SimulationResult(ctypes.Structure):
    _fields_ = [
        ("finalPredatorCount", ctypes.c_int),
        ("finalPreyCount", ctypes.c_int),
        ("normalizedPreyCount", ctypes.c_double),
        ("executionTimeMs", ctypes.c_int64),
        ("timeSteps", ctypes.c_int),
        ("predatorHistory", ctypes.POINTER(ctypes.c_int)),
        ("preyHistory", ctypes.POINTER(ctypes.c_int)),
        ("historySize", ctypes.c_int),
    ]

# Define function types
SimulationHandle = ctypes.c_void_p

# Function prototypes
sim_lib.simulation_reset_global_state.argtypes = []
sim_lib.simulation_reset_global_state.restype = None

sim_lib.simulation_create.argtypes = [CSimulationConfig]
sim_lib.simulation_create.restype = SimulationHandle

sim_lib.simulation_initialize.argtypes = [SimulationHandle]
sim_lib.simulation_initialize.restype = None

sim_lib.simulation_step.argtypes = [SimulationHandle]
sim_lib.simulation_step.restype = None

sim_lib.simulation_run.argtypes = [SimulationHandle, ctypes.c_int]
sim_lib.simulation_run.restype = None

sim_lib.simulation_pause.argtypes = [SimulationHandle]
sim_lib.simulation_pause.restype = None

sim_lib.simulation_resume.argtypes = [SimulationHandle]
sim_lib.simulation_resume.restype = None

sim_lib.simulation_end.argtypes = [SimulationHandle]
sim_lib.simulation_end.restype = None

sim_lib.simulation_get_status.argtypes = [SimulationHandle, ctypes.POINTER(SimulationStatus)]
sim_lib.simulation_get_status.restype = None

sim_lib.simulation_get_results.argtypes = [SimulationHandle]
sim_lib.simulation_get_results.restype = SimulationResult

sim_lib.simulation_free_results.argtypes = [ctypes.POINTER(SimulationResult)]
sim_lib.simulation_free_results.restype = None

sim_lib.simulation_destroy.argtypes = [SimulationHandle]
sim_lib.simulation_destroy.restype = None

# sim_lib.simulation_reset_global_state.argtypes = []
# sim_lib.simulation_reset_global_state.restype = None

class PredatorPreySimulation:
    """Python wrapper for the C++ Predator-Prey simulation"""
    
    def __init__(self, config: Dict = None):
        """
        Initialize a new simulation with the given configuration.
        
        Args:
            config: Dictionary with simulation parameters
                   If None, default parameters will be used.
        """
        # Default configuration
        default_config = {
            "worldWidth": 1.0,
            "worldHeight": 1.0,
            "initialPredators": 30,
            "initialPrey": 500,
            "MF": 0.05,                # Movement magnitude for predators
            "MR": 0.03,                # Movement magnitude for prey
            "interactionRadius": 0.02, # Interaction radius
            "cellSize": 0.02,          # Grid cell size
            "simulationSteps": 1000,   # Max timesteps
            "NR": 500.0,               # Carrying capacity of Prey
            "RR": 0.1,                 # Reproduction rate of Prey
            "DR": 1.0,                 # Death rate of prey when encountering predator
            "DF": 0.1,                 # Death rate of predator when no prey around
            "RF": 0.5,                 # Reproduction rate of predator
        }
        
        # Merge with provided config
        if config:
            default_config.update(config)

        # Save config here 
        self.config = default_config

        # Create C config structure
        c_config = CSimulationConfig()
        for key, value in default_config.items():
            setattr(c_config, key, value)
        
        # Create simulation
        self.handle = sim_lib.simulation_create(c_config)
        self.initialized = False
        self._result = None
    
    def initialize(self):
        """Initialize the simulation, creating the initial agent population"""
        sim_lib.simulation_initialize(self.handle)
        self.initialized = True
    
    def step(self):
        """Run a single timestep of the simulation"""
        if not self.initialized:
            self.initialize()
        sim_lib.simulation_step(self.handle)
    
    def run(self, steps: int = None):
        """
        Run the simulation for the specified number of steps
        
        Args:
            steps: Number of steps to run. If None, uses simulationSteps from config.
        """
        if not self.initialized:
            self.initialize()
        
        if steps is None:
            # Get status to determine steps remaining
            steps = self.config["simulationSteps"]

        
        sim_lib.simulation_run(self.handle, steps)
    
    def pause(self):
        """Pause the simulation"""
        sim_lib.simulation_pause(self.handle)
    
    def resume(self):
        """Resume the simulation from a paused state"""
        sim_lib.simulation_resume(self.handle)
    
    def end(self):
        """End the simulation"""
        sim_lib.simulation_end(self.handle)
    
    def get_status(self) -> Dict:
        """
        Get the current simulation status
        
        Returns:
            Dictionary containing current status information
        """
        status = SimulationStatus()
        sim_lib.simulation_get_status(self.handle, ctypes.byref(status))
        
        return {
            "predatorCount": status.predatorCount,
            "preyCount": status.preyCount,
            "currentStep": status.currentStep,
            "isRunning": bool(status.isRunning),
            "isPaused": bool(status.isPaused),
        }
    
    def get_results(self) -> Dict:
        """
        Get the simulation results
        
        Returns:
            Dictionary containing simulation results
        """
        # Free previous results if they exist
        if self._result:
            sim_lib.simulation_free_results(ctypes.byref(self._result))
        
        # Get new results
        self._result = sim_lib.simulation_get_results(self.handle)
        
        # Convert C arrays to Python lists
        predator_history = []
        prey_history = []
        
        if self._result.historySize > 0:
            predator_history = [self._result.predatorHistory[i] for i in range(self._result.historySize)]
            prey_history = [self._result.preyHistory[i] for i in range(self._result.historySize)]
        
        return {
            "finalPredatorCount": self._result.finalPredatorCount,
            "finalPreyCount": self._result.finalPreyCount,
            "normalizedPreyCount": self._result.normalizedPreyCount,
            "executionTimeMs": self._result.executionTimeMs,
            "timeSteps": self._result.timeSteps,
            "predatorHistory": predator_history,
            "preyHistory": prey_history,
        }
    
    def plot_results(self, save_path: Optional[str] = None):
        """
        Plot the simulation results
        
        Args:
            save_path: If provided, save the plot to this path
        """
        results = self.get_results()
        
        plt.figure(figsize=(10, 6))
        plt.plot(results["predatorHistory"], 'r-', label='Predators')
        plt.plot(results["preyHistory"], 'b-', label='Prey')
        plt.xlabel('Time Step')
        plt.ylabel('Population')
        plt.title('Predator-Prey Population Dynamics')
        plt.legend()
        plt.grid(True)
        
        if save_path:
            plt.savefig(save_path)
        
        plt.show()
    
    def __del__(self):
        """Clean up resources when the object is destroyed"""
        if hasattr(self, 'handle') and self.handle:
            # Free results if they exist
            if self._result:
                sim_lib.simulation_free_results(ctypes.byref(self._result))
            
            # Destroy simulation
            sim_lib.simulation_destroy(self.handle)
            
            # Reset global state
            sim_lib.simulation_reset_global_state()

def reset_global_state():
    """Reset global state for fresh simulations"""
    sim_lib.simulation_reset_global_state()

# Example for main function in predator_prey_sim.py
if __name__ == "__main__":
    print("\n--- First Simulation ---")
    # Create a simulation with default parameters
    sim = PredatorPreySimulation()
    print("Simulation object created")
    
    # Initialize and run
    print("Initializing simulation...")
    sim.initialize()
    print("Running simulation...")
    sim.run()
    print("Ending simulation...")
    sim.end()
    
    # Get and print results
    results = sim.get_results()
    print(f"Final Predator Count: {results['finalPredatorCount']}")
    print(f"Final Prey Count: {results['finalPreyCount']}")
    print(f"Execution Time: {results['executionTimeMs']}ms")

    # Explicitly clean up the first simulation
    print("Cleaning up first simulation...")
    if hasattr(sim, '_result') and sim._result:
        print("Freeing simulation results...")
        sim_lib.simulation_free_results(ctypes.byref(sim._result))
        sim._result = None
    
    if hasattr(sim, 'handle') and sim.handle:
        print("Destroying simulation...")
        sim_lib.simulation_destroy(sim.handle)
        sim.handle = None
    
    # Try to reset global state
    print("Attempting to reset global state...")
    try:
        reset_global_state()
    except Exception as e:
        print(f"Error resetting global state: {e}")
    
    # Force garbage collection
    print("Forcing garbage collection...")
    del sim
    gc.collect()
    sim = None
    print("First simulation cleanup complete")
    
    print("\n--- Second Simulation ---")
    # Create a new simulation
    print("Creating new simulation object...")
    sim = PredatorPreySimulation()
    
    # Initialize and run
    print("Initializing simulation...")
    sim.initialize()
    print("Running simulation...")
    sim.run()
    print("Ending simulation...")
    sim.end()
    
    # Get and print results
    results = sim.get_results()
    print(f"Final Predator Count: {results['finalPredatorCount']}")
    print(f"Final Prey Count: {results['finalPreyCount']}")
    print(f"Execution Time: {results['executionTimeMs']}ms")
    
    # Plot results
    # print("Plotting results (comment out if not needed)...")
    # sim.plot_results()