from predator_prey_sim import PredatorPreySimulation
import numpy as np
import matplotlib.pyplot as plt

def run_basic_simulation():
    """Run a basic simulation with default parameters"""
    # Create simulation
    sim = PredatorPreySimulation()
    
    # Initialize and run
    sim.initialize()
    sim.run()
    
    # Get and display results
    results = sim.get_results()
    print(f"Simulation completed in {results['executionTimeMs']}ms")
    print(f"Final populations: {results['finalPredatorCount']} predators, {results['finalPreyCount']} prey")
    
    # Plot results
    sim.plot_results()

def run_parameter_sweep():
    """Run a parameter sweep to explore different reproduction rates"""
    # Parameters to test
    predator_repro_rates = np.linspace(0.1, 0.9, 5)  # RF values
    prey_repro_rates = np.linspace(0.05, 0.2, 5)     # RR values
    
    results = []
    
    # Run simulations for each parameter combination
    for rf in predator_repro_rates:
        for rr in prey_repro_rates:
            config = {
                "RF": rf,       # Predator reproduction rate
                "RR": rr,       # Prey reproduction rate
                "simulationSteps": 500,
            }
            
            sim = PredatorPreySimulation(config)
            sim.initialize()
            sim.run()
            sim.end()
            # Store results
            sim_results = sim.get_results()
            results.append({
                "RF": rf,
                "RR": rr,
                "finalPredatorCount": sim_results["finalPredatorCount"],
                "finalPreyCount": sim_results["finalPreyCount"],
                "normalizedPreyCount": sim_results["normalizedPreyCount"],
            })
            
            print(f"Completed RF={rf:.2f}, RR={rr:.2f}: {sim_results['finalPredatorCount']} predators, {sim_results['finalPreyCount']} prey")
    
    # Create heatmap of results
    plot_parameter_sweep(results, predator_repro_rates, prey_repro_rates)

def plot_parameter_sweep(results, predator_rates, prey_rates):
    """Plot heatmaps of the parameter sweep results"""
    # Prepare data arrays
    pred_array = np.zeros((len(predator_rates), len(prey_rates)))
    prey_array = np.zeros((len(predator_rates), len(prey_rates)))
    
    # Fill arrays
    for res in results:
        i = np.where(predator_rates == res["RF"])[0][0]
        j = np.where(prey_rates == res["RR"])[0][0]
        pred_array[i, j] = res["finalPredatorCount"]
        prey_array[i, j] = res["finalPreyCount"]
    
    # Create figure with two subplots
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
    
    # Plot predator heatmap
    im1 = ax1.imshow(pred_array, cmap='hot', aspect='auto')
    ax1.set_title('Final Predator Population')
    ax1.set_xlabel('Prey Reproduction Rate (RR)')
    ax1.set_ylabel('Predator Reproduction Rate (RF)')
    ax1.set_xticks(range(len(prey_rates)))
    ax1.set_yticks(range(len(predator_rates)))
    ax1.set_xticklabels([f"{r:.2f}" for r in prey_rates])
    ax1.set_yticklabels([f"{r:.2f}" for r in predator_rates])
    fig.colorbar(im1, ax=ax1, label='Population')
    
    # Plot prey heatmap
    im2 = ax2.imshow(prey_array, cmap='cool', aspect='auto')
    ax2.set_title('Final Prey Population')
    ax2.set_xlabel('Prey Reproduction Rate (RR)')
    ax2.set_ylabel('Predator Reproduction Rate (RF)')
    ax2.set_xticks(range(len(prey_rates)))
    ax2.set_yticks(range(len(predator_rates)))
    ax2.set_xticklabels([f"{r:.2f}" for r in prey_rates])
    ax2.set_yticklabels([f"{r:.2f}" for r in predator_rates])
    fig.colorbar(im2, ax=ax2, label='Population')
    
    plt.tight_layout()
    plt.savefig('parameter_sweep_results.png')
    plt.show()

def run_custom_configuration():
    """Run a simulation with custom parameters"""
    config = {
        "initialPredators": 50,
        "initialPrey": 300,
        "MF": 0.07,             # Faster predator movement
        "MR": 0.05,             # Faster prey movement
        "interactionRadius": 0.03,
        "NR": 1000.0,           # Higher carrying capacity
        "RR": 0.15,             # Higher prey reproduction
        "DR": 0.8,              # Lower prey death rate
        "DF": 0.15,             # Higher predator death rate
        "RF": 0.6,              # Higher predator reproduction
        "simulationSteps": 2000,
    }
    
    sim = PredatorPreySimulation(config)
    sim.initialize()
    sim.run()
    sim.end()
    # Get and plot results
    results = sim.get_results()
    print(f"Custom simulation completed in {results['executionTimeMs']}ms")
    print(f"Final populations: {results['finalPredatorCount']} predators, {results['finalPreyCount']} prey")
    
    # Plot with custom title
    plt.figure(figsize=(10, 6))
    plt.plot(results["predatorHistory"], 'r-', label='Predators')
    plt.plot(results["preyHistory"], 'b-', label='Prey')
    plt.xlabel('Time Step')
    plt.ylabel('Population')
    plt.title('Custom Predator-Prey Simulation')
    plt.legend()
    plt.grid(True)
    plt.savefig('custom_simulation_results.png')
    plt.show()

if __name__ == "__main__":
    # Choose which example to run
    run_basic_simulation()
    # run_parameter_sweep()
    # run_custom_configuration()