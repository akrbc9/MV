#pragma once

#include "position.hpp"
#include "agent.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

// A cache-efficient spatial partitioning system for agent lookup
class SpatialGrid {
private:
    // Fixed-size grid cells using a flat array for cache efficiency
    std::vector<std::vector<std::shared_ptr<Agent>>> cells;
    int width, height;
    double cellSize;
    
    // Inlined cell calculation for speed
    inline int getCellIndex(const Position& pos) const {
        int x = static_cast<int>(std::floor(pos.x / cellSize));
        int y = static_cast<int>(std::floor(pos.y / cellSize));
        // Clamp to grid boundaries
        x = std::clamp(x, 0, width - 1);
        y = std::clamp(y, 0, height - 1);
        return y * width + x;
    }

public:
    SpatialGrid(double worldWidth, double worldHeight, double cellSize)
        : cellSize(cellSize) {
        width = static_cast<int>(std::ceil(worldWidth / cellSize));
        height = static_cast<int>(std::ceil(worldHeight / cellSize));
        cells.resize(width * height);
        
        // Reserve reasonable space for each cell to avoid reallocations
        for (auto& cell : cells) {
            cell.reserve(10); // Adjust based on expected density
        }
    }
    
    // Fast agent insertion
    void insert(const std::shared_ptr<Agent>& agent) {
        int cellIdx = getCellIndex(agent->getPosition());
        cells[cellIdx].push_back(agent);
    }
    
    // Fast agent removal with swap-and-pop
    void remove(const std::shared_ptr<Agent>& agent) {
        int cellIdx = getCellIndex(agent->getPosition());
        auto& cell = cells[cellIdx];
        
        for (size_t i = 0; i < cell.size(); ++i) {
            if (cell[i]->getId() == agent->getId()) {
                // Swap with last element and pop (O(1) removal)
                if (i != cell.size() - 1) {
                    std::swap(cell[i], cell.back());
                }
                cell.pop_back();
                break;
            }
        }
    }
    
    // Update agent position in the grid
    void updatePosition(const std::shared_ptr<Agent>& agent, 
                        const Position& oldPos) {
        int oldCellIdx = getCellIndex(oldPos);
        int newCellIdx = getCellIndex(agent->getPosition());
        
        if (oldCellIdx != newCellIdx) {
            // Remove from old cell
            auto& oldCell = cells[oldCellIdx];
            for (size_t i = 0; i < oldCell.size(); ++i) {
                if (oldCell[i]->getId() == agent->getId()) {
                    if (i != oldCell.size() - 1) {
                        std::swap(oldCell[i], oldCell.back());
                    }
                    oldCell.pop_back();
                    break;
                }
            }
            
            // Add to new cell
            cells[newCellIdx].push_back(agent);
        }
    }
    
    // Get all neighboring agents efficiently
    std::vector<std::shared_ptr<Agent>> getNeighbors(const Position& pos, double radius) {
        std::vector<std::shared_ptr<Agent>> neighbors;
        double radiusSquared = radius * radius;
        
        // Calculate cell bounds for the search
        int cellRadius = static_cast<int>(std::ceil(radius / cellSize));
        int centerX = static_cast<int>(std::floor(pos.x / cellSize));
        int centerY = static_cast<int>(std::floor(pos.y / cellSize));
        
        // Pre-allocate to avoid reallocations
        int potentialNeighbors = 0;
        for (int y = std::max(0, centerY - cellRadius); 
             y <= std::min(height - 1, centerY + cellRadius); ++y) {
            for (int x = std::max(0, centerX - cellRadius); 
                 x <= std::min(width - 1, centerX + cellRadius); ++x) {
                potentialNeighbors += cells[y * width + x].size();
            }
        }
        neighbors.reserve(potentialNeighbors);
        
        // Gather neighbors from surrounding cells
        for (int y = std::max(0, centerY - cellRadius); 
             y <= std::min(height - 1, centerY + cellRadius); ++y) {
            for (int x = std::max(0, centerX - cellRadius); 
                 x <= std::min(width - 1, centerX + cellRadius); ++x) {
                
                const auto& cellAgents = cells[y * width + x];
                for (const auto& agent : cellAgents) {
                    if (agent->isAlive() && 
                        pos.distanceSquaredTo(agent->getPosition()) <= radiusSquared) {
                        neighbors.push_back(agent);
                    }
                }
            }
        }
        
        return neighbors;
    }
    
    // Get neighbors of specific type (avoiding type checks in hot loops)
    std::vector<std::shared_ptr<Agent>> getNeighborsOfType(
            const Position& pos, double radius, Agent::Type type) {
        
        std::vector<std::shared_ptr<Agent>> neighbors;
        double radiusSquared = radius * radius;
        
        // Similar to getNeighbors, but with type filtering
        int cellRadius = static_cast<int>(std::ceil(radius / cellSize));
        int centerX = static_cast<int>(std::floor(pos.x / cellSize));
        int centerY = static_cast<int>(std::floor(pos.y / cellSize));
        
        // Conservative capacity estimation
        neighbors.reserve((2 * cellRadius + 1) * (2 * cellRadius + 1) * 5);
        
        for (int y = std::max(0, centerY - cellRadius); 
             y <= std::min(height - 1, centerY + cellRadius); ++y) {
            for (int x = std::max(0, centerX - cellRadius); 
                 x <= std::min(width - 1, centerX + cellRadius); ++x) {
                
                const auto& cellAgents = cells[y * width + x];
                for (const auto& agent : cellAgents) {
                    if (agent->isAlive() && 
                        agent->getType() == type &&
                        pos.distanceSquaredTo(agent->getPosition()) <= radiusSquared) {
                        neighbors.push_back(agent);
                    }
                }
            }
        }
        
        return neighbors;
    }
    
    // Check if any opposite-type neighbor exists (early exit optimization)
    bool hasOppositeTypeNeighbor(const std::shared_ptr<Agent>& agent, double radius) {
        if (!agent || !agent->isAlive()) return false;
        
        const Position& pos = agent->getPosition();
        Agent::Type oppositeType = (agent->getType() == Agent::PREDATOR) ? 
                                   Agent::PREY : Agent::PREDATOR;
        double radiusSquared = radius * radius;
        
        // Get surrounding cells
        int cellRadius = static_cast<int>(std::ceil(radius / cellSize));
        int centerX = static_cast<int>(std::floor(pos.x / cellSize));
        int centerY = static_cast<int>(std::floor(pos.y / cellSize));
        
        // Check cells in spiral pattern (more likely to find matches near center)
        for (int r = 0; r <= cellRadius; ++r) {
            // Check cells at distance r from center in a square pattern
            for (int dx = -r; dx <= r; ++dx) {
                for (int dy = -r; dy <= r; ++dy) {
                    // Only check cells on the perimeter of the square
                    if (r > 0 && std::abs(dx) < r && std::abs(dy) < r) continue;
                    
                    int x = centerX + dx;
                    int y = centerY + dy;
                    
                    if (x >= 0 && x < width && y >= 0 && y < height) {
                        const auto& cellAgents = cells[y * width + x];
                        for (const auto& other : cellAgents) {
                            if (other->isAlive() && 
                                other->getType() == oppositeType &&
                                pos.distanceSquaredTo(other->getPosition()) <= radiusSquared) {
                                return true; // Early exit
                            }
                        }
                    }
                }
            }
        }
        
        return false;
    }
    
    void clear() {
        for (auto& cell : cells) {
            cell.clear();
        }
    }
};