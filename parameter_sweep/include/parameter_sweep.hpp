#pragma once

#include <string>
#include <vector>
#include <tuple>
#include "simulation_config.hpp"
#include "simulation_controller.hpp"
#include "simulation_report.hpp"
#include "thread_pool.hpp"
class ParameterSweep {
public:
    ParameterSweep(const std::string& output_dir);
    void run(int num_samples, int num_reruns, int num_sims, int num_timesteps);
    
private:
    std::string output_dir_;
    std::string generateOutputFilename() const;
    std::tuple<double, double, double, double> runSingleSample(
        const SimulationConfig& config,
        int num_reruns,
        int num_sims,
        int num_timesteps
    );
}; 