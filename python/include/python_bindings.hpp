#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "simulation_controller.hpp"
#include "simulation_report.hpp"
#include "parameter_sweep.hpp"

namespace py = pybind11;

// Function to run parameter sweep and return results as numpy arrays
py::tuple run_parameter_sweep_py(
    int num_samples,
    int num_reruns,
    int num_sims,
    int num_timesteps,
    const std::string& output_dir
);

// Function to visualize simulation results
void visualize_simulation_py(
    const SimulationReport& report,
    const std::string& title = "Predator-Prey Simulation"
);

// Function to get population history as numpy arrays
py::tuple get_population_history_py(const SimulationReport& report);

PYBIND11_MODULE(predator_prey, m) {
    m.doc() = "Python bindings for predator-prey simulation"; // module docstring

    // Bind SimulationReport class
    py::class_<SimulationReport>(m, "SimulationReport")
        .def(py::init<>())
        .def("get_prey_count", &SimulationReport::getPreyCount)
        .def("get_predator_count", &SimulationReport::getPredatorCount)
        .def("get_execution_time", &SimulationReport::getExecutionTime)
        .def("get_total_steps", &SimulationReport::getTotalSteps)
        .def("visualize", &visualize_simulation_py)
        .def("get_population_history", &get_population_history_py);

    // Bind SimulationConfig class
    py::class_<SimulationConfig>(m, "SimulationConfig")
        .def(py::init<>())
        .def_readwrite("world_width", &SimulationConfig::worldWidth)
        .def_readwrite("world_height", &SimulationConfig::worldHeight)
        .def_readwrite("initial_predators", &SimulationConfig::initialPredators)
        .def_readwrite("initial_prey", &SimulationConfig::initialPrey)
        .def_readwrite("mf", &SimulationConfig::MF)
        .def_readwrite("mr", &SimulationConfig::MR)
        .def_readwrite("interaction_radius", &SimulationConfig::interactionRadius)
        .def_readwrite("cell_size", &SimulationConfig::cellSize)
        .def_readwrite("simulation_steps", &SimulationConfig::simulationSteps)
        .def_readwrite("nr", &SimulationConfig::NR)
        .def_readwrite("rr", &SimulationConfig::RR)
        .def_readwrite("dr", &SimulationConfig::DR)
        .def_readwrite("df", &SimulationConfig::DF)
        .def_readwrite("rf", &SimulationConfig::RF);

    // Bind SimulationController class
    py::class_<SimulationController>(m, "SimulationController")
        .def(py::init<const SimulationConfig&>())
        .def("initialize", &SimulationController::initialize)
        .def("run_for_timesteps", &SimulationController::runForTimesteps)
        .def("end", &SimulationController::end)
        .def("get_report", &SimulationController::getReport);

    // Bind ParameterSweep class
    py::class_<ParameterSweep>(m, "ParameterSweep")
        .def(py::init<const std::string&>())
        .def("run", &ParameterSweep::run);

    // Add module-level functions
    m.def("run_parameter_sweep", &run_parameter_sweep_py,
          "Run parameter sweep and return results as numpy arrays",
          py::arg("num_samples"),
          py::arg("num_reruns"),
          py::arg("num_sims"),
          py::arg("num_timesteps"),
          py::arg("output_dir"));
} 