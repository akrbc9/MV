#pragma once

#include "agent.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include "position.hpp"

// Optimized hash function for position pairs
struct PairHash {
    size_t operator()(const std::pair<int, int>& p) const {
        // More efficient hash function than XOR
        return static_cast<size_t>(p.first) * 0x1f1f1f1f + p.second;
    }
};

class Grid {
private:
    // Store agents by shared_ptr directly instead of weak_ptr
    std::vector<std::shared_ptr<Agent>> agents;
    std::unordered_map<std::pair<int, int>, std::vector<std::shared_ptr<Agent>>, PairHash> cellToAgents;
    
    // Convert position to cell index
    inline std::pair<int, int> positionToIndex(const Position& position) const {
        return std::make_pair(
            static_cast<int>(std::floor(position.x / cellSize)),
            static_cast<int>(std::floor(position.y / cellSize))
        );
    }
    
    // Get neighboring cell indices
    std::vector<std::pair<int, int>> getCellNeighborsIndices(const std::pair<int, int>& index) const;
    
    double cellSize;

public:
    // Constructor
    Grid(double cellSize);

    // Agent management
    const std::vector<std::shared_ptr<Agent>>& getAgents() const;
    std::vector<std::shared_ptr<Agent>> getAgentsAtIndex(size_t index);
    void addAgent(std::shared_ptr<Agent> agent);
    void removeAgent(const std::shared_ptr<Agent>& agent);
    void clearAll();

    // Spatial queries
    bool hasOppositeTypeNeighbor(const std::shared_ptr<Agent>& agent, double interactionRadius);
    std::vector<std::shared_ptr<Agent>> getNearbyAgents(const Position& position, double radius);
    std::vector<std::shared_ptr<Agent>> getNearbyAgentsOfType(const Position& position, double radius, Agent::Type type);
    
    // Update agent position
    void moveAgent(const std::shared_ptr<Agent>& agent, const Position& oldPosition, const Position& newPosition);
};