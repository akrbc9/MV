#pragma once

#include <utility>
#include <cmath>

struct Position {
    double x;
    double y;

    Position(double x, double y) : x(x), y(y) {}
    
    // Utility methods for position manipulation
    Position operator+(const Position& other) const {
        return {x + other.x, y + other.y};
    }

    Position operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }
    
    // Convert to integer grid cell
    std::pair<int, int> toGridCell(double cellSize) const {
        return std::make_pair(
            static_cast<int>(std::floor(x / cellSize)),
            static_cast<int>(std::floor(y / cellSize))
        );
    }

    double distanceSquaredTo(const Position& other) const {
        return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
    }

    double distanceTo(const Position& other) const {
        return std::sqrt(distanceSquaredTo(other));
    }   
}; 