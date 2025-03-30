#pragma once

#include "position.hpp"
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <unordered_set>

class SpatialHash {
private:
    double cellSize;
    std::unordered_map<std::int64_t, std::unordered_set<size_t>> grid;  // Changed to unordered_set for O(1) removal
    std::unordered_map<size_t, std::int64_t> agentCells;  // Track which cell each agent is in

    std::int64_t hashPosition(const Position& pos) const;
    std::vector<std::int64_t> getNeighboringCells(const Position& pos, double radius) const;

public:
    explicit SpatialHash(double cellSize);

    void clear();
    void insert(const Position& pos, size_t index);
    void remove(size_t index);
    void updatePosition(const Position& newPos, size_t index);
    std::vector<size_t> query(const Position& pos, double radius) const;
}; 