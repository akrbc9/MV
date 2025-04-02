#!/usr/bin/env python3
"""
Minimal test script to verify the C API functionality
"""
import ctypes
import os
import sys
import platform

def find_library(name):
    """Find the library in common locations"""
    lib_ext = {
        "Windows": ".dll",
        "Darwin": ".dylib",
        "Linux": ".so"
    }.get(platform.system(), ".so")
    
    possible_paths = [
        f"lib{name}{lib_ext}",
        f"{name}{lib_ext}",
        os.path.join("lib", f"lib{name}{lib_ext}"),
        os.path.join("build", "lib", f"lib{name}{lib_ext}"),
        os.path.join("../build", "lib", f"lib{name}{lib_ext}"),
        os.path.join("python_api", f"lib{name}{lib_ext}"),
        os.path.join("build", "python_api", f"lib{name}{lib_ext}"),
    ]
    
    for path in possible_paths:
        print(f"Checking for library at: {path}")
        if os.path.exists(path):
            print(f"Found library at: {path}")
            return path
    
    # If not found in expected locations, rely on system search path
    return f"lib{name}{lib_ext}"

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

# Try to load the library
try:
    lib_path = find_library("simulation_c_api")
    print(f"Loading library from: {lib_path}")
    sim_lib = ctypes.CDLL(lib_path)
    print("Library loaded successfully")
except Exception as e:
    print(f"Error loading library: {e}")
    sys.exit(1)

# Verify the simulation_reset_global_state function is available
try:
    sim_lib.simulation_reset_global_state.argtypes = []
    sim_lib.simulation_reset_global_state.restype = None
    print("Found simulation_reset_global_state function")
except AttributeError:
    print("ERROR: simulation_reset_global_state function not found")

# Try to create a simulation
print("\nAttempting to create a simulation...")
sim_lib.simulation_create.argtypes = [CSimulationConfig]
sim_lib.simulation_create.restype = ctypes.c_void_p

sim_lib.simulation_initialize.argtypes = [ctypes.c_void_p]
sim_lib.simulation_initialize.restype = None

sim_lib.simulation_destroy.argtypes = [ctypes.c_void_p]
sim_lib.simulation_destroy.restype = None

# Create test configuration
config = CSimulationConfig(
    worldWidth=1.0,
    worldHeight=1.0,
    initialPredators=30,
    initialPrey=500,
    MF=0.05,
    MR=0.03,
    interactionRadius=0.02,
    cellSize=0.02,
    simulationSteps=10,
    NR=500.0,
    RR=0.1,
    DR=1.0,
    DF=0.1,
    RF=0.5
)

try:
    handle = sim_lib.simulation_create(config)
    print(f"Simulation created successfully, handle: {handle}")
    
    if handle:
        print("Initializing simulation...")
        sim_lib.simulation_initialize(handle)
        print("Simulation initialized")
        
        print("Destroying simulation...")
        sim_lib.simulation_destroy(handle)
        print("Simulation destroyed")
        
        print("Resetting global state...")
        sim_lib.simulation_reset_global_state()
        print("Global state reset")
except Exception as e:
    print(f"Error during simulation test: {e}")

print("\nTest completed")