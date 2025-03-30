#include "python_bindings.hpp"
#include <matplotlibcpp.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace py = pybind11;
namespace plt = matplotlibcpp;

// Function to run parameter sweep and return results as numpy arrays
py::tuple run_parameter_sweep_py(
    int num_samples,
    int num_reruns,
    int num_sims,
    int num_timesteps,
    const std::string& output_dir
) {
    // Create parameter sweep instance
    ParameterSweep sweep(output_dir);
    
    // Run the sweep
    sweep.run(num_samples, num_reruns, num_sims, num_timesteps);
    
    // Read results from CSV file
    std::vector<double> nr_values, dr_values, df_values, rf_values;
    std::vector<double> avg_prey, std_prey, avg_pred, std_pred;
    
    std::filesystem::path output_path(output_dir);
    for (const auto& entry : std::filesystem::directory_iterator(output_dir)) {
        if (entry.path().extension() == ".csv") {
            std::ifstream file(entry.path());
            std::string line;
            std::getline(file, line); // Skip header
            
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string value;
                std::vector<std::string> values;
                
                while (std::getline(ss, value, ',')) {
                    values.push_back(value);
                }
                
                if (values.size() >= 9) {
                    nr_values.push_back(std::stod(values[1]));
                    dr_values.push_back(std::stod(values[2]));
                    df_values.push_back(std::stod(values[3]));
                    rf_values.push_back(std::stod(values[4]));
                    avg_prey.push_back(std::stod(values[5]));
                    std_prey.push_back(std::stod(values[6]));
                    avg_pred.push_back(std::stod(values[7]));
                    std_pred.push_back(std::stod(values[8]));
                }
            }
            break; // Only process the first CSV file
        }
    }
    
    // Convert to numpy arrays
    py::array_t<double> nr_array(nr_values.size());
    py::array_t<double> dr_array(dr_values.size());
    py::array_t<double> df_array(df_values.size());
    py::array_t<double> rf_array(rf_values.size());
    py::array_t<double> avg_prey_array(avg_prey.size());
    py::array_t<double> std_prey_array(std_prey.size());
    py::array_t<double> avg_pred_array(avg_pred.size());
    py::array_t<double> std_pred_array(std_pred.size());
    
    auto nr_ptr = static_cast<double*>(nr_array.request().ptr);
    auto dr_ptr = static_cast<double*>(dr_array.request().ptr);
    auto df_ptr = static_cast<double*>(df_array.request().ptr);
    auto rf_ptr = static_cast<double*>(rf_array.request().ptr);
    auto avg_prey_ptr = static_cast<double*>(avg_prey_array.request().ptr);
    auto std_prey_ptr = static_cast<double*>(std_prey_array.request().ptr);
    auto avg_pred_ptr = static_cast<double*>(avg_pred_array.request().ptr);
    auto std_pred_ptr = static_cast<double*>(std_pred_array.request().ptr);
    
    for (size_t i = 0; i < nr_values.size(); ++i) {
        nr_ptr[i] = nr_values[i];
        dr_ptr[i] = dr_values[i];
        df_ptr[i] = df_values[i];
        rf_ptr[i] = rf_values[i];
        avg_prey_ptr[i] = avg_prey[i];
        std_prey_ptr[i] = std_prey[i];
        avg_pred_ptr[i] = avg_pred[i];
        std_pred_ptr[i] = std_pred[i];
    }
    
    return py::make_tuple(nr_array, dr_array, df_array, rf_array,
                         avg_prey_array, std_prey_array,
                         avg_pred_array, std_pred_array);
}

// Function to visualize simulation results
void visualize_simulation_py(
    const SimulationReport& report,
    const std::string& title
) {
    std::vector<double> steps(report.getTotalSteps());
    std::iota(steps.begin(), steps.end(), 0.0);
    
    plt::figure_size(1200, 800);
    plt::plot(steps, report.getPreyCounts(), "b-", {{"label", "Prey"}});
    plt::plot(steps, report.getPredatorCounts(), "r-", {{"label", "Predators"}});
    plt::title(title);
    plt::xlabel("Time Step");
    plt::ylabel("Population");
    plt::legend();
    plt::grid(true);
    plt::show();
}

// Function to get population history as numpy arrays
py::tuple get_population_history_py(const SimulationReport& report) {
    auto prey_counts = report.getPreyCounts();
    auto pred_counts = report.getPredatorCounts();
    
    py::array_t<double> prey_array(prey_counts.size());
    py::array_t<double> pred_array(pred_counts.size());
    
    auto prey_ptr = static_cast<double*>(prey_array.request().ptr);
    auto pred_ptr = static_cast<double*>(pred_array.request().ptr);
    
    for (size_t i = 0; i < prey_counts.size(); ++i) {
        prey_ptr[i] = prey_counts[i];
        pred_ptr[i] = pred_counts[i];
    }
    
    return py::make_tuple(prey_array, pred_array);
} 