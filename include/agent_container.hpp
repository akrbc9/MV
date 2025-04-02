#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

// A cache-friendly container for storing agents with O(1) access, insertion, and removal
template <typename AgentType>
class AgentContainer {
private:
    // Main vector of agents
    std::vector<std::shared_ptr<AgentType>> agents;
    // Mapping of agent ID to index for O(1) lookup
    std::unordered_map<size_t, size_t> idToIndex;

public:
    // Default constructor
    AgentContainer() = default;
    
    // O(1) access by index
    std::shared_ptr<AgentType> get(size_t index) const {
        return index < agents.size() ? agents[index] : nullptr;
    }
    
    // O(1) access by ID
    std::shared_ptr<AgentType> getById(size_t id) const {
        auto it = idToIndex.find(id);
        return (it != idToIndex.end()) ? agents[it->second] : nullptr;
    }
    
    // O(1) insertion
    void add(std::shared_ptr<AgentType> agent) {
        idToIndex[agent->getId()] = agents.size();
        agents.emplace_back(std::move(agent));
    }
    
    // O(1) removal with swap-and-pop
    bool remove(size_t id) {
        auto it = idToIndex.find(id);
        if (it == idToIndex.end()) return false;
        
        size_t index = it->second;
        size_t lastIndex = agents.size() - 1;
        
        // If not the last element, swap with the last one
        if (index != lastIndex) {
            std::swap(agents[index], agents[lastIndex]);
            // Update the moved agent's index
            idToIndex[agents[index]->getId()] = index;
        }
        
        // Remove the last element
        agents.pop_back();
        idToIndex.erase(it);
        return true;
    }
    
    // Fast iteration
    const std::vector<std::shared_ptr<AgentType>>& getAll() const {
        return agents;
    }
    
    // Vector-like operations
    size_t size() const { return agents.size(); }
    bool empty() const { return agents.empty(); }
    void clear() { agents.clear(); idToIndex.clear(); }
    void reserve(size_t capacity) { agents.reserve(capacity); }

    // Iterator access for range-based for loops
    typename std::vector<std::shared_ptr<AgentType>>::iterator begin() { return agents.begin(); }
    typename std::vector<std::shared_ptr<AgentType>>::iterator end() { return agents.end(); }
    typename std::vector<std::shared_ptr<AgentType>>::const_iterator begin() const { return agents.begin(); }
    typename std::vector<std::shared_ptr<AgentType>>::const_iterator end() const { return agents.end(); }
};