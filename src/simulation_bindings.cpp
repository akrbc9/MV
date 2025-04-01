// simulation_bindings.cpp - This would be compiled into a Python module

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "simulation_controller.hpp"
#include "simulation_config.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simulation_module, m) {
    m.doc() = "Python bindings for Predator-Prey Simulation";
    
    // Bind the SimulationConfig class
    py::class_<SimulationConfig>(m, "SimulationConfig")
        .def(py::init<>())
        .def(py::init<int, int, double, double, int, double, double, double, int, double, double, double>(),
             py::arg("initialPredators") = 10,
             py::arg("initialPrey") = 100,
             py::arg("cellSize") = 0.05,
             py::arg("interactionRadius") = 0.1,
             py::arg("simulationSteps") = 1000,
             py::arg("RR") = 0.1,  // Prey reproduction rate
             py::arg("DR") = 0.5,  // Prey death rate 
             py::arg("MR") = 0.05, // Prey movement rate
             py::arg("NR") = 500,  // Prey carrying capacity
             py::arg("RF") = 0.2,  // Predator reproduction rate
             py::arg("DF") = 0.1,  // Predator death rate
             py::arg("MF") = 0.07  // Predator movement rate
        )
        .def_readwrite("initialPredators", &SimulationConfig::initialPredators)
        .def_readwrite("initialPrey", &SimulationConfig::initialPrey)
        .def_readwrite("cellSize", &SimulationConfig::cellSize)
        .def_readwrite("interactionRadius", &SimulationConfig::interactionRadius)
        .def_readwrite("simulationSteps", &SimulationConfig::simulationSteps)
        .def_readwrite("RR", &SimulationConfig::RR)
        .def_readwrite("DR", &SimulationConfig::DR)
        .def_readwrite("MR", &SimulationConfig::MR)
        .def_readwrite("NR", &SimulationConfig::NR)
        .def_readwrite("RF", &SimulationConfig::RF)
        .def_readwrite("DF", &SimulationConfig::DF)
        .def_readwrite("MF", &SimulationConfig::MF);
    
    // Bind SimulationStats struct
    py::class_<SimulationStats>(m, "SimulationStats")
        .def(py::init<>())
        .def_readonly("predatorCount", &SimulationStats::predatorCount)
        .def_readonly("preyCount", &SimulationStats::preyCount);
    
    // Bind SimulationReport class (simplified version)
    py::class_<SimulationReport>(m, "SimulationReport")
        .def(py::init<const std::vector<int>&, const std::vector<int>&>())
        .def_readonly("predatorCounts", &SimulationReport::predatorCounts)
        .def_readonly("preyCounts", &SimulationReport::preyCounts)
        .def_readonly("executionTime", &SimulationReport::executionTime)
        .def_readonly("totalSteps", &SimulationReport::totalSteps)
        .def_readonly("finalPredatorCount", &SimulationReport::finalPredatorCount)
        .def_readonly("finalPreyCount", &SimulationReport::finalPreyCount)
        .def("getPreyCount", &SimulationReport::getPreyCount)
        .def("getPredatorCount", &SimulationReport::getPredatorCount);
        // Note: we're not binding visualize() since we'll implement our own in Python
    
    // Bind the SimulationController class
    py::class_<SimulationController>(m, "SimulationController")
        .def(py::init<const SimulationConfig&>())
        .def("initialize", &SimulationController::initialize)
        .def("run", &SimulationController::run)
        .def("runForTimesteps", &SimulationController::runForTimesteps)
        .def("pause", &SimulationController::pause)
        .def("end", &SimulationController::end)
        .def("isSimulationRunning", &SimulationController::isSimulationRunning)
        .def("isSimulationPaused", &SimulationController::isSimulationPaused)
        .def("getCurrentStep", &SimulationController::getCurrentStep)
        .def("getCurrentStats", &SimulationController::getCurrentStats)
        .def("getReport", &SimulationController::getReport)
        .def("getCurrentPredatorCount", &SimulationController::getCurrentPredatorCount)
        .def("getCurrentPreyCount", &SimulationController::getCurrentPreyCount);
}