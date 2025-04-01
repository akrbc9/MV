// simulation_bindings.cpp - This would be compiled into a Python module

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "simulation_controller.hpp"
#include "simulation_config.hpp"

namespace py = pybind11;

void init_simulation(py::module& m) {
    m.doc() = "Python bindings for Predator-Prey Simulation";
    
    // Bind the SimulationConfig class
    py::class_<SimulationConfig>(m, "SimulationConfig")
        .def(py::init<>())
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

PYBIND11_MODULE(predator_prey, m) {
    init_simulation(m);  // This will bind your C++ functions/classes to the 'm' module
}