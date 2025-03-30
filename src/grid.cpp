#include "grid.hpp"
#include <cmath>
#include <algorithm>

Grid::Grid(double cellSize) : cellSize(cellSize) {}

std::vector<std::pair<int, int>> Grid::getCellNeighborsIndices(const std::pair<int, int>& index) const {
    std::vector<std::pair<int, int>> neighbors;
    // Reserve exact space to avoid reallocations
    neighbors.reserve(9);
    
    // Include self and 8 surrounding cells
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            neighbors.push_back({index.first + dx, index.second + dy});
        }
    }
    
    return neighbors;
}

const std::vector<std::shared_ptr<Agent>>& Grid::getAgents() const {
    return agents;
}

std::vector<std::shared_ptr<Agent>> Grid::getAgentsAtIndex(size_t index) {
    if (index < agents.size()) {
        std::vector<std::shared_ptr<Agent>> result = {agents[index]};
        return result;
    }
    return {};
}

void Grid::addAgent(std::shared_ptr<Agent> agent) {
    // Store in global list
    agents.push_back(agent);
    
    // Store in spatial hash
    auto cellIndex = positionToIndex(agent->getPosition());
    cellToAgents[cellIndex].push_back(agent);
}

void Grid::removeAgent(const std::shared_ptr<Agent>& agent) {
    // Remove from spatial hash
    auto cellIndex = positionToIndex(agent->getPosition());
    auto& cell = cellToAgents[cellIndex];
    
    cell.erase(
        std::remove_if(cell.begin(), cell.end(),
            [agent](const std::shared_ptr<Agent>& a) {
                return a.get() == agent.get();
            }),
        cell.end()
    );
    
    // Remove from global list
    agents.erase(
        std::remove_if(agents.begin(), agents.end(),
            [agent](const std::shared_ptr<Agent>& a) {
                return a.get() == agent.get();
            }),
        agents.end()
    );
}

void Grid::clearAll() {
    agents.clear();
    cellToAgents.clear();
}

bool Grid::hasOppositeTypeNeighbor(const std::shared_ptr<Agent>& agent, double interactionRadius) {
    if (!agent) return false;
    
    const Position& pos = agent->getPosition();
    auto agentType = agent->getType();
    double radiusSquared = interactionRadius * interactionRadius;
    
    // Calculate how many cells we need to check based on interaction radius
    int cellRadius = static_cast<int>(std::ceil(interactionRadius / cellSize));
    auto centerCell = positionToIndex(pos);
    
    // Check all cells that could contain agents within the interaction radius
    for (int dx = -cellRadius; dx <= cellRadius; ++dx) {
        for (int dy = -cellRadius; dy <= cellRadius; ++dy) {
            std::pair<int, int> checkCell = {centerCell.first + dx, centerCell.second + dy};
            
            auto it = cellToAgents.find(checkCell);
            if (it != cellToAgents.end()) {
                for (const auto& other : it->second) {
                    // Check if agent is of opposite type and within interaction radius
                    if (other->getType() != agentType && 
                        other->isAlive() &&
                        pos.distanceSquaredTo(other->getPosition()) <= radiusSquared) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

std::vector<std::shared_ptr<Agent>> Grid::getNearbyAgents(const Position& position, double radius) {
    std::vector<std::shared_ptr<Agent>> result;
    double radiusSquared = radius * radius;
    
    // Calculate how many cells we need to check
    int cellRadius = static_cast<int>(std::ceil(radius / cellSize));
    auto centerCell = positionToIndex(position);
    
    // Pre-allocate to avoid reallocations
    result.reserve(cellRadius * cellRadius * 4);
    
    // Check all cells that could contain agents within the radius
    for (int dx = -cellRadius; dx <= cellRadius; ++dx) {
        for (int dy = -cellRadius; dy <= cellRadius; ++dy) {
            std::pair<int, int> checkCell = {centerCell.first + dx, centerCell.second + dy};
            
            auto it = cellToAgents.find(checkCell);
            if (it != cellToAgents.end()) {
                for (const auto& agent : it->second) {
                    if (agent->isAlive() && 
                        position.distanceSquaredTo(agent->getPosition()) <= radiusSquared) {
                        result.push_back(agent);
                    }
                }
            }
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<Agent>> Grid::getNearbyAgentsOfType(
    const Position& position, double radius, Agent::Type type) {
    
    std::vector<std::shared_ptr<Agent>> result;
    double radiusSquared = radius * radius;
    
    // Calculate how many cells we need to check
    int cellRadius = static_cast<int>(std::ceil(radius / cellSize));
    auto centerCell = positionToIndex(position);
    
    // Pre-allocate to avoid reallocations
    result.reserve(cellRadius * cellRadius * 2);
    
    // Check all cells that could contain agents within the radius
    for (int dx = -cellRadius; dx <= cellRadius; ++dx) {
        for (int dy = -cellRadius; dy <= cellRadius; ++dy) {
            std::pair<int, int> checkCell = {centerCell.first + dx, centerCell.second + dy};
            
            auto it = cellToAgents.find(checkCell);
            if (it != cellToAgents.end()) {
                for (const auto& agent : it->second) {
                    if (agent->getType() == type && 
                        agent->isAlive() &&
                        position.distanceSquaredTo(agent->getPosition()) <= radiusSquared) {
                        result.push_back(agent);
                    }
                }
            }
        }
    }
    
    return result;
}

void Grid::moveAgent(const std::shared_ptr<Agent>& agent, const Position& oldPosition, const Position& newPosition) {
    auto oldCell = positionToIndex(oldPosition);
    auto newCell = positionToIndex(newPosition);
    
    // Only update spatial hash if the cell has changed
    if (oldCell != newCell) {
        // Remove from old cell
        auto& oldBucket = cellToAgents[oldCell];
        oldBucket.erase(
            std::remove_if(oldBucket.begin(), oldBucket.end(),
                [agent](const std::shared_ptr<Agent>& a) {
                    return a.get() == agent.get();
                }),
            oldBucket.end()
        );
        
        // Add to new cell
        cellToAgents[newCell].push_back(agent);
    }
}