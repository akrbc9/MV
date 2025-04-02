#pragma once

#include "agent.hpp"
#include "agent_container.hpp"
#include "spatial_grid.hpp"
#include "position.hpp"
#include <memory>
#include <vector>

/**
 * Optimized grid class for efficient agent management and spatial queries.
 * This implementation uses type-specific containers and spatial partitioning
 * to achieve O(1) insertion, removal, and neighbor queries.
 */
class Grid {
private:
    // Typed containers for fast type-specific operations
    AgentContainer<Predator> predators;
    AgentContainer<Prey> preys;
    
    // Spatial indexing
    SpatialGrid spatialGrid;
    
    // Cache-aligned fields to avoid false sharing in multithreaded contexts
    alignas(64) double cellSize;
    alignas(64) double worldWidth;
    alignas(64) double worldHeight;

public:
    /**
     * Constructor initializes the grid with the given dimensions and cell size.
     * 
     * @param worldWidth The width of the simulation world
     * @param worldHeight The height of the simulation world
     * @param cellSize The size of each spatial grid cell
     */
    Grid(double cellSize, double worldWidth = 1, double worldHeight = 1);
    
    /**
     * Adds an agent to the grid immediately.
     * For batch operations, use queueAgentAddition instead.
     * 
     * @param agent The agent to add
     */
    void addAgent(std::shared_ptr<Agent> agent);
    
    /**
     * Removes an agent from the grid immediately.
     * For batch operations, use queueAgentRemoval instead.
     * 
     * @param agent The agent to remove
     */
    void removeAgent(const std::shared_ptr<Agent>& agent);
    
    /**
     * Queues an agent to be added during the next batch update.
     * 
     * @param agent The agent to add
     */
    void queueAgentAddition(std::shared_ptr<Agent> agent);
    
    /**
     * Queues an agent to be removed during the next batch update.
     * 
     * @param agent The agent to remove
     */
    void queueAgentRemoval(std::shared_ptr<Agent> agent);
    
    /**
     * Queues an agent position update for the next batch update.
     * 
     * @param agent The agent that moved
     * @param oldPos The agent's previous position
     */
    void queueAgentMove(std::shared_ptr<Agent> agent, const Position& oldPos);
    
    /**
     * Applies all pending agent changes at once.
     * This is more efficient than performing individual updates.
     */
    void applyPendingChanges();
    
    /**
     * Gets all predators in the simulation.
     * 
     * @return A const reference to the predator container
     */
    const std::vector<std::shared_ptr<Predator>>& getPredators() const;
    
    /**
     * Gets all prey in the simulation.
     * 
     * @return A const reference to the prey container
     */
    const std::vector<std::shared_ptr<Prey>>& getPreys() const;
    
    /**
     * Gets all agents in the simulation.
     * This is less efficient than type-specific access.
     * 
     * @return A vector containing all agents
     */
    std::vector<std::shared_ptr<Agent>> getAgents() const;
    
    /**
     * Checks if an agent has any neighbors of the opposite type within the given radius.
     * 
     * @param agent The agent to check
     * @param radius The interaction radius
     * @return True if an opposite-type neighbor exists, false otherwise
     */
    bool hasOppositeTypeNeighbor(const std::shared_ptr<Agent>& agent, double radius);
    
    /**
     * Gets all agents within the given radius of the position.
     * 
     * @param pos The position to search around
     * @param radius The search radius
     * @return A vector of agents within the radius
     */
    std::vector<std::shared_ptr<Agent>> getNearbyAgents(const Position& pos, double radius);
    
    /**
     * Gets all agents of the specified type within the given radius of the position.
     * 
     * @param pos The position to search around
     * @param radius The search radius
     * @param type The type of agents to find
     * @return A vector of agents of the specified type within the radius
     */
    std::vector<std::shared_ptr<Agent>> getNearbyAgentsOfType(
            const Position& pos, double radius, Agent::Type type);
    
    /**
     * Updates an agent's position in the spatial grid.
     * For batch operations, use queueAgentMove instead.
     * 
     * @param agent The agent that moved
     * @param oldPos The agent's previous position
     * @param newPos The agent's new position
     */
    void moveAgent(const std::shared_ptr<Agent>& agent, 
                  const Position& oldPos, const Position& newPos);
    
    /**
     * Clears all agents from the grid.
     */
    void clearAll();
    
    /**
     * Gets the total number of agents in the grid.
     * 
     * @return The number of agents
     */
    size_t size() const;
    
    /**
     * Gets the number of predators in the grid.
     * 
     * @return The number of predators
     */
    size_t predatorCount() const;
    
    /**
     * Gets the number of prey in the grid.
     * 
     * @return The number of prey
     */
    size_t preyCount() const;
};