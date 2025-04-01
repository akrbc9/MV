#!/usr/bin/env python3

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pyDOE2 import lhs
import subprocess
import os
import sys
from datetime import datetime

def generate_lhs_samples(num_samples, ranges):
    """Generate Latin Hypercube samples for the given parameter ranges."""
    # Create normalized LHS samples (0-1 range)
    lhs_samples = lhs(4, samples=num_samples)
    
    # Scale samples to parameter ranges
    nr_samples = lhs_samples[:, 0] * (ranges['nr_max'] - ranges['nr_min']) + ranges['nr_min']
    dr_samples = lhs_samples[:, 1] * (ranges['dr_max'] - ranges['dr_min']) + ranges['dr_min']
    df_samples = lhs_samples[:, 2] * (ranges['df_max'] - ranges['df_min']) + ranges['df_min']
    rf_samples = lhs_samples[:, 3] * (ranges['rf_max'] - ranges['rf_min']) + ranges['rf_min']
    
    return np.column_stack((nr_samples, dr_samples, df_samples, rf_samples))

def run_simulation(samples, num_reruns, num_sims, num_timesteps):
    """Run the C++ simulation with the given parameter samples."""
    # Create output directory
    output_dir = "results"
    os.makedirs(output_dir, exist_ok=True)
    
    # Run the C++ executable from the build/bin directory
    executable_path = os.path.join("build", "bin", "parameter_sweep")
    cmd = [
        executable_path,
        "--samples", str(len(samples)),
        "--reruns", str(num_reruns),
        "--sims", str(num_sims),
        "--timesteps", str(num_timesteps),
        "--output", output_dir
    ]
    
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running simulation: {e}")
        sys.exit(1)

def plot_results(results_file):
    """Create visualizations of the parameter sweep results."""
    # Read results
    df = pd.read_csv(results_file)
    
    # Create output directory for plots
    plots_dir = "plots"
    os.makedirs(plots_dir, exist_ok=True)
    
    # Plot 1: Parameter distributions
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle("Parameter Distributions")
    
    parameters = ['nr', 'dr', 'df', 'rf']
    titles = ['Carrying Capacity (NR)', 'Death Rate (DR)', 
              'Predator Death Rate (DF)', 'Predator Reproduction Rate (RF)']
    
    for ax, param, title in zip(axes.flat, parameters, titles):
        ax.hist(df[param], bins=20)
        ax.set_title(title)
        ax.set_xlabel(param.upper())
        ax.set_ylabel('Count')
    
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, 'parameter_distributions.png'))
    plt.close()
    
    # Plot 2: Population vs Parameters
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle("Population vs Parameters")
    
    for ax, param, title in zip(axes.flat, parameters, titles):
        ax.scatter(df[param], df['avg_predators'], label='Predators', alpha=0.5)
        ax.scatter(df[param], df['avg_prey'], label='Prey', alpha=0.5)
        ax.set_title(title)
        ax.set_xlabel(param.upper())
        ax.set_ylabel('Population')
        ax.legend()
    
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, 'population_vs_parameters.png'))
    plt.close()
    
    # Plot 3: Standard Deviations
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle("Population Standard Deviations vs Parameters")
    
    for ax, param, title in zip(axes.flat, parameters, titles):
        ax.scatter(df[param], df['std_predators'], label='Predators', alpha=0.5)
        ax.scatter(df[param], df['std_prey'], label='Prey', alpha=0.5)
        ax.set_title(title)
        ax.set_xlabel(param.upper())
        ax.set_ylabel('Standard Deviation')
        ax.legend()
    
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, 'std_vs_parameters.png'))
    plt.close()

def main():
    # Parameter ranges
    ranges = {
        'nr_min': 400.0,
        'nr_max': 1000.0,
        'dr_min': 0.0,
        'dr_max': 1.0,
        'df_min': 0.0,
        'df_max': 1.0,
        'rf_min': 0.0,
        'rf_max': 1.0
    }
    
    # Simulation parameters
    num_samples = 100
    num_reruns = 2
    num_sims = 5
    num_timesteps = 300
    
    # Generate samples
    print("Generating Latin Hypercube samples...")
    samples = generate_lhs_samples(num_samples, ranges)
    
    # Run simulation
    print("Running simulations...")
    run_simulation(samples, num_reruns, num_sims, num_timesteps)
    
    # Find the most recent results file
    results_dir = "results"
    results_files = [f for f in os.listdir(results_dir) if f.endswith('.csv')]
    if not results_files:
        print("No results files found!")
        sys.exit(1)
    
    latest_file = max(results_files, key=lambda x: os.path.getctime(os.path.join(results_dir, x)))
    results_file = os.path.join(results_dir, latest_file)
    
    # Create visualizations
    print("Creating visualizations...")
    plot_results(results_file)
    
    print("Parameter sweep completed successfully!")

if __name__ == "__main__":
    main() 