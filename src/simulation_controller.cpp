#include "simulation_controller.hpp"
#include "agent.hpp"
#include "simulation_context.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <memory>
#include <algorithm>
#include <cmath>
#include <climits>

std::mt19937 SimulationController::rng(std::random_device{}());
std::uniform_real_distribution<double> SimulationController::reproDist(0, 1);
std::uniform_real_distribution<double> SimulationController::deathDist(0, 1);


// Helper function to generate random positionå
Position randomPosition() {
    std::mt19937& rng = SimulationController::rng;
    return Position{SimulationController::reproDist(rng), SimulationController::reproDist(rng)};
}

Position randomDirection() {
    std::mt19937& rng = SimulationController::rng;
    double x = 2*SimulationController::reproDist(rng)-1;
    double y = 2*SimulationController::reproDist(rng)-1;
    return Position{x, y};
}

int SimulationController::agentCount() const{
    return predatorCount + preyCount;
}

size_t SimulationController::randomAgentIndex() const {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<size_t> dist(0, agentCount() - 1);
    return dist(rng);
}

void SimulationController::initializePopulation() {
    grid.clearAll();
    SimulationConfig config = context.getConfig();
    
    // Add initial prey
    for (int i = 0; i < config.initialPrey; ++i) {
        Position pos = randomPosition();
        auto prey = std::make_shared<Prey>(pos, context);
        grid.addAgent(prey);
        preyCount++;
    }

    // Add initial predators
    for (int i = 0; i < config.initialPredators; ++i) {
        Position pos = randomPosition();
        auto predator = std::make_shared<Predator>(pos, context);
        grid.addAgent(predator);
        predatorCount++;
    }
}


SimulationController::SimulationController(const SimulationConfig& config)
    : context(config),
      grid(config.cellSize),
      predatorHistory(config.simulationSteps, 0),
      preyHistory(config.simulationSteps, 0) {
    initializePopulation();
}

void SimulationController::updateHistory() {
    predatorHistory.push_back(getCurrentPredatorCount());
    preyHistory.push_back(getCurrentPreyCount());
}

void SimulationController::initialize() {
    if (!isRunning) {
        isRunning = true;
        isPaused = false;
        currentStep = 0;
        startTime = std::chrono::steady_clock::now();
        predatorHistory.clear();
        preyHistory.clear();
        setPredatorCount(0);
        setPreyCount(0);
        initializePopulation();
        updateHistory();
    }
}

void SimulationController::updateSingleTimestep() {
    // Get all agents from grid
    double t = 0;

    while (t < 1.0 && agentCount() > 0) {
        t += 1.0 / agentCount();
        size_t randomIndex = randomAgentIndex();
        std::shared_ptr<Agent> agent = grid.getAgents()[randomIndex];
        if (agent) {
            updateSingleAgent(agent);
        }
    }

    currentStep++;
    // Update history
    updateHistory();
}

// void SimulationController::updateSingleTimestep() {
//     // Make a copy of the agents to avoid iterator invalidation during updates
//     const auto agents = grid.getAgents();
//     const size_t agentCount = agents.size();
    
//     // Pre-calculate simulation parameters for this step
//     const double interactionRadius = context.getConfig().interactionRadius;
//     const double NR = context.getConfig().NR;  // Carrying capacity of Prey
//     const double RR = context.getConfig().RR;  // Reproduction rate of Prey
//     const double DR = context.getConfig().DR;  // Death rate of prey when encountering predator
//     const double DF = context.getConfig().DF;  // Death rate of predator when no prey around
//     const double RF = context.getConfig().RF;  // Reproduction rate of predator
    
//     // Use a buffer for new agents to avoid modifying the collection during iteration
//     std::vector<std::shared_ptr<Agent>> newAgents;
//     newAgents.reserve(agentCount / 4); // Reserve some space based on expected growth
    
//     // Track agents to remove
//     std::vector<std::shared_ptr<Agent>> deadAgents;
//     deadAgents.reserve(agentCount / 4);
    
//     // Batch update all agent positions first
//     #pragma omp parallel for schedule(dynamic)
//     for (size_t i = 0; i < agentCount; ++i) {
//         auto agent = agents[i];
        
//         if (!agent->isAlive()) continue;
        
//         // Check for interactions
//         bool hasInteraction = grid.hasOppositeTypeNeighbor(agent, interactionRadius);
        
//         // Get action based on type and situation
//         AgentAction::Action action = agent->getAction(hasInteraction);
        
//         // Process the action
//         if (action == AgentAction::DIE) {
//             agent->die();
//             #pragma omp critical
//             {
//                 deadAgents.push_back(agent);
//                 if (agent->getType() == Agent::PREDATOR) {
//                     decrementPredatorCount();
//                 } else {
//                     decrementPreyCount();
//                 }
//             }
//         }
//         else if (action == AgentAction::REPRODUCE) {
//             // Create a new agent of the same type
//             Position newPos = agent->getPosition(); // For simplicity, same position
            
//             #pragma omp critical
//             {
//                 if (agent->getType() == Agent::PREDATOR) {
//                     newAgents.push_back(std::make_shared<Predator>(newPos, context));
//                     incrementPredatorCount();
//                 } else {
//                     newAgents.push_back(std::make_shared<Prey>(newPos, context));
//                     incrementPreyCount();
//                 }
//             }
//         }
//     }

//     // Remove dead agents
//     for (const auto& agent : deadAgents) {
//         grid.removeAgent(agent);
//     }

//     // Add new agents
//     for (const auto& agent : newAgents) {
//         grid.addAgent(agent);
//     }
    
//     currentStep++;
//     // Update history
//     updateHistory();
// }

void SimulationController::updateSingleAgent(std::shared_ptr<Agent> agent){
    if (!isRunning && isPaused) { return; }    

    // Update the randomly selected agent
    if (agent) {
        // Fix the ternary operator
        double m = (agent->getType() == Agent::Type::PREY) ? 
                   context.getConfig().MR : 
                   context.getConfig().MF;

        // Clamp the new position to the grid
        Position oldPosition = agent->getPosition();
        Position newPosition = agent->getPosition() + randomDirection() * m;
        newPosition.x = std::clamp(newPosition.x, 0.0, 1.0);
        newPosition.y = std::clamp(newPosition.y, 0.0, 1.0);

        // Make sure to update the agent's internal position too
        // This should be the only place where the agent's position is updated
        agent->setPosition(newPosition);
        grid.moveAgent(agent, oldPosition, newPosition);

        bool hasInteraction = grid.hasOppositeTypeNeighbor(agent, context.getConfig().interactionRadius);

        AgentAction::Action action = agent->getAction(hasInteraction);  

        if (action == AgentAction::Action::REPRODUCE) {
            std::shared_ptr<Agent> newAgent;
            if (agent->getType() == Agent::Type::PREY) {
                newAgent = std::make_shared<Prey>(newPosition, context);
                incrementPreyCount();
            } else {
                newAgent = std::make_shared<Predator>(newPosition, context);
                incrementPredatorCount();
            }
            grid.addAgent(newAgent);
        }

        else if (action == AgentAction::Action::DIE) {
            grid.removeAgent(agent);
            if (agent->getType() == Agent::Type::PREY) {
                decrementPreyCount();
            } else {
                decrementPredatorCount();
            }
        }
    }
}

void SimulationController::run() {
    // if (getCurrentPredatorCount() == 0) { 
    //     end();
    //     return; 
    // }
    if (isRunning && !isPaused) {
        updateSingleTimestep();
    }
}

void SimulationController::runForTimesteps(int numSteps) {
    for (int i = 0; i < numSteps; ++i) {
        run();
    }
}

void SimulationController::pause() {
    isPaused = true;
}

void SimulationController::end() {
    isRunning = false;
    isPaused = false;
    endTime = std::chrono::steady_clock::now();
}

SimulationStats SimulationController::getCurrentStats() const {
    SimulationStats stats;
    stats.predatorCount = predatorCount;
    stats.preyCount = preyCount;
    
    return stats;
}

void SimulationController::setPredatorCount(int count) {
    predatorCount = count;
    context.setPredatorCount(count);
}

void SimulationController::setPreyCount(int count) {
    preyCount = count;
    context.setPreyCount(count);
}   

void SimulationController::incrementPredatorCount() {
    predatorCount++;
    context.setPredatorCount(predatorCount);
}

void SimulationController::decrementPredatorCount() {
    predatorCount--;
    context.setPredatorCount(predatorCount);
}

void SimulationController::incrementPreyCount() {
    preyCount++;
    context.setPreyCount(preyCount);
}

void SimulationController::decrementPreyCount() {
    preyCount--;
    context.setPreyCount(preyCount);
}

int SimulationController::getCurrentPredatorCount() const {
    return predatorCount;
}

int SimulationController::getCurrentPreyCount() const {
    return preyCount;
}

void SimulationController::resetStats() {
    setPredatorCount(0);
    setPreyCount(0);
}

SimulationReport SimulationController::getReport() const {
    auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    SimulationReport report(predatorHistory, preyHistory);
    report.executionTime = executionTime;
    report.totalSteps = currentStep;
    report.finalPredatorCount = getCurrentPredatorCount();
    report.finalPreyCount = getCurrentPreyCount();
    
    return report;
} 