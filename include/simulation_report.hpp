#pragma once

#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

class SimulationReport {
public:
    SimulationReport(const std::vector<int>& prey_history, const std::vector<int>& predator_history);
    void visualize();
    int getPreyCount() const { return prey_history_.back(); }
    int getPredatorCount() const { return predator_history_.back(); }

    // Simulation data
    std::vector<int> predatorCounts;
    std::vector<int> preyCounts;
    std::chrono::milliseconds executionTime;
    int totalSteps;
    int finalPredatorCount;
    int finalPreyCount;

private:
    std::vector<int> prey_history_;
    std::vector<int> predator_history_;

    // Visualization constants
    static constexpr unsigned int WINDOW_WIDTH = 800;
    static constexpr unsigned int WINDOW_HEIGHT = 600;
    static constexpr float MARGIN = 50.f;

    // Helper methods for visualization
    void drawAxes(sf::RenderWindow& window) const;
    void drawGraph(sf::RenderWindow& window) const;
    void drawLegend(sf::RenderWindow& window) const;
    sf::Vector2f mapToScreen(float x, float y) const;
}; 