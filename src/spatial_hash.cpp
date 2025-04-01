#include "spatial_hash.hpp"
#include <cmath>
#include <algorithm>

SpatialHash::SpatialHash(double cellSize) : cellSize(cellSize) {}

std::int64_t SpatialHash::hashPosition(const Position& pos) const {
    int x = static_cast<int>(std::floor(pos.x / cellSize));
    int y = static_cast<int>(std::floor(pos.y / cellSize));
    return (static_cast<std::int64_t>(x) << 32) | static_cast<std::int64_t>(y);
}

std::vector<std::int64_t> SpatialHash::getNeighboringCells(const Position& pos, double radius) const {
    std::vector<std::int64_t> cells;
    int minX = static_cast<int>(std::floor((pos.x - radius) / cellSize));
    int maxX = static_cast<int>(std::floor((pos.x + radius) / cellSize));
    int minY = static_cast<int>(std::floor((pos.y - radius) / cellSize));
    int maxY = static_cast<int>(std::floor((pos.y + radius) / cellSize));

    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            cells.push_back((static_cast<std::int64_t>(x) << 32) | static_cast<std::int64_t>(y));
        }
    }
    return cells;
}

void SpatialHash::clear() {
    grid.clear();
    agentCells.clear();
}

void SpatialHash::insert(const Position& pos, size_t index) {
    std::int64_t cell = hashPosition(pos);
    grid[cell].insert(index);
    agentCells[index] = cell;
}

void SpatialHash::remove(size_t index) {
    auto it = agentCells.find(index);
    if (it != agentCells.end()) {
        std::int64_t cell = it->second;
        grid[cell].erase(index);
        if (grid[cell].empty()) {
            grid.erase(cell);
        }
        agentCells.erase(it);
    }
}

void SpatialHash::updatePosition(const Position& newPos, size_t index) {
    std::int64_t newCell = hashPosition(newPos);
    auto it = agentCells.find(index);
    
    if (it != agentCells.end()) {
        std::int64_t oldCell = it->second;
        if (oldCell != newCell) {
            // Remove from old cell
            grid[oldCell].erase(index);
            if (grid[oldCell].empty()) {
                grid.erase(oldCell);
            }
            
            // Add to new cell
            grid[newCell].insert(index);
            agentCells[index] = newCell;
        }
    }
}

std::vector<size_t> SpatialHash::query(const Position& pos, double radius) const {
    std::vector<size_t> result;
    auto cells = getNeighboringCells(pos, radius);
    
    for (auto cell : cells) {
        auto it = grid.find(cell);
        if (it != grid.end()) {
            result.insert(result.end(), it->second.begin(), it->second.end());
        }
    }
    return result;
} 