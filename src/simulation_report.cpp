#include "simulation_report.hpp"
#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>

void SimulationReport::visualize() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "Simulation Report");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        if (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::White);
        drawAxes(window);
        drawGraph(window);
        drawLegend(window);
        window.display();
    }
}

void SimulationReport::drawAxes(sf::RenderWindow& window) const {
    // Create vertex arrays for x and y axes
    sf::VertexArray xAxis(sf::PrimitiveType::Lines, 2);
    sf::VertexArray yAxis(sf::PrimitiveType::Lines, 2);

    // X-axis
    xAxis[0].position = sf::Vector2f(MARGIN, WINDOW_HEIGHT - MARGIN);
    xAxis[1].position = sf::Vector2f(WINDOW_WIDTH - MARGIN, WINDOW_HEIGHT - MARGIN);
    xAxis[0].color = sf::Color::Black;
    xAxis[1].color = sf::Color::Black;

    // Y-axis
    yAxis[0].position = sf::Vector2f(MARGIN, MARGIN);
    yAxis[1].position = sf::Vector2f(MARGIN, WINDOW_HEIGHT - MARGIN);
    yAxis[0].color = sf::Color::Black;
    yAxis[1].color = sf::Color::Black;

    window.draw(xAxis);
    window.draw(yAxis);
}

void SimulationReport::drawGraph(sf::RenderWindow& window) const {
    if (prey_history_.empty() || predator_history_.empty()) return;

    // Find max values for scaling
    int maxPrey = *std::max_element(prey_history_.begin(), prey_history_.end());
    int maxPred = *std::max_element(predator_history_.begin(), predator_history_.end());
    int maxVal = std::max(maxPrey, maxPred);

    // Create vertex arrays for predator and prey lines
    sf::VertexArray predatorLine(sf::PrimitiveType::LineStrip, predator_history_.size());
    sf::VertexArray preyLine(sf::PrimitiveType::LineStrip, prey_history_.size());

    for (size_t i = 0; i < predator_history_.size(); ++i) {
        float x = static_cast<float>(i) / (predator_history_.size() - 1);
        float y = static_cast<float>(predator_history_[i]) / maxVal;
        predatorLine[i].position = mapToScreen(x, y);
        predatorLine[i].color = sf::Color::Red;

        x = static_cast<float>(i) / (prey_history_.size() - 1);
        y = static_cast<float>(prey_history_[i]) / maxVal;
        preyLine[i].position = mapToScreen(x, y);
        preyLine[i].color = sf::Color::Blue;
    }

    window.draw(predatorLine);
    window.draw(preyLine);
}

void SimulationReport::drawLegend(sf::RenderWindow& window) const {
    // Create vertex arrays for legend lines
    sf::VertexArray predatorLegend(sf::PrimitiveType::Lines, 2);
    sf::VertexArray preyLegend(sf::PrimitiveType::Lines, 2);

    float legendY = MARGIN + 20;
    
    // Predator legend line
    predatorLegend[0].position = sf::Vector2f(WINDOW_WIDTH - MARGIN - 50, legendY);
    predatorLegend[1].position = sf::Vector2f(WINDOW_WIDTH - MARGIN - 30, legendY);
    predatorLegend[0].color = sf::Color::Red;
    predatorLegend[1].color = sf::Color::Red;

    // Prey legend line
    preyLegend[0].position = sf::Vector2f(WINDOW_WIDTH - MARGIN - 50, legendY + 20);
    preyLegend[1].position = sf::Vector2f(WINDOW_WIDTH - MARGIN - 30, legendY + 20);
    preyLegend[0].color = sf::Color::Blue;
    preyLegend[1].color = sf::Color::Blue;

    window.draw(predatorLegend);
    window.draw(preyLegend);
}

sf::Vector2f SimulationReport::mapToScreen(float x, float y) const {
    float screenX = MARGIN + x * (WINDOW_WIDTH - 2 * MARGIN);
    float screenY = WINDOW_HEIGHT - MARGIN - y * (WINDOW_HEIGHT - 2 * MARGIN);
    return sf::Vector2f(screenX, screenY);
} 
