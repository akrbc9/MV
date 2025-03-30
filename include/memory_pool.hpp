#pragma once
#include "agent.hpp"
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <algorithm>

template <typename T, size_t BlockSize = 4096>
class MemoryPool {
private:
    // Calculate alignment requirements
    static constexpr size_t Alignment = alignof(T);
    
    struct Block {
        char data[BlockSize];
        Block* next;
    };
    
    Block* currentBlock = nullptr;
    char* currentPos = nullptr;
    char* lastPos = nullptr;
    std::unordered_map<void*, bool> allocatedObjects;
    std::mutex mutex;
    
    // Align address to required boundary
    char* alignPointer(char* p) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(p);
        uintptr_t aligned = (addr + Alignment - 1) & ~(Alignment - 1);
        return reinterpret_cast<char*>(aligned);
    }
    
public:
    MemoryPool() {
        // Allocate first block
        currentBlock = new Block;
        currentBlock->next = nullptr;
        currentPos = alignPointer(currentBlock->data);
        lastPos = currentBlock->data + BlockSize;
    }
    
    ~MemoryPool() {
        // Free all allocated blocks
        Block* block = currentBlock;
        while (block) {
            Block* next = block->next;
            delete block;
            block = next;
        }
    }
    
    template <typename... Args>
    std::shared_ptr<T> allocate(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Ensure proper alignment
        currentPos = alignPointer(currentPos);
        
        // Check if we need a new block
        if (currentPos + sizeof(T) > lastPos) {
            Block* newBlock = new Block;
            newBlock->next = currentBlock;
            currentBlock = newBlock;
            currentPos = alignPointer(currentBlock->data);
            lastPos = currentBlock->data + BlockSize;
        }
        
        // Allocate from current block
        void* addr = currentPos;
        currentPos += sizeof(T);
        
        // Construct the object
        T* obj = new(addr) T(std::forward<Args>(args)...);
        allocatedObjects[addr] = true;
        
        // Create custom deleter that marks memory as available
        auto deleter = [this](T* ptr) {
            std::lock_guard<std::mutex> lock(mutex);
            // Mark as available in our tracking
            auto it = allocatedObjects.find(ptr);
            if (it != allocatedObjects.end()) {
                it->second = false;
            }
            // Call destructor but don't free memory
            ptr->~T();
        };
        
        return std::shared_ptr<T>(obj, deleter);
    }
    
    void collectGarbage() {
        std::lock_guard<std::mutex> lock(mutex);
        // Remove freed objects from tracking
        for (auto it = allocatedObjects.begin(); it != allocatedObjects.end();) {
            if (!it->second) {
                it = allocatedObjects.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    size_t activeCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return std::count_if(allocatedObjects.begin(), allocatedObjects.end(), 
                            [](const auto& pair) { return pair.second; });
    }
    
    size_t totalAllocations() const {
        std::lock_guard<std::mutex> lock(mutex);
        return allocatedObjects.size();
    }
};

// Specialized memory pools for predators and prey
class AgentPoolManager {
private:
    static MemoryPool<Predator> predatorPool;
    static MemoryPool<Prey> preyPool;
    
public:
    static std::shared_ptr<Predator> createPredator(Position pos, SimulationContext& ctx) {
        return predatorPool.allocate(pos, ctx);
    }
    
    static std::shared_ptr<Prey> createPrey(Position pos, SimulationContext& ctx) {
        return preyPool.allocate(pos, ctx);
    }
    
    static void garbageCollect() {
        predatorPool.collectGarbage();
        preyPool.collectGarbage();
    }
    
    static std::pair<size_t, size_t> getActiveCount() {
        return {predatorPool.activeCount(), preyPool.activeCount()};
    }
};