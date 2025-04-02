#!/usr/bin/env python3
"""
Parameter sweep script for predator-prey simulation using Latin Hypercube Sampling.
"""
# Results Dictionary:
# "finalPredatorCount": self._result.finalPredatorCount,
# "finalPreyCount": self._result.finalPreyCount,
# "normalizedPreyCount": self._result.normalizedPreyCount,
# "executionTimeMs": self._result.executionTimeMs,
# "timeSteps": self._result.timeSteps,
# "predatorHistory": predator_history,
# "preyHistory": prey_history,
# Question 1D
from pyDOE import lhs
import numpy.matlib as npm
import numpy as np
import matplotlib.pyplot as plt
from predator_prey_sim import PredatorPreySimulation
from tqdm import tqdm
import time
import gc
from matplotlib.tri import Triangulation

# Parameter-Sweep configuration 
def sweep():
    NUM_SAMPLES = 2000
    NUM_RERUNS = 2 # Number of re-runs per parameter sample set 
    NUM_PARAMS = 4 

    # Single Simluation Parameter 
    NUM_RUNS = 5
    TIMESTEPS = 300

    # Set up parameter array 
    # Each row is a new parameter set
    params = npm.repmat(lhs(NUM_PARAMS, samples = NUM_SAMPLES), NUM_RERUNS, 1)

    # Our results array for our histogram
    avg_foxes = np.zeros(NUM_SAMPLES * NUM_RERUNS)
    avg_rabbits_normalized = np.zeros(NUM_SAMPLES * NUM_RERUNS)

    # Run the simulations 
    for i in range(0, NUM_SAMPLES * NUM_RERUNS):
        # Update global parameters 
        nr = params[i,0] * (800 - 200) + 200 # Carrying capacity for rabbits
                                            # Parameter is rescaled to land in interval [200,1000]
        dr = params[i,1] # death probability for rabbits when encountering a fox 
        df = params[i,2] # death probability for foxes when no food is available 
        rf = params[i,3] # reproduction probability for foxes when food is available 
        print("Running for sample #"+ str(i + 1) + "/" + str(NUM_SAMPLES * NUM_RERUNS) + "...")


        num_rabbits = 0
        num_foxes = 0

        for j in range(NUM_RUNS):
            print("\tRunning simulation #"+str(j+1)+"...")
            config = {
                "initialPredators": 50,
                "initialPrey": 300,
                "MF": 0.07,             # Faster predator movement
                "MR": 0.05,             # Faster prey movement
                "interactionRadius": 0.03,
                "NR": nr,           # Higher carrying capacity
                "RR": 0.15,             # Higher prey reproduction
                "DR": dr,              # Lower prey death rate
                "DF": df,             # Higher predator death rate
                "RF": rf,              # Higher predator reproduction
                "simulationSteps": 2000,
            }
            sim = None
            sim = PredatorPreySimulation(config)
            sim.initialize()
            sim.run()
            sim.end()

            results = sim.get_results()
            # Record final number of rabbits and foxes for the run 
            num_rabbits += results['finalPreyCount']
            num_foxes += results['finalPredatorCount']
            print(f"Custom simulation completed in {results['executionTimeMs']}ms")
            print(f"Final populations: {results['finalPredatorCount']} predators, {results['finalPreyCount']} prey")
            print("")
        
        # Compute average number of foxes and rabbits for this parameter sweep
        # Average number of rabbits is normalized by carrying capacity
        avg_rabbits_normalized[i] = num_rabbits/NUM_RUNS/nr
        avg_foxes[i] = num_foxes/NUM_RUNS  

        print("...Done")
        print("\n")

    # Plot histogram for avg_foxes
    plt.figure(figsize=(10, 5))

    plt.subplot(1, 2, 1)  # Create a subplot (1 row, 2 columns, 1st plot)
    plt.hist(avg_foxes, bins=20, color='orange', edgecolor='black')
    plt.title('Histogram of Avg Foxes')
    plt.xlabel('Avg Foxes')
    plt.ylabel('Frequency')

    # Plot histogram for avg_rabbits
    plt.subplot(1, 2, 2)  # Create a subplot (1 row, 2 columns, 2nd plot)
    plt.hist(avg_rabbits_normalized, bins=10, color='green', edgecolor='black')
    plt.title('Histogram of Avg Rabbits')
    plt.xlabel('Avg Rabbits (Normalized by NR)')
    plt.ylabel('Frequency')

    # Adjust layout and show the plot
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    sweep()