#!/usr/bin/env python3
"""
Parameter sweep script for predator-prey simulation using Latin Hypercube Sampling.
With fixes to ensure proper state reset between runs.
"""

from pyDOE import lhs
import numpy.matlib as npm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from predator_prey_sim import PredatorPreySimulation
from tqdm import tqdm
import time
import gc

# Set up parameter sweep
nsamples = 100
reruns = 2
nparams = 4

# Set up parameter array using Latin Hypercube Sampling
params = npm.repmat(lhs(nparams, samples=nsamples), reruns, 1)

def run_parameter_sweep():
    # Define parameter ranges
    nr_range = (200, 800)  # Carrying capacity range
    dr_range = (0, 1)      # Death rate of prey when encountering predator
    df_range = (0.05, 0.3) # Death rate of predator when no prey around (narrowed)
    rf_range = (0.2, 0.8)  # Reproduction rate of predator (narrowed)
    
    # Scale the parameters to their actual ranges
    params_scaled = np.zeros_like(params)
    params_scaled[:, 0] = params[:, 0] * (nr_range[1] - nr_range[0]) + nr_range[0]  # NR
    params_scaled[:, 1] = params[:, 1] * (dr_range[1] - dr_range[0]) + dr_range[0]  # DR
    params_scaled[:, 2] = params[:, 2] * (df_range[1] - df_range[0]) + df_range[0]  # DF
    params_scaled[:, 3] = params[:, 3] * (rf_range[1] - rf_range[0]) + rf_range[0]  # RF
    
    # Set up results storage
    results = []
    
    # Run simulations
    print(f"Running {params_scaled.shape[0]} simulations...")
    for i, param_set in enumerate(tqdm(params_scaled)):
        nr, dr, df, rf = param_set
        
        # Create simulation configuration
        config = {
            "NR": nr,       # Carrying capacity of Prey
            "DR": dr,       # Death rate of prey when encountering predator
            "DF": df,       # Death rate of predator when no prey around
            "RF": rf,       # Reproduction rate of predator
            "simulationSteps": 1000,  # Number of steps to run
        }
        
        try:
            # Create a new simulation for each parameter set
            sim = None
            sim = PredatorPreySimulation(config)
            
            # Debug output
            if i < 5 or i % 20 == 0:
                print(f"\nSimulation {i+1}/{params_scaled.shape[0]}")
                print(f"Parameters: NR={nr:.1f}, DR={dr:.3f}, DF={df:.3f}, RF={rf:.3f}")
            
            # Initialize and run
            sim.initialize()
            sim.run()
            sim.end()  # Important!
            
            # Get results
            sim_results = sim.get_results()
            
            # Debug output
            if i < 5 or i % 20 == 0:
                print(f"  Results: predators={sim_results['finalPredatorCount']}, "
                      f"prey={sim_results['finalPreyCount']}, "
                      f"normalized={sim_results['normalizedPreyCount']:.3f}")
            
            # Store results
            results.append({
                "NR": nr,
                "DR": dr,
                "DF": df,
                "RF": rf,
                "finalPredatorCount": sim_results["finalPredatorCount"],
                "finalPreyCount": sim_results["finalPreyCount"],
                "normalizedPreyCount": sim_results["normalizedPreyCount"],
                "executionTimeMs": sim_results["executionTimeMs"],
            })
            
            # Force cleanup
            del sim
            gc.collect()
            sim = None
            print("")
            
        except Exception as e:
            print(f"Error in simulation {i}: {e}")
            import traceback
            traceback.print_exc()
    
    # Convert results to DataFrame
    results_df = pd.DataFrame(results)
    
    # Save results to CSV
    results_df.to_csv("parameter_sweep_results.csv", index=False)
    print("Results saved to parameter_sweep_results.csv")
    
    return results_df

def visualize_results(results_df):
    """Create all visualizations"""
    # Set up visuals
    sns.set(style="whitegrid", context="notebook", font_scale=1.2)
    
    # 1. Histograms
    plt.figure(figsize=(20, 8))
    
    # Plot normalized prey population
    plt.subplot(1, 2, 1)
    sns.histplot(data=results_df, x="normalizedPreyCount", kde=True, 
                 color="skyblue", edgecolor="darkblue", alpha=0.7)
    plt.title("Distribution of Normalized Prey Population", fontsize=16)
    plt.xlabel("Normalized Prey Population", fontsize=14)
    plt.ylabel("Frequency", fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.7)
    
    # Plot final prey population
    plt.subplot(1, 2, 2)
    sns.histplot(data=results_df, x="finalPreyCount", kde=True, 
                 color="lightgreen", edgecolor="darkgreen", alpha=0.7)
    plt.title("Distribution of Final Prey Population", fontsize=16)
    plt.xlabel("Prey Population", fontsize=14)
    plt.ylabel("Frequency", fontsize=14)
    plt.grid(True, linestyle='--', alpha=0.7)
    
    plt.tight_layout()
    plt.savefig("population_histograms.png", dpi=300)
    plt.show()
    
    # 2. Scatterplots
    # Parameters and outputs
    params = ["NR", "DR", "DF", "RF"]
    param_labels = ["Carrying Capacity (NR)", "Prey Death Rate (DR)", 
                    "Predator Death Rate (DF)", "Predator Reproduction (RF)"]
    
    outputs = ["finalPredatorCount", "finalPreyCount"]
    output_labels = ["Final Predator Count", "Final Prey Count"]
    output_colors = ["crimson", "forestgreen"]
    
    # Create a grid of scatterplots
    fig, axes = plt.subplots(2, 4, figsize=(24, 12))
    
    for i, output in enumerate(outputs):
        for j, param in enumerate(params):
            ax = axes[i, j]
            
            # Create scatterplot with regression line
            sns.regplot(x=param, y=output, data=results_df, 
                       scatter_kws={"alpha": 0.6, "s": 50}, 
                       line_kws={"color": output_colors[i], "lw": 2},
                       ax=ax)
            
            # Calculate correlation
            corr = results_df[[param, output]].corr().iloc[0, 1]
            
            # Set labels and title
            ax.set_xlabel(param_labels[j], fontsize=12)
            ax.set_ylabel(output_labels[i], fontsize=12)
            ax.set_title(f"{output_labels[i]} vs {param_labels[j]}\nCorrelation: {corr:.2f}", 
                        fontsize=14)
            
            # Add grid
            ax.grid(True, linestyle='--', alpha=0.7)
    
    plt.tight_layout()
    plt.savefig("parameter_scatterplots.png", dpi=300)
    plt.show()
    
    # 3. Heatmap of prey population vs NR and RF
    plt.figure(figsize=(12, 10))
    
    # Create a pivot table for the heatmap
    heatmap_data = pd.pivot_table(
        results_df,
        values='finalPreyCount',
        index=pd.cut(results_df['RF'], bins=10), 
        columns=pd.cut(results_df['NR'], bins=10),
        aggfunc='mean'
    )
    
    # Plot heatmap
    sns.heatmap(heatmap_data, cmap='viridis', annot=False, fmt='.0f',
               cbar_kws={'label': 'Final Prey Count'})
    
    plt.title('Final Prey Count as a Function of NR and RF', fontsize=16)
    plt.xlabel('Carrying Capacity (NR)', fontsize=14)
    plt.ylabel('Predator Reproduction Rate (RF)', fontsize=14)
    
    plt.tight_layout()
    plt.savefig("prey_heatmap.png", dpi=300)
    plt.show()

def main():
    start_time = time.time()
    
    # Run parameter sweep
    results_df = run_parameter_sweep()
    
    # Print summary statistics
    print("\nSummary Statistics:")
    print(results_df.describe())
    
    # Create visualizations
    visualize_results(results_df)
    
    elapsed_time = time.time() - start_time
    print(f"Total execution time: {elapsed_time:.2f} seconds")

if __name__ == "__main__":
    main()