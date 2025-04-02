#pragma once

#include "position.hpp"
#include "forward_decls.hpp"
#include "simulation_context.hpp"
#include <memory>
#include <atomic>
#include <iostream>

// Forward declaration
class Grid;

struct AgentAction {
    enum Action {REPRODUCE,DIE,NOTHING};
};

class Agent {
public:
    enum Type { PREDATOR, PREY };

protected:
    Type type;
    Position position;
    bool alive;
    SimulationContext& context;
    size_t id;  // Unique identifier for each agent

public:
    Agent(Type t, Position pos, SimulationContext& ctx, size_t agentId)
        : type(t), position(pos), alive(true), context(ctx), id(agentId) {}
    Agent(Type t, Position pos, SimulationContext& ctx)
        : type(t), position(pos), alive(true), context(ctx), id(getNextId()) {}
    virtual ~Agent() = default;
    
    // Update the update method to take a shared_ptr reference when necessary
    virtual AgentAction::Action getAction(bool hasInteraction) const { return AgentAction::Action::NOTHING; }
    
    virtual bool isAlive() const { return alive; }
    Position getPosition() const { return position; }
    void setPosition(const Position& pos) { position = pos; } // Add this method to update position
    Type getType() const { return type; }
    size_t getId() const { return id; }
    void die() { alive = false; }

    // Add an overload for shared_ptr
    double distanceSquaredFrom(const Position& other) const {
        return position.distanceSquaredTo(other);
    }

    double distanceSquaredFrom(const std::shared_ptr<Agent>& other) const {
        if (!other || !other->isAlive()) {
            return std::numeric_limits<double>::infinity();
        }
        return position.distanceSquaredTo(other->getPosition());
    }
    
    static int getNextId();
    static void resetIdCounter() { nextId_ = 0; }


private:
    static std::atomic<int> nextId_;
};

class Predator : public Agent {
public:
    Predator(Position pos, SimulationContext& ctx, size_t id)
        : Agent(PREDATOR, pos, ctx, id) {}
    Predator(Position pos, SimulationContext& ctx)
        : Agent(PREDATOR, pos, ctx, Agent::getNextId()) {}
    AgentAction::Action getAction(bool hasInteraction) const override;
};

class Prey : public Agent {
public:
    Prey(Position pos, SimulationContext& ctx, size_t id)
        : Agent(PREY, pos, ctx, id) {}
    Prey(Position pos, SimulationContext& ctx)
        : Agent(PREY, pos, ctx, Agent::getNextId()) {}
    AgentAction::Action getAction(bool hasInteraction) const override;
};
