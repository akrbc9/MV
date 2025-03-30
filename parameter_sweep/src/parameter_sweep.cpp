#include "parameter_sweep.hpp"
#include "lhs_sampler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include <numeric>
#include <cmath>
#include <future>  // For async and future
#include "thread_pool.hpp"

namespace fs = std::__fs::filesystem;

ParameterSweep::ParameterSweep(const std::string& output_dir)
    : output_dir_(output_dir) {
    fs::create_directories(output_dir_);
}

void ParameterSweep::run(int num_samples, int num_reruns, int num_sims, int num_timesteps) {
    std::cout << "Starting parameter sweep..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<std::string> output_lines;  // Buffer for storing result lines

    // Define parameter ranges for LHS
    std::vector<LHSSampler::ParameterRange> ranges = {
        {100.0, 1000.0},  // NR (carrying capacity)
        {0.5, 2.0},       // DR (death rate)
        {0.05, 0.2},      // DF (predator death rate)
        {0.3, 0.7}        // RF (predator reproduction rate)
    };

    // Create LHS sampler
    LHSSampler sampler(ranges, num_samples);
    auto samples = sampler.generateAllSamples();

    // Get the number of hardware threads
    unsigned int num_threads = std::min(static_cast<unsigned int>(std::thread::hardware_concurrency()), static_cast<unsigned int>(num_samples));

    // Create a thread pool
    ThreadPool pool(num_threads);

    // Enqueue tasks in the thread pool
    for (int i = 0; i < num_samples; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();

            // Generate sample configuration
            SimulationConfig config;
            config.NR = samples[i][0];  // Carrying capacity
            config.DR = samples[i][1];  // Death rate
            config.DF = samples[i][2];  // Predator death rate
            config.RF = samples[i][3];  // Predator reproduction rate

            // Set other parameters to default values
            config.worldWidth = 1.0;
            config.worldHeight = 1.0;
            config.initialPredators = 30;
            config.initialPrey = 500;
            config.MF = 0.05;
            config.MR = 0.03;
            config.interactionRadius = 0.02;
            config.cellSize = 0.02;
            config.simulationSteps = num_timesteps;
            config.randomizeInitialPositions = true;
            config.RR = 0.1;
            config.saveStatistics = true;
            config.outputFile = "simulation_stats.csv";

            // Run single sample and get results
            auto [avg_prey, std_prey, avg_pred, std_pred] = runSingleSample(config, num_reruns, num_sims, num_timesteps);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            std::cout << "Completed sample " << (i + 1) << " of " << num_samples 
                      << " in " << duration.count() << " milliseconds." << std::endl;

            // Format result line
            std::stringstream line;
            line << i << ","
                 << config.NR << ","
                 << config.DR << ","
                 << config.DF << ","
                 << config.RF << ","
                 << avg_prey << ","
                 << std_prey << ","
                 << avg_pred << ","
                 << std_pred << "\n";

            // Append the result line to the buffer (output_lines)
            output_lines.push_back(line.str());
    }

    // Wait for all tasks to finish
    // The ThreadPool destructor will join all threads automatically

    // Write all results to file at once (this is done after all simulations are complete)
    std::string filename = generateOutputFilename();
    std::ofstream outfile(filename);
    outfile << "sample,nr,dr,df,rf,avg_prey,std_prey,avg_predators,std_predators\n";

    // Write the buffered results
    for (const auto& line : output_lines) {
        outfile << line;
    }

    std::cout << "Parameter sweep completed. Results saved to: " << filename << std::endl;
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Total time taken: " << duration.count() << " milliseconds" << std::endl;
}

std::tuple<double, double, double, double> ParameterSweep::runSingleSample(
    const SimulationConfig& config,
    int num_reruns,
    int num_sims,
    int num_timesteps
) {
    std::vector<double> prey_means;
    std::vector<double> pred_means;
    
    // Run multiple reruns
    for (int rerun = 0; rerun < num_reruns; ++rerun) {
        std::vector<double> prey_counts;
        std::vector<double> pred_counts;
        
        // Run multiple simulations
        auto t1 = std::chrono::high_resolution_clock::now();
        for (int sim = 0; sim < num_sims; ++sim) {
            SimulationController controller(config);
            
            // Run simulation for specified timesteps
            controller.initialize();
            controller.runForTimesteps(num_timesteps);
            controller.end();

            
            // Get final population counts
            auto report = controller.getReport();
            prey_counts.push_back(report.getPreyCount());
            pred_counts.push_back(report.getPredatorCount());
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

        // Calculate means for this rerun
        double prey_mean = std::accumulate(prey_counts.begin(), prey_counts.end(), 0.0) / prey_counts.size();
        double pred_mean = std::accumulate(pred_counts.begin(), pred_counts.end(), 0.0) / pred_counts.size();
        
        prey_means.push_back(prey_mean);
        pred_means.push_back(pred_mean);
    }

    // Calculate overall statistics
    double prey_avg = std::accumulate(prey_means.begin(), prey_means.end(), 0.0) / prey_means.size();
    double pred_avg = std::accumulate(pred_means.begin(), pred_means.end(), 0.0) / pred_means.size();
    
    // Calculate standard deviations
    double prey_std = 0.0;
    double pred_std = 0.0;
    
    for (double mean : prey_means) {
        prey_std += std::pow(mean - prey_avg, 2);
    }
    for (double mean : pred_means) {
        pred_std += std::pow(mean - pred_avg, 2);
    }
    
    prey_std = std::sqrt(prey_std / (prey_means.size() - 1));
    pred_std = std::sqrt(pred_std / (pred_means.size() - 1));
    
    return {prey_avg, prey_std, pred_avg, pred_std};
}

std::string ParameterSweep::generateOutputFilename() const {
    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    ss << output_dir_ << "/sweep_" 
       << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S")
       << ".csv";
    return ss.str();
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    int num_samples = 0;
    int num_reruns = 0;
    int num_sims = 0;
    int num_timesteps = 0;
    std::string output_dir;

    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (i + 1 >= argc) {
            std::cerr << "Missing value for argument " << arg << std::endl;
            return 1;
        }
        
        if (arg == "--samples") {
            num_samples = std::stoi(argv[i + 1]);
        } else if (arg == "--reruns") {
            num_reruns = std::stoi(argv[i + 1]);
        } else if (arg == "--sims") {
            num_sims = std::stoi(argv[i + 1]);
        } else if (arg == "--timesteps") {
            num_timesteps = std::stoi(argv[i + 1]);
        } else if (arg == "--output") {
            output_dir = argv[i + 1];
        }
    }

    // Validate arguments
    if (num_samples <= 0 || num_reruns <= 0 || num_sims <= 0 || num_timesteps <= 0 || output_dir.empty()) {
        std::cerr << "Invalid or missing arguments" << std::endl;
        std::cerr << "Usage: " << argv[0] << " --samples N --reruns N --sims N --timesteps N --output DIR" << std::endl;
        return 1;
    }

    try {
        ParameterSweep sweep(output_dir);
        sweep.run(num_samples, num_reruns, num_sims, num_timesteps);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
