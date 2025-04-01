import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import subprocess
import os

def run_parameter_sweep(num_samples=100, num_reruns=5, num_sims=10, num_timesteps=300, output_dir="./results"):
    """Run the parameter sweep using the C++ executable"""
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Get the absolute path to the executable
    current_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(os.path.dirname(os.path.dirname(current_dir)))
    executable_path = os.path.join(project_root,"MV", "build", "bin", "parameter_sweep")
    
    print(f"Looking for executable at: {executable_path}")
    if not os.path.exists(executable_path):
        raise FileNotFoundError(f"Parameter sweep executable not found at: {executable_path}")
    
    # Run the parameter sweep executable
    cmd = [
        executable_path,
        "--samples", str(num_samples),
        "--reruns", str(num_reruns),
        "--sims", str(num_sims),
        "--timesteps", str(num_timesteps),
        "--output", output_dir
    ]
    
    print(f"Running command: {' '.join(cmd)}")
    subprocess.run(cmd, check=True)
    
    # Find the CSV file in the output directory
    csv_files = [f for f in os.listdir(output_dir) if f.endswith('.csv')]
    if not csv_files:
        raise FileNotFoundError("No CSV files found in output directory")
    
    # Read the most recent CSV file
    latest_csv = max(csv_files, key=lambda x: os.path.getctime(os.path.join(output_dir, x)))
    return pd.read_csv(os.path.join(output_dir, latest_csv))

def plot_results(df):
    """Create visualizations of the parameter sweep results"""
    # Set up the plotting style
    plots_dir = os.path.join(".", "plots")
    os.makedirs(plots_dir, exist_ok=True)
    plt.style.use('seaborn')
    df['normalized_prey'] = df['avg_prey'] / df['nr']
    print(df.head())
    # Create a figure with subplots
    fig = plt.figure(figsize=(18, 12))

    # Plot 1: Parameter distributions (boxplot for each parameter)
    ax1 = plt.subplot(231)
    # Use melt to reshape the DataFrame and pass to sns.boxplot
    df_melted = df[['nr', 'dr', 'df', 'rf']].melt(var_name='Parameter', value_name='Value')
    sns.boxplot(x='Parameter', y='Value', data=df_melted, ax=ax1)
    ax1.set_title('Parameter Distributions')
    ax1.set_ylabel('Value')

    # Plot 2: Prey vs Predator populations (with error bars)
    ax2 = plt.subplot(232)
    ax2.errorbar(df['avg_prey'], df['avg_predators'],  # Changed from avg_pred to avg_predators
                 xerr=df['std_prey'], yerr=df['std_predators'],
                 fmt='o', alpha=0.5)
    ax2.set_xlabel('Average Prey Population')
    ax2.set_ylabel('Average Predator Population')
    ax2.set_title('Prey vs Predator Populations')

    # Plot 3: Parameter correlations (heatmap)
    ax3 = plt.subplot(233)
    sns.heatmap(df[['dr', 'df', 'rf', 'nr', 'avg_prey']].corr(), annot=True, cmap='coolwarm', ax=ax3)
    ax3.set_title('Parameter Heatmap')

    # Plot 4: Population stability (scatter of CVs)
    ax4 = plt.subplot(234)
    ax4.scatter(df['std_prey'] / df['avg_prey'],
                df['std_predators'] / df['avg_predators'],
                alpha=0.5)
    ax4.set_xlabel('Prey Population CV')
    ax4.set_ylabel('Predator Population CV')
    ax4.set_title('Population Stability')

    # Plot 5: Distribution of Prey Population (Histogram)
    ax5 = plt.subplot(235)
    
    # Normalize prey population by carrying capacity (nr)
    df = df[np.isfinite(df['normalized_prey'])]
    
    # sns.histplot(df['normalized_prey'], kde=True, bins=20, color='blue', ax=ax5)
    ax5.hist(df['normalized_prey'].to_numpy(), bins='auto', color='blue')

    ax5.set_xlabel('Normalized Prey Population (avg_prey / nr)')
    ax5.set_ylabel('Frequency')
    ax5.set_title('Distribution of Normalized Prey Population')


    # Plot 6: Distribution of Predator Population (Histogram)
    ax6 = plt.subplot(236)
    ax6.hist(df['avg_predators'].to_numpy(), bins='auto', color='red')
    ax6.set_xlabel('Average Predator Population')
    ax6.set_ylabel('Frequency')
    ax6.set_title('Distribution of Predator Population')

    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, 'parameter_sweep_results.png'))
    plt.close()



def main():
    # Run parameter sweep
    print("Running parameter sweep...")
    df = run_parameter_sweep(
        num_samples=500,    # Number of different parameter combinations
        num_reruns=2,       # Number of times to rerun each combination
        num_sims=20,         # Number of simulations per rerun
        num_timesteps=500,  # Number of timesteps per simulation
        output_dir="./results"
    )
    
    # Plot results
    print("Creating visualizations...")
    plot_results(df)
    
    # Print summary statistics
    print("\nSummary Statistics:")
    print(df[['avg_prey', 'avg_predators', 'std_prey', 'std_predators']].describe())

if __name__ == "__main__":
    main() 