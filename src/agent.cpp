#include "agent.hpp"
#include "grid.hpp"
#include "simulation_context.hpp"
#include <random>
#include <iostream>
#include <algorithm>
#include <memory>å
#include <cmath>

// Definition of static members
std::atomic<int> Agent::nextId_(0);

int Agent::getNextId() {
    return nextId_++;
}

AgentAction::Action Predator::getAction(bool hasInteraction) const {
    if (!isAlive()) return AgentAction::Action::NOTHING;

    const auto& config = context.getConfig();

    // Handle predation and reproduction
    if (hasInteraction) {
        // Try to reproduce
        std::uniform_real_distribution<double> reproDist(0.0, 1.0);
        if (reproDist(context.getRNG()) < config.RF) {
            // Create new predator at current position
            return AgentAction::Action::REPRODUCE;
        }
    } else {
        // Risk of death when no prey around
        std::uniform_real_distribution<double> deathDist(0.0, 1.0);
        if (deathDist(context.getRNG()) < config.DF) {
            return AgentAction::Action::DIE;
        }
    }
    return AgentAction::Action::NOTHING;
}

AgentAction::Action Prey::getAction(bool hasInteraction) const {
    if (!isAlive()) return AgentAction::Action::NOTHING;
    const auto& config = context.getConfig();

    // Handle predation and reproduction
    if (hasInteraction) {
        // Risk of being eaten
        std::uniform_real_distribution<double> deathDist(0.0, 1.0);
        if (deathDist(context.getRNG()) < config.DR) {
            return AgentAction::Action::DIE;
        }
    } 

    // Try to reproduce if below carrying capacity
    std::uniform_real_distribution<double> reproDist(0.0, 1.0);
    if (reproDist(context.getRNG()) < config.RR * (1 - static_cast<double>(context.getPreyCount()) / static_cast<double>(config.NR))) {
        
        // Create new prey at current position
        return AgentAction::Action::REPRODUCE;
    }
    
    return AgentAction::Action::NOTHING;
}
  