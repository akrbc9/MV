#include "grid.hpp"
#include <algorithm>

// Grid implementation
Grid::Grid(double cellSize, double worldWidth, double worldHeight)
    : spatialGrid(worldWidth, worldHeight, cellSize),
      cellSize(cellSize),
      worldWidth(worldWidth),
      worldHeight(worldHeight) {
    // Reserve space for expected agent counts
    predators.reserve(1000);
    preys.reserve(5000);
}

void Grid::addAgent(std::shared_ptr<Agent> agent) {
    if (agent->getType() == Agent::PREDATOR) {
        predators.add(std::static_pointer_cast<Predator>(agent));
    } else {
        preys.add(std::static_pointer_cast<Prey>(agent));
    }
    
    spatialGrid.insert(agent);
}

void Grid::removeAgent(const std::shared_ptr<Agent>& agent) {
    // Remove from type-specific container
    if (agent->getType() == Agent::PREDATOR) {
        predators.remove(agent->getId());
    } else {
        preys.remove(agent->getId());
    }
    
    // Remove from spatial grid
    spatialGrid.remove(agent);
}


const std::vector<std::shared_ptr<Predator>>& Grid::getPredators() const {
    return predators.getAll();
}

const std::vector<std::shared_ptr<Prey>>& Grid::getPreys() const {
    return preys.getAll();
}

std::vector<std::shared_ptr<Agent>> Grid::getAgents() const {
    std::vector<std::shared_ptr<Agent>> allAgents;
    allAgents.reserve(predators.size() + preys.size());
    
    // Copy predators
    const auto& predatorList = predators.getAll();
    allAgents.insert(allAgents.end(), predatorList.begin(), predatorList.end());
    
    // Copy prey
    const auto& preyList = preys.getAll();
    allAgents.insert(allAgents.end(), preyList.begin(), preyList.end());
    
    return allAgents;
}

bool Grid::hasOppositeTypeNeighbor(const std::shared_ptr<Agent>& agent, double radius) {
    return spatialGrid.hasOppositeTypeNeighbor(agent, radius);
}

std::vector<std::shared_ptr<Agent>> Grid::getNearbyAgents(const Position& pos, double radius) {
    return spatialGrid.getNeighbors(pos, radius);
}

std::vector<std::shared_ptr<Agent>> Grid::getNearbyAgentsOfType(
        const Position& pos, double radius, Agent::Type type) {
    return spatialGrid.getNeighborsOfType(pos, radius, type);
}

void Grid::moveAgent(const std::shared_ptr<Agent>& agent, 
                     const Position& oldPos, const Position& newPos) {
    spatialGrid.updatePosition(agent, oldPos);
}

void Grid::clearAll() {
    predators.clear();
    preys.clear();
    spatialGrid.clear();
}

size_t Grid::size() const {
    return predators.size() + preys.size();
}

size_t Grid::predatorCount() const {
    return predators.size();
}

size_t Grid::preyCount() const {
    return preys.size();
}